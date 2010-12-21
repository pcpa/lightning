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
message(expr_t *expr, const char *kind, const char *format, va_list ap);

/*
 * Implementation
 */
void
error(expr_t *expr, const char *format, ...)
{
    va_list	ap;
    va_start(ap, format);
    message(expr, "error", format, ap);
    va_end(ap);
    exit(-1);
}

void
warn(expr_t *expr, const char *format, ...)
{
    va_list	ap;
    va_start(ap, format);
    message(expr, "warning", format, ap);
    va_end(ap);
}

static void
message(expr_t *expr, const char *kind, const char *format, va_list ap)
{
    fprintf(stderr, "%s:", parser.name);
    if (expr)
	fprintf(stderr, "%d:%d: ", expr->lineno, expr->column);
    else
	fprintf(stderr, "%d: ", parser.lineno - parser.newline);
    fprintf(stderr, "%s: ", kind);
    vfprintf(stderr, format, ap);
    fputc('\n', stderr);
}
