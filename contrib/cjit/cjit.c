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
hash_t		*strings;
void		*the_data;

/*
 * Implementation
 */
int
main(int argc, char *argv[])
{
    expr_t	*expr;

    init_alloc();
    init_stack();
    init_parser();
    init_type();
    init_emit();

    expr = parse();
    print(expr);
    the_data = xmalloc(globals->length);
    emit(expr);

    return (0);
}