#include "consts.inc"
#include "float_branch.inc"

/*====================================================================*/
.data	1024
glbc:
.c	0
glbuc:
.c	0
.align	2
glbs:
.s	0
glbus:
.s	0
.align	4
glbi:
.i	0
glbui:
.i	0
.align	8
glbl:
.l	0
glbul:
.l	0
glbp:
.p	0
glbf:
.f	0
.align	8
glbd:
.d	0
vl:
.size	$((__WORDSIZE / 8) * 16)
vf:
.size	$(4 * 16)
vd:
.size	$(8 * 16)

fmtc:
.c	" c: %lx %lx\n"
fmtuc:
.c	"uc: %lx %lx\n"
fmts:
.c	" s: %lx %lx\n"
fmtus:
.c	"us: %lx %lx\n"
fmti:
.c	" i: %lx %lx\n"
fmtui:
.c	"ui: %lx %lx\n"
fmtl:
.c	" l: %lx %lx\n"
fmtul:
.c	"ul: %lx %lx\n"
fmtp:
.c	" p: %p %p\n"
fmtf:
.c	" f: %f %f\n"
fmtd:
.c	" d: %f %f\n"

/*====================================================================*/
.code	$(16 * 1024 * 1024)
	jmpi main

/*--------------------------------------------------------------------*/
#define ARG_I0()
#define ARG_I1()			arg_l $argl0
#define ARG_I2()	ARG_I1()	arg_l $argl1
#define ARG_I3()	ARG_I2()	arg_l $argl2
#define ARG_I4()	ARG_I3()	arg_l $argl3
#define ARG_I5()	ARG_I4()	arg_l $argl4
#define ARG_I6()	ARG_I5()	arg_l $argl5
#define ARG_I7()	ARG_I6()	arg_l $argl6
#define ARG_I8()	ARG_I7()	arg_l $argl7
#define ARG_I9()	ARG_I8()	arg_l $argl8
#define ARG_I10()	ARG_I9()	arg_l $argl9
#define ARG_I11()	ARG_I10()	arg_l $argl10
#define ARG_I12()	ARG_I11()	arg_l $argl11
#define ARG_I13()	ARG_I12()	arg_l $argl12
#define ARG_I14()	ARG_I13()	arg_l $argl13
#define ARG_I15()	ARG_I14()	arg_l $argl14

#define ARG_F0()
#define ARG_F1()			arg_f $argf0
#define ARG_F2()	ARG_F1()	arg_f $argf1
#define ARG_F3()	ARG_F2()	arg_f $argf2
#define ARG_F4()	ARG_F3()	arg_f $argf3
#define ARG_F5()	ARG_F4()	arg_f $argf4
#define ARG_F6()	ARG_F5()	arg_f $argf5
#define ARG_F7()	ARG_F6()	arg_f $argf6
#define ARG_F8()	ARG_F7()	arg_f $argf7
#define ARG_F9()	ARG_F8()	arg_f $argf8
#define ARG_F10()	ARG_F9()	arg_f $argf9
#define ARG_F11()	ARG_F10()	arg_f $argf10
#define ARG_F12()	ARG_F11()	arg_f $argf11
#define ARG_F13()	ARG_F12()	arg_f $argf12
#define ARG_F14()	ARG_F13()	arg_f $argf13
#define ARG_F15()	ARG_F14()	arg_f $argf14

#define ARG_D0()
#define ARG_D1()			arg_d $argd0
#define ARG_D2()	ARG_D1()	arg_d $argd1
#define ARG_D3()	ARG_D2()	arg_d $argd2
#define ARG_D4()	ARG_D3()	arg_d $argd3
#define ARG_D5()	ARG_D4()	arg_d $argd4
#define ARG_D6()	ARG_D5()	arg_d $argd5
#define ARG_D7()	ARG_D6()	arg_d $argd6
#define ARG_D8()	ARG_D7()	arg_d $argd7
#define ARG_D9()	ARG_D8()	arg_d $argd8
#define ARG_D10()	ARG_D9()	arg_d $argd9
#define ARG_D11()	ARG_D10()	arg_d $argd10
#define ARG_D12()	ARG_D11()	arg_d $argd11
#define ARG_D13()	ARG_D12()	arg_d $argd12
#define ARG_D14()	ARG_D13()	arg_d $argd13
#define ARG_D15()	ARG_D14()	arg_d $argd14

#define CHECK_I(N, V)					\
	getarg_l	%r0 $argl##V			\
	ldxi_l		%r1 %v0 $(V * (__WORDSIZE / 8))	\
	beqr_l		N##l##V %r0 %r1			\
	prepare 3					\
		pusharg_l %r1				\
		pusharg_l %r0				\
		movi_p %r0 fmtl				\
		pusharg_p %r0				\
	finish @printf					\
N##l##V:

#define CHECK_F(N, V)					\
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

#define CHECK_D(N, V)					\
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

#define GETARG_I0(N)
#define GETARG_I1(N)			CHECK_I(N, 0)
#define GETARG_I2(N)	GETARG_I1(N)	CHECK_I(N, 1)
#define GETARG_I3(N)	GETARG_I2(N)	CHECK_I(N, 2)
#define GETARG_I4(N)	GETARG_I3(N)	CHECK_I(N, 3)
#define GETARG_I5(N)	GETARG_I4(N)	CHECK_I(N, 4)
#define GETARG_I6(N)	GETARG_I5(N)	CHECK_I(N, 5)
#define GETARG_I7(N)	GETARG_I6(N)	CHECK_I(N, 6)
#define GETARG_I8(N)	GETARG_I7(N)	CHECK_I(N, 7)
#define GETARG_I9(N)	GETARG_I8(N)	CHECK_I(N, 8)
#define GETARG_I10(N)	GETARG_I9(N)	CHECK_I(N, 9)
#define GETARG_I11(N)	GETARG_I10(N)	CHECK_I(N, 10)
#define GETARG_I12(N)	GETARG_I11(N)	CHECK_I(N, 11)
#define GETARG_I13(N)	GETARG_I12(N)	CHECK_I(N, 12)
#define GETARG_I14(N)	GETARG_I13(N)	CHECK_I(N, 13)
#define GETARG_I15(N)	GETARG_I14(N)	CHECK_I(N, 14)

#define GETARG_F0(N)
#define GETARG_F1(N)			CHECK_F(N, 0)
#define GETARG_F2(N)	GETARG_F1(N)	CHECK_F(N, 1)
#define GETARG_F3(N)	GETARG_F2(N)	CHECK_F(N, 2)
#define GETARG_F4(N)	GETARG_F3(N)	CHECK_F(N, 3)
#define GETARG_F5(N)	GETARG_F4(N)	CHECK_F(N, 4)
#define GETARG_F6(N)	GETARG_F5(N)	CHECK_F(N, 5)
#define GETARG_F7(N)	GETARG_F6(N)	CHECK_F(N, 6)
#define GETARG_F8(N)	GETARG_F7(N)	CHECK_F(N, 7)
#define GETARG_F9(N)	GETARG_F8(N)	CHECK_F(N, 8)
#define GETARG_F10(N)	GETARG_F9(N)	CHECK_F(N, 9)
#define GETARG_F11(N)	GETARG_F10(N)	CHECK_F(N, 10)
#define GETARG_F12(N)	GETARG_F11(N)	CHECK_F(N, 11)
#define GETARG_F13(N)	GETARG_F12(N)	CHECK_F(N, 12)
#define GETARG_F14(N)	GETARG_F13(N)	CHECK_F(N, 13)
#define GETARG_F15(N)	GETARG_F14(N)	CHECK_F(N, 14)

#define GETARG_D0(N)
#define GETARG_D1(N)			CHECK_D(N, 0)
#define GETARG_D2(N)	GETARG_D1(N)	CHECK_D(N, 1)
#define GETARG_D3(N)	GETARG_D2(N)	CHECK_D(N, 2)
#define GETARG_D4(N)	GETARG_D3(N)	CHECK_D(N, 3)
#define GETARG_D5(N)	GETARG_D4(N)	CHECK_D(N, 4)
#define GETARG_D6(N)	GETARG_D5(N)	CHECK_D(N, 5)
#define GETARG_D7(N)	GETARG_D6(N)	CHECK_D(N, 6)
#define GETARG_D8(N)	GETARG_D7(N)	CHECK_D(N, 7)
#define GETARG_D9(N)	GETARG_D8(N)	CHECK_D(N, 8)
#define GETARG_D10(N)	GETARG_D9(N)	CHECK_D(N, 9)
#define GETARG_D11(N)	GETARG_D10(N)	CHECK_D(N, 10)
#define GETARG_D12(N)	GETARG_D11(N)	CHECK_D(N, 11)
#define GETARG_D13(N)	GETARG_D12(N)	CHECK_D(N, 12)
#define GETARG_D14(N)	GETARG_D13(N)	CHECK_D(N, 13)
#define GETARG_D15(N)	GETARG_D14(N)	CHECK_D(N, 14)

#define DEFUN(I, F, D)			\
test_##I##_##F##_##D:			\
	prolog I			\
	prolog_f F			\
	prolog_d D			\
	ARG_I##I()			\
	ARG_F##F()			\
	ARG_D##D()			\
	movi_p %v0 vl			\
	GETARG_I##I(L##I##_##F##_##D)	\
	movi_p %v0 vf			\
	GETARG_F##F(L##I##_##F##_##D)	\
	movi_p %v0 vd			\
	GETARG_D##D(L##I##_##F##_##D)	\
	ret

#define DEFUN_I(F, D)		\
	DEFUN(0, F, D)		\
	DEFUN(1, F, D)		\
	DEFUN(2, F, D)		\
	DEFUN(3, F, D)		\
	DEFUN(4, F, D)		\
	DEFUN(5, F, D)		\
	DEFUN(6, F, D)		\
	DEFUN(7, F, D)		\
	DEFUN(8, F, D)		\
	DEFUN(9, F, D)		\
	DEFUN(10, F, D)		\
	DEFUN(11, F, D)		\
	DEFUN(12, F, D)		\
	DEFUN(13, F, D)		\
	DEFUN(14, F, D)		\
	DEFUN(15, F, D)

#define DEFUN_F(D)		\
	DEFUN_I(0, D)		\
	DEFUN_I(1, D)		\
	DEFUN_I(2, D)		\
	DEFUN_I(3, D)		\
	DEFUN_I(4, D)		\
	DEFUN_I(5, D)		\
	DEFUN_I(6, D)		\
	DEFUN_I(7, D)		\
	DEFUN_I(8, D)		\
	DEFUN_I(9, D)		\
	DEFUN_I(10, D)		\
	DEFUN_I(11, D)		\
	DEFUN_I(12, D)		\
	DEFUN_I(13, D)		\
	DEFUN_I(14, D)		\
	DEFUN_I(15, D)

	DEFUN_F(0)
	DEFUN_F(1)
	DEFUN_F(2)
	DEFUN_F(3)
	DEFUN_F(4)
	DEFUN_F(5)
	DEFUN_F(6)
	DEFUN_F(7)
	DEFUN_F(8)
	DEFUN_F(9)
	DEFUN_F(10)
	DEFUN_F(11)
	DEFUN_F(12)
	DEFUN_F(13)
	DEFUN_F(14)
	DEFUN_F(15)

/*--------------------------------------------------------------------*/
test:
	prolog 9
	prolog_f 1
	prolog_d 1
	arg_c	$argc
	arg_uc	$arguc
	arg_s	$args
	arg_us	$argus
	arg_i	$argi
	arg_ui	$argui
	arg_l	$argl
	arg_ul	$argul
	arg_p	$argp
	arg_f	$argf
	arg_d	$argd

#undef CHECK_I
#define CHECK_I(T, I)				\
	getarg_##T	%r0 $arg##T		\
	movi_p		%v0 glb##T		\
	ldr_##T		%r1 %v0			\
	beqr_##I	tested_##T##I %r0 %r1	\
	prepare 3				\
		pusharg_##T %r1			\
		pusharg_##T %r0			\
		movi_p %r0 fmt##T		\
		pusharg_p %r0			\
	finish @printf				\
tested_##T##I:

	CHECK_I(c, i)
	CHECK_I(c, l)
	CHECK_I(uc, ui)
	CHECK_I(uc, ul)
	CHECK_I(s, i)
	CHECK_I(s, l)
	CHECK_I(us, ui)
	CHECK_I(us, ul)
	CHECK_I(i, i)
	CHECK_I(i, l)
	CHECK_I(ui, ui)
	CHECK_I(ui, ul)
	CHECK_I(l, l)
	CHECK_I(ul, ul)
	CHECK_I(p, l)
	CHECK_I(p, ul)

	getarg_f	%f0 $argf
	movi_p		%v0 glbf
	ldr_f		%f1 %v0
	float_cmp_branch_true(tested_f, r0, f0, f1)
	prepare 1
	prepare_d 2
		extr_f_d %f1 %f1
		pusharg_d %f1
		extr_f_d %f0 %f0
		pusharg_d %f0
		movi_p %r0 fmtf
		pusharg_p %r0
	finish @printf
tested_f:

	getarg_d	%f0 $argd
	movi_p		%v0 glbd
	ldr_d		%f1 %v0
	double_cmp_branch_true(tested_d, r0, f0, f1)
	prepare 1
	prepare_d 2
		pusharg_d %f1
		pusharg_d %f0
		movi_p %r0 fmtd
		pusharg_p %r0
	finish @printf
tested_d:

	ret

/*--------------------------------------------------------------------*/
/* fill vl vector with reversed numbers, e.g. if called as:
 *	fill_l(3)
 * vl will become:
 *	{ 3, 2, 1, ... }
 */
fill_l:
	prolog 1
	arg_l $argl
	getarg_l %r1 $argl
	blei_l fill_l_done %r1 0
	movi_l %r0 0
	movi_p %v0 vl
fill_l_loop:
	stxr_l %r0 %v0 %r1
	subi_l %r1 %r1 1
	beqi_l fill_l_done %r1 0
	addi_l %r0 %r0 $(__WORDSIZE / 8)
	jmpi fill_l_loop
fill_l_done:
	ret

/*--------------------------------------------------------------------*/
fill_f:
	prolog 1
	arg_l $argl
	getarg_l %r1 $argl
	blei_l fill_f_done %r1 0
	extr_i_f %f0 %r1
	movi_f %f1 1.0
	movi_l %r0 0
	movi_p %v0 vf
fill_f_loop:
	stxr_f %r0 %v0 %f0
	subi_l %r1 %r1 1
	beqi_l fill_f_done %r1 0
	subr_f %f0 %f0 %f1
	addi_l %r0 %r0 4
	jmpi fill_f_loop
fill_f_done:
	ret

/*--------------------------------------------------------------------*/
fill_d:
	prolog 1
	arg_l $argl
	getarg_l %r1 $argl
	blei_l fill_d_done %r1 0
	extr_i_d %f0 %r1
	movi_d %f1 1.0
	movi_l %r0 0
	movi_p %v0 vd
fill_d_loop:
	stxr_d %r0 %v0 %f0
	subi_l %r1 %r1 1
	beqi_l fill_d_done %r1 0
	subr_d %f0 %f0 %f1
	addi_l %r0 %r0 8
	jmpi fill_d_loop
fill_d_done:
	ret

/*--------------------------------------------------------------------*/
main:
	prolog 0

#define setup(M, T, R, V, N)			\
		movi_##M %R V##N		\
		pusharg_##T %R			\
		movi_p %v0 glb##T		\
		str_##T %v0 %R

#define TEST(N)					\
	prepare 9				\
	prepare_f 1				\
	prepare_d 1				\
		setup(d,  d,  f0, dv,  N)	\
		setup(f,  f,  f0, fv,  N)	\
		setup(p,  p,  r0, lv,  N)	\
		setup(ul, ul, r0, lv,  N)	\
		setup(l,  l,  r0, lv,  N)	\
		setup(ui, ui, r0, iv,  N)	\
		setup(i,  i,  r0, lsi, N)	\
		setup(ui, us, r0, sv,  N)	\
		setup(i,  s,  r0, lss, N)	\
		setup(ui, uc, r0, cv,  N)	\
		setup(i,  c,  r0, lsc, N)	\
	finish test

	TEST(0)
	TEST(1)
	TEST(2)
	TEST(3)

#define PUSH_I0()
#define PUSH_I1()			/**/ movi_l %r0 1  /**/ pusharg_l %r0
#define PUSH_I2()	PUSH_I1()	/**/ movi_l %r0 2  /**/ pusharg_l %r0
#define PUSH_I3()	PUSH_I2()	/**/ movi_l %r0 3  /**/ pusharg_l %r0
#define PUSH_I4()	PUSH_I3()	/**/ movi_l %r0 4  /**/ pusharg_l %r0
#define PUSH_I5()	PUSH_I4()	/**/ movi_l %r0 5  /**/ pusharg_l %r0
#define PUSH_I6()	PUSH_I5()	/**/ movi_l %r0 6  /**/ pusharg_l %r0
#define PUSH_I7()	PUSH_I6()	/**/ movi_l %r0 7  /**/ pusharg_l %r0
#define PUSH_I8()	PUSH_I7()	/**/ movi_l %r0 8  /**/ pusharg_l %r0
#define PUSH_I9()	PUSH_I8()	/**/ movi_l %r0 9  /**/ pusharg_l %r0
#define PUSH_I10()	PUSH_I9()	/**/ movi_l %r0 10 /**/ pusharg_l %r0
#define PUSH_I11()	PUSH_I10()	/**/ movi_l %r0 11 /**/ pusharg_l %r0
#define PUSH_I12()	PUSH_I11()	/**/ movi_l %r0 12 /**/ pusharg_l %r0
#define PUSH_I13()	PUSH_I12()	/**/ movi_l %r0 13 /**/ pusharg_l %r0
#define PUSH_I14()	PUSH_I13()	/**/ movi_l %r0 14 /**/ pusharg_l %r0
#define PUSH_I15()	PUSH_I14()	/**/ movi_l %r0 15 /**/ pusharg_l %r0

#define PUSH_F0()
#define PUSH_F1()			/**/ movi_f %f0 1  /**/ pusharg_f %f0
#define PUSH_F2()	PUSH_F1()	/**/ movi_f %f0 2  /**/ pusharg_f %f0
#define PUSH_F3()	PUSH_F2()	/**/ movi_f %f0 3  /**/ pusharg_f %f0
#define PUSH_F4()	PUSH_F3()	/**/ movi_f %f0 4  /**/ pusharg_f %f0
#define PUSH_F5()	PUSH_F4()	/**/ movi_f %f0 5  /**/ pusharg_f %f0
#define PUSH_F6()	PUSH_F5()	/**/ movi_f %f0 6  /**/ pusharg_f %f0
#define PUSH_F7()	PUSH_F6()	/**/ movi_f %f0 7  /**/ pusharg_f %f0
#define PUSH_F8()	PUSH_F7()	/**/ movi_f %f0 8  /**/ pusharg_f %f0
#define PUSH_F9()	PUSH_F8()	/**/ movi_f %f0 9  /**/ pusharg_f %f0
#define PUSH_F10()	PUSH_F9()	/**/ movi_f %f0 10 /**/ pusharg_f %f0
#define PUSH_F11()	PUSH_F10()	/**/ movi_f %f0 11 /**/ pusharg_f %f0
#define PUSH_F12()	PUSH_F11()	/**/ movi_f %f0 12 /**/ pusharg_f %f0
#define PUSH_F13()	PUSH_F12()	/**/ movi_f %f0 13 /**/ pusharg_f %f0
#define PUSH_F14()	PUSH_F13()	/**/ movi_f %f0 14 /**/ pusharg_f %f0
#define PUSH_F15()	PUSH_F14()	/**/ movi_f %f0 15 /**/ pusharg_f %f0

#define PUSH_D0()
#define PUSH_D1()			/**/ movi_d %f0 1  /**/ pusharg_d %f0
#define PUSH_D2()	PUSH_D1()	/**/ movi_d %f0 2  /**/ pusharg_d %f0
#define PUSH_D3()	PUSH_D2()	/**/ movi_d %f0 3  /**/ pusharg_d %f0
#define PUSH_D4()	PUSH_D3()	/**/ movi_d %f0 4  /**/ pusharg_d %f0
#define PUSH_D5()	PUSH_D4()	/**/ movi_d %f0 5  /**/ pusharg_d %f0
#define PUSH_D6()	PUSH_D5()	/**/ movi_d %f0 6  /**/ pusharg_d %f0
#define PUSH_D7()	PUSH_D6()	/**/ movi_d %f0 7  /**/ pusharg_d %f0
#define PUSH_D8()	PUSH_D7()	/**/ movi_d %f0 8  /**/ pusharg_d %f0
#define PUSH_D9()	PUSH_D8()	/**/ movi_d %f0 9  /**/ pusharg_d %f0
#define PUSH_D10()	PUSH_D9()	/**/ movi_d %f0 10 /**/ pusharg_d %f0
#define PUSH_D11()	PUSH_D10()	/**/ movi_d %f0 11 /**/ pusharg_d %f0
#define PUSH_D12()	PUSH_D11()	/**/ movi_d %f0 12 /**/ pusharg_d %f0
#define PUSH_D13()	PUSH_D12()	/**/ movi_d %f0 13 /**/ pusharg_d %f0
#define PUSH_D14()	PUSH_D13()	/**/ movi_d %f0 14 /**/ pusharg_d %f0
#define PUSH_D15()	PUSH_D14()	/**/ movi_d %f0 15 /**/ pusharg_d %f0

#define FUNCALL(I, F, D)		\
	prepare 1			\
		movi_l %r0 I		\
		pusharg_l %r0		\
	finish fill_l			\
	prepare 1			\
		movi_l %r0 F		\
		pusharg_l %r0		\
	finish fill_f			\
	prepare 1			\
		movi_l %r0 D		\
		pusharg_l %r0		\
	finish fill_d			\
	prepare I			\
	prepare_f F			\
	prepare_d D			\
		PUSH_D##D()		\
		PUSH_F##F()		\
		PUSH_I##I()		\
	finish test_##I##_##F##_##D

#define FUNCALL_I(F, D)		\
	FUNCALL(0, F, D)	\
	FUNCALL(1, F, D)	\
	FUNCALL(2, F, D)	\
	FUNCALL(3, F, D)	\
	FUNCALL(4, F, D)	\
	FUNCALL(5, F, D)	\
	FUNCALL(6, F, D)	\
	FUNCALL(7, F, D)	\
	FUNCALL(8, F, D)	\
	FUNCALL(9, F, D)	\
	FUNCALL(10, F, D)	\
	FUNCALL(11, F, D)	\
	FUNCALL(12, F, D)	\
	FUNCALL(13, F, D)	\
	FUNCALL(14, F, D)	\
	FUNCALL(15, F, D)

#define FUNCALL_F(D)		\
	FUNCALL_I(0, D)		\
	FUNCALL_I(1, D)		\
	FUNCALL_I(2, D)		\
	FUNCALL_I(3, D)		\
	FUNCALL_I(4, D)		\
	FUNCALL_I(5, D)		\
	FUNCALL_I(6, D)		\
	FUNCALL_I(7, D)		\
	FUNCALL_I(8, D)		\
	FUNCALL_I(9, D)		\
	FUNCALL_I(10, D)	\
	FUNCALL_I(11, D)	\
	FUNCALL_I(12, D)	\
	FUNCALL_I(13, D)	\
	FUNCALL_I(14, D)	\
	FUNCALL_I(15, D)

	FUNCALL_F(0)
	FUNCALL_F(1)
	FUNCALL_F(2)
	FUNCALL_F(3)
	FUNCALL_F(4)
	FUNCALL_F(5)
	FUNCALL_F(6)
	FUNCALL_F(7)
	FUNCALL_F(8)
	FUNCALL_F(9)
	FUNCALL_F(10)
	FUNCALL_F(11)
	FUNCALL_F(12)
	FUNCALL_F(13)
	FUNCALL_F(14)
	FUNCALL_F(15)

	ret
