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
static token_t
statement_noeof(void);

static token_t
statement(void);

static void
statement_paren_comma_list(void);

static expr_t *
get_block(token_t token);

static symbol_t *
declare(expr_t *type, expr_t *decl);

static token_t
declaration(void);

static function_t *
prototype(tag_t *tag, expr_t *decl);

static void
function(void);

static token_t
group(int data);

static token_t
precedence_noeof(void);

static token_t
precedence(void);

static token_t
expression_noeof(void);

static token_t
expression(void);

static token_t
binary_assign(void);

static token_t
binary_left(token_t token);

static void
binary_right(token_t token);

static token_t
unary_noeof(void);

static token_t
unary(void);

static token_t
unary_loop(token_t token);

static token_t
unary_value(token_t token);

static token_t
unary_type(void);

static token_t
unary_record(token_t token);

static token_t
unary_decl(void);

static token_t
unary_list(void);

static token_t
unary_vector(void);

static token_t
unary_field(void);

/*
 * Initialization
 */
parser_t	 parser;
expr_t		*ablock;
hash_t		*keywords;
record_t	*current;
record_t	*globals;
hash_t		*functions;
static expr_t	*aexpr;
static expr_t	*atail;

/*
 * Implementation
 */
void
init_parser(void)
{
    int		 offset;
    keyword_t	*keyword;
    static struct {
	char	*name;
	token_t	 token;
    } keyword_init[] = {
	{ "if",		tok_if		},
	{ "else",	tok_else	},
	{ "goto",	tok_goto	},
	{ "return",	tok_return	},
	{ "switch",	tok_switch	},
	{ "case",	tok_case	},
	{ "default",	tok_default	},
	{ "break",	tok_break	},
	{ "for", 	tok_for		},
	{ "do",		tok_do		},
	{ "while",	tok_while	},
	{ "continue",	tok_continue	},
	{ "signed",	tok_signed	},
	{ "unsigned",	tok_unsigned	},
	{ "struct",	tok_struct	},
	{ "union",	tok_union	},
	{ "sizeof",	tok_sizeof	},
	{ "typedef",	tok_typedef	},
    };

    parser.fp = stdin;
    strcpy(parser.name, "<stdin>");
    parser.lineno = parser.column = 1;
    parser.string = (char *)xmalloc(parser.length = 4096);

    strings = new_hash(hash_string);
    keywords = new_hash(hash_string);

    for (offset = 0;
	 offset < sizeof(keyword_init) / sizeof(keyword_init[0]);
	 offset++) {
	keyword = xcalloc(1, sizeof(keyword_t));
	keyword->name = xstrdup(keyword_init[offset].name);
	keyword->token = keyword_init[offset].token;
	put_hash(keywords, (entry_t *)keyword);
    }

    globals = new_record(type_namespace);
    functions = new_hash(hash_pointer);

    current = globals;
}

expr_t *
parse(void)
{
    expr_t	*expr;

    for (;;) {
	if (statement() == tok_eof) {
	    if (estack.offset)
		error(atail, "syntax error");
	    return (aexpr);
	}
	expr = pop_expr();
	if (aexpr == NULL)
	    aexpr = atail = expr;
	else {
	    atail->next = expr;
	    atail = expr;
	}
    }
}

static token_t
statement_noeof(void)
{
    token_t	token = statement();

    if (token == tok_eof)
	error(NULL, "unexpected end of file");

    return (token);
}

static token_t
statement(void)
{
    token_t	 token;
    entry_t	*entry;
    expr_t	*expr, *block;

    token = lookahead();
    switch (token) {
	case tok_eof:
	    return (token);
	case tok_obrace:
	    /* brace is code, not data */
	    primary();
	    break;
	default:
	    token = expression();
	    break;
    }
    expr = top_expr();
    switch (token) {
	case tok_label:
	    break;
	case tok_decl:
	    token = declaration();
	    break;
	case tok_int:		case tok_float:
	case tok_symbol: 	case tok_expr:
	case tok_defn:
	    token = tok_stat;
	    expr = new_expr(tok_stat, top_expr()->lineno, top_expr()->column);
	    expr->data._unary.expr = top_expr();
	    top_expr() = expr;
	    switch (lookahead()) {
		case tok_comma:
		    consume();
		    if (group(0) != tok_semicollon)
			error(expr, "syntax error");
		    expr->next = pop_expr();
		case tok_semicollon:
		    consume();
		    break;
		default:
		    error(expr, "syntax error");
	    }
	    break;
	case tok_obrace:
	    token = expr->token = tok_code;
	    expr = NULL;
	    while (lookahead() != tok_cbrace) {
		statement_noeof();
		if (expr == NULL) {
		    expr = pop_expr();
		    top_expr()->data._unary.expr = expr;
		    expr = top_expr()->data._unary.expr;
		}
		else {
		    expr->next = pop_expr();
		    expr = expr->next;
		}
	    }
	    if (expr == NULL)
		top_expr()->data._unary.expr = NULL;
	    consume();
	    break;
	case tok_semicollon:
	    expr->token = token = tok_stat;
	    break;
	case tok_if:
	    (void)get_block(tok_if);
	    statement_paren_comma_list();
	    expr->data._if.test = pop_expr();
	    statement_noeof();
	    expr->data._if.tcode = pop_expr();
	    if (lookahead() == tok_else) {
		consume();
		statement_noeof();
		expr->data._if.fcode = pop_expr();
	    }
	    break;
	case tok_while:
	    (void)get_block(tok_while);
	    statement_paren_comma_list();
	    expr->data._while.test = pop_expr();
	    expr->block = ablock;
	    ablock = expr;
	    statement_noeof();
	    ablock = expr->block;
	    expr->data._while.code = pop_expr();
	    break;
	case tok_for:
	    (void)get_block(tok_for);
	    if (primary() != tok_oparen)
		error(top_expr(), "syntax error");
	    discard();
	    if (group(0) != tok_semicollon)
		error(ahead, "syntax error");
	    consume();
	    expr->data._for.init = pop_expr();
	    if (group(0) != tok_semicollon)
		error(ahead, "syntax error");
	    consume();
	    expr->data._for.test = pop_expr();
	    if (group(0) != tok_cparen)
		error(ahead, "syntax error");
	    consume();
	    expr->data._for.incr = pop_expr();
	    expr->block = ablock;
	    ablock = expr;
	    statement_noeof();
	    ablock = expr->block;
	    expr->data._for.code = pop_expr();
	    break;
	case tok_do:
	    (void)get_block(tok_do);
	    expr->block = ablock;
	    ablock = expr;
	    statement_noeof();
	    ablock = expr->block;
	    expr->data._binary.lvalue = pop_expr();
	    if (primary() != tok_while)
		error(top_expr(), "syntax error");
	    discard();
	    statement_paren_comma_list();
	    expr->data._binary.rvalue = pop_expr();
	    if (primary() != tok_semicollon)
		error(top_expr(), "syntax error");
	    discard();
	    break;
	case tok_switch:
	    (void)get_block(tok_switch);
	    statement_paren_comma_list();
	    expr->data._switch.test = pop_expr();
	    if (lookahead() != tok_obrace)
		error(ahead, "syntax error");
	    expr->data._switch.hash = new_hash(hash_integer);
	    expr->block = ablock;
	    ablock = expr;
	    statement_noeof();
	    ablock = expr->block;
	    expr->data._switch.code = pop_expr();
	    break;
	case tok_case:
	    if (primary() != tok_int) {
		(void)eval(top_expr());
		if (top_expr()->token != tok_int)
		    error(top_expr(), "case value not an integer");
	    }
	    expr->data._binary.lvalue = top_expr()->data._binary.lvalue;
	    discard();
	    if (primary() != tok_collon)
		error(top_expr(), "syntax error");
	    discard();
	    block = get_block(tok_switch);
	    if (get_hash(block->data._switch.hash, (void *)expr->data._unary.i))
		error(NULL, "duplicated case %ld", expr->data._unary.i);
	    entry = xcalloc(1, sizeof(entry_t));
	    entry->name.integer = expr->data._unary.i;
	    put_hash(block->data._switch.hash, entry);
	    expr->block = block;
	    break;
	case tok_default:
	    if (primary() != tok_collon)
		error(top_expr(), "syntax error");
	    discard();
	    block = get_block(tok_default);
	    if (block->data._switch._default)
		error(NULL, "duplicated default");
	    block->data._switch._default = expr;
	    expr->block = block;
	    break;
	case tok_break:
	    block = get_block(tok_break);
	    expr->block = block;
	    if (primary_noeof() != tok_semicollon)
		error(top_expr(), "expecting ;");
	    discard();
	    break;
	case tok_continue:
	    block = get_block(tok_continue);
	    if (primary_noeof() != tok_semicollon)
		error(top_expr(), "expecting ;");
	    discard();
	    expr->block = block;
	    break;
	case tok_goto:
	    (void)get_block(tok_goto);
	    if (unary_noeof() != tok_symbol)
		error(top_expr(), "expecting label");
	    expr->data._unary.expr = pop_expr();
	    if (primary_noeof() != tok_semicollon)
		error(top_expr(), "expecting ;");
	    discard();
	    break;
	case tok_return:
	    (void)get_block(tok_return);
	    if (lookahead() != tok_semicollon) {
		expression_noeof();
		expr->data._unary.expr = pop_expr();
	    }
	    if (primary_noeof() != tok_semicollon)
		error(top_expr(), "expecting ;");
	    discard();
	    break;
	default:
	    error(expr, "syntax error");
    }

    return (token);
}

static void
statement_paren_comma_list(void)
{
    if (primary() != tok_oparen)
	error(top_expr(), "syntax error");
    discard();
    if (group(0) != tok_cparen)
	error(ahead, "syntax error");
    consume();
    if (top_expr() == NULL)
	error(NULL, "syntax error");
}

static expr_t *
get_block(token_t token)
{
    expr_t	*block;

    for (block = ablock; block; block = block->block) {
	switch (token) {
	    case tok_case:		case tok_default:
		if (block->token == tok_switch)
		    return (block);
		break;
	    case tok_break:
		switch (block->token) {
		    case tok_for:	case tok_while:
		    case tok_switch:	case tok_do:
			return (block);
		    default:
			break;
		}
		break;
	    case tok_continue:
		switch (block->token) {
		    case tok_for:	case tok_while:
		    case tok_do:
			return (block);
		    default:
			break;
		}
		break;
	    default:
		if (block->token == tok_function)
		    return (block);
		break;
	}
    }
    error(NULL, "syntax error");
}

static symbol_t *
declare(expr_t *type, expr_t *decl)
{
    tag_t	*tag;
    expr_t	*vexp;
    symbol_t	*symbol;

    if (decl == NULL)
	error(NULL, "syntax error");
    vexp = NULL;
    while (decl->token == tok_set) {
	vexp = decl->data._binary.rvalue;
	decl = decl->data._binary.lvalue;
    }
    tag = tag_decl(type->data._unary.vp, &decl);
    if (decl->token == tok_call) {
	(void)prototype(tag, decl);
	return (NULL);
    }
    if (decl->token != tok_symbol)
	error(decl, "internal error");
    if (tag->size == 0) {
	if (type_mask(tag->type) != type_vector)
	    error(decl, "syntax error");
	/* patch tag size information of unspecified, but initialized vector */
	if (vexp)
	    tag = data_tag(tag, vexp);
    }
    if ((symbol = new_symbol(current, tag, decl->data._unary.cp)) == NULL)
	error(decl, "syntax error");

    if (current == globals)
	symbol->glb = 1;
    else if (current->type & type_namespace)
	symbol->loc = 1;
    else if (current->type & (type_struct | type_union))
	symbol->fld = 1;

    return (symbol);
}

static token_t
declaration(void)
{
    expr_t	*type;
    expr_t	*decl;

    type = top_expr();
    assert(type->token == tok_declexpr);
    type->token = tok_decl;
    decl = type->data._binary.rvalue;
    type = type->data._binary.lvalue;
    assert(type->token == tok_type);
    switch (lookahead()) {
	case tok_comma:
	    consume();
	    if (group(0) != tok_semicollon)
		error(ahead, "syntax error");
	    decl->next = pop_expr();
	case tok_semicollon:
	    consume();
	    break;
	case tok_obrace:
	    function();
	    return (tok_decl);
	default:
	    error(ahead, "syntax error");
    }

    for (; decl; decl = decl->next)
	(void)declare(type, decl);

    return (tok_decl);
}

static function_t *
prototype(tag_t *tag, expr_t *decl)
{
    expr_t	*expr;
    char	*name;
    function_t	*function;

    expr = decl->data._binary.lvalue;
    assert(expr->token == tok_symbol);
    name = expr->data._unary.cp;
    if ((function = (function_t *)get_hash(functions, name))) {
	if (function->tag != tag)
	    error(decl, "'%s' redeclared as a different type", name);
    }
    else if (get_hash((hash_t *)globals, name))
	error(decl, "'%s' redeclared as a different type", name);
    else {
	function = xcalloc(1, sizeof(function_t));
	function->name = name;
	put_hash(functions, (entry_t *)function);
	function->tag = tag;
    }

    return (function);
}

static void
function(void)
{
    expr_t	*expr;
    expr_t	*type;
    expr_t	*call;
    expr_t	*list;
    symbol_t	*symbol;
    function_t	*function;

    if (current != globals)
	error(top_expr(), "syntax error");
    expr = top_expr();
    expr->token = tok_function;
    type = expr->data._function.type;
    call = expr->data._function.call;
    function = prototype(type->data._unary.vp, call);
    expr->data._function.function = function;
    function->table = new_record(type_namespace);
    current = function->table;
    if ((list = call->data._binary.rvalue)) {
	if (list->data._unary.vp == void_tag) {
	    if (list->next)
		error(list, "syntax error");
	}
	else {
	    for (; list; list = list->next) {
		symbol = declare(list->data._binary.lvalue,
				 list->data._binary.rvalue);
		if (symbol == NULL)
		    error(list, "syntax error");
		symbol->arg = 1;
	    }
	}
    }
    expr->block = ablock;
    ablock = expr;
    /* adjust for alloca displacement */
    current->offset = 0;
    statement_noeof();
    end_record(current);
    ablock = expr->block;
    expr->data._function.body = pop_expr();
    current = globals;
}

static token_t
group(int data)
{
    expr_t	*expr;
    token_t	 token;

    expr = NULL;
    switch (token = lookahead()) {
	case tok_cparen:
	case tok_semicollon:
	    push_expr(NULL);
	    return (token);
	default:
	    break;
    }
    for (;;) {
	(void)expression();
	if (expr == NULL)
	    expr = top_expr();
	else {
	    expr->next = pop_expr();
	    expr = expr->next;
	}
	if ((token = lookahead()) == tok_comma) {
	    consume();
	    if (data && lookahead() == tok_cbrace) {
		if (expr == NULL)
		    push_expr(NULL);
		return (tok_cbrace);
	    }
	}
	else {
	    if (expr == NULL)
		push_expr(NULL);
	    return (token);
	}
    }
}

static token_t
precedence_noeof(void)
{
    token_t	token = precedence();

    if (token == tok_eof)
	error(NULL, "unexpected end of file");

    return (token);
}

static token_t
precedence(void)
{
    expr_t	*expr;
    token_t	 next, token;

    for (token = unary();;) {
	switch (token) {
	    case tok_obrace:
		expr = top_expr();
		if (group(1) != tok_cbrace)
		    error(expr, "syntax error");
		consume();
		expr->token = tok_data;
		expr->data._unary.expr = pop_expr();
		return (tok_expr);
	    case tok_int:
	    case tok_float:
	    case tok_symbol:
	    case tok_expr:
		switch (next = lookahead()) {
		    case tok_set:	case tok_andset:
		    case tok_orset:	case tok_xorset:
		    case tok_lshset:	case tok_rshset:
		    case tok_addset:	case tok_subset:
		    case tok_mulset:	case tok_divset:
		    case tok_remset:
			token = binary_assign();
			break;
		    case tok_andand:	case tok_oror:
		    case tok_lt:	case tok_le:
		    case tok_eq:	case tok_ge:
		    case tok_gt:	case tok_ne:
		    case tok_and:	case tok_or:
		    case tok_xor:	case tok_lsh:
		    case tok_rsh:	case tok_add:
		    case tok_sub:	case tok_mul:
		    case tok_div:	case tok_rem:
			token = binary_left(next);
			break;
		    case tok_question:
			(void)primary();
			expr = pop_expr();
			expr->data._if.test = top_expr();
			top_expr() = expr;
			precedence_noeof();
			expr->data._if.tcode = pop_expr();
			if (primary() != tok_collon)
			    error(top_expr(), "syntax error");
			discard();
			precedence_noeof();
			expr->data._if.fcode = pop_expr();
			break;
		    default:
			return (token);
		}
		break;
	    default:
		return (token);
	}
    }
}

static token_t
expression_noeof(void)
{
    token_t	token = expression();

    if (token == tok_eof)
	error(NULL, "unexpected end of file");

    return (token);
}

static token_t
expression(void)
{
    token_t	token;

    switch (token = precedence()) {
	case tok_symbol:
	    if (lookahead() == tok_collon) {
		consume();
		top_expr()->token = token = tok_label;
	    }
	default:
	    break;
    }

    return (token);
}

static token_t
binary_assign(void)
{
    expr_t	*lexp;
    expr_t	*mexp;
    expr_t	*rexp;
    token_t	 token;

    for (;;) {
	token = primary();
	switch (expression()) {
	    case tok_int:
	    case tok_float:
	    case tok_string:
	    case tok_symbol:
	    case tok_pointer:
	    case tok_expr:
		break;
	    case tok_address:
		/* ok: a = &b;		error: a += &b; */
		if (token == tok_set)
		    break;
	    default:
		error(top_expr(), "syntax error");
	}
	rexp = pop_expr();
	mexp = pop_expr();
	lexp = top_expr();
	mexp->data._binary.lvalue = lexp;
	mexp->data._binary.rvalue = rexp;
	top_expr() = mexp;
	switch (lookahead()) {
	    case tok_set:		case tok_andset:
	    case tok_orset:		case tok_xorset:
	    case tok_lshset:		case tok_rshset:
	    case tok_addset:		case tok_subset:
	    case tok_mulset:		case tok_divset:
	    case tok_remset:
		break;
	    default:
		return (tok_expr);
	}
    }
}

static token_t
binary_left(token_t token)
{
    expr_t	*lexp;
    expr_t	*mexp;
    expr_t	*rexp;

    for (;;) {
	(void)primary();
	binary_right(token);
	rexp = pop_expr();
	mexp = pop_expr();
	lexp = top_expr();
	mexp->data._binary.lvalue = lexp;
	mexp->data._binary.rvalue = rexp;
	top_expr() = mexp;
	switch (lookahead()) {
	    case tok_andand ... tok_oror:
		if (token <= tok_oror)	break;
		return (tok_expr);
	    case tok_lt ... tok_ne:
		if (token <= tok_ne)	break;
		return (tok_expr);
	    case tok_and ... tok_xor:
		if (token <= tok_xor)	break;
		return (tok_expr);
	    case tok_lsh ... tok_rsh:
		if (token <= tok_rsh)	break;
		return (tok_expr);
	    case tok_add ... tok_sub:
		if (token <= tok_sub)	break;
		return (tok_expr);
	    case tok_mul ... tok_rem:	break;
	    default:
		return (tok_expr);
	}
    }
}

static void
binary_right(token_t token)
{
    token_t	next;

    (void)unary();
    for (;;) {
	switch (next = lookahead()) {
	    case tok_lt ... tok_ne:
		if (token <= tok_oror)	break;
		return;
	    case tok_and ... tok_xor:
		if (token <= tok_ne)	break;
		return;
	    case tok_lsh ... tok_rsh:
		if (token <= tok_xor)	break;
		return;
	    case tok_add ... tok_sub:
		if (token <= tok_rsh)	break;
		return;
	    case tok_mul ... tok_rem:
		if (token <= tok_sub)	break;
		return;
	    default:
		return;
	}
	binary_left(next);
    }
}

static token_t
unary_noeof(void)
{
    token_t	token = unary();

    if (token == tok_eof)
	error(NULL, "unexpected end of file");

    return (token);
}

static token_t
unary(void)
{
    expr_t	*expr;
    token_t	 token;

    switch (token = primary()) {
	case tok_signed:	case tok_unsigned:
	case tok_type:		case tok_struct:
	case tok_union:
	    return (unary_decl());
	case tok_symbol:		case tok_string:
	    return (unary_loop(token));
	case tok_oparen:
	    return (unary_list());
	case tok_obrack:
	    expr = top_expr();
	    expr->token = tok_elemref;
	    expression_noeof();
	    expr->data._unary.expr = pop_expr();
	    if (primary_noeof() != tok_cbrack)
		error(expr, "syntax error");
	    discard();
	    return (tok_expr);
	case tok_dot:
	    expr = top_expr();
	    expr->token = tok_fieldref;
	    if (primary_noeof() != tok_symbol)
		error(expr, "syntax error");
	    expr->data._unary.expr = pop_expr();
	    return (tok_expr);
	case tok_inc:		case tok_dec:
	case tok_add:		case tok_sub:
	case tok_mul:		case tok_and:
	case tok_not:		case tok_com:
	    return (unary_value(token));
	case tok_sizeof:
	    expr = top_expr();
	    if ((token = lookahead()) == tok_oparen)
		consume();
	    expression_noeof();
	    expr->data._unary.expr = pop_expr();
	    if (token == tok_oparen) {
		if (primary_noeof() != tok_cparen)
		    error(expr, "expecting ')'");
		discard();
	    }
	    return (tok_expr);
	default:
	    return (token);
    }
}

static token_t
unary_loop(token_t token)
{
    expr_t	*expr;
    token_t	 next;

    for (;;) {
	switch (next = lookahead()) {
	    case tok_obrack:
		token = tok_expr;
		(void)unary_vector();
		break;
	    case tok_oparen:
		token = tok_expr;
		(void)primary();
		discard();
		if (group(0) != tok_cparen)
		    error(ahead, "expecting )");
		expr = ahead;
		ahead = NULL;
		expr->token = tok_call;
		expr->data._binary.rvalue = pop_expr();
		expr->data._binary.lvalue = top_expr();
		top_expr() = expr;
		break;
	    case tok_arrow:	case tok_dot:
		token = tok_expr;
		(void)unary_field();
		break;
	    case tok_inc:	case tok_dec:
		(void)primary();
		expr = pop_expr();
		expr->token = next == tok_inc ? tok_postinc : tok_postdec;
		expr->data._unary.expr = top_expr();
		top_expr() = expr;
		break;
	    default:
		return (token);
	}
    }
}

static token_t
unary_value(token_t token)
{
    expr_t	*expr, *value;

    switch (lookahead()) {
	case tok_int:			case tok_float:
	case tok_symbol:		case tok_string:
	case tok_expr:			case tok_mul:
	    break;
	case tok_and:
	    /* do not fail on "&*expr" */
	case tok_inc:			case tok_dec:
	    /* neither on "*++expr" */
	    if (token == tok_mul)
		break;
	default:
	    error(top_expr(), "syntax error");
    }

    (void)unary();
    expr = pop_expr();
    value = top_expr();
    switch (token) {
	case tok_add:		value->token = tok_add;		break;
	case tok_sub:		value->token = tok_neg;		break;
	case tok_mul:		value->token = tok_pointer;	break;
	case tok_and:		value->token = tok_address;	break;
	case tok_inc:		case tok_dec:			break;
	case tok_not:		case tok_com:			break;
	default:		error(value, "internal error");
    }
    value->data._unary.expr = expr;

    return (unary_loop(tok_expr));
}

static token_t
unary_type(void)
{
    int		 type;
    record_t	*record;
    expr_t	*expr = top_expr();

    type = 0;
    switch (expr->token) {
	case tok_unsigned:
	    type = type_unsigned;
	case tok_signed:
	    if (lookahead() == tok_type) {
		record = ahead->data._unary.vp;
		if (record->type < type_char || record->type > type_long)
		    error(ahead, "syntax error");
		consume();
		type |= record->type;
		if (record->type == type_short || record->type == type_long) {
		    if (lookahead() == tok_type) {
			record = ahead->data._unary.entry->value;
			if (record->type != type_int)
			    error(ahead, "syntax error");
			consume();
		    }
		}
	    }
	    else
		type |= type_int;
	    expr->data._unary.vp = tag_type(type);
	    break;
	case tok_struct:		case tok_union:
	    (void)unary_record(expr->token);
	    break;
	case tok_type:
	    record = expr->data._unary.vp;
	    type = record->type;
	    if (type == type_short || type == type_long) {
		switch (lookahead()) {
		    case tok_unsigned:
			type |= type_unsigned;
		    case tok_signed:
			if (lookahead() != tok_type)
			    error(ahead, "syntax error");
			consume();
		    case tok_type:
			record = ahead->data._unary.entry->value;
			if (record->type != type_int)
			    error(ahead, "syntax error");
			consume();
			break;
		    default:
			break;
		}
	    }
	    expr->data._unary.vp = tag_type(type);
	    break;
	default:
	    error(expr, "syntax error");
    }

    return (tok_type);
}

static token_t
unary_record(token_t token)
{
    int		 mask;
    expr_t	*expr;
    record_t	*save;
    record_t	*record;

    expr = top_expr();
    expr->token = tok_type;
    mask = token == tok_struct ? type_struct : type_union;
    switch (lookahead()) {
	case tok_obrace:
	    consume();
	    record = get_type(NULL, mask);
	    break;
	case tok_symbol:
	    record = get_type(ahead->data._unary.cp, mask);
	    consume();
	    if (lookahead() != tok_obrace) {
		expr->data._unary.vp = tag_type(record->type);
		return (tok_type);
	    }
	    consume();
	    break;
	default:
	    error(ahead, "syntax error");
    }

    save = current;
    current = record;
    while ((token = expression()) != tok_cbrace) {
	switch (token) {
	    case tok_decl:
		declaration();
		discard();
		break;
	    default:
		error(top_expr(), "syntax error");
	}
    }
    discard();
    current = save;
    end_record(record);
    if (record->length == 0)
	error(NULL, "empty declaration");
    expr->data._unary.vp = tag_type(record->type);

    return (tok_type);
}

static token_t
unary_decl(void)
{
    tag_t	*tag;
    expr_t	*expr;
    token_t	 token;

    if ((token = unary_type()) != tok_type)
	return (token);
    token = lookahead();
    if (token == tok_comma || token == tok_cparen)
	return (tok_type);
    if (token == tok_semicollon) {
	tag = top_expr()->data._unary.vp;
	if (type_mask(tag->type) == type_struct ||
	    type_mask(tag->type) == type_union)
	    return (tok_defn);
	error(top_expr(), "syntax error");
    }

    expr = new_expr(tok_declexpr, top_expr()->lineno, top_expr()->column);
    expr->data._binary.lvalue = top_expr();
    top_expr() = expr;
    switch ((token = expression_noeof())) {
	case tok_symbol:		case tok_expr:		case tok_pointer:
	    break;
	default:
	    error(top_expr(), "syntax error");
    }
    expr->data._binary.rvalue = pop_expr();

    return (tok_decl);
}

static token_t
unary_list(void)
{
    expr_t	*expr, *list;

    if (group(0) != tok_cparen)
	error(ahead, "expecting )");
    consume();
    list = pop_expr();
    expr = top_expr();
    expr->token = tok_list;
    expr->data._unary.expr = list;

    return (unary_loop(tok_expr));
}

static token_t
unary_vector(void)
{
    expr_t	*expr;

    expr = ahead;
    ahead = NULL;
    expr->token = tok_vector;
    expr->data._binary.lvalue = top_expr();
    top_expr() = expr;
    if (expression_noeof() == tok_cbrack)
	discard();
    else if (primary_noeof() != tok_cbrack)
	error(top_expr(), "syntax error");
    else {
	discard();
	expr->data._binary.rvalue = pop_expr();
    }

    return (unary_loop(tok_expr));
}

static token_t
unary_field(void)
{
    expr_t	*expr;

    expr = ahead;
    ahead = NULL;
    expr->data._binary.lvalue = top_expr();
    top_expr() = expr;
    if (primary_noeof() != tok_symbol)
	error(top_expr(), "syntax error");
    expr->data._binary.rvalue = pop_expr();

    return (unary_loop(tok_expr));
}
