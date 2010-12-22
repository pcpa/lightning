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
emit_pointer(expr_t *expr);

static tag_t *
emit_field(expr_t *expr);

static tag_t *
emit_cmp(expr_t *expr);

static tag_t *
emit_binint(expr_t *expr, token_t token);

static tag_t *
emit_binary(expr_t *expr, token_t token);

static tag_t *
emit_binary_setup(expr_t *expr, token_t token, tag_t *ltag, tag_t *rtag,
		  value_t *lval, value_t *rval);

static void
emit_coerce(expr_t *expr, tag_t *tag, value_t *value);

static tag_t *
emit_function(expr_t *expr);

static void
emit_load(value_t *value);

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
	    value->type = value_ltype;
	    value->disp = 0;
	    value->u.lval = expr->data._unary.i;
	    inc_value_stack();
	    return (long_tag);
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
	    return (emit_field(expr));
	case tok_not:
	    return (emit_not(expr));
	case tok_neg:
	    return (emit_neg(expr));
	case tok_com:
	    return (emit_com(expr));
	case tok_address:
	    return (emit_address(expr));
	case tok_pointer:
	    return (emit_pointer(expr));
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
	    return (emit_binint(expr, expr->token));
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

/* FIXME like emmit_load() for now only barely generating code for
 * symbols (not vectors or structs/unions, neither casts still
 * implemented, etc) */
static tag_t *
emit_set(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*decl;
    value_t	*value;
    symbol_t	*symbol;

    decl = expr->data._binary.rvalue;
    expr = expr->data._binary.lvalue;
    if (expr->token != tok_symbol) {
	warn(expr, "store of aggregate not handled");
	return (void_tag);
    }
    symbol = get_symbol(expr->data._unary.cp);
    if (symbol == NULL) {
	if (get_hash(functions, expr->data._unary.cp))
	    error(expr, "not a lvalue");
	error(expr, "undefined symbol '%s'", expr->data._unary.cp);
    }
    tag = emit_expr(decl);
    value = top_value_stack();
    emit_load(value);
    /* FIXME should not change "implicit value" if need to coerce? */
    if (symbol->tag != tag)
	emit_coerce(expr, symbol->tag, value);
    emit_store_symbol(expr, symbol, value);

    return (symbol->tag);
}

static tag_t *
emit_setop(expr_t *expr)
{
    tag_t	*tag;
    char	*name;
    value_t	*value;
    token_t	 token;
    symbol_t	*symbol;

    if (expr->data._binary.lvalue->token != tok_symbol) {
	warn(expr, "store of aggregate not handled");
	return (void_tag);
    }
    name = expr->data._binary.lvalue->data._unary.cp;
    symbol = get_symbol(name);
    if (symbol == NULL) {
	if (get_hash(functions, name))
	    error(expr, "not a lvalue");
	error(expr, "undefined symbol '%s'", name);
    }
    switch (expr->token) {
	case tok_andset:	token = tok_and;	break;
	case tok_orset:		token = tok_or;		break;
	case tok_xorset:	token = tok_xor;	break;
	case tok_lshset:	token = tok_lsh;	break;
	case tok_rshset:	token = tok_rsh;	break;
	case tok_addset:	token = tok_add;	break;
	case tok_subset:	token = tok_sub;	break;
	case tok_mulset:	token = tok_mul;	break;
	case tok_divset:	token = tok_div;	break;
	default:		token = tok_rem;	break;
    }
    switch (token) {
	case tok_and:		case tok_or:		case tok_xor:
	case tok_lsh:		case tok_rsh:		case tok_rem:
	    tag = emit_binint(expr, token);
	    break;
	default:
	    tag = emit_binary(expr, token);
	    break;
    }
    value = top_value_stack();
    if (symbol->tag != tag)
	emit_coerce(expr, symbol->tag, value);
    emit_store_symbol(expr, symbol, value);

    return (symbol->tag);
}

static tag_t *
emit_incdec(expr_t *expr)
{
    tag_t	*tag;
    int		 inc;
    int		 post;
    int		 lreg;
    int		 rreg;
    value_t	*value;
    symbol_t	*symbol;

    tag = emit_expr(expr->data._unary.expr);
    value = top_value_stack();
    if (value->type != value_symbl) {
	warn(expr, "store of aggregate not handled");
	return (void_tag);
    }
    symbol = value->u.pval;
    emit_load(value);
    lreg = value->u.ival;
    inc = expr->token == tok_inc || expr->token == tok_postinc;
    if (expr->token == tok_postinc || expr->token == tok_postdec) {
	post = 1;
	rreg = get_register(0);
    }
    else {
	post = 0;
	rreg = lreg;
    }
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
	    if ((tag->type & type_pointer) && tag->tag->size) {
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
    emit_store_symbol(expr, symbol, value);
    /* if post, change result to saved one */
    /* note that usage of get_register() without an associated
     * value_t on stack relies on having at least 3 register
     * of the given class, and not allocating any other while
     * it is "unbound" */
    value->u.ival = rreg;

    return (tag);
}

static tag_t *
emit_field(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*field;
    value_t	*value;
    int		 regno;
    record_t	*record;
    symbol_t	*symbol;

    field = expr->data._binary.rvalue;
    if (field->token != tok_symbol)
	error(expr, "syntax error");
    tag = emit_expr(expr->data._binary.lvalue);
    if (!(tag->type & (type_struct | type_union)))
	error(expr, "not a record");
    value = top_value_stack();
    emit_load(value);
    regno = value->u.ival;
    if (tag->type & type_pointer) {
	if (expr->token != tok_arrow)
	    error(expr, "value is a pointer");
	tag = tag->tag;
	/* FIXME parser may let it? */
	if (tag->type & type_pointer)
	    error(expr, "not a record");
	ejit_ldr_p(state, regno, regno);
    }
    else if (expr->token != tok_dot)
	error(expr, "not a pointer");
    /* type punned toplevel tag */
    record = (record_t *)tag->name;
    symbol = (symbol_t *)get_hash((hash_t *)record, field->data._unary.cp);
    if (symbol == NULL)
	error(expr, "no '%s' field in '%s'", field->data._unary.cp,
	      record->name ? record->name->name.string : "<anonymous>");
    tag = symbol->tag;
    switch (tag->type) {
	case type_char:
	    value->type = 0;
	    ejit_ldxi_c(state, regno, regno, symbol->offset);
	    break;
	case type_uchar:
	    value->type = value_utype;
	    ejit_ldxi_uc(state, regno, regno, symbol->offset);
	    break;
	case type_short:
	    value->type = 0;
	    ejit_ldxi_s(state, regno, regno, symbol->offset);
	    break;
	case type_ushort:
	    value->type = value_utype;
	    ejit_ldxi_us(state, regno, regno, symbol->offset);
	    break;
	case type_int:
	    value->type = 0;
	    ejit_ldxi_i(state, regno, regno, symbol->offset);
	    break;
	case type_uint:
	    value->type = value_utype;
	    ejit_ldxi_ui(state, regno, regno, symbol->offset);
	    break;
	case type_long:
	    value->type = value_ltype;
	    ejit_ldxi_l(state, regno, regno, symbol->offset);
	    break;
	case type_ulong:
	    value->type = value_utype | value_ltype;
	    ejit_ldxi_ul(state, regno, regno, symbol->offset);
	    break;
	case type_float:
	    regno = get_register(value_ftype);
	    value->type = value_ftype;
	    ejit_ldxi_f(state, regno, value->u.ival, symbol->offset);
	    value->u.ival = regno;
	    break;
	case type_double:
	    regno = get_register(value_dtype);
	    value->type = value_dtype;
	    ejit_ldxi_d(state, regno, value->u.ival, symbol->offset);
	    value->u.ival = regno;
	    break;
	default:
	    value->type = value_ptype;
	    ejit_addi_p(state, regno, regno, (void *)symbol->offset);
	    break;
    }
    value->type |= value_regno;

    return (tag);
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
	    fval->disp = 0;
	    fval->u.fval = 0.0;
	    inc_value_stack();
	    emit_load(fval);
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
	    emit_load(fval);
	    ireg = get_register(0);
	    ejit_ltgtr_d(state, ireg, regno, fval->u.ival);
	    dec = 2;
	    value->type = value_regno;
	    value->u.ival = ireg;
	    break;
	default:
	    if (!(tag->type & type_pointer))
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
    dec_value_stack(1);

    return (tag);
}

static tag_t *
emit_address(expr_t *expr)
{
    tag_t	*tag;
    expr_t	*addr;
    value_t	*value;
    symbol_t	*symbol;
    function_t	*function;

    if ((addr = expr->data._unary.expr) == NULL)
	error(expr, "not an lvalue");
    value = get_value_stack();
    value->type = value_ptype | value_regno;
    value->disp = 0;
    value->u.ival = get_register(0);
    inc_value_stack();
    switch (addr->token) {
	case tok_symbol:
	    symbol = get_symbol(expr->data._unary.cp);
	    if (symbol) {
		if (symbol->arg)
		    /* FIXME because can be in a register... */
		    error(expr, "address of argument not supported");
		else if (symbol->loc)
		    ejit_addi_p(state, value->u.ival, FRAME_POINTER,
				(void *)symbol->offset);
		else
		    ejit_movi_p(state, value->u.ival,
				(char *)the_data + symbol->offset);
		tag = symbol->tag;
	    }
	    else {
		function = (function_t *)
		    get_hash(functions, expr->data._unary.cp);
		if (function) {
		    warn(expr, "function address not handled");
		    tag = function->tag;
		}
		else
		    error(expr, "undefined symbol '%s'", expr->data._unary.cp);
	    }
	    break;
	case tok_dot:		case tok_arrow:
	case tok_vector:
	    warn(expr, "aggregate address not handled");
	    tag = void_tag;
	    break;
	default:
	    error(expr, "not an lvalue");
    }

    return (tag_pointer(tag));
}

static tag_t *
emit_pointer(expr_t *expr)
{
    tag_t	*tag;
    int		 freg;
    int		 regno;
    value_t	*value;

    tag = emit_expr(expr->data._unary.expr);
    if (!(tag->type & type_pointer))
	error(expr, "not a pointer");
    value = top_value_stack();
    emit_load(value);
    regno = value->u.ival;
    tag = tag->tag;

    switch (tag->type) {
	case type_char:
	    value->type = value_regno;
	    ejit_ldr_c(state, regno, regno);
	    break;
	case type_uchar:
	    value->type = value_utype | value_regno;
	    ejit_ldr_c(state, regno, regno);
	    break;
	case type_short:
	    value->type = value_regno;
	    ejit_ldr_s(state, regno, regno);
	    break;
	case type_ushort:
	    value->type = value_utype | value_regno;
	    ejit_ldr_us(state, regno, regno);
	    break;
	case type_int:
	    value->type = value_regno;
	    ejit_ldr_i(state, regno, regno);
	    break;
	case type_uint:
	    value->type = value_utype | value_regno;
	    ejit_ldr_ui(state, regno, regno);
	    break;
	case type_long:
	    value->type = value_ltype | value_regno;
	    ejit_ldr_l(state, regno, regno);
	    break;
	case type_ulong:
	    value->type = value_utype | value_ltype | value_regno;
	    ejit_ldr_ul(state, regno, regno);
	    break;
	case type_float:
	    freg = get_register(value_ftype);
	    value->type = value_ftype | value_regno;
	    ejit_ldr_f(state, freg, regno);
	    value->u.ival = freg;
	    break;
	case type_double:
	    freg = get_register(value_dtype);
	    value->type = value_dtype | value_regno;
	    ejit_ldr_d(state, freg, regno);
	    value->u.ival = freg;
	    break;
	default:
	    value->type = value_ptype | value_regno;
	    ejit_ldr_p(state, regno, regno);
	    break;
    }

    return (tag);
}

static tag_t *
emit_cmp(expr_t *expr)
{
    long	 li;
    int		 dec;
    tag_t	*tag;
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    int		 lreg;
    int		 rreg;
    int		 lfreg;
    int		 rfreg;

    dec = 1;
    ltag = emit_expr(expr->data._binary.lvalue);
    lval = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rval = top_value_stack();
    emit_load(lval);
    lreg = lval->u.ival;
    if (rval->type != value_ltype) {
	emit_load(rval);
	rreg = rval->u.ival;
    }
    else
	rreg = -1;
    tag = emit_binary_setup(expr, expr->token, ltag, rtag, lval, rval);
    switch (tag->type) {
	case type_int:
	    switch (expr->token) {
		case tok_lt:	ejit_ltr_i(state, lreg, lreg, rreg);	break;
		case tok_le:	ejit_ler_i(state, lreg, lreg, rreg);	break;
		case tok_eq:	ejit_eqr_i(state, lreg, lreg, rreg);	break;
		case tok_ge:	ejit_ger_i(state, lreg, lreg, rreg);	break;
		case tok_gt:	ejit_gtr_i(state, lreg, lreg, rreg);	break;
		default:	ejit_ner_i(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_uint:
	    switch (expr->token) {
		case tok_lt:	ejit_ltr_ui(state, lreg, lreg, rreg);	break;
		case tok_le:	ejit_ler_ui(state, lreg, lreg, rreg);	break;
		case tok_eq:	ejit_eqr_ui(state, lreg, lreg, rreg);	break;
		case tok_ge:	ejit_ger_ui(state, lreg, lreg, rreg);	break;
		case tok_gt:	ejit_gtr_ui(state, lreg, lreg, rreg);	break;
		default:	ejit_ner_ui(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_long:
	    if (rval->type == value_ltype) {
		dec = 0;
		li = rval->u.lval;
		switch (expr->token) {
		    case tok_lt:ejit_lti_l(state, lreg, lreg, li);	break;
		    case tok_le:ejit_lei_l(state, lreg, lreg, li);	break;
		    case tok_eq:ejit_eqi_l(state, lreg, lreg, li);	break;
		    case tok_ge:ejit_gei_l(state, lreg, lreg, li);	break;
		    case tok_gt:ejit_gti_l(state, lreg, lreg, li);	break;
		    default:	ejit_nei_l(state, lreg, lreg, li);	break;
		}
	    }
	    else {
		switch (expr->token) {
		    case tok_lt:ejit_ltr_l(state, lreg, lreg, rreg);	break;
		    case tok_le:ejit_ler_l(state, lreg, lreg, rreg);	break;
		    case tok_eq:ejit_eqr_l(state, lreg, lreg, rreg);	break;
		    case tok_ge:ejit_ger_l(state, lreg, lreg, rreg);	break;
		    case tok_gt:ejit_gtr_l(state, lreg, lreg, rreg);	break;
		    default:	ejit_ner_l(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_ulong:
	    switch (expr->token) {
		case tok_lt:	ejit_ltr_ul(state, lreg, lreg, rreg);	break;
		case tok_le:	ejit_ler_ul(state, lreg, lreg, rreg);	break;
		case tok_eq:	ejit_eqr_ul(state, lreg, lreg, rreg);	break;
		case tok_ge:	ejit_ger_ul(state, lreg, lreg, rreg);	break;
		case tok_gt:	ejit_gtr_ul(state, lreg, lreg, rreg);	break;
		default:	ejit_ner_ul(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_float:
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
	    switch (expr->token) {
		case tok_lt:	ejit_ltr_p(state, lreg, lreg, rreg);	break;
		case tok_le:	ejit_ler_p(state, lreg, lreg, rreg);	break;
		case tok_eq:	ejit_eqr_p(state, lreg, lreg, rreg);	break;
		case tok_ge:	ejit_ger_p(state, lreg, lreg, rreg);	break;
		case tok_gt:	ejit_gtr_p(state, lreg, lreg, rreg);	break;
		default:	ejit_ner_p(state, lreg, lreg, rreg);	break;
	    }
	    break;
    }
    if (dec)
	dec_value_stack(1);
    lval->type = value_regno;

    return (int_tag);
}

static tag_t *
emit_binint(expr_t *expr, token_t token)
{
    long	 li;
    int		 dec;
    tag_t	*tag;
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    int		 lreg;
    int		 rreg;

    dec = 1;
    ltag = emit_expr(expr->data._binary.lvalue);
    lval = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rval = top_value_stack();
    emit_load(lval);
    if (rval->type != value_ltype)
	emit_load(rval);
    tag = emit_binary_setup(expr, token, ltag, rtag, lval, rval);
    lreg = lval->u.ival;
    rreg = rval->type != value_ltype ? rval->u.ival : -1;
    switch (tag->type) {
	case type_int:
	    switch (token) {
		case tok_and:	ejit_andr_i(state, lreg, lreg, rreg);	break;
		case tok_or:	ejit_orr_i(state, lreg, lreg, rreg);	break;
		case tok_xor:	ejit_xorr_i(state, lreg, lreg, rreg);	break;
		case tok_lsh:	ejit_lshr_i(state, lreg, lreg, rreg);	break;
		case tok_rsh:	ejit_rshr_i(state, lreg, lreg, rreg);	break;
		default:	ejit_modr_i(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_uint:
	    switch (token) {
		case tok_and:	ejit_andr_ui(state, lreg, lreg, rreg);	break;
		case tok_or:	ejit_orr_ui(state, lreg, lreg, rreg);	break;
		case tok_xor:	ejit_xorr_ui(state, lreg, lreg, rreg);	break;
		case tok_lsh:	ejit_lshr_ui(state, lreg, lreg, rreg);	break;
		case tok_rsh:	ejit_rshr_ui(state, lreg, lreg, rreg);	break;
		default:	ejit_modr_ui(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_long:
	    if (rval->type == value_ltype) {
		dec = 0;
		li = rval->u.lval;
		switch (token) {
		    case tok_and:ejit_andi_l(state, lreg, lreg, li);	break;
		    case tok_or: ejit_ori_l(state, lreg, lreg, li);	break;
		    case tok_xor:ejit_xori_l(state, lreg, lreg, li);	break;
		    case tok_lsh:ejit_lshi_l(state, lreg, lreg, li);	break;
		    case tok_rsh:ejit_rshi_l(state, lreg, lreg, li);	break;
		    default:	 ejit_modi_l(state, lreg, lreg, li);	break;
		}
	    }
	    else {
		switch (token) {
		    case tok_and:ejit_andr_l(state, lreg, lreg, rreg);	break;
		    case tok_or: ejit_orr_l(state, lreg, lreg, rreg);	break;
		    case tok_xor:ejit_xorr_l(state, lreg, lreg, rreg);	break;
		    case tok_lsh:ejit_lshr_l(state, lreg, lreg, rreg);	break;
		    case tok_rsh:ejit_rshr_l(state, lreg, lreg, rreg);	break;
		    default:	 ejit_modr_l(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	default:
	    switch (token) {
		case tok_and:	ejit_andr_ul(state, lreg, lreg, rreg);	break;
		case tok_or:	ejit_orr_ul(state, lreg, lreg, rreg);	break;
		case tok_xor:	ejit_xorr_ul(state, lreg, lreg, rreg);	break;
		case tok_lsh:	ejit_lshr_ul(state, lreg, lreg, rreg);	break;
		case tok_rsh:	ejit_rshr_ul(state, lreg, lreg, rreg);	break;
		default:	ejit_modr_ul(state, lreg, lreg, rreg);	break;
	    }
	    break;
    }
    if (dec)
	dec_value_stack(1);

    return (tag);
}

static tag_t *
emit_binary(expr_t *expr, token_t token)
{
    long	 li;
    int		 dec;
    tag_t	*tag;
    tag_t	*ltag;
    tag_t	*rtag;
    value_t	*lval;
    value_t	*rval;
    int		 lreg;
    int		 rreg;

    dec = 1;
    ltag = emit_expr(expr->data._binary.lvalue);
    lval = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rval = top_value_stack();
    emit_load(lval);
    if (rval->type != value_ltype)
	emit_load(rval);
    tag = emit_binary_setup(expr, token, ltag, rtag, lval, rval);
    lreg = lval->u.ival;
    rreg = rval->type != value_ltype ? rval->u.ival : -1;
    switch (tag->type) {
	case type_int:
	    switch (token) {
		case tok_add:	ejit_addr_i(state, lreg, lreg, rreg);	break;
		case tok_sub:	ejit_subr_i(state, lreg, lreg, rreg);	break;
		case tok_mul:	ejit_mulr_i(state, lreg, lreg, rreg);	break;
		default:	ejit_divr_i(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_uint:
	    switch (token) {
		case tok_add:	ejit_addr_ui(state, lreg, lreg, rreg);	break;
		case tok_sub:	ejit_subr_ui(state, lreg, lreg, rreg);	break;
		case tok_mul:	ejit_mulr_ui(state, lreg, lreg, rreg);	break;
		default:	ejit_divr_ui(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_long:
	    if (rval->type == value_ltype) {
		dec = 0;
		li = rval->u.lval;
		switch (token) {
		    case tok_add:ejit_addi_l(state, lreg, lreg, li);	break;
		    case tok_sub:ejit_subi_l(state, lreg, lreg, li);	break;
		    case tok_mul:ejit_muli_l(state, lreg, lreg, li);	break;
		    default:	 ejit_divi_l(state, lreg, lreg, li);	break;
		}
	    }
	    else {
		switch (token) {
		    case tok_add:ejit_addr_l(state, lreg, lreg, rreg);	break;
		    case tok_sub:ejit_subr_l(state, lreg, lreg, rreg);	break;
		    case tok_mul:ejit_mulr_l(state, lreg, lreg, rreg);	break;
		    default:	 ejit_divr_l(state, lreg, lreg, rreg);	break;
		}
	    }
	    break;
	case type_ulong:
	    switch (token) {
		case tok_add:	ejit_addr_ul(state, lreg, lreg, rreg);	break;
		case tok_sub:	ejit_subr_ul(state, lreg, lreg, rreg);	break;
		case tok_mul:	ejit_mulr_ul(state, lreg, lreg, rreg);	break;
		default:	ejit_divr_ul(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_float:
	    switch (token) {
		case tok_add:	ejit_addr_f(state, lreg, lreg, rreg);	break;
		case tok_sub:	ejit_subr_f(state, lreg, lreg, rreg);	break;
		case tok_mul:	ejit_mulr_f(state, lreg, lreg, rreg);	break;
		default:	ejit_divr_f(state, lreg, lreg, rreg);	break;
	    }
	    break;
	case type_double:
	    switch (token) {
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
		if (tag->size != 1) {
		    if (ltag->type & type_pointer)
			/* pointer + int */
			ejit_muli_l(state, rreg, rreg, tag->size);
		    else
			/* int + pointer */
			ejit_muli_l(state, lreg, lreg, tag->size);
		}
		ejit_addr_p(state, lreg, lreg, rreg);
	    }
	    else {
		if ((ltag->type & type_pointer) &&
		    (rtag->type & type_pointer)) {
		    /* pointer - pointer only allowed in subtraction */
		    ejit_subr_p(state, lreg, lreg, rreg);
		    if (tag->size != 1)
			ejit_muli_l(state, lreg, lreg, tag->size);
		    tag = ulong_tag;
		    lval->type = value_utype | value_ltype | value_regno;
		}
		else {
		    /* only "pointer - int" reach here */
		    if (tag->size != 1)
			ejit_muli_l(state, rreg, rreg, tag->size);
		    ejit_subr_p(state, lreg, lreg, rreg);
		}
	    }
	    break;
    }
    if (dec)
	dec_value_stack(1);

    return (tag);
}

static tag_t *
emit_binary_setup(expr_t *expr, token_t token, tag_t *ltag, tag_t *rtag,
		  value_t *lval, value_t *rval)
{
    tag_t	*tag;
    int		 freg;
    int		 lreg;
    int		 rreg;

    lreg = lval->u.ival;
    if (rval->type != value_ltype) {
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
			    if (rtag->type & type_pointer) {
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
			    if (rtag->type & type_pointer) {
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
			    if (rtag->type & type_pointer) {
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
			    if (rtag->type & type_pointer) {
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
		    freg = get_register(value_ftype);
		    ejit_extr_i_f(state, freg, rreg);
		    tag = float_tag;
		    rval->u.ival = freg;
		    rval->type = value_ftype | value_regno;
		    break;
		case type_long:		case type_ulong:
		    freg = get_register(value_ftype);
		    if (rreg == -1) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    ejit_extr_l_f(state, freg, rreg);
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
		    freg = get_register(value_dtype);
		    ejit_extr_i_d(state, freg, rreg);
		    tag = double_tag;
		    rval->u.ival = freg;
		    rval->type = value_dtype | value_regno;
		    break;
		case type_long:		case type_ulong:
		    freg = get_register(value_ftype);
		    if (rreg == -1) {
			emit_load(rval);
			rreg = rval->u.ival;
		    }
		    ejit_extr_l_d(state, freg, rreg);
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
	    if (!(ltag->type & type_pointer) || ltag->tag->size == 0)
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
		value->type = value_ftype | value_regno;
		value->u.ival = nreg;
	    }
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
		value->type = value_dtype | value_regno;
		value->u.ival = nreg;
	    }
	    break;
	default:
	    if (flt || !(tag->type & type_pointer))
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
emit_load(value_t *value)
{
    int		 fval;
    int		 regno;
    symbol_t	*symbol;
    void	*pointer;

    fval = (value->type & (value_ftype | value_dtype)) != 0;
    if (!(value->type & value_regno)) {
	regno = get_register(fval);
	switch (value->type) {
	    case value_funct:
		warn(NULL, "function value not handled");
		/* FIXME must be resolved if not jit */
		break;
	    case value_symbl:
		symbol = value->u.pval;
		if (symbol->arg) {
		    switch (symbol->tag->type) {
			case type_char:
			    value->type = 0;
			    ejit_getarg_c(state, regno, symbol->jit);
			    break;
			case type_uchar:
			    value->type = value_utype;
			    ejit_getarg_uc(state, regno, symbol->jit);
			    break;
			case type_short:
			    value->type = 0;
			    ejit_getarg_s(state, regno, symbol->jit);
			    break;
			case type_ushort:
			    value->type = value_utype;
			    ejit_getarg_us(state, regno, symbol->jit);
			    break;
			case type_int:
			    value->type = 0;
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
			    value->type = value_utype | value_ltype;
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
			    /* structures by value not supported */
			    assert(symbol->tag->type & type_pointer);
			    value->type = value_ptype;
			    ejit_getarg_p(state, regno, symbol->jit);
			    break;
		    }
		}
		else if (symbol->loc) {
		    switch (symbol->tag->type) {
			case type_char:
			    value->type = 0;
			    ejit_ldxi_c(state, regno, FRAME_POINTER,
					symbol->offset);
			    break;
			case type_uchar:
			    value->type = value_utype;
			    ejit_ldxi_uc(state, regno, FRAME_POINTER,
					 symbol->offset);
			    break;
			case type_short:
			    value->type = 0;
			    ejit_ldxi_s(state, regno, FRAME_POINTER,
					symbol->offset);
			    break;
			case type_ushort:
			    value->type = value_utype;
			    ejit_ldxi_us(state, regno, FRAME_POINTER,
					 symbol->offset);
			    break;
			case type_int:
			    value->type = 0;
			    ejit_ldxi_i(state, regno, FRAME_POINTER,
					symbol->offset);
			    break;
			case type_uint:
			    value->type = value_utype;
			    ejit_ldxi_ui(state, regno, FRAME_POINTER,
					 symbol->offset);
			    break;
			case type_long:
			    value->type = value_ltype;
			    ejit_ldxi_l(state, regno, FRAME_POINTER,
					symbol->offset);
			    break;
			case type_ulong:
			    value->type = value_utype | value_ltype;
			    ejit_ldxi_ul(state, regno, FRAME_POINTER,
					 symbol->offset);
			    break;
			case type_float:
			    value->type = value_ftype;
			    ejit_ldxi_f(state, regno, FRAME_POINTER,
					symbol->offset);
			    break;
			case type_double:
			    value->type = value_dtype;
			    ejit_ldxi_d(state, regno, FRAME_POINTER,
					symbol->offset);
			    break;
			default:
			    value->type = value_ptype;
			    if (symbol->tag->type & type_pointer)
				ejit_ldxi_p(state, regno, FRAME_POINTER,
					    symbol->offset);
			    else
				ejit_addi_p(state, regno, FRAME_POINTER,
					    (void *)symbol->offset);
			    break;
		    }
		}
		else {
		    pointer = (char *)the_data + symbol->offset;
		    switch (symbol->tag->type) {
			case type_char:
			    value->type = 0;
			    ejit_ldi_c(state, regno, pointer);
			    break;
			case type_uchar:
			    value->type = value_utype;
			    ejit_ldi_uc(state, regno, pointer);
			    break;
			case type_short:
			    value->type = 0;
			    ejit_ldi_s(state, regno, pointer);
			    break;
			case type_ushort:
			    value->type = value_utype;
			    ejit_ldi_us(state, regno, pointer);
			    break;
			case type_int:
			    value->type = 0;
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
			    value->type = value_utype | value_ltype;
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
		break;
	    case value_ltype:
		ejit_movi_l(state, regno, value->u.lval);
		value->u.ival = regno;
		break;
	    case value_dtype:
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
	if (fval)
	    ejit_ldxi_d(state, value->disp, FRAME_POINTER, regno);
	else
	    ejit_ldxi_l(state, value->disp, FRAME_POINTER, regno);
	value->type &= ~value_spill;
    }
}

static void
emit_store_symbol(expr_t *expr, symbol_t *symbol, value_t *value)
{
    int		 regno;
    void	*pointer;

    regno = value->u.ival;
    if (symbol->arg) {
	switch (symbol->tag->type) {
	    case type_char:
		ejit_putarg_c(state, symbol->jit, regno);
		break;
	    case type_uchar:
		ejit_putarg_uc(state, symbol->jit, regno);
		break;
	    case type_short:
		ejit_putarg_s(state, symbol->jit, regno);
		break;
	    case type_ushort:
		ejit_putarg_us(state, symbol->jit, regno);
		break;
	    case type_int:
		ejit_putarg_i(state, symbol->jit, regno);
		break;
	    case type_uint:
		ejit_putarg_ui(state, symbol->jit, regno);
		break;
	    case type_long:
		ejit_putarg_l(state, symbol->jit, regno);
		break;
	    case type_ulong:
		ejit_putarg_ul(state, symbol->jit, regno);
		break;
	    case type_float:
		ejit_putarg_f(state, symbol->jit, regno);
		break;
	    case type_double:
		ejit_putarg_d(state, symbol->jit, regno);
		break;
	    default:
		/* struct argument by value not supported */
		assert(symbol->tag->type & type_pointer);
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
		if (symbol->tag->type & type_pointer)
		    ejit_stxi_p(state, symbol->offset, FRAME_POINTER, regno);
		else
		    warn(expr, "struct copy not handled");
		break;
	}
    }
    else {
	pointer = (char *)the_data + symbol->offset;
	switch (symbol->tag->type) {
	    case type_char:
		ejit_sti_c(state, pointer, regno);
		break;
	    case type_uchar:
		ejit_sti_uc(state, pointer, regno);
		break;
	    case type_short:
		ejit_sti_s(state, pointer, regno);
		break;
	    case type_ushort:
		ejit_sti_us(state, pointer, regno);
		break;
	    case type_int:
		ejit_sti_i(state, pointer, regno);
		break;
	    case type_uint:
		ejit_sti_ui(state, pointer, regno);
		break;
	    case type_long:
		ejit_sti_l(state, pointer, regno);
		break;
	    case type_ulong:
		ejit_sti_ul(state, pointer, regno);
		break;
	    case type_float:
		ejit_sti_f(state, pointer, regno);
		break;
	    case type_double:
		ejit_sti_d(state, pointer, regno);
		break;
	    default:
		if (symbol->tag->type & type_pointer)
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
    int		 ient, count, regno, size, offset;

    count = freg ? fcount : icount;
    for (regno = 0; regno < count; regno++) {
	offset = 0;
	entry = vstack.values;
	for (; offset < vstack.offset; offset++, entry++) {
	    if ((entry->type & value_regno) && entry->u.ival == regno) {
		ient = (entry->type & (value_ftype | value_dtype)) == 0;
		/* try next one if register is live */
		if ((freg ^ ient) && !(entry->type & value_spill))
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
	    if (freg ^ ient)
		break;
	}
    }
    regno = offset;

    size = freg ? sizeof(double) : sizeof(long);
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
    if (freg)
	ejit_stxi_d(state, entry->disp, FRAME_POINTER, regno);
    else
	ejit_stxi_l(state, entry->disp, FRAME_POINTER, regno);

    return (regno);
}
