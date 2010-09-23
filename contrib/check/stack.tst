/*
 * Test logic:
 *	o Implement functions with 1 to 16 arguments of a given type
 *	o 16 arguments function calls 15 arguments function,
 *	  15 arguments function calls 14 arguments function, and so on
 *	o Every function calls jit_allocai to create a packed vector
 *	  on the stack, that is an extra argument, and is used to
 *	  validate arguments
 *	o Validation should happen as late as possible, to ensure contents
 *	  were not overrun
 */

#include "float_branch.inc"

/*====================================================================*/
.data	1024

.	$($szofc = 1)
#define $szofuc		$szofc
.	$($szofs = 2)
#define $szofus		$szofs
.	$($szofi = 4)
#define $szofui		$szofi
.	$($szofl = __WORDSIZE >> 3)
#define $szoful		$szofl
#define $szofp		$szofl
.	$($szoff = 4)
.	$($szofd = 8)

fmtc:
.c	" c: %lx %lx\n"
fmt_c:
.c	"c"
fmtuc:
.c	"uc: %lx %lx\n"
fmt_uc:
.c	"uc"
fmts:
.c	" s: %lx %lx\n"
fmt_s:
.c	"s"
fmtus:
.c	"us: %lx %lx\n"
fmt_us:
.c	"us"
fmti:
.c	" i: %lx %lx\n"
fmt_i:
.c	"i"
fmtui:
.c	"ui: %lx %lx\n"
fmt_ui:
.c	"ui"
fmtl:
.c	" l: %lx %lx\n"
fmt_l:
.c	"l"
fmtul:
.c	"ul: %lx %lx\n"
fmt_ul:
.c	"ul"
fmtp:
.c	" p: %p %p\n"
fmt_p:
.c	"p"
fmtf:
.c	" f: %f %f\n"
fmt_f:
.c	"f"
fmtd:
.c	" d: %f %f\n"
fmt_d:
.c	"d"
fmt_t_n:
.c	"test_%s_%d error\n"

#define ARG1(T)						arg_##T $arg##T##0
#define ARG2(T)			ARG1(T)			arg_##T $arg##T##1
#define ARG3(T)			ARG2(T)			arg_##T $arg##T##2
#define ARG4(T)			ARG3(T)			arg_##T $arg##T##3
#define ARG5(T)			ARG4(T)			arg_##T $arg##T##4
#define ARG6(T)			ARG5(T)			arg_##T $arg##T##5
#define ARG7(T)			ARG6(T)			arg_##T $arg##T##6
#define ARG8(T)			ARG7(T)			arg_##T $arg##T##7
#define ARG9(T)			ARG8(T)			arg_##T $arg##T##8
#define ARG10(T)		ARG9(T)			arg_##T $arg##T##9
#define ARG11(T)		ARG10(T)		arg_##T $arg##T##10
#define ARG12(T)		ARG11(T)		arg_##T $arg##T##11
#define ARG13(T)		ARG12(T)		arg_##T $arg##T##12
#define ARG14(T)		ARG13(T)		arg_##T $arg##T##13
#define ARG15(T)		ARG14(T)		arg_##T $arg##T##14
#define ARG16(T)		ARG15(T)		arg_##T $arg##T##15

#define CHECKI(N, T, I, V)				\
	getarg_##T	%r0 $arg##T##V			\
	ldxi_##T	%r1 %v0 $(V * $szof##T)		\
	beqr_##I	N##T##I##V %r0 %r1		\
	prepare 3					\
		pusharg_##T %r1				\
		pusharg_##T %r0				\
		movi_p %r0 fmt##T			\
		pusharg_p %r0				\
	finish @printf					\
N##T##I##V:

#define CHECKF(N, T, I, V)				\
	getarg_f	%f0 $argf##V			\
	ldxi_f		%f1 %v0 $(V * 4)		\
	float_cmp_branch_true(N##f##V, r0, f0, f1)	\
	prepare 1					\
	prepare_d 2					\
		extr_f_d %f1 %f1			\
		pusharg_d %f1				\
		extr_f_d %f0 %f0			\
		pusharg_d %f0				\
		movi_p %r0 fmtf				\
		pusharg_p %r0				\
	finish @printf					\
N##f##V:

#define CHECKD(N, T, I, V)				\
	getarg_d	%f0 $argd##V			\
	ldxi_d		%f1 %v0 $(V * 8)		\
	double_cmp_branch_true(N##d##V, r0, f0, f1)	\
	prepare 1					\
	prepare_d 2					\
		pusharg_d %f1				\
		pusharg_d %f0				\
		movi_p %r0 fmtd				\
		pusharg_p %r0				\
	finish @printf					\
N##d##V:

#define  GETARG1(K, N, T, I, V)				CHECK##K(N, T, I, 0)
#define  GETARG2(K, N, T, I, V)	 GETARG1(K, N, T, I, V)	CHECK##K(N, T, I, 1)
#define  GETARG3(K, N, T, I, V)	 GETARG2(K, N, T, I, V)	CHECK##K(N, T, I, 2)
#define  GETARG4(K, N, T, I, V)	 GETARG3(K, N, T, I, V)	CHECK##K(N, T, I, 3)
#define  GETARG5(K, N, T, I, V)	 GETARG4(K, N, T, I, V)	CHECK##K(N, T, I, 4)
#define  GETARG6(K, N, T, I, V)	 GETARG5(K, N, T, I, V)	CHECK##K(N, T, I, 5)
#define  GETARG7(K, N, T, I, V)	 GETARG6(K, N, T, I, V)	CHECK##K(N, T, I, 6)
#define  GETARG8(K, N, T, I, V)	 GETARG7(K, N, T, I, V)	CHECK##K(N, T, I, 7)
#define  GETARG9(K, N, T, I, V)	 GETARG8(K, N, T, I, V)	CHECK##K(N, T, I, 8)
#define GETARG10(K, N, T, I, V)	 GETARG9(K, N, T, I, V)	CHECK##K(N, T, I, 9)
#define GETARG11(K, N, T, I, V)	GETARG10(K, N, T, I, V)	CHECK##K(N, T, I, 10)
#define GETARG12(K, N, T, I, V)	GETARG11(K, N, T, I, V)	CHECK##K(N, T, I, 11)
#define GETARG13(K, N, T, I, V)	GETARG12(K, N, T, I, V)	CHECK##K(N, T, I, 12)
#define GETARG14(K, N, T, I, V)	GETARG13(K, N, T, I, V)	CHECK##K(N, T, I, 13)
#define GETARG15(K, N, T, I, V)	GETARG14(K, N, T, I, V)	CHECK##K(N, T, I, 14)
#define GETARG16(K, N, T, I, V)	GETARG15(K, N, T, I, V)	CHECK##K(N, T, I, 15)

#define  PUSH0(T, R)
#define  PUSH1(T, R)			movi_##T %R 1  /**/ pusharg_##T %R
#define  PUSH2(T, R)	 PUSH1(T, R)	movi_##T %R 2  /**/ pusharg_##T %R
#define  PUSH3(T, R)	 PUSH2(T, R)	movi_##T %R 3  /**/ pusharg_##T %R
#define  PUSH4(T, R)	 PUSH3(T, R)	movi_##T %R 4  /**/ pusharg_##T %R
#define  PUSH5(T, R)	 PUSH4(T, R)	movi_##T %R 5  /**/ pusharg_##T %R
#define  PUSH6(T, R)	 PUSH5(T, R)	movi_##T %R 6  /**/ pusharg_##T %R
#define  PUSH7(T, R)	 PUSH6(T, R)	movi_##T %R 7  /**/ pusharg_##T %R
#define  PUSH8(T, R)	 PUSH7(T, R)	movi_##T %R 8  /**/ pusharg_##T %R
#define  PUSH9(T, R)	 PUSH8(T, R)	movi_##T %R 9  /**/ pusharg_##T %R
#define PUSH10(T, R)	 PUSH9(T, R)	movi_##T %R 10 /**/ pusharg_##T %R
#define PUSH11(T, R)	PUSH10(T, R)	movi_##T %R 11 /**/ pusharg_##T %R
#define PUSH12(T, R)	PUSH11(T, R)	movi_##T %R 12 /**/ pusharg_##T %R
#define PUSH13(T, R)	PUSH12(T, R)	movi_##T %R 13 /**/ pusharg_##T %R
#define PUSH14(T, R)	PUSH13(T, R)	movi_##T %R 14 /**/ pusharg_##T %R
#define PUSH15(T, R)	PUSH14(T, R)	movi_##T %R 15 /**/ pusharg_##T %R
#define PUSH16(T, R)	PUSH15(T, R)	movi_##T %R 16 /**/ pusharg_##T %R

/* nowhere to go */
#define DEFUN0(T)						\
test_##T##_0:							\
	prolog 0						\
	/* add some possible check here */			\
	ret

#define EXPAND_GETARGc(N, M)					\
	GETARG##N(I, c##N,  c,  i,  M)				\
	GETARG##N(I, c##N,  c,  l,  M)
#define EXPAND_GETARGuc(N, M)					\
	GETARG##N(I, uc##N, uc, ui, M)				\
	GETARG##N(I, uc##N, uc, ul, M)
#define EXPAND_GETARGs(N, M)					\
	GETARG##N(I, s##N,  s,  i,  M)				\
	GETARG##N(I, s##N,  s,  l,  M)
#define EXPAND_GETARGus(N, M)					\
	GETARG##N(I, us##N, us, ui, M)				\
	GETARG##N(I, us##N, us, ul, M)
#define EXPAND_GETARGi(N, M)					\
	GETARG##N(I, i##N,  i,  i,  M)				\
	GETARG##N(I, i##N,  i,  l,  M)
#define EXPAND_GETARGui(N, M)					\
	GETARG##N(I, ui##N, ui, ui, M)				\
	GETARG##N(I, ui##N, ui, ul, M)
#define EXPAND_GETARGl(N, M)					\
	GETARG##N(I, l##N,  l,  l,  M)
#define EXPAND_GETARGul(N, M)					\
	GETARG##N(I, ul##N, ul, ul, M)
#define EXPAND_GETARGp(N, M)					\
	GETARG##N(I, p##N,  p,  p,  M)
#define EXPAND_GETARGf(N, M)					\
	GETARG##N(F, f##N,  f,  ,   M)
#define EXPAND_GETARGd(N, M)					\
	GETARG##N(D, d##N,  d,  ,   M)

#define EXPAND_PUSHc(N)		PUSH##N(i, r0)
#define EXPAND_PUSHuc(N)	PUSH##N(i, r0)
#define EXPAND_PUSHs(N)		PUSH##N(i, r0)
#define EXPAND_PUSHus(N)	PUSH##N(i, r0)
#define EXPAND_PUSHi(N)		PUSH##N(i, r0)
#define EXPAND_PUSHui(N)	PUSH##N(i, r0)
#define EXPAND_PUSHl(N)		PUSH##N(l, r0)
#define EXPAND_PUSHul(N)	PUSH##N(l, r0)
#define EXPAND_PUSHp(N)		PUSH##N(p, r0)
#define EXPAND_PUSHf(N)		PUSH##N(f, f0)
#define EXPAND_PUSHd(N)		PUSH##N(d, f0)

#define EXPAND_PROLOGc(N)	prolog $(N + 1)
#define EXPAND_PROLOGuc(N)	prolog $(N + 1)
#define EXPAND_PROLOGs(N)	prolog $(N + 1)
#define EXPAND_PROLOGus(N)	prolog $(N + 1)
#define EXPAND_PROLOGi(N)	prolog $(N + 1)
#define EXPAND_PROLOGui(N)	prolog $(N + 1)
#define EXPAND_PROLOGl(N)	prolog $(N + 1)
#define EXPAND_PROLOGul(N)	prolog $(N + 1)
#define EXPAND_PROLOGp(N)	prolog $(N + 1)
#define EXPAND_PROLOGf(N)	prolog 1 /**/ prolog_f N
#define EXPAND_PROLOGd(N)	prolog 1 /**/ prolog_d N

#define EXPAND_PREPAREc(N, M)	prepare N
#define EXPAND_PREPAREuc(N, M)	prepare N
#define EXPAND_PREPAREs(N, M)	prepare N
#define EXPAND_PREPAREus(N, M)	prepare N
#define EXPAND_PREPAREi(N, M)	prepare N
#define EXPAND_PREPAREui(N, M)	prepare N
#define EXPAND_PREPAREl(N, M)	prepare N
#define EXPAND_PREPAREul(N, M)	prepare N
#define EXPAND_PREPAREp(N, M)	prepare N
#define EXPAND_PREPAREf(N, M)	prepare 1 /**/ prepare_f M
#define EXPAND_PREPAREd(N, M)	prepare 1 /**/ prepare_d M

#define DEFUNN(N, M, T)						\
test_##T##_##N:							\
	EXPAND_PROLOG##T(N)					\
	/* keep stack pointer in %v0 */				\
	arg_p $argp						\
	getarg_p %v0 $argp					\
	ARG##N(T)						\
	/* validate arguments once */				\
	EXPAND_GETARG##T(N, M)					\
	/* allocate heap buffer to save stack contents */	\
	prepare 1						\
		movi_l %r0 $(N * $szof##T)			\
		pusharg_l %r0					\
	finish @malloc						\
	/* keep heap pointer in %v1 */				\
	movr_p %v1 %ret						\
	/* copy stack buffer to heap buffer */			\
	prepare 3						\
		movi_l %r0 $(N * $szof##T)			\
		pusharg_l %r0					\
		pusharg_p %v0					\
		pusharg_p %v1					\
	finish @memcpy						\
	/* allocate stack buffer for next routine */		\
	allocai $(M * $szof##T) $index				\
	/* put stack pointer for next routine in %v2 */		\
	addi_p %v2 %fp $index					\
	/* fill stack buffer for next routine */		\
	prepare 2						\
		movi_i %r0 M					\
		pusharg_i %r0					\
		pusharg_p %v2					\
	finish fill_##T						\
	/* call next routine */					\
	EXPAND_PREPARE##T(N, M)					\
		EXPAND_PUSH##T(M)				\
		pusharg_p %v2					\
	finish test_##T##_##M					\
	/* fast validate stack buffer again  */			\
	prepare 3						\
		movi_l %r0 $(N * $szof##T)			\
		pusharg_l %r0					\
		pusharg_p %v1					\
		pusharg_p %v0					\
	finish @memcmp						\
	movr_i %r0 %ret						\
	beqi_i test_##T##_##N##_done %r0 0			\
	prepare 3						\
		movi_i %r0 N					\
		pusharg_i %r0					\
		movi_p %r0 fmt_##T				\
		pusharg_p %r0					\
		movi_p %r0 fmt_t_n				\
		pusharg_p %r0					\
	finish @printf						\
test_##T##_##N##_done:						\
	/* release heap bufer */				\
	prepare 1						\
		pusharg_p %v1					\
	finish @free						\
	ret

#define DEFUNX(T)						\
test_##T##_17:							\
	prolog 0						\
	/* allocate heap buffer to save stack contents */	\
	prepare 1						\
		movi_l %r0 $(16 * $szof##T)			\
		pusharg_l %r0					\
	finish @malloc						\
	/* keep heap pointer in %v1 */				\
	movr_p %v1 %ret						\
	/* fill heap buffer */					\
	prepare 2						\
		movi_i %r0 16					\
		pusharg_i %r0					\
		pusharg_p %v1					\
	finish fill_##T						\
	/* allocate stack buffer before calling top routine */	\
	allocai $(16 * $szof##T) $index				\
	/* put stack pointer for top routine in %v2 */		\
	addi_p %v2 %fp $index					\
	/* copy heap buffer to stack buffer */			\
	prepare 3						\
		movi_l %r0 $(16 * $szof##T)			\
		pusharg_l %r0					\
		pusharg_p %v1					\
		pusharg_p %v2					\
	finish @memcpy						\
	/* call top routine */					\
	EXPAND_PREPARE##T(17, 16)				\
		EXPAND_PUSH##T(16)				\
		pusharg_p %v2					\
	finish test_##T##_16					\
	/* fast validate stack buffer again  */			\
	prepare 3						\
		movi_l %r0 $(16 * $szof##T)			\
		pusharg_l %r0					\
		pusharg_p %v1					\
		pusharg_p %v2					\
	finish @memcmp						\
	movr_i %r0 %ret						\
	beqi_i test_##T##_17_done %r0 0				\
	prepare 3						\
		movi_i %r0 17					\
		pusharg_i %r0					\
		movi_p %r0 fmt_##T				\
		pusharg_p %r0					\
		movi_p %r0 fmt_t_n				\
		pusharg_p %r0					\
	finish @printf						\
test_##T##_17_done:						\
	/* release heap bufer */				\
	prepare 1						\
		pusharg_p %v1					\
	finish @free						\
	ret

#define DEFUN(T)		\
	DEFUN0(T)		\
	DEFUNN(1,  0,  T)	\
	DEFUNN(2,  1,  T)	\
	DEFUNN(3,  2,  T)	\
	DEFUNN(4,  3,  T)	\
	DEFUNN(5,  4,  T)	\
	DEFUNN(6,  5,  T)	\
	DEFUNN(7,  6,  T)	\
	DEFUNN(8,  7,  T)	\
	DEFUNN(9,  8,  T)	\
	DEFUNN(10, 9,  T)	\
	DEFUNN(11, 10, T)	\
	DEFUNN(12, 11, T)	\
	DEFUNN(13, 12, T)	\
	DEFUNN(14, 13, T)	\
	DEFUNN(15, 14, T)	\
	DEFUNN(16, 15, T)	\
	DEFUNX(T)

/*====================================================================*/
.code	$(1024 * 1024)
	jmpi main

#define fill_uc		fill_c
#define fill_us		fill_s
#define fill_ui		fill_i
#define fill_ul		fill_l
#define fill_p		fill_l

#define DEFFILLI(T)			\
fill_##T:				\
	prolog 2			\
	arg_p $argp			\
	getarg_p %v0 $argp		\
	arg_i $argi			\
	getarg_i %r1 $argi		\
	blei_i fill_##T##_done %r1 0	\
	movi_i %r0 0			\
fill_##T##_loop:			\
	stxr_##T %r0 %v0 %r1		\
	subi_i %r1 %r1 1		\
	beqi_i fill_##T##_done %r1 0	\
	addi_i %r0 %r0 $szof##T		\
	jmpi fill_##T##_loop		\
fill_##T##_done:			\
	ret

#define DEFFILLF(T)			\
fill_##T:				\
	prolog 2			\
	arg_p $argp			\
	getarg_p %v0 $argp		\
	arg_i $argi			\
	getarg_i %r1 $argi		\
	blei_i fill_##T##_done %r1 0	\
	extr_i_##T %f0 %r1		\
	movi_##T %f1 1.0		\
	movi_i %r0 0			\
fill_##T##_loop:			\
	stxr_##T %r0 %v0 %f0		\
	subi_i %r1 %r1 1		\
	beqi_i fill_##T##_done %r1 0	\
	subr_##T %f0 %f0 %f1		\
	addi_i %r0 %r0 $szof##T		\
	jmpi fill_##T##_loop		\
fill_##T##_done:			\
	ret

	DEFFILLI(c)
	DEFFILLI(s)
	DEFFILLI(i)
	DEFFILLI(l)
	DEFFILLF(f)
	DEFFILLF(d)
	DEFUN(c)
	DEFUN(uc)
	DEFUN(s)
	DEFUN(us)
	DEFUN(i)
	DEFUN(ui)
	DEFUN(l)
	DEFUN(ul)
	DEFUN(p)
	DEFUN(f)
	DEFUN(d)

#define FUNCALL(T)		finish test_##T##_17
/*--------------------------------------------------------------------*/
main:
	prolog 0

	FUNCALL(c)
	FUNCALL(uc)
	FUNCALL(s)
	FUNCALL(us)
	FUNCALL(i)
	FUNCALL(ui)
	FUNCALL(l)
	FUNCALL(ul)
	FUNCALL(p)
	FUNCALL(f)
	FUNCALL(d)

	ret
