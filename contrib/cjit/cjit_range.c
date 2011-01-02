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
 * Types
 */
typedef struct range	range_t;

struct range {
    range_t	*next;
    int		 x0;
    int		 x1;
};

/*
 * Initialization
 */
static range_t	*list;

/*
 * Implementation
 */
void
range_reset(void)
{
    range_t	*r, *n;

    for (r = list; r; r = n) {
	n = r->next;
	free(r);
    }
    list = NULL;
}

int
range_get(int length)
{
    range_t	*r, *p;
    int		 offset;

    length = (length + ALIGN - 1) & -ALIGN;
    for (offset = 0, r = p = list; r; p = r, r = r->next) {
	if (offset + length < r->x0) {
	    if (p != r) {
		p->x1 = offset + length;
	    }
	    else {
		list = xmalloc(sizeof(range_t));
		list->next = r;
		list->x0 = offset;
		list->x1 = offset + length;
	    }
	}
	offset = r->x1;
    }
    if (r == NULL) {
	if (p != NULL) {
	    offset = p->x1;
	    p->x1 += length;
	}
	else {
	    list = xmalloc(sizeof(range_t));
	    list->x0 = 0;
	    list->x1 = offset + length;
	}
    }

    return (offset);    
}

void
range_clr(int offset, int length)
{
    int		x0, x1;
    range_t	*r, *p, *n;

    x0 = (offset + ALIGN - 1) & -ALIGN;
    x1 = (x0 + length + ALIGN - 1) & -ALIGN;

    for (r = p = list; r; p = r, r = r->next) {
	if (r->x0 == offset)
	    break;
    }

    if (r == NULL)
	return;

    for (; r; p = r, r = n) {
	if (r->x0 >= x1)
	    break;
	n = r->next;
	if (p == r)
	    list = p = n;
	free(r);
    }
}
