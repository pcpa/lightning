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
static tag_t *
data_vector_tag(tag_t *tag, expr_t *expr);

static void
data_decl(expr_t *expr);

static void
data_set(expr_t *expr);

static void
data_init(void *pointer, tag_t *tag, expr_t *expr);

static void
data_record(void *pointer, tag_t *tag, expr_t *expr);

static void
data_vector(void *pointer, tag_t *tag, expr_t *expr);

static void
init_rodata(void);

static char *
rodata_string(expr_t *expr, char *string);

/*
 * Initialization
 */
void		*the_data;
/* as the name implies, should be read-only memory at jit execution time */
void		*the_rodata;
long		 rodata_length;
static hash_t	*data_hash;

/*
 * Implementation
 */
expr_t *
data(expr_t *expr)
{
    expr_t	*base;
    expr_t	*prev;
    expr_t	*next;

    init_rodata();
    for (base = prev = expr; expr;) {
	next = expr->next;
	switch (expr->token) {
	    case tok_decl:
		data_decl(expr->data._binary.rvalue);
		if (prev == expr)
		    base = prev = next;
		else
		    prev->next = next;
		del_expr(expr);
		expr = next;
		break;
	    default:
		prev = expr;
		expr = next;
		break;
	}
    }

    return (base);
}

tag_t *
data_tag(tag_t *tag, expr_t *expr)
{
    if (expr->token == tok_data)
	return (data_vector_tag(tag, expr->data._unary.expr));
    else if (expr->token == tok_string)
	/* if the base type is not char error will be triggered later */
	return (tag_vector(tag->tag, strlen(expr->data._unary.cp) + 1));
    return (tag);
}

static tag_t *
data_vector_tag(tag_t *tag, expr_t *expr)
{
    tag_t	*btag;
    tag_t	*rtag;
    long	 length;

    /* FIXME check for offset initializers */
    btag = tag->tag;
    for (length = 0; expr; expr = expr->next, length++) {
	rtag = data_tag(btag, expr);
	if (rtag->size > btag->size)
	    btag = rtag;
    }
    if (length * btag->size > tag->size)
	return (tag_vector(btag, length));
    return (tag);
}

static void
data_decl(expr_t *expr)
{
    for (; expr; expr = expr->next) {
	switch (expr->token) {
	    case tok_set:
		data_set(expr);
		break;
	    case tok_symbol:		case tok_vector:
	    case tok_pointer:
		break;
	    default:
		error(expr, "syntax error");
	}
    }
}

static void
data_set(expr_t *expr)
{
    expr_t	*lexp;
    expr_t	*rexp;
    char	*name;
    symbol_t	*symbol;
    void	*pointer;

    lexp = expr->data._binary.lvalue;
    rexp = expr->data._binary.rvalue;
    for (;;) {
	switch (lexp->token) {
	    case tok_symbol:
		name = lexp->data._unary.cp;
		if ((symbol = get_symbol(name)) == NULL) {
		    if (get_hash(functions, name))
			error(lexp, "not a lvalue");
		    error(lexp, "undefined symbol '%s'", name);
		}
		if (!symbol->glb)
		    error(lexp, "internal error");
		pointer = (char *)the_data + symbol->offset;
		data_init(pointer, symbol->tag, rexp);
		return;
	    case tok_vector:
		lexp = lexp->data._binary.lvalue;
		break;
	    case tok_pointer:
		lexp = lexp->data._unary.expr;
		break;
	    default:
		error(expr, "non constant initializer");
	}
    }
}

static void
data_init(void *pointer, tag_t *tag, expr_t *expr)
{
    union_t	u;

    u.v = pointer;
    switch (tag->type) {
	case type_char:			case type_uchar:
	    switch (expr->token) {
		case tok_int:	*u.c = expr->data._unary.i;	break;
		case tok_float:	*u.c = expr->data._unary.d;	break;
		default:	error(expr, "syntax error");
	    }
	    break;
	case type_short:		case type_ushort:
	    switch (expr->token) {
		case tok_int:	*u.s = expr->data._unary.i;	break;
		case tok_float:	*u.s = expr->data._unary.d;	break;
		default:	error(expr, "syntax error");
	    }
	    break;
	case type_int:			case type_uint:
	    switch (expr->token) {
		case tok_int:	*u.i = expr->data._unary.i;	break;
		case tok_float:	*u.i = expr->data._unary.d;	break;
		default:	error(expr, "syntax error");
	    }
	    break;
	case type_long:			case type_ulong:
	    switch (expr->token) {
		case tok_int:	*u.l = expr->data._unary.i;	break;
		case tok_float:	*u.l = expr->data._unary.d;	break;
		default:	error(expr, "syntax error");
	    }
	    break;
	case type_float:
	    switch (expr->token) {
		case tok_int:	*u.f = expr->data._unary.i;	break;
		case tok_float:	*u.f = expr->data._unary.d;	break;
		default:	error(expr, "syntax error");
	    }
	    break;
	case type_double:
	    switch (expr->token) {
		case tok_int:	*u.d = expr->data._unary.i;	break;
		case tok_float:	*u.d = expr->data._unary.d;	break;
		default:	error(expr, "syntax error");
	    }
	    break;
	case type_pointer|type_char:	case type_pointer|type_uchar:
	    break;
	default:
	    if (tag->type & type_pointer) {
		switch (expr->token) {
		    case tok_int:
			if (expr->data._unary.i != 0)
			    goto data_error;
			break;
		    case tok_string:
			if (tag->tag->type != type_char &&
			    tag->tag->type != type_uchar)
			    goto data_error;
			*u.p = rodata_string(expr, expr->data._unary.cp);
			break;
		    default:
			goto data_error;
		}
	    }
	    else if (tag->type & type_vector)
		data_vector(pointer, tag, expr);
	    else if (tag->type & (type_struct | type_union))
		data_record(pointer, tag, expr);
	    else {
	    data_error:
		error(expr, "invalid initializer");
	    }
	    break;
    }
}

static void
data_record(void *pointer, tag_t *tag, expr_t *expr)
{
    union_t	 u;
    expr_t	*prev;
    int		 offset;
    record_t	*record;
    symbol_t	*symbol;

    if (expr->token != tok_data || tag->tag)
	error(expr, "syntax error");
    expr = expr->data._unary.expr;
    /* type punned toplevel tag */
    record = tag->name;

    u.v = pointer;
    tag = tag->tag;
    /* use prev only to better locate error */
    for (prev = expr, offset = 0;
	 expr && offset < record->count;
	 offset++, prev = expr, expr = expr->next) {
	symbol = record->vector[offset];
	data_init(u.c + symbol->offset, symbol->tag, expr);
    }
    if (expr)
	error(prev, "too many initializers");
}

static void
data_vector(void *pointer, tag_t *tag, expr_t *expr)
{
    union_t	 u;
    expr_t	*base;
    expr_t	*prev;
    int		 offset;
    long	 length;

    u.v = pointer;
    length = tag->size;
    tag = tag->tag;
    length /= tag->size;

    if (expr->token != tok_data) {
	if (expr->token == tok_string &&
	    (tag->type == type_char || tag->type == type_uchar)) {
	    if (length > strlen(expr->data._unary.cp) + 1)
		error(expr, "too many initializers");
	    strcpy(pointer, expr->data._unary.cp);
	    return;
	}
	error(expr, "syntax error");
    }

    expr = expr->data._unary.expr;
    /* use prev only to better locate error */
    for (base = prev = expr, offset = 0;
	 expr && offset < length;
	 offset++, prev = expr, expr = expr->next)
	;
    if (expr)
	error(prev, "too many initializers");
    expr = base;
    switch (tag->type) {
	case type_char:			case type_uchar:
	    for (; expr; expr = expr->next, ++u.c) {
		switch (expr->token) {
		    case tok_int:	*u.c = expr->data._unary.i;	break;
		    case tok_float:	*u.c = expr->data._unary.d;	break;
		    default:		error(expr, "invalid initializer");
		}
	    }
	    break;
	case type_short:		case type_ushort:
	    for (; expr; expr = expr->next, ++u.s) {
		switch (expr->token) {
		    case tok_int:	*u.s = expr->data._unary.i;	break;
		    case tok_float:	*u.s = expr->data._unary.d;	break;
		    default:		error(expr, "invalid initializer");
		}
	    }
	    break;
	case type_int:			case type_uint:
	    for (; expr; expr = expr->next, ++u.i) {
		switch (expr->token) {
		    case tok_int:	*u.i = expr->data._unary.i;	break;
		    case tok_float:	*u.i = expr->data._unary.d;	break;
		    default:		error(expr, "invalid initializer");
		}
	    }
	    break;
	case type_long:			case type_ulong:
	    for (; expr; expr = expr->next, ++u.l) {
		switch (expr->token) {
		    case tok_int:	*u.l = expr->data._unary.i;	break;
		    case tok_float:	*u.l = expr->data._unary.d;	break;
		    default:		error(expr, "invalid initializer");
		}
	    }
	    break;
	case type_float:
	    for (; expr; expr = expr->next, ++u.f) {
		switch (expr->token) {
		    case tok_int:	*u.f = expr->data._unary.i;	break;
		    case tok_float:	*u.f = expr->data._unary.d;	break;
		    default:		error(expr, "invalid initializer");
		}
	    }
	    break;
	case type_double:
	    for (; expr; expr = expr->next, ++u.d) {
		switch (expr->token) {
		    case tok_int:	*u.d = expr->data._unary.i;	break;
		    case tok_float:	*u.d = expr->data._unary.d;	break;
		    default:		error(expr, "invalid initializer");
		}
	    }
	default:
	    for (; expr; expr = expr->next, u.c += tag->size)
		data_init(u.p, tag, expr);
	    break;
    }
}

static void
init_rodata(void)
{
    entry_t	*entry;
    entry_t	*rodata;
    int		 offset;
    long	 length;
    char	*pointer;

    data_hash = new_hash(hash_pointer);
    for (offset = 0, length = 0; offset < strings->size; offset++) {
	for (entry = strings->entries[offset]; entry; entry = entry->next) {
	    rodata = xcalloc(1, sizeof(entry_t));
	    rodata->name.string = entry->name.string;
	    rodata->value = (void *)length;
	    length += strlen(entry->name.string) + 1;
	    put_hash(data_hash, rodata);
	}
    }

    the_rodata = xmalloc(rodata_length = length);
    pointer = the_rodata;
    for (offset = 0; offset < strings->size; offset++) {
	for (entry = data_hash->entries[offset]; entry; entry = entry->next) {
	    entry->value = pointer + (long)entry->value;
	    strcpy(entry->value, entry->name.string);
	}
    }
}

static char *
rodata_string(expr_t *expr, char *string)
{
    entry_t	*entry;

    if ((entry = get_hash(data_hash, string)) == NULL)
	error(expr, "internal error");

    return (entry->value);
}
