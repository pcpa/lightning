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
static tag_t *tag_proto(tag_t *tag, expr_t *list);

/*
 * Initialization
 */
static record_t	**types;
int		  num_types;
static hash_t	 *stags;
static hash_t	 *utags;
static hash_t	 *table;
tag_t		 *void_tag;
tag_t		 *int_tag;
tag_t		 *uint_tag;
tag_t		 *long_tag;
tag_t		 *ulong_tag;
tag_t		 *float_tag;
tag_t		 *double_tag;
void		 *pointer_string;

/*
 * Implementation
 */
void
init_type(void)
{
    int		 offset;
    record_t	*record;
    keyword_t	*keyword;

    static struct {
	char	*name;
	int	 size;
    } type_init[] = {
	{ "void",	0		},
	{ "char",	1		},
	{ "short",	2		},
	{ "int",	4		},
	{ "long",	sizeof(long)	},
	{ "float",	sizeof(float)	},
	{ "double",	sizeof(double)	},
    };

    table = new_hash(hash_pointer);
    types = (record_t **)xcalloc(16, sizeof(record_t *));
    for (offset = 0;
	 offset < sizeof(type_init) / sizeof(type_init[0]);
	 offset++) {
	keyword = xcalloc(1, sizeof(keyword_t));
	keyword->name = xintern(type_init[offset].name);
	keyword->token = tok_type;
	put_hash(keywords, (entry_t *)keyword);
	record = get_type(keyword->name, 0);
	keyword->value = record;
	record->length = type_init[offset].size;
    }
    stags = new_hash(hash_pointer);
    utags = new_hash(hash_pointer);

    pointer_string = xintern("*");
    void_tag = tag_type(type_void);
    int_tag = tag_type(type_int);
    uint_tag = tag_type(type_uint);
    long_tag = tag_type(type_long);
    ulong_tag = tag_type(type_ulong);
    float_tag = tag_type(type_float);
    double_tag = tag_type(type_double);
}

record_t *
new_record(int mask)
{
    record_t	*record;

    record = xcalloc(1, sizeof(record_t));
    record->entries = (symbol_t **)xcalloc(record->size = 32, sizeof(void *));
    record->kind = hash_pointer;
    record->type = mask;

    return (record);
}

void
end_record(record_t *record)
{
    record->length = (record->offset + DEFAULT_ALIGN - 1) & -DEFAULT_ALIGN;
}

record_t *
get_type(char *name, int mask)
{
    entry_t	*entry;
    record_t	*record;

    if (name && (entry = get_hash(table, name))) {
	record = entry->value;
	if (type_mask(record->type) ^ mask)
	    error(NULL, "%s redeclared as a different object", name);
	return (record);
    }

    if ((num_types & 15) == 0)
	types = (record_t **)
	    xrealloc(types, sizeof(record_t *) * (num_types + 17));
    record = new_record(mask);
    record->type |= num_types;
    if (name) {
	entry = xmalloc(sizeof(entry_t));
	entry->name.string = name;
	put_hash(table, entry);
	entry->value = record;
	record->name = entry;
    }
    types[num_types] = record;
    ++num_types;

    return (record);
}

tag_t *
tag_type(int type)
{
    tag_t	*tag;
    hash_t	*hash;
    int		 offset;
    record_t	*record;

    hash = type & type_unsigned ? utags : stags;
    offset = type_base(type);
    assert((unsigned)offset < num_types);
    record = types[offset];
    if ((tag = (tag_t *)get_hash(hash, record)) == NULL) {
	tag = (tag_t *)xmalloc(sizeof(tag_t));
	tag->name = record;
	put_hash(hash, (entry_t *)tag);
	tag->hash = new_hash(hash_pointer);
	tag->type = type;
	tag->size = record->length;
	tag->tag  = NULL;
    }

    return (tag);
}

tag_t *
tag_pointer(tag_t *tag)
{
    hash_t	*hash;
    int		 type;
    tag_t	*entry;

    hash = tag->hash;
    type = tag->type;
    if ((entry = (tag_t *)get_hash(hash, pointer_string)) == NULL) {
	entry = (tag_t *)xmalloc(sizeof(tag_t));
	entry->name = pointer_string;
	put_hash(hash, (entry_t *)entry);
	entry->hash = new_hash(hash_pointer);
	entry->type = type_pointer | type;
	entry->size = sizeof(void *);
	entry->tag  = tag;
    }

    return (entry);
}

tag_t *
tag_vector(tag_t *tag, long length)
{
    hash_t	*hash;
    char	*name;
    int		 type;
    tag_t	*entry;
    char	 stk[32];

    /* use a string just in case the declared length would match
     * some pointer, otherwise, could cast it to a pointer, because
     * it is a "hash_pointer" */
    sprintf(stk, "%ld", length);
    name = xintern(stk);
    hash = tag->hash;
    type = tag->type;
    if ((entry = (tag_t *)get_hash(hash, name)) == NULL) {
	entry = (tag_t *)xmalloc(sizeof(tag_t));
	entry->name = name;
	put_hash(hash, (entry_t *)entry);
	entry->hash = new_hash(hash_pointer);
	entry->type = type_vector | type;
	entry->size = tag->size * length;
	entry->tag  = tag;
    }

    return (entry);
}

tag_t *
tag_decl(tag_t *tag, expr_t **decl)
{
    expr_t	*expr;
    long	 length;

    if ((expr = *decl) == NULL)
	return (tag);
    for (;;) {
	switch (expr->token) {
	    case tok_pointer:
		tag = tag_pointer(tag);
		if ((expr = expr->data._unary.expr) == NULL) {
		    *decl = expr;
		    return (tag);
		}
		break;
	    case tok_vector:
		/* handle specially unspecified length */
		if (expr->data._binary.rvalue == NULL)
		    length = 0;
		else if (expr->data._binary.rvalue->token != tok_int)
		    error(expr, "not an integer");
		else {
		    length = expr->data._binary.rvalue->data._unary.i;
		    if (length < 0)
			error(expr, "negative length");
		}
		tag = tag_vector(tag, length);
		if ((expr = expr->data._binary.lvalue) == NULL) {
		    *decl = expr;
		    return (tag);
		}
		break;
	    case tok_call:
		tag = tag_proto(tag, expr->data._binary.rvalue);
	    case tok_symbol:
		*decl = expr;
		return (tag);
		break;
	    default:
		error(expr, "syntax error");
	}
    }
}

symbol_t *
get_symbol(char *name)
{
    symbol_t	*symbol;

    symbol = (symbol_t *)get_hash((hash_t *)current, name);
    if (symbol == NULL && current != globals)
	symbol = (symbol_t *)get_hash((hash_t *)globals, name);

    return (symbol);
}

symbol_t *
get_symbol_lvalue(expr_t *expr)
{
    symbol_t	*symbol = get_symbol(expr->data._unary.cp);

    if (symbol == NULL) {
	if (get_hash(functions, expr->data._unary.cp))
	    error(expr, "not a lvalue");
	error(expr, "undefined symbol '%s'", expr->data._unary.cp);
    }

    return (symbol);
}

symbol_t *
new_symbol(record_t *record, tag_t *tag, char *name)
{
    record_t	*type;
    symbol_t	*symbol;

    if ((symbol = (symbol_t *)get_hash((hash_t *)record, name))) {
	if (record != globals)
	    error(NULL, "'%s' redeclared", name);
	if (symbol->tag != tag)
	    error(NULL, "'%s' redeclared as a different type", name);
    }

    type = types[type_base(tag->type)];
    symbol = xmalloc(sizeof(symbol_t));
    symbol->name = name;
    put_hash((hash_t *)record, (entry_t *)symbol);
    symbol->tag = tag;
    symbol->type = tag->type;
    symbol->table = record;

    if (type->length == 0 && tag->type != (type_pointer | type_void))
	/* open type declaration */
	error(NULL, "syntax error");
    if (record->type & type_union) {
	symbol->offset = 0;
	if (record->offset < type->length)
	    record->offset = type->length;
    }
    else {
	switch (tag->size) {
	    case 1:
		symbol->offset = record->offset;
		break;
	    case 2:
		symbol->offset = (record->offset + 1) & ~1;
		break;
	    case 4:
		symbol->offset = (record->offset + 3) & ~3;
		break;
	    default:
		if (record->type == type_double)
		    symbol->offset = (record->offset + (DOUBLE_ALIGN - 1)) &
				      -DOUBLE_ALIGN;
		else
		    symbol->offset = (record->offset + (DEFAULT_ALIGN - 1)) &
				      -DEFAULT_ALIGN;
		break;
	}
	record->offset = symbol->offset + tag->size;
    }
    if (record->vector == NULL)
	record->vector = (symbol_t **)xmalloc(sizeof(symbol_t *));
    else
	record->vector =
		(symbol_t **)xrealloc(record->vector,
				      sizeof(symbol_t *) * record->count);

    return (record->vector[record->count - 1] = symbol);
}

static tag_t *
tag_proto(tag_t *tag, expr_t *list)
{
    expr_t	*type;
    expr_t	*expr;
    char	*name;
    char	*temp;
    int		 count;
    tag_t	*entry;
    char	 stk[1024];

    for (expr = list, count = 1; expr; expr = expr->next, count++)
	;
    count = (count * (__WORDSIZE >> 3)) + count * 2 + 1;
    if (count >= sizeof(stk))
	name = (char *)xmalloc(count);
    else
	name = stk;
    count = sprintf(name, "%p", tag);

    for (; list; list = list->next) {
	if (list->token == tok_type) {
	    type = list;
	    expr = NULL;
	}
	else {
	    if (list->token != tok_declexpr)
		error(list, "syntax error");
	    type = list->data._binary.lvalue;
	    expr = list->data._binary.rvalue;
	    if (type->token != tok_type)
		error(type, "internal error");
	}
	entry = tag_decl(type->data._unary.vp, &expr);
	count += sprintf(name + count, "%p", entry);
    }
    name[count] = '\0';
    temp = xintern(name);
    if (name != stk)
	free(name);
    name = temp;
    if ((entry = (tag_t *)get_hash(tag->hash, name)) == NULL) {
	entry = (tag_t *)xmalloc(sizeof(tag_t));
	entry->name = name;
	put_hash(tag->hash, (entry_t *)entry);
	entry->hash = new_hash(hash_pointer);
	entry->type = type_function | tag->type;
	entry->size = sizeof(void *);
	entry->tag  = tag;
    }

    return (entry);
}
