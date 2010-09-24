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

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <getopt.h>
#include <lightning.h>
#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#if DISASSEMBLER
#  include <dis-asm.h>
#endif

#if DYNAMIC
#  include <dlfcn.h>
#endif

#if defined(__GNUC__)
#  define noreturn		__attribute__ ((noreturn))
#  define printf_format(f, v)	__attribute__ ((format (printf, f, v)))
#  define maybe_unused		__attribute__ ((unused))
#else
#  define noreturn		/**/
#  define printf_format(f, v)	/**/
#  define maybe_unused		/**/
#endif

/* FIXME feature request: patch finish/calli */
#define CALLI_PATCH		1

#define check_data()							\
    do {								\
	if (data >= data_end)						\
	    error(".data too small (%ld < %ld)",			\
		  (long)(data_end - data_start),			\
		  (long)(data - data_start));				\
    } while (0)

#define check_code()							\
    do {								\
	if ((char *)jit_get_label() >= code_end)			\
	    error(".code too small (%ld < %ld)",			\
		  (long)(code_end - code_start),			\
		  (long)((char *)jit_get_label() - code_start));	\
    } while (0)

#define get_label_by_name(name)	((label_t *)get_hash(labels, name))

#define MAX_IDENTIFIER			256

/*
 * Types
 */
typedef union data	  data_t;
typedef struct instr	  instr_t;
typedef union value	  value_t;
typedef struct hash	  hash_t;
typedef struct entry	  entry_t;
typedef struct parser	  parser_t;
typedef struct label	  label_t;
typedef struct patch	  patch_t;
typedef struct symbol	  symbol_t;
typedef int		(*function_t)(int argc, char *argv[]);

typedef enum {
    tok_eof = -1,
    tok_symbol,
    tok_char,
    tok_int,
    tok_float,
    tok_pointer,
    tok_string,
    tok_register,
    tok_dot,
    tok_newline,
    tok_semicollon,
} token_t;

typedef enum {
    skip_none,
    skip_ws,
    skip_nl,
} skip_t;

typedef enum {
    type_none,
    type_c,
    type_s,
    type_i,
    type_l,
    type_f,
    type_d,
    type_p,
} type_t;

#define compose(a, b)		(((a) << 8) | b)
typedef enum {
    expr_inc	 = compose('+', '+'),
    expr_dec	 = compose('-', '-'),
    expr_not	 = '!',
    expr_com	 = '~',
    expr_mul	 = '*',
    expr_div	 = '/',
    expr_rem	 = '%',
    expr_add	 = '+',
    expr_sub	 = '-',
    expr_lsh	 = compose('<', '<'),
    expr_rsh	 = compose('>', '>'),
    expr_and	 = '&',
    expr_or	 = '|',
    expr_xor	 = '^',
    expr_set	 = '=',
    expr_mulset	 = compose('*', '='),
    expr_divset	 = compose('/', '='),
    expr_remset	 = compose('%', '='),
    expr_addset	 = compose('+', '='),
    expr_subset	 = compose('-', '='),
    expr_lshset	 = compose(expr_lsh, '='),
    expr_rshset	 = compose(expr_rsh, '='),
    expr_andset	 = compose('&', '='),
    expr_orset	 = compose('|', '='),
    expr_xorset	 = compose('^', '='),
    expr_lt	 = '<',
    expr_le	 = compose('<', '='),
    expr_eq	 = compose('=', '='),
    expr_ne	 = compose('!', '='),
    expr_gt	 = '>',
    expr_ge	 = compose('>', '='),
    expr_andand	 = compose('&', '&'),
    expr_oror	 = compose('|', '|'),
    expr_lparen	 = '(',
    expr_rparen	 = ')',
    expr_int	 = '0',
    expr_float	 = '.',
    expr_pointer = '@',
    expr_symbol  = '$',
} expr_t;
#undef compose

struct instr {
    instr_t		 *next;
    const char		 *name;
    void		(*function)(void);
    int			  flag;
};

union value {
    long		 i;
    unsigned long	 ui;
    float		 f;
    double		 d;
    void		*p;
    char		*cp;
    label_t		*label;
    patch_t		*patch;
};

struct parser {
    FILE		*fp;
    char		 name[256];
    int			 line;
    int			 regval;
    type_t		 regtype;
    expr_t		 expr;
    type_t		 type;
    value_t		 value;

    /* variable length string buffer */
    char		*string;
    int			 length;
    int			 offset;

    int			 newline;
    expr_t		 putback;
    int			 short_circuit;
    struct {
	unsigned char	 buffer[4096];
	int		 offset;
	int		 length;
    } data;
};

typedef enum {
    label_kind_data,
    label_kind_code,
    label_kind_code_forward,
#if DYNAMIC
    label_kind_dynamic,
#endif
} label_kind_t;

struct hash {
    entry_t		**entries;
    int			  size;
    int			  count;
};

struct entry {
    entry_t		 *next;
    char		 *name;
    void		 *value;
    int			  flag;
};

struct label {
    label_t		*next;
    char		*name;
    void		*value;
    label_kind_t	 kind;
};

typedef enum {
    patch_kind_jmp,
    patch_kind_mov,
#if CALLI_PATCH
    patch_kind_call,
#endif
} patch_kind_t;

struct patch {
    patch_t		*next;
    label_t		*label;
    void		*value;
    patch_kind_t	 kind;
};

/* minor support for expressions */
struct symbol {
    symbol_t		*next;
    char		*name;
    value_t		 value;
    type_t		 type;
};

/*
 * Prototypes
 */
#if DISASSEMBLER
static int bcmp_labels_by_value(const void *left, const void *right);
static int qcmp_labels_by_value(const void *left, const void *right);
static void print_address(bfd_vma addr, struct disassemble_info *info);
static void disassemble(void *code, int length);
static void print_data(unsigned char *data, int length);
static void dump(char *data, int length);
static void show_symbols(void);
#endif

static jit_gpr_t get_ireg(void);
static jit_fpr_t get_freg(void);
static symbol_t *get_symbol(void);
static void jmp_forward(void *value, label_t *label);
static void mov_forward(void *value, label_t *label);
#if CALLI_PATCH
static void call_forward(void *value, label_t *label);
#endif
static void make_arg(long value);
static long get_arg(void);

static void addi_i(void);	static void addi_ui(void);
static void addi_l(void);	static void addi_ul(void);
static void addi_p(void);
static void addr_i(void);	static void addr_ui(void);
static void addr_l(void);	static void addr_ul(void);
static void addr_p(void);
static void addr_f(void);	static void addr_d(void);
static void addci_i(void);	static void addci_ui(void);
static void addci_l(void);	static void addci_ul(void);
static void addcr_i(void);	static void addcr_ui(void);
static void addcr_l(void);	static void addcr_ul(void);
static void addxi_i(void);	static void addxi_ui(void);
static void addxi_l(void);	static void addxi_ul(void);
static void addxr_i(void);	static void addxr_ui(void);
static void addxr_l(void);	static void addxr_ul(void);
static void subi_i(void);	static void subi_ui(void);
static void subi_l(void);	static void subi_ul(void);
static void subi_p(void);
static void subr_i(void);	static void subr_ui(void);
static void subr_l(void);	static void subr_ul(void);
static void subr_p(void);
static void subr_f(void);	static void subr_d(void);
static void subci_i(void);	static void subci_ui(void);
static void subci_l(void);	static void subci_ul(void);
static void subcr_i(void);	static void subcr_ui(void);
static void subcr_l(void);	static void subcr_ul(void);
static void subxi_i(void);	static void subxi_ui(void);
static void subxi_l(void);	static void subxi_ul(void);
static void subxr_i(void);	static void subxr_ui(void);
static void subxr_l(void);	static void subxr_ul(void);
static void rsbi_i(void);	static void rsbi_ui(void);
static void rsbi_l(void);	static void rsbi_ul(void);
static void rsbi_p(void);
static void rsbr_i(void);	static void rsbr_ui(void);
static void rsbr_l(void);	static void rsbr_ul(void);
static void rsbr_p(void);
static void rsbr_f(void);	static void rsbr_d(void);
static void muli_i(void);	static void muli_ui(void);
static void muli_l(void);	static void muli_ul(void);
static void mulr_i(void);	static void mulr_ui(void);
static void mulr_l(void);	static void mulr_ul(void);
static void mulr_f(void);	static void mulr_d(void);
static void hmuli_i(void);	static void hmuli_ui(void);
static void hmuli_l(void);	static void hmuli_ul(void);
static void hmulr_i(void);	static void hmulr_ui(void);
static void hmulr_l(void);	static void hmulr_ul(void);
static void divi_i(void);	static void divi_ui(void);
static void divi_l(void);	static void divi_ul(void);
static void divr_i(void);	static void divr_ui(void);
static void divr_l(void);	static void divr_ul(void);
static void divr_f(void);	static void divr_d(void);
static void modi_i(void);	static void modi_ui(void);
static void modi_l(void);	static void modi_ul(void);
static void modr_i(void);	static void modr_ui(void);
static void modr_l(void);	static void modr_ul(void);
static void andi_i(void);	static void andi_ui(void);
static void andi_l(void);	static void andi_ul(void);
static void andr_i(void);	static void andr_ui(void);
static void andr_l(void);	static void andr_ul(void);
static void ori_i(void);	static void ori_ui(void);
static void ori_l(void);	static void ori_ul(void);
static void orr_i(void);	static void orr_ui(void);
static void orr_l(void);	static void orr_ul(void);
static void xori_i(void);	static void xori_ui(void);
static void xori_l(void);	static void xori_ul(void);
static void xorr_i(void);	static void xorr_ui(void);
static void xorr_l(void);	static void xorr_ul(void);
static void lshi_i(void);	static void lshi_ui(void);
static void lshi_l(void);	static void lshi_ul(void);
static void lshr_i(void);	static void lshr_ui(void);
static void lshr_l(void);	static void lshr_ul(void);
static void rshi_i(void);	static void rshi_ui(void);
static void rshi_l(void);	static void rshi_ul(void);
static void rshr_i(void);	static void rshr_ui(void);
static void rshr_l(void);	static void rshr_ul(void);
static void negr_i(void);	static void negr_l(void);
static void negr_f(void);	static void negr_d(void);
static void notr_i(void);	static void notr_ui(void);
static void notr_l(void);	static void notr_ul(void);
static void lti_i(void);	static void lti_ui(void);
static void lti_l(void);	static void lti_ul(void);
static void lti_p(void);
static void ltr_i(void);	static void ltr_ui(void);
static void ltr_l(void);	static void ltr_ul(void);
static void ltr_p(void);
static void ltr_f(void);	static void ltr_d(void);
static void lei_i(void);	static void lei_ui(void);
static void lei_l(void);	static void lei_ul(void);
static void lei_p(void);
static void ler_i(void);	static void ler_ui(void);
static void ler_l(void);	static void ler_ul(void);
static void ler_p(void);
static void ler_f(void);	static void ler_d(void);
static void gti_i(void);	static void gti_ui(void);
static void gti_l(void);	static void gti_ul(void);
static void gti_p(void);
static void gtr_i(void);	static void gtr_ui(void);
static void gtr_l(void);	static void gtr_ul(void);
static void gtr_p(void);
static void gtr_f(void);	static void gtr_d(void);
static void gei_i(void);	static void gei_ui(void);
static void gei_l(void);	static void gei_ul(void);
static void gei_p(void);
static void ger_i(void);	static void ger_ui(void);
static void ger_l(void);	static void ger_ul(void);
static void ger_p(void);
static void ger_f(void);	static void ger_d(void);
static void eqi_i(void);	static void eqi_ui(void);
static void eqi_l(void);	static void eqi_ul(void);
static void eqi_p(void);
static void eqr_i(void);	static void eqr_ui(void);
static void eqr_l(void);	static void eqr_ul(void);
static void eqr_p(void);
static void eqr_f(void);	static void eqr_d(void);
static void nei_i(void);	static void nei_ui(void);
static void nei_l(void);	static void nei_ul(void);
static void nei_p(void);
static void ner_i(void);	static void ner_ui(void);
static void ner_l(void);	static void ner_ul(void);
static void ner_p(void);
static void ner_f(void);	static void ner_d(void);
static void unltr_f(void);	static void unltr_d(void);
static void unler_f(void);	static void unler_d(void);
static void ungtr_f(void);	static void ungtr_d(void);
static void unger_f(void);	static void unger_d(void);
static void uneqr_f(void);	static void uneqr_d(void);
static void ltgtr_f(void);	static void ltgtr_d(void);
static void ordr_f(void);	static void ordr_d(void);
static void unordr_f(void);	static void unordr_d(void);
static void movi_i(void);	static void movi_ui(void);
static void movi_l(void);	static void movi_ul(void);
static void movi_p(void);
static void movi_f(void);	static void movi_d(void);
static void movr_i(void);	static void movr_ui(void);
static void movr_l(void);	static void movr_ul(void);
static void movr_p(void);
static void movr_f(void);	static void movr_d(void);
static void extr_c_s(void);	static void extr_c_us(void);
static void extr_c_i(void);	static void extr_c_ui(void);
static void extr_c_l(void);	static void extr_c_ul(void);
static void extr_uc_s(void);	static void extr_uc_us(void);
static void extr_uc_i(void);	static void extr_uc_ui(void);
static void extr_uc_l(void);	static void extr_uc_ul(void);
static void extr_s_i(void);	static void extr_s_ui(void);
static void extr_s_l(void);	static void extr_s_ul(void);
static void extr_us_i(void);	static void extr_us_ui(void);
static void extr_us_l(void);	static void extr_us_ul(void);
static void extr_i_l(void);	static void extr_i_ul(void);
static void extr_ui_l(void);	static void extr_ui_ul(void);
static void extr_i_f(void);	static void extr_i_d(void);
static void extr_l_f(void);	static void extr_l_d(void);
static void extr_f_d(void);	static void extr_d_f(void);
#if defined(jit_rintr_d_i)
static void rintr_f_i(void);	static void rintr_d_i(void);
static void rintr_f_l(void);	static void rintr_d_l(void);
#endif
static void roundr_f_i(void);	static void roundr_d_i(void);
static void truncr_f_i(void);	static void truncr_d_i(void);
static void floorr_f_i(void);	static void floorr_d_i(void);
static void ceilr_f_i(void);	static void ceilr_d_i(void);
static void roundr_f_l(void);	static void roundr_d_l(void);
static void truncr_f_l(void);	static void truncr_d_l(void);
static void floorr_f_l(void);	static void floorr_d_l(void);
static void ceilr_f_l(void);	static void ceilr_d_l(void);
static void hton_us(void);	static void hton_ui(void);
static void ntoh_us(void);	static void ntoh_ui(void);
static void ldi_c(void);	static void ldi_uc(void);
static void ldi_s(void);	static void ldi_us(void);
static void ldi_i(void);	static void ldi_ui(void);
static void ldi_l(void);	static void ldi_ul(void);
static void ldi_p(void);
static void ldi_f(void);	static void ldi_d(void);
static void ldr_c(void);	static void ldr_uc(void);
static void ldr_s(void);	static void ldr_us(void);
static void ldr_i(void);	static void ldr_ui(void);
static void ldr_l(void);	static void ldr_ul(void);
static void ldr_p(void);
static void ldr_f(void);	static void ldr_d(void);
static void ldxi_c(void);	static void ldxi_uc(void);
static void ldxi_s(void);	static void ldxi_us(void);
static void ldxi_i(void);	static void ldxi_ui(void);
static void ldxi_l(void);	static void ldxi_ul(void);
static void ldxi_p(void);
static void ldxi_f(void);	static void ldxi_d(void);
static void ldxr_c(void);	static void ldxr_uc(void);
static void ldxr_s(void);	static void ldxr_us(void);
static void ldxr_i(void);	static void ldxr_ui(void);
static void ldxr_l(void);	static void ldxr_ul(void);
static void ldxr_p(void);
static void ldxr_f(void);	static void ldxr_d(void);
static void sti_c(void);	static void sti_uc(void);
static void sti_s(void);	static void sti_us(void);
static void sti_i(void);	static void sti_ui(void);
static void sti_l(void);	static void sti_ul(void);
static void sti_p(void);
static void sti_f(void);	static void sti_d(void);
static void str_c(void);	static void str_uc(void);
static void str_s(void);	static void str_us(void);
static void str_i(void);	static void str_ui(void);
static void str_l(void);	static void str_ul(void);
static void str_p(void);
static void str_f(void);	static void str_d(void);
static void stxi_c(void);	static void stxi_uc(void);
static void stxi_s(void);	static void stxi_us(void);
static void stxi_i(void);	static void stxi_ui(void);
static void stxi_l(void);	static void stxi_ul(void);
static void stxi_p(void);
static void stxi_f(void);	static void stxi_d(void);
static void stxr_c(void);	static void stxr_uc(void);
static void stxr_s(void);	static void stxr_us(void);
static void stxr_i(void);	static void stxr_ui(void);
static void stxr_l(void);	static void stxr_ul(void);
static void stxr_p(void);
static void stxr_f(void);	static void stxr_d(void);
static void blti_i(void);	static void blti_ui(void);
static void blti_l(void);	static void blti_ul(void);
static void blti_p(void);
static void bltr_i(void);	static void bltr_ui(void);
static void bltr_l(void);	static void bltr_ul(void);
static void bltr_p(void);
static void bltr_f(void);	static void bltr_d(void);
static void blei_i(void);	static void blei_ui(void);
static void blei_l(void);	static void blei_ul(void);
static void blei_p(void);
static void bler_i(void);	static void bler_ui(void);
static void bler_l(void);	static void bler_ul(void);
static void bler_p(void);
static void bler_f(void);	static void bler_d(void);
static void bgti_i(void);	static void bgti_ui(void);
static void bgti_l(void);	static void bgti_ul(void);
static void bgti_p(void);
static void bgtr_i(void);	static void bgtr_ui(void);
static void bgtr_l(void);	static void bgtr_ul(void);
static void bgtr_p(void);
static void bgtr_f(void);	static void bgtr_d(void);
static void bgei_i(void);	static void bgei_ui(void);
static void bgei_l(void);	static void bgei_ul(void);
static void bgei_p(void);
static void bger_i(void);	static void bger_ui(void);
static void bger_l(void);	static void bger_ul(void);
static void bger_p(void);
static void bger_f(void);	static void bger_d(void);
static void beqi_i(void);	static void beqi_ui(void);
static void beqi_l(void);	static void beqi_ul(void);
static void beqi_p(void);
static void beqr_i(void);	static void beqr_ui(void);
static void beqr_l(void);	static void beqr_ul(void);
static void beqr_p(void);
static void beqr_f(void);	static void beqr_d(void);
static void bnei_i(void);	static void bnei_ui(void);
static void bnei_l(void);	static void bnei_ul(void);
static void bnei_p(void);
static void bner_i(void);	static void bner_ui(void);
static void bner_l(void);	static void bner_ul(void);
static void bner_p(void);
static void bner_f(void);	static void bner_d(void);
static void bunltr_f(void);	static void bunltr_d(void);
static void bunler_f(void);	static void bunler_d(void);
static void bungtr_f(void);	static void bungtr_d(void);
static void bunger_f(void);	static void bunger_d(void);
static void buneqr_f(void);	static void buneqr_d(void);
static void bltgtr_f(void);	static void bltgtr_d(void);
static void bordr_f(void);	static void bordr_d(void);
static void bunordr_f(void);	static void bunordr_d(void);
static void bmsi_i(void);	static void bmsi_ui(void);
static void bmsi_l(void);	static void bmsi_ul(void);
static void bmsr_i(void);	static void bmsr_ui(void);
static void bmsr_l(void);	static void bmsr_ul(void);
static void bmci_i(void);	static void bmci_ui(void);
static void bmci_l(void);	static void bmci_ul(void);
static void bmcr_i(void);	static void bmcr_ui(void);
static void bmcr_l(void);	static void bmcr_ul(void);
static void boaddi_i(void);	static void boaddi_ui(void);
static void boaddi_l(void);	static void boaddi_ul(void);
static void boaddr_i(void);	static void boaddr_ui(void);
static void boaddr_l(void);	static void boaddr_ul(void);
static void bosubi_i(void);	static void bosubi_ui(void);
static void bosubi_l(void);	static void bosubi_ul(void);
static void bosubr_i(void);	static void bosubr_ui(void);
static void bosubr_l(void);	static void bosubr_ul(void);
static void prepare(void);
static void prepare_f(void);	static void prepare_d(void);
static void pusharg_c(void);	static void pusharg_uc(void);
static void pusharg_s(void);	static void pusharg_us(void);
static void pusharg_i(void);	static void pusharg_ui(void);
static void pusharg_l(void);	static void pusharg_ul(void);
static void pusharg_p(void);
static void pusharg_f(void);	static void pusharg_d(void);
static void getarg_c(void);	static void getarg_uc(void);
static void getarg_s(void);	static void getarg_us(void);
static void getarg_i(void);	static void getarg_ui(void);
static void getarg_l(void);	static void getarg_ul(void);
static void getarg_p(void);
static void getarg_f(void);	static void getarg_d(void);
static void arg_c(void);	static void arg_uc(void);
static void arg_s(void);	static void arg_us(void);
static void arg_i(void);	static void arg_ui(void);
static void arg_l(void);	static void arg_ul(void);
static void arg_p(void);
static void arg_f(void);	static void arg_d(void);
static void calli(void);	static void callr(void);
static void finish(void);	static void finishr(void);
static void jmpi(void);		static void jmpr(void);
static void ret(void);
static void retval_c(void);	static void retval_uc(void);
static void retval_s(void);	static void retval_us(void);
static void retval_i(void);	static void retval_ui(void);
static void retval_l(void);	static void retval_ul(void);
static void retval_p(void);
static void retval_f(void);	static void retval_d(void);
static void prolog(void);
static void prolog_f(void);	static void prolog_d(void);
static void leaf(void);
static void leaf_f(void);	static void leaf_d(void);
static void allocai(void);
#if defined(jit_absr_f)
static void absr_f(void);	static void absr_d(void);
#endif
#if defined(jit_sqrtr_f)
static void sqrtr_f(void);	static void sqrtr_d(void);
#endif
#if defined(jit_sinr_f)
static void sinr_f(void);	static void sinr_d(void);
static void cosr_f(void);	static void cosr_d(void);
static void tanr_f(void);	static void tanr_d(void);
static void atanr_f(void);	static void atanr_d(void);
static void logr_f(void);	static void logr_d(void);
static void log2r_f(void);	static void log2r_d(void);
static void log10r_f(void);	static void log10r_d(void);
#endif

static void error(const char *format, ...) noreturn printf_format(1, 2);
static void warn(const char *format, ...) printf_format(1, 2) maybe_unused;
static void message(const char *kind, const char *format, va_list ap);
static int getch(void);
static int getch_noeof(void);
static int ungetch(int ch);
static int skipws(void);
static int skipnl(void);
static int skipct(void);
static int skipcp(void);
static long get_int(skip_t skip);
static unsigned long get_uint(skip_t skip);
static double get_float(skip_t skip);
static void *get_pointer(skip_t skip);
static label_t *get_label(skip_t skip);
static token_t regname(void);
static int qcmp_instrs(const void *left, const void *right);
static token_t identifier(int ch);
static void get_data(type_t type);
static void dot(void);
static token_t number(int ch);
static int escape(int ch);
static token_t string(void);
#if DYNAMIC
static token_t dynamic(void);
#endif
static token_t character(void);
static void expression_prim(void);
static void expression_inc(int pre);
static void expression_dec(int pre);
static void expression_unary(void);
static void expression_mul(void);
static void expression_add(void);
static void expression_shift(void);
static void expression_bit(void);
static void expression_rel(void);
static void expression_cond(void);
static token_t expression(void);
static token_t primary(skip_t skip);
static void parse(void);
static int execute(int argc, char *argv[]);
static label_t *new_label(label_kind_t kind, char *name, void *value);
static patch_t *new_patch(patch_kind_t kind, label_t *label, void *value);
static int bcmp_symbols(const void *left, const void *right);
static int qcmp_symbols(const void *left, const void *right);
static symbol_t *new_symbol(char *name);
static symbol_t *get_symbol_by_name(char *name);
static void *xmalloc(size_t size);
static void *xrealloc(void *pointer, size_t size);
static void *xcalloc(size_t nmemb, size_t size);
static hash_t *new_hash(void);
static int hash_string(char *name);
static void put_hash(hash_t *hash, entry_t *entry);
static entry_t *get_hash(hash_t *hash, char *name);
static void rehash(hash_t *hash);

/*
 * Initialization
 */
static char		 *progname;
#if DISASSEMBLER
static label_t		**labels_by_value;
#endif
static int		  flag_verbose;
static char		 *data_start;
static char		 *data;
static char		 *data_end;
static char		 *code_start;
static char		 *code_end;
static parser_t		  parser;
static hash_t		 *labels;
#if DISASSEMBLER
static int		  label_length;
#endif
static int		  label_offset;
static patch_t		 *patches;
static symbol_t		**symbols;
static int		  symbol_length;
static int		  symbol_offset;
static hash_t		 *instrs;
static instr_t		  instr_vector[] = {
#define entry(value)	{ NULL, #value, value }
    entry(addi_i),	entry(addi_ui),
    entry(addi_l),	entry(addi_ul),
    entry(addi_p),
    entry(addr_i),	entry(addr_ui),
    entry(addr_l),	entry(addr_ul),
    entry(addr_p),
    entry(addr_f),	entry(addr_d),
    entry(addci_i),	entry(addci_ui),
    entry(addci_l),	entry(addci_ul),
    entry(addcr_i),	entry(addcr_ui),
    entry(addcr_l),	entry(addcr_ul),
    entry(addxi_i),	entry(addxi_ui),
    entry(addxi_l),	entry(addxi_ul),
    entry(addxr_i),	entry(addxr_ui),
    entry(addxr_l),	entry(addxr_ul),
    entry(subi_i),	entry(subi_ui),
    entry(subi_l),	entry(subi_ul),
    entry(subi_p),
    entry(subr_i),	entry(subr_ui),
    entry(subr_l),	entry(subr_ul),
    entry(subr_p),
    entry(subr_f),	entry(subr_d),
    entry(subci_i),	entry(subci_ui),
    entry(subci_l),	entry(subci_ul),
    entry(subcr_i),	entry(subcr_ui),
    entry(subcr_l),	entry(subcr_ul),
    entry(subxi_i),	entry(subxi_ui),
    entry(subxi_l),	entry(subxi_ul),
    entry(subxr_i),	entry(subxr_ui),
    entry(subxr_l),	entry(subxr_ul),
    entry(rsbi_i),	entry(rsbi_ui),
    entry(rsbi_l),	entry(rsbi_ul),
    entry(rsbi_p),
    entry(rsbr_i),	entry(rsbr_ui),
    entry(rsbr_l),	entry(rsbr_ul),
    entry(rsbr_p),
    entry(rsbr_f),	entry(rsbr_d),
    entry(muli_i),	entry(muli_ui),
    entry(muli_l),	entry(muli_ul),
    entry(mulr_i),	entry(mulr_ui),
    entry(mulr_l),	entry(mulr_ul),
    entry(mulr_f),	entry(mulr_d),
    entry(hmuli_i),	entry(hmuli_ui),
    entry(hmuli_l),	entry(hmuli_ul),
    entry(hmulr_i),	entry(hmulr_ui),
    entry(hmulr_l),	entry(hmulr_ul),
    entry(divi_i),	entry(divi_ui),
    entry(divi_l),	entry(divi_ul),
    entry(divr_i),	entry(divr_ui),
    entry(divr_l),	entry(divr_ul),
    entry(divr_f),	entry(divr_d),
    entry(modi_i),	entry(modi_ui),
    entry(modi_l),	entry(modi_ul),
    entry(modr_i),	entry(modr_ui),
    entry(modr_l),	entry(modr_ul),
    entry(andi_i),	entry(andi_ui),
    entry(andi_l),	entry(andi_ul),
    entry(andr_i),	entry(andr_ui),
    entry(andr_l),	entry(andr_ul),
    entry(ori_i),	entry(ori_ui),
    entry(ori_l),	entry(ori_ul),
    entry(orr_i),	entry(orr_ui),
    entry(orr_l),	entry(orr_ul),
    entry(xori_i),	entry(xori_ui),
    entry(xori_l),	entry(xori_ul),
    entry(xorr_i),	entry(xorr_ui),
    entry(xorr_l),	entry(xorr_ul),
    entry(lshi_i),	entry(lshi_ui),
    entry(lshi_l),	entry(lshi_ul),
    entry(lshr_i),	entry(lshr_ui),
    entry(lshr_l),	entry(lshr_ul),
    entry(rshi_i),	entry(rshi_ui),
    entry(rshi_l),	entry(rshi_ul),
    entry(rshr_i),	entry(rshr_ui),
    entry(rshr_l),	entry(rshr_ul),
    entry(negr_i),	entry(negr_l),
    entry(negr_f),	entry(negr_d),
    entry(notr_i),	entry(notr_ui),
    entry(notr_l),	entry(notr_ul),
    entry(lti_i),	entry(lti_ui),
    entry(lti_l),	entry(lti_ul),
    entry(lti_p),
    entry(ltr_i),	entry(ltr_ui),
    entry(ltr_l),	entry(ltr_ul),
    entry(ltr_p),
    entry(ltr_f),	entry(ltr_d),
    entry(lei_i),	entry(lei_ui),
    entry(lei_l),	entry(lei_ul),
    entry(lei_p),
    entry(ler_i),	entry(ler_ui),
    entry(ler_l),	entry(ler_ul),
    entry(ler_p),
    entry(ler_f),	entry(ler_d),
    entry(gti_i),	entry(gti_ui),
    entry(gti_l),	entry(gti_ul),
    entry(gti_p),
    entry(gtr_i),	entry(gtr_ui),
    entry(gtr_l),	entry(gtr_ul),
    entry(gtr_p),
    entry(gtr_f),	entry(gtr_d),
    entry(gei_i),	entry(gei_ui),
    entry(gei_l),	entry(gei_ul),
    entry(gei_p),
    entry(ger_i),	entry(ger_ui),
    entry(ger_l),	entry(ger_ul),
    entry(ger_p),
    entry(ger_f),	entry(ger_d),
    entry(eqi_i),	entry(eqi_ui),
    entry(eqi_l),	entry(eqi_ul),
    entry(eqi_p),
    entry(eqr_i),	entry(eqr_ui),
    entry(eqr_l),	entry(eqr_ul),
    entry(eqr_p),
    entry(eqr_f),	entry(eqr_d),
    entry(nei_i),	entry(nei_ui),
    entry(nei_l),	entry(nei_ul),
    entry(nei_p),
    entry(ner_i),	entry(ner_ui),
    entry(ner_l),	entry(ner_ul),
    entry(ner_p),
    entry(ner_f),	entry(ner_d),
    entry(unltr_f),	entry(unltr_d),
    entry(unler_f),	entry(unler_d),
    entry(ungtr_f),	entry(ungtr_d),
    entry(unger_f),	entry(unger_d),
    entry(uneqr_f),	entry(uneqr_d),
    entry(ltgtr_f),	entry(ltgtr_d),
    entry(ordr_f),	entry(ordr_d),
    entry(unordr_f),	entry(unordr_d),
    entry(movi_i),	entry(movi_ui),
    entry(movi_l),	entry(movi_ul),
    entry(movi_p),
    entry(movi_f),	entry(movi_d),
    entry(movr_i),	entry(movr_ui),
    entry(movr_l),	entry(movr_ul),
    entry(movr_p),
    entry(movr_f),	entry(movr_d),
    entry(extr_c_s),	entry(extr_c_us),
    entry(extr_c_i),	entry(extr_c_ui),
    entry(extr_c_l),	entry(extr_c_ul),
    entry(extr_uc_s),	entry(extr_uc_us),
    entry(extr_uc_i),	entry(extr_uc_ui),
    entry(extr_uc_l),	entry(extr_uc_ul),
    entry(extr_s_i),	entry(extr_s_ui),
    entry(extr_s_l),	entry(extr_s_ul),
    entry(extr_us_i),	entry(extr_us_ui),
    entry(extr_us_l),	entry(extr_us_ul),
    entry(extr_i_l),	entry(extr_i_ul),
    entry(extr_ui_l),	entry(extr_ui_ul),
    entry(extr_i_f),	entry(extr_i_d),
    entry(extr_l_f),	entry(extr_l_d),
    entry(extr_f_d),	entry(extr_d_f),
#if defined(jit_rintr_d_i)
    entry(rintr_f_i),	entry(rintr_d_i),
    entry(rintr_f_l),	entry(rintr_d_l),
#endif
    entry(roundr_f_i),	entry(roundr_d_i),
    entry(truncr_f_i),	entry(truncr_d_i),
    entry(floorr_f_i),	entry(floorr_d_i),
    entry(ceilr_f_i),	entry(ceilr_d_i),
    entry(roundr_f_l),	entry(roundr_d_l),
    entry(truncr_f_l),	entry(truncr_d_l),
    entry(floorr_f_l),	entry(floorr_d_l),
    entry(ceilr_f_l),	entry(ceilr_d_l),
    entry(hton_us),	entry(hton_ui),
    entry(ntoh_us),	entry(ntoh_ui),
    entry(ldi_c),	entry(ldi_uc),
    entry(ldi_s),	entry(ldi_us),
    entry(ldi_i),	entry(ldi_ui),
    entry(ldi_l),	entry(ldi_ul),
    entry(ldi_p),
    entry(ldi_f),	entry(ldi_d),
    entry(ldr_c),	entry(ldr_uc),
    entry(ldr_s),	entry(ldr_us),
    entry(ldr_i),	entry(ldr_ui),
    entry(ldr_l),	entry(ldr_ul),
    entry(ldr_p),
    entry(ldr_f),	entry(ldr_d),
    entry(ldxi_c),	entry(ldxi_uc),
    entry(ldxi_s),	entry(ldxi_us),
    entry(ldxi_i),	entry(ldxi_ui),
    entry(ldxi_l),	entry(ldxi_ul),
    entry(ldxi_p),
    entry(ldxi_f),	entry(ldxi_d),
    entry(ldxr_c),	entry(ldxr_uc),
    entry(ldxr_s),	entry(ldxr_us),
    entry(ldxr_i),	entry(ldxr_ui),
    entry(ldxr_l),	entry(ldxr_ul),
    entry(ldxr_p),
    entry(ldxr_f),	entry(ldxr_d),
    entry(sti_c),	entry(sti_uc),
    entry(sti_s),	entry(sti_us),
    entry(sti_i),	entry(sti_ui),
    entry(sti_l),	entry(sti_ul),
    entry(sti_p),
    entry(sti_f),	entry(sti_d),
    entry(str_c),	entry(str_uc),
    entry(str_s),	entry(str_us),
    entry(str_i),	entry(str_ui),
    entry(str_l),	entry(str_ul),
    entry(str_p),
    entry(str_f),	entry(str_d),
    entry(stxi_c),	entry(stxi_uc),
    entry(stxi_s),	entry(stxi_us),
    entry(stxi_i),	entry(stxi_ui),
    entry(stxi_l),	entry(stxi_ul),
    entry(stxi_p),
    entry(stxi_f),	entry(stxi_d),
    entry(stxr_c),	entry(stxr_uc),
    entry(stxr_s),	entry(stxr_us),
    entry(stxr_i),	entry(stxr_ui),
    entry(stxr_l),	entry(stxr_ul),
    entry(stxr_p),
    entry(stxr_f),	entry(stxr_d),
    entry(blti_i),	entry(blti_ui),
    entry(blti_l),	entry(blti_ul),
    entry(blti_p),
    entry(bltr_i),	entry(bltr_ui),
    entry(bltr_l),	entry(bltr_ul),
    entry(bltr_p),
    entry(bltr_f),	entry(bltr_d),
    entry(blei_i),	entry(blei_ui),
    entry(blei_l),	entry(blei_ul),
    entry(blei_p),
    entry(bler_i),	entry(bler_ui),
    entry(bler_l),	entry(bler_ul),
    entry(bler_p),
    entry(bler_f),	entry(bler_d),
    entry(bgti_i),	entry(bgti_ui),
    entry(bgti_l),	entry(bgti_ul),
    entry(bgti_p),
    entry(bgtr_i),	entry(bgtr_ui),
    entry(bgtr_l),	entry(bgtr_ul),
    entry(bgtr_p),
    entry(bgtr_f),	entry(bgtr_d),
    entry(bgei_i),	entry(bgei_ui),
    entry(bgei_l),	entry(bgei_ul),
    entry(bgei_p),
    entry(bger_i),	entry(bger_ui),
    entry(bger_l),	entry(bger_ul),
    entry(bger_p),
    entry(bger_f),	entry(bger_d),
    entry(beqi_i),	entry(beqi_ui),
    entry(beqi_l),	entry(beqi_ul),
    entry(beqi_p),
    entry(beqr_i),	entry(beqr_ui),
    entry(beqr_l),	entry(beqr_ul),
    entry(beqr_p),
    entry(beqr_f),	entry(beqr_d),
    entry(bnei_i),	entry(bnei_ui),
    entry(bnei_l),	entry(bnei_ul),
    entry(bnei_p),
    entry(bner_i),	entry(bner_ui),
    entry(bner_l),	entry(bner_ul),
    entry(bner_p),
    entry(bner_f),	entry(bner_d),
    entry(bunltr_f),	entry(bunltr_d),
    entry(bunler_f),	entry(bunler_d),
    entry(bungtr_f),	entry(bungtr_d),
    entry(bunger_f),	entry(bunger_d),
    entry(buneqr_f),	entry(buneqr_d),
    entry(bltgtr_f),	entry(bltgtr_d),
    entry(bordr_f),	entry(bordr_d),
    entry(bunordr_f),	entry(bunordr_d),
    entry(bmsi_i),	entry(bmsi_ui),
    entry(bmsi_l),	entry(bmsi_ul),
    entry(bmsr_i),	entry(bmsr_ui),
    entry(bmsr_l),	entry(bmsr_ul),
    entry(bmci_i),	entry(bmci_ui),
    entry(bmci_l),	entry(bmci_ul),
    entry(bmcr_i),	entry(bmcr_ui),
    entry(bmcr_l),	entry(bmcr_ul),
    entry(boaddi_i),	entry(boaddi_ui),
    entry(boaddi_l),	entry(boaddi_ul),
    entry(boaddr_i),	entry(boaddr_ui),
    entry(boaddr_l),	entry(boaddr_ul),
    entry(bosubi_i),	entry(bosubi_ui),
    entry(bosubi_l),	entry(bosubi_ul),
    entry(bosubr_i),	entry(bosubr_ui),
    entry(bosubr_l),	entry(bosubr_ul),
    entry(prepare),
    entry(prepare_f),	entry(prepare_d),
    entry(pusharg_c),	entry(pusharg_uc),
    entry(pusharg_s),	entry(pusharg_us),
    entry(pusharg_i),	entry(pusharg_ui),
    entry(pusharg_l),	entry(pusharg_ul),
    entry(pusharg_p),
    entry(pusharg_f),	entry(pusharg_d),
    entry(getarg_c),	entry(getarg_uc),
    entry(getarg_s),	entry(getarg_us),
    entry(getarg_i),	entry(getarg_ui),
    entry(getarg_l),	entry(getarg_ul),
    entry(getarg_p),
    entry(getarg_f),	entry(getarg_d),
    entry(arg_c),	entry(arg_uc),
    entry(arg_s),	entry(arg_us),
    entry(arg_i),	entry(arg_ui),
    entry(arg_l),	entry(arg_ul),
    entry(arg_p),
    entry(arg_f),	entry(arg_d),
    entry(calli),	entry(callr),
    entry(finish),	entry(finishr),
    entry(jmpi),	entry(jmpr),
    entry(ret),
    entry(retval_c),	entry(retval_uc),
    entry(retval_s),	entry(retval_us),
    entry(retval_i),	entry(retval_ui),
    entry(retval_l),	entry(retval_ul),
    entry(retval_p),
    entry(retval_f),	entry(retval_d),
    entry(prolog),
    entry(prolog_f),	entry(prolog_d),
    entry(leaf),
    entry(leaf_f),	entry(leaf_d),
    entry(allocai),
#if defined(jit_absr_f)
    entry(absr_f),	entry(absr_d),
#endif
#if defined(jit_sqrtr_f)
    entry(sqrtr_f),	entry(sqrtr_d),
#endif
#if defined(jit_sinr_f)
    entry(sinr_f),	entry(sinr_d),
    entry(cosr_f),	entry(cosr_d),
    entry(tanr_f),	entry(tanr_d),
    entry(atanr_f),	entry(atanr_d),
    entry(logr_f),	entry(logr_d),
    entry(log2r_f),	entry(log2r_d),
    entry(log10r_f),	entry(log10r_d),
#endif
#undef entry
};

/*
 * Implementation
 */
#if DISASSEMBLER
static int
bcmp_labels_by_value(const void *left, const void *right)
{
    return (left < (*(label_t **)right)->value ? -1 :
	    left > (*(label_t **)right)->value ?  1 : 0);
}

static int
qcmp_labels_by_value(const void *left, const void *right)
{
    return ((*(label_t **)left)->value < (*(label_t **)right)->value ? -1 :
	    (*(label_t **)left)->value > (*(label_t **)right)->value ?  1 : 0);
}

static label_t *
get_label_by_value(void *value)
{
    label_t	**label_pointer;

    label_pointer = (label_t **)bsearch(value, labels_by_value,
					label_offset, sizeof(label_t *),
					bcmp_labels_by_value);

    return (label_pointer ? *label_pointer : NULL);
}

#  if __WORDSIZE == 32
#    define address_buffer_length	16
#    define address_buffer_format	"%llx"
#  else
#    define address_buffer_length	32
#    define address_buffer_format	"%lx"
#  endif
static void
print_address(bfd_vma addr, struct disassemble_info *info)
{
    label_t		*label;
    char		 buffer[address_buffer_length];

    sprintf(buffer, address_buffer_format, addr);
    (*info->fprintf_func)(info->stream, "0x%s", buffer);
    if ((label = get_label_by_value((void *)(long)addr)))
	(*info->fprintf_func)(info->stream, " # %s", label->name);
}

#  define stream			stdout
static void disassemble(void *code, int length)
{
    static bfd			*bfd;
    static disassemble_info	 info;
    int				 bytes;
    label_t			*label;
    int				 offset = 0;
    static disassembler_ftype	 print_insn;
    bfd_vma			 pc = (unsigned long)code;
    bfd_vma			 end = (unsigned long)code + length;
    char			 buffer[address_buffer_length];

    if (bfd == NULL) {
	bfd_init();
	bfd = bfd_openr(progname, NULL);
	assert(bfd != NULL);
	bfd_check_format(bfd, bfd_object);
	bfd_check_format(bfd, bfd_archive);
	print_insn = disassembler(bfd);
	assert(print_insn != NULL);
	INIT_DISASSEMBLE_INFO(info, stream, fprintf);
#  if defined(__i386__) || defined(__x86_64__)
	info.arch = bfd_arch_i386;
#    if defined(__x86_64__)
	info.mach = bfd_mach_x86_64;
#    else
	info.mach = bfd_mach_i386_i386;
#    endif
	info.print_address_func = print_address;
#  endif
    }
    info.buffer = (bfd_byte *)code;
    info.buffer_vma = (unsigned long)code;
    info.buffer_length = length;
    while (pc < end) {
	for (; offset < label_offset; offset++) {
	    label = labels_by_value[offset];
	    if (label->value == (void *)(long)pc)
		info.fprintf_func(stream, "%s:\n", label->name);
	    else if (label->value > (void *)(long)pc)
		break;
	}
	bytes = sprintf(buffer, address_buffer_format, pc);
	(*info.fprintf_func)(stream, "%*c0x%s\t", 16 - bytes, ' ', buffer);
	bytes = (*print_insn)(pc, &info);
	if (flag_verbose > 1) {
	    fprintf(stream, "\n\t\t\t");
	    print_data((unsigned char *)(unsigned long)pc, bytes);
	}
	else
	    putc('\n', stream);
	pc += bytes;
    }
}

static void
print_data(unsigned char *data, int length)
{
    int		offset;

    for (offset = 0; offset < length - 1; offset++)
	fprintf(stream, "%02x ", data[offset]);
    if (offset < length)
	fprintf(stream, "%02x\n", data[offset]);
}

static void
dump(char *data, int length)
{
    label_t	*label;
    int		 bytes, offset = 0;
    char	*end = data + length, buffer[address_buffer_length];
    while (data < end) {
	if ((length = end - data) > 16)
	    length = 16;
	for (; offset < label_offset; offset++) {
	    label = labels_by_value[offset];
	    if (data == (char *)label->value)
		fprintf(stream, "%s:\n", label->name);
	    else if (data < (char *)label->value) {
		if (data + length > (char *)label->value)
		    length = (char *)label->value - data;
		break;
	    }
	}
	bytes = sprintf(buffer, "%lx", (unsigned long)data);
	fprintf(stream, "%*c0x%s\t", 16 - bytes, ' ', buffer);
	print_data((unsigned char *)data, length);
	data += length;
    }
}

static void
show_symbols(void)
{
    int		 offset;
    symbol_t	*symbol;

    for (offset = 0; offset < symbol_offset; offset++) {
	symbol = symbols[offset];
	fprintf(stream, "%-32s= ", symbol->name);
	switch (symbol->type) {
	    case type_l:
		fprintf(stream, "%ld\n", symbol->value.i);
		break;
	    case type_d:
		fprintf(stream, "%f\n", symbol->value.d);
		break;
	    default:
		fprintf(stream, "%p\n", symbol->value.p);
		break;
	}
    }
}
#endif

static jit_gpr_t
get_ireg(void)
{
    if (primary(skip_ws) != tok_register)
	error("bad register");
    if (parser.regtype != type_l)
	error("bad int register");

    return ((jit_gpr_t)parser.regval);
}

static jit_fpr_t
get_freg(void)
{
    if (primary(skip_ws) != tok_register)
	error("bad register");
    if (parser.regtype != type_d)
	error("bad float register");

    return ((jit_fpr_t)parser.regval);
}

static symbol_t *
get_symbol(void)
{
    symbol_t	*symbol;
    int		 ch = skipws();

    if (ch != '$')
	error("expecting variable");
    (void)identifier('$');
    if (parser.string[1] == '\0')
	error("expecting variable");
    if ((symbol = get_symbol_by_name(parser.string)) == NULL)
	symbol = new_symbol(parser.string);

    return (symbol);
}

static void
jmp_forward(void *value, label_t *label)
{
    (void)new_patch(patch_kind_jmp, label, value);
}

static void
mov_forward(void *value, label_t *label)
{
    (void)new_patch(patch_kind_mov, label, value);
}

#if CALLI_PATCH
static void
call_forward(void *value, label_t *label)
{
    (void)new_patch(patch_kind_call, label, value);
}
#endif

static void
make_arg(long value)
{
    symbol_t	*symbol = get_symbol();

    symbol->type = type_l;
    symbol->value.i = value;
}

static long
get_arg(void)
{
    symbol_t	*symbol = get_symbol();

    if (symbol->type != type_l)
	error("bad argument %s type", symbol->name);

    return symbol->value.i;
}

#define entry_lb_ir_im(name)						\
static void								\
name(void)								\
{									\
    label_t	*label = get_label(skip_ws);				\
    jit_gpr_t	r0 = get_ireg();					\
    long	im = get_int(skip_ws);					\
    if (label->kind == label_kind_code_forward)				\
	jmp_forward((void *)jit_##name(jit_forward(), r0, im), label);	\
    else								\
	(void)jit_##name((jit_insn *)label->value, r0, im);		\
}
#define entry_lb_ir_ir(name)						\
static void								\
name(void)								\
{									\
    label_t	*label = get_label(skip_ws);				\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg();			\
    if (label->kind == label_kind_code_forward)				\
	jmp_forward((void *)jit_##name(jit_forward(), r0, r1), label);	\
    else								\
	(void)jit_##name((jit_insn *)label->value, r0, r1);		\
}
#define entry_lb_fr_fr(name)						\
static void								\
name(void)								\
{									\
    label_t	*label = get_label(skip_ws);				\
    jit_fpr_t	f0 = get_freg(), f1 = get_freg();			\
    if (label->kind == label_kind_code_forward)				\
	jmp_forward((void *)jit_##name(jit_forward(), f0, f1), label);	\
    else								\
	(void)jit_##name((jit_insn *)label->value, f0, f1);		\
}
#define entry_ir_ir(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg();			\
    jit_##name(r0, r1);							\
}
#define entry_ir_im(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg();					\
    long	im = get_int(skip_ws);					\
    jit_##name(r0, im);							\
}
#define entry_ir_um(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t		r0 = get_ireg();				\
    unsigned long	um = get_uint(skip_ws);				\
    jit_##name(r0, um);							\
}
#define entry_fr_fm(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg();					\
    double	fm = get_float(skip_ws);				\
    jit_##name(f0, fm);							\
}
#define entry_ir_pm(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	 r0 = get_ireg();					\
    void	*pm = get_pointer(skip_ws);				\
    jit_##name(r0, pm);							\
}
#define entry_ir_ir_im(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg();			\
    long	im = get_int(skip_ws);					\
    jit_##name(r0, r1, im);						\
}
#define entry_ir_ir_ir(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg(), r2 = get_ireg();	\
    jit_##name(r0, r1, r2);						\
}
#define entry_im_ir_ir(name)						\
static void								\
name(void)								\
{									\
    long	im = get_int(skip_ws);					\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg();			\
    (void)jit_##name(im, r0, r1);					\
}
#define entry_fr_ir(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg();					\
    jit_gpr_t	r0 = get_ireg();					\
    jit_##name(f0, r0);							\
}
#define entry_fr_ir_im(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg();					\
    jit_gpr_t	r0 = get_ireg();					\
    long	im = get_int(skip_ws);					\
    jit_##name(f0, r0, im);						\
}
#define entry_fr_ir_ir(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg();					\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg();			\
    jit_##name(f0, r0, r1);						\
}
#define entry_ir_fr(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg();					\
    jit_fpr_t	f0 = get_freg();					\
    jit_##name(r0, f0);							\
}
#define entry_im_ir_fr(name)						\
static void								\
name(void)								\
{									\
    long	im = get_int(skip_ws);					\
    jit_gpr_t	r0 = get_ireg();					\
    jit_fpr_t	f0 = get_freg();					\
    jit_##name(im, r0, f0);						\
}
#define entry_ir_ir_fr(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg(), r1 = get_ireg();			\
    jit_fpr_t	f0 = get_freg();					\
    jit_##name(r0, r1, f0);						\
}
#define entry_ir_fr_fr(name)						\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg();					\
    jit_fpr_t	f0 = get_freg(), f1 = get_freg();			\
    jit_##name(r0, f0, f1);						\
}
#define entry_fr_fr(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg(), f1 = get_freg();			\
    jit_##name(f0, f1);							\
}
#define entry_fr_fr_fr(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg(), f1 = get_freg(), f2 = get_freg();	\
    jit_##name(f0, f1, f2);						\
}
#define entry_im(name)							\
static void								\
name(void)								\
{									\
    maybe_unused long im = get_int(skip_ws);				\
    jit_##name(im);							\
}
#define entry_ir(name)							\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg();					\
    jit_##name(r0);							\
}
#define entry_fr(name)							\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg();					\
    jit_##name(f0);							\
}
#define entry_ca(name)							\
static void								\
name(void)								\
{									\
    make_arg(jit_##name());						\
}
#define entry_ia(name)							\
static void								\
name(void)								\
{									\
    jit_gpr_t	r0 = get_ireg();					\
    long	ac = get_arg();						\
    jit_##name(r0, ac);							\
}
#define entry_fa(name)							\
static void								\
name(void)								\
{									\
    jit_fpr_t	f0 = get_freg();					\
    long	ac = get_arg();						\
    jit_##name(f0, ac);							\
}
#define entry(name)							\
static void								\
name(void)								\
{									\
    jit_##name();							\
}
#define entry_pm(name)							\
static void								\
name(void)								\
{									\
    void	*pm = get_pointer(skip_ws);				\
    (void)jit_##name(pm);						\
}
#define entry_lb(name)							\
static void								\
name(void)								\
{									\
    label_t	*label = get_label(skip_ws);				\
    if (label->kind == label_kind_code_forward)				\
	jmp_forward((void *)jit_##name(jit_forward()), label);		\
    else								\
	(void)jit_##name((jit_insn *)label->value);			\
}
#define entry_lf(name)							\
static void								\
name(void)								\
{									\
    label_t	*label = get_label(skip_ws);				\
    if (label->kind == label_kind_code_forward)				\
	call_forward((void *)jit_##name(jit_forward()), label);		\
    else								\
	(void)jit_##name(label->value);					\
}
#define entry_fr_pm(name)						\
static void								\
name(void)								\
{									\
    jit_fpr_t	 f0 = get_freg();					\
    void	*pm = get_pointer(skip_ws);				\
    jit_##name(f0, pm);							\
}
#define entry_pm_ir(name)						\
static void								\
name(void)								\
{									\
    void	*pm = get_pointer(skip_ws);				\
    jit_gpr_t	 r0 = get_ireg();					\
    jit_##name(pm, r0);							\
}
#define entry_pm_fr(name)						\
static void								\
name(void)								\
{									\
    void	*pm = get_pointer(skip_ws);				\
    jit_fpr_t	 f0 = get_freg();					\
    jit_##name(pm, f0);							\
}

entry_ir_ir_im(addi_i)		entry_ir_ir_im(addi_ui)
entry_ir_ir_im(addi_l)		entry_ir_ir_im(addi_ul)
entry_ir_ir_im(addi_p)
entry_ir_ir_ir(addr_i)		entry_ir_ir_ir(addr_ui)
entry_ir_ir_ir(addr_l)		entry_ir_ir_ir(addr_ul)
entry_ir_ir_ir(addr_p)
entry_fr_fr_fr(addr_f)		entry_fr_fr_fr(addr_d)
entry_ir_ir_im(addci_i)		entry_ir_ir_im(addci_ui)
entry_ir_ir_im(addci_l)		entry_ir_ir_im(addci_ul)
entry_ir_ir_ir(addcr_i)		entry_ir_ir_ir(addcr_ui)
entry_ir_ir_ir(addcr_l)		entry_ir_ir_ir(addcr_ul)
entry_ir_ir_im(addxi_i)		entry_ir_ir_im(addxi_ui)
entry_ir_ir_im(addxi_l)		entry_ir_ir_im(addxi_ul)
entry_ir_ir_ir(addxr_i)		entry_ir_ir_ir(addxr_ui)
entry_ir_ir_ir(addxr_l)		entry_ir_ir_ir(addxr_ul)
entry_ir_ir_im(subi_i)		entry_ir_ir_im(subi_ui)
entry_ir_ir_im(subi_l)		entry_ir_ir_im(subi_ul)
entry_ir_ir_im(subi_p)
entry_ir_ir_ir(subr_i)		entry_ir_ir_ir(subr_ui)
entry_ir_ir_ir(subr_l)		entry_ir_ir_ir(subr_ul)
entry_ir_ir_ir(subr_p)
entry_fr_fr_fr(subr_f)		entry_fr_fr_fr(subr_d)
entry_ir_ir_im(subci_i)		entry_ir_ir_im(subci_ui)
entry_ir_ir_im(subci_l)		entry_ir_ir_im(subci_ul)
entry_ir_ir_ir(subcr_i)		entry_ir_ir_ir(subcr_ui)
entry_ir_ir_ir(subcr_l)		entry_ir_ir_ir(subcr_ul)
entry_ir_ir_im(subxi_i)		entry_ir_ir_im(subxi_ui)
entry_ir_ir_im(subxi_l)		entry_ir_ir_im(subxi_ul)
entry_ir_ir_ir(subxr_i)		entry_ir_ir_ir(subxr_ui)
entry_ir_ir_ir(subxr_l)		entry_ir_ir_ir(subxr_ul)
entry_ir_ir_im(rsbi_i)		entry_ir_ir_im(rsbi_ui)
entry_ir_ir_im(rsbi_l)		entry_ir_ir_im(rsbi_ul)
entry_ir_ir_im(rsbi_p)
entry_ir_ir_ir(rsbr_i)		entry_ir_ir_ir(rsbr_ui)
entry_ir_ir_ir(rsbr_l)		entry_ir_ir_ir(rsbr_ul)
entry_ir_ir_ir(rsbr_p)
entry_fr_fr_fr(rsbr_f)		entry_fr_fr_fr(rsbr_d)
entry_ir_ir_im(muli_i)		entry_ir_ir_im(muli_ui)
entry_ir_ir_im(muli_l)		entry_ir_ir_im(muli_ul)
entry_ir_ir_ir(mulr_i)		entry_ir_ir_ir(mulr_ui)
entry_ir_ir_ir(mulr_l)		entry_ir_ir_ir(mulr_ul)
entry_fr_fr_fr(mulr_f)		entry_fr_fr_fr(mulr_d)
entry_ir_ir_im(hmuli_i)		entry_ir_ir_im(hmuli_ui)
entry_ir_ir_im(hmuli_l)		entry_ir_ir_im(hmuli_ul)
entry_ir_ir_ir(hmulr_i)		entry_ir_ir_ir(hmulr_ui)
entry_ir_ir_ir(hmulr_l)		entry_ir_ir_ir(hmulr_ul)
entry_ir_ir_im(divi_i)		entry_ir_ir_im(divi_ui)
entry_ir_ir_im(divi_l)		entry_ir_ir_im(divi_ul)
entry_ir_ir_ir(divr_i)		entry_ir_ir_ir(divr_ui)
entry_ir_ir_ir(divr_l)		entry_ir_ir_ir(divr_ul)
entry_fr_fr_fr(divr_f)		entry_fr_fr_fr(divr_d)
entry_ir_ir_im(modi_i)		entry_ir_ir_im(modi_ui)
entry_ir_ir_im(modi_l)		entry_ir_ir_im(modi_ul)
entry_ir_ir_ir(modr_i)		entry_ir_ir_ir(modr_ui)
entry_ir_ir_ir(modr_l)		entry_ir_ir_ir(modr_ul)
entry_ir_ir_im(andi_i)		entry_ir_ir_im(andi_ui)
entry_ir_ir_im(andi_l)		entry_ir_ir_im(andi_ul)
entry_ir_ir_ir(andr_i)		entry_ir_ir_ir(andr_ui)
entry_ir_ir_ir(andr_l)		entry_ir_ir_ir(andr_ul)
entry_ir_ir_im(ori_i)		entry_ir_ir_im(ori_ui)
entry_ir_ir_im(ori_l)		entry_ir_ir_im(ori_ul)
entry_ir_ir_ir(orr_i)		entry_ir_ir_ir(orr_ui)
entry_ir_ir_ir(orr_l)		entry_ir_ir_ir(orr_ul)
entry_ir_ir_im(xori_i)		entry_ir_ir_im(xori_ui)
entry_ir_ir_im(xori_l)		entry_ir_ir_im(xori_ul)
entry_ir_ir_ir(xorr_i)		entry_ir_ir_ir(xorr_ui)
entry_ir_ir_ir(xorr_l)		entry_ir_ir_ir(xorr_ul)
entry_ir_ir_im(lshi_i)		entry_ir_ir_im(lshi_ui)
entry_ir_ir_im(lshi_l)		entry_ir_ir_im(lshi_ul)
entry_ir_ir_ir(lshr_i)		entry_ir_ir_ir(lshr_ui)
entry_ir_ir_ir(lshr_l)		entry_ir_ir_ir(lshr_ul)
entry_ir_ir_im(rshi_i)		entry_ir_ir_im(rshi_ui)
entry_ir_ir_im(rshi_l)		entry_ir_ir_im(rshi_ul)
entry_ir_ir_ir(rshr_i)		entry_ir_ir_ir(rshr_ui)
entry_ir_ir_ir(rshr_l)		entry_ir_ir_ir(rshr_ul)
entry_ir_ir(negr_i)		entry_ir_ir(negr_l)
entry_fr_fr(negr_f)		entry_fr_fr(negr_d)
entry_ir_ir(notr_i)		entry_ir_ir(notr_ui)
entry_ir_ir(notr_l)		entry_ir_ir(notr_ul)
entry_ir_ir_im(lti_i)		entry_ir_ir_im(lti_ui)
entry_ir_ir_im(lti_l)		entry_ir_ir_im(lti_ul)
entry_ir_ir_im(lti_p)
entry_ir_ir_ir(ltr_i)		entry_ir_ir_ir(ltr_ui)
entry_ir_ir_ir(ltr_l)		entry_ir_ir_ir(ltr_ul)
entry_ir_ir_ir(ltr_p)
entry_ir_fr_fr(ltr_f)		entry_ir_fr_fr(ltr_d)
entry_ir_ir_im(lei_i)		entry_ir_ir_im(lei_ui)
entry_ir_ir_im(lei_l)		entry_ir_ir_im(lei_ul)
entry_ir_ir_im(lei_p)
entry_ir_ir_ir(ler_i)		entry_ir_ir_ir(ler_ui)
entry_ir_ir_ir(ler_l)		entry_ir_ir_ir(ler_ul)
entry_ir_ir_ir(ler_p)
entry_ir_fr_fr(ler_f)		entry_ir_fr_fr(ler_d)
entry_ir_ir_im(gti_i)		entry_ir_ir_im(gti_ui)
entry_ir_ir_im(gti_l)		entry_ir_ir_im(gti_ul)
entry_ir_ir_im(gti_p)
entry_ir_ir_ir(gtr_i)		entry_ir_ir_ir(gtr_ui)
entry_ir_ir_ir(gtr_l)		entry_ir_ir_ir(gtr_ul)
entry_ir_ir_ir(gtr_p)
entry_ir_fr_fr(gtr_f)		entry_ir_fr_fr(gtr_d)
entry_ir_ir_im(gei_i)		entry_ir_ir_im(gei_ui)
entry_ir_ir_im(gei_l)		entry_ir_ir_im(gei_ul)
entry_ir_ir_im(gei_p)
entry_ir_ir_ir(ger_i)		entry_ir_ir_ir(ger_ui)
entry_ir_ir_ir(ger_l)		entry_ir_ir_ir(ger_ul)
entry_ir_ir_ir(ger_p)
entry_ir_fr_fr(ger_f)		entry_ir_fr_fr(ger_d)
entry_ir_ir_im(eqi_i)		entry_ir_ir_im(eqi_ui)
entry_ir_ir_im(eqi_l)		entry_ir_ir_im(eqi_ul)
entry_ir_ir_im(eqi_p)
entry_ir_ir_ir(eqr_i)		entry_ir_ir_ir(eqr_ui)
entry_ir_ir_ir(eqr_l)		entry_ir_ir_ir(eqr_ul)
entry_ir_ir_ir(eqr_p)
entry_ir_fr_fr(eqr_f)		entry_ir_fr_fr(eqr_d)
entry_ir_ir_im(nei_i)		entry_ir_ir_im(nei_ui)
entry_ir_ir_im(nei_l)		entry_ir_ir_im(nei_ul)
entry_ir_ir_im(nei_p)
entry_ir_ir_ir(ner_i)		entry_ir_ir_ir(ner_ui)
entry_ir_ir_ir(ner_l)		entry_ir_ir_ir(ner_ul)
entry_ir_ir_ir(ner_p)
entry_ir_fr_fr(ner_f)		entry_ir_fr_fr(ner_d)
entry_ir_fr_fr(unltr_f)		entry_ir_fr_fr(unltr_d)
entry_ir_fr_fr(unler_f)		entry_ir_fr_fr(unler_d)
entry_ir_fr_fr(ungtr_f)		entry_ir_fr_fr(ungtr_d)
entry_ir_fr_fr(unger_f)		entry_ir_fr_fr(unger_d)
entry_ir_fr_fr(uneqr_f)		entry_ir_fr_fr(uneqr_d)
entry_ir_fr_fr(ltgtr_f)		entry_ir_fr_fr(ltgtr_d)
entry_ir_fr_fr(ordr_f)		entry_ir_fr_fr(ordr_d)
entry_ir_fr_fr(unordr_f)	entry_ir_fr_fr(unordr_d)
entry_ir_um(movi_i)		entry_ir_um(movi_ui)
entry_ir_im(movi_l)		entry_ir_um(movi_ul)
static void
movi_p(void)
{
    int		 ch;
    label_t	*label;
    void	*value;
    jit_gpr_t	 r0 = get_ireg();
    ch = skipws();
    switch (ch) {
	case '0' ... '9':
	    ungetch(ch);
	    value = (void *)(long)get_uint(skip_none);
	    break;
	case '$':
	    switch (expression()) {
		case tok_int:
		    value = (void *)parser.value.i;
		    break;
		case tok_pointer:
		    value = parser.value.p;
		    break;
		default:
		    error("expecting pointer");
	    }
	    break;
#if DYNAMIC
	case '@':
	    dynamic();
	    value = parser.value.p;
	    break;
#endif
	default:
	    ungetch(ch);
	    label = get_label(skip_none);
	    if (label->kind == label_kind_code_forward) {
		mov_forward((void *)jit_movi_p(r0, jit_forward()), label);
		return;
	    }
	    value = label->value;
	    break;
    }
    (void)jit_movi_p(r0, value);
}
entry_fr_fm(movi_f)		entry_fr_fm(movi_d)
entry_ir_ir(movr_i)		entry_ir_ir(movr_ui)
entry_ir_ir(movr_l)		entry_ir_ir(movr_ul)
entry_ir_ir(movr_p)
entry_fr_fr(movr_f)		entry_fr_fr(movr_d)
entry_ir_ir(extr_c_s)		entry_ir_ir(extr_c_us)
entry_ir_ir(extr_c_i)		entry_ir_ir(extr_c_ui)
entry_ir_ir(extr_c_l)		entry_ir_ir(extr_c_ul)
entry_ir_ir(extr_uc_s)		entry_ir_ir(extr_uc_us)
entry_ir_ir(extr_uc_i)		entry_ir_ir(extr_uc_ui)
entry_ir_ir(extr_uc_l)		entry_ir_ir(extr_uc_ul)
entry_ir_ir(extr_s_i)		entry_ir_ir(extr_s_ui)
entry_ir_ir(extr_s_l)		entry_ir_ir(extr_s_ul)
entry_ir_ir(extr_us_i)		entry_ir_ir(extr_us_ui)
entry_ir_ir(extr_us_l)		entry_ir_ir(extr_us_ul)
entry_ir_ir(extr_i_l)		entry_ir_ir(extr_i_ul)
entry_ir_ir(extr_ui_l)		entry_ir_ir(extr_ui_ul)
entry_fr_ir(extr_i_f)		entry_fr_ir(extr_i_d)
entry_fr_ir(extr_l_f)		entry_fr_ir(extr_l_d)
entry_fr_fr(extr_f_d)		entry_fr_fr(extr_d_f)
#if defined(jit_rintr_d_i)
entry_ir_fr(rintr_f_i)		entry_ir_fr(rintr_d_i)
entry_ir_fr(rintr_f_l)		entry_ir_fr(rintr_d_l)
#endif
entry_ir_fr(roundr_f_i)		entry_ir_fr(roundr_d_i)
entry_ir_fr(truncr_f_i)		entry_ir_fr(truncr_d_i)
entry_ir_fr(floorr_f_i)		entry_ir_fr(floorr_d_i)
entry_ir_fr(ceilr_f_i)		entry_ir_fr(ceilr_d_i)
entry_ir_fr(roundr_f_l)		entry_ir_fr(roundr_d_l)
entry_ir_fr(truncr_f_l)		entry_ir_fr(truncr_d_l)
entry_ir_fr(floorr_f_l)		entry_ir_fr(floorr_d_l)
entry_ir_fr(ceilr_f_l)		entry_ir_fr(ceilr_d_l)
entry_ir_ir(hton_us)		entry_ir_ir(hton_ui)
entry_ir_ir(ntoh_us)		entry_ir_ir(ntoh_ui)
entry_ir_pm(ldi_c)		entry_ir_pm(ldi_uc)
entry_ir_pm(ldi_s)		entry_ir_pm(ldi_us)
entry_ir_pm(ldi_i)		entry_ir_pm(ldi_ui)
entry_ir_pm(ldi_l)		entry_ir_pm(ldi_ul)
entry_ir_pm(ldi_p)
entry_fr_pm(ldi_f)		entry_fr_pm(ldi_d)
entry_ir_ir(ldr_c)		entry_ir_ir(ldr_uc)
entry_ir_ir(ldr_s)		entry_ir_ir(ldr_us)
entry_ir_ir(ldr_i)		entry_ir_ir(ldr_ui)
entry_ir_ir(ldr_l)		entry_ir_ir(ldr_ul)
entry_ir_ir(ldr_p)
entry_fr_ir(ldr_f)		entry_fr_ir(ldr_d)
entry_ir_ir_im(ldxi_c)		entry_ir_ir_im(ldxi_uc)
entry_ir_ir_im(ldxi_s)		entry_ir_ir_im(ldxi_us)
entry_ir_ir_im(ldxi_i)		entry_ir_ir_im(ldxi_ui)
entry_ir_ir_im(ldxi_l)		entry_ir_ir_im(ldxi_ul)
entry_ir_ir_im(ldxi_p)
entry_fr_ir_im(ldxi_f)		entry_fr_ir_im(ldxi_d)
entry_ir_ir_ir(ldxr_c)		entry_ir_ir_ir(ldxr_uc)
entry_ir_ir_ir(ldxr_s)		entry_ir_ir_ir(ldxr_us)
entry_ir_ir_ir(ldxr_i)		entry_ir_ir_ir(ldxr_ui)
entry_ir_ir_ir(ldxr_l)		entry_ir_ir_ir(ldxr_ul)
entry_ir_ir_ir(ldxr_p)
entry_fr_ir_ir(ldxr_f)		entry_fr_ir_ir(ldxr_d)
entry_pm_ir(sti_c)		entry_pm_ir(sti_uc)
entry_pm_ir(sti_s)		entry_pm_ir(sti_us)
entry_pm_ir(sti_i)		entry_pm_ir(sti_ui)
entry_pm_ir(sti_l)		entry_pm_ir(sti_ul)
entry_pm_ir(sti_p)
entry_pm_fr(sti_f)		entry_pm_fr(sti_d)
entry_ir_ir(str_c)		entry_ir_ir(str_uc)
entry_ir_ir(str_s)		entry_ir_ir(str_us)
entry_ir_ir(str_i)		entry_ir_ir(str_ui)
entry_ir_ir(str_l)		entry_ir_ir(str_ul)
entry_ir_ir(str_p)
entry_ir_fr(str_f)		entry_ir_fr(str_d)
entry_im_ir_ir(stxi_c)		entry_im_ir_ir(stxi_uc)
entry_im_ir_ir(stxi_s)		entry_im_ir_ir(stxi_us)
entry_im_ir_ir(stxi_i)		entry_im_ir_ir(stxi_ui)
entry_im_ir_ir(stxi_l)		entry_im_ir_ir(stxi_ul)
entry_im_ir_ir(stxi_p)
entry_im_ir_fr(stxi_f)		entry_im_ir_fr(stxi_d)
entry_ir_ir_ir(stxr_c)		entry_ir_ir_ir(stxr_uc)
entry_ir_ir_ir(stxr_s)		entry_ir_ir_ir(stxr_us)
entry_ir_ir_ir(stxr_i)		entry_ir_ir_ir(stxr_ui)
entry_ir_ir_ir(stxr_l)		entry_ir_ir_ir(stxr_ul)
entry_ir_ir_ir(stxr_p)
entry_ir_ir_fr(stxr_f)		entry_ir_ir_fr(stxr_d)
entry_lb_ir_im(blti_i)		entry_lb_ir_im(blti_ui)
entry_lb_ir_im(blti_l)		entry_lb_ir_im(blti_ul)
entry_lb_ir_im(blti_p)
entry_lb_ir_ir(bltr_i)		entry_lb_ir_ir(bltr_ui)
entry_lb_ir_ir(bltr_l)		entry_lb_ir_ir(bltr_ul)
entry_lb_ir_ir(bltr_p)
entry_lb_fr_fr(bltr_f)		entry_lb_fr_fr(bltr_d)
entry_lb_ir_im(blei_i)		entry_lb_ir_im(blei_ui)
entry_lb_ir_im(blei_l)		entry_lb_ir_im(blei_ul)
entry_lb_ir_im(blei_p)
entry_lb_ir_ir(bler_i)		entry_lb_ir_ir(bler_ui)
entry_lb_ir_ir(bler_l)		entry_lb_ir_ir(bler_ul)
entry_lb_ir_ir(bler_p)
entry_lb_fr_fr(bler_f)		entry_lb_fr_fr(bler_d)
entry_lb_ir_im(bgti_i)		entry_lb_ir_im(bgti_ui)
entry_lb_ir_im(bgti_l)		entry_lb_ir_im(bgti_ul)
entry_lb_ir_im(bgti_p)
entry_lb_ir_ir(bgtr_i)		entry_lb_ir_ir(bgtr_ui)
entry_lb_ir_ir(bgtr_l)		entry_lb_ir_ir(bgtr_ul)
entry_lb_ir_ir(bgtr_p)
entry_lb_fr_fr(bgtr_f)		entry_lb_fr_fr(bgtr_d)
entry_lb_ir_im(bgei_i)		entry_lb_ir_im(bgei_ui)
entry_lb_ir_im(bgei_l)		entry_lb_ir_im(bgei_ul)
entry_lb_ir_im(bgei_p)
entry_lb_ir_ir(bger_i)		entry_lb_ir_ir(bger_ui)
entry_lb_ir_ir(bger_l)		entry_lb_ir_ir(bger_ul)
entry_lb_ir_ir(bger_p)
entry_lb_fr_fr(bger_f)		entry_lb_fr_fr(bger_d)
entry_lb_ir_im(beqi_i)		entry_lb_ir_im(beqi_ui)
entry_lb_ir_im(beqi_l)		entry_lb_ir_im(beqi_ul)
entry_lb_ir_im(beqi_p)
entry_lb_ir_ir(beqr_i)		entry_lb_ir_ir(beqr_ui)
entry_lb_ir_ir(beqr_l)		entry_lb_ir_ir(beqr_ul)
entry_lb_ir_ir(beqr_p)
entry_lb_fr_fr(beqr_f)		entry_lb_fr_fr(beqr_d)
entry_lb_ir_im(bnei_i)		entry_lb_ir_im(bnei_ui)
entry_lb_ir_im(bnei_l)		entry_lb_ir_im(bnei_ul)
entry_lb_ir_im(bnei_p)
entry_lb_ir_ir(bner_i)		entry_lb_ir_ir(bner_ui)
entry_lb_ir_ir(bner_l)		entry_lb_ir_ir(bner_ul)
entry_lb_ir_ir(bner_p)
entry_lb_fr_fr(bner_f)		entry_lb_fr_fr(bner_d)
entry_lb_fr_fr(bunltr_f)	entry_lb_fr_fr(bunltr_d)
entry_lb_fr_fr(bunler_f)	entry_lb_fr_fr(bunler_d)
entry_lb_fr_fr(bungtr_f)	entry_lb_fr_fr(bungtr_d)
entry_lb_fr_fr(bunger_f)	entry_lb_fr_fr(bunger_d)
entry_lb_fr_fr(buneqr_f)	entry_lb_fr_fr(buneqr_d)
entry_lb_fr_fr(bltgtr_f)	entry_lb_fr_fr(bltgtr_d)
entry_lb_fr_fr(bordr_f)		entry_lb_fr_fr(bordr_d)
entry_lb_fr_fr(bunordr_f)	entry_lb_fr_fr(bunordr_d)
entry_lb_ir_im(bmsi_i)		entry_lb_ir_im(bmsi_ui)
entry_lb_ir_im(bmsi_l)		entry_lb_ir_im(bmsi_ul)
entry_lb_ir_ir(bmsr_i)		entry_lb_ir_ir(bmsr_ui)
entry_lb_ir_ir(bmsr_l)		entry_lb_ir_ir(bmsr_ul)
entry_lb_ir_im(bmci_i)		entry_lb_ir_im(bmci_ui)
entry_lb_ir_im(bmci_l)		entry_lb_ir_im(bmci_ul)
entry_lb_ir_ir(bmcr_i)		entry_lb_ir_ir(bmcr_ui)
entry_lb_ir_ir(bmcr_l)		entry_lb_ir_ir(bmcr_ul)
entry_lb_ir_im(boaddi_i)	entry_lb_ir_im(boaddi_ui)
entry_lb_ir_im(boaddi_l)	entry_lb_ir_im(boaddi_ul)
entry_lb_ir_ir(boaddr_i)	entry_lb_ir_ir(boaddr_ui)
entry_lb_ir_ir(boaddr_l)	entry_lb_ir_ir(boaddr_ul)
entry_lb_ir_im(bosubi_i)	entry_lb_ir_im(bosubi_ui)
entry_lb_ir_im(bosubi_l)	entry_lb_ir_im(bosubi_ul)
entry_lb_ir_ir(bosubr_i)	entry_lb_ir_ir(bosubr_ui)
entry_lb_ir_ir(bosubr_l)	entry_lb_ir_ir(bosubr_ul)
entry_im(prepare)
entry_im(prepare_f)		entry_im(prepare_d)
entry_ir(pusharg_c)		entry_ir(pusharg_uc)
entry_ir(pusharg_s)		entry_ir(pusharg_us)
entry_ir(pusharg_i)		entry_ir(pusharg_ui)
entry_ir(pusharg_l)		entry_ir(pusharg_ul)
entry_ir(pusharg_p)
entry_fr(pusharg_f)		entry_fr(pusharg_d)
entry_ia(getarg_c)		entry_ia(getarg_uc)
entry_ia(getarg_s)		entry_ia(getarg_us)
entry_ia(getarg_i)		entry_ia(getarg_ui)
entry_ia(getarg_l)		entry_ia(getarg_ul)
entry_ia(getarg_p)
entry_fa(getarg_f)		entry_fa(getarg_d)
entry_ca(arg_c)			entry_ca(arg_uc)
entry_ca(arg_s)			entry_ca(arg_us)
entry_ca(arg_i)			entry_ca(arg_ui)
entry_ca(arg_l)			entry_ca(arg_ul)
entry_ca(arg_p)
entry_ca(arg_f)			entry_ca(arg_d)
#if CALLI_PATCH
entry_lf(calli)			entry_lf(finish)
#else
entry_pm(calli)			entry_pm(finish)
#endif
entry_ir(callr)
entry_ir(finishr)
entry_lb(jmpi);			entry_ir(jmpr)
entry(ret)
entry_ir(retval_c)		entry_ir(retval_uc)
entry_ir(retval_s)		entry_ir(retval_us)
entry_ir(retval_i)		entry_ir(retval_ui)
entry_ir(retval_l)		entry_ir(retval_ul)
entry_ir(retval_p)
entry_fr(retval_f)		entry_fr(retval_d)
entry_im(prolog)
entry_im(prolog_f)		entry_im(prolog_d)
entry_im(leaf)
entry_im(leaf_f)		entry_im(leaf_d)
void
allocai(void) {
    symbol_t	*symbol;
    long	 i, im = get_int(skip_ws);
    i = jit_allocai(im);
    symbol = get_symbol();
    symbol->type = type_l;
    symbol->value.i = i;
}
#if defined(jit_absr_f)
entry_fr_fr(absr_f)		entry_fr_fr(absr_d)
#endif
#if defined(jit_sqrtr_f)
entry_fr_fr(sqrtr_f)		entry_fr_fr(sqrtr_d)
#endif
#if defined(jit_sinr_f)
entry_fr_fr(sinr_f)		entry_fr_fr(sinr_d)
entry_fr_fr(cosr_f)		entry_fr_fr(cosr_d)
entry_fr_fr(tanr_f)		entry_fr_fr(tanr_d)
entry_fr_fr(atanr_f)		entry_fr_fr(atanr_d)
entry_fr_fr(logr_f)		entry_fr_fr(logr_d)
entry_fr_fr(log2r_f)		entry_fr_fr(log2r_d)
entry_fr_fr(log10r_f)		entry_fr_fr(log10r_d)
#endif

static void
error(const char *format, ...)
{
    va_list	 ap;
    int		 length;
    char	*string;

    va_start(ap, format);
    message("error", format, ap);
    va_end(ap);
    length = parser.data.length - parser.data.offset;
    string = (char *)(parser.data.buffer + parser.data.offset - 1);
    if (length > 77)
	strcpy(string + 74, "...");
    else
	parser.data.buffer[parser.data.length - 1] = '\0';
    fprintf(stderr, "(%s)\n", string);
    exit(-1);
}

static void
warn(const char *format, ...)
{
    va_list	ap;
    va_start(ap, format);
    message("warning", format, ap);
    va_end(ap);
}

static void
message(const char *kind, const char *format, va_list ap)
{
    fprintf(stderr, "%s:%d: %s: ", parser.name,
	    parser.line - parser.newline, kind);
    vfprintf(stderr, format, ap);
    fputc('\n', stderr);
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
    if ((parser.newline = ch == '\n'))
	++parser.line;

    return (ch);
}

static int
getch_noeof(void)
{
    int		ch = getch();

    if (ch == EOF)
	error("unexpected end of file");

    return (ch);
}

static int
ungetch(int ch)
{
    if ((parser.newline = ch == '\n'))
	--parser.line;

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
	    case ' ': case '\f': case '\r': case '\t':
		break;
	    default:
		return (ch);
	}
    }
}

static int
skipnl(void)
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
	    case ' ': case '\f': case '\n': case '\r': case '\t':
		break;
		/* handle as newline */
	    case ';':
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
		    parser.line = parser.value.i - 1;
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

static long
get_int(skip_t skip)
{
    switch (primary(skip)) {
	case tok_int:
	    break;
#if 0
	case tok_float:
	    parser.value.i = parser.value.d;
	    break;
#endif
	case tok_pointer:
	    parser.type = type_l;
	    parser.value.i = (long)parser.value.p;
	    break;
	default:
	    error("expecting integer");
    }

    return (parser.value.i);
}

static unsigned long
get_uint(skip_t skip)
{
    switch (primary(skip)) {
	case tok_char:		case tok_int:
	    break;
#if 0
	case tok_float:
	    parser.type = type_l;
	    parser.value.ui = parser.value.d;
	    break;
#endif
	case tok_pointer:
	    parser.type = type_l;
	    parser.value.ui = (unsigned long)parser.value.p;
	    break;
	default:
	    error("expecting integer");
    }

    return (parser.value.ui);
}

static double
get_float(skip_t skip)
{
    switch (primary(skip)) {
	case tok_char:
	case tok_int:
	    parser.type = type_d;
	    parser.value.d = parser.value.i;
	    break;
	case tok_float:
	    break;
#if 0
	case tok_pointer:
	    parser.type = type_d;
	    parser.value.d = (double)(long)parser.value.p;
	    break;
#endif
	default:
	    error("expecting float");
    }

    return (parser.value.d);
}

static void *
get_pointer(skip_t skip)
{
    label_t	*label;
    token_t	 token = primary(skip);

    switch (token) {
	case tok_symbol:
	    label = get_label_by_name(parser.string);
	    if (label == NULL)
		error("bad identifier %s", parser.string);
	    switch (label->kind) {
		case label_kind_data:
		case label_kind_code:
		    break;
		case label_kind_code_forward:
		    /* as expression arguments */
		    error("forward references not implemented");
		    break;
#if DYNAMIC
		case label_kind_dynamic:
		    break;
#endif
	    }
	    parser.type = type_p;
	    return (parser.value.p = label->value);
	case tok_int:
	    parser.type = type_p;
	    return (parser.value.p = (void *)parser.value.ui);
	case tok_pointer:
	    return (parser.value.p);
	default:		error("bad pointer");
    }
}

static label_t *
get_label(skip_t skip)
{
    label_t	*label;
    int		 ch = skipws();

    switch (ch) {
#if DYNAMIC
	case '@':
	    (void)dynamic();
	    break;
#endif
	case 'a' ... 'z': case 'A' ... 'Z': case '_':
	    (void)identifier(ch);
	    break;
	default:
	    error("expecting label");
    }
    if ((label = get_label_by_name(parser.string)) == NULL)
	label = new_label(label_kind_code_forward,
			  parser.string, jit_forward());

    return (label);
}

static token_t
regname(void)
{
    long	num;
    int		check = 1, ch = getch();

    switch (ch) {
	case 'r':
	    parser.regtype = type_l;
	    switch (ch = getch()) {
		case '0':	parser.regval = JIT_R0;	break;
		case '1':	parser.regval = JIT_R1;	break;
		case '2':	parser.regval = JIT_R2;	break;
		case 'e':
		    if (getch() != 't')			goto fail;
		    parser.regval = JIT_RET;
		    break;
		case '(':
		    num = get_int(skip_none);
		    if (num < 0 || num >= JIT_R_NUM)	goto fail;
		    parser.regval = JIT_R(num);
		    if (getch() != ')')			goto fail;
		    check = 0;
		    break;
		default:				goto fail;
	    }
	    break;
	case 'v':
	    parser.regtype = type_l;
	    switch (ch = getch()) {
		case '0':	parser.regval = JIT_V0;	break;
		case '1':	parser.regval = JIT_V1;	break;
		case '2':	parser.regval = JIT_V2;	break;
		default:				goto fail;
		case '(':
		    num = get_int(skip_none);
		    if (num < 0 || num >= JIT_V_NUM)	goto fail;
		    parser.regval = JIT_V(num);
		    if (getch() != ')')			goto fail;
		    check = 0;
		    break;
	    }
	    break;
	case 'f':
	    parser.regtype = type_d;
	    switch (ch = getch()) {
		case '0':	parser.regval = JIT_FPR0;	break;
		case '1':	parser.regval = JIT_FPR1;	break;
		case '2':	parser.regval = JIT_FPR2;	break;
		case '3':	parser.regval = JIT_FPR3;	break;
		case '4':	parser.regval = JIT_FPR4;	break;
		case '5':	parser.regval = JIT_FPR5;	break;
		case 'p':
		    parser.regtype = type_l;	/* oops */
		    parser.regval = JIT_FP;			break;
		case 'r':
		    if (getch() != 'e' || getch() != 't')	goto fail;
		    parser.regval = JIT_FPRET;
		    break;
		case '(':
		    num = get_int(skip_none);
		    if (num < 0 || num >= JIT_FPR_NUM)		goto fail;
		    parser.regval = JIT_FPR(num);
		    if (getch() != ')')				goto fail;
		    check = 0;
		    break;
		default:					goto fail;
	    }
	    break;
	case 's':
	    parser.regtype = type_l;
	    if (getch() != 'p')
		goto fail;
	    parser.regval = JIT_SP;
	    break;
	default:
	fail:
	    error("bad register");
    }
    if (check) {
	ch = getch();
	if ((ch >= 'a' && ch <= 'z') ||
	    (ch >= 'A' && ch <= 'Z') ||
	    (ch >= '0' && ch <= '9') ||
	    ch == '_')
	    goto fail;
	ungetch(ch);
    }

    return (tok_register);
}

static int
qcmp_instrs(const void *left, const void *right)
{
    return (strcmp(((instr_t *)left)->name, ((instr_t *)right)->name));
}

static token_t
identifier(int ch)
{
    parser.string[0] = ch;
    for (parser.offset = 1;;) {
	switch ((ch = getch())) {
	    case 'a' ... 'z': case 'A' ... 'Z': case '0' ... '9' :  case '_':
		if (parser.offset + 1 >= MAX_IDENTIFIER) {
		    parser.string[parser.offset] = '\0';
		    error("bad identifier %s", parser.string);
		}
		parser.string[parser.offset++] = ch;
		break;
	    default:
		parser.string[parser.offset] = '\0';
		ungetch(ch);
		return (tok_symbol);
	}
    }
}

static void
get_data(type_t type)
{
    int		 ch;
    token_t	 token;
    char	*test = data;

    for (;;) {
	if (data == NULL)	error("data must be after .data");
	switch (type) {
	    case type_c:
		switch (token = primary(skip_ws)) {
		    case tok_char: case tok_int:
			*(signed char *)data = parser.value.i;
			data += sizeof(char);
			break;
		    case tok_string:
			memcpy(data, parser.string, parser.offset);
			data += parser.offset;
			break;
		    case tok_newline:
		    case tok_semicollon:
			if (test == data)	error("syntax error");
			return;
		    default:			error("bad initializer");
		}
		break;
	    case type_s:
		*(signed short *)data = get_int(skip_ws);
		data += sizeof(short);
		break;
	    case type_i:
		*(signed int *)data = get_int(skip_ws);
		data += sizeof(int);
		break;
	    case type_l:
		*(signed long *)data = get_int(skip_ws);
		data += sizeof(long);
		break;
	    case type_f:
		*(float *)data = get_float(skip_ws);
		data += sizeof(float);
		break;
	    case type_d:
		*(double *)data = get_float(skip_ws);
		data += sizeof(double);
		break;
	    case type_p:
		*(void **)data = get_pointer(skip_ws);
		data += sizeof(void*);
		break;
	    default:
		abort();
	}
	check_data();
	ch = skipws();
	if (ch == '\n' || ch == ';' || ch == EOF)
	    break;
	ungetch(ch);
    }
}

static void
dot(void)
{
    int		ch;
    size_t	size, value;

    switch (ch = getch_noeof()) {
	case '$':
	    /* use .$(expression) for non side effects expression */
	    (void)expression();
	    return;
	case 'a' ... 'z': case 'A' ... 'Z': case '_':
	    (void)identifier(ch);
	    break;
	default:
	    if (skipws() != '$')
		error("expecting symbol");
	    /* allow spaces before an expression */
	    (void)expression();
	    return;
    }
    if (parser.string[1] == '\0') {
	switch (parser.string[0]) {
	    case 'c':	get_data(type_c);	break;
	    case 's':	get_data(type_s);	break;
	    case 'i':	get_data(type_i);	break;
	    case 'l':	get_data(type_l);	break;
	    case 'f':	get_data(type_f);	break;
	    case 'd':	get_data(type_d);	break;
	    case 'p':	get_data(type_p);	break;
	    default:	error("bad type .%c", parser.string[0]);
	}
    }
    else if (strcmp(parser.string, "data") == 0) {
	if (data_start != NULL)	error(".data must be specified once only");
	size = get_int(skip_ws);
	data_start = data = (char *)xcalloc(1, size);
	data_end = data_start + size;
    }
    else if (strcmp(parser.string, "code") == 0) {
	if (code_start != NULL)	error(".code must be specified once only");
	size = get_int(skip_ws);
	code_start = (char *)xmalloc(size);
	code_end = code_start + size;
	(void)jit_set_ip((jit_insn *)code_start);
    }
    else if (strcmp(parser.string, "align") == 0) {
	size = get_int(skip_ws);
	if (data_start == NULL)	error(".align must be after .data");
	if (code_start != NULL)	error(".align must be before .code");
	if (size > 1 && size <= 4096 && !(size & (size - 1))) {
	    value = (unsigned long)data;
	    data += size - ((value + size) % size);
	    check_data();
	}
	else
	    error("bad .align %ld (must be a power of 2, >= 2 && <= 4096)",
		  (long)size);
    }
    else if (strcmp(parser.string, "size") == 0) {
	size = get_int(skip_ws);
	if (data_start == NULL)	error(".size must be after .data");
	if (code_start != NULL)	error(".size must be before .code");
	data += size;
	check_data();
    }
}

static token_t
number(int ch)
{
    char	buffer[1024], *endptr;
    int		integer = 1, offset = 0, neg = 0, e = 0, d = 0, base = 10;

    for (;; ch = getch()) {
	switch (ch) {
	    case '-':
		if (offset == 0) {
		    neg = 1;
		    continue;
		}
		if (offset && buffer[offset - 1] != 'e') {
		    ungetch(ch);
		    goto done;
		}
		break;
	    case '+':
		if (offset == 0)
		    continue;
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
		error("bad constant %s", buffer);
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
	parser.value.ui = strtoul(buffer, &endptr, base);
	parser.type = type_l;
	if (neg)
	    parser.value.i = -parser.value.i;
    }
    else {
	parser.type = type_d;
	parser.value.d = strtod(buffer, &endptr);
	if (neg)
	    parser.value.d = -parser.value.d;
    }
    if (*endptr)
	goto fail;

    return (integer ? tok_int : tok_float);
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
		    parser.value.p = parser.string;
		    parser.type = type_p;
		    return (tok_string);
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
    int		ch, esc = 0;

    if ((ch = getch_noeof()) == '\\') {
	esc = 1;
	ch = getch();
    }
    if (getch_noeof() != '\'')
	error("bad single byte char");
    if (esc)
	ch = escape(ch);
    parser.type = type_l;
    parser.value.i = ch & 0xff;

    return (tok_char);
}

#if DYNAMIC
static token_t
dynamic(void)
{
    label_t	*label;
    void	*value;
    char	*string;
    (void)identifier('@');
    if ((label = get_label_by_name(parser.string)) == NULL) {
	value = dlsym(NULL, parser.string + 1);
	if ((string = dlerror()))
	    error("%s", string);
	label = new_label(label_kind_dynamic, parser.string, value);
    }
    parser.type = type_p;
    parser.value.p = label->value;

    return (tok_pointer);
}
#endif

static void
expression_prim(void)
{
    int		 ch;
    token_t	 token;
    label_t	*label;
    symbol_t	*symbol;

    if (parser.putback) {
	parser.expr = parser.putback;
	parser.putback = (expr_t)0;
	return;
    }
    switch (ch = skipws()) {
	case '!':
	    if ((ch = getch_noeof()) == '=')	parser.expr = expr_ne;
	    else {
		ungetch(ch);			parser.expr = expr_not;
	    }
	    break;
	case '~':				parser.expr = expr_com;
	    break;
	case '*':
	    if ((ch = getch_noeof()) == '=')	parser.expr = expr_mulset;
	    else {
		ungetch(ch);			parser.expr = expr_mul;
	    }
	    break;
	case '/':
	    if ((ch = getch_noeof()) == '=')	parser.expr = expr_divset;
	    else {
		ungetch(ch);			parser.expr = expr_div;
	    }
	    break;
	case '%':
	    if ((ch = getch_noeof()) == '=')	parser.expr = expr_remset;
	    else {
		ungetch(ch);			parser.expr = expr_rem;
	    }
	    break;
	case '+':
	    switch (ch = getch_noeof()) {
		case '+':			parser.expr = expr_inc;
		    break;
		case '=':			parser.expr = expr_addset;
		    break;
		default:	ungetch(ch);	parser.expr = expr_add;
		    break;
	    }
	    break;
	case '-':
	    switch (ch = getch_noeof()) {
		case '-':			parser.expr = expr_dec;
		    break;
		case '=':			parser.expr = expr_subset;
		    break;
		default:	ungetch(ch);	parser.expr = expr_sub;
		    break;
	    }
	    break;
	case '<':
	    switch (ch = getch_noeof()) {
		case '=':			parser.expr = expr_le;
		    break;
		case '<':			ch = getch_noeof();
		    if (ch == '=')		parser.expr = expr_lshset;
		    else {
			ungetch(ch);		parser.expr = expr_lsh;
		    }
		    break;
		default:	ungetch(ch);	parser.expr = expr_lt;
		    break;
	    }
	    break;
	case '>':
	    switch (ch = getch_noeof()) {
		case '=':			parser.expr = expr_ge;
		    break;
		case '>':			ch = getch_noeof();
		    if (ch == '=')		parser.expr = expr_rshset;
		    else {
			ungetch(ch);		parser.expr = expr_rsh;
		    }
		    break;
		default:	ungetch(ch);	parser.expr = expr_gt;
		    break;
	    }
	    break;
	case '&':
	    switch (ch = getch_noeof()) {
		case '=':			parser.expr = expr_andset;
		    break;
		case '&':			parser.expr = expr_andand;
		    break;
		default:	ungetch(ch);	parser.expr = expr_and;
		    break;
	    }
	    break;
	case '|':
	    switch (ch = getch_noeof()) {
		case '=':			parser.expr = expr_orset;
		    break;
		case '|':			parser.expr = expr_oror;
		    break;
		default:	ungetch(ch);	parser.expr = expr_or;
		    break;
	    }
	    break;
	case '^':
	    if ((ch = getch_noeof()) == '=')	parser.expr = expr_xorset;
	    else {
		ungetch(ch);			parser.expr = expr_xor;
	    }
	    break;
	case '=':
	    if ((ch = getch_noeof()) == '=')	parser.expr = expr_eq;
	    else {
		ungetch(ch);			parser.expr = expr_set;
	    }
	    break;
	case '(':				parser.expr = expr_lparen;
	    break;
	case ')':				parser.expr = expr_rparen;
	    break;
	case '0' ... '9':
	    token = number(ch);
	    parser.expr = token == tok_int ? expr_int : expr_float;
	    break;
#if DYNAMIC
	case '@':
	    (void)dynamic();
	    parser.expr = expr_pointer;
	    break;
#endif
	case '$':
	    identifier('$');
	    /* no support for nested expressions */
	    if (parser.string[0] == '\0')
		error("syntax error");
	    parser.expr = expr_symbol;
	    if ((symbol = get_symbol_by_name(parser.string)) != NULL) {
		parser.type = symbol->type;
		parser.value = symbol->value;
	    }
	    else
		/* only create symbol on assignment */
		parser.type = type_none;
	    break;
	case 'a' ... 'z': case 'A' ... 'Z': case '_':
	    identifier(ch);
	    if ((label = get_label_by_name(parser.string))) {
		if (label->kind == label_kind_code_forward)
		    error("forward value for %s not supported",
			  parser.string);
		parser.expr = expr_pointer;
		parser.type = type_p;
		parser.value.p = label->value;
	    }
	    else
		error("invalid identifier %s", parser.string);
	    break;
	case '\'':
	    character();
	    parser.expr = expr_int;
	    break;
	case '"':
	    /* not smart enough to put it in data and/or relocate it, etc */
	    error("must declare strings as data");
	default:
	    error("syntax error");
    }
}

static void
expression_inc(int pre)
{
    symbol_t	*symbol;

    if (pre) {
	expression_prim();
	if (parser.expr != expr_symbol)
	    error("syntax error");
    }
    if ((symbol = get_symbol_by_name(parser.string)) == NULL) {
	if (!parser.short_circuit)
	    error("undefined symbol %s", symbol->name);
    }
    if (!parser.short_circuit) {
	parser.type = symbol->type;
	if (!pre)
	    parser.value = symbol->value;
	switch (symbol->type) {
	    case type_l:
		++symbol->value.i;
		break;
	    case type_d:
		/* should really be an error */
		symbol->value.d += 1.0;
		break;
	    default:
		++parser.value.cp;
		break;
	}
	if (pre)
	    parser.value = symbol->value;
    }
    expression_prim();
}

static void
expression_dec(int pre)
{
    symbol_t	*symbol;

    if (pre) {
	expression_prim();
	if (parser.expr != expr_symbol)
	    error("syntax error");
    }
    if ((symbol = get_symbol_by_name(parser.string)) == NULL) {
	if (!parser.short_circuit)
	    error("undefined symbol %s", symbol->name);
    }
    if (!parser.short_circuit) {
	parser.type = symbol->type;
	if (!pre)
	    parser.value = symbol->value;
	switch (symbol->type) {
	    case type_l:
		--symbol->value.i;
		break;
	    case type_d:
		/* should really be an error */
		symbol->value.d -= 1.0;
		break;
	    default:
		--parser.value.cp;
		break;
	}
	if (pre)
	    parser.value = symbol->value;
    }
    expression_prim();
}

static void
expression_unary(void)
{
    symbol_t	*symbol;
    char	 buffer[256];

    expression_prim();
    switch (parser.expr) {
	case expr_add:
	    expression_unary();
	    switch (parser.type) {
		case type_l:
		case type_d:
		    break;
		default:
		    error("syntax error");
	    }
	    break;
	case expr_sub:
	    expression_unary();
	    switch (parser.type) {
		case type_l:
		    parser.value.i = -parser.value.i;
		    break;
		case type_d:
		    parser.value.d = -parser.value.d;
		    break;
		default:
		    error("syntax error");
	    }
	    break;
	case expr_inc:
	    expression_inc(1);
	    break;
	case expr_dec:
	    expression_dec(1);
	    break;
	case expr_not:
	    expression_unary();
	    switch (parser.type) {
		case type_l:
		    parser.value.i = !parser.value.i;
		    break;
		case type_d:
		    parser.value.i = parser.value.d != 0;
		    break;
		case type_p:
		    parser.value.i = parser.value.p != NULL;
		    break;
		default:
		    error("syntax error");
	    }
	    parser.type = type_l;
	    break;
	case expr_com:
	    expression_unary();
	    if (parser.type != type_l)
		error("syntax error");
	    parser.value.i = ~parser.value.i;
	    break;
	case expr_lparen:
	    expression_cond();
	    if (parser.expr != expr_rparen)
		error("syntax error");
	    expression_prim();
	    break;
	case expr_symbol:
	    strcpy(buffer, parser.string);
	    expression_prim();
	    switch (parser.expr) {
		case expr_set:
		    if ((symbol = get_symbol_by_name(buffer)) == NULL) {
			if (!parser.short_circuit)
			    symbol = new_symbol(buffer);
		    }
		    expression_cond();
		set:
		    if (!parser.short_circuit) {
			if (symbol == NULL)
			    error("syntax error");
			symbol->type = parser.type;
			symbol->value = parser.value;
		    }
		    break;
		case expr_mulset:		parser.putback = expr_mul;
		    goto check;
		case expr_divset:		parser.putback = expr_div;
		    goto check;
		case expr_remset:		parser.putback = expr_rem;
		    goto check;
		case expr_addset:		parser.putback = expr_add;
		    goto check;
		case expr_subset:		parser.putback = expr_sub;
		    goto check;
		case expr_lshset:		parser.putback = expr_lsh;
		    goto check;
		case expr_rshset:		parser.putback = expr_rsh;
		    goto check;
		case expr_andset:		parser.putback = expr_and;
		    goto check;
		case expr_orset:		parser.putback = expr_or;
		    goto check;
		case expr_xorset:		parser.putback = expr_xor;
		check:
		    if ((symbol = get_symbol_by_name(buffer)) == NULL) {
			if (!parser.short_circuit)
			    error("undefined symbol %s", buffer);
			parser.type = type_l;
			parser.value.i = 1;
		    }
		    switch (parser.putback) {
			case expr_mul:	case expr_div:	case expr_rem:
			    expression_mul();
			    break;
			case expr_add:	case expr_sub:
			    expression_add();
			    break;
			case expr_lsh:	case expr_rsh:
			    expression_shift();
			    break;
			case expr_and:	case expr_or:  case expr_xor:
			    expression_bit();
			    break;
			default:
			    abort();
		    }
		    goto set;
		case expr_inc:
		    expression_inc(0);
		    break;
		case expr_dec:
		    expression_dec(0);
		    break;
		default:
		    break;
	    }
	    break;
	case expr_int:
	case expr_float:
	case expr_pointer:
	    /* make next token available */
	    expression_prim();
	default:
	    break;
    }
}

static void
expression_mul(void)
{
    type_t	type;
    value_t	value;

    expression_unary();
    switch (parser.type) {
	case type_l:	case type_d:	case type_p:	break;
	default:					return;
    }
    for (;;) {
	switch (parser.expr) {
	    case expr_mul:
		type = parser.type, value = parser.value;
		expression_unary();
		switch (parser.type) {
		    case type_l:
			if (type == type_l)
			    value.i *= parser.value.i;
			else
			    value.d *= parser.value.i;
			break;
		    case type_d:
			if (type == type_l) {
			    type = type_d;
			    value.d = value.i;
			}
			value.d *= parser.value.d;
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type, parser.value = value;
		break;
	    case expr_div:
		type = parser.type, value = parser.value;
		expression_unary();
		switch (parser.type) {
		    case type_l:
			if (type == type_l) {
			    if (parser.value.i == 0)
				error("divide by zero");
			    value.i /= parser.value.i;
			}
			else
			    value.d /= parser.value.i;
			break;
		    case type_d:
			if (type == type_l) {
			    type = type_d;
			    value.d = value.i;
			}
			value.d /= parser.value.d;
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type, parser.value = value;
		break;
	    case expr_rem:
		type = parser.type, value = parser.value;
		expression_unary();
		switch (parser.type) {
		    case type_l:
			if (type == type_l) {
			    if (parser.value.i == 0)
				error("divide by zero");
			    value.i %= parser.value.i;
			}
			else
			    error("invalid operand");
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type, parser.value = value;
		break;
	    default:
		return;
	}
    }
}

static void
expression_add(void)
{
    type_t	type;
    value_t	value;

    expression_mul();
    switch (parser.type) {
	case type_l:	case type_d:	case type_p:	break;
	default:					return;
    }
    for (;;) {
	switch (parser.expr) {
	    case expr_add:
		type = parser.type, value = parser.value;
		expression_mul();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i += parser.value.i;
				break;
			    case type_d:
				value.d += parser.value.i;
				break;
			    default:
				value.cp += parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				type = type_d;
				value.d = value.i;
				break;
			    case type_d:
				break;
			    default:
				error("invalid operand");
			}
			value.d += parser.value.d;
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				type = type_p;
				value.cp = value.i + parser.value.cp;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type, parser.value = value;
		break;
	    case expr_sub:
		type = parser.type, value = parser.value;
		expression_mul();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i -= parser.value.i;
				break;
			    case type_d:
				value.d -= parser.value.i;
				break;
			    default:
				value.cp -= parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				type = type_d;
				value.d = value.i;
				break;
			    case type_d:
				break;
			    default:
				error("invalid operand");
			}
			value.d -= parser.value.d;
			break;
		    case type_p:
			switch (type) {
			    case type_p:
				type = type_l;
				value.i = value.cp - parser.value.cp;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type, parser.value = value;
		break;
	    default:
		return;
	}
    }
}

static void
expression_shift(void)
{
    long	value;
    expression_add();

    switch (parser.type) {
	case type_l:	case type_d:	case type_p:	break;
	default:					return;
    }
    for (;;) {
	switch (parser.expr) {
	    case expr_lsh:
		value = parser.value.i;
		if (parser.type != type_l)
		    error("invalid operand");
		expression_add();
		if (parser.type != type_l)
		    error("invalid operand");
		value <<= parser.value.i;
		parser.value.i = value;
		break;
	    case expr_rsh:
		value = parser.value.i;
		if (parser.type != type_l)
		    error("invalid operand");
		expression_add();
		if (parser.type != type_l)
		    error("invalid operand");
		value >>= parser.value.i;
		parser.value.i = value;
		break;
	    default:
		return;
	}
    }
}

static void
expression_bit(void)
{
    long	i;

    expression_shift();
    switch (parser.type) {
	case type_l:	case type_d:	case type_p:	break;
	default:					return;
    }
    for (;;) {
	switch (parser.expr) {
	    case expr_and:
		if (parser.type != type_l)
		    error("invalid operand");
		i = parser.value.i;
		expression_shift();
		if (parser.type != type_l)
		    error("invalid operand");
		i &= parser.value.i;
		parser.value.i = i;
		break;
	    case expr_or:
		if (parser.type != type_l)
		    error("invalid operand");
		i = parser.value.i;
		expression_shift();
		if (parser.type != type_l)
		    error("invalid operand");
		i |= parser.value.i;
		parser.value.i = i;
		break;
	    case expr_xor:
		if (parser.type != type_l)
		    error("invalid operand");
		i = parser.value.i;
		expression_shift();
		if (parser.type != type_l)
		    error("invalid operand");
		i ^= parser.value.i;
		parser.value.i = i;
		break;
	    default:
		return;
	}
    }
}

static void
expression_rel(void)
{
    type_t	type;
    value_t	value;

    expression_bit();
    switch (parser.type) {
	case type_l:	case type_d:	case type_p:	break;
	default:					return;
    }
    for (;;) {
	switch (parser.expr) {
	    case expr_lt:
		type = parser.type, value = parser.value;
		expression_bit();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i < parser.value.i;
				break;
			    case type_d:
				value.i = value.d < parser.value.i;
				break;
			    default:
				value.i = (long)value.p < parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i < parser.value.d;
				break;
			    case type_d:
				value.i = value.d < parser.value.d;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i < (long)parser.value.p;
				break;
			    case type_d:
				error("invalid operand");
			    default:
				value.i = (long)value.p < (long)parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value = value;
		break;
	    case expr_le:
		type = parser.type, value = parser.value;
		expression_bit();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i <= parser.value.i;
				break;
			    case type_d:
				value.i = value.d <= parser.value.i;
				break;
			    default:
				value.i = (long)value.p <= parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i <= parser.value.d;
				break;
			    case type_d:
				value.i = value.d <= parser.value.d;
				break;
			    default:
				value.i = (long)value.p <= parser.value.d;
				break;
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i <= (long)parser.value.p;
				break;
			    case type_d:
				error("invalid operand");
			    default:
				value.i = (long)value.p <= (long)parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value = value;
		break;
	    case expr_eq:
		type = parser.type, value = parser.value;
		expression_bit();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i == parser.value.i;
				break;
			    case type_d:
				value.i = value.d == parser.value.i;
				break;
			    default:
				value.i = (long)value.p == parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i == parser.value.d;
				break;
			    case type_d:
				value.i = value.d == parser.value.d;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i == (long)parser.value.p;
				break;
			    case type_d:
				error("invalid operand");
			    default:
				value.i = value.p == parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value = value;
		break;
	    case expr_ge:
		type = parser.type, value = parser.value;
		expression_bit();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i >= parser.value.i;
				break;
			    case type_d:
				value.i = value.d >= parser.value.i;
				break;
			    default:
				value.i = (long)value.p >= parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i >= parser.value.d;
				break;
			    case type_d:
				value.i = value.d >= parser.value.d;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i >= (long)parser.value.p;
				break;
			    case type_d:
				error("invalid operand");
			    default:
				value.i = (long)value.p >= (long)parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value = value;
		break;
	    case expr_gt:
		type = parser.type, value = parser.value;
		expression_bit();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i > parser.value.i;
				break;
			    case type_d:
				value.i = value.d > parser.value.i;
				break;
			    default:
				value.i = (long)value.p > parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i > parser.value.d;
				break;
			    case type_d:
				value.i = value.d > parser.value.d;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i > (long)parser.value.p;
				break;
			    case type_d:
				error("invalid operand");
			    default:
				value.i = (long)value.p > (long)parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value = value;
		break;
	    case expr_ne:
		type = parser.type, value = parser.value;
		expression_bit();
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i != parser.value.i;
				break;
			    case type_d:
				value.i = value.d != parser.value.i;
				break;
			    default:
				value.i = (long)value.p != parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i != parser.value.d;
				break;
			    case type_d:
				value.i = value.d != parser.value.d;
				break;
			    default:
				error("invalid operand");
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i != (long)parser.value.p;
				break;
			    case type_d:
				error("invalid operand");
			    default:
				value.i = value.p != parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value = value;
		break;
	    default:
		return;
	}
    }
}

static void
expression_cond(void)
{
    type_t	type;
    value_t	value;
    int		short_circuit;

    expression_rel();
    switch (parser.type) {
	case type_l:	case type_d:	case type_p:	break;
	default:					return;
    }
    for (;;) {
	switch (parser.expr) {
	    case expr_andand:
		type = parser.type, value = parser.value;
		switch (type) {
		    case type_l:
			short_circuit = value.i == 0;
			break;
		    case type_d:
			short_circuit = value.d == 0.0;
			break;
		    default:
			short_circuit = value.p == NULL;
			break;
		}
		parser.short_circuit += short_circuit;
		expression_rel();
		parser.short_circuit -= short_circuit;
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i && parser.value.i;
				break;
			    case type_d:
				value.i = value.d && parser.value.i;
				break;
			    default:
				value.i = value.p && parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i && parser.value.d;
				break;
			    case type_d:
				value.i = value.d && parser.value.d;
				break;
			    default:
				value.i = value.p && parser.value.d;
				break;
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i && parser.value.p;
				break;
			    case type_d:
				value.i = value.d && parser.value.p;
				break;
			    default:
				value.i = value.p && parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value.i = value.i;
		break;
	    case expr_oror:
		type = parser.type, value = parser.value;
		switch (type) {
		    case type_l:
			short_circuit = value.i != 0;
			break;
		    case type_d:
			short_circuit = value.d != 0.0;
			break;
		    default:
			short_circuit = value.p != NULL;
			break;
		}
		parser.short_circuit += short_circuit;
		expression_rel();
		parser.short_circuit -= short_circuit;
		switch (parser.type) {
		    case type_l:
			switch (type) {
			    case type_l:
				value.i = value.i || parser.value.i;
				break;
			    case type_d:
				value.i = value.d || parser.value.i;
				break;
			    default:
				value.i = value.p || parser.value.i;
				break;
			}
			break;
		    case type_d:
			switch (type) {
			    case type_l:
				value.i = value.i || parser.value.d;
				break;
			    case type_d:
				value.i = value.d || parser.value.d;
				break;
			    default:
				value.i = value.p || parser.value.d;
				break;
			}
			break;
		    case type_p:
			switch (type) {
			    case type_l:
				value.i = value.i || parser.value.p;
				break;
			    case type_d:
				value.i = value.d || parser.value.p;
				break;
			    default:
				value.i = value.p || parser.value.p;
				break;
			}
			break;
		    default:
			error("invalid operand");
		}
		parser.type = type_l, parser.value.i = value.i;
		break;
	    default:
		return;
	}
    }
}

static token_t
expression(void)
{
    symbol_t	*symbol;

    (void)identifier('$');
    if (parser.string[1] == '\0') {
	if (getch_noeof() != '(')
	    error("bad symbol or expression");
	parser.type = type_none;
	expression_cond();
	if (parser.expr != expr_rparen)
	    error("bad expression");
	switch (parser.type) {
	    case type_l:
		return (tok_int);
	    case type_d:
		return (tok_float);
	    case type_p:
		return (tok_pointer);
	    default:
		error("bad expression");
	}
    }
    else if ((symbol = get_symbol_by_name(parser.string))) {
	switch (parser.type = symbol->type) {
	    case type_l:
		parser.value.i = symbol->value.i;
		return (tok_int);
	    case type_d:
		parser.value.d = symbol->value.d;
		return (tok_float);
	    default:
		parser.value.p = symbol->value.p;
		return (tok_pointer);
	}
    }
    else
	error("undefined symbol %s", parser.string);
}

static token_t
primary(skip_t skip)
{
    int		ch;

    switch (skip) {
	case skip_none:	ch = getch();	break;
	case skip_ws:	ch = skipws();	break;
	case skip_nl:	ch = skipnl();	break;
	default:			abort();
    }
    switch (ch) {
	case '%':
	    return (regname());
	case 'a' ... 'z': case 'A' ... 'Z': case '_':
	    return (identifier(ch));
	case '0' ... '9': case '+': case '-':
	    return (number(ch));
	case '.':
	    return (tok_dot);
	case '"':
	    return (string());
	case '\'':
	    return (character());
#if DYNAMIC
	case '@':
	    return (dynamic());
#endif
	case '$':
	    return (expression());
	case EOF:
	    return (tok_eof);
	case '\n':
	    return (tok_newline);
	case ';':
	    return (tok_semicollon);
	default:
	    error("syntax error");
    }
}

static void
parse(void)
{
    int		 ch;
    label_kind_t kind;
    token_t	 token;
    instr_t	*instr;
    label_t	*label;
    void	*value;

    for (;;) {
	switch (token = primary(skip_nl)) {
	    case tok_symbol:
		ch = getch_noeof();
		if (ch == ':') {
		    if ((label = get_label_by_name(parser.string))) {
			if (label->kind == label_kind_code_forward) {
			    label->kind = label_kind_code;
			    label->value = jit_get_label();
			}
			else
			    error("label %s: redefined", parser.string);
		    }
		    else {
			if (code_start == NULL) {
			    kind = label_kind_data;
			    value = data;
			}
			else {
			    kind = label_kind_code;
			    value = jit_get_label();
			}
			label = new_label(kind, parser.string, value);
		    }
		    break;
		}
		ungetch(ch);
		if ((instr =
		     (instr_t *)get_hash(instrs, parser.string)) == NULL)
		    error("unhandled symbol %s", parser.string);
		if (code_start == NULL)
		    error(".code must be specified before instructions");
		check_code();
		(*instr->function)();
		break;
	    case tok_dot:
		dot();
		break;
	    case tok_eof:
		return;
	    default:
		error("syntax error");
	}
    }
}

static int
execute(int argc, char *argv[])
{
    label_t	*label;
    function_t	 function;
    patch_t	*patch, *next;

    if (code_start == NULL)
	error(".code must be specified");
    for (patch = patches; patch; patch = next) {
	next = patch->next;
	label = patch->label;
	if (label->kind == label_kind_code_forward)
	    error("undefined label %s", label->name);
	switch (patch->kind) {
	    case patch_kind_jmp:
		jit_patch_at((jit_insn *)patch->value,
			     (jit_insn *)label->value);
		break;
	    case patch_kind_mov:
		jit_patch_movi((jit_insn *)patch->value,
			       (jit_insn *)label->value);
		break;
#if CALLI_PATCH
	    case patch_kind_call:
		jit_patch_calli((jit_insn *)patch->value,
				(jit_insn *)label->value);
#endif
	}
	free(patch);
	patch = next;
    }
    check_code();
    code_end = jit_get_ip().ptr;
    jit_flush_code(code_start, code_end);
    function = (function_t)code_start;
    data_end = data;
#if DISASSEMBLER
    if (flag_verbose) {
	if (flag_verbose > 2 && symbol_offset) {
	    show_symbols();
	    fprintf(stream, "  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	}
	/* only required for disassembly */
	qsort(labels_by_value, label_offset,
	      sizeof(label_t *), qcmp_labels_by_value);
	dump(data_start, data_end - data_start);
	if (data_end - data_start)
	    fprintf(stream, "  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
	disassemble(code_start, code_end - code_start);
	fprintf(stream, "  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
    }
#endif
    return ((*function)(argc, argv));
}

static label_t *
new_label(label_kind_t kind, char *name, void *value)
{
    label_t	*label;

#if DISASSEMBLER
    if (flag_verbose && (label_offset & 15) == 0) {
	if ((label_length += 16) == 16)
	    labels_by_value = (label_t **)xmalloc(sizeof(label_t *) *
						  label_length);
	else
	    labels_by_value = (label_t **)xrealloc(labels_by_value,
						   sizeof(label_t *) *
						   label_length);
    }
#endif

    label = (label_t *)xmalloc(sizeof(label_t));
    label->kind = kind;
    label->name = strdup(name);
    label->value = value;
    put_hash(labels, (entry_t *)label);
#if DISASSEMBLER
    if (flag_verbose)
	labels_by_value[label_offset] = label;
#endif
    label_offset++;
    return (label);
}

static patch_t *
new_patch(patch_kind_t kind, label_t *label, void *value)
{
    patch_t	*patch = (patch_t *)xmalloc(sizeof(patch_t));
    patch->kind = kind;
    patch->label = label;
    if (value < (void *)code_start)
	error("bad patch (address too low)");
    if (value > (void *)jit_get_label())
	error("bad patch (address too high)");
    patch->value = value;
    patch->next = patches;
    patches = patch;

    return (patch);
}

static int
bcmp_symbols(const void *left, const void *right)
{
    return (strcmp((char *)left, (*(symbol_t **)right)->name));
}

static int
qcmp_symbols(const void *left, const void *right)
{
    return (strcmp((*(symbol_t **)left)->name, (*(symbol_t **)right)->name));
}

static symbol_t *
new_symbol(char *name)
{
    symbol_t	*symbol;

    if ((symbol_offset & 15) == 0) {
	if ((symbol_length += 16) == 16)
	    symbols = (symbol_t **)xmalloc(sizeof(symbol_t *) *
					   symbol_length);
	else
	    symbols = (symbol_t **)xrealloc(symbols, sizeof(symbol_t *) *
					    symbol_length);
    }
    symbol = (symbol_t *)xmalloc(sizeof(symbol_t));
    symbol->name = strdup(name);
    symbols[symbol_offset++] = symbol;
    qsort(symbols, symbol_offset, sizeof(symbol_t *), qcmp_symbols);

    return (symbol);
}

static symbol_t *
get_symbol_by_name(char *name)
{
    symbol_t	**symbol_pointer;

    symbol_pointer = (symbol_t **)bsearch(name, symbols, symbol_offset,
					  sizeof(symbol_t *), bcmp_symbols);

    return (symbol_pointer ? *symbol_pointer : NULL);
}

static void *
xmalloc(size_t size)
{
    void	*pointer = malloc(size);

    if (pointer == NULL)
	error("out of memory");

    return (pointer);
}

static void *
xrealloc(void *pointer, size_t size)
{
    pointer = realloc(pointer, size);

    if (pointer == NULL)
	error("out of memory");

    return (pointer);
}

static void *
xcalloc(size_t nmemb, size_t size)
{
    void	*pointer = calloc(nmemb, size);

    if (pointer == NULL)
	error("out of memory");

    return (pointer);
}

static hash_t *
new_hash(void)
{
    hash_t	*hash;

    hash = (hash_t *)xmalloc(sizeof(hash_t));
    hash->count = 0;
    hash->entries = (entry_t **)xcalloc(hash->size = 32, sizeof(void *));

    return (hash);
}

static int
hash_string(char *name)
{
    char	*ptr;
    int		 key;

    for (key = 0, ptr = name; *ptr; ptr++)
	key = (key << (key & 1)) ^ *ptr;

    return (key);
}

static void
put_hash(hash_t *hash, entry_t *entry)
{
    entry_t	*prev, *ptr;
    int		 key = hash_string(entry->name) & (hash->size - 1);

    for (prev = ptr = hash->entries[key]; ptr; prev = ptr, ptr = ptr->next) {
	if (strcmp(entry->name, ptr->name) == 0)
	    error("duplicated entry %s", entry->name);
    }
    if (prev == NULL)
	hash->entries[key] = entry;
    else
	prev->next = entry;
    entry->next = NULL;
    ++hash->count;
    if (hash->count > hash->size * 0.75)
	rehash(hash);
}

static entry_t *
get_hash(hash_t *hash, char *name)
{
    entry_t	*entry;
    int		 key = hash_string(name) & (hash->size - 1);

    for (entry = hash->entries[key]; entry; entry = entry->next) {
	if (strcmp(entry->name, name) == 0)
	    return (entry);
    }
    return (NULL);
}

static void
rehash(hash_t *hash)
{
    int		 i, size, key;
    entry_t	*entry, *next, **entries;

    entries = (entry_t **)xcalloc(size = hash->size * 2, sizeof(void *));
    for (i = 0; i < hash->size; i++) {
	for (entry = hash->entries[i]; entry; entry = next) {
	    next = entry->next;
	    key = hash_string(entry->name) & (size - 1);
	    entry->next = entries[key];
	    entries[key] = entry;
	}
    }
    free(hash->entries);
    hash->entries = entries;
    hash->size = size;
}

static void
usage(void)
{
    fprintf(stderr, "\
Usage: %s [assembler-options] file [jit-program-options]\n\
Options:\n\
  -help                    Display this information\n"
#if DISASSEMBLER
	    "\
  -v[0-3]                  Verbose output level (needs disassembler)\n\
                           0:  Silent\n\
                           1:  Dump data and code mnemonics\n\
                           2:  Dump code in hexadecimal format\n\
                           3:  Print symbols and some extra information\n"
#endif
#if PREPROCESSOR
	    "\
  -D<macro>[=<val>]        C preprocessor options\n"
#endif
	    , progname);
    exit(1);
}

int
main(int argc, char *argv[])
{
    static const char	*short_options = "v::";
    static struct option long_options[] = {
	{ "help",		0, 0, 'h' },
	{ 0,			0, 0, 0   }
    };
    char		*endptr;
    int			 opt_index;
    int			 opt_short;
#if PREPROCESSOR
    char		 cmdline[8192];
#endif

    progname = argv[0];
    for (;;) {
	if ((opt_short = getopt_long_only(argc, argv, short_options,
					  long_options, &opt_index)) < 0)
	    break;
	switch (opt_short) {
	    case 'h':
	    default:
		usage();
		break;
	    case 'v':
		if (optarg) {
		    flag_verbose = strtol(optarg, &endptr, 10);
		    if (*endptr || flag_verbose < 0)
			usage();
		}
		else
		    flag_verbose = 1;
		break;
	}
    }
    if (flag_verbose > 2) {
	strncpy(parser.name, argv[0], sizeof(parser.name));
	parser.name[sizeof(parser.name) - 1] = '\0';
#if !DISASSEMBLER
	warn("disassembler not enabled");
#endif
#if !DYNAMIC
	warn("dynamic symbols not enabled");
#endif
#if !PREPROCESSOR
	warn("preprocessor not enabled");
#endif
#if !CALLI_PATCH
	warn("feature request: patch finish/calli");
#endif
    }

    qsort(instr_vector, sizeof(instr_vector) / sizeof(instr_vector[0]),
	  sizeof(instr_vector[0]), qcmp_instrs);
    instrs = new_hash();
    for (opt_index = 0;
	 opt_index < (int)(sizeof(instr_vector) / sizeof(instr_vector[0]));
	 opt_index++)
	put_hash(instrs, (entry_t *)(instr_vector + opt_index));

    opt_index = optind;
    if (opt_index < 0 || opt_index >= argc)
	usage();
    if (strcmp(argv[opt_index], "-") == 0) {
#if !PREPROCESSOR
	parser.fp = stdin;
#endif
	strcpy(parser.name, "<stdin>");
    }
    else {
#if !PREPROCESSOR
	if ((parser.fp = fopen(argv[opt_index], "r")) == NULL)
	    error("cannot open %s", argv[opt_index]);
	else
#endif
	{
	    if ((endptr = strrchr(argv[opt_index], '/')) == NULL)
		endptr = argv[opt_index];
	    else
		++endptr;
	    strncpy(parser.name, endptr, sizeof(parser.name));
	    parser.name[sizeof(parser.name) - 1] = '\0';
	}
    }
#if PREPROCESSOR
    opt_short = snprintf(cmdline, sizeof(cmdline), "cpp %s", argv[opt_index]);
    for (++opt_index; opt_index < argc; opt_index++) {
	if (argv[opt_index][0] == '-')
	    opt_short += snprintf(cmdline + opt_short,
				  sizeof(cmdline) - opt_short,
				  " %s", argv[opt_index]);
	else {
	    --opt_index;
	    break;
	}
    }
    opt_short += snprintf(cmdline + opt_short,
			  sizeof(cmdline) - opt_short,
			  " -D__WORDSIZE=%d", __WORDSIZE);
    opt_short += snprintf(cmdline + opt_short,
			  sizeof(cmdline) - opt_short,
			  " -D__LITTLE_ENDIAN=%d", __LITTLE_ENDIAN);
    opt_short += snprintf(cmdline + opt_short,
			  sizeof(cmdline) - opt_short,
			  " -D__BIG_ENDIAN=%d", __BIG_ENDIAN);
    opt_short += snprintf(cmdline + opt_short,
			  sizeof(cmdline) - opt_short,
			  " -D__BYTE_ORDER=%d", __BYTE_ORDER);
#  if defined(__i386__)
    opt_short += snprintf(cmdline + opt_short,
			  sizeof(cmdline) - opt_short,
			  " -D__i386__=1");
#  endif
#  if defined(__x86_64__)
    opt_short += snprintf(cmdline + opt_short,
			  sizeof(cmdline) - opt_short,
			  " -D__x86_64__=1");
#  endif
    if ((parser.fp = popen(cmdline, "r")) == NULL)
	error("cannot execute %s", cmdline);
#endif
    parser.line = 1;

    parser.string = (char *)xmalloc(parser.length = 4096);

    labels = new_hash();

    parse();
#if PREPROCESSOR
    pclose(parser.fp);
#else
    if (parser.fp != stdin)
	fclose(parser.fp);
#endif
    parser.fp = NULL;
    for (opt_short = 0; opt_index < argc; opt_short++, opt_index++)
	argv[opt_short] = argv[opt_index];
    argv[opt_short] = NULL;
    argc = opt_short;
    execute(argc, argv);

    return (0);
}
