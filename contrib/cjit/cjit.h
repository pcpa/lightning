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

#ifndef _cjit_h
#define _cjit_h

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "thunder.h"

#if defined(__GNUC__)
#  define noreturn		__attribute__ ((noreturn))
#  define printf_format(f, v)	__attribute__ ((format (printf, f, v)))
#  define maybe_unused		__attribute__ ((unused))
#else
#  define noreturn		/**/
#  define printf_format(f, v)	/**/
#  define maybe_unused		/**/
#endif

#define MAX_IDENTIFIER		256
#define top_expr()		estack.values[estack.offset - 1]

#define hash_string		0
#define hash_pointer		1
#define hash_integer		2

#define DOUBLE_ALIGN		(__WORDSIZE >> 3)
#define DEFAULT_ALIGN		(__WORDSIZE >> 3)

#define type_void		0x00000000
#define type_char		0x00000001
#define type_short		0x00000002
#define type_int		0x00000003
#define type_long		0x00000004
#define type_float		0x00000005
#define type_double		0x00000006
#define type_namespace		0x02000000
#define type_function		0x04000000
#define type_union		0x08000000
#define type_struct		0x10000000
#define type_unsigned		0x20000000
#define type_vector		0x40000000
#define type_pointer		0x80000000
#define type_mask(type)		((type) & 		  \
				 (type_namespace	| \
				  type_function		| \
				  type_union		| \
				  type_struct		| \
				  type_unsigned		| \
				  type_vector		| \
				  type_pointer))
#define type_base(type)		((type) & 		  \
				 ~(type_namespace	| \
				   type_function	| \
				   type_union		| \
				   type_struct		| \
				   type_unsigned	| \
				   type_vector		| \
				   type_pointer))
#define type_uchar		(type_unsigned | type_char)
#define type_ushort		(type_unsigned | type_short)
#define type_uint		(type_unsigned | type_int)
#define type_ulong		(type_unsigned | type_long)
#define pointer_type_p(type)	((type) & (type_vector | type_pointer))

#define value_itype		0x00000000
#define value_funct		0x00800000
#define value_symbl		0x01000000
#define value_ltype		0x02000000
#define value_utype		0x04000000
#define value_ptype		0x08000000
#define value_ftype		0x10000000
#define value_dtype		0x20000000
#define value_regno		0x40000000
#define value_spill		0x80000000
#define value_ultype		(value_utype | value_ltype)
#define value_float_p(value)	((value)->type & (value_ftype | value_dtype))
/* must use immediate opcode ? */
#define value_const_p(value)	!((value)->type & value_regno)
/* must (re)load register ? */
#define value_load_p(value)	((value)->type &  \
				 (value_funct	| \
				  value_symbl	| \
				  value_ftype	| \
				  value_dtype	| \
				  value_spill))

/*
 * Types
 */
typedef struct hash		hash_t;
typedef struct record		record_t;
typedef struct entry		entry_t;
typedef struct keyword		keyword_t;
typedef struct tag		tag_t;
typedef struct symbol		symbol_t;
typedef struct function		function_t;
typedef union data		data_t;
typedef struct parser		parser_t;
typedef struct expr		expr_t;
typedef struct value		value_t;
typedef struct estack		estack_t;
typedef struct vstack		vstack_t;

typedef enum {
    tok_eof = -1,	tok_none,	tok_type,	tok_symbol,
    tok_int,		tok_float,	tok_string,	tok_oparen,
    tok_cparen,		tok_obrack,	tok_cbrack,	tok_obrace,
    tok_cbrace,		tok_semicollon,	tok_collon,	tok_comma,
    tok_dot,		tok_arrow,	tok_ellipsis,	tok_set,
    tok_andset,		tok_orset,	tok_xorset,	tok_lshset,
    tok_rshset,		tok_addset,	tok_subset,	tok_mulset,
    tok_divset,		tok_remset,	tok_andand,	tok_oror,
    tok_lt,		tok_le,		tok_eq,		tok_ge,
    tok_gt,		tok_ne,		tok_and,	tok_or,
    tok_xor,		tok_lsh,	tok_rsh,	tok_add,
    tok_sub,		tok_mul,	tok_div,	tok_rem,
    tok_inc,		tok_dec,	tok_postinc,	tok_postdec,
    tok_plus,		tok_neg,	tok_not,	tok_com,
    tok_pointer,	tok_address,	tok_sizeof,	tok_question,
    tok_if,		tok_else,	tok_goto,	tok_return,
    tok_switch,		tok_case,	tok_default,	tok_break,
    tok_for,		tok_do,		tok_while,	tok_continue,
    tok_signed,		tok_unsigned,	tok_struct,	tok_union,
    tok_typedef,	tok_list,	tok_expr,	tok_stat,
    tok_call,		tok_code,	tok_data,	tok_label,
    tok_vector,		tok_declexpr,	tok_decl,	tok_defn,
    tok_fieldref,	tok_elemref,	tok_function,
} token_t;

typedef union {
    void		 *v;
    signed char		 *c;
    unsigned char	 *uc;
    signed short	 *s;
    unsigned short	 *us;
    signed int		 *i;
    unsigned int	 *ui;
    signed long		 *l;
    unsigned long	 *ul;
    float		 *f;
    double		 *d;
    void		**p;
} union_t;

struct hash {
    entry_t		**entries;
    int			  kind;
    int			  size;
    int			  count;
};

struct record {
    symbol_t		**entries;
    int			  kind;
    int			  size;
    int			  count;
    entry_t		 *name;
    int			  type;
    int			  length;
    int			  offset;
    symbol_t		**vector;
};

struct entry {
    entry_t		*next;
    union {
	char		*string;
	void		*pointer;
	long		 integer;
    } name;
    void		*value;
};

struct keyword {
    keyword_t		*next;
    char		*name;
    void		*value;
    token_t		 token;
};

struct tag {
    tag_t		*next;
    void		*name;
    hash_t		*hash;
    int			 type;
    int			 size;
    tag_t		*tag;
};

struct symbol {
    symbol_t		*next;
    char		*name;
    tag_t		*tag;
    int			 type;
    int			 offset;
    record_t		*table;
    ejit_register_t	*regptr;
    unsigned int	 arg	: 1;	/* argument variable */
    unsigned int	 loc	: 1;	/* local variable */
    unsigned int	 glb	: 1;	/* global variable */
    unsigned int	 fld	: 1;	/* struct/union field */
    unsigned int	 reg	: 1;	/* register variable */
    unsigned int	 mem	: 1;	/* force memory if would use register */
};

struct function {
    function_t		*next;
    char		*name;
    tag_t		*tag;
    expr_t		*expr;
    record_t		*table;
};

union data {
    long		 i;
    unsigned long	 ui;
    double		 d;
    void		*vp;
    char		*cp;
    expr_t		*expr;
    entry_t		*entry;
    symbol_t		*symbol;
};

struct parser {
    FILE		*fp;
    char		 name[256];
    int			 lineno;
    int			 column;
    token_t		 token;
    data_t		 value;
    char		*string;
    int			 length;
    int			 offset;
    int			 newline;
    struct {
	unsigned char	 buffer[4096];
	int		 offset;
	int		 length;
    } data;
};

struct expr {
    expr_t		*next;
    expr_t		*block;
    token_t		 token;
    int			 lineno;
    int			 column;
    union {
	data_t		_unary;
	struct {
	    expr_t	*lvalue;
	    expr_t	*rvalue;
	} _binary;
	struct {
	    expr_t	*test;
	    expr_t	*tcode;
	    expr_t	*fcode;
	} _if;
	struct {
	    expr_t	*test;
	    expr_t	*code;
	} _while;
	struct {
	    expr_t	*init;
	    expr_t	*test;
	    expr_t	*incr;
	    expr_t	*code;
	} _for;
	struct {
	    expr_t	*code;
	    expr_t	*test;
	} _do;
	struct {
	    expr_t	*test;
	    expr_t	*code;
	    hash_t	*hash;
	    expr_t	*_default;
	} _switch;
	struct {
	    expr_t	*type;
	    expr_t	*call;
	    expr_t	*body;
	    function_t	*function;
	} _function;
    } data;
};

struct value {
    int			 type;
    int			 disp;
    union {
	int		 ival;
	long		 lval;
	unsigned long	ulval;
	float		 fval;
	double		 dval;
	void		*pval;
    } u;
};

struct estack {
    expr_t		**values;
    int			  offset;
    int			  length;
};

struct vstack {
    value_t		*values;
    int			 offset;
    int			 length;
};

/*
 * Prototypes
 */
extern void
error(expr_t *expr, const char *format, ...) noreturn printf_format(2, 3);

extern void
warn(expr_t *expr, const char *format, ...) printf_format(2, 3) maybe_unused;

extern void
init_alloc(void);

extern void *
xmalloc(size_t size);

extern void *
xrealloc(void *pointer, size_t size);

extern void *
xcalloc(size_t nmemb, size_t size);

extern char *
xintern(const char *string);

extern char *
xstrdup(const char *string);

extern hash_t *
new_hash(int kind);

extern void
put_hash(hash_t *hash, entry_t *entry);

extern entry_t *
get_hash(hash_t *hash, void *name);

extern void
del_hash(hash_t *hash);

extern void
init_stack(void);

extern void
push_expr(expr_t *expr);

extern expr_t *
pop_expr(void);

extern expr_t *
new_expr(token_t token, int lineno, int column);

extern void
del_expr(expr_t *expr);

extern void
del_stat(expr_t *expr);

extern token_t
lookahead(void);

extern void
consume(void);

extern void
discard(void);

extern token_t
primary_noeof(void);

extern token_t
primary(void);

extern void
init_type(void);

extern record_t *
new_record(int mask);

extern void
end_record(record_t *record);

extern record_t *
get_type(char *name, int mask);

extern tag_t *
tag_type(int type);

extern tag_t *
tag_pointer(tag_t *tag);

extern tag_t *
tag_vector(tag_t *tag, long length);

extern tag_t *
tag_decl(tag_t *tag, expr_t **decl);

extern symbol_t *
get_symbol(char *name);

extern symbol_t *
get_symbol_lvalue(expr_t *expr);

extern symbol_t *
new_symbol(record_t *record, tag_t *tag, char *name);

extern void
variable(ejit_state_t *state, symbol_t *symbol);

extern void
init_parser(void);

extern expr_t *
parse(void);

extern int
eval(expr_t *expr);

extern int
eval_stat(expr_t *expr);

extern void
print(expr_t *expr);

extern void
dump(void);

extern expr_t *
data(expr_t *expr);

extern tag_t *
data_tag(tag_t *tag, expr_t *expr);

extern void
init_emit(void);

extern void
emit(expr_t *expr);

/*
 * Externs
 */
extern parser_t		 parser;
extern estack_t	 	 estack;
extern expr_t		*ahead;
extern hash_t		*strings;
extern hash_t		*keywords;
extern record_t		*current;
extern record_t		*globals;
extern hash_t		*functions;
extern tag_t		*void_tag;
extern tag_t		*int_tag;
extern tag_t		*uint_tag;
extern tag_t		*long_tag;
extern tag_t		*ulong_tag;
extern tag_t		*float_tag;
extern tag_t		*double_tag;
extern void		*the_data;
extern void		*the_rodata;
extern long		 rodata_length;

#endif /* _cjit_h */
