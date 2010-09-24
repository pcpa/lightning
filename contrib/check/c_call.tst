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
	prolog 0

#define v0	0
#define v1	1
#define v2	$(1+2)
#define v3	$(1+2+3)
#define v4	$(1+2+3+4)
#define v5	$(1+2+3+4+5)
#define v6	$(1+2+3+4+5+6)
#define v7	$(1+2+3+4+5+6+7)
#define v8	$(1+2+3+4+5+6+7+8)
#define v9	$(1+2+3+4+5+6+7+8+9)
#define v10	$(1+2+3+4+5+6+7+8+9+10)
#define v11	$(1+2+3+4+5+6+7+8+9+10+11)
#define v12	$(1+2+3+4+5+6+7+8+9+10+11+12)
#define v13	$(1+2+3+4+5+6+7+8+9+10+11+12+13)
#define v14	$(1+2+3+4+5+6+7+8+9+10+11+12+13+14)
#define v15	$(1+2+3+4+5+6+7+8+9+10+11+12+13+14+15)

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

#define extr_f_d(f)	extr_f_d %f %f
#define extr_d_d(f)	/**/

#define callin(t, b, n)							\
	prepare n							\
		push##n(b, r0)						\
	finish @t##n							\
	retval_##t %r0							\
	beqi_i t##n %r0 v##n						\
	prepare 5							\
		pusharg_##b %r0						\
		movi_i %r0 n						\
		pusharg_i %r0						\
		movi_p %r0 t						\
		pusharg_p %r0						\
		movi_i %r0 v##n						\
		pusharg_i %r0						\
		movi_p %r0 fmti						\
		pusharg_p %r0						\
	finish @printf							\
t##n:
#define callfn(t, b, n)							\
	prepare 0							\
	prepare_##t n							\
		push##n(t, f0)						\
	finish @t##n							\
	retval_##t %f0							\
	movi_##t %f1 v##n						\
	beqr_##t t##n %f0 %f1						\
	prepare 3							\
	prepare_d 2							\
		extr_##t##_d(f0)					\
		pusharg_d %f0						\
		movi_i %r0 n						\
		pusharg_i %r0						\
		movi_p %r0 t						\
		pusharg_p %r0						\
		movi_d %f0 v##n						\
		pusharg_d %f0						\
		movi_p %r0 fmtf						\
		pusharg_p %r0						\
	finish @printf							\
t##n:
#define callx0(x, t, b)					call##x##n(t, b, 0)
#define callx1(x, t, b)		callx0(x, t, b)		call##x##n(t, b, 1)
#define callx2(x, t, b)		callx1(x, t, b)		call##x##n(t, b, 2)
#define callx3(x, t, b)		callx2(x, t, b)		call##x##n(t, b, 3)
#define callx4(x, t, b)		callx3(x, t, b)		call##x##n(t, b, 4)
#define callx5(x, t, b)		callx4(x, t, b)		call##x##n(t, b, 5)
#define callx6(x, t, b)		callx5(x, t, b)		call##x##n(t, b, 6)
#define callx7(x, t, b)		callx6(x, t, b)		call##x##n(t, b, 7)
#define callx8(x, t, b)		callx7(x, t, b)		call##x##n(t, b, 8)
#define callx9(x, t, b)		callx8(x, t, b)		call##x##n(t, b, 9)
#define callx10(x, t, b)	callx9(x, t, b)		call##x##n(t, b, 10)
#define callx11(x, t, b)	callx10(x, t, b)	call##x##n(t, b, 11)
#define callx12(x, t, b)	callx11(x, t, b)	call##x##n(t, b, 12)
#define callx13(x, t, b)	callx12(x, t, b)	call##x##n(t, b, 13)
#define callx14(x, t, b)	callx13(x, t, b)	call##x##n(t, b, 14)
#define callx15(x, t, b)	callx14(x, t, b)	call##x##n(t, b, 15)
#define calli(t, b)		callx15(i, t, b)
#define callf(t)		callx15(f, t, t)

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
