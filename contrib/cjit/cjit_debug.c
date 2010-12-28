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
dump_data(void *pointer, tag_t *tag);

static void
dump_record(void *pointer, tag_t *tag);

static void
dump_vector(void *pointer, tag_t *tag);

static void
print_unary(char *string, expr_t *expr);

static void
print_binary(char *string, expr_t *expr);

static void
print_comma_list(expr_t *expr);

static void
print_paren_comma_list(expr_t *expr);

static void
print_fields(record_t *record);

static void
print_tag(tag_t *tag);

static void
print_expr(expr_t *expr);

/*
 * Implementation
 */
void
print(expr_t *expr)
{
    for (; expr;) {
	print_expr(expr);
	if ((expr = expr->next))
	    putchar(' ');
    }
}

void
dump(void)
{
    int			 offset;
    symbol_t		*symbol;
    unsigned char	*rodata;

    for (offset = 0; offset < globals->count; offset++) {
	symbol = globals->vector[offset];
	printf("%s%s: ", offset ? ", " : "", symbol->name);
	dump_data((char *)the_data + symbol->offset, symbol->tag);
    }
    if (rodata_length) {
	rodata = (unsigned char *)the_rodata;
	printf("\nrodata:");
	for (offset = 0; offset < rodata_length; offset++, rodata++)
	    printf(" %02u", *rodata);
    }
}

static void
dump_data(void *pointer, tag_t *tag)
{
    union_t	u;

    u.v = pointer;
    switch (tag->type) {
	case type_char:		printf("%d",  *u.c);	break;
	case type_uchar:	printf("%x",  *u.uc);	break;
	case type_short:	printf("%d",  *u.s);	break;
	case type_ushort:	printf("%x",  *u.us);	break;
	case type_int:		printf("%d",  *u.i);	break;
	case type_uint:		printf("%x",  *u.ui);	break;
	case type_long:		printf("%ld", *u.l);	break;
	case type_ulong:	printf("%lx", *u.ul);	break;
	case type_float:	printf("%f",  *u.f);	break;
	case type_double:	printf("%f",  *u.d);	break;
	default:
	    if (tag->type & type_pointer)
		printf("%p", u.p);
	    else if (tag->type & type_vector)
		dump_vector(pointer, tag);
	    else
		dump_record(pointer, tag);
	    break;
    }
}

static void
dump_record(void *pointer, tag_t *tag)
{
    union_t	 u;
    int		 offset;
    record_t	*record;
    symbol_t	*symbol;

    u.v = pointer;
    record = tag->name;
    printf("{ ");
    if (record->count) {
	symbol = record->vector[0];
	dump_data(pointer, symbol->tag);
	for (offset = 1; offset < record->count; offset++) {
	    symbol = record->vector[offset];
	    printf(", ");
	    dump_data(u.c + symbol->offset, symbol->tag);
	}
	printf(" }");
    }
    else
	putchar('}');
}

static void
dump_vector(void *pointer, tag_t *tag)
{
    union_t	u;
    long	length;

    u.v = pointer;
    length = tag->size;
    tag = tag->tag;
    length /= tag->size;
    printf("{ ");
    if (--length >= 0) {
	switch (tag->type) {
	    case type_char:
		printf("%d", *u.c);
		for (u.c++; length; length--, u.c++)	printf(", %d", *u.c);
		break;
	    case type_uchar:
		printf("%x", *u.uc);
		for (u.uc++; length; length--, u.uc++)	printf(", %x", *u.uc);
		break;
	    case type_short:
		printf("%d", *u.s);
		for (u.s++; length; length--, u.s++)	printf(", %d", *u.s);
		break;
	    case type_ushort:
		printf("%x", *u.us);
		for (u.s++; length; length--, u.us++)	printf(", %x", *u.us);
		break;
	    case type_int:
		printf("%d", *u.i);
		for (u.i++; length; length--, u.i++)	printf(", %d", *u.i);
		break;
	    case type_uint:
		printf("%x", *u.ui);
		for (u.ui++; length; length--, u.ui++)	printf(", %x", *u.ui);
		break;
	    case type_long:
		printf("%ld", *u.l);
		for (u.l++; length; length--, u.l++)	printf(", %ld", *u.l);
		break;
	    case type_ulong:
		printf("%lx", *u.ul);
		for (u.ul++; length; length--, u.ul++)	printf(", %lx", *u.ul);
		break;
	    case type_float:
		printf("%f", *u.f);
		for (u.f++; length; length--, u.f++)	printf(", %f", *u.f);
		break;
	    case type_double:
		printf("%f", *u.d);
		for (u.d++; length; length--, u.d++)	printf(", %f", *u.d);
		break;
	    default:
		dump_data(pointer, tag);
		for (u.c += tag->size; length; length--, u.c += tag->size) {
		    printf(", ");
		    dump_data(u.v, tag);
		}
		break;
	}
	printf(" }");
    }
    else
	putchar('}');
}

static void
print_unary(char *string, expr_t *expr)
{
    printf("%s ", string);
    print_expr(expr->data._unary.expr);
}

static void
print_binary(char *string, expr_t *expr)
{
    print_expr(expr->data._binary.lvalue);
    printf(" %s ", string);
    print_expr(expr->data._binary.rvalue);
}

static void
print_comma_list(expr_t *expr)
{
    print_expr(expr);
    if (expr) {
	for (expr = expr->next; expr; expr = expr->next) {
	    printf(" , ");
	    print_expr(expr);
	}
    }
}

static void
print_paren_comma_list(expr_t *expr)
{
    if (expr) {
	printf("( ");
	print_comma_list(expr);
	printf(" )");
    }
    else
	printf("( )");
}


static void
print_fields(record_t *record)
{
    int		 offset;
    symbol_t	*symbol;

    printf(" { ");
    for (offset = 0; offset < record->count; offset++) {
	symbol = record->vector[offset];
	print_tag(symbol->tag);
	printf(" %s ; ", symbol->name);
    }
    putchar('}');
}

static void
print_tag(tag_t *tag)
{
    tag_t	*base;
    record_t	*record;

    if (tag->type & type_unsigned)
	printf("unsigned");
    else if (tag->type & type_struct)
	printf("struct");
    else if (tag->type & type_union)
	printf("union");
    for (base = tag; base->tag; base = base->tag)
	;
    record = base->name;
    if (record->name) {
	if (tag->type & (type_unsigned | type_struct | type_union))
	    putchar(' ');
	printf("%s", record->name->name.string);
    }
    if ((tag->type & type_struct) || (tag->type & type_union))
	print_fields(record);
    if (base != tag) {
	for (; tag != base; tag = tag->tag)
	    printf(" %s", (char *)tag->name);
    }
}

static void
print_expr(expr_t *expr)
{
    if (expr == NULL)
	return;
    switch (expr->token) {
	case tok_int:
	    printf("%ld", expr->data._unary.i);
	    break;
	case tok_float:
	    printf("%f", expr->data._unary.d);
	    break;
	case tok_string:
	    printf("\"%s\"", expr->data._unary.cp);
	    break;
	case tok_symbol:
	    printf("%s", expr->data._unary.cp);
	    break;
	case tok_type:
	    print_tag(expr->data._unary.vp);
	    break;
	case tok_declexpr:
	    print_expr(expr->data._binary.lvalue);
	    putchar(' ');
	    print_expr(expr->data._binary.rvalue);
	    break;
	case tok_decl:
	    print_expr(expr->data._binary.lvalue);
	    putchar(' ');
	    print_comma_list(expr->data._binary.rvalue);
	    printf(" ;");
	    break;
	case tok_oparen:
	    putchar('(');
	    break;
	case tok_cparen:
	    putchar(')');
	    break;
	case tok_obrack:
	    putchar('[');
	    break;
	case tok_cbrack:
	    putchar(']');
	    break;
	case tok_obrace:
	    putchar('{');
	    break;
	case tok_cbrace:
	    putchar('}');
	    break;
	case tok_collon:
	    putchar(':');
	    break;
	case tok_semicollon:
	    putchar(';');
	    break;
	case tok_comma:
	    putchar(',');
	    break;
	case tok_dot:		print_binary(".", expr);	break;
	case tok_arrow:		print_binary("->", expr);	break;
	case tok_ellipsis:	printf("...");			break;
	case tok_set:		print_binary("=", expr);	break;
	case tok_andset:	print_binary("&=", expr);	break;
	case tok_orset:		print_binary("|=", expr);	break;
	case tok_xorset:	print_binary("^=", expr);	break;
	case tok_lshset:	print_binary("<<=", expr);	break;
	case tok_rshset:	print_binary(">>=", expr);	break;
	case tok_addset:	print_binary("+=", expr);	break;
	case tok_subset:	print_binary("-=", expr);	break;
	case tok_mulset:	print_binary("*=", expr);	break;
	case tok_divset:	print_binary("/=", expr);	break;
	case tok_remset:	print_binary("%=", expr);	break;
	case tok_andand:	print_binary("&&", expr);	break;
	case tok_oror:		print_binary("||", expr);	break;
	case tok_lt:		print_binary("<", expr);	break;
	case tok_le:		print_binary("<=", expr);	break;
	case tok_eq:		print_binary("==", expr);	break;
	case tok_ge:		print_binary(">=", expr);	break;
	case tok_gt:		print_binary(">", expr);	break;
	case tok_ne:		print_binary("!=", expr);	break;
	case tok_and:		print_binary("&", expr);	break;
	case tok_or:		print_binary("|", expr);	break;
	case tok_xor:		print_binary("^", expr);	break;
	case tok_lsh:		print_binary("<<", expr);	break;
	case tok_rsh:		print_binary(">>", expr);	break;
	case tok_add:		print_binary("+", expr);	break;
	case tok_sub:		print_binary("-", expr);	break;
	case tok_mul:		print_binary("*", expr);	break;
	case tok_div:		print_binary("/", expr);	break;
	case tok_rem:		print_binary("%", expr);	break;
	case tok_inc:		print_unary("++", expr);	break;
	case tok_dec:		print_unary("--", expr);	break;
	case tok_postinc:
	    print_expr(expr->data._unary.expr);
	    printf(" ++");
	    break;
	case tok_postdec:
	    print_expr(expr->data._unary.expr);
	    printf(" --");
	    break;
	case tok_plus:		print_unary("+", expr);		break;
	case tok_neg:		print_unary("-", expr);		break;
	case tok_not:		print_unary("!", expr);		break;
	case tok_com:		print_unary("~", expr);		break;
	case tok_pointer:	print_unary("*", expr);		break;
	case tok_address:	print_unary("&", expr);		break;
	case tok_sizeof:
	    printf("sizeof ( ");
	    print(expr->data._unary.expr);
	    printf(" )");
	    break;
	case tok_goto:
	    print_unary("goto", expr);
	    printf(" ;");
	    break;
	case tok_question:
	    print_expr(expr->data._if.test);
	    printf(" ? ");
	    print_expr(expr->data._if.tcode);
	    printf(" : ");
	    print_expr(expr->data._if.fcode);
	    break;
	case tok_return:
	    print_unary("return", expr);
	    printf(" ;");
	    break;
	case tok_break:		printf("break ;");		break;
	case tok_continue:	printf("continue ;");		break;
	case tok_struct:	printf("struct");		break;
	case tok_union:		printf("union");		break;
	case tok_typedef:	printf("typedef");		break;
	case tok_list:
	    print_paren_comma_list(expr->data._unary.expr);
	    break;
	case tok_vector:
	    print_expr(expr->data._binary.lvalue);
	    printf(" [ ");
	    if (expr->data._binary.rvalue) {
		print_expr(expr->data._binary.rvalue);
		printf(" ]");
	    }
	    else
		putchar(']');
	    break;
	case tok_stat:
	    print_comma_list(expr->data._unary.expr);
	    printf(" ;");
	    break;
	case tok_call:
	    print_expr(expr->data._binary.lvalue);
	    putchar(' ');
	    print_paren_comma_list(expr->data._binary.rvalue);
	    break;
	case tok_code:
	    if (expr->data._unary.expr) {
		printf("{ ");
		print(expr->data._unary.expr);
		printf(" }");
	    }
	    else
		printf("{ }");
	    break;
	case tok_data:
	    if (expr->data._unary.expr) {
		printf("{ ");
		print_comma_list(expr->data._unary.expr);
		printf(" }");
	    }
	    else
		printf("{ }");
	    break;
	case tok_if:
	    printf("if ");
	    print_paren_comma_list(expr->data._if.test);
	    putchar(' ');
	    print_expr(expr->data._if.tcode);
	    if (expr->data._if.fcode) {
		printf(" else ");
		print_expr(expr->data._if.fcode);
	    }
	    break;
	case tok_while:
	    printf("while ");
	    print_paren_comma_list(expr->data._while.test);
	    putchar(' ');
	    print_expr(expr->data._while.code);
	    break;
	case tok_for:
	    printf("for ( ");
	    print_comma_list(expr->data._for.init);
	    printf(" ; ");
	    print_comma_list(expr->data._for.test);
	    printf(" ; ");
	    print_comma_list(expr->data._for.incr);
	    printf(" ) ");
	    print_expr(expr->data._for.code);
	    break;
	case tok_do:
	    printf("do ");
	    print_expr(expr->data._do.code);
	    printf(" while ");
	    print_paren_comma_list(expr->data._do.test);
	    printf(" ;");
	    break;
	case tok_label:
	    print_expr(expr->data._unary.expr);
	    printf(" :");
	    break;
	case tok_switch:
	    printf("switch ");
	    print_paren_comma_list(expr->data._switch.test);
	    putchar(' ');
	    print_expr(expr->data._switch.code);
	    break;
	case tok_case:
	    printf("case %ld :", expr->data._unary.i);
	    break;
	case tok_default:
	    printf("default :");
	    break;
	case tok_elemref:
	    printf("[ ");
	    print(expr->data._unary.expr);
	    printf(" ]");
	    break;
	case tok_fieldref:
	    printf(". ");
	    print(expr->data._unary.expr);
	    break;
	case tok_function:
	    print_expr(expr->data._function.type);
	    putchar(' ');
	    print_expr(expr->data._function.call);
	    putchar(' ');
	    print_expr(expr->data._function.body);
	    break;
	default:
	    warn(expr, "cannot print %d\n", expr->token);
	    break;
    }
}
