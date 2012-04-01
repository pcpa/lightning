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
static int
getch(void);

static int
getch_noeof(void);

static int
ungetch(int ch);

static int
skipws(void);

static int
skipct(void);

static int
skipcp(void);

static token_t
identifier(int ch);

static token_t
number(int ch);

static int
escape(int ch);

static token_t
string(void);

static token_t
character(void);

/*
 * Initialization
 */
expr_t		*ahead;

/*
 * Implementation
 */
token_t
lookahead(void)
{
    token_t	token;
    data_t	value;

    if (ahead == NULL) {
	token = parser.token;
	value = parser.value;
	(void)primary();
	parser.token = token;
	parser.value = value;
	ahead = pop_expr();
    }

    return (ahead->token);
}

void
consume(void)
{
    if (ahead) {
	del_expr(ahead);
	ahead = NULL;
    }
    else
	error(NULL, "internal error");
}

void
discard(void)
{
    expr_t	*expr = pop_expr();

    if (expr)
	del_expr(expr);
}

token_t
primary_noeof(void)
{
    token_t	token = primary();

    if (token == tok_eof)
	error(NULL, "unexpected end of file");

    return (token);
}

token_t
primary(void)
{
    expr_t	*expr;
    entry_t	*entry;
    token_t	 token;
    int		 ch, lineno, column;

    if (ahead) {
	token = ahead->token;
	push_expr(ahead);
	ahead = NULL;
	return (token);
    }
    ch = skipws();
    lineno = parser.lineno;
    column = parser.column - 1;
    switch (ch) {
	case EOF:				token = tok_eof;	break;
	case '(':				token = tok_oparen;	break;
	case ')':				token = tok_cparen;	break;
	case '[':				token = tok_obrack;	break;
	case ']':				token = tok_cbrack;	break;
	case '{':				token = tok_obrace;	break;
	case '}':				token = tok_cbrace;	break;
	case ';':				token = tok_semicollon;	break;
	case ':':				token = tok_collon;	break;
	case ',':				token = tok_comma;	break;
	case '.':
	    if ((ch = getch()) != '.') {
		ungetch(ch);			token = tok_dot;
	    }
	    else if (getch() != '.')		error(NULL, "error near '..'");
	    else				token = tok_ellipsis;	break;
	case '=':
	    if ((ch = getch()) == '=')		token = tok_eq;
	    else {
		ungetch(ch);			token = tok_set;
	    }
	    break;
	case '&':
	    if ((ch = getch()) == '&')		token = tok_andand;
	    else if (ch == '=')			token = tok_andset;
	    else {
		ungetch(ch);			token = tok_and;
	    }
	    break;
	case '|':
	    if ((ch = getch()) == '|')		token = tok_oror;
	    else if (ch == '=')			token = tok_orset;
	    else {
		ungetch(ch);			token = tok_or;
	    }
	    break;
	case '^':
	    if ((ch = getch()) == '=')		token = tok_xorset;
	    else {
		ungetch(ch);			token = tok_xor;
	    }
	    break;
	case '<':
	    if ((ch = getch()) == '=')		token = tok_le;
	    else if (ch == '<') {
		if ((ch = getch()) == '=')	token = tok_lshset;
		else {
		    ungetch(ch);		token = tok_lsh;
		}
	    }
	    else {
		ungetch(ch);			token = tok_lt;
	    }
	    break;
	case '>':
	    if ((ch = getch()) == '=')		token = tok_ge;
	    else if (ch == '>') {
		if ((ch = getch()) == '=')	token = tok_rshset;
		else {
		    ungetch(ch);		token = tok_rsh;
		}
	    }
	    else {
		ungetch(ch);			token = tok_gt;
	    }
	    break;
	case '+':
	    if ((ch = getch()) == '+')		token = tok_inc;
	    else if (ch == '=')			token = tok_addset;
	    else {
		ungetch(ch);			token = tok_add;
	    }
	    break;
	case '-':
	    if ((ch = getch()) == '-')		token = tok_dec;
	    else if (ch == '=')			token = tok_subset;
	    else if (ch == '>')			token = tok_arrow;
	    else {
		ungetch(ch);			token = tok_sub;
	    }
	    break;
	case '*':
	    if ((ch = getch()) == '=')		token = tok_mulset;
	    else {
		ungetch(ch);			token = tok_mul;
	    }
	    break;
	case '/':
	    if ((ch = getch()) == '=')		token = tok_divset;
	    else {
		ungetch(ch);			token = tok_div;
	    }
	    break;
	case '%':
	    if ((ch = getch()) == '=')		token = tok_remset;
	    else {
		ungetch(ch);			token = tok_rem;
	    }
	    break;
	case '!':
	    if ((ch = getch()) == '=')		token = tok_ne;
	    else {
		ungetch(ch);			token = tok_not;
	    }
	    break;
	case '~':				token = tok_com;	break;
	case '?':				token = tok_question;	break;
	case 'a' ... 'z': case 'A' ... 'Z': case '_':
	    token = identifier(ch);					break;
	case '0' ... '9':			token = number(ch);	break;
	case '"':				token = string();	break;
	case '\'':				token = character();	break;
	default:				error(NULL, "syntax error");
    }

    parser.token = token;
    expr = new_expr(token, lineno, column);
    if (token == tok_string) {
	if ((entry = get_hash(strings, parser.string)) == NULL) {
	    entry = (entry_t *)xmalloc(sizeof(entry_t));
	    entry->name.string = xstrdup(parser.string);
	    entry->value = entry->name.string;
	    put_hash(strings, entry);
	}
	expr->data._unary.cp = entry->value;
    }
    else
	expr->data._unary = parser.value;
    push_expr(expr);

    return (token);
}

static int
getch(void)
{
    int		ch;

    if (parser.data.offset < parser.data.length)
	ch = parser.data.buffer[parser.data.offset++];
    else {
	/* keep first offset for ungetch */
	if ((parser.data.length = fread(parser.data.buffer + 1, 1,
					sizeof(parser.data.buffer) - 1,
					parser.fp) + 1) <= 1) {
	    ch = EOF;
	    parser.data.offset = 1;
	}
	else {
	    ch = parser.data.buffer[1];
	    parser.data.offset = 2;
	}
    }
    if ((parser.newline = ch == '\n')) {
	++parser.lineno;
	parser.column = 1;
    }
    else if (ch != EOF)
	++parser.column;

    return (ch);
}

static int
getch_noeof(void)
{
    int		ch = getch();

    if (ch == EOF)
	error(NULL, "unexpected end of file");

    return (ch);
}

static int
ungetch(int ch)
{
    /* column will be out of sync if ungetch() more characters */
    if ((parser.newline = ch == '\n'))
	--parser.lineno;
    else if (ch == EOF)
	error(NULL, "internal error");
    else
	--parser.column;

    if (parser.data.offset)
	parser.data.buffer[--parser.data.offset] = ch;
    else
	/* overwrite */
	parser.data.buffer[0] = ch;

    return (ch);
}

static int
skipws(void)
{
    int		ch;

    for (ch = getch();; ch = getch()) {
	switch (ch) {
	    case '/':
		ch = skipct();
		break;
	    case '#':
		ch = skipcp();
		break;
	}
	switch (ch) {
	    case ' ': case '\f': case '\r': case '\n':	case '\t':
		break;
	    default:
		return (ch);
	}
    }
}

static int
skipct(void)
{
    int		ch;

    ch = getch();
    switch (ch) {
	case '/':
	    for (ch = getch(); ch != '\n' && ch != EOF; ch = getch())
		;
	    return (ch);
	case '*':
	    for (; ch != '/';) {
		while (getch_noeof() != '*')
		    ;
		while ((ch = getch_noeof()) == '*')
		    ;
	    }
	    return (getch());
	default:
	    ungetch(ch);
	    return ('/');
    }
}

static int
skipcp(void)
{
    int		ch;

    for (ch = getch(); ch != '\n' && ch != EOF; ch = getch()) {
	switch (ch) {
	    case '0' ... '9':
		if ((number(ch)) == tok_int)
		    parser.lineno = parser.value.i - 1;
		break;
	    case '"':
		string();
		if (parser.offset >= (int)sizeof(parser.name)) {
		    strncpy(parser.name, parser.string, sizeof(parser.name));
		    parser.name[sizeof(parser.name) - 1] = '\0';
		}
		else
		    strcpy(parser.name, parser.string);
		break;
	    default:
		break;
	}
    }

    return (ch);
}

static token_t
identifier(int ch)
{
    keyword_t	*keyword;

    parser.string[0] = ch;
    for (parser.offset = 1;;) {
	switch ((ch = getch())) {
	    case 'a' ... 'z': case 'A' ... 'Z': case '0' ... '9' :  case '_':
		if (parser.offset + 1 >= MAX_IDENTIFIER) {
		    parser.string[parser.offset] = '\0';
		    error(NULL, "bad identifier %s", parser.string);
		}
		parser.string[parser.offset++] = ch;
		break;
	    default:
		parser.string[parser.offset] = '\0';
		ungetch(ch);
		keyword = (keyword_t *)get_hash(keywords, parser.string);
		if (keyword) {
		    parser.value.vp = keyword->value;
		    return (parser.token = keyword->token);
		}
		parser.value.cp = xintern(parser.string);
		return (parser.token = tok_symbol);
	}
    }
}

static token_t
number(int ch)
{
    char	buffer[1024], *endptr;
    int		integer = 1, offset = 0, e = 0, d = 0, base = 10;

    for (;; ch = getch()) {
	switch (ch) {
	    case '-':
		if (offset && buffer[offset - 1] != 'e') {
		    ungetch(ch);
		    goto done;
		}
		break;
	    case '+':
		if (offset && buffer[offset - 1] != 'e') {
		    ungetch(ch);
		    goto done;
		}
		break;
	    case '.':
		if (d)
		    goto fail;
		d = 1;
		base = 10;
		integer = 0;
		break;
	    case '0':
		if (offset == 0 && base == 10) {
		    base = 8;
		    continue;
		}
		break;
	    case 'b':
		if (offset == 0 && base == 8) {
		    base = 2;
		    continue;
		}
		if (base != 16)
		    goto fail;
		break;
	    case '1':
		break;
	    case '2' ... '7':
		if (base < 8)
		    goto fail;
		break;
	    case '8': case '9':
		if (base < 10)
		    goto fail;
		break;
	    case 'x':
		if (offset == 0 && base == 8) {
		    base = 16;
		    continue;
		}
		goto fail;
	    case 'a': case 'c': case 'd': case 'f':
		if (base < 16)
		    goto fail;
		break;
	    case 'e':
		if (e)
		    goto fail;
		if (base != 16) {
		    e = 1;
		    base = 10;
		    integer = 0;
		}
		break;
	    case '_': case 'g' ... 'w': case 'y': case 'z': case 'A' ... 'Z':
	    fail:
		buffer[offset++] = '\0';
		error(NULL, "bad number constant %s", buffer);
	    default:
		ungetch(ch);
		goto done;
	}
	if (offset + 1 >= (int)sizeof(buffer))
	    goto fail;
	buffer[offset++] = ch;
    }
done:
    /* check for literal 0 */
    if (offset == 0 && base == 8)	buffer[offset++] = '0';
    buffer[offset] = '\0';
    if (integer) {
	parser.token = tok_int;
	parser.value.ui = strtoul(buffer, &endptr, base);
    }
    else {
	parser.token = tok_float;
	parser.value.d = strtod(buffer, &endptr);
    }
    if (*endptr)
	goto fail;

    return (parser.token);
}

static int
escape(int ch)
{
    switch (ch) {
	case 'a':	ch = '\a';	break;
	case 'b':	ch = '\b';	break;
	case 'f':	ch = '\f';	break;
	case 'n':	ch = '\n';	break;
	case 'r':	ch = '\r';	break;
	case 't':	ch = '\t';	break;
	case 'v':	ch = '\v';	break;
	default:			break;
    }

    return (ch);
}

static token_t
string(void)
{
    int		ch, esc = 0;

    for (parser.offset = 0;;) {
	switch (ch = getch_noeof()) {
	    case '\\':
		if (esc)		goto append;
		esc = 1;
		break;
	    case '"':
		if (!esc) {
		    parser.string[parser.offset++] = '\0';
		    return (parser.token = tok_string);
		}
		/* FALLTHROUGH */
	    default:
	    append:
		if (esc) {
		    ch = escape(ch);
		    esc = 0;
		}
		if (parser.offset + 1 >= parser.length) {
		    parser.length += 4096;
		    parser.string = (char *)xrealloc(parser.string,
						     parser.length);
		}
		parser.string[parser.offset++] = ch;
		break;
	}
    }
}

static token_t
character(void)
{
    long	value;
    int		count, ch, esc = 0;

    for (count = 0, value = 0;;) {
	switch (ch = getch_noeof()) {
	    case '\\':
		if (esc)		goto append;
		esc = 1;
		break;
	    case '\'':
		if (!esc) {
		    if (count > sizeof(int))
			warn(NULL, "char constant truncated");
		    parser.value.i = value;
		    return (parser.token = tok_int);
		}
		/* FALLTHROUGH */
	    default:
	    append:
		if (esc) {
		    ch = escape(ch);
		    esc = 0;
		}
		value = (value << 8) | (ch & 0xff);
		++count;
		break;
	}
    }
}
