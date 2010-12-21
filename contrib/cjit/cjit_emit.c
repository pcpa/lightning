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
emit_cmp(expr_t *expr);

static tag_t *
emit_binint(expr_t *expr);

static tag_t *
emit_function(expr_t *expr);

static void
emit_load(value_t *value);

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
	    value->u.ival = expr->data._unary.i;
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
	case tok_rsh:
	    return (emit_binint(expr));
	case tok_add:
	case tok_sub:		case tok_mul:
	case tok_div:		case tok_rem:
	    (void)emit_expr(expr->data._binary.lvalue);
	    return (emit_expr(expr->data._binary.rvalue));
	    break;
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

/* FIXME coercion of signed op unsigned (or vice versa) may be reversed,
 * that is, it does signed comparison... */
static tag_t *
emit_cmp(expr_t *expr)
{
    int		 dec;
    tag_t	*ltag;
    tag_t	*rtag;
    int		 freg;
    int		 lreg;
    int		 rreg;
    value_t	*lvalue;
    value_t	*rvalue;

    dec = 1;
    ltag = emit_expr(expr->data._binary.lvalue);
    lvalue = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rvalue = top_value_stack();
    emit_load(lvalue);
    lreg = lvalue->u.ival;

    switch (ltag->type) {
	case type_char:			case type_short:	case type_int:
	    switch (rtag->type) {
		case type_uchar:	case type_ushort:	case type_uint:
		    warn(expr, "mixed sign comparison");
		case type_char:		case type_short:	case type_int:
		int_int:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_i(state, lreg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_i(state, lreg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_i(state, lreg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_i(state, lreg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_i(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_ner_i(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_ulong:
		    warn(expr, "mixed sign comparison");
		    ejit_extr_i_l(state, lreg, lreg);
		long_rlong:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_l(state, lreg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_l(state, lreg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_l(state, lreg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_l(state, lreg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_l(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_ner_l(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_long:
		int_xlong:
		    ejit_extr_i_l(state, lreg, lreg);
		long_xlong:
		    if (rvalue->type == value_ltype) {
			dec = 0;
			switch (expr->token) {
			    case tok_lt:
				ejit_lti_l(state, lreg, lreg, rvalue->u.lval);
				break;
			    case tok_le:
				ejit_lei_l(state, lreg, lreg, rvalue->u.lval);
				break;
			    case tok_eq:
				ejit_eqi_l(state, lreg, lreg, rvalue->u.lval);
				break;
			    case tok_ge:
				ejit_gei_l(state, lreg, lreg, rvalue->u.lval);
				break;
			    case tok_gt:
				ejit_gti_l(state, lreg, lreg, rvalue->u.lval);
				break;
			    default:
				ejit_nei_l(state, lreg, lreg, rvalue->u.lval);
				break;
			}
		    }
		    else
			goto long_rlong;
		    break;
		case type_float:
		int_float:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    freg = get_register(value_ftype);
		    ejit_extr_i_f(state, freg, lreg);
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_f(state, freg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_f(state, freg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_f(state, freg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_f(state, freg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_f(state, freg, lreg, rreg);
			    break;
			default:
			    ejit_ner_f(state, freg, lreg, rreg);
			    break;
		    }
		    break;
		case type_double:
		int_double:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    freg = get_register(value_dtype);
		    ejit_extr_i_d(state, freg, lreg);
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_d(state, freg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_d(state, freg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_d(state, freg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_d(state, freg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_d(state, freg, lreg, rreg);
			    break;
			default:
			    ejit_ner_d(state, freg, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    error(expr, "invalid comparison");
	    }
	    break;
	case type_uchar:		case type_ushort:	case type_uint:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		    warn(expr, "mixed sign comparison");
		    goto int_int;
		case type_uchar:	case type_ushort:	case type_uint:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_ui(state, lreg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_ui(state, lreg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_ui(state, lreg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_ui(state, lreg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_ui(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_ner_ui(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_long:
		    warn(expr, "mixed sign comparison");
		    goto int_xlong;
		case type_ulong:
		    ejit_extr_ui_ul(state, lreg, lreg);
		ulong_ulong:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_ul(state, lreg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_ul(state, lreg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_ul(state, lreg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_ul(state, lreg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_ul(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_ner_ul(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_float:
		    /* FIXME unsigned */
		    goto int_float;
		case type_double:
		    goto int_double;
		default:
		    error(expr, "invalid comparison");
	    }
	    break;
	case type_long:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		    ejit_extr_i_l(state, lreg, lreg);
		    goto long_rlong;
		case type_uchar:	case type_ushort:	case type_uint:
		    warn(expr, "mixed sign comparison");
		    ejit_extr_ui_ul(state, lreg, lreg);
		    goto long_rlong;
		case type_long:
		    goto long_xlong;
		case type_ulong:
		    warn(expr, "mixed sign comparison");
		    goto long_rlong;
		case type_float:
		long_float:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    freg = get_register(value_ftype);
		    ejit_extr_l_f(state, freg, lreg);
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_f(state, freg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_f(state, freg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_f(state, freg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_f(state, freg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_f(state, freg, lreg, rreg);
			    break;
			default:
			    ejit_ner_f(state, freg, lreg, rreg);
			    break;
		    }
		    break;
		case type_double:
		long_double:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    freg = get_register(value_dtype);
		    ejit_extr_l_d(state, freg, lreg);
		    switch (expr->token) {
			case tok_lt:
			    ejit_ltr_d(state, freg, lreg, rreg);
			    break;
			case tok_le:
			    ejit_ler_d(state, freg, lreg, rreg);
			    break;
			case tok_eq:
			    ejit_eqr_d(state, freg, lreg, rreg);
			    break;
			case tok_ge:
			    ejit_ger_d(state, freg, lreg, rreg);
			    break;
			case tok_gt:
			    ejit_gtr_d(state, freg, lreg, rreg);
			    break;
			default:
			    ejit_ner_d(state, freg, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    error(expr, "invalid comparison");
	    }
	    break;
	case type_ulong:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		    warn(expr, "mixed sign comparison");
		    ejit_extr_i_l(state, lreg, lreg);
		    goto long_rlong;
		case type_uchar:	case type_ushort:	case type_uint:
		    ejit_extr_ui_ul(state, lreg, lreg);
		    goto ulong_ulong;
		case type_long:
		    warn(expr, "mixed sign comparison");
		    goto long_xlong;
		case type_ulong:
		    goto ulong_ulong;
		case type_float:
		    /* FIXME unsigned */
		    goto long_float;
		case type_double:
		    goto long_double;
		default:
		    error(expr, "invalid comparison");
	    }
	    break;
	default:
	    if (!(ltag->type & type_pointer) || !(rtag->type & type_pointer))
		error(expr, "invalid comparison");
	    if (ltag != rtag)
		warn(expr, "mixed pointer comparison");
	    emit_load(rvalue);
	    rreg = rvalue->u.ival;
	    freg = get_register(value_dtype);
	    ejit_extr_l_d(state, freg, lreg);
	    switch (expr->token) {
		case tok_lt:
		    ejit_ltr_p(state, lreg, lreg, rreg);
		    break;
		case tok_le:
		    ejit_ler_p(state, lreg, lreg, rreg);
		    break;
		case tok_eq:
		    ejit_eqr_p(state, lreg, lreg, rreg);
		    break;
		case tok_ge:
		    ejit_ger_p(state, lreg, lreg, rreg);
		    break;
		case tok_gt:
		    ejit_gtr_p(state, lreg, lreg, rreg);
		    break;
		default:
		    ejit_ner_p(state, lreg, lreg, rreg);
		    break;
	    }
	    break;
    }

    if (dec)
	dec_value_stack(1);
    lvalue->type = value_regno;
    return (int_tag);
}

static tag_t *
emit_binint(expr_t *expr)
{
    int		 dec;
    tag_t	*ltag;
    tag_t	*rtag;
    int		 lreg;
    int		 rreg;
    long	 rval;
    value_t	*lvalue;
    value_t	*rvalue;

    dec = 1;
    ltag = emit_expr(expr->data._binary.lvalue);
    lvalue = top_value_stack();
    rtag = emit_expr(expr->data._binary.rvalue);
    rvalue = top_value_stack();
    emit_load(lvalue);
    lreg = lvalue->u.ival;

    switch (ltag->type) {
	case type_char:			case type_short:	case type_int:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    ltag = int_tag;
		    lvalue->type = value_regno;
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_i(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_i(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_i(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_i(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_i(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_long:
		    if (expr->token < tok_lsh) {
			ejit_extr_i_l(state, lreg, lreg);
			ltag = long_tag;
			lvalue->type = value_ltype | value_regno;
		    }
		    else {
			ltag = int_tag;
			lvalue->type = value_regno;
		    }
		    if (rvalue->type == value_ltype) {
			dec = 0;
			rval = rvalue->u.lval;
			switch (expr->token) {
			    case tok_and:
				ejit_andi_l(state, lreg, lreg, rval);
				break;
			    case tok_or:
				ejit_ori_l(state, lreg, lreg, rval);
				break;
			    case tok_xor:
				ejit_xori_l(state, lreg, lreg, rval);
				break;
			    case tok_lsh:
				ejit_lshi_i(state, lreg, lreg, rval);
				break;
			    default:
				ejit_rshi_i(state, lreg, lreg, rval);
				break;
			}
		    }
		    else {
			emit_load(rvalue);
			rreg = rvalue->u.ival;
			switch (expr->token) {
			    case tok_and:
				ejit_andr_l(state, lreg, lreg, rreg);
				break;
			    case tok_or:
				ejit_orr_l(state, lreg, lreg, rreg);
				break;
			    case tok_xor:
				ejit_xorr_l(state, lreg, lreg, rreg);
				break;
			    case tok_lsh:
				ejit_lshr_i(state, lreg, lreg, rreg);
				break;
			    default:
				ejit_rshr_i(state, lreg, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_ulong:
		    if (expr->token < tok_lsh) {
			ejit_extr_ui_ul(state, lreg, lreg);
			ltag = ulong_tag;
			lvalue->type = value_ltype | value_regno;
		    }
		    else {
			ltag = int_tag;
			lvalue->type = value_regno;
		    }
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_l(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_l(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_l(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_i(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_i(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    error(expr, "not an integer");
	    }
	    break;
	case type_uchar:		case type_ushort:	case type_uint:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    if (expr->token >= tok_lsh ||
			(rtag->type & type_unsigned)) {
			ltag = uint_tag;
			lvalue->type = value_utype | value_regno;
		    }
		    else {
			ltag = int_tag;
			lvalue->type = value_regno;
		    }
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_i(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_i(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_i(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_i(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_ui(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_long:
		    if (expr->token < tok_lsh) {
			ejit_extr_ui_ul(state, lreg, lreg);
			ltag = ulong_tag;
			lvalue->type = value_utype | value_ltype | value_regno;
		    }
		    else {
			ltag = uint_tag;
			lvalue->type = value_utype | value_regno;
		    }
		    if (rvalue->type == value_ltype) {
			dec = 0;
			rval = rvalue->u.lval;
			switch (expr->token) {
			    case tok_and:
				ejit_andi_l(state, lreg, lreg, rval);
				break;
			    case tok_or:
				ejit_ori_l(state, lreg, lreg, rval);
				break;
			    case tok_xor:
				ejit_xori_l(state, lreg, lreg, rval);
				break;
			    case tok_lsh:
				ejit_lshi_i(state, lreg, lreg, rval);
				break;
			    default:
				ejit_rshi_ui(state, lreg, lreg, rval);
				break;
			}
		    }
		    else {
			emit_load(rvalue);
			rreg = rvalue->u.ival;
			switch (expr->token) {
			    case tok_and:
				ejit_andr_l(state, lreg, lreg, rreg);
				break;
			    case tok_or:
				ejit_orr_l(state, lreg, lreg, rreg);
				break;
			    case tok_xor:
				ejit_xorr_l(state, lreg, lreg, rreg);
				break;
			    case tok_lsh:
				ejit_lshr_i(state, lreg, lreg, rreg);
				break;
			    default:
				ejit_rshr_ui(state, lreg, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_ulong:
		    if (expr->token < tok_lsh) {
			ejit_extr_ui_ul(state, lreg, lreg);
			ltag = ulong_tag;
			lvalue->type = value_utype | value_ltype | value_regno;
		    }
		    else {
			ltag = uint_tag;
			lvalue->type = value_utype | value_regno;
		    }
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_l(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_l(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_l(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_i(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_ui(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    error(expr, "not an integer");
	    }
	    break;
	case type_long:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    ltag = long_tag;
		    lvalue->type = value_ltype | value_regno;
		    if (expr->token < tok_lsh) {
			if (rtag->type & type_unsigned)
			    ejit_extr_ui_ul(state, rreg, rreg);
			else
			    ejit_extr_i_l(state, rreg, rreg);
		    }
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_l(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_l(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_l(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_l(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_l(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_long:
		    ltag = long_tag;
		    lvalue->type = value_ltype | value_regno;
		    if (rvalue->type == value_ltype) {
			dec = 0;
			rval = rvalue->u.lval;
			switch (expr->token) {
			    case tok_and:
				ejit_andi_l(state, lreg, lreg, rval);
				break;
			    case tok_or:
				ejit_ori_l(state, lreg, lreg, rval);
				break;
			    case tok_xor:
				ejit_xori_l(state, lreg, lreg, rval);
				break;
			    case tok_lsh:
				ejit_lshi_l(state, lreg, lreg, rval);
				break;
			    default:
				ejit_rshi_l(state, lreg, lreg, rval);
				break;
			}
		    }
		    else {
			emit_load(rvalue);
			rreg = rvalue->u.ival;
			switch (expr->token) {
			    case tok_and:
				ejit_andr_l(state, lreg, lreg, rreg);
				break;
			    case tok_or:
				ejit_orr_l(state, lreg, lreg, rreg);
				break;
			    case tok_xor:
				ejit_xorr_l(state, lreg, lreg, rreg);
				break;
			    case tok_lsh:
				ejit_lshr_l(state, lreg, lreg, rreg);
				break;
			    default:
				ejit_rshr_l(state, lreg, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_ulong:
		    ltag = long_tag;
		    lvalue->type = value_ltype | value_regno;
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_l(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_l(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_l(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_i(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_i(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    error(expr, "not an integer");
	    }
	    break;
	case type_ulong:
	    switch (rtag->type) {
		case type_char:		case type_short:	case type_int:
		case type_uchar:	case type_ushort:	case type_uint:
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    ltag = ulong_tag;
		    lvalue->type = value_utype | value_ltype | value_regno;
		    if (expr->token < tok_lsh) {
			if (rtag->type & type_unsigned)
			    ejit_extr_ui_ul(state, rreg, rreg);
			else
			    ejit_extr_i_l(state, rreg, rreg);
		    }
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_l(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_l(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_l(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_l(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_ul(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		case type_long:
		    ltag = ulong_tag;
		    lvalue->type = value_utype | value_ltype | value_regno;
		    if (rvalue->type == value_ltype) {
			dec = 0;
			rval = rvalue->u.lval;
			switch (expr->token) {
			    case tok_and:
				ejit_andi_l(state, lreg, lreg, rval);
				break;
			    case tok_or:
				ejit_ori_l(state, lreg, lreg, rval);
				break;
			    case tok_xor:
				ejit_xori_l(state, lreg, lreg, rval);
				break;
			    case tok_lsh:
				ejit_lshi_l(state, lreg, lreg, rval);
				break;
			    default:
				ejit_rshi_ul(state, lreg, lreg, rval);
				break;
			}
		    }
		    else {
			emit_load(rvalue);
			rreg = rvalue->u.ival;
			switch (expr->token) {
			    case tok_and:
				ejit_andr_l(state, lreg, lreg, rreg);
				break;
			    case tok_or:
				ejit_orr_l(state, lreg, lreg, rreg);
				break;
			    case tok_xor:
				ejit_xorr_l(state, lreg, lreg, rreg);
				break;
			    case tok_lsh:
				ejit_lshr_l(state, lreg, lreg, rreg);
				break;
			    default:
				ejit_rshr_ul(state, lreg, lreg, rreg);
				break;
			}
		    }
		    break;
		case type_ulong:
		    ltag = ulong_tag;
		    lvalue->type = value_utype | value_ltype | value_regno;
		    emit_load(rvalue);
		    rreg = rvalue->u.ival;
		    switch (expr->token) {
			case tok_and:
			    ejit_andr_l(state, lreg, lreg, rreg);
			    break;
			case tok_or:
			    ejit_orr_l(state, lreg, lreg, rreg);
			    break;
			case tok_xor:
			    ejit_xorr_l(state, lreg, lreg, rreg);
			    break;
			case tok_lsh:
			    ejit_lshr_l(state, lreg, lreg, rreg);
			    break;
			default:
			    ejit_rshr_ul(state, lreg, lreg, rreg);
			    break;
		    }
		    break;
		default:
		    error(expr, "not an integer");
	    }
	    break;
	default:
	    error(expr, "not an integer");
    }
    if (dec)
	dec_value_stack(1);

    return (ltag);
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
			    assert(!(symbol->tag->type & type_pointer));
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
			    if (symbol->tag->type & type_pointer)
				ejit_ldi_p(state, regno, pointer);
			    else
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
