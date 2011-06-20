#define extr_f_f(f0, f1)	/**/
#define extr_f_d(f0, f1)	extr_f_d %f0 %f1
#define extr_d_d(f0, f1)	/**/
#define extr_d_f(f0, f1)	extr_d_f %f0 %f1

#define ix7f			0x7fffffff
#define ix80			0x80000000
#if __WORDSIZE == 64
#  define lx7f			0x7fffffffffffffff
#  define lx80			0x8000000000000000
#else
#  define lx7f			ix7f
#  define lx80			ix80
#endif

#if __mips__
#  define nan_i			ix7f
#  define nan_l			lx7f
#else
#  define nan_i			ix80
#  define nan_l			lx80
#endif
#if __mips__ || __arm__
#  define pinf_i		ix7f
#  define pinf_l		lx7f
#else
#  define pinf_i		ix80
#  define pinf_l		lx80
#endif
#define ninf_i			ix80
#define ninf_l			lx80

.data	256
str_rint:
.c	"rint"
str_round:
.c	"round"
str_trunc:
.c	"trunc"
str_floor:
.c	"floor"
str_ceil:
.c	"ceil"
str_i:
.c	"i"
str_l:
.c	"l"
str_f:
.c	"f"
str_d:
.c	"d"
fmt_f2i:
.c	"%2ld: %sr_%s_%s %8.5f = %2ld\n"

.code	$(24 * 1024 * 1024)
	prolog 0

/* r0 = function(f0) 	->	ir = function((ftype)fr) */
#define f2i(n, fn, tt, fr, tr, ir, if, ft, ie)		\
	movi_##ft %fr if				\
	movi_##tt %tr ie				\
	fn##r_##ft##_##tt %ir %fr			\
	beqr_##tt fn##fr##tr##ir##ft##tt##n %ir %tr	\
	prepare 6					\
	prepare_d 1					\
		pusharg_##tt %ir	/* result */	\
		extr_##ft##_d(fr, fr)			\
		pusharg_d %fr		/* argument */	\
		movi_p %ir str_##tt			\
		pusharg_p %ir		/* to type */	\
		movi_p %ir str_##ft			\
		pusharg_p %ir		/* from type */	\
		movi_p %ir str_##fn			\
		pusharg_p %ir		/* function */	\
		pusharg_##tt %tr	/* expected */	\
		movi_p %ir fmt_f2i			\
		pusharg_p %ir		/* format */	\
	finish @printf					\
fn##fr##tr##ir##ft##tt##n:


#define f2i0(n, fn, f0, r0, r1, if, ie, le)	\
	 f2i(n, fn, i, f0, r0, r1, if, f, ie)	\
	 f2i(n, fn, i, f0, r0, r1, if, d, ie)	\
	 f2i(n, fn, l, f0, r0, r1, if, f, le)	\
	 f2i(n, fn, l, f0, r0, r1, if, d, le)

#define f2i1(n, fn, r0, r1, if, ie, le)		\
	f2i0(n, fn, f0, r0, r1, if, ie, le)	\
	f2i0(n, fn, f1, r0, r1, if, ie, le)	\
	f2i0(n, fn, f2, r0, r1, if, ie, le)	\
	f2i0(n, fn, f3, r0, r1, if, ie, le)	\
	f2i0(n, fn, f4, r0, r1, if, ie, le)	\
	f2i0(n, fn, f5, r0, r1, if, ie, le)

#define f2i2(n, fn, r0, r1, if, ie, le)		\
	f2i1(n, fn, r0, r1, if, ie, le)		\
	f2i1(n, fn, r1, r0, if, ie, le)

#define f2i3(n, fn, if, ie, le)			\
	f2i2(n, fn, v0, v1, if, ie, le)		\
	f2i2(n, fn, v0, v2, if, ie, le)		\
	f2i2(n, fn, v0, r0, if, ie, le)		\
	f2i2(n, fn, v0, r1, if, ie, le)		\
	f2i2(n, fn, v0, r2, if, ie, le)		\
	f2i2(n, fn, v1, v2, if, ie, le)		\
	f2i2(n, fn, v1, r0, if, ie, le)		\
	f2i2(n, fn, v1, r1, if, ie, le)		\
	f2i2(n, fn, v1, r2, if, ie, le)		\
	f2i2(n, fn, r0, r1, if, ie, le)		\
	f2i2(n, fn, r0, r2, if, ie, le)		\
	f2i2(n, fn, r1, r2, if, ie, le)

#define f2i4(n, fn, if, ie)			\
	f2i3(n, fn, if, ie, ie)

#define  rint(n, if, ie)	f2i4(n, rint, if, ie)
#define round(n, if, ie)	f2i4(n, round, if, ie)
#define trunc(n, if, ie)	f2i4(n, trunc, if, ie)
#define floor(n, if, ie)	f2i4(n, floor, if, ie)
#define  ceil(n, if, ie)	f2i4(n, ceil,  if, ie)
#define nan_check(n, fn)	f2i3(n, fn, $(0.0/0.0), nan_i, nan_l)
#define pinf_check(n, fn)	f2i3(n, fn, $(1.0/0.0), pinf_i, pinf_l)
#define ninf_check(n, fn)	f2i3(n, fn, $(-1.0/0.0), ninf_i, ninf_l)

	/* ties (or any non integral) to default rounding mode,
	 * usually to nearest even */
	rint(__LINE__,    0.0,			 0)
	rint(__LINE__,    1.0,			 1)
	rint(__LINE__,   -1.0,			-1)
	rint(__LINE__,    2.0,			 2)
	rint(__LINE__,   -2.0,			-2)
	rint(__LINE__,    3.0,			 3)
	rint(__LINE__,   -3.0,			-3)
	rint(__LINE__, $( 1.0/3.0),		 0)
	rint(__LINE__, $(-1.0/3.0),		 0)
	rint(__LINE__, $( 1.0/2.0),		 0)
	rint(__LINE__, $(-1.0/2.0),		 0)
	rint(__LINE__, $( 2.0/3.0),		 1)
	rint(__LINE__, $(-2.0/3.0),		-1)
	rint(__LINE__, $( 3.0/2.0),		 2)
	rint(__LINE__, $(-3.0/2.0),		-2)
	rint(__LINE__, $( 4.0/3.0),		 1)
	rint(__LINE__, $(-4.0/3.0),		-1)
	rint(__LINE__, $( 5.0/3.0),		 2)
	rint(__LINE__, $(-5.0/3.0),		-2)
	rint(__LINE__, $( 5.0/2.0),		 2)
	rint(__LINE__, $(-5.0/2.0),		-2)
	rint(__LINE__, $( 7.0/2.0),		 4)
	rint(__LINE__, $(-7.0/2.0),		-4)
	rint(__LINE__, $( 7.0/3.0),		 2)
	rint(__LINE__, $(-7.0/3.0),		-2)
	rint(__LINE__, $( 0x80000000*1.0),	0x80000000)
	nan_check(__LINE__, rint)
	pinf_check(__LINE__, rint)
	ninf_check(__LINE__, rint)

	/* ties away from zero */
	round(__LINE__,    0.0,			 0)
	round(__LINE__,    1.0,			 1)
	round(__LINE__,   -1.0,			-1)
	round(__LINE__,    2.0,			 2)
	round(__LINE__,   -2.0,			-2)
	round(__LINE__,    3.0,			 3)
	round(__LINE__,   -3.0,			-3)
	round(__LINE__, $( 1.0/3.0),		 0)
	round(__LINE__, $(-1.0/3.0),		 0)
	round(__LINE__, $( 1.0/2.0),		 1)
	round(__LINE__, $(-1.0/2.0),		-1)
	round(__LINE__, $( 2.0/3.0),		 1)
	round(__LINE__, $(-2.0/3.0),		-1)
	round(__LINE__, $( 3.0/2.0),		 2)
	round(__LINE__, $(-3.0/2.0),		-2)
	round(__LINE__, $( 4.0/3.0),		 1)
	round(__LINE__, $(-4.0/3.0),		-1)
	round(__LINE__, $( 5.0/2.0),		 3)
	round(__LINE__, $(-5.0/2.0),		-3)
	round(__LINE__, $( 5.0/3.0),		 2)
	round(__LINE__, $(-5.0/3.0),		-2)
	round(__LINE__, $( 7.0/2.0),		 4)
	round(__LINE__, $(-7.0/2.0),		-4)
	round(__LINE__, $( 7.0/3.0),		 2)
	round(__LINE__, $(-7.0/3.0),		-2)
	round(__LINE__, $( 0x80000000*1.0),	0x80000000)
	nan_check(__LINE__, round)
	pinf_check(__LINE__, round)
	ninf_check(__LINE__, round)

	/* round toward zero */
	trunc(__LINE__,    0.0,			 0)
	trunc(__LINE__,    1.0,			 1)
	trunc(__LINE__,   -1.0,			-1)
	trunc(__LINE__,    2.0,			 2)
	trunc(__LINE__,   -2.0,			-2)
	trunc(__LINE__,    3.0,			 3)
	trunc(__LINE__,   -3.0,			-3)
	trunc(__LINE__, $( 1/3.0),		 0)
	trunc(__LINE__, $(-1/3.0),		 0)
	trunc(__LINE__, $( 1/2.0),		 0)
	trunc(__LINE__, $(-1/2.0),		 0)
	trunc(__LINE__, $( 2/3.0),		 0)
	trunc(__LINE__, $(-2/3.0),		 0)
	trunc(__LINE__, $( 3/2.0),		 1)
	trunc(__LINE__, $(-3/2.0),		-1)
	trunc(__LINE__, $( 4.0/3.0),		 1)
	trunc(__LINE__, $(-4.0/3.0),		-1)
	trunc(__LINE__, $( 5/2.0),		 2)
	trunc(__LINE__, $(-5/2.0),		-2)
	trunc(__LINE__, $( 5.0/3.0),		 1)
	trunc(__LINE__, $(-5.0/3.0),		-1)
	trunc(__LINE__, $( 7/2.0),		 3)
	trunc(__LINE__, $(-7/2.0),		-3)
	trunc(__LINE__, $( 7.0/3.0),		 2)
	trunc(__LINE__, $(-7.0/3.0),		-2)
	trunc(__LINE__, $( 0x80000000*1.0),	0x80000000)
	nan_check(__LINE__, trunc)
	pinf_check(__LINE__, trunc)
	ninf_check(__LINE__, trunc)

	/* round toward minus infinity */
	floor(__LINE__,    0.0,			 0)
	floor(__LINE__,    1.0,			 1)
	floor(__LINE__,   -1.0,			-1)
	floor(__LINE__,    2.0,			 2)
	floor(__LINE__,   -2.0,			-2)
	floor(__LINE__,    3.0,			 3)
	floor(__LINE__,   -3.0,			-3)
	floor(__LINE__, $( 1/3.0),		 0)
	floor(__LINE__, $(-1/3.0),		-1)
	floor(__LINE__, $( 1/2.0),		 0)
	floor(__LINE__, $(-1/2.0),		-1)
	floor(__LINE__, $( 2/3.0),		 0)
	floor(__LINE__, $(-2/3.0),		-1)
	floor(__LINE__, $( 3/2.0),		 1)
	floor(__LINE__, $(-3/2.0),		-2)
	floor(__LINE__, $( 4.0/3.0),		 1)
	floor(__LINE__, $(-4.0/3.0),		-2)
	floor(__LINE__, $( 5/2.0),		 2)
	floor(__LINE__, $(-5/2.0),		-3)
	floor(__LINE__, $( 5.0/3.0),		 1)
	floor(__LINE__, $(-5.0/3.0),		-2)
	floor(__LINE__, $( 7/2.0),		 3)
	floor(__LINE__, $(-7/2.0),		-4)
	floor(__LINE__, $( 7.0/3.0),		 2)
	floor(__LINE__, $(-7.0/3.0),		-3)
	floor(__LINE__, $( 0x80000000*1.0),	0x80000000)
	nan_check(__LINE__, floor)
	pinf_check(__LINE__, floor)
	ninf_check(__LINE__, floor)

	/* round toward plus infinity */
	ceil(__LINE__,    0.0,			 0)
	ceil(__LINE__,    1.0,			 1)
	ceil(__LINE__,   -1.0,			-1)
	ceil(__LINE__,    2.0,			 2)
	ceil(__LINE__,   -2.0,			-2)
	ceil(__LINE__,    3.0,			 3)
	ceil(__LINE__,   -3.0,			-3)
	ceil(__LINE__, $( 1.0/3.0),		 1)
	ceil(__LINE__, $(-1.0/3.0),		 0)
	ceil(__LINE__, $( 1.0/2.0),		 1)
	ceil(__LINE__, $(-1.0/2.0),		 0)
	ceil(__LINE__, $( 2.0/3.0),		 1)
	ceil(__LINE__, $(-2.0/3.0),		 0)
	ceil(__LINE__, $( 3.0/2.0),		 2)
	ceil(__LINE__, $(-3.0/2.0),		-1)
	ceil(__LINE__, $( 4.0/3.0),		 2)
	ceil(__LINE__, $(-4.0/3.0),		-1)
	ceil(__LINE__, $( 5.0/2.0),		 3)
	ceil(__LINE__, $(-5.0/2.0),		-2)
	ceil(__LINE__, $( 5.0/3.0),		 2)
	ceil(__LINE__, $(-5.0/3.0),		-1)
	ceil(__LINE__, $( 7.0/2.0),		 4)
	ceil(__LINE__, $(-7.0/2.0),		-3)
	ceil(__LINE__, $( 7.0/3.0),		 3)
	ceil(__LINE__, $(-7.0/3.0),		-2)
	ceil(__LINE__, $( 0x80000000*1.0),	0x80000000)
	nan_check(__LINE__, ceil)
	pinf_check(__LINE__, ceil)
	ninf_check(__LINE__, ceil)

	ret
