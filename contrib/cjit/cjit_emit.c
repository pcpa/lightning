/*
 * Copyright (C) 2010  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#include "cjit.h"

#define add_tjump(node)		add_jump(bstack.tjump + bstack.offset - 1, node)
#define add_fjump(node)		add_jump(bstack.fjump + bstack.offset - 1, node)
#define top_value_stack()	(vstack.values + vstack.offset - 1)
#define vstack_reset(ofs)						\
    do {								\
	assert(vstack.offset >= (ofs));					\
	if (vstack.offset > (ofs))					\
	    dec_value_stack(vstack.offset - (ofs));			\
    } while (0)

/*
 * Types
 */
typedef struct jump {
    ejit_node_t	**table;
    int		  offset;
    int		  length;
    token_t	  token;
} jump_t;

typedef struct branch {
    jump_t	*tjump;
    jump_t	*fjump;
    int		 offset;
    int		 length;
} branch_t;

/*
 * Prototypes
 */
static tag_t *
emit_stat(expr_t *expr);

static tag_t *
emit_expr(expr_t *expr);

static tag_t *
emit_decl(expr_t *expr);

static tag_t *
emit_set(expr_t *expr);

static tag_t *
emit_setop(expr_t *expr);

static tag_t *
emit_incdec(expr_t *expr);

static tag_t *
emit_not(expr_t *expr);

static tag_t *
emit_neg(expr_t *expr);

static tag_t *
emit_com(expr_t *expr);

static tag_t *
emit_address(expr_t *expr);

static tag_t *
emit_pointer(expr_t *expr, token_t token, expr_t *rexp);

static tag_t *
emit_field(expr_t *expr, token_t token, expr_t *vexp);

static tag_t *
emit_symbol(expr_t *expr, token_t token, expr_t *rexp);

static tag_t *
emit_vector(expr_t *expr, token_t token, expr_t *vexp);

static tag_t *
emit_cond(expr_t *expr);

static tag_t *
emit_cmp(expr_t *expr);

static tag_t *
emit_binary(expr_t *expr, token_t token);

static tag_t *
emit_binary_next(expr_t *expr, tag_t *ltag, tag_t *rtag,
		 value_t *lval, value_t *rval, token_t token);

static tag_t *
emit_binary_setup(expr_t *expr, tag_t *ltag, tag_t *rtag,
		  value_t *lval, value_t *rval, token_t token);

static void
emit_coerce_const(expr_t *expr, tag_t *tag, value_t *value);

static void
emit_coerce(expr_t *expr, tag_t *tag, value_t *value);

static void
emit_test_branch(expr_t *expr, int jmpif, int level);

static tag_t *
emit_question(expr_t *expr);

static tag_t *
emit_if(expr_t *expr);

static tag_t *
emit_while(expr_t *expr);

static tag_t *
emit_do(expr_t *expr);

static tag_t *
emit_for(expr_t *expr);

static tag_t *
emit_break(expr_t *expr);

static tag_t *
emit_continue(expr_t *expr);

static tag_t *
emit_switch(expr_t *expr);

static tag_t *
emit_case(expr_t *expr);

static tag_t *
emit_default(expr_t *expr);

static tag_t *
emit_function(expr_t *expr);

static tag_t *
emit_return(expr_t *expr);

static tag_t *
emit_call(expr_t *expr);

static void
emit_load(value_t *value);

static void
emit_save(value_t *value);

static void
emit_load_symbol(symbol_t *symbol, value_t *value);

static void
emit_store_symbol(symbol_t *symbol, value_t *value);

static value_t *
get_value_stack(void);

static void
inc_value_stack(void);

static void
dec_value_stack(int count);

static int
get_register(int freg);

static void
inc_branch_stack(token_t token);

static void
dec_branch_stack(int count);

static void
add_jump(jump_t *list, ejit_node_t *node);

/*
 * Initialization
 */
static ejit_state_t	*state;
static vstack_t		 vstack;
static branch_t		 bstack;

/*
 * Implementation
 */
void
init_emit(void)
{
    state = ejit_create_state();
    vstack.length = 16;
    vstack.values = (value_t *)xmalloc(vstack.length * sizeof(value_t));

    /* cause brach stack information to be initialized */
    inc_branch_stack(tok_none);
    dec_branch_stack(1);
}

void
emit(expr_t *expr)
{
    (void)emit_stat(expr);
    ejit_optimize(state);
    printf("\njit:");
    ejit_print(state);
}

static tag_t *
emit_stat(expr_t *expr)
{
    tag_t	*tag = void_tag;
    int		 offset = vstack.offset;

    for (; expr; expr = expr->next) {
	tag = emit_expr(expr);
	vstack_reset(offset);
    }

    return (tag);
}

static tag_t *
emit_expr(expr_t *expr)
{
    value_t	*value;
    symbol_t	*symbol;
    function_t	*function;

    switch (expr->token) {
	case tok_int:
	    value = get_value_stack();
#if __WORDSIZE == 32
	    value->type = value_itype;
	    value->u.ival = expr->data._unary.i;
#else
	    if (expr->data._unary.i < -2147483648 ||
		expr->data._unary.i >  2147483647) {
		value->type = value_ltype;
		value->u.lval = expr->data._unary.i;
	    }
	    else {
		value->type = value_itype;
		value->u.ival = expr->data._unary.i;
	    }
#endif
	    inc_value_stack();
	    return (value->type ? long_tag : int_tag);
	case tok_float:
	    value = get_value_stack();
	    value->type = value_dtype;
	    value->u.dval = expr->data._unary.d;
	    inc_value_stack();
	    return (double_tag);
	case tok_symbol:
	    value = get_value_stack();
	    inc_value_stack();
	    symbol = get_symbol(expr->data._unary.cp);
	    if (symbol) {
		value->type = value_symbl;
		value->u.pval = symbol;
		return (symbol->tag);
	    }
	    function = (function_t *)get_hash(functions, expr->data._unary.cp);
	    if (function) {
		value->type = value_funct;
		value->u.pval = symbol;
		return (function->tag);
	    }
	    error(expr, "undefined symbol '%s'", expr->data._unary.cp);
	case tok_dot:		case tok_arrow:
	    return (emit_field(expr, tok_none, NULL));
	case tok_vector:
	    return (emit_vector(expr, tok_none, NULL));
	case tok_not:
	    return (emit_not(expr));
	case tok_neg:
	    return (emit_neg(expr));
	case tok_com:
	    return (emit_com(expr));
	case tok_address:
	    return (emit_address(expr));
	case tok_pointer:
	    return (emit_pointer(expr, tok_none, NULL));
	case tok_andand:	case tok_oror:
	    return (emit_cond(expr));
	case tok_lt:		case tok_le:
	case tok_eq:		case tok_ge:
	case tok_gt:		case tok_ne:
	    return (emit_cmp(expr));
	case tok_and:		case tok_or:
	case tok_xor:		case tok_lsh:
	case tok_rsh:		case tok_rem:
	case tok_add:		case tok_sub:
	case tok_mul:		case tok_div:
	    return (emit_binary(expr, expr->token));
	case tok_set:
	    return (emit_set(expr));
	case tok_andset:	case tok_orset:
	case tok_xorset:	case tok_lshset:
	case tok_rshset:	case tok_remset:
	case tok_addset:	case tok_subset:
	case tok_mulset:	case tok_divset:
	    return (emit_setop(expr));
	case tok_inc:		case tok_dec:
	case tok_postinc:	case tok_postdec:
	    return (emit_incdec(expr));
	case tok_decl:
	    return (emit_decl(expr));
	case tok_code:		case tok_stat:
	    return (emit_stat(expr->data._unary.expr));
	    break;
	case tok_question:
	    return (emit_question(expr));
	case tok_if:
	    return (emit_if(expr));
	case tok_while:
	    return (emit_while(expr));
	case tok_do:
	    return (emit_do(expr));
	case tok_for:
	    return (emit_for(expr));
	case tok_break:
	    return (emit_break(expr));
	case tok_continue:
	    return (emit_continue(expr));
	case tok_switch:
	    return (emit_switch(expr));
	case tok_case:
	    return (emit_case(expr));
	case tok_default:
	    return (emit_default(expr));
	case tok_function:
	    return (emit_function(expr));
	case tok_return:
	    return (emit_return(expr));
	case tok_call:
	    return (emit_call(expr));
	default:
	    warn(expr, "not yet handled");
	    return (void_tag);
    }
}

static tag_t *
emit_decl(expr_t *expr)
{
    int		 offset = vstack.offset;

    for (expr = expr->data._binary.rvalue; expr; expr = expr->next) {
	if (expr->token == tok_set) {
	    emit_set(expr);
	    vstack_reset(offset);
	}
    }
    return (void_tag);
}

static tag_t *
emit_set(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*lexp;
    expr_t	*rexp;

    lexp = expr->data._binary.lvalue;
    rexp = expr->data._binary.rvalue;
    switch (lexp->token) {
	case tok_symbol:
	    tag = emit_symbol(lexp, tok_none, rexp);
	    break;
	case tok_dot:		case tok_arrow:
	    tag = emit_field(lexp, tok_none, rexp);
	    break;
	case tok_vector:
	    tag = emit_vector(lexp, tok_none, rexp);
	    break;
	case tok_pointer:
	    tag = emit_pointer(lexp, tok_none, rexp);
	    break;
	default:
	    error(lexp, "not a lvalue");
    }
    if (type_mask(tag->type) == type_vector)
	error(expr, "not a lvalue");

    return (tag);
}

static tag_t *
emit_setop(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*lexp;
    expr_t	*rexp;
    token_t	 token;

    lexp = expr->data._binary.lvalue;
    rexp = expr->data._binary.rvalue;
    token = (token_t)(expr->token + (tok_and - tok_andset));
    switch (lexp->token) {
	case tok_symbol:
	    tag = emit_symbol(lexp, token, rexp);
	    break;
	case tok_dot:		case tok_arrow:
	    tag = emit_field(lexp, token, rexp);
	    break;
	case tok_vector:
	    tag = emit_vector(lexp, token, rexp);
	    break;
	case tok_pointer:
	    tag = emit_pointer(lexp, token, rexp);
	    break;
	default:
	    error(lexp, "not a lvalue");
    }
    if (type_mask(tag->type) == type_vector)
	error(expr, "not a lvalue");

    return (tag);
}

static tag_t *
emit_incdec(expr_t *expr)
{
    tag_t	*tag;
    int		 inc;
    int		 post;
    int		 lreg;
    int		 rreg;
    int		 vreg;
    expr_t	*lexp;
    value_t	*lval;
    value_t	*rval;
    symbol_t	*symbol;

    switch (expr->token) {
	case tok_inc:		inc = 1; post = 0;	break;
	case tok_dec:		inc = 0; post = 0;	break;
	case tok_postinc:	inc = 1; post = 1;	break;
	default:		inc = 0; post = 1;	break;
    }
    lexp = expr->data._unary.expr;
    switch (lexp->token) {
	case tok_symbol:
	    tag = emit_expr(lexp);
	    break;
	case tok_dot:		case tok_arrow:
	    tag = emit_field(lexp, tok_address, NULL)->tag;
	    break;
	case tok_vector:
	    tag = emit_vector(lexp, tok_address, NULL)->tag;
	    break;
	case tok_pointer:
	    tag = emit_pointer(lexp, tok_address, NULL)->tag;
	    break;
	default:
	    error(lexp, "not a lvalue");
    }
    lval = top_value_stack();
    if (lexp->token == tok_symbol) {
	symbol = lval->u.pval;
	emit_load(lval);
	lreg = lval->u.ival;
    }
    else {
	lreg = lval->u.ival;
	/* save pointer in top of value stack */
	rval = get_value_stack();
	rval->type = lval->type;
	rval->u.ival = vreg = get_register(0);
	inc_value_stack();
	switch (tag->type) {
	    case type_char:	ejit_ldr_c (state, vreg, lreg);	break;
	    case type_uchar:	ejit_ldr_uc(state, vreg, lreg);	break;
	    case type_short:	ejit_ldr_s (state, vreg, lreg);	break;
	    case type_ushort:	ejit_ldr_us(state, vreg, lreg);	break;
	    case type_int:	ejit_ldr_i (state, vreg, lreg);	break;
	    case type_uint:	ejit_ldr_ui(state, vreg, lreg);	break;
	    case type_long:	ejit_ldr_l (state, vreg, lreg);	break;
	    case type_ulong:	ejit_ldr_ul(state, vreg, lreg);	break;
		/* only check error below... */
	    default:		ejit_ldr_p (state, vreg, lreg);	break;
	}
	/* swap register values */
	rval->u.ival = lreg;
	lreg = vreg;
    }
    if (post)
	rreg = get_register(0);

    switch (tag->type) {
	case type_char:		case type_short:	case type_int:
	    if (post)		ejit_movr_i(state, rreg, lreg);
	    if (inc)		ejit_addi_i(state, lreg, lreg, 1);
	    else		ejit_subi_i(state, lreg, lreg, 1);
	    break;
	case type_uchar:	case type_ushort:	case type_uint:
	    if (post)		ejit_movr_ui(state, rreg, lreg);
	    if (inc)		ejit_addi_ui(state, lreg, lreg, 1);
	    else		ejit_addi_ui(state, lreg, lreg, 1);
	    break;
	case type_long:
	    if (post)		ejit_movr_l(state, rreg, lreg);
	    if (inc) 		ejit_addi_l(state, lreg, lreg, 1);
	    else 		ejit_subi_l(state, lreg, lreg, 1);
	    break;
	case type_ulong:
	    if (post)		ejit_movr_ul(state, rreg, lreg);
	    if (inc)		ejit_addi_ul(state, lreg, lreg, 1);
	    else		ejit_subi_ul(state, lreg, lreg, 1);
	    break;
	default:
	    if (pointer_type_p(tag->type) && tag->tag->size) {
		if (post)	ejit_movr_p(state, rreg, lreg);
		if (inc)	ejit_addi_p(state, lreg, lreg,
					    (void *)(long)tag->tag->size);
		else		ejit_subi_p(state, lreg, lreg,
					    (void *)(long)tag->tag->size);
		break;
	    }
	case type_float:	case type_double:
	    error(expr, "not an integer or pointer");
    }
    if (lexp->token == tok_symbol)
	emit_store_symbol(symbol, lval);
    else {
	vreg = rval->u.ival;
	switch (tag->type) {
	    case type_char:	ejit_str_c (state, vreg, lreg);	break;
	    case type_uchar:	ejit_str_uc(state, vreg, lreg);	break;
	    case type_short:	ejit_str_s (state, vreg, lreg);	break;
	    case type_ushort:	ejit_str_us(state, vreg, lreg);	break;
	    case type_int:	ejit_str_i (state, vreg, lreg);	break;
	    case type_uint:	ejit_str_ui(state, vreg, lreg);	break;
	    case type_long:	ejit_str_l (state, vreg, lreg);	break;
	    case type_ulong:	ejit_str_ul(state, vreg, lreg);	break;
	    default:		ejit_str_p (state, vreg, lreg);	break;
	}
	dec_value_stack(1);
    }
    if (post)
	lval->u.ival = rreg;

    return (tag);
}

static tag_t *
emit_field(expr_t *expr, token_t token, expr_t *vexp)
{
    int		 lr;
    int		 vr;
    expr_t	*fexp;
    tag_t	*tag;
    tag_t	*ltag;
    tag_t	*vtag;
    value_t	*lval;
    value_t	*vval;
    value_t	*xval;
    char	*field;
    record_t	*record;
    symbol_t	*symbol;

    fexp = expr->data._binary.rvalue;
    if (expr->data._binary.rvalue->token != tok_symbol)
	error(expr, "syntax error");
    field = expr->data._binary.rvalue->data._unary.cp;
    /* record tag type */
    ltag = emit_expr(expr->data._binary.lvalue);
    if (!(ltag->type & (type_struct | type_union)))
	error(expr->data._binary.lvalue, "not a struct or union");
    lval = top_value_stack();
    emit_load(lval);
    lr = lval->u.ival;
    if (pointer_type_p(ltag->type)) {
	if (expr->token != tok_arrow)
	    error(expr, "value is a pointer");
	ltag = ltag->tag;
	if (pointer_type_p(ltag->type))
	    error(expr, "not a struct or union");
	ejit_ldr_p(state, lr, lr);
    }
    else if (expr->token != tok_dot)
	error(expr, "not a pointer");
    /* type punned toplevel tag */
    record = (record_t *)ltag->name;
    symbol = (symbol_t *)get_hash((hash_t *)record, field);
    if (symbol == NULL)
	error(expr, "no '%s' field in '%s'", field,
	      record->name ? record->name->name.string : "<anonymous>");
    /* expression result tag type */
    tag = symbol->tag;
    if (vexp) {
	if (token) {
	    vval = get_value_stack();
	    switch (tag->type) {
		case type_char:		case type_short:	case type_int:
		    vval->type = value_itype;
		    break;
		case type_uchar:	case type_ushort:	case type_uint:
		    vval->type = value_utype;
		    break;
		case type_long:
		    vval->type = value_ltype;
		    break;
		case type_ulong:
		    vval->type = value_ultype;
		    break;
		case type_float:
		    vval->type = value_ftype;
		    break;
		case type_double:
		    vval->type = value_dtype;
		    break;
		default:
		    vval->type = value_ptype;
		    break;
	    }
	    vval->type |= value_regno;
	    vval->u.ival = vr = get_register(value_float_p(vval));
	    inc_value_stack();
	    switch (tag->type) {
		case type_char:
		    ejit_ldxi_c(state, vr, lr, symbol->offset);
		    break;
		case type_uchar:
		    ejit_ldxi_uc(state, vr, lr, symbol->offset);
		    break;
		case type_short:
		    ejit_ldxi_s(state, vr, lr, symbol->offset);
		    break;
		case type_ushort:
		    ejit_ldxi_us(state, vr, lr, symbol->offset);
		    break;
		case type_int:
		    ejit_ldxi_i(state, vr, lr, symbol->offset);
		    break;
		case type_uint:
		    ejit_ldxi_ui(state, vr, lr, symbol->offset);
		    break;
		case type_long:
		    ejit_ldxi_l(state, vr, lr, symbol->offset);
		    break;
		case type_ulong:
		    ejit_ldxi_ul(state, vr, lr, symbol->offset);
		    break;
		case type_float:
		    ejit_ldxi_f(state, vr, lr, symbol->offset);
		    break;
		case type_double:
		    ejit_ldxi_d(state, vr, lr, symbol->offset);
		    break;
		default:
		    ejit_addi_p(state, vr, lr, (void *)(long)symbol->offset);
		    break;
	    }
	    vtag = emit_expr(vexp);
	    xval = top_value_stack();
	    vtag = emit_binary_next(expr, tag, vtag, vval, xval, token);
	    dec_value_stack(1);
	}
	else
	    vtag = emit_expr(vexp);

	vval = top_value_stack();
	/* stored value must be in a register even if a literal constant */
	if (tag != vtag)
	    emit_coerce_const(expr, tag, vval);
	emit_load(vval);
	if (tag != vtag)
	    emit_coerce(expr, tag, vval);
	vr = vval->u.ival;
	/* reload if spilled, otherwise a noop */
	emit_load(lval);
    }

    else if (token) {
	if (symbol->offset)
	    ejit_addi_p(state, lr, lr, (void *)(long)symbol->offset);
	lval->type = value_ptype | value_regno;
	return (tag_pointer(tag));
    }

    switch (tag->type) {
	case type_char:
	    lval->type = value_itype;
	    if (vexp)	ejit_stxi_c(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_c(state, lr, lr, symbol->offset);
	    break;
	case type_uchar:
	    lval->type = value_utype;
	    if (vexp)	ejit_stxi_uc(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_uc(state, lr, lr, symbol->offset);
	    break;
	case type_short:
	    lval->type = value_itype;
	    if (vexp)	ejit_stxi_s(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_s(state, lr, lr, symbol->offset);
	    break;
	case type_ushort:
	    lval->type = value_utype;
	    if (vexp)	ejit_stxi_us(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_us(state, lr, lr, symbol->offset);
	    break;
	case type_int:
	    lval->type = value_itype;
	    if (vexp)	ejit_stxi_i(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_i(state, lr, lr, symbol->offset);
	    break;
	case type_uint:
	    lval->type = value_utype;
	    if (vexp)	ejit_stxi_ui(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_ui(state, lr, lr, symbol->offset);
	    break;
	case type_long:
	    lval->type = value_ltype;
	    if (vexp)	ejit_stxi_l(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_l(state, lr, lr, symbol->offset);
	    break;
	case type_ulong:
	    lval->type = value_ultype;
	    if (vexp)	ejit_stxi_ul(state, symbol->offset, lr, vr);
	    else	ejit_ldxi_ul(state, lr, lr, symbol->offset);
	    break;
	case type_float:
	    if (vexp)
		ejit_stxi_f(state, symbol->offset, lr, vr);
	    else {
		vr = get_register(value_ftype);
		ejit_ldxi_f(state, vr, lr, symbol->offset);
		lval->u.ival = vr;
	    }
	    lval->type = value_ftype;
	    break;
	case type_double:
	    if (vexp)
		ejit_stxi_d(state, symbol->offset, lr, vr);
	    else {
		vr = get_register(value_dtype);
		ejit_ldxi_d(state, vr, lr, symbol->offset);
		lval->u.ival = vr;
	    }
	    lval->type = value_dtype;
	    break;
	default:
	    lval->type = value_ptype;
	    if (vexp) {
		if (!pointer_type_p(tag->type)) {
		    warn(expr, "aggregate store not handled");
		    return (void_tag);
		}
		ejit_stxi_p(state, symbol->offset, lr, vr);
	    }
	    else
		ejit_addi_p(state, lr, lr, (void *)(long)symbol->offset);
	    break;
    }
    lval->type |= value_regno;
    if (vexp) {
	lval->u.ival = vr;
	dec_value_stack(1);
    }

    return (tag);
}

/* FIXME should scale, adjust, bound check, and/or handle 64 bit offsets?! */
/* FIXME optimization: if going to spill offset or base pointer, better to
 * adjust pointer to release one register... */
static tag_t *
emit_vector(expr_t *expr, token_t token, expr_t *vexp)
{
    int		 lr;
    int		 rr;
    int		 vr;
    tag_t	*tag;
    tag_t	*ltag;
    tag_t	*rtag;
    tag_t	*vtag;
    value_t	*lval;
    value_t	*rval;
    value_t	*vval;
    value_t	*xval;

    ltag = emit_expr(expr->data._binary.lvalue);
    if (!pointer_type_p(ltag->type))
	error(expr->data._binary.lvalue, "not a vector");
    lval = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    switch (rtag->type) {
	case type_char:		case type_short:	case type_int:
	case type_uchar:	case type_ushort:	case type_uint:
	case type_long:		case type_ulong:
	    break;
	default:
	    error(expr->data._binary.rvalue, "not an integer");
    }
    rval = top_value_stack();
    emit_load(lval);
    lr = lval->u.ival;
    if (value_load_p(rval))
	/* must be done before evaluating vexp (if set) because it may
	 * have side effects that could change the offset, e.g. change
	 * the value of a symbol */
	emit_load(rval);
    rr = rval->u.ival;
    tag = ltag->tag;

    if (vexp) {
	if (token) {
	    vval = get_value_stack();
	    switch (tag->type) {
		case type_char:		case type_short:	case type_int:
		    vval->type = value_itype;
		    break;
		case type_uchar:	case type_ushort:	case type_uint:
		    vval->type = value_utype;
		    break;
		case type_long:
		    vval->type = value_ltype;
		    break;
		case type_ulong:
		    vval->type = value_ultype;
		    break;
		case type_float:
		    vval->type = value_ftype;
		    break;
		case type_double:
		    vval->type = value_dtype;
		    break;
		default:
		    vval->type = value_ptype;
		    break;
	    }
	    vval->type |= value_regno;
	    vval->u.ival = vr = get_register(value_float_p(vval));
	    inc_value_stack();
	    switch (tag->type) {
		case type_char:
		    if (value_const_p(rval))
			ejit_ldxi_c(state, vr, lr, rr);
		    else
			ejit_ldxr_c(state, vr, lr, rr);
		    break;
		case type_uchar:
		    if (value_const_p(rval))
			ejit_ldxi_uc(state, vr, lr, rr);
		    else
			ejit_ldxr_uc(state, vr, lr, rr);
		    break;
		case type_short:
		    if (value_const_p(rval))
			ejit_ldxi_s(state, vr, lr, rr * sizeof(short));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(short));
			ejit_ldxr_s(state, vr, lr, rr);
		    }
		    break;
		case type_ushort:
		    if (value_const_p(rval))
			ejit_ldxi_us(state, vr, lr, rr * sizeof(short));
		    else {
			ejit_muli_i(state, vr, rr, sizeof(short));
			ejit_ldxr_us(state, vr, lr, rr);
		    }
		    break;
		case type_int:
		    if (value_const_p(rval))
			ejit_ldxi_i(state, vr, lr, rr * sizeof(int));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(int));
			ejit_ldxr_i(state, vr, lr, rr);
		    }
		    break;
		case type_uint:
		    if (value_const_p(rval))
			ejit_ldxi_ui(state, vr, lr, rr * sizeof(int));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(int));
			ejit_ldxr_ui(state, vr, lr, rr);
		    }
		    break;
		case type_long:
		    if (value_const_p(rval))
			ejit_ldxi_l(state, vr, lr, rr * sizeof(long));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(long));
			ejit_ldxr_l(state, vr, lr, rr);
		    }
		    break;
		case type_ulong:
		    if (value_const_p(rval))
			ejit_ldxi_ul(state, vr, lr, rr * sizeof(long));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(long));
			ejit_ldxr_ul(state, vr, lr, rr);
		    }
		    break;
		case type_float:
		    if (value_const_p(rval))
			ejit_ldxi_f(state, vr, lr, rr * sizeof(float));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(float));
			ejit_ldxr_f(state, vr, lr, rr);
		    }
		    lval->u.ival = vr;
		    break;
		case type_double:
		    if (value_const_p(rval))
			ejit_ldxi_d(state, vr, lr, rr * sizeof(double));
		    else {
			ejit_muli_i(state, rr, rr, sizeof(double));
			ejit_ldxr_d(state, vr, lr, rr);
		    }
		    break;
		default:
		    if (value_const_p(rval))
			ejit_addi_p(state, vr, lr,
				    (void *)(long)(rr * tag->size));
		    else {
			ejit_muli_i(state, rr, rr, rr * tag->size);
			ejit_addr_p(state, vr, lr, rr);
		    }
		    break;
	    }
	    vtag = emit_expr(vexp);
	    xval = top_value_stack();
	    vtag = emit_binary_next(expr, tag, vtag, vval, xval, token);
	    dec_value_stack(1);
	}
	else
	    vtag = emit_expr(vexp);

	vval = top_value_stack();
	/* stored value must be in a register even if a literal constant */
	if (tag != vtag)
	    emit_coerce_const(expr, tag, vval);
	if (token == tok_none)
	    emit_load(vval);
	if (tag != vtag)
	    emit_coerce(expr, tag, vval);
	vr = vval->u.ival;
	/* reload if spilled, otherwise a noop */
	emit_load(lval);
	if (token && !value_const_p(rval))
	    emit_load(rval);
    }

    else if (token) {
	if (value_const_p(rval))
	    ejit_addi_p(state, lr, lr, (void *)(long)(rr * tag->size));
	else {
	    ejit_muli_i(state, rr, rr, tag->size);
	    ejit_addr_p(state, lr, lr, rr);
	}
	lval->type = value_ptype | value_regno;
	dec_value_stack(1);

	return (tag_pointer(tag));
    }

    switch (tag->type) {
	case type_char:
	    lval->type = value_itype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_c(state, rr, lr, vr);
		else
		    ejit_stxr_c(state, rr, lr, vr);
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_c(state, lr, lr, rr);
		else
		    ejit_ldxr_c(state, lr, lr, rr);
	    }
	    break;
	case type_uchar:
	    lval->type = value_utype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_uc(state, rr, lr, vr);
		else
		    ejit_stxr_uc(state, rr, lr, vr);
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_uc(state, lr, lr, rr);
		else
		    ejit_ldxr_uc(state, lr, lr, rr);
	    }
	    break;
	case type_short:
	    lval->type = value_itype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_s(state, rr * sizeof(short), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(short));
		    ejit_stxr_s(state, rr, lr, vr);
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_s(state, lr, lr, rr * sizeof(short));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(short));
		    ejit_ldxr_s(state, lr, lr, rr);
		}
	    }
	    break;
	case type_ushort:
	    lval->type = value_utype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_us(state, rr * sizeof(short), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(short));
		    ejit_stxr_us(state, rr, lr, vr);
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_us(state, lr, lr, rr * sizeof(short));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(short));
		    ejit_ldxr_us(state, lr, lr, rr);
		}
	    }
	    break;
	case type_int:
	    lval->type = value_itype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_i(state, rr * sizeof(int), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(int));
		    ejit_stxr_i(state, rr, lr, vr);
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_i(state, lr, lr, rr * sizeof(int));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(int));
		    ejit_ldxr_i(state, lr, lr, rr);
		}
	    }
	    break;
	case type_uint:
	    lval->type = value_utype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_ui(state, rr * sizeof(int), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(int));
		    ejit_stxr_ui(state, rr, lr, vr);
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_ui(state, lr, lr, rr * sizeof(int));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(int));
		    ejit_ldxr_ui(state, lr, lr, rr);
		}
	    }
	    break;
	case type_long:
	    lval->type = value_ltype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_l(state, rr * sizeof(long), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(long));
		    ejit_stxr_l(state, rr, lr, vr);
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_l(state, lr, lr, rr * sizeof(long));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(long));
		    ejit_ldxr_l(state, lr, lr, rr);
		}
	    }
	    break;
	case type_ulong:
	    lval->type = value_ultype;
	    if (vexp) {
		if (value_const_p(rval))
		    ejit_stxi_ul(state, rr * sizeof(long), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(long));
		    ejit_stxr_ul(state, rr, lr, vr);
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_ldxi_ul(state, lr, lr, rr * sizeof(long));
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(long));
		    ejit_ldxr_ul(state, lr, lr, rr);
		}
	    }
	    break;
	case type_float:
	    if (vexp) {
		/* already coerced */
		if (value_const_p(rval))
		    ejit_stxi_f(state, rr * sizeof(float), lr, vr);
		else {
		    ejit_muli_i(state, rr, rr, sizeof(float));
		    ejit_stxr_f(state, rr, lr, vr);
		}
	    }
	    else {
		vr = get_register(value_ftype);
		if (value_const_p(rval))
		    ejit_ldxi_f(state, vr, lr, rr * sizeof(float));
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(float));
		    ejit_ldxr_f(state, vr, lr, rr);
		}
		lval->u.ival = vr;
	    }
	    lval->type = value_ftype;
	    break;
	case type_double:
	    if (vexp) {
		/* already coerced */
		if (value_const_p(rval))
		    ejit_stxi_d(state, rr * sizeof(double), lr, vr);
		else {
		    if (token == tok_none)
			ejit_muli_i(state, rr, rr, sizeof(double));
		    ejit_stxr_d(state, rr, lr, vr);
		}
	    }
	    else {
		vr = get_register(value_dtype);
		if (value_const_p(rval))
		    ejit_ldxi_d(state, vr, lr, rr * sizeof(double));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(double));
		    ejit_ldxr_d(state, vr, lr, rr);
		}
		lval->u.ival = vr;
	    }
	    lval->type = value_dtype;
	    break;
	default:
	    lval->type = value_ptype;
	    if (vexp) {
		if (value_const_p(rval)) {
		    if (!pointer_type_p(tag->type))
			warn(expr, "aggregate store not handled");
		    else
			/* tag->size == sizeof(void*) here ... */
			ejit_stxi_p(state, rr * tag->size, lr, vr);
		}
		else {
		    if (!pointer_type_p(tag->type))
			warn(expr, "aggregate store not handled");
		    else {
			if (token == tok_none)
			    ejit_muli_i(state, rr, rr, sizeof(void*));
			ejit_stxr_p(state, rr, lr, vr);
		    }
		}
	    }
	    else {
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(long)(rr * tag->size));
		else {
		    ejit_muli_i(state, rr, rr, rr * tag->size);
		    ejit_addr_p(state, lr, lr, rr);
		}
	    }
	    break;
    }
    lval->type |= value_regno;
    if (vexp) {
	lval->u.ival = vr;
	dec_value_stack(2);
    }
    else
	dec_value_stack(1);

    return (tag);
}

static tag_t *
emit_symbol(expr_t *expr, token_t token, expr_t *rexp)
{
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    symbol_t	*symbol;

    /* avoid duplicated code to check symbol and allocate a value_t */
    ltag = emit_expr(expr);
    lval = top_value_stack();
    if (lval->type == value_symbl)
	symbol = lval->u.pval;
    else {
	if (rexp)
	    error(expr, "not a lvalue");
	warn(expr, "function address not handled");
	return (void_tag);
    }
    if (rexp) {
	if (token)
	    emit_load_symbol(symbol, lval);
	rtag = emit_expr(rexp);
	rval = top_value_stack();
	if (token) {
	    rtag = emit_binary_next(expr, ltag, rtag, lval, rval, token);
	    if (ltag != rtag)
		emit_coerce_const(expr, symbol->tag, lval);
	    if (ltag != rtag)
		emit_coerce(rexp, symbol->tag, lval);
	    emit_store_symbol(symbol, lval);
	}
	else {
	    if (ltag != rtag)
		emit_coerce_const(expr, symbol->tag, rval);
	    emit_load(rval);
	    if (ltag != rtag)
		emit_coerce(rexp, symbol->tag, rval);
	    emit_store_symbol(symbol, rval);
	    lval->type = rval->type;
	    lval->u.ival = rval->u.ival;
	}
	dec_value_stack(1);
    }
    else
	emit_load_symbol(symbol, lval);

    return (ltag);
}

static tag_t *
emit_not(expr_t *expr)
{
    tag_t	*tag;
    int		 ireg;
    value_t	*fval;
    value_t	*value;
    int		 regno;

    tag = emit_expr(expr->data._unary.expr);
    value = top_value_stack();
    emit_load(value);
    regno = value->u.ival;

    switch (tag->type) {
	case type_char:		case type_short:	case type_int:
	    ejit_nei_i(state, regno, regno, 0);
	    break;
	case type_uchar:	case type_ushort:	case type_uint:
	    ejit_nei_ui(state, regno, regno, 0);
	    break;
	case type_long:
	    ejit_nei_l(state, regno, regno, 0);
	    break;
	case type_ulong:
	    ejit_nei_ul(state, regno, regno, 0);
	    break;
	case type_float:
	    fval = get_value_stack();
	    fval->type = value_ftype;
	    fval->u.fval = 0.0;
	    inc_value_stack();
	    emit_load(fval);
	    ireg = get_register(0);
	    ejit_ltgtr_f(state, ireg, regno, fval->u.ival);
	    value->u.ival = ireg;
	    dec_value_stack(1);
	    break;
	case type_double:
	    fval = get_value_stack();
	    fval->type = value_dtype;
	    fval->u.dval = 0.0;
	    inc_value_stack();
	    emit_load(fval);
	    ireg = get_register(0);
	    ejit_ltgtr_d(state, ireg, regno, fval->u.ival);
	    value->u.ival = ireg;
	    dec_value_stack(1);
	    break;
	default:
	    if (!pointer_type_p(tag->type))
		warn(expr, "value is always true");
	    ejit_nei_p(state, regno, regno, NULL);
	    break;
    }
    value->type = value_regno;

    return (int_tag);
}

static tag_t *
emit_neg(expr_t *expr)
{
    tag_t	*tag;
    value_t	*value;
    int		 regno;

    tag = emit_expr(expr->data._unary.expr);
    value = top_value_stack();
    emit_load(value);
    regno = value->u.ival;

    switch (tag->type) {
	case type_uchar:	case type_ushort:	case type_uint:
	    warn(expr, "value is unsigned");
	case type_char:		case type_short:	case type_int:
	    ejit_negr_i(state, regno, regno);
	    tag = int_tag;
	    break;
	case type_ulong:
	    warn(expr, "value is unsigned");
	case type_long:
	    ejit_negr_l(state, regno, regno);
	    tag = long_tag;
	    break;
	case type_float:
	    ejit_negr_f(state, regno, regno);
	    break;
	case type_double:
	    ejit_negr_d(state, regno, regno);
	    break;
	default:
	    error(expr, "value is a pointer");
    }

    return (tag);
}

static tag_t *
emit_com(expr_t *expr)
{
    tag_t	*tag;
    value_t	*value;
    int		 regno;

    tag = emit_expr(expr->data._unary.expr);
    value = top_value_stack();
    emit_load(value);
    regno = value->u.ival;

    switch (tag->type) {
	case type_char:		case type_short:	case type_int:
	case type_uchar:	case type_ushort:	case type_uint:
	    ejit_notr_i(state, regno, regno);
	    tag = int_tag;
	    break;
	case type_long:		case type_ulong:
	    ejit_notr_l(state, regno, regno);
	    tag = long_tag;
	    break;
	default:
	    error(expr, "not an integer");
    }

    return (tag);
}

static tag_t *
emit_address(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*lexp;
    value_t	*lval;
    symbol_t	*symbol;

    lexp = expr->data._unary.expr;
    switch (lexp->token) {
	case tok_symbol:
	    tag = emit_expr(lexp);
	    lval = top_value_stack();
	    if (lval->type == value_funct) {
		warn(expr, "address of function not handed");
		return (void_tag);
	    }
	    symbol = lval->u.pval;
	    lval->u.ival = get_register(0);
	    lval->type = value_ptype | value_regno;
	    if (symbol->arg)
		/* FIXME because it can be in a register... */
		error(expr, "address of argument not supported");
	    else if (symbol->loc)
		ejit_addi_p(state, lval->u.ival, EJIT_FP,
			    (void *)(long)symbol->offset);
	    else
		ejit_movi_p(state, lval->u.ival,
			    (char *)the_data + symbol->offset);
	    tag = tag_pointer(tag);
	    break;
	case tok_dot:		case tok_arrow:
	    tag = emit_field(lexp, tok_address, NULL);
	    break;
	case tok_vector:
	    tag = emit_vector(lexp, tok_address, NULL);
	    break;
	case tok_pointer:
	    tag = emit_pointer(lexp, tok_address, NULL);
	    break;
	default:
	    error(lexp, "not a lvalue");
    }

    return (tag);
}

static tag_t *
emit_pointer(expr_t *expr, token_t token, expr_t *rexp)
{
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    value_t	*xval;
    int		 lreg;
    int		 rreg;

    ltag = emit_expr(expr->data._unary.expr);
    if (!pointer_type_p(ltag->type))
	error(expr, "not a pointer");
    lval = top_value_stack();
    emit_load(lval);
    lreg = lval->u.ival;
    ltag = ltag->tag;

    if (rexp) {
	if (token) {
	    rval = get_value_stack();
	    switch (ltag->type) {
		case type_char:		case type_short:	case type_int:
		    rval->type = value_itype;
		    break;
		case type_uchar:	case type_ushort:	case type_uint:
		    rval->type = value_utype;
		    break;
		case type_long:
		    rval->type = value_ltype;
		    break;
		case type_ulong:
		    rval->type = value_ultype;
		    break;
		case type_float:
		    rval->type = value_ftype;
		    break;
		case type_double:
		    rval->type = value_dtype;
		    break;
		default:
		    rval->type = value_ptype;
		    break;
	    }
	    rval->type |= value_regno;
	    rval->u.ival = rreg = get_register(value_float_p(rval));
	    inc_value_stack();
	    switch (ltag->type) {
		case type_char:
		    ejit_ldr_c(state, rreg, lreg);
		    break;
		case type_uchar:
		    ejit_ldr_c(state, rreg, lreg);
		    break;
		case type_short:
		    ejit_ldr_s(state, rreg, lreg);
		    break;
		case type_ushort:
		    ejit_ldr_us(state, rreg, lreg);
		    break;
		case type_int:
		    ejit_ldr_i(state, rreg, lreg);
		    break;
		case type_uint:
		    ejit_ldr_ui(state, rreg, lreg);
		    break;
		case type_long:
		    ejit_ldr_l(state, rreg, lreg);
		    break;
		case type_ulong:
		    ejit_ldr_ul(state, rreg, lreg);
		    break;
		case type_float:
		    ejit_ldr_f(state, rreg, lreg);
		    break;
		case type_double:
		    ejit_ldr_d(state, rreg, lreg);
		    break;
		default:
		    ejit_ldr_p(state, rreg, lreg);
		    break;
	    }
	    rtag = emit_expr(rexp);
	    xval = top_value_stack();
	    rtag = emit_binary_next(expr, ltag, rtag, rval, xval, token);
	    dec_value_stack(1);
	}
	else
	    rtag = emit_expr(rexp);

	rval = top_value_stack();
	if (token == tok_none)
	    emit_load(rval);
	if (ltag != rtag)
	    emit_coerce(expr, ltag, rval);
	rreg = rval->u.ival;
	/* reload if spilled, otherwise a noop */
	emit_load(lval);
    }

    else if (token)
	/* duh */
	return (tag_pointer(ltag));

    switch (ltag->type) {
	case type_char:
	    lval->type = value_itype;
	    if (rexp)	ejit_str_c(state, lreg, rreg);
	    else	ejit_ldr_c(state, lreg, lreg);
	    break;
	case type_uchar:
	    lval->type = value_utype;
	    if (rexp)	ejit_str_uc(state, lreg, rreg);
	    else	ejit_ldr_uc(state, lreg, lreg);
	    break;
	case type_short:
	    lval->type = value_itype;
	    if (rexp)	ejit_str_s(state, lreg, rreg);
	    else	ejit_ldr_s(state, lreg, lreg);
	    break;
	case type_ushort:
	    lval->type = value_utype;
	    if (rexp)	ejit_str_us(state, lreg, rreg);
	    else	ejit_ldr_us(state, lreg, lreg);
	    break;
	case type_int:
	    lval->type = value_itype;
	    if (rexp)	ejit_str_i(state, lreg, rreg);
	    else	ejit_ldr_i(state, lreg, lreg);
	    break;
	case type_uint:
	    lval->type = value_utype;
	    if (rexp)	ejit_str_ui(state, lreg, rreg);
	    else	ejit_ldr_ui(state, lreg, lreg);
	    break;
	case type_long:
	    lval->type = value_ltype;
	    if (rexp)	ejit_str_l(state, lreg, rreg);
	    else	ejit_ldr_l(state, lreg, lreg);
	    break;
	case type_ulong:
	    lval->type = value_utype;
	    if (rexp)	ejit_str_ul(state, lreg, rreg);
	    else	ejit_ldr_ul(state, lreg, lreg);
	    break;
	case type_float:
	    if (rexp)
		ejit_str_f(state, lreg, rreg);
	    else {
		rreg = get_register(value_ftype);
		ejit_ldr_f(state, rreg, lreg);
	    }
	    lval->type = value_ftype;
	    lval->u.ival = rreg;
	    break;
	case type_double:
	    if (rexp)
		ejit_str_d(state, lreg, rreg);
	    else {
		rreg = get_register(value_dtype);
		ejit_ldr_d(state, rreg, lreg);
	    }
	    lval->type = value_dtype;
	    lval->u.ival = rreg;
	    break;
	default:
	    lval->type = value_ptype;
	    if (rexp) {
		if (!pointer_type_p(ltag->type))
		    warn(expr, "aggregate store not handled");
		else
		    ejit_str_p(state, lreg, rreg);
	    }
	    else
		ejit_ldr_p(state, lreg, lreg);
	    break;
    }
    lval->type |= value_regno;
    if (rexp)
	dec_value_stack(1);

    return (ltag);
}

static tag_t *
emit_cond(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*cond;
    ejit_node_t	*node;
    int		 lreg;
    int		 rreg;
    value_t	*lval;
    value_t	*rval;
    int		 flreg;
    int		 frreg;
    ejit_node_t	*label;

    cond = expr->data._binary.lvalue;
    tag = emit_expr(cond);
    lval = top_value_stack();
    emit_load(lval);
    lreg = lval->u.ival;
    switch (tag->type) {
	case type_char:		case type_short:	case type_int:
	case type_uchar:	case type_ushort:	case type_uint:
	    ejit_nei_i(state, lreg, lreg, 0);
	    break;
	case type_long:		case type_ulong:
	    ejit_nei_l(state, lreg, lreg, 0);
	    break;
	case type_float:
	    flreg = lreg;
	    lreg = get_register(0);
	    /* need to allocate a value_t on stack if integer and float
	     * register classes may overlap
	     * (comment apply to other switch cases in this function) */
	    frreg = get_register(value_ftype);
	    ejit_movi_f(state, frreg, 0.0);
	    ejit_ner_f(state, lreg, flreg, frreg);
	    lval->u.ival = lreg;
	    break;
	case type_double:
	    flreg = lreg;
	    lreg = get_register(0);
	    frreg = get_register(value_dtype);
	    ejit_movi_d(state, frreg, 0.0);
	    ejit_ner_d(state, lreg, flreg, frreg);
	    lval->u.ival = lreg;
	    break;
	default:
	    ejit_nei_p(state, lreg, lreg, NULL);
	    break;
    }
    lval->type = value_itype | value_regno;
    if (expr->token == tok_andand)
	node = ejit_beqi_i(state, NULL, lreg, 0);
    else
	node = ejit_bnei_i(state, NULL, lreg, 0);

    cond = expr->data._binary.rvalue;
    tag = emit_expr(cond);
    rval = top_value_stack();
    emit_load(rval);
    rreg = rval->u.ival;
    switch (tag->type) {
	case type_char:		case type_short:	case type_int:
	case type_uchar:	case type_ushort:	case type_uint:
	    ejit_nei_i(state, lreg, rreg, 0);
	    break;
	case type_long:		case type_ulong:
	    ejit_nei_l(state, lreg, rreg, 0);
	    break;
	case type_float:
	    flreg = rreg;
	    frreg = get_register(value_ftype);
	    ejit_movi_f(state, frreg, 0.0);
	    ejit_ner_f(state, lreg, flreg, frreg);
	    break;
	case type_double:
	    flreg = rreg;
	    frreg = get_register(value_dtype);
	    ejit_movi_d(state, frreg, 0.0);
	    ejit_ner_d(state, lreg, flreg, frreg);
	    break;
	default:
	    ejit_nei_p(state, lreg, rreg, NULL);
	    break;
    }
    label = ejit_label(state);
    ejit_patch(state, label, node);

    /* in case it was spilled, remember it is not required to reload
     * and possibly, could remove a spill that can be generated
     * when parsing the right side operation */
    lval->type = value_itype | value_regno;
    dec_value_stack(1);

    return (int_tag);
}

static tag_t *
emit_cmp(expr_t *expr)
{
    long	 il;
    void	*ip;
    tag_t	*tag;
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    int		 lreg;
    int		 rreg;
    int		 lfreg;
    int		 rfreg;

    ltag = emit_expr(expr->data._binary.lvalue);
    lval = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rval = top_value_stack();
    emit_load(lval);
    tag = emit_binary_setup(expr, ltag, rtag, lval, rval, expr->token);
    lreg = lval->u.ival;
    switch (tag->type) {
	case type_int:
	    if (value_const_p(rval)) {
		il = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_lti_i(state, lreg, lreg, il);	break;
		    case tok_le: ejit_lei_i(state, lreg, lreg, il);	break;
		    case tok_eq: ejit_eqi_i(state, lreg, lreg, il);	break;
		    case tok_ge: ejit_gei_i(state, lreg, lreg, il);	break;
		    case tok_gt: ejit_gti_i(state, lreg, lreg, il);	break;
		    default:	 ejit_nei_i(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_ltr_i(state, lreg, lreg, rreg);	break;
		    case tok_le: ejit_ler_i(state, lreg, lreg, rreg);	break;
		    case tok_eq: ejit_eqr_i(state, lreg, lreg, rreg);	break;
		    case tok_ge: ejit_ger_i(state, lreg, lreg, rreg);	break;
		    case tok_gt: ejit_gtr_i(state, lreg, lreg, rreg);	break;
		    default:	 ejit_ner_i(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_uint:
	    if (value_const_p(rval)) {
		il = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_lti_ui(state, lreg, lreg, il);	break;
		    case tok_le: ejit_lei_ui(state, lreg, lreg, il);	break;
		    case tok_eq: ejit_eqi_ui(state, lreg, lreg, il);	break;
		    case tok_ge: ejit_gei_ui(state, lreg, lreg, il);	break;
		    case tok_gt: ejit_gti_ui(state, lreg, lreg, il);	break;
		    default:	 ejit_nei_ui(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_ltr_ui(state, lreg, lreg, rreg);	break;
		    case tok_le: ejit_ler_ui(state, lreg, lreg, rreg);	break;
		    case tok_eq: ejit_eqr_ui(state, lreg, lreg, rreg);	break;
		    case tok_ge: ejit_ger_ui(state, lreg, lreg, rreg);	break;
		    case tok_gt: ejit_gtr_ui(state, lreg, lreg, rreg);	break;
		    default:	 ejit_ner_ui(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_long:
	    if (value_const_p(rval)) {
		il = rval->u.lval;
		switch (expr->token) {
		    case tok_lt: ejit_lti_l(state, lreg, lreg, il);	break;
		    case tok_le: ejit_lei_l(state, lreg, lreg, il);	break;
		    case tok_eq: ejit_eqi_l(state, lreg, lreg, il);	break;
		    case tok_ge: ejit_gei_l(state, lreg, lreg, il);	break;
		    case tok_gt: ejit_gti_l(state, lreg, lreg, il);	break;
		    default:	 ejit_nei_l(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_ltr_l(state, lreg, lreg, rreg);	break;
		    case tok_le: ejit_ler_l(state, lreg, lreg, rreg);	break;
		    case tok_eq: ejit_eqr_l(state, lreg, lreg, rreg);	break;
		    case tok_ge: ejit_ger_l(state, lreg, lreg, rreg);	break;
		    case tok_gt: ejit_gtr_l(state, lreg, lreg, rreg);	break;
		    default:	 ejit_ner_l(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_ulong:
	    if (value_const_p(rval)) {
		il = rval->u.lval;
		switch (expr->token) {
		    case tok_lt: ejit_lti_ul(state, lreg, lreg, il);	break;
		    case tok_le: ejit_lei_ul(state, lreg, lreg, il);	break;
		    case tok_eq: ejit_eqi_ul(state, lreg, lreg, il);	break;
		    case tok_ge: ejit_gei_ul(state, lreg, lreg, il);	break;
		    case tok_gt: ejit_gti_ul(state, lreg, lreg, il);	break;
		    default:	 ejit_nei_ul(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_ltr_ul(state, lreg, lreg, rreg);	break;
		    case tok_le: ejit_ler_ul(state, lreg, lreg, rreg);	break;
		    case tok_eq: ejit_eqr_ul(state, lreg, lreg, rreg);	break;
		    case tok_ge: ejit_ger_ul(state, lreg, lreg, rreg);	break;
		    case tok_gt: ejit_gtr_ul(state, lreg, lreg, rreg);	break;
		    default:	 ejit_ner_ul(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_float:
	    rreg  = rval->u.ival;
	    lfreg = lval->u.ival;
	    rfreg = rval->u.ival;
	    if (ltag->type == type_float) {
		if (rtag->type == type_float)
		    lreg = get_register(0);
		else
		    lreg = rreg;
		lval->u.ival = lreg;
	    }
	    /* else lreg is already an integer register */
	    switch (expr->token) {
		case tok_lt:	ejit_ltr_f(state, lreg, lfreg, rfreg);	break;
		case tok_le:	ejit_ler_f(state, lreg, lfreg, rfreg);	break;
		case tok_eq:	ejit_eqr_f(state, lreg, lfreg, rfreg);	break;
		case tok_ge:	ejit_ger_f(state, lreg, lfreg, rfreg);	break;
		case tok_gt:	ejit_gtr_f(state, lreg, lfreg, rfreg);	break;
		default:	ejit_ner_f(state, lreg, lfreg, rfreg);	break;
	    }
	    break;
	case type_double:
	    rreg  = rval->u.ival;
	    lfreg = lval->u.ival;
	    rfreg = rval->u.ival;
	    if (ltag->type == type_float || lval->type == type_double) {
		if (rtag->type == type_float || rtag->type == type_double)
		    lreg = get_register(0);
		else
		    lreg = rreg;
		lval->u.ival = lreg;
	    }
	    /* else lreg is already an integer register */
	    switch (expr->token) {
		case tok_lt:	ejit_ltr_d(state, lreg, lfreg, rfreg);	break;
		case tok_le:	ejit_ler_d(state, lreg, lfreg, rfreg);	break;
		case tok_eq:	ejit_eqr_d(state, lreg, lfreg, rfreg);	break;
		case tok_ge:	ejit_ger_d(state, lreg, lfreg, rfreg);	break;
		case tok_gt:	ejit_gtr_d(state, lreg, lfreg, rfreg);	break;
		default:	ejit_ner_d(state, lreg, lfreg, rfreg);	break;
	    }
	    break;
	default:
	    if (value_const_p(rval)) {
		ip = rval->u.pval;
		switch (expr->token) {
		    case tok_lt: ejit_lti_p(state, lreg, lreg, ip);	break;
		    case tok_le: ejit_lei_p(state, lreg, lreg, ip);	break;
		    case tok_eq: ejit_eqi_p(state, lreg, lreg, ip);	break;
		    case tok_ge: ejit_gei_p(state, lreg, lreg, ip);	break;
		    case tok_gt: ejit_gti_p(state, lreg, lreg, ip);	break;
		    default:	 ejit_nei_p(state, lreg, lreg, ip);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (expr->token) {
		    case tok_lt: ejit_ltr_p(state, lreg, lreg, rreg);	break;
		    case tok_le: ejit_ler_p(state, lreg, lreg, rreg);	break;
		    case tok_eq: ejit_eqr_p(state, lreg, lreg, rreg);	break;
		    case tok_ge: ejit_ger_p(state, lreg, lreg, rreg);	break;
		    case tok_gt: ejit_gtr_p(state, lreg, lreg, rreg);	break;
		    default:	 ejit_ner_p(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
    }
    dec_value_stack(1);
    lval->type = value_regno;

    return (int_tag);
}

static tag_t *
emit_binary(expr_t *expr, token_t token)
{
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;

    ltag = emit_expr(expr->data._binary.lvalue);
    lval = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rval = top_value_stack();
    emit_load(lval);

    return (emit_binary_next(expr, ltag, rtag, lval, rval, token));    
}

static tag_t *
emit_binary_next(expr_t *expr, tag_t *ltag, tag_t *rtag,
		 value_t *lval, value_t *rval, token_t token)
{
    long	 il;
    tag_t	*tag;
    int		 lreg;
    int		 rreg;

    tag = emit_binary_setup(expr, ltag, rtag, lval, rval, token);
    lreg = lval->u.ival;
    switch (tag->type) {
	case type_int:
	    if (value_const_p(rval)) {
		il = rval->u.ival;
		switch (token) {
		    case tok_and: ejit_andi_i(state, lreg, lreg, il);	break;
		    case tok_or:  ejit_ori_i (state, lreg, lreg, il);	break;
		    case tok_xor: ejit_xori_i(state, lreg, lreg, il);	break;
		    case tok_lsh: ejit_lshi_i(state, lreg, lreg, il);	break;
		    case tok_rsh: ejit_rshi_i(state, lreg, lreg, il);	break;
		    case tok_add: ejit_addi_i(state, lreg, lreg, il);	break;
		    case tok_sub: ejit_subi_i(state, lreg, lreg, il);	break;
		    case tok_mul: ejit_muli_i(state, lreg, lreg, il);	break;
		    case tok_div: ejit_divi_i(state, lreg, lreg, il);	break;
		    default:	  ejit_modi_i(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (token) {
		    case tok_and: ejit_andr_i(state, lreg, lreg, rreg);	break;
		    case tok_or:  ejit_orr_i (state, lreg, lreg, rreg);	break;
		    case tok_xor: ejit_xorr_i(state, lreg, lreg, rreg);	break;
		    case tok_lsh: ejit_lshr_i(state, lreg, lreg, rreg);	break;
		    case tok_rsh: ejit_rshr_i(state, lreg, lreg, rreg);	break;
		    case tok_add: ejit_addr_i(state, lreg, lreg, rreg);	break;
		    case tok_sub: ejit_subr_i(state, lreg, lreg, rreg);	break;
		    case tok_mul: ejit_mulr_i(state, lreg, lreg, rreg);	break;
		    case tok_div: ejit_divr_i(state, lreg, lreg, rreg);	break;
		    default:	  ejit_modr_i(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_uint:
	    if (value_const_p(rval)) {
		il = rval->u.ival;
		switch (token) {
		    case tok_and: ejit_andi_ui(state, lreg, lreg, il);	break;
		    case tok_or:  ejit_ori_ui (state, lreg, lreg, il);	break;
		    case tok_xor: ejit_xori_ui(state, lreg, lreg, il);	break;
		    case tok_lsh: ejit_lshi_ui(state, lreg, lreg, il);	break;
		    case tok_rsh: ejit_rshi_ui(state, lreg, lreg, il);	break;
		    case tok_add: ejit_addi_ui(state, lreg, lreg, il);	break;
		    case tok_sub: ejit_subi_ui(state, lreg, lreg, il);	break;
		    case tok_mul: ejit_muli_ui(state, lreg, lreg, il);	break;
		    case tok_div: ejit_divi_ui(state, lreg, lreg, il);	break;
		    default:	  ejit_modi_ui(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (token) {
		    case tok_and: ejit_andr_ui(state, lreg, lreg, rreg); break;
		    case tok_or:  ejit_orr_ui (state, lreg, lreg, rreg); break;
		    case tok_xor: ejit_xorr_ui(state, lreg, lreg, rreg); break;
		    case tok_lsh: ejit_lshr_ui(state, lreg, lreg, rreg); break;
		    case tok_rsh: ejit_rshr_ui(state, lreg, lreg, rreg); break;
		    case tok_add: ejit_addr_ui(state, lreg, lreg, rreg); break;
		    case tok_sub: ejit_subr_ui(state, lreg, lreg, rreg); break;
		    case tok_mul: ejit_mulr_ui(state, lreg, lreg, rreg); break;
		    case tok_div: ejit_divr_ui(state, lreg, lreg, rreg); break;
		    default:	  ejit_modr_ui(state, lreg, lreg, rreg); break;
		}
	    }
	    break;
	case type_long:
	    if (value_const_p(rval)) {
		il = rval->u.lval;
		switch (token) {
		    case tok_and: ejit_andi_l(state, lreg, lreg, il);	break;
		    case tok_or:  ejit_ori_l (state, lreg, lreg, il);	break;
		    case tok_xor: ejit_xori_l(state, lreg, lreg, il);	break;
		    case tok_lsh: ejit_lshi_l(state, lreg, lreg, il);	break;
		    case tok_rsh: ejit_rshi_l(state, lreg, lreg, il);	break;
		    case tok_add: ejit_addi_l(state, lreg, lreg, il);	break;
		    case tok_sub: ejit_subi_l(state, lreg, lreg, il);	break;
		    case tok_mul: ejit_muli_l(state, lreg, lreg, il);	break;
		    case tok_div: ejit_divi_l(state, lreg, lreg, il);	break;
		    default:	  ejit_modi_l(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (token) {
		    case tok_and: ejit_andr_l(state, lreg, lreg, rreg);	break;
		    case tok_or:  ejit_orr_l (state, lreg, lreg, rreg);	break;
		    case tok_xor: ejit_xorr_l(state, lreg, lreg, rreg);	break;
		    case tok_lsh: ejit_lshr_l(state, lreg, lreg, rreg);	break;
		    case tok_rsh: ejit_rshr_l(state, lreg, lreg, rreg);	break;
		    case tok_add: ejit_addr_l(state, lreg, lreg, rreg);	break;
		    case tok_sub: ejit_subr_l(state, lreg, lreg, rreg);	break;
		    case tok_mul: ejit_mulr_l(state, lreg, lreg, rreg);	break;
		    case tok_div: ejit_divr_l(state, lreg, lreg, rreg);	break;
		    default:	  ejit_modr_l(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_ulong:
	    if (value_const_p(rval)) {
		il = rval->u.lval;
		switch (token) {
		    case tok_and: ejit_andi_ul(state, lreg, lreg, il);	break;
		    case tok_or:  ejit_ori_ul (state, lreg, lreg, il);	break;
		    case tok_xor: ejit_xori_ul(state, lreg, lreg, il);	break;
		    case tok_lsh: ejit_lshi_ul(state, lreg, lreg, il);	break;
		    case tok_rsh: ejit_rshi_ul(state, lreg, lreg, il);	break;
		    case tok_add: ejit_addr_ul(state, lreg, lreg, il);	break;
		    case tok_sub: ejit_subr_ul(state, lreg, lreg, il);	break;
		    case tok_mul: ejit_mulr_ul(state, lreg, lreg, il);	break;
		    case tok_div: ejit_divr_ul(state, lreg, lreg, il);	break;
		    default:	  ejit_modi_ul(state, lreg, lreg, il);	break;
		}
	    }
	    else {
		rreg = rval->u.ival;
		switch (token) {
		    case tok_and: ejit_andr_ul(state, lreg, lreg, rreg); break;
		    case tok_or:  ejit_orr_ul (state, lreg, lreg, rreg); break;
		    case tok_xor: ejit_xorr_ul(state, lreg, lreg, rreg); break;
		    case tok_lsh: ejit_lshr_ul(state, lreg, lreg, rreg); break;
		    case tok_rsh: ejit_rshr_ul(state, lreg, lreg, rreg); break;
		    case tok_add: ejit_addr_ul(state, lreg, lreg, rreg); break;
		    case tok_sub: ejit_subr_ul(state, lreg, lreg, rreg); break;
		    case tok_mul: ejit_mulr_ul(state, lreg, lreg, rreg); break;
		    case tok_div: ejit_divr_ul(state, lreg, lreg, rreg); break;
		    default:	  ejit_modr_ul(state, lreg, lreg, rreg); break;
		}
	    }
	    break;
	case type_float:
	    rreg = rval->u.ival;
	    switch (token) {
		/* other operations already triggered an error */
		case tok_add:	ejit_addr_f(state, lreg, lreg, rreg);	break;
		case tok_sub:	ejit_subr_f(state, lreg, lreg, rreg);	break;
		case tok_mul:	ejit_mulr_f(state, lreg, lreg, rreg);	break;
		default:	ejit_divr_f(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_double:
	    rreg = rval->u.ival;
	    switch (token) {
		/* other operations already triggered an error */
		case tok_add:	ejit_addr_d(state, lreg, lreg, rreg);	break;
		case tok_sub:	ejit_subr_d(state, lreg, lreg, rreg);	break;
		case tok_mul:	ejit_mulr_d(state, lreg, lreg, rreg);	break;
		default:	ejit_divr_d(state, lreg, lreg, rreg);	break;
	    }
	    break;
	default:
	    /* only add/sub reach here and pointer type check already done */
	    tag = tag->tag;
	    if (token == tok_add) {
		if (value_const_p(rval)) {
		    if (pointer_type_p(ltag->type)) {
			il = rval->type & value_ltype ?
			    rval->u.lval : rval->u.ival;
			il *= tag->size;
			ejit_addi_p(state, lreg, lreg, (void *)il);
		    }
		    else {
			emit_load(rval);
			rreg = rval->u.ival;
			ejit_muli_l(state, lreg, lreg, tag->size);
			ejit_addr_p(state, lreg, lreg, rreg);
		    }
		}
		else {
		    rreg = rval->u.ival;
		    if (tag->size != 1) {
			if (pointer_type_p(ltag->type))
			    /* pointer + int */
			    ejit_muli_l(state, rreg, rreg, tag->size);
			else
			    /* int + pointer */
			    ejit_muli_l(state, lreg, lreg, tag->size);
		    }
		    ejit_addr_p(state, lreg, lreg, rreg);
		}
	    }
	    else {
		if (pointer_type_p(ltag->type) && pointer_type_p(rtag->type)) {
		    /* pointer - pointer only allowed in subtraction */
		    if (value_const_p(rval))
			ejit_subi_p(state, lreg, lreg, rval->u.pval);
		    else
			ejit_subr_p(state, lreg, lreg, rval->u.ival);
		    if (tag->size != 1)
			ejit_muli_l(state, lreg, lreg, tag->size);
		    tag = ulong_tag;
		    lval->type = value_utype | value_ltype | value_regno;
		}
		else {
		    /* only "pointer - int" reach here */
		    if (value_const_p(rval)) {
			il = rval->type & value_ltype ?
			    rval->u.lval : rval->u.ival;
			il *= tag->size;
			ejit_subi_p(state, lreg, lreg, (void *)il);
		    }
		    else {
			rreg = rval->u.ival;
			if (tag->size != 1)
			    ejit_muli_l(state, rreg, rreg, tag->size);
			ejit_subr_p(state, lreg, lreg, rreg);
		    }
		}
	    }
	    break;
    }
    dec_value_stack(1);

    return (tag);
}

static tag_t *
emit_binary_setup(expr_t *expr, tag_t *ltag, tag_t *rtag,
		  value_t *lval, value_t *rval, token_t token)
{
    tag_t	*tag;
    int		 freg;
    int		 lreg;
    int		 rreg;

    lreg = lval->u.ival;
    if (value_load_p(rval)) {
	emit_load(rval);
	rreg = rval->u.ival;
    }
    else
	rreg = -1;
    switch (ltag->type) {
	case type_char:			case type_short:	case type_int:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		int_int:
		    tag = int_tag;
		    lval->type = 0;
		    break;
		case type_long:		case type_ulong:
		int_long:
		    ejit_extr_i_l(state, lreg, lreg);
		    tag = long_tag;
		    lval->type = value_ltype;
		    break;
		case type_float:
		int_float:
		    switch (token) {
			case tok_and:	case tok_or:		case tok_xor:
			case tok_lsh:	case tok_rsh:
			int_error:
			    error(expr, "not an integer");
			default:
			    break;
		    }
		    if (value_const_p(rval)) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_ftype);
		    ejit_extr_i_f(state, freg, rreg);
		    tag = float_tag;
		    lval->u.ival = freg;
		    lval->type = value_ftype;
		    break;
		case type_double:
		int_double:
		    switch (token) {
			case tok_and:	case tok_or:		case tok_xor:
			case tok_lsh:	case tok_rsh:
			    goto int_error;
			default:
			    break;
		    }
		    if (value_const_p(rval)) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_dtype);
		    ejit_extr_i_d(state, freg, rreg);
		    tag = double_tag;
		    lval->u.ival = freg;
		    lval->type = value_dtype;
		    break;
		default:
		    switch (token) {
			case tok_add:
			    /* int + void* */
			    if (rtag->size == 0)
				goto type_error;
			case tok_lt:	case tok_le:		case tok_eq:
			case tok_ge:	case tok_gt:		case tok_ne:
			    if (pointer_type_p(rtag->type)) {
#if __WORDSIZE == 64
				ejit_extr_i_l(state, lreg, lreg);
#endif
				lval->type = value_ptype;
				tag = rtag;
				break;
			    }
			default:
			type_error:
			    error(expr, "invalid operation");
		    }
		    break;
	    }
	    break;
	case type_uchar:		case type_ushort:	case type_uint:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		    /* mixed signs */
		    goto int_int;
		case type_uchar:	case type_ushort:	case type_uint:
		    tag = uint_tag;
		    lval->type = value_utype;
		    break;
		case type_long:
		    /* mixed signs */
		    if (token == tok_lsh || token == tok_rsh)
			goto int_int;
		    goto int_long;
		case type_ulong:
		    ejit_extr_ui_ul(state, lreg, lreg);
		    tag = ulong_tag;
		    lval->type = value_utype | value_ltype;
		    break;
		case type_float:
		    /* FIXME unsigned */
		    goto int_float;
		case type_double:
		    /* FIXME unsigned */
		    goto int_double;
		default:
		    switch (token) {
			case tok_add:
			    if (rtag->size == 0)
				goto type_error;
			case tok_lt:	case tok_le:		case tok_eq:
			case tok_ge:	case tok_gt:		case tok_ne:
			    if (pointer_type_p(rtag->type)) {
#if __WORDSIZE == 64
				ejit_extr_ui_ul(state, lreg, lreg);
#endif
				lval->type = value_ptype;
				tag = rtag;
				break;
			    }
			default:
			    goto type_error;
		    }
		    break;
	    }
	    break;
	case type_long:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		long_int:
		    if (token == tok_lsh || token == tok_rsh) {
			tag = int_tag;
			lval->type = 0;
		    }
		    else {
			if (!value_const_p(rval))
			    ejit_extr_i_l(state, rreg, rreg);
			tag = long_tag;
			lval->type = value_ltype;
		    }
		    break;
		case type_long:		case type_ulong:
		long_long:
		    tag = long_tag;
		    lval->type = value_ltype;
		    break;
		case type_float:
		long_float:
		    switch (token) {
			case tok_and:	case tok_or:		case tok_xor:
			case tok_lsh:	case tok_rsh:
			    goto int_error;
			default:
			    break;
		    }
		    if (value_const_p(rval)) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_ftype);
		    ejit_extr_l_f(state, freg, rreg);
		    tag = float_tag;
		    lval->u.ival = freg;
		    lval->type = value_ftype;
		    break;
		case type_double:
		    long_double:
		    switch (token) {
			case tok_and:	case tok_or:		case tok_xor:
			case tok_lsh:	case tok_rsh:
			    goto int_error;
			default:
			    break;
		    }
		    if (value_const_p(rval)) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_dtype);
		    ejit_extr_l_d(state, freg, rreg);
		    tag = double_tag;
		    lval->u.ival = freg;
		    lval->type = value_dtype;
		    break;
		default:
		    switch (token) {
			case tok_add:
			    if (rtag->size == 0)
				goto type_error;
			case tok_lt:	case tok_le:		case tok_eq:
			case tok_ge:	case tok_gt:		case tok_ne:
			    if (pointer_type_p(rtag->type)) {
				lval->type = value_ptype;
				tag = rtag;
				break;
			    }
			default:
			    goto type_error;
		    }
		    break;
	    }
	    break;
	case type_ulong:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		    goto long_int;
		case type_uchar:	case type_ushort:	case type_uint:
		    if (token == tok_lsh || token == tok_rsh) {
			tag = uint_tag;
			lval->type = value_utype;
		    }
		    else {
			if (!value_const_p(rval))
			    ejit_extr_ui_ul(state, rreg, rreg);
			tag = ulong_tag;
			lval->type = value_utype | value_ltype;
		    }
		    break;
		case type_long:
		    goto long_long;
		case type_ulong:
		    tag = ulong_tag;
		    lval->type = value_utype | value_ltype;
		    break;
		case type_float:
		    /* FIXME unsigned */
		    goto long_float;
		case type_double:
		    /* FIXME unsigned */
		    goto long_double;
		default:
		    switch (token) {
			case tok_add:
			    if (rtag->size == 0)
				goto type_error;
			case tok_lt:	case tok_le:		case tok_eq:
			case tok_ge:	case tok_gt:		case tok_ne:
			    if (pointer_type_p(rtag->type)) {
				lval->type = value_ptype;
				tag = rtag;
				break;
			    }
			default:
			    goto type_error;
		    }
		    break;
	    }
	    break;
	case type_float:
	    switch (token) {
		case tok_and:		case tok_or:		case tok_xor:
		case tok_lsh:		case tok_rsh:
		    goto int_error;
		default:
		    break;
	    }
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		case type_long:		case type_ulong:
		    if (value_const_p(rval)) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_ftype);
		    if (rreg == -1) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    if (rtag->type == type_long || rtag->type == type_ulong)
			ejit_extr_l_f(state, freg, rreg);
		    else
			ejit_extr_i_f(state, freg, rreg);
		    tag = float_tag;
		    rval->u.ival = freg;
		    rval->type = value_ftype | value_regno;
		    break;
		case type_float:
		    tag = float_tag;
		    break;
		case type_double:
		    ejit_extr_f_d(state, lreg, lreg);
		    tag = double_tag;
		    lval->type = value_dtype;
		    break;
		default:
		    goto type_error;
	    }
	    break;
	case type_double:
	    switch (token) {
		case tok_and:		case tok_or:		case tok_xor:
		case tok_lsh:		case tok_rsh:
		    goto int_error;
		default:
		    break;
	    }
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		case type_long:		case type_ulong:
		    if (value_const_p(rval)) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_ftype);
		    if (rreg == -1) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    if (rtag->type == type_long || rtag->type == type_ulong)
			ejit_extr_l_d(state, freg, rreg);
		    else
			ejit_extr_i_d(state, freg, rreg);
		    tag = double_tag;
		    rval->u.ival = freg;
		    rval->type = value_dtype | value_regno;
		    break;
		case type_float:
		    ejit_extr_f_d(state, rreg, rreg);
		    tag = double_tag;
		    rval->type = value_dtype | value_regno;
		    break;
		case type_double:
		    tag = double_tag;
		    break;
		default:
		    goto type_error;
	    }
	    break;
	default:
	    if (!pointer_type_p(ltag->type) || ltag->tag->size == 0)
		goto type_error;
	    switch (token) {
		case tok_lt:		case tok_le:		case tok_eq:
		case tok_ge:		case tok_gt:		case tok_ne:
		case tok_add:		case tok_sub:
		    break;
		default:
		    goto type_error;
	    }
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
#if __WORDSIZE == 64
		    ejit_extr_i_l(state, lreg, lreg);
		    lval->type = value_ptype;
		    tag = ltag;
		    break;
#endif
		case type_uchar:	case type_ushort:	case type_uint:
#if __WORDSIZE == 64
		    ejit_extr_ui_ul(state, lreg, lreg);
		    lval->type = value_ptype;
		    tag = ltag;
		    break;
#endif
		case type_long:		case type_ulong:
		    lval->type = value_ptype;
		    tag = ltag;
		    break;
		default:
		    if (token == tok_sub &&
			(rtag == ltag ||
			 (rtag->type ^ ltag->type) == type_unsigned)) {
			lval->type = value_ptype;
			tag = ltag;
			break;
		    }
		case type_float:	case type_double:
		    goto type_error;
	    }
	    break;
    }
    lval->type |= value_regno;

    return (tag);
}

static void
emit_coerce_const(expr_t *expr, tag_t *tag, value_t *value)
{
    switch (value->type) {
	case value_itype:		case value_utype:
	    switch (tag->type) {
		case type_long:		case type_ulong:
		    value->u.lval = value->u.ival;
		    value->type = value_ltype;
		    break;
		case type_float:
		    value->u.fval = value->u.ival;
		    value->type = value_ftype;
		    break;
		case type_double:
		    value->u.dval = value->u.ival;
		    value->type = value_dtype;
		    break;
		default:
		    break;
	    }
	    break;
	case value_ltype:		case value_ultype:
	    switch (tag->type) {
		case type_float:
		    value->u.fval = value->u.lval;
		    value->type = value_ftype;
		    break;
		case type_double:
		    value->u.dval = value->u.lval;
		    value->type = value_dtype;
		    break;
		default:
		    break;
	    }
	    break;
	case value_ftype:
	    switch (tag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		    value->u.ival = value->u.fval;
		    value->type = value_itype;
		    break;
		case type_long:		case type_ulong:
		    value->u.lval = value->u.fval;
		    value->type = value_ltype;
		    break;
		default:
		    break;
	    }
	    break;
	case value_dtype:
	    switch (tag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		    value->u.ival = value->u.dval;
		    value->type = value_itype;
		    break;
		case type_long:		case type_ulong:
		    value->u.lval = value->u.dval;
		    value->type = value_ltype;
		    break;
		default:
		    break;
	    }
	    break;
	default:
	    break;
    }
}

/* value must be a live register */
static void
emit_coerce(expr_t *expr, tag_t *tag, value_t *value)
{
    int		flt, oreg, nreg;

    flt = value->type & (value_ftype | value_dtype);
    oreg = value->u.ival;
    switch (tag->type) {
	case type_char:		case type_short:	case type_int:
	case type_uchar:	case type_ushort:	case type_uint:
	    if (flt) {
		nreg = get_register(0);
		if (flt & value_ftype)
		    ejit_truncr_f_i(state, nreg, oreg);
		else
		    ejit_truncr_d_i(state, nreg, oreg);
		value->type = value_regno;
		value->u.ival = nreg;
	    }
	    break;
	case type_long:		case type_ulong:
	    if (flt) {
		nreg = get_register(0);
		if (flt & value_ftype)
		    ejit_truncr_f_l(state, nreg, oreg);
		else
		    ejit_truncr_d_l(state, nreg, oreg);
		value->type = value_regno;
		value->u.ival = nreg;
	    }
	    break;
	case type_float:
	    if (value->type & value_ptype)
		error(expr, "invalid conversion");
	    if (flt == 0) {
		nreg = get_register(value_ftype);
		if (!(value->type & value_ltype))
		    ejit_extr_i_f(state, nreg, oreg);
		else
		    ejit_extr_l_f(state, nreg, oreg);
		value->u.ival = nreg;
	    }
	    else if (flt == value_dtype)
		ejit_extr_d_f(state, oreg, oreg);
	    value->type = value_ftype | value_regno;
	    break;
	case type_double:
	    if (value->type & value_ptype)
		error(expr, "invalid conversion");
	    if (flt == 0) {
		nreg = get_register(value_dtype);
		if (!(value->type & value_ltype))
		    ejit_extr_i_d(state, nreg, oreg);
		else
		    ejit_extr_l_d(state, nreg, oreg);
		value->u.ival = nreg;
	    }
	    else if (flt == value_ftype)
		ejit_extr_f_d(state, oreg, oreg);
	    value->type = value_dtype | value_regno;
	    break;
	default:
	    if (flt || !pointer_type_p(tag->type))
		error(expr, "invalid conversion");
#if __WORDSIZE == 64
	    if (!(value->type & (value_ptype | value_ltype))) {
		if (value->type & value_utype)
		    ejit_extr_ui_ul(state, oreg, oreg);
		else
		    ejit_extr_i_l(state, oreg, oreg);
	    }
#endif
	    value->type = value_ptype | value_regno;
	    break;
    }
}

static void
emit_test_branch(expr_t *expr, int jmpif, int level)
{
    long	 il;
    void	*ip;
    int		 freg;
    int		 uoff;
    int		 loff;
    ejit_node_t	*node;
    int		 lreg;
    int		 rreg;
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    jump_t	*ajump;
    jump_t	*bjump;
    ejit_node_t	*label;

    switch (expr->token) {
	case tok_andand:	case tok_oror:
	    uoff = bstack.offset - 1;
	    loff = bstack.offset;
	    inc_branch_stack(expr->token);
	    /* evaluate left operand */
	    emit_test_branch(expr->data._binary.lvalue,
			     expr->token == tok_oror, level + 1);
	    ajump = bstack.tjump + loff;
	    if (ajump->offset) {
		if (expr->token == tok_oror) {
		    /* lift true test jumps down for oror */
		    bjump = bstack.tjump + uoff;
		    do
			add_jump(bjump, ajump->table[--ajump->offset]);
		    while (ajump->offset);
		}
		else /* must check next test for andand */ {
		    label = ejit_label(state);
		    do
			ejit_patch(state, label, ajump->table[--ajump->offset]);
		    while (ajump->offset);
		}
	    }
	    ajump = bstack.fjump + loff;
	    if (ajump->offset) {
		if (expr->token == tok_andand) {
		    /* lift false test jumps down for andand */
		    bjump = bstack.fjump + uoff;
		    do
			add_jump(bjump, ajump->table[--ajump->offset]);
		    while (ajump->offset);
		}
		else /* must check next test for oror */ {
		    label = ejit_label(state);
		    do
			ejit_patch(state, label, ajump->table[--ajump->offset]);
		    while (ajump->offset);
		}
	    }
	    /* evaluate right operand */
	    emit_test_branch(expr->data._binary.rvalue, 0, level + 1);
	    dec_branch_stack(1);
	    ajump = bstack.tjump + loff;
	    if (ajump->offset) {
		if (level > 0) {
		    /* lift true test jumps down if nested */
		    bjump = bstack.tjump + uoff;
		    do
			add_jump(bjump, ajump->table[--ajump->offset]);
		    while (ajump->offset);
		}
		else {
		    /* add label before true code if at toplevel */
		    label = ejit_label(state);
		    do
			ejit_patch(state, label, ajump->table[--ajump->offset]);
		    while (ajump->offset);
		}
	    }
	    ajump = bstack.fjump + loff;
	    if (ajump->offset) {
		/* always lift down rigt operand evaluating to false */
		bjump = bstack.fjump + uoff;
		do
		    add_jump(bjump, ajump->table[--ajump->offset]);
		while (ajump->offset);
	    }
	    break;
	case tok_lt:		case tok_le:		case tok_eq:
	case tok_ge:		case tok_gt:		case tok_ne:
	    ltag = emit_expr(expr->data._binary.lvalue);
	    lval = top_value_stack();
	    rtag = emit_expr(expr->data._binary.rvalue);
	    rval = top_value_stack();
	    emit_load(lval);
	    ltag = emit_binary_setup(expr, ltag, rtag, lval, rval, expr->token);
	    lreg = lval->u.ival;
	    switch (ltag->type) {
		case type_int:
		    if (value_const_p(rval)) {
			il = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_blti_i(state, NULL, lreg, il);
				else
				    node = ejit_bgei_i(state, NULL, lreg, il);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_blei_i(state, NULL, lreg, il);
				else
				    node = ejit_bgti_i(state, NULL, lreg, il);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqi_i(state, NULL, lreg, il);
				else
				    node = ejit_bnei_i(state, NULL, lreg, il);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bgei_i(state, NULL, lreg, il);
				else
				    node = ejit_blti_i(state, NULL, lreg, il);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgti_i(state, NULL, lreg, il);
				else
				    node = ejit_blei_i(state, NULL, lreg, il);
				break;
			    default:
				if (jmpif)
				    node = ejit_bnei_i(state, NULL, lreg, il);
				else
				    node = ejit_beqi_i(state, NULL, lreg, il);
				break;
			}
		    }
		    else {
			rreg = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_bltr_i(state, NULL, lreg, rreg);
				else
				    node = ejit_bger_i(state, NULL, lreg, rreg);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_bler_i(state, NULL, lreg, rreg);
				else
				    node = ejit_bgtr_i(state, NULL, lreg, rreg);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqr_i(state, NULL, lreg, rreg);
				else
				    node = ejit_bner_i(state, NULL, lreg, rreg);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bger_i(state, NULL, lreg, rreg);
				else
				    node = ejit_bltr_i(state, NULL, lreg, rreg);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgtr_i(state, NULL, lreg, rreg);
				else
				    node = ejit_bler_i(state, NULL, lreg, rreg);
				break;
			    default:
				if (jmpif)
				    node = ejit_bner_i(state, NULL, lreg, rreg);
				else
				    node = ejit_beqr_i(state, NULL, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_uint:
		    if (value_const_p(rval)) {
			il = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_blti_ui(state, NULL, lreg, il);
				else
				    node = ejit_bgei_ui(state, NULL, lreg, il);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_blei_ui(state, NULL, lreg, il);
				else
				    node = ejit_bgti_ui(state, NULL, lreg, il);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqi_ui(state, NULL, lreg, il);
				else
				    node = ejit_bnei_ui(state, NULL, lreg, il);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bgei_ui(state, NULL, lreg, il);
				else
				    node = ejit_blti_ui(state, NULL, lreg, il);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgti_ui(state, NULL, lreg, il);
				else
				    node = ejit_blei_ui(state, NULL, lreg, il);
				break;
			    default:
				if (jmpif)
				    node = ejit_bnei_ui(state, NULL, lreg, il);
				else
				    node = ejit_beqi_ui(state, NULL, lreg, il);
				break;
			}
		    }
		    else {
			rreg = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_bltr_ui(state, NULL, lreg, rreg);
				else
				    node = ejit_bger_ui(state, NULL, lreg, rreg);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_bler_ui(state, NULL, lreg, rreg);
				else
				    node = ejit_bgtr_ui(state, NULL, lreg, rreg);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqr_ui(state, NULL, lreg, rreg);
				else
				    node = ejit_bner_ui(state, NULL, lreg, rreg);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bger_ui(state, NULL, lreg, rreg);
				else
				    node = ejit_bltr_ui(state, NULL, lreg, rreg);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgtr_ui(state, NULL, lreg, rreg);
				else
				    node = ejit_bler_ui(state, NULL, lreg, rreg);
				break;
			    default:
				if (jmpif)
				    node = ejit_bner_ui(state, NULL, lreg, rreg);
				else
				    node = ejit_beqr_ui(state, NULL, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_long:
		    if (value_const_p(rval)) {
			il = rval->u.lval;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_blti_l(state, NULL, lreg, il);
				else
				    node = ejit_bgei_l(state, NULL, lreg, il);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_blei_l(state, NULL, lreg, il);
				else
				    node = ejit_bgti_l(state, NULL, lreg, il);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqi_l(state, NULL, lreg, il);
				else
				    node = ejit_bnei_l(state, NULL, lreg, il);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bgei_l(state, NULL, lreg, il);
				else
				    node = ejit_blti_l(state, NULL, lreg, il);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgti_l(state, NULL, lreg, il);
				else
				    node = ejit_blei_l(state, NULL, lreg, il);
				break;
			    default:
				if (jmpif)
				    node = ejit_bnei_l(state, NULL, lreg, il);
				else
				    node = ejit_beqi_l(state, NULL, lreg, il);
				break;
			}
		    }
		    else {
			rreg = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_bltr_l(state, NULL, lreg, rreg);
				else
				    node = ejit_bger_l(state, NULL, lreg, rreg);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_bler_l(state, NULL, lreg, rreg);
				else
				    node = ejit_bgtr_l(state, NULL, lreg, rreg);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqr_l(state, NULL, lreg, rreg);
				else
				    node = ejit_bner_l(state, NULL, lreg, rreg);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bger_l(state, NULL, lreg, rreg);
				else
				    node = ejit_bltr_l(state, NULL, lreg, rreg);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgtr_l(state, NULL, lreg, rreg);
				else
				    node = ejit_bler_l(state, NULL, lreg, rreg);
				break;
			    default:
				if (jmpif)
				    node = ejit_bner_l(state, NULL, lreg, rreg);
				else
				    node = ejit_beqr_l(state, NULL, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_ulong:
		    if (value_const_p(rval)) {
			il = rval->u.lval;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_blti_ul(state, NULL, lreg, il);
				else
				    node = ejit_bgei_ul(state, NULL, lreg, il);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_blei_ul(state, NULL, lreg, il);
				else
				    node = ejit_bgti_ul(state, NULL, lreg, il);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqi_ul(state, NULL, lreg, il);
				else
				    node = ejit_bnei_ul(state, NULL, lreg, il);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bgei_ul(state, NULL, lreg, il);
				else
				    node = ejit_blti_ul(state, NULL, lreg, il);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgti_ul(state, NULL, lreg, il);
				else
				    node = ejit_blei_ul(state, NULL, lreg, il);
				break;
			    default:
				if (jmpif)
				    node = ejit_bnei_ul(state, NULL, lreg, il);
				else
				    node = ejit_beqi_ul(state, NULL, lreg, il);
				break;
			}
		    }
		    else {
			rreg = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_bltr_ul(state, NULL, lreg, rreg);
				else
				    node = ejit_bger_ul(state, NULL, lreg, rreg);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_bler_ul(state, NULL, lreg, rreg);
				else
				    node = ejit_bgtr_ul(state, NULL, lreg, rreg);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqr_ul(state, NULL, lreg, rreg);
				else
				    node = ejit_bner_ul(state, NULL, lreg, rreg);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bger_ul(state, NULL, lreg, rreg);
				else
				    node = ejit_bltr_ul(state, NULL, lreg, rreg);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgtr_ul(state, NULL, lreg, rreg);
				else
				    node = ejit_bler_ul(state, NULL, lreg, rreg);
				break;
			    default:
				if (jmpif)
				    node = ejit_bner_ul(state, NULL, lreg, rreg);
				else
				    node = ejit_beqr_ul(state, NULL, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_float:
		    if (value_const_p(rval))
			emit_load(rval);
		    rreg = rval->u.ival;
		    switch (expr->token) {
			case tok_lt:
			    if (jmpif)
				node = ejit_bltr_f(state, NULL, lreg, rreg);
			    else
				node = ejit_bunger_f(state, NULL, lreg, rreg);
			    break;
			case tok_le:
			    if (jmpif)
				node = ejit_bler_f(state, NULL, lreg, rreg);
			    else
				node = ejit_bungtr_f(state, NULL, lreg, rreg);
			    break;
			case tok_eq:
			    if (jmpif)
				node = ejit_beqr_f(state, NULL, lreg, rreg);
			    else
				node = ejit_bltgtr_f(state, NULL, lreg, rreg);
			    break;
			case tok_ge:
			    if (jmpif)
				node = ejit_bger_f(state, NULL, lreg, rreg);
			    else
				node = ejit_bunltr_f(state, NULL, lreg, rreg);
			    break;
			case tok_gt:
			    if (jmpif)
				node = ejit_bgtr_f(state, NULL, lreg, rreg);
			    else
				node = ejit_bunler_f(state, NULL, lreg, rreg);
			    break;
			default:
			    if (jmpif)
				node = ejit_bner_f(state, NULL, lreg, rreg);
			    else
				node = ejit_buneqr_f(state, NULL, lreg, rreg);
			    break;
		    }
		    break;
		case type_double:
		    if (value_const_p(rval))
			emit_load(rval);
		    rreg = rval->u.ival;
		    switch (expr->token) {
			case tok_lt:
			    if (jmpif)
				node = ejit_bltr_d(state, NULL, lreg, rreg);
			    else
				node = ejit_bunger_d(state, NULL, lreg, rreg);
			    break;
			case tok_le:
			    if (jmpif)
				node = ejit_bler_d(state, NULL, lreg, rreg);
			    else
				node = ejit_bungtr_d(state, NULL, lreg, rreg);
			    break;
			case tok_eq:
			    if (jmpif)
				node = ejit_beqr_d(state, NULL, lreg, rreg);
			    else
				node = ejit_bltgtr_d(state, NULL, lreg, rreg);
			    break;
			case tok_ge:
			    if (jmpif)
				node = ejit_bger_d(state, NULL, lreg, rreg);
			    else
				node = ejit_bunltr_d(state, NULL, lreg, rreg);
			    break;
			case tok_gt:
			    if (jmpif)
				node = ejit_bgtr_d(state, NULL, lreg, rreg);
			    else
				node = ejit_bunler_d(state, NULL, lreg, rreg);
			    break;
			default:
			    if (jmpif)
				node = ejit_bner_d(state, NULL, lreg, rreg);
			    else
				node = ejit_buneqr_d(state, NULL, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    if (value_const_p(rval)) {
			ip = rval->u.pval;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_blti_p(state, NULL, lreg, ip);
				else
				    node = ejit_bgei_p(state, NULL, lreg, ip);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_blei_p(state, NULL, lreg, ip);
				else
				    node = ejit_bgti_p(state, NULL, lreg, ip);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqi_p(state, NULL, lreg, ip);
				else
				    node = ejit_bnei_p(state, NULL, lreg, ip);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bgei_p(state, NULL, lreg, ip);
				else
				    node = ejit_blti_p(state, NULL, lreg, ip);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgti_p(state, NULL, lreg, ip);
				else
				    node = ejit_blei_p(state, NULL, lreg, ip);
				break;
			    default:
				if (jmpif)
				    node = ejit_bnei_p(state, NULL, lreg, ip);
				else
				    node = ejit_beqi_p(state, NULL, lreg, ip);
				break;
			}
		    }
		    else {
			rreg = rval->u.ival;
			switch (expr->token) {
			    case tok_lt:
				if (jmpif)
				    node = ejit_bltr_p(state, NULL, lreg, rreg);
				else
				    node = ejit_bger_p(state, NULL, lreg, rreg);
				break;
			    case tok_le:
				if (jmpif)
				    node = ejit_bler_p(state, NULL, lreg, rreg);
				else
				    node = ejit_bgtr_p(state, NULL, lreg, rreg);
				break;
			    case tok_eq:
				if (jmpif)
				    node = ejit_beqr_p(state, NULL, lreg, rreg);
				else
				    node = ejit_bner_p(state, NULL, lreg, rreg);
				break;
			    case tok_ge:
				if (jmpif)
				    node = ejit_bger_p(state, NULL, lreg, rreg);
				else
				    node = ejit_bltr_p(state, NULL, lreg, rreg);
				break;
			    case tok_gt:
				if (jmpif)
				    node = ejit_bgtr_p(state, NULL, lreg, rreg);
				else
				    node = ejit_bler_p(state, NULL, lreg, rreg);
				break;
			    default:
				if (jmpif)
				    node = ejit_bner_p(state, NULL, lreg, rreg);
				else
				    node = ejit_beqr_p(state, NULL, lreg, rreg);
				break;
			}
		    }
		    break;
	    }
	    if (jmpif)		add_tjump(node);
	    else		add_fjump(node);
	    dec_value_stack(2);
	    break;
	case tok_int:
	    node = ejit_jmpi(state, NULL);
	    if (!!expr->data._unary.i ^ !jmpif)
		add_fjump(node);
	    else
		add_tjump(node);
	    break;
	case tok_float:
	    node = ejit_jmpi(state, NULL);
	    if (!!expr->data._unary.d ^ !jmpif)
		add_fjump(node);
	    else
		add_tjump(node);
	    break;
	default:
	    ltag = emit_expr(expr);
	    lval = top_value_stack();
	    emit_load(lval);
	    lreg = lval->u.ival;
	    switch (lval->type & ~value_regno) {
		case value_itype:
		    if (jmpif)	node = ejit_bnei_i(state, NULL, lreg, 0);
		    else	node = ejit_beqi_i(state, NULL, lreg, 0);
		    break;
		case value_utype:
		    if (jmpif)	node = ejit_bnei_ui(state, NULL, lreg, 0);
		    else	node = ejit_beqi_ui(state, NULL, lreg, 0);
		    break;
		case value_ltype:
		    if (jmpif)	node = ejit_bnei_l(state, NULL, lreg, 0);
		    else	node = ejit_beqi_l(state, NULL, lreg, 0);
		    break;
		case value_ultype:
		    if (jmpif)	node = ejit_bnei_ul(state, NULL, lreg, 0);
		    else	node = ejit_beqi_ul(state, NULL, lreg, 0);
		    break;
		case value_ftype:
		    freg = get_register(value_ftype);
		    ejit_movi_f(state, freg, 0.0);
		    if (jmpif)	node = ejit_bner_f(state, NULL, lreg, freg);
		    else	node = ejit_beqr_f(state, NULL, lreg, freg);
		    break;
		case value_dtype:
		    freg = get_register(value_dtype);
		    ejit_movi_d(state, freg, 0.0);
		    if (jmpif)	node = ejit_bner_d(state, NULL, lreg, freg);
		    else	node = ejit_beqr_d(state, NULL, lreg, freg);
		    break;
		default:
		    if (jmpif)	node = ejit_bnei_p(state, NULL, lreg, NULL);
		    else	node = ejit_beqi_p(state, NULL, lreg, NULL);
		    break;
	    }
	    if (jmpif)		add_tjump(node);
	    else		add_fjump(node);
	    dec_value_stack(1);
	    break;
    }
}

static tag_t *
emit_question(expr_t *expr)
{
    int		 lreg;
    int		 rreg;
    value_t	*lval;
    value_t	*rval;
    tag_t	*ltag;
    tag_t	*rtag;
    jump_t	*jump;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = vstack.offset;

    inc_branch_stack(tok_question);
    emit_test_branch(expr->data._if.test, 0, 0);
    vstack_reset(offset);
    jump = bstack.tjump + bstack.offset - 1;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    ltag = emit_expr(expr->data._if.tcode);
    lval = top_value_stack();
    if (value_const_p(lval))
	emit_load(lval);
    lreg = lval->u.ival;
    node = ejit_jmpi(state, NULL);
    /* release register so that result of false condition may
     * leave result in the same register */
    dec_value_stack(1);
    dec_branch_stack(1);
    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    rtag = emit_expr(expr->data._if.fcode);
    /* actually, rval == lval */
    rval = top_value_stack();
    if (value_const_p(rval))
	emit_load(rval);
    rreg = rval->u.ival;
    if (lreg != rreg) {
	switch (ltag->type) {
	    case type_char:	case type_short:	case type_int:
		ejit_movr_i(state, lreg, rreg);
		break;
	    case type_uchar:	case type_ushort:	case type_uint:
		ejit_movr_ui(state, lreg, rreg);
		break;
	    case type_long:
		ejit_movr_l(state, lreg, rreg);
		break;
	    case type_ulong:
		ejit_movr_ul(state, lreg, rreg);
		break;
	    case type_float:
		ejit_movr_f(state, lreg, rreg);
		break;
	    case type_double:
		ejit_movr_d(state, lreg, rreg);
		break;
	    default:
		ejit_movr_p(state, lreg, rreg);
		break;
	}
	/* both paths leave expression in same register */
	rval->u.ival = lreg;
    }
    label = ejit_label(state);
    ejit_patch(state, label, node);
    if ((ltag->type & ~type_unsigned) != (rtag->type & ~type_unsigned))
	error(expr, "different type results");

    return (ltag);
}

static tag_t *
emit_if(expr_t *expr)
{
    jump_t	*jump;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = vstack.offset;
    expr_t	*test = expr->data._if.test;

    for (; test->next; test = test->next) {
	(void)emit_expr(test);
	vstack_reset(offset);
    }

    inc_branch_stack(tok_if);
    emit_test_branch(test, 0, 0);
    jump = bstack.tjump + bstack.offset - 1;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    emit_stat(expr->data._if.tcode);
    if (expr->data._if.fcode)
	node = ejit_jmpi(state, NULL);
    dec_branch_stack(1);
    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    if (expr->data._if.fcode) {
	emit_stat(expr->data._if.fcode);
	label = ejit_label(state);
	ejit_patch(state, label, node);
    }

    vstack_reset(offset);

    return (void_tag);
}

static tag_t *
emit_while(expr_t *expr)
{
    jump_t	*jump;
    expr_t	*test;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = vstack.offset;

    /* start of loop */
    label = ejit_label(state);
    for (test = expr->data._while.test; expr->next; expr = expr->next) {
	(void)emit_expr(test);
	vstack_reset(offset);
    }
    inc_branch_stack(tok_while);
    emit_test_branch(test, 0, 0);
    jump = bstack.tjump + bstack.offset - 1;
    if (jump->offset) {
	node = ejit_label(state);
	do
	    ejit_patch(state, node, jump->table[--jump->offset]);
	while (jump->offset);
    }
    emit_stat(expr->data._while.code);
    dec_branch_stack(1);

    /* true condition jump (also continue target) is now to start of loop */
    node = ejit_jmpi(state, label);
    /* FIXME this may look wrong and easy to get confused
     * but the requirement to patch a backward jump is to
     * tell it the target is a node, what makes few sense
     * here, but possible to have jumps to known addresses...
     * it is more a point for functions, so that, when calling
     * ejit_patch, it should add it to the proper linked list
     * of patches to be done at actual jit generation */
    ejit_patch(state, label, node);

    jump = bstack.tjump + bstack.offset;
    if (jump->offset) {
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    /* exit of loop */
    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }

    vstack_reset(offset);

    return (void_tag);
}

static tag_t *
emit_do(expr_t *expr)
{
    jump_t	*jump;
    expr_t	*test;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = vstack.offset;

    /* start of loop */
    label = ejit_label(state);
    inc_branch_stack(tok_do);
    emit_stat(expr->data._do.code);
    for (test = expr->data._do.test; expr->next; expr = expr->next) {
	(void)emit_expr(test);
	vstack_reset(offset);
    }
    emit_test_branch(test, 0, 0);
    dec_branch_stack(1);
    jump = bstack.tjump + bstack.offset;
    if (jump->offset) {
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    node = ejit_jmpi(state, label);
    ejit_patch(state, label, node);
    /* exit of loop */
    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }

    vstack_reset(offset);

    return (void_tag);
}

static tag_t *
emit_for(expr_t *expr)
{
    jump_t	*jump;
    expr_t	*test;
    ejit_node_t	*init;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = vstack.offset;

    if (expr->data._for.init)
	(void)emit_expr(expr->data._for.init);
    if (expr->data._for.incr)
	node = ejit_jmpi(state, NULL);

    /* start of loop */
    label = ejit_label(state);

    if (expr->data._for.incr) {
	/* jump over single time expanded increment */
	emit_expr(expr->data._for.incr);
	init = ejit_label(state);
	ejit_patch(state, init, node);
    }

    for (test = expr->data._for.test; expr->next; expr = expr->next) {
	(void)emit_expr(test);
	vstack_reset(offset);
    }
    inc_branch_stack(tok_for);
    if (test) {
	emit_test_branch(test, 0, 0);
	jump = bstack.tjump + bstack.offset - 1;
	if (jump->offset) {
	    node = ejit_label(state);
	    do
		ejit_patch(state, node, jump->table[--jump->offset]);
	    while (jump->offset);
	}
    }
    emit_stat(expr->data._for.code);
    dec_branch_stack(1);
    /* loop */
    node = ejit_jmpi(state, label);
    ejit_patch(state, label, node);

    jump = bstack.tjump + bstack.offset;
    if (jump->offset) {
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    /* exit of loop */
    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }

    vstack_reset(offset);

    return (void_tag);
}

static tag_t *
emit_break(expr_t *expr)
{
    ejit_node_t	*node;
    jump_t	*jump;
    int		 offset = bstack.offset;

    while (offset > 0) {
	jump = bstack.fjump + --offset;
	switch (jump->token) {
	    case tok_do:	case tok_for:		case tok_while:
	    case tok_switch:
		node = ejit_jmpi(state, NULL);
		add_jump(jump, node);
		return (void_tag);
	    default:
		break;
	}
    }
    error(expr, "internal error");
}

static tag_t *
emit_continue(expr_t *expr)
{
    ejit_node_t	*node;
    jump_t	*jump;
    int		 offset = bstack.offset;

    while (offset > 0) {
	jump = bstack.tjump + --offset;
	switch (jump->token) {
	    case tok_do:	case tok_for:		case tok_while:
		node = ejit_jmpi(state, NULL);
		add_jump(jump, node);
		return (void_tag);
	    default:
		break;
	}
    }
    error(expr, "internal error");
}

/* very simple implementation */
/* FIXME this should be good enough for a first implementation, but some
 * support for jump tables is desirable; in this case, it should be stored
 * in "the_rodata", and possible implementation could be to allocate a
 * vector with a power of two size where jump targets are stored, and
 * unused entries filled with the default target if any or exit of block,
 * and, the test value would be "and'ed" with jump_table_size-1, and
 * there it would implement basically the logic here comparing every
 * entry that did match
 * FIXME it could also have different strategies, simplest one would be
 * if the min-max range is small enough, and then, test/adjust once and
 * jump to target/default/out of block */
static tag_t *
emit_switch(expr_t *expr)
{
    jump_t	*jump;
    ejit_node_t	*node;
    expr_t	*test;
    entry_t	*entry;
    ejit_node_t	*label;
    int		 regno;
    value_t	*value;
    int		 offset;
    int		 voffset = vstack.offset;

    for (test = expr->data._switch.test; test->next; test = test->next) {
	(void)emit_expr(test);
	vstack_reset(voffset);
    }
    if (emit_expr(test) != int_tag)
	error(test, "switch test is not an integer");
    value = top_value_stack();
    emit_load(value);
    regno = value->u.ival;

    /* order of comparisons is not order of definition, but sorted
     * order of "case->value & hash->size-1" */
    for (offset = 0; offset < expr->data._switch.hash->size; offset++) {
	for (entry = expr->data._switch.hash->entries[offset];
	     entry; entry = entry->next) {
	    test = entry->value;
	    test->data._binary.rvalue = (expr_t *)
		ejit_beqi_i(state, NULL, regno, (int)entry->name.integer);
	}
    }
    node = ejit_jmpi(state, NULL);
    if (expr->data._switch._default)
	expr->data._switch._default->data._unary.vp = node;

    dec_value_stack(vstack.offset - voffset);
    inc_branch_stack(tok_switch);
    emit_expr(expr->data._switch.code);
    dec_branch_stack(1);

    /* there is no use for true condition jump */
    assert(bstack.tjump[bstack.offset].offset == 0);

    /* break targets */
    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	if (expr->data._switch._default == NULL)
	    ejit_patch(state, label, node);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    else if (expr->data._switch._default == NULL) {
	label = ejit_label(state);
	ejit_patch(state, label, node);
    }

    vstack_reset(voffset);

    return (void_tag);
}

static tag_t *
emit_case(expr_t *expr)
{
    jump_t	*jump;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = bstack.offset;

    while (offset > 0) {
	jump = bstack.tjump + --offset;
	if (jump->token == tok_switch) {
	    label = ejit_label(state);
	    node = (ejit_node_t *)expr->data._binary.rvalue;
	    ejit_patch(state, label, node);
	    return (void_tag);
	}
    }
    error(expr, "internal error");
}

static tag_t *
emit_default(expr_t *expr)
{
    jump_t	*jump;
    ejit_node_t	*node;
    ejit_node_t	*label;
    int		 offset = bstack.offset;

    while (offset > 0) {
	jump = bstack.tjump + --offset;
	if (jump->token == tok_switch) {
	    label = ejit_label(state);
	    node = expr->data._unary.vp;
	    ejit_patch(state, label, node);
	    return (void_tag);
	}
    }
    error(expr, "internal error");
}

static tag_t *
emit_function(expr_t *expr)
{
    jump_t	*jump;
    ejit_node_t	*label;
    value_t	*value;
    int		 offset;
    symbol_t	*symbol;
    function_t	*function;

    ejit_prolog(state);

    inc_branch_stack(tok_function);
    function = expr->data._function.function;
    current = function->table;
    for (offset = 0; offset < current->count; offset++) {
	/* FIXME need extra logic for local variables
	 * declared in block scope */
	symbol = current->vector[offset];
	variable(state, symbol);
	if (symbol->reg) {
	    value = get_value_stack();
	    switch (symbol->tag->type) {
		case type_float:
		    value->type = value_ftype;
		    break;
		case type_double:
		    value->type = value_dtype;
		    break;
		case type_char:		case type_short:	case type_int:
		    value->type = value_itype;
		    break;
		case type_uchar:	case type_ushort:	case type_uint:
		    value->type = value_utype;
		    break;
		case type_long:
		    value->type = value_ltype;
		    break;
		case type_ulong:
		    value->type = value_ultype;
		    break;
		default:
		    value->type = value_ptype;
		    break;
	    }
	    value->u.ival = symbol->offset;
	    value->type |= value_regno;
	    if (symbol->mem) {
		/* relocate symbol to stack */
		symbol->reg = 0;
		emit_store_symbol(symbol, value);
	    }
	    else
		inc_value_stack();
	}
    }
    emit_stat(expr->data._function.body);
    dec_branch_stack(1);
    vstack_reset(0);

    jump = bstack.fjump + bstack.offset;
    if (jump->offset) {
	label = ejit_label(state);
	do
	    ejit_patch(state, label, jump->table[--jump->offset]);
	while (jump->offset);
    }
    /* else need to know if flow reaches here when the function must
     * return a value, and trigger an error in that case if there is
     * no return statement */

    ejit_epilog(state);
    current = globals;

    return (void_tag);
}

static tag_t *
emit_return(expr_t *expr)
{
    ejit_node_t	*node;
    jump_t	*jump;
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*value;
    int		 regno;
    function_t	*function;
    int		 boffset = bstack.offset;
    int		 voffset = vstack.offset;

    function = (function_t *)current->name;
    ltag = function->tag->tag;
    if (expr->data._unary.expr) {
	rtag = emit_expr(expr->data._unary.expr);
	value = top_value_stack();
	emit_load(value);
	emit_coerce(expr, ltag, value);
	regno = value->u.ival;
	switch (ltag->type) {
	    case type_char:	ejit_retval_c(state, regno);	break;
	    case type_uchar:	ejit_retval_uc(state, regno);	break;
	    case type_short:	ejit_retval_s(state, regno);	break;
	    case type_ushort:	ejit_retval_us(state, regno);	break;
	    case type_int:	ejit_retval_i(state, regno);	break;
	    case type_uint:	ejit_retval_ui(state, regno);	break;
	    case type_long:	ejit_retval_l(state, regno);	break;
	    case type_ulong:	ejit_retval_ul(state, regno);	break;
	    case type_float:	ejit_retval_f(state, regno);	break;
	    case type_double:	ejit_retval_d(state, regno);	break;
	    default:
		if (!pointer_type_p(ltag->type))
		    error(expr, "aggregate by value return not supported");
		ejit_retval_p(state, regno);			break;
	}
    }
    else if (ltag->type != type_void)
	error(expr, "void return on non void function");

    vstack_reset(voffset);
    while (boffset > 0) {
	jump = bstack.fjump + --boffset;
	if (jump->token == tok_function) {
	    node = ejit_jmpi(state, NULL);
	    add_jump(jump, node);
	    return (void_tag);
	}
    }
    /* could not find jump to exit point */
    error(expr, "internal error");
}

static tag_t *
emit_call(expr_t *expr)
{
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    int		 rreg;
    int		 offset;

    ltag = emit_expr(expr->data._binary.lvalue);
    lval = top_value_stack();
    if (lval->type != value_funct)
	error(expr, "not a function");
    offset = vstack.offset;

    ejit_prepare(state);
    for (expr = expr->data._binary.rvalue; expr; expr = expr->next) {
	rtag = emit_expr(expr);
	rval = top_value_stack();
	emit_load(rval);
	rreg = rval->u.ival;
	switch (rtag->type) {
	    case type_char:	ejit_pusharg_c (state, rreg);	break;
	    case type_uchar:	ejit_pusharg_uc(state, rreg);	break;
	    case type_short:	ejit_pusharg_s (state, rreg);	break;
	    case type_ushort:	ejit_pusharg_us(state, rreg);	break;
	    case type_int:	ejit_pusharg_i (state, rreg);	break;
	    case type_uint:	ejit_pusharg_ui(state, rreg);	break;
	    case type_long:	ejit_pusharg_l (state, rreg);	break;
	    case type_ulong:	ejit_pusharg_ul(state, rreg);	break;
	    case type_float:	ejit_pusharg_f (state, rreg);	break;
	    case type_double:	ejit_pusharg_d (state, rreg);	break;
	    default:
		if (pointer_type_p(rtag->type))
		    error(expr, "aggregate argument by value not supported");
		ejit_pusharg_p(state, rreg);
		break;
	}
    }
    ejit_finish(state, lval->u.pval);

    ltag = ltag->tag;
    lval->u.ival = 0;
    switch (ltag->type) {
	case type_void:
	    break;
	case type_char:
	    lval->type = value_itype;
	    ejit_retval_c(state, 0);
	    break;
	case type_uchar:
	    lval->type = value_utype;
	    ejit_retval_uc(state, 0);
	    break;
	case type_short:
	    lval->type = value_itype;
	    ejit_retval_s(state, 0);
	    break;
	case type_ushort:
	    lval->type = value_utype;
	    ejit_retval_us(state, 0);
	    break;
	case type_int:
	    lval->type = value_itype;
	    ejit_retval_i(state, 0);
	    break;
	case type_uint:
	    lval->type = value_utype;
	    ejit_retval_ui(state, 0);
	    break;
	case type_long:
	    lval->type = value_ltype;
	    ejit_retval_l(state, 0);
	    break;
	case type_ulong:
	    lval->type = value_ltype;
	    ejit_retval_ul(state, 0);
	    break;
	case type_float:
	    lval->type = value_ftype;
	    ejit_retval_f(state, 0);
	    break;
	case type_double:
	    lval->type = value_dtype;
	    ejit_retval_d(state, 0);
	    break;
	default:
	    lval->type = value_ptype;
	    ejit_retval_p(state, 0);
	    break;
    }
    if (ltag->type != type_void)
	lval->type |= value_regno;
    vstack_reset(offset);

    return (ltag);
}

static void
emit_load(value_t *value)
{
    int		 regno;

    if (!(value->type & value_regno)) {
	switch (value->type) {
	    case value_funct:
		warn(NULL, "function value not handled");
		/* FIXME must be resolved if not jit */
		break;
	    case value_symbl:
		emit_load_symbol(value->u.pval, value);
		return;
	    case value_itype:
		regno = get_register(0);
		ejit_movi_i(state, regno, value->u.ival);
		value->u.ival = regno;
		break;
	    case value_ltype:
		regno = get_register(0);
		ejit_movi_l(state, regno, value->u.lval);
		value->u.ival = regno;
		break;
	    case value_ptype:
		regno = get_register(0);
		ejit_movi_p(state, regno, value->u.pval);
		value->u.ival = regno;
		break;
	    case value_ftype:
		regno = get_register(value_ftype);
		ejit_movi_f(state, regno, (float)value->u.dval);
		value->u.ival = regno;
		break;
	    case value_dtype:
		regno = get_register(value_dtype);
		ejit_movi_d(state, regno, value->u.dval);
		value->u.ival = regno;
		break;
	    default:
		error(NULL, "internal error");
	}
	value->u.ival = regno;
	value->type |= value_regno;
    }
    else if (value->type & value_spill) {
	regno = value->u.ival;
	if (value_float_p(value))
	    ejit_ldxi_d(state, value->disp, EJIT_FP, regno);
	else
	    ejit_ldxi_l(state, value->disp, EJIT_FP, regno);
	value->type &= ~value_spill;
    }
}

static void
emit_save(value_t *value)
{
    int		fent;
    int		regno;
    int		offset;

    if (!(value->type & value_spill)) {
	regno = value->u.ival;
	fent = value_float_p(value);
	if (value->type & value_displ)
	    offset = value->disp;
	else {
	    offset = ejit_allocai(state, fent ? sizeof(double) : sizeof(long));
	    value->disp = offset;
	    value->type |= value_displ;
	}
	value->type |= value_spill;
	if (fent)
	    ejit_stxi_d(state, value->disp, EJIT_FP, regno);
	else
	    ejit_stxi_l(state, value->disp, EJIT_FP, regno);
    }
}

static void
emit_load_symbol(symbol_t *symbol, value_t *value)
{
    int		 regno;
    void	*pointer;

    regno = get_register(symbol->tag->type == type_float ||
			 symbol->tag->type == type_double);
    if (symbol->reg) {
	switch (symbol->tag->type) {
	    case type_char:	case type_short:	case type_int:
		value->type = value_itype;
		ejit_movr_i(state, regno, symbol->offset);
		break;
	    case type_uchar:	case type_ushort:	case type_uint:
		value->type = value_utype;
		ejit_movr_ui(state, regno, symbol->offset);
		break;
	    case type_long:
		value->type = value_ltype;
		ejit_movr_l(state, regno, symbol->offset);
		break;
	    case type_ulong:
		value->type = value_ultype;
		ejit_movr_ul(state, regno, symbol->offset);
		break;
	    case type_float:
		value->type = value_ftype;
		if (!symbol->regptr->isflt)
		    ejit_movr_i_f(state, regno, symbol->offset);
		else
		    ejit_movr_f(state, regno, symbol->offset);
		break;
	    case type_double:
		value->type = value_dtype;
		if (!symbol->regptr->isflt)
		    ejit_movr_l_d(state, regno, symbol->offset);
		else
		    ejit_movr_d(state, regno, symbol->offset);
		break;
	    default:
		value->type = value_ptype;
		ejit_movr_p(state, regno, symbol->offset);
		break;
	}
    }
    else if (symbol->glb) {
	pointer = (char *)the_data + symbol->offset;
	switch (symbol->tag->type) {
	    case type_char:
		value->type = value_itype;
		ejit_ldi_c(state, regno, pointer);
		break;
	    case type_uchar:
		value->type = value_utype;
		ejit_ldi_uc(state, regno, pointer);
		break;
	    case type_short:
		value->type = value_itype;
		ejit_ldi_s(state, regno, pointer);
		break;
	    case type_ushort:
		value->type = value_utype;
		ejit_ldi_us(state, regno, pointer);
		break;
	    case type_int:
		value->type = value_itype;
		ejit_ldi_i(state, regno, pointer);
		break;
	    case type_uint:
		value->type = value_utype;
		ejit_ldi_ui(state, regno, pointer);
		break;
	    case type_long:
		value->type = value_ltype;
		ejit_ldi_l(state, regno, pointer);
		break;
	    case type_ulong:
		value->type = value_ultype;
		ejit_ldi_ul(state, regno, pointer);
		break;
	    case type_float:
		value->type = value_ftype;
		ejit_ldi_f(state, regno, pointer);
		break;
	    case type_double:
		value->type = value_dtype;
		ejit_ldi_d(state, regno, pointer);
		break;
	    default:
		value->type = value_ptype;
		ejit_movi_p(state, regno, pointer);
		break;
	}
    }
    else {
	switch (symbol->tag->type) {
	    case type_char:
		value->type = value_itype;
		ejit_ldxi_c(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_uchar:
		value->type = value_utype;
		ejit_ldxi_uc(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_short:
		value->type = value_itype;
		ejit_ldxi_s(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_ushort:
		value->type = value_utype;
		ejit_ldxi_us(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_int:
		value->type = value_itype;
		ejit_ldxi_i(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_uint:
		value->type = value_utype;
		ejit_ldxi_ui(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_long:
		value->type = value_ltype;
		ejit_ldxi_l(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_ulong:
		value->type = value_ultype;
		ejit_ldxi_ul(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_float:
		value->type = value_ftype;
		ejit_ldxi_f(state, regno, EJIT_FP, symbol->offset);
		break;
	    case type_double:
		value->type = value_dtype;
		ejit_ldxi_d(state, regno, EJIT_FP, symbol->offset);
		break;
	    default:
		value->type = value_ptype;
		ejit_ldxi_p(state, regno, EJIT_FP, symbol->offset);
		break;
	}
    }
    value->type |= value_regno;
    value->u.ival = regno;
}

static void
emit_store_symbol(symbol_t *symbol, value_t *value)
{
    int		 regno;
    void	*pointer;

    assert((value->type & (value_regno | value_spill)) == value_regno);
    regno = value->u.ival;
    if (symbol->reg) {
	switch (symbol->tag->type) {
	    case type_char:	case type_short:	case type_int:
		ejit_movr_i(state, symbol->offset, regno);
		break;
	    case type_uchar:	case type_ushort:	case type_uint:
		ejit_movr_ui(state, symbol->offset, regno);
		break;
	    case type_long:
		ejit_movr_l(state, symbol->offset, regno);
		break;
	    case type_ulong:
		ejit_movr_ul(state, symbol->offset, regno);
		break;
	    case type_float:
		if (!symbol->regptr->isflt)
		    ejit_movr_f_i(state, symbol->offset, regno);
		else
		    ejit_movr_f(state, symbol->offset, regno);
		break;
	    case type_double:
		if (!symbol->regptr->isflt)
		    ejit_movr_d_l(state, symbol->offset, regno);
		else
		    ejit_movr_d(state, symbol->offset, regno);
		break;
	    default:
		ejit_movr_p(state, symbol->offset, regno);
		break;
	}
    }
    else if (symbol->glb) {
	pointer = (char *)the_data + symbol->offset;
	switch (symbol->tag->type) {
	    case type_char:	ejit_sti_c(state, pointer, regno);
		break;
	    case type_uchar:	ejit_sti_uc(state, pointer, regno);
		break;
	    case type_short:	ejit_sti_s(state, pointer, regno);
		break;
	    case type_ushort:	ejit_sti_us(state, pointer, regno);
		break;
	    case type_int:	ejit_sti_i(state, pointer, regno);
		break;
	    case type_uint:	ejit_sti_ui(state, pointer, regno);
		break;
	    case type_long:	ejit_sti_l(state, pointer, regno);
		break;
	    case type_ulong:	ejit_sti_ul(state, pointer, regno);
		break;
	    case type_float:	ejit_sti_f(state, pointer, regno);
		break;
	    case type_double:	ejit_sti_d(state, pointer, regno);
		break;
	    default:
		if (pointer_type_p(symbol->tag->type))
		    ejit_sti_p(state, pointer, regno);
		else
		    warn(NULL, "struct copy not handled");
		break;
	}
    }
    else {
	switch (symbol->tag->type) {
	    case type_char:
		ejit_stxi_c(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_uchar:
		ejit_stxi_uc(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_short:
		ejit_stxi_s(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_ushort:
		ejit_stxi_us(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_int:
		ejit_stxi_i(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_uint:
		ejit_stxi_ui(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_long:
		ejit_stxi_l(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_ulong:
		ejit_stxi_ul(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_float:
		ejit_stxi_f(state, symbol->offset, EJIT_FP, regno);
		break;
	    case type_double:
		ejit_stxi_d(state, symbol->offset, EJIT_FP, regno);
		break;
	    default:
		if (pointer_type_p(symbol->tag->type))
		    ejit_stxi_p(state, symbol->offset, EJIT_FP, regno);
		else
		    warn(NULL, "struct copy not handled");
		break;
	}
    }
}

static value_t *
get_value_stack(void)
{
    if (vstack.offset >= vstack.length) {
	vstack.length += 16;
	vstack.values = (value_t *)
	    xrealloc(vstack.values, vstack.length * sizeof(value_t));
    }
    return (vstack.values + vstack.offset);
}

static void
inc_value_stack(void)
{
    ++vstack.offset;
    assert(vstack.offset <= vstack.length);
}

static void
dec_value_stack(int count)
{
    vstack.offset -= count;
    assert(count > 0 && vstack.offset >= 0);
}

static int
get_register(int freg)
{
    value_t	*entry;
    int		 ient, fent, count, regno, offset;

    fent = freg ? 1 : 0;
    count = fent ? EJIT_NUM_FPR_REGS : EJIT_NUM_GPR_REGS;
    for (regno = 0; regno < count; regno++) {
	entry = vstack.values;
	for (offset = 0; offset < vstack.offset; offset++, entry++) {
	    if ((entry->type & value_regno) && entry->u.ival == regno) {
		ient = (entry->type & (value_ftype | value_dtype)) == 0;
		/* try next one if register is live */
		if ((fent ^ ient) && !(entry->type & value_spill))
		    break;
	    }
	}
	/* register is free */
	if (offset == vstack.offset)
	    return (regno);
    }

    /* no register found; spill first match */
    offset = 0;
    entry = vstack.values;
    for (; offset < vstack.offset; offset++, entry++) {
	if (entry->type & value_regno) {
	    ient = (entry->type & (value_ftype | value_dtype)) == 0;
	    if (fent ^ ient) {
		regno = entry->u.ival;
		if (ient && ejit_gpr_regs[regno].isarg)
		    continue;
		if (fent && ejit_fpr_regs[regno].isarg)
		    continue;
		break;
	    }
	}
    }
    emit_save(entry);

    return (regno);
}

static void
inc_branch_stack(token_t token)
{
    int		offset;

    ++bstack.offset;
    if (bstack.offset >= bstack.length) {
	offset = bstack.length;
	bstack.length += 16;
	bstack.tjump = xrealloc(bstack.tjump, bstack.length * sizeof(jump_t));
	bstack.fjump = xrealloc(bstack.fjump, bstack.length * sizeof(jump_t));
	for (; offset < bstack.length; offset++) {
	    bstack.tjump[offset].offset = 0;
	    bstack.tjump[offset].length = 16;
	    bstack.tjump[offset].table = xmalloc(16 * sizeof(ejit_node_t *));
	    bstack.fjump[offset].offset = 0;
	    bstack.fjump[offset].length = 16;
	    bstack.fjump[offset].table = xmalloc(16 * sizeof(ejit_node_t *));
	}
    }
    bstack.tjump[bstack.offset - 1].token = token;
    bstack.fjump[bstack.offset - 1].token = token;
}

static void
dec_branch_stack(int count)
{
    bstack.offset -= count;
    assert(count > 0 && bstack.offset >= 0);
}

static void
add_jump(jump_t *list, ejit_node_t *node)
{
    if (list->offset >= list->length) {
	list->length += 16;
	list->table = xmalloc(list->length * sizeof(ejit_node_t *));
    }
    list->table[list->offset++] = node;
}
