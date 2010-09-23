#include "ldst_consts.inc"

/*------------------------------------------------------------------------*/
.code	$(8 * 1024 * 1024)
	jmpi main

/*
 *	signed extension check
 *	basically checks that the high word is minus one
 *	X:		c, s or i
 *	Y:		i or l (i if X is not u)
 */
#define X2Y(C, N, X, Y, R0, V0)				\
	ldxi_##X		%R0 %V0		$off##X	\
	beqi_##Y L##X##2##Y##C	%R0 Y##s##X##N		\
	prepare	3					\
		pusharg_##X	%R0			\
		movi_i		%R0 X##v##N		\
		pusharg_##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##2##Y##C:						\

/*
 *	unsigned extension check
 *	basically checks that the high word is zero
 *	X:		c, s or i
 *	Y:		i or l (i if X is not u)
 */
#define UX2UY(C, N, X, Y, R0, V0)			\
	ldxi_u##X		%R0 %V0		$offu##X\
	beqi_u##Y Lu##X##2u##Y##C	%R0	X##v##N	\
	prepare	3					\
		pusharg_u##X	%R0			\
		movi_i		%R0 X##v##N		\
		pusharg_u##X	%R0			\
		movi_p		%R0 fmt_u##X		\
		pusharg_p	%R0			\
	finish @printf					\
Lu##X##2u##Y##C:					\

/*	float load and compare to constant
 *	printf does not know about float arguments so it is split in two macros
 */
#define F2F(C, N, R0, F0, F1, V0)			\
	ldxi_f			%F0 %V0		$offf	\
	movi_f			%F1 fv##N		\
	float_cmp_branch_true(Lf##C, R0, F0, F1)	\
	prepare 1					\
	prepare_d 2					\
		extr_f_d	%F0 %F0			\
		pusharg_d	%F0			\
		extr_f_d	%F1 %F1			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_f		\
		pusharg_p	%R0			\
	finish @printf					\
Lf##C:

/*	double load and compare to constant
 */
#define D2D(C, N, R0, F0, F1, V0)			\
	ldxi_d			%F0 %V0		$offd	\
	movi_d			%F1 dv##N		\
	double_cmp_branch_true(Ld##C, R0, F0, F1)	\
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_f		\
		pusharg_p	%R0			\
	finish @printf					\
Ld##C:

/*	float load, convert to double and compare to double constant
 */
#define F2D(C, N, R0, F0, F1, V0)			\
	ldxi_f			%F0 %V0		$offf	\
	extr_f_d		%F0 %F0			\
	movi_d			%F1 dv##N		\
	double_cmp_branch_true(Lf2d##C, R0, F0, F1)	\
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_fd		\
		pusharg_p	%R0			\
	finish @printf					\
Lf2d##C:

/*	double load, convert to float and compare to float constant
 */
#define D2F(C, N, R0, F0, F1, V0)			\
	ldxi_d			%F0 %V0		$offd	\
	extr_d_f		%F0 %F0			\
	movi_f			%F1 fv##N		\
	float_cmp_branch_true(Ld2f##C, R0, F0, F1)	\
	prepare 1					\
	prepare_d 2					\
		extr_f_d	%F0 %F0			\
		pusharg_d	%F0			\
		extr_f_d	%F1 %F1			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_df		\
		pusharg_p	%R0			\
	finish @printf					\
Ld2f##C:

/*
 *	C:		unique identifier
 *	N:		0-3 for the testing integer/float value
 *	V0:		integer register (does not need to be JIT_V?)
 *	R0:		integer register (does not need to be JIT_R?)
 *	F0 and F1:	float register
 *
 *	V0 and R0 should be general purpose registers
 */
#define STXI_0(C, N, V0, R0, F0, F1)			\
L##C:							\
	movi_p		%V0 t0				\
	movi_i		%R0 cv##N			\
	stxi_c	$offc	%V0 %R0				\
	stxi_uc	$offuc	%V0 %R0				\
	movi_i		%R0 sv##N			\
	stxi_s	$offs	%V0 %R0				\
	stxi_us	$offus	%V0 %R0				\
	movi_i		%R0 iv##N			\
	stxi_i	$offi	%V0 %R0				\
	stxi_ui	$offui	%V0 %R0				\
	movi_l		%R0 lv##N			\
	stxi_l	$offl	%V0 %R0				\
	stxi_ul	$offul	%V0 %R0				\
	movi_p		%R0 pv##N			\
	stxi_p	$offp	%V0 %R0				\
	movi_f		%F0 fv##N			\
	stxi_f	$offf	%V0 %F0				\
	movi_d		%F0 dv##N			\
	stxi_d	$offd	%V0 %F0				\
							\
	X2Y	(C, N, c, i, R0, V0)			\
	X2Y	(C, N, c, l, R0, V0)			\
	UX2UY	(C, N, c, i, R0, V0)			\
	UX2UY	(C, N, c, l, R0, V0)			\
	X2Y	(C, N, s, i, R0, V0)			\
	X2Y	(C, N, s, l, R0, V0)			\
	UX2UY	(C, N, s, i, R0, V0)			\
	UX2UY	(C, N, s, l, R0, V0)			\
	X2Y	(C, N, i, l, R0, V0)			\
	UX2UY	(C, N, i, l, R0, V0)			\
							\
	ldxi_p		%R0 %V0	$offp			\
	beqi_p	Lp##C	%R0 pv##N			\
	prepare 3					\
		pusharg_p	%R0			\
		movi_p		%R0 pv##N		\
		pusharg_p	%R0			\
		movi_p		%R0 fmt_p		\
		pusharg_p	%R0			\
	finish @printf					\
Lp##C:							\
							\
	F2F(C, N, R0, F0, F1, V0)			\
	D2D(C, N, R0, F0, F1, V0)			\
	F2D(C, N, R0, F0, F1, V0)			\
	D2F(C, N, R0, F0, F1, V0)			\

#define	STXI_1(v0, r0, f0, f1)				\
	STXI_0(0_##v0##_##r0##_##f0##_##f1, 0, v0, r0, f0, f1)	\
	STXI_0(1_##v0##_##r0##_##f0##_##f1, 1, v0, r0, f0, f1)	\
	STXI_0(2_##v0##_##r0##_##f0##_##f1, 2, v0, r0, f0, f1)	\
	STXI_0(3_##v0##_##r0##_##f0##_##f1, 3, v0, r0, f0, f1)

/* combination of float registers tests should be redundant,
 * but better safer than sorry, otherwise, just do something like:
 *	#define VRFF_STXI_FX(v0, r0, f0, f1, f2, f3, f4, f5)	\
 *		VRFF_STXI(v0, r0, f0, f1)
 * or #if 0 or comment entries after the first one
 */
#define	STXI_2(v0, r0, f0, f1, f2, f3, f4, f5)		\
	STXI_1(v0, r0, f0, f1)				\
	STXI_1(v0, r0, f0, f2)				\
	STXI_1(v0, r0, f0, f3)				\
	STXI_1(v0, r0, f0, f4)				\
	STXI_1(v0, r0, f0, f5)				\
	STXI_1(v0, r0, f1, f0)				\
	STXI_1(v0, r0, f1, f2)				\
	STXI_1(v0, r0, f1, f3)				\
	STXI_1(v0, r0, f1, f4)				\
	STXI_1(v0, r0, f1, f5)				\
	STXI_1(v0, r0, f2, f0)				\
	STXI_1(v0, r0, f2, f1)				\
	STXI_1(v0, r0, f2, f3)				\
	STXI_1(v0, r0, f2, f4)				\
	STXI_1(v0, r0, f2, f5)				\
	STXI_1(v0, r0, f3, f0)				\
	STXI_1(v0, r0, f3, f1)				\
	STXI_1(v0, r0, f3, f2)				\
	STXI_1(v0, r0, f3, f4)				\
	STXI_1(v0, r0, f3, f5)				\
	STXI_1(v0, r0, f4, f0)				\
	STXI_1(v0, r0, f4, f1)				\
	STXI_1(v0, r0, f4, f2)				\
	STXI_1(v0, r0, f4, f3)				\
	STXI_1(v0, r0, f4, f5)				\
	STXI_1(v0, r0, f5, f0)				\
	STXI_1(v0, r0, f5, f1)				\
	STXI_1(v0, r0, f5, f2)				\
	STXI_1(v0, r0, f5, f3)				\
	STXI_1(v0, r0, f5, f4)

#define STXI(v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)	\
	STXI_2(v0, v1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v0, v2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v0, r0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v0, r1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v0, r2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v1, v0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v1, v2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v1, r0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v1, r1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v1, r2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v2, v0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v2, v1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v2, r0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v2, r1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(v2, r2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r0, r1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r0, r2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r0, v0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r0, v1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r0, v2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r1, r0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r1, r2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r1, v0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r1, v1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r1, v2, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r2, r0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r2, r1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r2, v0, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r2, v1, f0, f1, f2, f3, f4, f5)		\
	STXI_2(r2, v2, f0, f1, f2, f3, f4, f5)

main:
	prolog 0

	STXI(v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)

#define CMPX(X, Y, V0, V1, R0, R1, F0, F1)		\
	ldxi_##X	%R0 %V0 $off##X			\
	ldxi_##X	%R1 %V1 $off##X			\
	beqr_##Y L##X##Y##_##V0##_##V1##_##R0##_##R1##_##F0##_##F1 %R0 %R1 \
	prepare	3					\
		pusharg_##X	%R0			\
		pusharg_##X	%R1			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##Y##_##V0##_##V1##_##R0##_##R1##_##F0##_##F1:

#define CMPF(V0, V1, R0, R1, F0, F1)			\
	ldxi_f		%F0 %V0 $offf			\
	ldxi_f		%F1 %V1 $offf			\
	float_cmp_branch_true(Lff_##V0##_##V1##_##R0##_##R1##_##F0##_##F1, R0, F0, F1) \
	prepare 1					\
	prepare_d 2					\
		extr_f_d	%F0 %F0			\
		pusharg_d	%F0			\
		extr_f_d	%F1 %F1			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_f		\
		pusharg_p	%R0			\
	finish @printf					\
Lff_##V0##_##V1##_##R0##_##R1##_##F0##_##F1:

#define CMPD(V0, V1, R0, R1, F0, F1)			\
	ldxi_d		%F0 %V0 $offd			\
	ldxi_d		%F1 %V1 $offd			\
	double_cmp_branch_true(Ldd_##V0##_##V1##_##R0##_##R1##_##F0##_##F1, R0, F0, F1) \
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_d		\
		pusharg_p	%R0			\
	finish @printf					\
Ldd_##V0##_##V1##_##R0##_##R1##_##F0##_##F1:

#define MOVXI_1(V0, V1, R0, R1, F0, F1)			\
	movi_p		%V0 t0				\
	movi_p		%V1 t1				\
	ldxi_c		%R0 %V0	$offc			\
	stxi_c	$offc	%V1 %R0				\
	ldxi_uc		%R0 %V0	$offuc			\
	stxi_uc	$offuc	%V1 %R0				\
	ldxi_s		%R0 %V0	$offs			\
	stxi_s	$offs	%V1 %R0				\
	ldxi_us		%R0 %V0	$offus			\
	stxi_us	$offus	%V1 %R0				\
	ldxi_i		%R0 %V0	$offi			\
	stxi_i	$offi	%V1 %R0				\
	ldxi_ui		%R0 %V0	$offui			\
	stxi_ui	$offui	%V1 %R0				\
	ldxi_l		%R0 %V0	$offl			\
	stxi_l	$offl	%V1 %R0				\
	ldxi_ul		%R0 %V0	$offul			\
	stxi_ul	$offul	%V1 %R0				\
	ldxi_p		%R0 %V0	$offp			\
	stxi_p	$offp	%V1 %R0				\
	ldxi_f		%F0 %V0	$offf			\
	stxi_f	$offf	%V1 %F0				\
	ldxi_d		%F0 %V0	$offd			\
	stxi_d	$offd	%V1 %F0				\
	CMPX( c,  i, V0, V1, R0, R1, F0, F1)		\
	CMPX( c,  l, V0, V1, R0, R1, F0, F1)		\
	CMPX(uc, ui, V0, V1, R0, R1, F0, F1)		\
	CMPX(uc, ul, V0, V1, R0, R1, F0, F1)		\
	CMPX( s,  i, V0, V1, R0, R1, F0, F1)		\
	CMPX( s,  l, V0, V1, R0, R1, F0, F1)		\
	CMPX(us, ui, V0, V1, R0, R1, F0, F1)		\
	CMPX(us, ul, V0, V1, R0, R1, F0, F1)		\
	CMPX( i,  i, V0, V1, R0, R1, F0, F1)		\
	CMPX( i,  l, V0, V1, R0, R1, F0, F1)		\
	CMPX(ui, ui, V0, V1, R0, R1, F0, F1)		\
	CMPX(ui, ul, V0, V1, R0, R1, F0, F1)		\
	CMPX( l,  l, V0, V1, R0, R1, F0, F1)		\
	CMPX(ul, ul, V0, V1, R0, R1, F0, F1)		\
	CMPX( p,  p, V0, V1, R0, R1, F0, F1)		\
	CMPF(	     V0, V1, R0, R1, F0, F1)		\
	CMPD(	     V0, V1, R0, R1, F0, F1)

#define MOVXI_2(V0, V1, R0, R1, F0, F1)			\
	MOVXI_1(V0, V1, R0, R1, F0, F1)			\
	MOVXI_1(V0, V1, R1, R0, F0, F1)

#define MOVXI_3(V0, V1, R0, R1, F0, F1)			\
	MOVXI_2(V0, V1, R0, R1, F0, F1)			\
	MOVXI_2(V0, R0, V1, R1, F0, F1)			\
	MOVXI_2(V0, R1, R0, V1, F0, F1)

#define MOVXI_4(V0, V1, R0, R1, F0, F1)			\
	MOVXI_3(V0, V1, R0, R1, F0, F1)			\
	MOVXI_3(V1, V0, R0, R1, F0, F1)			\
	MOVXI_3(R0, V1, V0, R1, F0, F1)			\
	MOVXI_3(R1, V1, R0, V0, F0, F1)

#define   MOVXI(v0, v1, v2, r0, r1, r2, f0, f1)		\
	MOVXI_4(v0, v1, v2, r0, f0, f1)			\
	MOVXI_4(v0, v1, v2, r1, f0, f1)			\
	MOVXI_4(v0, v1, v2, r2, f0, f1)			\
	MOVXI_4(v0, v1, r0, r1, f0, f1)			\
	MOVXI_4(v0, v1, r0, r2, f0, f1)			\
	MOVXI_4(v0, r0, r1, v2, f0, f1)			\
	MOVXI_4(v0, r0, r1, r2, f0, f1)			\
	MOVXI_4(v1, r0, r1, r2, f0, f1)			\
	MOVXI_4(v2, r0, r1, r2, f0, f1)

	MOVXI(v0, v1, v2, r0, r1, r2, f0, f1)

	ret
