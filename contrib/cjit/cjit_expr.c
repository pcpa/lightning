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
 * Implementation
 */
expr_t *
new_expr(token_t token, int lineno, int column)
{
    expr_t	*expr = xcalloc(1, sizeof(expr_t));

    expr->token = token;
    expr->lineno = lineno;
    expr->column = column;

    return (expr);
}

void
del_expr(expr_t *expr)
{
    if (expr == NULL)
	return;
    switch (expr->token) {
	case tok_inc:			case tok_dec:
	case tok_postinc:		case tok_postdec:
	case tok_plus:			case tok_neg:
	case tok_not:			case tok_com:
	case tok_pointer:		case tok_address:
	case tok_sizeof:		case tok_list:
	case tok_goto:			case tok_return:
	    del_stat(expr->data._unary.expr);
	    break;
	case tok_set ... tok_andand:
	case tok_oror ... tok_rem:
	case tok_vector:		case tok_call:
	case tok_dot:			case tok_arrow:
	case tok_declexpr:		case tok_decl:
	    del_stat(expr->data._binary.lvalue);
	    del_stat(expr->data._binary.rvalue);
	    break;
	case tok_if:			case tok_question:
	    del_stat(expr->data._if.test);
	    del_stat(expr->data._if.tcode);
	    del_stat(expr->data._if.fcode);
	    break;
	case tok_while:
	    del_stat(expr->data._while.test);
	    del_stat(expr->data._while.code);
	    break;
	case tok_for:
	    del_stat(expr->data._for.init);
	    del_stat(expr->data._for.test);
	    del_stat(expr->data._for.incr);
	    del_stat(expr->data._for.code);
	    break;
	case tok_do:
	    del_stat(expr->data._do.code);
	    del_stat(expr->data._do.test);
	    break;
	case tok_switch:
	    del_stat(expr->data._switch.test);
	    del_stat(expr->data._switch.code);
	    del_hash(expr->data._switch.hash);
	    break;
	default:
	    break;
    }
    free(expr);
}

void
del_stat(expr_t *expr)
{
    expr_t	*next;

    while (expr) {
	next = expr->next;
	del_expr(expr);
	expr = next;
    }
}
