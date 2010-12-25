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

/*
 * Prototypes
 */
static void
binary(expr_t *expr);

static void
binary_int(expr_t *expr, expr_t *lexp, expr_t *rexp);

static void
binary_float(expr_t *expr, expr_t *lexp, expr_t *rexp);

static void
unary(expr_t *expr);

static void
unary_int(expr_t *expr, expr_t *uexp);

static void
unary_float(expr_t *expr, expr_t *uexp);

static void
unary_sizeof(expr_t *expr);

static tag_t *
eval_type(expr_t *expr);

/*
 * Implementation
 */
int
eval(expr_t *expr)
{
    expr_t	*temp;

    switch (expr->token) {
	case tok_set:		case tok_andset:
	case tok_orset:		case tok_xorset:
	case tok_lshset:	case tok_rshset:
	case tok_addset:	case tok_subset:
	case tok_mulset:	case tok_divset:
	case tok_remset:	case tok_vector:
	case tok_dot:		case tok_arrow:
	    eval(expr->data._binary.lvalue);
	    eval(expr->data._binary.rvalue);
	    break;
	case tok_andand:	case tok_oror:
	case tok_lt:		case tok_le:
	case tok_eq:		case tok_ge:
	case tok_gt:		case tok_ne:
	case tok_and:		case tok_or:
	case tok_xor:		case tok_lsh:
	case tok_rsh:		case tok_add:
	case tok_sub:		case tok_mul:
	case tok_div:		case tok_rem:
	    binary(expr);
	    break;
	case tok_inc:		case tok_dec:
	case tok_postinc:	case tok_postdec:
	case tok_pointer:	case tok_address:
	    eval(expr->data._unary.expr);
	    break;
	case tok_plus:		case tok_neg:
	case tok_not:		case tok_com:
	    unary(expr);
	    break;
	case tok_sizeof:
	    unary_sizeof(expr);
	    break;
	case tok_question:
	    eval(expr->data._if.test);
	    eval(expr->data._if.tcode);
	    eval(expr->data._if.fcode);
	    break;
	case tok_if:
	    eval_stat(expr->data._if.test);
	    eval_stat(expr->data._if.tcode);
	    eval_stat(expr->data._if.fcode);
	    break;
	case tok_return:
	    if (expr->data._unary.expr)
		eval(expr->data._unary.expr);
	    break;
	case tok_switch:
	    eval_stat(expr->data._switch.test);
	    eval_stat(expr->data._switch.code);
	    break;
	case tok_for:
	    eval_stat(expr->data._for.init);
	    eval_stat(expr->data._for.test);
	    eval_stat(expr->data._for.incr);
	    eval_stat(expr->data._for.code);
	    break;
	case tok_do:
	    eval_stat(expr->data._do.code);
	    eval_stat(expr->data._do.test);
	    break;
	case tok_while:
	    eval_stat(expr->data._while.test);
	    eval_stat(expr->data._while.code);
	    break;
	case tok_list:
	    temp = expr->data._unary.expr;
	    eval_stat(temp);
	    if (temp->next == NULL) {
		memcpy(expr, temp, sizeof(expr_t));
		temp->token = tok_none;
		del_expr(temp);
	    }
	    break;
	case tok_stat:
	    eval_stat(expr->data._unary.expr);
	    break;
	case tok_call:
	    eval(expr->data._binary.lvalue);
	    eval_stat(expr->data._binary.rvalue);
	    break;
	case tok_code:		case tok_data:
	    eval_stat(expr->data._unary.expr);
	    break;
	case tok_decl:
	    eval(expr->data._binary.lvalue);
	    eval_stat(expr->data._binary.rvalue);
	    break;
	case tok_function:
	    current = expr->data._function.function->table;
	    eval(expr->data._function.type);
	    eval(expr->data._function.call);
	    eval(expr->data._function.body);
	    current = globals;
	    break;
	default:
	    break;
    }

    return (0);
}

int
eval_stat(expr_t *expr)
{
    for (; expr; expr = expr->next)
	eval(expr);

    return (0);
}

static void
binary(expr_t *expr)
{
    expr_t	*lexp = expr->data._binary.lvalue;
    expr_t	*rexp = expr->data._binary.rvalue;

    eval(lexp);
    eval(rexp);
    switch (lexp->token) {
	case tok_int:
	    switch (rexp->token) {
		case tok_int:
		    binary_int(expr, lexp, rexp);
		    break;
		case tok_float:
		    lexp->token = tok_float;
		    lexp->data._unary.d = lexp->data._unary.i;
		    binary_float(expr, lexp, rexp);
		    break;
		default:
		    break;
	    }
	    break;
	case tok_float:
	    switch (rexp->token) {
		case tok_int:
		    rexp->token = tok_float;
		    rexp->data._unary.d = rexp->data._unary.i;
		case tok_float:
		    binary_float(expr, lexp, rexp);
		    break;
		default:
		    break;
	    }
	    break;
	default:
	    break;
    }
}

static void
binary_int(expr_t *expr, expr_t *lexp, expr_t *rexp)
{
    long	lval = lexp->data._unary.i;
    long	rval = rexp->data._unary.i;

    switch (expr->token) {
	case tok_andand:	lval = lval && rval;	break;
	case tok_oror:		lval = lval || rval;	break;
	case tok_lt:		lval = lval <  rval;	break;
	case tok_le:		lval = lval <= rval;	break;
	case tok_eq:		lval = lval == rval;	break;
	case tok_ge:		lval = lval >= rval;	break;
	case tok_gt:		lval = lval >  rval;	break;
	case tok_ne:		lval = lval != rval;	break;
	case tok_and:		lval = lval &  rval;	break;
	case tok_or:		lval = lval |  rval;	break;
	case tok_xor:		lval = lval ^  rval;	break;
	case tok_lsh:		lval = lval << rval;	break;
	case tok_rsh:		lval = lval >> rval;	break;
	case tok_add:		lval = lval +  rval;	break;
	case tok_sub:		lval = lval -  rval;	break;
	case tok_mul:		lval = lval *  rval;	break;
	case tok_div:
	    if (rval == 0) {
		warn(expr, "divide by zero");
		return;
	    }
	    lval = lval / rval;
	    break;
	case tok_rem:
	    if (rval == 0) {
		warn(expr, "divide by zero");
		return;
	    }
	    lval = lval % rval;
	    break;
	default:
	    error(expr, "internal error");
    }

    del_expr(lexp);
    del_expr(rexp);
    expr->data._unary.i = lval;
    expr->token = tok_int;
}

static void
binary_float(expr_t *expr, expr_t *lexp, expr_t *rexp)
{
    int		iexp;
    long	ival;
    double	lval = lexp->data._unary.d;
    double	rval = rexp->data._unary.d;

    switch (expr->token) {
	case tok_andand:	iexp = 1; ival = lval && rval;	break;
	case tok_oror:		iexp = 1; ival = lval || rval;	break;
	case tok_lt:		iexp = 1; ival = lval <  rval;	break;
	case tok_le:		iexp = 1; ival = lval <= rval;	break;
	case tok_eq:		iexp = 1; ival = lval == rval;	break;
	case tok_ge:		iexp = 1; ival = lval >= rval;	break;
	case tok_gt:		iexp = 1; ival = lval >  rval;	break;
	case tok_ne:		iexp = 1; ival = lval != rval;	break;
	case tok_and:		case tok_or:		case tok_xor:
	case tok_lsh:		case tok_rsh:		case tok_rem:
	    error(expr, "not an integer");
	case tok_add:		iexp = 0; lval = lval +  rval;	break;
	case tok_sub:		iexp = 0; lval = lval -  rval;	break;
	case tok_mul:		iexp = 0; lval = lval *  rval;	break;
	case tok_div:
	    if (rval == 0.0) {
		warn(expr, "divide by zero");
		return;
	    }
	    lval = lval / rval;
	    break;
	default:
	    error(expr, "internal error");
    }

    del_expr(expr->data._binary.lvalue);
    del_expr(expr->data._binary.rvalue);
    if (iexp) {
	expr->data._unary.i = ival;
	expr->token = tok_int;
    }
    else {
	expr->data._unary.d = lval;
	expr->token = tok_float;
    }
}

static void
unary(expr_t *expr)
{
    expr_t	*uexp = expr->data._unary.expr;

    switch (expr->token) {
	case tok_int:		unary_int(expr, uexp);		break;
	case tok_float:		unary_float(expr, uexp);	break;
	default:						return;
    }
}

static void
unary_int(expr_t *expr, expr_t *uexp)
{
    long	uval = uexp->data._unary.i;

    switch (expr->token) {
	case tok_plus:					break;
	case tok_neg:		uval = -uval;		break;
	case tok_not:		uval = !uval;		break;
	case tok_com:		uval = ~uval;		break;
	default:	error(expr, "internal error");
    }

    del_expr(uexp);
    expr->data._unary.i = uval;
    expr->token = tok_int;
}

static void
unary_float(expr_t *expr, expr_t *uexp)
{
    int		iexp;
    long	ival;
    double	dval = uexp->data._unary.d;

    switch (expr->token) {
	case tok_plus:	iexp = 0;			break;
	case tok_neg:	iexp = 0; dval = -dval;		break;
	case tok_not:	iexp = 1; ival = !dval;		break;
	case tok_com:	error(expr, "not an integer");
	default:	error(expr, "internal error");
    }

    del_expr(uexp);
    if (iexp) {
	expr->data._unary.i = ival;
	expr->token = tok_int;
    }
    else {
	expr->data._unary.d = dval;
	expr->token = tok_float;
    }
}

static void
unary_sizeof(expr_t *expr)
{
    tag_t	*tag;
    int		 size;
    expr_t	*uexp;

    eval(expr->data._unary.expr);
    uexp = expr->data._unary.expr;
    switch (uexp->token) {
	case tok_type:
	    tag = uexp->data._unary.vp;
	    size = tag->size;
	    break;
	case tok_int:
	    size = sizeof(int);
	    break;
	case tok_float:
	    size = sizeof(double);
	    break;
	case tok_string:
	    size = strlen(uexp->data._unary.cp) + 1;
	    break;
	default:
	    tag = eval_type(uexp);
	    size = tag->size;
	    break;
    }
    del_expr(uexp);
    expr->token = tok_int;
    expr->data._unary.i = size;
}

static tag_t *
eval_type(expr_t *expr)
{
    tag_t	*tag;
    symbol_t	*symbol;
    record_t	*record;

    switch (expr->token) {
	case tok_symbol:
	    if ((symbol = get_symbol(expr->data._unary.cp)) == NULL)
		error(expr, "syntax error");
	    return (symbol->tag);
	case tok_pointer:
	    tag = eval_type(expr->data._unary.expr);
	    if (!pointer_type_p(tag->type))
		error(expr, "not a pointer");
	    return (tag->tag);
	case tok_vector:
	    tag = eval_type(expr->data._binary.lvalue)->tag;
	    if (!pointer_type_p(tag->type))
		error(expr, "not a vector");
	    return (tag->tag);
	case tok_dot:
	    tag = eval_type(expr->data._binary.lvalue);
	    if (pointer_type_p(tag->type))
		error(expr, "not a struct or union");
	    goto record_tag;
	case tok_arrow:
	    tag = eval_type(expr->data._binary.lvalue);
	    if (!pointer_type_p(tag->type))
		error(expr, "not a pointer");
	    tag = tag->tag;
	record_tag:
	    if (type_mask(tag->type) != type_struct &&
		type_mask(tag->type) != type_union)
		error(expr, "not a struct or union");
	    record = tag->name;
	    expr = expr->data._binary.rvalue;
	    if (expr->token != tok_symbol)
		error(expr, "syntax error");
	    symbol = (symbol_t *)
		get_hash((hash_t *)record, expr->data._unary.cp);
	    if (symbol == NULL)
		error(expr, "no '%s' field in '%s'", expr->data._unary.cp,
		      record->name ? record->name->name.string : "<anonymous>");
	    return (symbol->tag);
	default:
	    error(expr, "syntax error");
    }
}
