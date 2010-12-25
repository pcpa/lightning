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
    printf("\ncode:\n");
    print(expr);
    /* eval after printing because certain changes may "look" wrong,
     * like removal of tok_list for single expression inside parenthesis
     * FIXME this should be dropped at some point either way, but
     * print code should be used for preprocessor stringify #expr */
    eval_stat(expr);
    globals->length = globals->offset;
    the_data = xmalloc(globals->length);
    expr = data(expr);
    printf("\ndata:\n");
    dump();
    emit(expr);

    return (0);
}
