.data	256
c:
.c	"c"
uc:
.c	"uc"
s:
.c	"s"
us:
.c	"us"
i:
.c	"i"
ui:
.c	"ui"
l:
.c	"l"
ul:
.c	"ul"
p:
.c	"p"
f:
.c	"f"
d:
.c	"d"
fmtii:
.c	"(0x%lx) %s_fn_%s(0x%lx) = 0x%lx\n"
fmtif:
.c	"(0x%lx) %s_fn_%s(%f) = 0x%lx\n"
fmtfi:
.c	"(%f) %s_fn_%s(0x%lx) = %f\n"
fmtff:
.c	"(%f) %s_fn_%s(%f) = %f\n"
fmtx:
.c	"%ld != %ld\n"

.code	$(1024 * 1024)

	prolog 0
	jmpi main

/* just to make macros expansions easier... */
#define   EXTR_c_i(r)		  extr_c_i %r %r
#define   EXTR_c_l(r)		  extr_c_l %r %r
#define EXTR_uc_ui(r)		extr_uc_ui %r %r
#define EXTR_uc_ul(r)		extr_uc_ul %r %r
#define   EXTR_s_i(r)		  extr_s_i %r %r
#define   EXTR_s_l(r)		  extr_s_l %r %r
#define EXTR_us_ui(r)		extr_us_ui %r %r
#define EXTR_us_ul(r)		extr_us_ul %r %r
#define   EXTR_i_i(r)		/**/
#define  EXTR_i_ui(r)		/**/
#define   EXTR_i_l(r)		  extr_i_l %r %r
#define  EXTR_ui_i(r)		/**/
#define EXTR_ui_ui(r)		/**/
#define EXTR_ui_ul(r)		extr_ui_ul %r %r
#define   EXTR_l_l(r)		/**/
#define  EXTR_l_ul(r)		/**/
#define  EXTR_ul_l(r)		/**/
#define EXTR_ul_ul(r)		/**/
#define  EXTR_p_ul(r)		/**/
#define   EXTR_d_d(r)		/**/
#define   EXTR_f_f(r)		/**/
#define   EXTR_d_f(r)		extr_d_f %r %r
#define   EXTR_f_d(r)		extr_f_d %r %r

#define DEFUNII(R, A, I)	\
R##_fn_##A:			\
	prolog 1		\
	arg_##A $arg##A		\
	getarg_##A %r2 $arg##A	\
	movr_##I %ret %r2	\
	ret

#define DEFUNIF(R, A, I)	\
R##_fn_##A:			\
	prolog 0		\
	prolog_##A 1		\
	arg_##A $arg##A		\
	getarg_##A %f5 $arg##A	\
	truncr_##A##_##I %r2 %f5\
	movr_##I %ret %r2	\
	ret

#define DEFUNFI(R, A, I)	\
R##_fn_##A:			\
	prolog 1		\
	arg_##A $arg##A		\
	getarg_##A %r2 $arg##A	\
	extr_##I##_##R %f5 %r2	\
	movr_##R %fret %f5	\
	ret

#define DEFUNFF(R, A)		\
R##_fn_##A:			\
	prolog 0		\
	prolog_##A 1		\
	arg_##A $arg##A		\
	getarg_##A %f5 $arg##A	\
	extr_##A##_##R %f5 %f5	\
	movr_##R %fret %f5	\
	ret

#define DEFUNF_(F)		\
F##_fn_##F:			\
	prolog 0		\
	prolog_##F 1		\
	arg_##F $arg##F		\
	getarg_##F %f5 $arg##F	\
	movr_##F %fret %f5	\
	ret

#define  DEFUNI(T,  I)		\
	DEFUNII(T,  c, i)	\
	DEFUNII(T, uc, ui)	\
	DEFUNII(T,  s, i)	\
	DEFUNII(T, us, ui)	\
	DEFUNII(T,  i, i)	\
	DEFUNII(T, ui, ui)	\
	DEFUNII(T,  l, l)	\
	DEFUNII(T, ul, ul)	\
	DEFUNII(T,  p, ul)	\
	DEFUNIF(T,  f, I)	\
	DEFUNIF(T,  d, I)

#define  DEFUNF(F, D)		\
	DEFUNFI(F,  c, i)	\
	DEFUNFI(F, uc, i)	\
	DEFUNFI(F,  s, i)	\
	DEFUNFI(F, us, i)	\
	DEFUNFI(F,  i, i)	\
	DEFUNFI(F, ui, i)	\
	DEFUNFI(F,  l, l)	\
	DEFUNFI(F, ul, l)	\
	DEFUNFF(F,  D)		\
	DEFUNF_(F)

	DEFUNI( c, i)
	DEFUNI(uc, i)
	DEFUNI( s, i)
	DEFUNI(us, i)
	DEFUNI( i, i)
	DEFUNI(ui, i)
	DEFUNI( l, l)
	DEFUNI(ul, l)
	DEFUNI( p, l)
	DEFUNF( f, d)
	DEFUNF( d, f)

main:
#define FUNCALLII(N, R, A, ARG, RET, I)		\
	prepare 1				\
		movi_##I %v0 ARG		\
		pusharg_##A %v0			\
	finish R##_fn_##A			\
	retval_##R %r0				\
	EXTR_##R##_##I(r0)			\
	movi_##I %r1 RET			\
	beqr_##I R##N##A %r0 %r1		\
	prepare 6				\
		pusharg_##I %r0			\
		pusharg_##I %v0			\
		movi_p %r0 A			\
		pusharg_p %r0			\
		movi_p %r0 R			\
		pusharg_p %r0			\
		pusharg_##I %r1			\
		movi_p %r0 fmtii		\
		pusharg_p %r0			\
	finish @printf				\
R##N##A:

#define FUNCALLIF(N, R, A, ARG, RET, I)	\
	prepare 0			\
	prepare_##A 1			\
		movi_##A %f0 ARG	\
		pusharg_##A %f0		\
	finish R##_fn_##A		\
	retval_##R %r0			\
	EXTR_##R##_##I(r0)		\
	movi_##I %r1 RET		\
	beqr_##I R##N##A %r0 %r1	\
	prepare 5			\
	prepare_d 1			\
		pusharg_##I %r0		\
		EXTR_##A##_d(f0)	\
		pusharg_d %f0		\
		movi_p %r0 A		\
		pusharg_p %r0		\
		movi_p %r0 R		\
		pusharg_p %r0		\
		pusharg_##I %r1		\
		movi_p %r0 fmtif	\
		pusharg_p %r0		\
	finish @printf			\
R##N##A:

#define FUNCALLFI(N, R, A, ARG, RET, I)	\
	prepare 1			\
		movi_##I %v2 ARG	\
		pusharg_##A %v2		\
	finish R##_fn_##A		\
	retval_##R %f2			\
	movi_##R %f1 RET		\
	beqr_##R R##N##A %f2 %f1	\
	prepare 4			\
	prepare_d 2			\
		EXTR_##R##_d(f2)	\
		pusharg_d %f2		\
		pusharg_##I %v2		\
		movi_p %r0 A		\
		pusharg_p %r0		\
		movi_p %r0 R		\
		pusharg_p %r0		\
		EXTR_##R##_d(f1)	\
		pusharg_d %f1		\
		movi_p %r0 fmtfi	\
		pusharg_p %r0		\
	finish @printf			\
R##N##A:

#define FUNCALLFF(N, R, A, ARG, RET)	\
	prepare 0			\
	prepare_##A 1			\
		movi_##A %f4 ARG	\
		pusharg_##A %f4		\
	finish R##_fn_##A		\
	retval_##R %f0			\
	movi_##R %f1 RET		\
	beqr_##R R##N##A %f0 %f1	\
	movi_##A %f4 ARG		\
	prepare 3			\
	prepare_d 3			\
		EXTR_##R##_d(f0)	\
		pusharg_d %f0		\
		EXTR_##R##_d(f4)	\
		pusharg_d %f4		\
		movi_p %r0 A		\
		pusharg_p %r0		\
		movi_p %r0 R		\
		pusharg_p %r0		\
		EXTR_##R##_d(f1)	\
		pusharg_d %f1		\
		movi_p %r0 fmtff	\
		pusharg_p %r0		\
	finish @printf			\
R##N##A:

#define c7f	0x7f
#define c80	0x80
#define c81	0x81
#define cff	0xff

#define s7f	0x7fff
#define s80	0x8000
#define s81	0x8001

#define i7f	0x7fffffff
#define i80	0x80000000
#define i81	0x80000001
#define iff	0xffffffff

#define f7f	 127.0
#define f80	-128.0
#define f81	-127.0

#if __WORDSIZE == 64
#  define l7f	0x7fffffffffffffff
#  define l80	0x8000000000000000
#  define l81	0x8000000000000001
#  define lff	0xffffffffffffffff
#  define il7f	lff
#  define il80	0
#  define il81	1
#  define Il7f	iff
#  define lc80	0xffffffffffffff80
#  define lc81	0xffffffffffffff81
#  define ls80	0xffffffffffff8000
#  define ls81	0xffffffffffff8001
#else
#  define l7f	i7f
#  define l80	i80
#  define l81	i81
#  define lff	iff
#  define il7f	i7f
#  define il80	i80
#  define il81	i81
#  define Il7f	i7f
#  define lc80	ic80
#  define lc81	ic81
#  define ls80	is80
#  define ls81	is81
#endif
#define p7f	l7f
#define p80	l80
#define p81	l81

#define cc7f	c7f
#define cc80	0xffffff80
#define cc81	0xffffff81
#define cs7f	iff
#define cs80	0
#define cs81	1
#define ci7f	iff
#define ci80	0
#define ci81	1
#define cl7f	lff
#define cl80	0
#define cl81	1

#define sc80	0xffffff80
#define Sc80	0xff80
#define sc81	0xffffff81
#define Sc81	0xff81
#define ss80	0xffff8000
#define ss81	0xffff8001
#define si7f	iff
#define Si7f	0xffff
#define si80	0
#define si81	1
#define sl7f	lff
#define Sl7f	Si7f
#define sl80	0
#define sl81	1

#define ic80	0xffffff80
#define ic81	0xffffff81
#define is80	0xffff8000
#define is81	0xffff8001

	FUNCALLII(0,  c,  c, c7f,  c7f, i)
	FUNCALLII(1,  c,  c, c80, ic80, i)
	FUNCALLII(2,  c,  c, c81, ic81, i)
	FUNCALLII(0,  c, uc, c7f,  c7f, i)
	FUNCALLII(1,  c, uc, c80, ic80, i)
	FUNCALLII(2,  c, uc, c81, ic81, i)
	FUNCALLII(0,  c,  s, s7f, cs7f, i)
	FUNCALLII(1,  c,  s, s80, cs80, i)
	FUNCALLII(2,  c,  s, s81, cs81, i)
	FUNCALLII(0,  c, us, s7f, cs7f, i)
	FUNCALLII(1,  c, us, s80, cs80, i)
	FUNCALLII(2,  c, us, s81, cs81, i)
	FUNCALLII(0,  c,  i, i7f, ci7f, i)
	FUNCALLII(1,  c,  i, i80, ci80, i)
	FUNCALLII(2,  c,  i, i81, ci81, i)
	FUNCALLII(0,  c, ui, i7f, ci7f, i)
	FUNCALLII(1,  c, ui, i80, ci80, i)
	FUNCALLII(2,  c, ui, i81, ci81, i)
	FUNCALLII(0,  c,  l, l7f, cl7f, l)
	FUNCALLII(1,  c,  l, l80, cl80, l)
	FUNCALLII(2,  c,  l, l81, cl81, l)
	FUNCALLII(0,  c, ul, l7f, cl7f, l)
	FUNCALLII(1,  c, ul, l80, cl80, l)
	FUNCALLII(2,  c, ul, l81, cl81, l)
	FUNCALLII(0,  c,  p, l7f, cl7f, l)
	FUNCALLII(1,  c,  p, l80, cl80, l)
	FUNCALLII(2,  c,  p, l81, cl81, l)
	FUNCALLIF(0,  c,  f, f7f,  c7f, i)
	FUNCALLIF(1,  c,  f, f80, ic80, i)
	FUNCALLIF(2,  c,  f, f81, ic81, i)
	FUNCALLIF(0,  c,  d, f7f,  c7f, i)
	FUNCALLIF(1,  c,  d, f80, ic80, i)
	FUNCALLIF(2,  c,  d, f81, ic81, i)
	FUNCALLII(0, uc,  c, c7f,  c7f, ui)
	FUNCALLII(1, uc,  c, c80,  c80, ui)
	FUNCALLII(2, uc,  c, c81,  c81, ui)
	FUNCALLII(0, uc, uc, c7f,  c7f, ui)
	FUNCALLII(1, uc, uc, c80,  c80, ui)
	FUNCALLII(2, uc, uc, c81,  c81, ui)
	FUNCALLII(0, uc,  s, s7f,  cff, ui)
	FUNCALLII(1, uc,  s, s80, cs80, ui)
	FUNCALLII(2, uc,  s, s81, cs81, ui)
	FUNCALLII(0, uc, us, s7f,  cff, ui)
	FUNCALLII(1, uc, us, s80, cs80, ui)
	FUNCALLII(2, uc, us, s81, cs81, ui)
	FUNCALLII(0, uc,  i, i7f,  cff, ui)
	FUNCALLII(1, uc,  i, i80, ci80, ui)
	FUNCALLII(2, uc,  i, i81, ci81, ui)
	FUNCALLII(0, uc, ui, i7f,  cff, ui)
	FUNCALLII(1, uc, ui, i80, ci80, ui)
	FUNCALLII(2, uc, ui, i81, ci81, ui)
	FUNCALLII(0, uc,  l, l7f,  cff, ul)
	FUNCALLII(1, uc,  l, l80, cl80, ul)
	FUNCALLII(2, uc,  l, l81, cl81, ul)
	FUNCALLII(0, uc, ul, l7f,  cff, ul)
	FUNCALLII(1, uc, ul, l80, cl80, ul)
	FUNCALLII(2, uc, ul, l81, cl81, ul)
	FUNCALLII(0, uc,  p, l7f,  cff, ul)
	FUNCALLII(1, uc,  p, l80, cl80, ul)
	FUNCALLII(2, uc,  p, l81, cl81, ul)
	FUNCALLIF(0, uc,  f, f7f,  c7f, ui)
	FUNCALLIF(1, uc,  f, f80,  c80, ui)
	FUNCALLIF(2, uc,  f, f81,  c81, ui)
	FUNCALLIF(0, uc,  d, f7f,  c7f, ui)
	FUNCALLIF(1, uc,  d, f80,  c80, ui)
	FUNCALLIF(2, uc,  d, f81,  c81, ui)
	FUNCALLII(0,  s,  c, c7f,  c7f, i)
	FUNCALLII(1,  s,  c, c80, sc80, i)
	FUNCALLII(2,  s,  c, c81, sc81, i)
	FUNCALLII(0,  s, uc, c7f,  c7f, i)
	FUNCALLII(1,  s, uc, c80,  c80, i)
	FUNCALLII(2,  s, uc, c81,  c81, i)
	FUNCALLII(0,  s,  s, s7f,  s7f, i)
	FUNCALLII(1,  s,  s, s80, ss80, i)
	FUNCALLII(2,  s,  s, s81, ss81, i)
	FUNCALLII(0,  s, us, s7f,  s7f, i)
	FUNCALLII(1,  s, us, s80, ss80, i)
	FUNCALLII(2,  s, us, s81, ss81, i)
	FUNCALLII(0,  s,  i, i7f, si7f, i)
	FUNCALLII(1,  s,  i, i80, si80, i)
	FUNCALLII(2,  s,  i, i81, si81, i)
	FUNCALLII(0,  s, ui, i7f, si7f, i)
	FUNCALLII(1,  s, ui, i80, si80, i)
	FUNCALLII(2,  s, ui, i81, si81, i)
	FUNCALLII(0,  s,  l, l7f, sl7f, l)
	FUNCALLII(1,  s,  l, l80, sl80, l)
	FUNCALLII(2,  s,  l, l81, sl81, l)
	FUNCALLII(0,  s, ul, l7f, sl7f, l)
	FUNCALLII(1,  s, ul, l80, sl80, l)
	FUNCALLII(2,  s, ul, l81, sl81, l)
	FUNCALLII(0,  s,  p, l7f, sl7f, l)
	FUNCALLII(1,  s,  p, l80, sl80, l)
	FUNCALLII(2,  s,  p, l81, sl81, l)
	FUNCALLIF(0,  s,  f, f7f,  c7f, i)
	FUNCALLIF(1,  s,  f, f80, ic80, i)
	FUNCALLIF(2,  s,  f, f81, ic81, i)
	FUNCALLIF(0,  s,  d, f7f,  c7f, i)
	FUNCALLIF(1,  s,  d, f80, ic80, i)
	FUNCALLIF(2,  s,  d, f81, ic81, i)
	FUNCALLII(0, us,  c, c7f,  c7f, ui)
	FUNCALLII(1, us,  c, c80, Sc80, ui)
	FUNCALLII(2, us,  c, c81, Sc81, ui)
	FUNCALLII(0, us, uc, c7f,  c7f, ui)
	FUNCALLII(1, us, uc, c80,  c80, ui)
	FUNCALLII(2, us, uc, c81,  c81, ui)
	FUNCALLII(0, us,  s, s7f,  s7f, ui)
	FUNCALLII(1, us,  s, s80,  s80, ui)
	FUNCALLII(2, us,  s, s81,  s81, ui)
	FUNCALLII(0, us, us, s7f,  s7f, ui)
	FUNCALLII(1, us, us, s80,  s80, ui)
	FUNCALLII(2, us, us, s81,  s81, ui)
	FUNCALLII(0, us,  i, i7f, Si7f, ui)
	FUNCALLII(1, us,  i, i80, si80, ui)
	FUNCALLII(2, us,  i, i81, si81, ui)
	FUNCALLII(0, us, ui, i7f, Si7f, ui)
	FUNCALLII(1, us, ui, i80, si80, ui)
	FUNCALLII(2, us, ui, i81, si81, ui)
	FUNCALLII(0, us,  l, l7f, Sl7f, ul)
	FUNCALLII(1, us,  l, l80, sl80, ul)
	FUNCALLII(2, us,  l, l81, sl81, ul)
	FUNCALLII(0, us, ul, l7f, Sl7f, ul)
	FUNCALLII(1, us, ul, l80, sl80, ul)
	FUNCALLII(2, us, ul, l81, sl81, ul)
	FUNCALLII(0, us,  p, l7f, Sl7f, ul)
	FUNCALLII(1, us,  p, l80, sl80, ul)
	FUNCALLII(2, us,  p, l81, sl81, ul)
	FUNCALLIF(0, us,  f, f7f,  c7f, ui)
	FUNCALLIF(1, us,  f, f80, Sc80, ui)
	FUNCALLIF(2, us,  f, f81, Sc81, ui)
	FUNCALLIF(0, us,  d, f7f,  c7f, ui)
	FUNCALLIF(1, us,  d, f80, Sc80, ui)
	FUNCALLIF(2, us,  d, f81, Sc81, ui)
	FUNCALLII(0,  i,  c, c7f,  c7f, i)
	FUNCALLII(1,  i,  c, c80, ic80, i)
	FUNCALLII(2,  i,  c, c81, ic81, i)
	FUNCALLII(0,  i, uc, c7f,  c7f, i)
	FUNCALLII(1,  i, uc, c80,  c80, i)
	FUNCALLII(2,  i, uc, c81,  c81, i)
	FUNCALLII(0,  i,  s, s7f,  s7f, i)
	FUNCALLII(1,  i,  s, s80, is80, i)
	FUNCALLII(2,  i,  s, s81, is81, i)
	FUNCALLII(0,  i, us, s7f,  s7f, i)
	FUNCALLII(1,  i, us, s80,  s80, i)
	FUNCALLII(2,  i, us, s81,  s81, i)
	FUNCALLII(0,  i,  i, i7f,  i7f, i)
	FUNCALLII(1,  i,  i, i80,  i80, i)
	FUNCALLII(2,  i,  i, i81,  i81, i)
	FUNCALLII(0,  i, ui, i7f,  i7f, i)
	FUNCALLII(1,  i, ui, i80,  i80, i)
	FUNCALLII(2,  i, ui, i81,  i81, i)
	FUNCALLII(0,  i,  l, l7f, il7f, l)
	FUNCALLII(1,  i,  l, l80, il80, l)
	FUNCALLII(2,  i,  l, l81, il81, l)
	FUNCALLII(0,  i, ul, l7f, il7f, l)
	FUNCALLII(1,  i, ul, l80, il80, l)
	FUNCALLII(2,  i, ul, l81, il81, l)
	FUNCALLII(0,  i,  p, l7f, il7f, l)
	FUNCALLII(1,  i,  p, l80, il80, l)
	FUNCALLII(2,  i,  p, l81, il81, l)
	FUNCALLIF(0,  i,  f, f7f,  c7f, i)
	FUNCALLIF(1,  i,  f, f80, ic80, i)
	FUNCALLIF(2,  i,  f, f81, ic81, i)
	FUNCALLIF(0,  i,  d, f7f,  c7f, i)
	FUNCALLIF(1,  i,  d, f80, ic80, i)
	FUNCALLIF(2,  i,  d, f81, ic81, i)
	FUNCALLII(0, ui,  c, c7f,  c7f, ui)
	FUNCALLII(1, ui,  c, c80, ic80, ui)
	FUNCALLII(2, ui,  c, c81, ic81, ui)
	FUNCALLII(0, ui, uc, c7f,  c7f, ui)
	FUNCALLII(1, ui, uc, c80,  c80, ui)
	FUNCALLII(2, ui, uc, c81,  c81, ui)
	FUNCALLII(0, ui,  s, s7f,  s7f, ui)
	FUNCALLII(1, ui,  s, s80, is80, ui)
	FUNCALLII(2, ui,  s, s81, is81, ui)
	FUNCALLII(0, ui, us, s7f,  s7f, ui)
	FUNCALLII(1, ui, us, s80,  s80, ui)
	FUNCALLII(2, ui, us, s81,  s81, ui)
	FUNCALLII(0, ui,  i, i7f,  i7f, ui)
	FUNCALLII(1, ui,  i, i80,  i80, ui)
	FUNCALLII(2, ui,  i, i81,  i81, ui)
	FUNCALLII(0, ui, ui, i7f,  i7f, ui)
	FUNCALLII(1, ui, ui, i80,  i80, ui)
	FUNCALLII(2, ui, ui, i81,  i81, ui)
	FUNCALLII(0, ui,  l, l7f, Il7f, ul)
	FUNCALLII(1, ui,  l, l80, il80, ul)
	FUNCALLII(2, ui,  l, l81, il81, ul)
	FUNCALLII(0, ui, ul, l7f, Il7f, ul)
	FUNCALLII(1, ui, ul, l80, il80, ul)
	FUNCALLII(2, ui, ul, l81, il81, ul)
	FUNCALLII(0, ui,  p, l7f, Il7f, ul)
	FUNCALLII(1, ui,  p, l80, il80, ul)
	FUNCALLII(2, ui,  p, l81, il81, ul)
	FUNCALLIF(0, ui,  f, f7f,  c7f, ui)
	FUNCALLIF(1, ui,  f, f80, ic80, ui)
	FUNCALLIF(2, ui,  f, f81, ic81, ui)
	FUNCALLIF(0, ui,  d, f7f,  c7f, ui)
	FUNCALLIF(1, ui,  d, f80, ic80, ui)
	FUNCALLIF(2, ui,  d, f81, ic81, ui)
	FUNCALLII(0,  l,  c, c7f,  c7f, l)
	FUNCALLII(1,  l,  c, c80, lc80, l)
	FUNCALLII(2,  l,  c, c81, lc81, l)
	FUNCALLII(0,  l, uc, c7f,  c7f, l)
	FUNCALLII(1,  l, uc, c80,  c80, l)
	FUNCALLII(2,  l, uc, c81,  c81, l)
	FUNCALLII(0,  l,  s, s7f,  s7f, l)
	FUNCALLII(1,  l,  s, s80, ls80, l)
	FUNCALLII(2,  l,  s, s81, ls81, l)
	FUNCALLII(0,  l, us, s7f,  s7f, l)
	FUNCALLII(1,  l, us, s80,  s80, l)
	FUNCALLII(2,  l, us, s81,  s81, l)
	FUNCALLII(0,  l,  i, i7f,  i7f, l)
	FUNCALLII(1,  l,  i, i80,  i80, l)
	FUNCALLII(2,  l,  i, i81,  i81, l)
	FUNCALLII(0,  l, ui, i7f,  i7f, l)
	FUNCALLII(1,  l, ui, i80,  i80, l)
	FUNCALLII(2,  l, ui, i81,  i81, l)
	FUNCALLII(0,  l,  l, l7f,  l7f, l)
	FUNCALLII(1,  l,  l, l80,  l80, l)
	FUNCALLII(2,  l,  l, l81,  l81, l)
	FUNCALLII(0,  l, ul, l7f,  l7f, l)
	FUNCALLII(1,  l, ul, l80,  l80, l)
	FUNCALLII(2,  l, ul, l81,  l81, l)
	FUNCALLII(0,  l,  p, l7f,  l7f, l)
	FUNCALLII(1,  l,  p, l80,  l80, l)
	FUNCALLII(2,  l,  p, l81,  l81, l)
	FUNCALLIF(0,  l,  f, f7f,  c7f, l)
	FUNCALLIF(1,  l,  f, f80, lc80, l)
	FUNCALLIF(2,  l,  f, f81, lc81, l)
	FUNCALLIF(0,  l,  d, f7f,  c7f, l)
	FUNCALLIF(1,  l,  d, f80, lc80, l)
	FUNCALLIF(2,  l,  d, f81, lc81, l)
	FUNCALLII(0, ul,  c, c7f,  c7f, ul)
	FUNCALLII(1, ul,  c, c80, lc80, ul)
	FUNCALLII(2, ul,  c, c81, lc81, ul)
	FUNCALLII(0, ul, uc, c7f,  c7f, ul)
	FUNCALLII(1, ul, uc, c80,  c80, ul)
	FUNCALLII(2, ul, uc, c81,  c81, ul)
	FUNCALLII(0, ul,  s, s7f,  s7f, ul)
	FUNCALLII(1, ul,  s, s80, ls80, ul)
	FUNCALLII(2, ul,  s, s81, ls81, ul)
	FUNCALLII(0, ul, us, s7f,  s7f, ul)
	FUNCALLII(1, ul, us, s80,  s80, ul)
	FUNCALLII(2, ul, us, s81,  s81, ul)
	FUNCALLII(0, ul,  i, i7f,  i7f, ul)
	FUNCALLII(1, ul,  i, i80,  i80, ul)
	FUNCALLII(2, ul,  i, i81,  i81, ul)
	FUNCALLII(0, ul, ui, i7f,  i7f, ul)
	FUNCALLII(1, ul, ui, i80,  i80, ul)
	FUNCALLII(2, ul, ui, i81,  i81, ul)
	FUNCALLII(0, ul,  l, l7f,  l7f, ul)
	FUNCALLII(1, ul,  l, l80,  l80, ul)
	FUNCALLII(2, ul,  l, l81,  l81, ul)
	FUNCALLII(0, ul, ul, l7f,  l7f, ul)
	FUNCALLII(1, ul, ul, l80,  l80, ul)
	FUNCALLII(2, ul, ul, l81,  l81, ul)
	FUNCALLII(0, ul,  p, l7f,  l7f, ul)
	FUNCALLII(1, ul,  p, l80,  l80, ul)
	FUNCALLII(2, ul,  p, l81,  l81, ul)
	FUNCALLIF(0, ul,  f, f7f,  c7f, ul)
	FUNCALLIF(1, ul,  f, f80, lc80, ul)
	FUNCALLIF(2, ul,  f, f81, lc81, ul)
	FUNCALLIF(0, ul,  d, f7f,  c7f, ul)
	FUNCALLIF(1, ul,  d, f80, lc80, ul)
	FUNCALLIF(2, ul,  d, f81, lc81, ul)
	FUNCALLII(0,  p,  c, c7f,  c7f, ul)
	FUNCALLII(1,  p,  c, c80, lc80, ul)
	FUNCALLII(2,  p,  c, c81, lc81, ul)
	FUNCALLII(0,  p, uc, c7f,  c7f, ul)
	FUNCALLII(1,  p, uc, c80,  c80, ul)
	FUNCALLII(2,  p, uc, c81,  c81, ul)
	FUNCALLII(0,  p,  s, s7f,  s7f, ul)
	FUNCALLII(1,  p,  s, s80, ls80, ul)
	FUNCALLII(2,  p,  s, s81, ls81, ul)
	FUNCALLII(0,  p, us, s7f,  s7f, ul)
	FUNCALLII(1,  p, us, s80,  s80, ul)
	FUNCALLII(2,  p, us, s81,  s81, ul)
	FUNCALLII(0,  p,  i, i7f,  i7f, ul)
	FUNCALLII(1,  p,  i, i80,  i80, ul)
	FUNCALLII(2,  p,  i, i81,  i81, ul)
	FUNCALLII(0,  p, ui, i7f,  i7f, ul)
	FUNCALLII(1,  p, ui, i80,  i80, ul)
	FUNCALLII(2,  p, ui, i81,  i81, ul)
	FUNCALLII(0,  p,  l, l7f,  l7f, ul)
	FUNCALLII(1,  p,  l, l80,  l80, ul)
	FUNCALLII(2,  p,  l, l81,  l81, ul)
	FUNCALLII(0,  p, ul, l7f,  l7f, ul)
	FUNCALLII(1,  p, ul, l80,  l80, ul)
	FUNCALLII(2,  p, ul, l81,  l81, ul)
	FUNCALLII(0,  p,  p, l7f,  l7f, ul)
	FUNCALLII(1,  p,  p, l80,  l80, ul)
	FUNCALLII(2,  p,  p, l81,  l81, ul)
	FUNCALLFI(0,  f,  c, c7f,  c7f, i)
	FUNCALLFI(0,  f, uc, c7f,  c7f, ui)
	FUNCALLFI(0,  f,  s, c7f,  c7f, i)
	FUNCALLFI(0,  f, us, c7f,  c7f, ui)
	FUNCALLFI(0,  f,  i, c7f,  c7f, i)
	FUNCALLFI(0,  f, ui, c7f,  c7f, ui)
	FUNCALLFI(0,  f,  l, c7f,  c7f, i)
	FUNCALLFI(0,  f, ul, c7f,  c7f, ui)
	FUNCALLFF(0,  f,  f, f7f,  f7f)
	FUNCALLFF(1,  f,  f, f80,  f80)
	FUNCALLFF(2,  f,  f, f81,  f81)
	FUNCALLFF(0,  f,  d, f7f,  f7f)
	FUNCALLFF(1,  f,  d, f80,  f80)
	FUNCALLFF(2,  f,  d, f81,  f81)
	FUNCALLFI(0,  d,  c, c7f,  c7f, i)
	FUNCALLFI(0,  d, uc, c7f,  c7f, ui)
	FUNCALLFI(0,  d,  s, c7f,  c7f, i)
	FUNCALLFI(0,  d, us, c7f,  c7f, ui)
	FUNCALLFI(0,  d,  i, c7f,  c7f, i)
	FUNCALLFI(0,  d, ui, c7f,  c7f, ui)
	FUNCALLFI(0,  d,  l, c7f,  c7f, i)
	FUNCALLFI(0,  d, ul, c7f,  c7f, ui)
	FUNCALLFF(0,  d,  f, f7f,  f7f)
	FUNCALLFF(1,  d,  f, f80,  f80)
	FUNCALLFF(2,  d,  f, f81,  f81)
	FUNCALLFF(0,  d,  d, f7f,  f7f)
	FUNCALLFF(1,  d,  d, f80,  f80)
	FUNCALLFF(2,  d,  d, f81,  f81)

	movi_p %v1 backward	/* not really */
	movi_i %v0 3
backward:
	movi_p %r0 forward
	movi_i %r1 2
	prepare 3
		pusharg_i %r1
		subi_i %r1 %r1 1
		pusharg_i %r1
		subi_i %r1 %r1 1
		pusharg_i %r1
	finishr %r0
	blei_i done %v0 0
	subi_i %v0 %v0 1
	jmpr %v1
done:

	/* check patch for forward finish/calli */
	movi_i %r0 2
	prepare 3
		pusharg_i %r0
		subi_i %r0 %r0 1
		pusharg_i %r0
		subi_i %r0 %r0 1
		pusharg_i %r0
	finish forward

#define CHECK(R)		\
	movi_p %R nothing	\
	callr %R		\
	movi_p %R nothing	\
	prepare 0		\
	finishr %R
	CHECK(v0)
	CHECK(v1)
	CHECK(v2)
	CHECK(r0)
	CHECK(r1)
	CHECK(r2)

#undef CHECK
#define CHECK(R)		\
	movi_p %R R##forward	\
	jmpr %R			\
	movi_p %R 0		\
	R##forward:
	CHECK(v0)
	CHECK(v1)
	CHECK(v2)
	CHECK(r0)
	CHECK(r1)
	CHECK(r2)

	ret

forward:
	prolog 3

	arg_i $arg0
	arg_i $arg1
	arg_i $arg2
	getarg_i %v0 $arg0
	getarg_i %v1 $arg1
	getarg_i %v2 $arg2
	movi_i %r0 0
	beqr_i forward_0 %v0 %r0
	prepare 3
		pusharg_i %v0
		pusharg_i %r0
		movi_p %r0 fmtx
		pusharg_p %r0
	finish @printf
forward_0:
	movi_i %r1 1
	beqr_i forward_1 %v1 %r1
	prepare 3
		pusharg_i %v1
		pusharg_i %r1
		movi_p %r1 fmtx
		pusharg_p %r1
	finish @printf
forward_1:
	movi_i %r2 2
	beqr_i forward_2 %v2 %r2
	prepare 3
		pusharg_i %v2
		pusharg_i %r2
		movi_p %r2 fmtx
		pusharg_p %r2
	finish @printf
forward_2:

	ret

nothing:
	prolog 0
	ret
