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

/* avoid complication of needing to patch nodes based on stack direction
 * and padding of frame pointer */
#define STACK_DIRECTION		-1
#define ALLOCA_OFFSET		 0

/* FIXME this should be JIT_FP() and may need to be a variable if the
 * the frame pointer register may change based on runtime constraints */
#define FRAME_POINTER		 6

#define top_value_stack()	(vstack.values + vstack.offset - 1)

/*
 * Prototypes
 */
static tag_t *
emit_stat(expr_t *expr);

static tag_t *
emit_expr(expr_t *expr);

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
emit_coerce(expr_t *expr, tag_t *tag, value_t *value);

static tag_t *
emit_function(expr_t *expr);

static void
emit_load(expr_t *expr, value_t *value);

static void
emit_load_symbol(expr_t *expr, symbol_t *symbol, value_t *value);

static void
emit_store_symbol(expr_t *expr, symbol_t *symbol, value_t *value);

static value_t *
get_value_stack(void);

static void
inc_value_stack(void);

static void
dec_value_stack(int count);

static int
get_register(int freg);

/*
 * Initialization
 */
static ejit_state_t	*state;
static vstack_t		 vstack;
static int		 fcount;
static int		 icount;
static int		 alloca_offset;
static int		 alloca_length;
static ejit_node_t	*alloca_node;

/*
 * Implementation
 */
void
init_emit(void)
{
    state = ejit_create_state();
    vstack.length = 16;
    vstack.values = (value_t *)xmalloc(vstack.length * sizeof(value_t));
    fcount = icount = 6;
}

void
emit(expr_t *expr)
{
    (void)emit_stat(expr);
    ejit_print(state);
}

static tag_t *
emit_stat(expr_t *expr)
{
    tag_t	*tag = void_tag;
    int		 offset = vstack.offset;

    for (; expr; expr = expr->next)
	tag = emit_expr(expr);
    /* FIXME should only happen if non side effect expressions reach here */
    if (vstack.offset > offset)
	dec_value_stack(vstack.offset - offset);

    return (tag);
}

static tag_t *
emit_expr(expr_t *expr)
{
    value_t	*value;
    symbol_t	*symbol;
    function_t	*function;

    /* FIXME assume branch instructions will check arguments, to generate
     * proper conditional branches, but should work either way */
    switch (expr->token) {
	case tok_int:
	    value = get_value_stack();
	    value->disp = 0;
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
	    value->disp = 0;
	    value->u.dval = expr->data._unary.d;
	    inc_value_stack();
	    return (double_tag);
	case tok_symbol:
	    value = get_value_stack();
	    value->disp = 0;
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
#if 0
	    /* FIXME (may) need branch */
	case tok_andand:	case tok_oror:
#endif
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
	case tok_code:		case tok_stat:
	    return (emit_stat(expr->data._unary.expr));
	    break;
	case tok_function:
	    return (emit_function(expr));
	    break;
	default:
	    warn(expr, "not yet handled");
	    return (void_tag);
    }
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
	emit_load(lexp, lval);
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
					    (void *)tag->tag->size);
		else		ejit_subi_p(state, lreg, lreg,
					    (void *)tag->tag->size);
		break;
	    }
	case type_float:	case type_double:
	    error(expr, "not an integer or pointer");
    }
    if (lexp->token == tok_symbol)
	emit_store_symbol(expr, symbol, lval);
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
    emit_load(expr, lval);
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
		    ejit_addi_p(state, vr, lr, (void *)symbol->offset);
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
	emit_load(vexp, vval);
	if (tag != vtag)
	    emit_coerce(expr, tag, vval);
	vr = vval->u.ival;
	/* reload if spilled, otherwise a noop */
	emit_load(expr, lval);
    }

    else if (token) {
	if (symbol->offset)
	    ejit_addi_p(state, lr, lr, (void *)symbol->offset);
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
		ejit_addi_p(state, lr, lr, (void *)symbol->offset);
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
    emit_load(expr, lval);
    lr = lval->u.ival;
    if (value_load_p(rval))
	/* must be done before evaluating vexp (if set) because it may
	 * have side effects that could change the offset, e.g. change
	 * the value of a symbol */
	emit_load(expr, rval);
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
			ejit_addi_p(state, vr, lr, (void *)(rr * tag->size));
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
	if (token == tok_none)
	    emit_load(vexp, vval);
	if (tag != vtag)
	    emit_coerce(expr, tag, vval);
	vr = vval->u.ival;
	/* reload if spilled, otherwise a noop */
	emit_load(expr, lval);
	if (token && !value_const_p(rval))
	    emit_load(expr, rval);
    }

    else if (token) {
	switch (tag->type) {
	    case type_char:	case type_uchar:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)rr);
		else
		    ejit_addr_p(state, lr, lr, rr);
		break;
	    case type_short:	case type_ushort:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(rr * sizeof(short)));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(short));
		    ejit_addr_p(state, lr, lr, rr);
		}
		break;
	    case type_int:	case type_uint:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(rr * sizeof(int)));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(int));
		    ejit_addr_p(state, lr, lr, rr);
		}
		break;
	    case type_long:	case type_ulong:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(rr * sizeof(long)));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(long));
		    ejit_addr_p(state, lr, lr, rr);
		}
		break;
	    case type_float:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(rr * sizeof(float)));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(float));
		    ejit_addr_p(state, lr, lr, rr);
		}
		break;
	    case type_double:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(rr * sizeof(double)));
		else {
		    ejit_muli_i(state, rr, rr, sizeof(double));
		    ejit_addr_p(state, lr, lr, rr);
		}
		break;
	    default:
		if (value_const_p(rval))
		    ejit_addi_p(state, lr, lr, (void *)(rr * tag->size));
		else {
		    ejit_muli_i(state, rr, rr, tag->size);
		    ejit_addr_p(state, lr, lr, rr);
		}
		break;
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
		    ejit_addi_p(state, lr, lr, (void *)(rr * tag->size));
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
	    emit_load_symbol(expr, symbol, lval);
	rtag = emit_expr(rexp);
	rval = top_value_stack();
	if (token) {
	    rtag = emit_binary_next(expr, ltag, rtag, lval, rval, token);
	    if (ltag != rtag)
		emit_coerce(rexp, symbol->tag, lval);
	    emit_store_symbol(expr, symbol, lval);
	}
	else {
	    emit_load(rexp, rval);
	    if (ltag != rtag)
		emit_coerce(rexp, symbol->tag, rval);
	    emit_store_symbol(expr, symbol, rval);
	    lval->type = rval->type;
	    lval->u.ival = rval->u.ival;
	}
	dec_value_stack(1);
    }
    else
	emit_load_symbol(expr, symbol, lval);

    return (ltag);
}

static tag_t *
emit_not(expr_t *expr)
{
    int		 dec;
    tag_t	*tag;
    int		 ireg;
    value_t	*fval;
    value_t	*value;
    int		 regno;

    dec = 1;
    tag = emit_expr(expr->data._unary.expr);
    value = top_value_stack();
    emit_load(expr, value);
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
	    fval->disp = 0;
	    fval->u.fval = 0.0;
	    inc_value_stack();
	    emit_load(expr, fval);
	    ireg = get_register(0);
	    ejit_ltgtr_f(state, ireg, regno, fval->u.ival);
	    dec = 2;
	    value->type = value_regno;
	    value->u.ival = ireg;
	    break;
	case type_double:
	    fval = get_value_stack();
	    fval->type = value_dtype;
	    fval->disp = 0;
	    fval->u.dval = 0.0;
	    inc_value_stack();
	    emit_load(expr, fval);
	    ireg = get_register(0);
	    ejit_ltgtr_d(state, ireg, regno, fval->u.ival);
	    dec = 2;
	    value->type = value_regno;
	    value->u.ival = ireg;
	    break;
	default:
	    if (!pointer_type_p(tag->type))
		warn(expr, "value is always true");
	    ejit_nei_p(state, regno, regno, NULL);
	    break;
    }
    dec_value_stack(dec);

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
    emit_load(expr, value);
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
    dec_value_stack(1);

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
    emit_load(expr, value);
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
    dec_value_stack(1);

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
		ejit_addi_p(state, lval->u.ival, FRAME_POINTER,
			    (void *)symbol->offset);
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
    emit_load(expr, lval);
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
	    emit_load(expr, rval);
	if (ltag != rtag)
	    emit_coerce(expr, ltag, rval);
	rreg = rval->u.ival;
	/* reload if spilled, otherwise a noop */
	emit_load(expr, lval);
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
    emit_load(expr, lval);
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
    emit_load(expr, lval);

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
			emit_load(expr, rval);
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
	emit_load(expr, rval);
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
			emit_load(expr, rval);
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
			emit_load(expr, rval);
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
			emit_load(expr, rval);
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
			emit_load(expr, rval);
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
			emit_load(expr, rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_ftype);
		    if (rreg == -1) {
			emit_load(expr, rval);
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
			emit_load(expr, rval);
			rreg = rval->u.ival;
		    }
		    freg = get_register(value_ftype);
		    if (rreg == -1) {
			emit_load(expr, rval);
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

static tag_t *
emit_function(expr_t *expr)
{
    expr_t	*list;
    int		 offset;
    symbol_t	*symbol;
    int		 num_int, num_float, num_double;

    current = expr->data._function.function->table;
    alloca_offset = alloca_length = current->length;
    offset = num_int = num_float = num_double = 0;
    for (; offset < current->count; offset++) {
	symbol = current->vector[offset];
	if (!symbol->arg)
	    break;
	switch (symbol->tag->type) {
	    case type_float:
		++num_float;
		break;
	    case type_double:
		++num_double;
		break;
	    default:
		++num_int;
		break;
	}
    }
    ejit_prolog(state, num_int);
    if (num_float)
	ejit_prolog_f(state, num_float);
    if (num_double)
	ejit_prolog_d(state, num_double);

    list = expr->data._function.call->data._binary.rvalue;
    for (offset = 0; offset < current->count; offset++) {
	symbol = current->vector[offset];
	if (!symbol->arg)
	    break;
	switch (symbol->tag->type) {
	    case type_char:	symbol->jit = ejit_arg_c(state);	break;
	    case type_uchar:	symbol->jit = ejit_arg_uc(state);	break;
	    case type_short:	symbol->jit = ejit_arg_s(state);	break;
	    case type_ushort:	symbol->jit = ejit_arg_us(state);	break;
	    case type_int:	symbol->jit = ejit_arg_i(state);	break;
	    case type_uint:	symbol->jit = ejit_arg_ui(state);	break;
	    case type_long:	symbol->jit = ejit_arg_l(state);	break;
	    case type_ulong:	symbol->jit = ejit_arg_ul(state);	break;
	    case type_float:	symbol->jit = ejit_arg_f(state);	break;
	    case type_double:	symbol->jit = ejit_arg_d(state);	break;
	    default:		symbol->jit = ejit_arg_p(state);	break;
	}
	list = list->next;
    }
    alloca_node = ejit_allocai(state, current->length);
    if (offset < current->count) {
#if STACK_DIRECTION < 0
	symbol->offset = (symbol->offset + ALLOCA_OFFSET) - current->length;
#elif ALLOCA_OFFSET
	symbol->offset += ALLOCA_OFFSET;
#endif
	symbol->jit = alloca_node;
	for (++offset; offset < current->count; offset++) {
#if STACK_DIRECTION < 0
	symbol->offset = (symbol->offset + ALLOCA_OFFSET) - current->length;
#elif ALLOCA_OFFSET
	symbol->offset += ALLOCA_OFFSET;
#endif
	    symbol = current->vector[offset];
	    symbol->jit = alloca_node;
	}
    }
    emit_stat(expr->data._function.body);
    ejit_ret(state);
    current = globals;

    return (void_tag);
}

static void
emit_load(expr_t *expr, value_t *value)
{
    int		 regno;

    if (!(value->type & value_regno)) {
	switch (value->type) {
	    case value_funct:
		warn(expr, "function value not handled");
		/* FIXME must be resolved if not jit */
		break;
	    case value_symbl:
		emit_load_symbol(expr, value->u.pval, value);
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
#if 0		/* no information so far about it */
	    case value_ftype:
		regno = get_register(value_ftype);
		ejit_movi_f(state, regno, (float)value->u.dval);
		value->u.ival = regno;
		break;
#endif
	    case value_dtype:
		regno = get_register(value_dtype);
		ejit_movi_d(state, regno, value->u.dval);
		value->u.ival = regno;
		break;
	    default:
		error(expr, "internal error");
	}
	value->u.ival = regno;
	value->type |= value_regno;
    }
    else if (value->type & value_spill) {
	regno = value->u.ival;
	if (value_float_p(value))
	    ejit_ldxi_d(state, value->disp, FRAME_POINTER, regno);
	else
	    ejit_ldxi_l(state, value->disp, FRAME_POINTER, regno);
	value->type &= ~value_spill;
    }
}

static void
emit_load_symbol(expr_t *expr, symbol_t *symbol, value_t *value)
{
    int		 regno;
    void	*pointer;

    regno = get_register(symbol->tag->type == type_float ||
			 symbol->tag->type == type_double);
    if (symbol->arg) {
	switch (symbol->tag->type) {
	    case type_char:
		value->type = value_itype;
		ejit_getarg_c(state, regno, symbol->jit);
		break;
	    case type_uchar:
		value->type = value_utype;
		ejit_getarg_uc(state, regno, symbol->jit);
		break;
	    case type_short:
		value->type = value_itype;
		ejit_getarg_s(state, regno, symbol->jit);
		break;
	    case type_ushort:
		value->type = value_utype;
		ejit_getarg_us(state, regno, symbol->jit);
		break;
	    case type_int:
		value->type = value_itype;
		ejit_getarg_i(state, regno, symbol->jit);
		break;
	    case type_uint:
		value->type = value_utype;
		ejit_getarg_ui(state, regno, symbol->jit);
		break;
	    case type_long:
		value->type = value_ltype;
		ejit_getarg_l(state, regno, symbol->jit);
		break;
	    case type_ulong:
		value->type = value_ultype;
		ejit_getarg_ul(state, regno, symbol->jit);
		break;
	    case type_float:
		value->type = value_ftype;
		ejit_getarg_f(state, regno, symbol->jit);
		break;
	    case type_double:
		value->type = value_dtype;
		ejit_getarg_d(state, regno, symbol->jit);
		break;
	    default:
		value->type = value_ptype;
		ejit_getarg_p(state, regno, symbol->jit);
		break;
	}
    }
    else if (symbol->loc) {
	switch (symbol->tag->type) {
	    case type_char:
		value->type = value_itype;
		ejit_ldxi_c(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_uchar:
		value->type = value_utype;
		ejit_ldxi_uc(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_short:
		value->type = value_itype;
		ejit_ldxi_s(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_ushort:
		value->type = value_utype;
		ejit_ldxi_us(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_int:
		value->type = value_itype;
		ejit_ldxi_i(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_uint:
		value->type = value_utype;
		ejit_ldxi_ui(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_long:
		value->type = value_ltype;
		ejit_ldxi_l(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_ulong:
		value->type = value_ultype;
		ejit_ldxi_ul(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_float:
		value->type = value_ftype;
		ejit_ldxi_f(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    case type_double:
		value->type = value_dtype;
		ejit_ldxi_d(state, regno, FRAME_POINTER, symbol->offset);
		break;
	    default:
		value->type = value_ptype;
		ejit_ldxi_p(state, regno, FRAME_POINTER, symbol->offset);
		break;
	}
    }
    else {
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
    value->type |= value_regno;
    value->u.ival = regno;
}

static void
emit_store_symbol(expr_t *expr, symbol_t *symbol, value_t *value)
{
    int		 regno;
    void	*pointer;

    assert((value->type & (value_regno | value_spill)) == value_regno);
    regno = value->u.ival;
    if (symbol->arg) {
	switch (symbol->tag->type) {
	    case type_char:	ejit_putarg_c(state, symbol->jit, regno);
		break;
	    case type_uchar:	ejit_putarg_uc(state, symbol->jit, regno);
		break;
	    case type_short:	ejit_putarg_s(state, symbol->jit, regno);
		break;
	    case type_ushort:	ejit_putarg_us(state, symbol->jit, regno);
		break;
	    case type_int:	ejit_putarg_i(state, symbol->jit, regno);
		break;
	    case type_uint:	ejit_putarg_ui(state, symbol->jit, regno);
		break;
	    case type_long:	ejit_putarg_l(state, symbol->jit, regno);
		break;
	    case type_ulong:	ejit_putarg_ul(state, symbol->jit, regno);
		break;
	    case type_float:	ejit_putarg_f(state, symbol->jit, regno);
		break;
	    case type_double:	ejit_putarg_d(state, symbol->jit, regno);
		break;
	    default:
		/* struct argument by value not supported */
		assert(pointer_type_p(symbol->tag->type));
		ejit_putarg_p(state, symbol->jit, regno);
		break;
	}
    }
    else if (symbol->loc) {
	switch (symbol->tag->type) {
	    case type_char:
		ejit_stxi_c(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_uchar:
		ejit_stxi_uc(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_short:
		ejit_stxi_s(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_ushort:
		ejit_stxi_us(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_int:
		ejit_stxi_i(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_uint:
		ejit_stxi_ui(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_long:
		ejit_stxi_l(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_ulong:
		ejit_stxi_ul(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_float:
		ejit_stxi_f(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    case type_double:
		ejit_stxi_d(state, symbol->offset, FRAME_POINTER, regno);
		break;
	    default:
		if (pointer_type_p(symbol->tag->type))
		    ejit_stxi_p(state, symbol->offset, FRAME_POINTER, regno);
		else
		    warn(expr, "struct copy not handled");
		break;
	}
    }
    else {
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
		    warn(expr, "struct copy not handled");
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
    value_t	*value;

    /* FIXME if releasing registers, may need to update alloca_offset */

    value = vstack.values + vstack.offset;
    vstack.offset -= count;
    assert(count > 0 && vstack.offset >= 0);
}

static int
get_register(int freg)
{
    value_t	*entry;
    int		 ient, fent, count, regno, size, offset;

    fent = freg ? 1 : 0;
    count = fent ? fcount : icount;
    for (regno = 0; regno < count; regno++) {
	offset = 0;
	entry = vstack.values;
	for (; offset < vstack.offset; offset++, entry++) {
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
	    if (fent ^ ient)
		break;
	}
    }
    regno = offset;

    size = fent ? sizeof(double) : sizeof(long);
    alloca_offset = (alloca_offset + size - 1) & -size;
    alloca_offset += size;
    if (alloca_offset > alloca_length) {
	alloca_length = alloca_offset;
	alloca_node->u.i = alloca_length;
    }
#if STACK_DIRECTION < 0
    entry->disp = (alloca_offset + ALLOCA_OFFSET) - alloca_length;
#elif ALLOCA_OFFSET
    entry->disp += ALLOCA_OFFSET;
#endif
    entry->type |= value_spill;
    if (fent)
	ejit_stxi_d(state, entry->disp, FRAME_POINTER, regno);
    else
	ejit_stxi_l(state, entry->disp, FRAME_POINTER, regno);

    return (regno);
}
