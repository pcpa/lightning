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

/* FIXME eval() should be called on the toplevel and traverse all
 * expressions (also, case switches should be filled here), non
 * side effect expressions removed and safe float operations
 * constant folded
 */

#define unary_integer_p(expr)						\
    ((expr)->data._unary.expr->token == tok_int)

#define binary_integer_p(expr)						\
    ((expr)->data._binary.lvalue->token == tok_int &&			\
     (expr)->data._binary.rvalue->token == tok_int)

/*
 * Prototypes
 */
static void
eval_stat(expr_t *expr);

static void
eval_binary(expr_t *expr);

static void
binary(expr_t *expr);

static void
unary(expr_t *expr);

/*
 * Implementation
 */
void
eval(expr_t *expr)
{
    switch (expr->token) {
	case tok_andand:	case tok_oror:
	case tok_lt:		case tok_le:
	case tok_eq:		case tok_ge:
	case tok_gt:		case tok_ne:
	case tok_and:		case tok_or:
	case tok_xor:		case tok_lsh:
	case tok_rsh:		case tok_add:
	case tok_sub:		case tok_mul:
	case tok_div:		case tok_rem:
	    eval_binary(expr);
	    if (binary_integer_p(expr))
		binary(expr);
	    break;
	case tok_plus:		case tok_neg:
	case tok_not:		case tok_com:
	    eval(expr->data._unary.expr);
	    if (unary_integer_p(expr))
		unary(expr);
	    break;
	default:
	    break;
    }
}

static void
eval_stat(expr_t *expr)
{
    for (; expr; expr = expr->next)
	eval(expr);
}

static void
eval_binary(expr_t *expr)
{
    eval(expr->data._binary.lvalue);
    eval(expr->data._binary.rvalue);
}

static void
binary(expr_t *expr)
{
    long	lval = expr->data._binary.lvalue->data._unary.i;
    long	rval = expr->data._binary.rvalue->data._unary.i;

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
	    return;
    }

    del_expr(expr->data._binary.lvalue);
    del_expr(expr->data._binary.rvalue);
    expr->data._unary.i = lval;
    expr->token = tok_int;
}

static void
unary(expr_t *expr)
{
    long	value = expr->data._unary.expr->data._unary.i;

    switch (expr->token) {
	case tok_plus:					break;
	case tok_neg:		value = -value;		break;
	case tok_not:		value = !value;		break;
	case tok_com:		value = ~value;		break;
	default:					return;
    }

    del_expr(expr->data._unary.expr);
    expr->data._unary.i = value;
    expr->token = tok_int;
}
