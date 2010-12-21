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
 * Initialization
 */
estack_t	estack;
vstack_t	vstack;

/*
 * Implementation
 */
void
init_stack(void)
{
    estack.length = 16;
    estack.values = (expr_t **)xmalloc(estack.length * sizeof(expr_t *));
}

void
push_expr(expr_t *expr)
{
    if (estack.offset >= estack.length) {
	estack.length += 16;
	estack.values = (expr_t **)
	    xrealloc(estack.values, estack.length * sizeof(expr_t *));
    }
    estack.values[estack.offset++] = expr;
}

expr_t *
pop_expr(void)
{
    assert(estack.offset > 0);
    return (estack.values[--estack.offset]);
}
