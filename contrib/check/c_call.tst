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
f:
.c	"f"
d:
.c	"d"
fmti:
.c	"(%d) %s%d = %ld\n"
fmtf:
.c	"(%f) %s%d = %f\n"

.code	$(1024 * 1024)
	jmpi main

#define i0	0
#define i1	1
#define i2	$(1-2)
#define i3	$(1-2-3)
#define i4	$(1-2-3-4)
#define i5	$(1-2-3-4-5)
#define i6	$(1-2-3-4-5-6)
#define i7	$(1-2-3-4-5-6-7)
#define i8	$(1-2-3-4-5-6-7-8)
#define i9	$(1-2-3-4-5-6-7-8-9)
#define i10	$(1-2-3-4-5-6-7-8-9-10)
#define i11	$(1-2-3-4-5-6-7-8-9-10-11)
#define i12	$(1-2-3-4-5-6-7-8-9-10-11-12)
#define i13	$(1-2-3-4-5-6-7-8-9-10-11-12-13)
#define i14	$(1-2-3-4-5-6-7-8-9-10-11-12-13-14)
#define i15	$(1-2-3-4-5-6-7-8-9-10-11-12-13-14-15)

#define c0	i0
#define c1	i1
#define c2	i2
#define c3	i3
#define c4	i4
#define c5	i5
#define c6	i6
#define c7	i7
#define c8	i8
#define c9	i9
#define c10	i10
#define c11	i11
#define c12	i12
#define c13	i13
#define c14	i14
#define c15	i15
#define s0	i0
#define s1	i1
#define s2	i2
#define s3	i3
#define s4	i4
#define s5	i5
#define s6	i6
#define s7	i7
#define s8	i8
#define s9	i9
#define s10	i10
#define s11	i11
#define s12	i12
#define s13	i13
#define s14	i14
#define s15	i15
#define ui0	i0
#define ui1	i1
#define ui2	i2
#define ui3	i3
#define ui4	i4
#define ui5	i5
#define ui6	i6
#define ui7	i7
#define ui8	i8
#define ui9	i9
#define ui10	i10
#define ui11	i11
#define ui12	i12
#define ui13	i13
#define ui14	i14
#define ui15	i15
#define l0	i0
#define l1	i1
#define l2	i2
#define l3	i3
#define l4	i4
#define l5	i5
#define l6	i6
#define l7	i7
#define l8	i8
#define l9	i9
#define l10	i10
#define l11	i11
#define l12	i12
#define l13	i13
#define l14	i14
#define l15	i15
#define ul0	l0
#define ul1	l1
#define ul2	l2
#define ul3	l3
#define ul4	l4
#define ul5	l5
#define ul6	l6
#define ul7	l7
#define ul8	l8
#define ul9	l9
#define ul10	l10
#define ul11	l11
#define ul12	l12
#define ul13	l13
#define ul14	l14
#define ul15	l15
#define uc0	0
#define uc1	1
#define uc2	$((1-2)&0xff)
#define uc3	$((1-2-3)&0xff)
#define uc4	$((1-2-3-4)&0xff)
#define uc5	$((1-2-3-4-5)&0xff)
#define uc6	$((1-2-3-4-5-6)&0xff)
#define uc7	$((1-2-3-4-5-6-7)&0xff)
#define uc8	$((1-2-3-4-5-6-7-8)&0xff)
#define uc9	$((1-2-3-4-5-6-7-8-9)&0xff)
#define uc10	$((1-2-3-4-5-6-7-8-9-10)&0xff)
#define uc11	$((1-2-3-4-5-6-7-8-9-10-11)&0xff)
#define uc12	$((1-2-3-4-5-6-7-8-9-10-11-12)&0xff)
#define uc13	$((1-2-3-4-5-6-7-8-9-10-11-12-13)&0xff)
#define uc14	$((1-2-3-4-5-6-7-8-9-10-11-12-13-14)&0xff)
#define uc15	$((1-2-3-4-5-6-7-8-9-10-11-12-13-14-15)&0xff)
#define us0	0
#define us1	1
#define us2	$((1-2)&0xffff)
#define us3	$((1-2-3)&0xffff)
#define us4	$((1-2-3-4)&0xffff)
#define us5	$((1-2-3-4-5)&0xffff)
#define us6	$((1-2-3-4-5-6)&0xffff)
#define us7	$((1-2-3-4-5-6-7)&0xffff)
#define us8	$((1-2-3-4-5-6-7-8)&0xffff)
#define us9	$((1-2-3-4-5-6-7-8-9)&0xffff)
#define us10	$((1-2-3-4-5-6-7-8-9-10)&0xffff)
#define us11	$((1-2-3-4-5-6-7-8-9-10-11)&0xffff)
#define us12	$((1-2-3-4-5-6-7-8-9-10-11-12)&0xffff)
#define us13	$((1-2-3-4-5-6-7-8-9-10-11-12-13)&0xffff)
#define us14	$((1-2-3-4-5-6-7-8-9-10-11-12-13-14)&0xffff)
#define us15	$((1-2-3-4-5-6-7-8-9-10-11-12-13-14-15)&0xffff)

#define extr_f_d(f)	extr_f_d	%f %f
#define extr_d_d(f)	/**/
#define extr_c_i(r)	extr_c_i	%r %r
#define extr_uc_ui(r)	extr_uc_ui	%r %r
#define extr_s_i(r)	extr_s_i	%r %r
#define extr_us_ui(r)	extr_us_ui	%r %r
#define extr_i_i(r)	/**/
#define extr_ui_ui(r)	/**/
#define extr_l_l(r)	/**/
#define extr_ul_ul(r)	/**/
#define extr_i_l(r)	extr_i_l	%r %r
#define extr_ui_ul(r)	extr_ui_ul	%r %r

#define  push0(t, r)
#define  push1(t, r)	movi_##t %r 1  /**/ pusharg_##t %r
#define  push2(t, r)	movi_##t %r 2  /**/ pusharg_##t %r	 push1(t, r)
#define  push3(t, r)	movi_##t %r 3  /**/ pusharg_##t %r	 push2(t, r)
#define  push4(t, r)	movi_##t %r 4  /**/ pusharg_##t %r	 push3(t, r)
#define  push5(t, r)	movi_##t %r 5  /**/ pusharg_##t %r	 push4(t, r)
#define  push6(t, r)	movi_##t %r 6  /**/ pusharg_##t %r	 push5(t, r)
#define  push7(t, r)	movi_##t %r 7  /**/ pusharg_##t %r	 push6(t, r)
#define  push8(t, r)	movi_##t %r 8  /**/ pusharg_##t %r	 push7(t, r)
#define  push9(t, r)	movi_##t %r 9  /**/ pusharg_##t %r	 push8(t, r)
#define push10(t, r)	movi_##t %r 10 /**/ pusharg_##t %r	 push9(t, r)
#define push11(t, r)	movi_##t %r 11 /**/ pusharg_##t %r	push10(t, r)
#define push12(t, r)	movi_##t %r 12 /**/ pusharg_##t %r	push11(t, r)
#define push13(t, r)	movi_##t %r 13 /**/ pusharg_##t %r	push12(t, r)
#define push14(t, r)	movi_##t %r 14 /**/ pusharg_##t %r	push13(t, r)
#define push15(t, r)	movi_##t %r 15 /**/ pusharg_##t %r	push14(t, r)

#define arg0(t)			/**/
#define arg1(t)						arg_##t $arg##t##1
#define arg2(t)			arg1(t)			arg_##t $arg##t##2
#define arg3(t)			arg2(t)			arg_##t $arg##t##3
#define arg4(t)			arg3(t)			arg_##t $arg##t##4
#define arg5(t)			arg4(t)			arg_##t $arg##t##5
#define arg6(t)			arg5(t)			arg_##t $arg##t##6
#define arg7(t)			arg6(t)			arg_##t $arg##t##7
#define arg8(t)			arg7(t)			arg_##t $arg##t##8
#define arg9(t)			arg8(t)			arg_##t $arg##t##9
#define arg10(t)		arg9(t)			arg_##t $arg##t##10
#define arg11(t)		arg10(t)		arg_##t $arg##t##11
#define arg12(t)		arg11(t)		arg_##t $arg##t##12
#define arg13(t)		arg12(t)		arg_##t $arg##t##13
#define arg14(t)		arg13(t)		arg_##t $arg##t##14
#define arg15(t)		arg14(t)		arg_##t $arg##t##15

#define getarg0(t, b, r)	movi_##b %r##0 0
#define getarg1(t, b, r)	getarg_##t %r##0 $arg##t##1
#define getarg2(t, b, r)						\
	getarg1(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##2					\
	subr_##b %r##0 %r##1 %r##0
#define getarg3(t, b, r)						\
	getarg2(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##3					\
	subr_##b %r##0 %r##1 %r##0
#define getarg4(t, b, r)						\
	getarg3(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##4					\
	subr_##b %r##0 %r##1 %r##0
#define getarg5(t, b, r)						\
	getarg4(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##5					\
	subr_##b %r##0 %r##1 %r##0
#define getarg6(t, b, r)						\
	getarg5(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##6					\
	subr_##b %r##0 %r##1 %r##0
#define getarg7(t, b, r)						\
	getarg6(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##7					\
	subr_##b %r##0 %r##1 %r##0
#define getarg8(t, b, r)						\
	getarg7(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##8					\
	subr_##b %r##0 %r##1 %r##0
#define getarg9(t, b, r)						\
	getarg8(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##9					\
	subr_##b %r##0 %r##1 %r##0
#define getarg10(t, b, r)						\
	getarg9(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##10					\
	subr_##b %r##0 %r##1 %r##0
#define getarg11(t, b, r)						\
	getarg10(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##11					\
	subr_##b %r##0 %r##1 %r##0
#define getarg12(t, b, r)						\
	getarg11(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##12					\
	subr_##b %r##0 %r##1 %r##0
#define getarg13(t, b, r)						\
	getarg12(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##13					\
	subr_##b %r##0 %r##1 %r##0
#define getarg14(t, b, r)						\
	getarg13(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##14					\
	subr_##b %r##0 %r##1 %r##0
#define getarg15(t, b, r)						\
	getarg14(t, b, r)						\
	movr_##b %r##1 %r##0						\
	getarg_##t %r##0 $arg##t##15					\
	subr_##b %r##0 %r##1 %r##0

#define defuni(t, b, n)							\
jit_##t##n:								\
	prolog n							\
	arg##n(t)							\
	getarg##n(t, b, r)						\
	extr_##t##_##b(r0)						\
	movr_##b %ret %r0						\
	ret
#define defunf(t, b, n)							\
jit_##t##n:								\
	prolog 0							\
	prolog_##t n							\
	arg##n(t)							\
	getarg##n(t, b, f)						\
	movr_##t %fret %f0						\
	ret

#define  defun0(x, t, b)				defun##x(t, b, 0)
#define  defun1(x, t, b)	 defun0(x, t, b)	defun##x(t, b, 1)
#define  defun2(x, t, b)	 defun1(x, t, b)	defun##x(t, b, 2)
#define  defun3(x, t, b)	 defun2(x, t, b)	defun##x(t, b, 3)
#define  defun4(x, t, b)	 defun3(x, t, b)	defun##x(t, b, 4)
#define  defun5(x, t, b)	 defun4(x, t, b)	defun##x(t, b, 5)
#define  defun6(x, t, b)	 defun5(x, t, b)	defun##x(t, b, 6)
#define  defun7(x, t, b)	 defun6(x, t, b)	defun##x(t, b, 7)
#define  defun8(x, t, b)	 defun7(x, t, b)	defun##x(t, b, 8)
#define  defun9(x, t, b)	 defun8(x, t, b)	defun##x(t, b, 9)
#define defun10(x, t, b)	 defun9(x, t, b)	defun##x(t, b, 10)
#define defun11(x, t, b)	defun10(x, t, b)	defun##x(t, b, 11)
#define defun12(x, t, b)	defun11(x, t, b)	defun##x(t, b, 12)
#define defun13(x, t, b)	defun12(x, t, b)	defun##x(t, b, 13)
#define defun14(x, t, b)	defun13(x, t, b)	defun##x(t, b, 14)
#define defun15(x, t, b)	defun14(x, t, b)	defun##x(t, b, 15)
#define defun(t, b)		defun15(i, t, b)
	defun(c,	i)
	defun(uc,	ui)
	defun(s,	i)
	defun(us,	ui)
	defun(i,	i)
	defun(ui,	ui)
	defun(l,	l)
	defun(ul,	ul)
#undef defun
#define defun(t)		defun15(f, t, t)
	defun(f)
	defun(d)
#undef defun

#define callin(t, b, n)							\
	prepare n							\
		push##n(b, r0)						\
	finish @C##_##t##n						\
	retval_##t %r0							\
	extr_##t##_##b(r0)						\
	beqi_##b LC##t##n %r0 t##n					\
	prepare 5							\
		pusharg_##b %r0						\
		movi_i %r0 n						\
		pusharg_i %r0						\
		movi_p %r0 t						\
		pusharg_p %r0						\
		movi_i %r0 t##n						\
		pusharg_i %r0						\
		movi_p %r0 fmti						\
		pusharg_p %r0						\
	finish @printf							\
LC##t##n:
#define callfn(t, b, n)							\
	prepare 0							\
	prepare_##t n							\
		push##n(t, f0)						\
	finish @C##_##t##n						\
	retval_##t %f0							\
	movi_##t %f1 i##n						\
	beqr_##t LC##t##n %f0 %f1					\
	prepare 3							\
	prepare_d 2							\
		extr_##t##_d(f0)					\
		pusharg_d %f0						\
		movi_i %r0 n						\
		pusharg_i %r0						\
		movi_p %r0 t						\
		pusharg_p %r0						\
		movi_d %f0 i##n						\
		pusharg_d %f0						\
		movi_p %r0 fmtf						\
		pusharg_p %r0						\
	finish @printf							\
LC##t##n:
#define  callx0(x, t, b)				call##x##n(t, b, 0)
#define  callx1(x, t, b)	 callx0(x, t, b)	call##x##n(t, b, 1)
#define  callx2(x, t, b)	 callx1(x, t, b)	call##x##n(t, b, 2)
#define  callx3(x, t, b)	 callx2(x, t, b)	call##x##n(t, b, 3)
#define  callx4(x, t, b)	 callx3(x, t, b)	call##x##n(t, b, 4)
#define  callx5(x, t, b)	 callx4(x, t, b)	call##x##n(t, b, 5)
#define  callx6(x, t, b)	 callx5(x, t, b)	call##x##n(t, b, 6)
#define  callx7(x, t, b)	 callx6(x, t, b)	call##x##n(t, b, 7)
#define  callx8(x, t, b)	 callx7(x, t, b)	call##x##n(t, b, 8)
#define  callx9(x, t, b)	 callx8(x, t, b)	call##x##n(t, b, 9)
#define callx10(x, t, b)	 callx9(x, t, b)	call##x##n(t, b, 10)
#define callx11(x, t, b)	callx10(x, t, b)	call##x##n(t, b, 11)
#define callx12(x, t, b)	callx11(x, t, b)	call##x##n(t, b, 12)
#define callx13(x, t, b)	callx12(x, t, b)	call##x##n(t, b, 13)
#define callx14(x, t, b)	callx13(x, t, b)	call##x##n(t, b, 14)
#define callx15(x, t, b)	callx14(x, t, b)	call##x##n(t, b, 15)
#define calli(t, b)		callx15(i, t, b)
#define callf(t)		callx15(f, t, t)

main:
	prolog 0

	calli(c,	i)
	calli(uc,	ui)
	calli(s,	i)
	calli(us,	ui)
	calli(i,	i)
	calli(ui,	ui)
	calli(l,	l)
	calli(ul,	ul)
	callf(f)
	callf(d)

#define initn(t, n)							\
	movi_p %v0 @jit_##t##n						\
	movi_p %r0 jit_##t##n						\
	str_p %v0 %r0
#define init0(t)	 initn(t, 0)
#define init1(t)	 init0(t)	 initn(t, 1)
#define init2(t)	 init1(t)	 initn(t, 2)
#define init3(t)	 init2(t)	 initn(t, 3)
#define init4(t)	 init3(t)	 initn(t, 4)
#define init5(t)	 init4(t)	 initn(t, 5)
#define init6(t)	 init5(t)	 initn(t, 6)
#define init7(t)	 init6(t)	 initn(t, 7)
#define init8(t)	 init7(t)	 initn(t, 8)
#define init9(t)	 init8(t)	 initn(t, 9)
#define init10(t)	 init9(t)	initn(t, 10)
#define init11(t)	init10(t)	initn(t, 11)
#define init12(t)	init11(t)	initn(t, 12)
#define init13(t)	init12(t)	initn(t, 13)
#define init14(t)	init13(t)	initn(t, 14)
#define init15(t)	init14(t)	initn(t, 15)
#define init(t)		init15(t)

	/* initialize pointers in dynamic C object */
	init(c)
	init(uc)
	init(s)
	init(us)
	init(i)
	init(ui)
	init(l)
	init(ul)
	init(f)
	init(d)

#undef callin
#define callin(t, b, n)							\
	calli @Cjit_##t##n						\
	retval_##t %r0							\
	extr_##t##_##b(r0)						\
	beqi_##b LCjit##t##n %r0 t##n					\
	prepare 5							\
		pusharg_##b %r0						\
		movi_i %r0 n						\
		pusharg_i %r0						\
		movi_p %r0 t						\
		pusharg_p %r0						\
		movi_i %r0 t##n						\
		pusharg_i %r0						\
		movi_p %r0 fmti						\
		pusharg_p %r0						\
	finish @printf							\
LCjit##t##n:
#undef callfn
#define callfn(t, b, n)							\
	calli @Cjit_##t##n						\
	retval_##t %f0							\
	movi_##t %f1 i##n						\
	beqr_##t LCjit##t##n %f0 %f1					\
	prepare 3							\
	prepare_d 2							\
		extr_##t##_d(f0)					\
		pusharg_d %f0						\
		movi_i %r0 n						\
		pusharg_i %r0						\
		movi_p %r0 t						\
		pusharg_p %r0						\
		movi_d %f0 i##n						\
		pusharg_d %f0						\
		movi_p %r0 fmtf						\
		pusharg_p %r0						\
	finish @printf							\
LCjit##t##n:

	/* call C functions, that call back jit, and return what the
	 * jit function returned, and check that the values match */
	calli(c,	i)
	calli(uc,	ui)
	calli(s,	i)
	calli(us,	ui)
	calli(i,	i)
	calli(ui,	ui)
	calli(l,	l)
	calli(ul,	ul)
	callf(f)
	callf(d)

	ret
