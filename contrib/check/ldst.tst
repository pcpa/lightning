/* mostly a cut&paste of ldst_xr.tst */

#include "ldst_consts.inc"

/*------------------------------------------------------------------------*/
.code	$(4 * 1024 * 1024)
	jmpi main

#define X2Y(C, N, X, Y, R0, V0)				\
	movi_p		%V0 $(t0 + $off##X)		\
	ldr_##X		%R0 %V0				\
	beqi_##Y L##X##2##Y##C	%R0 Y##s##X##N		\
	prepare	3					\
		pusharg_##X	%R0			\
		movi_i		%R0 X##v##N		\
		pusharg_##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##2##Y##C:						\

#define UX2UY(C, N, X, Y, R0, V0)			\
	movi_p		%V0 $(t0 + $offu##X)		\
	ldr_u##X	%R0 %V0				\
	beqi_u##Y Lu##X##2u##Y##C	%R0 X##v##N	\
	prepare	3					\
		pusharg_u##X	%R0			\
		movi_i		%R0 X##v##N		\
		pusharg_u##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
Lu##X##2u##Y##C:					\

#define F2F(C, N, R0, F0, F1, V0)			\
	movi_p		%V0 $(t0 + $offf)		\
	ldr_f		%F0 %V0				\
	movi_f		%F1 fv##N			\
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

#define D2D(C, N, R0, F0, F1, V0)			\
	movi_p		%V0 $(t0 + $offd)		\
	ldr_d		%F0 %V0				\
	movi_d		%F1 dv##N			\
	double_cmp_branch_true(Ld##C, R0, F0, F1)	\
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_f		\
		pusharg_p	%R0			\
	finish @printf					\
Ld##C:

#define F2D(C, N, R0, F0, F1, V0)			\
	movi_p		%V0 $(t0 + $offf)		\
	ldr_f		%F0 %V0				\
	extr_f_d	%F0 %F0				\
	movi_d		%F1 dv##N			\
	double_cmp_branch_true(Lf2d##C, R0, F0, F1)	\
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_fd		\
		pusharg_p	%R0			\
	finish @printf					\
Lf2d##C:

#define D2F(C, N, R0, F0, F1, V0)			\
	movi_p		%V0 $(t0 + $offd)		\
	ldr_d		%F0 %V0				\
	extr_d_f	%F0 %F0				\
	movi_f		%F1 fv##N			\
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

#define STR_0(C, N, V0, R0, F0, F1)			\
L##C:							\
	movi_p	%V0 $(t0 + $offc)			\
	movi_i	%R0 cv##N				\
	str_c	%V0 %R0					\
	movi_p	%V0 $(t0 + $offuc)			\
	str_uc	%V0 %R0					\
	movi_p	%V0 $(t0 + $offs)			\
	movi_i	%R0 sv##N				\
	str_s	%V0 %R0					\
	movi_p	%V0 $(t0 + $offus)			\
	str_us	%V0 %R0					\
	movi_p	%V0 $(t0 + $offi)			\
	movi_i	%R0 iv##N				\
	str_i	%V0 %R0					\
	movi_p	%V0 $(t0 + $offui)			\
	str_ui	%V0 %R0					\
	movi_p	%V0 $(t0 + $offl)			\
	movi_l	%R0 lv##N				\
	str_l	%V0 %R0					\
	movi_p	%V0 $(t0 + $offul)			\
	str_ul	%V0 %R0					\
	movi_p	%V0 $(t0 + $offp)			\
	movi_p	%R0 pv##N				\
	str_p	%V0 %R0					\
	movi_p	%V0 $(t0 + $offf)			\
	movi_f	%F0 fv##N				\
	str_f	%V0 %F0					\
	movi_p	%V0 $(t0 + $offd)			\
	movi_d	%F0 dv##N				\
	str_d	%V0 %F0					\
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
	movi_p	%V0 $(t0 + $offp)			\
	ldr_p	%R0 %V0					\
	beqi_p	Lp##C	%R0 pv##N			\
	prepare 3					\
		pusharg_p	%R0			\
		movi_p		%R0 pv##N		\
		pusharg_p	%R0			\
		movi_p		%R0 fmt_p		\
		pusharg_p	%R0			\
	finish @printf					\
Lp##C:							\
	F2F(C, N, R0, F0, F1, V0)			\
	D2D(C, N, R0, F0, F1, V0)			\
	F2D(C, N, R0, F0, F1, V0)			\
	D2F(C, N, R0, F0, F1, V0)

#define	STR_1(v0, r0, f0, f1)					\
	STR_0(0_##v0##_##r0##_##f0##_##f1, 0, v0, r0, f0, f1)	\
	STR_0(1_##v0##_##r0##_##f0##_##f1, 1, v0, r0, f0, f1)	\
	STR_0(2_##v0##_##r0##_##f0##_##f1, 2, v0, r0, f0, f1)	\
	STR_0(3_##v0##_##r0##_##f0##_##f1, 3, v0, r0, f0, f1)

#define   STR(v0, v1, v2, r0, r1, r2,	f0, f1)		\
	STR_1(v0,	  v1,		f0, f1)		\
	STR_1(v0,	  v2,		f0, f1)		\
	STR_1(v0,	  r0,		f0, f1)		\
	STR_1(v0,	  r1,		f0, f1)		\
	STR_1(v0,	  r2,		f0, f1)		\
	STR_1(v1,	  r0,		f0, f1)		\
	STR_1(v1,	  r1,		f0, f1)		\
	STR_1(v1,	  r2,		f0, f1)		\
	STR_1(v2,	  r0,		f0, f1)		\
	STR_1(v2,	  r1,		f0, f1)		\
	STR_1(v2,	  r2,		f0, f1)

main:
	prolog 0

	STR(v0, v1, v2, r0, r1, r2, f0, f1)

#define CMPX(X, Y, V0, V1, R0, R1, F0, F1)		\
	movi_p	%V0 $(t0 + $off##X)			\
	movi_p	%V1 $(t1 + $off##X)			\
	ldr_##X	%R0 %V0					\
	ldr_##X	%R1 %V1					\
	beqr_##Y L##X##Y##_##V0##_##V1##_##R0##_##R1##_##F0##_##F1 %R0 %R1 \
	prepare	3					\
		pusharg_##X	%R0			\
		pusharg_##X	%R1			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##Y##_##V0##_##V1##_##R0##_##R1##_##F0##_##F1:

#define CMPF(V0, V1, R0, R1, F0, F1)			\
	movi_p	%V0 $(t0 + $offf)			\
	movi_p	%V1 $(t1 + $offf)			\
	ldr_f	%F0 %V0					\
	ldr_f	%F1 %V1					\
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
	movi_p	%V0 $(t0 + $offd)			\
	movi_p	%V1 $(t1 + $offd)			\
	ldr_d	%F0 %V0					\
	ldr_d	%F1 %V1					\
	double_cmp_branch_true(Ldd_##V0##_##V1##_##R0##_##R1##_##F0##_##F1, R0, F0, F1) \
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_d		\
		pusharg_p	%R0			\
	finish @printf					\
Ldd_##V0##_##V1##_##R0##_##R1##_##F0##_##F1:

#define MOVR_1(V0, V1, R0, R1, F0, F1)			\
	movi_p	%V0 $(t0 + $offc)			\
	movi_p	%V1 $(t1 + $offc)			\
	ldr_c	%R0 %V0					\
	str_c	%V1 %R0					\
	movi_p	%V0 $(t0 + $offuc)			\
	movi_p	%V1 $(t1 + $offuc)			\
	ldr_uc	%R0 %V0					\
	str_uc	%V1 %R0					\
	movi_p	%V0 $(t0 + $offs)			\
	movi_p	%V1 $(t1 + $offs)			\
	ldr_s	%R0 %V0					\
	str_s	%V1 %R0					\
	movi_p	%V0 $(t0 + $offus)			\
	movi_p	%V1 $(t1 + $offus)			\
	ldr_us	%R0 %V0					\
	str_us	%V1 %R0					\
	movi_p	%V0 $(t0 + $offi)			\
	movi_p	%V1 $(t1 + $offi)			\
	ldr_i	%R0 %V0					\
	str_i	%V1 %R0					\
	movi_p	%V0 $(t0 + $offui)			\
	movi_p	%V1 $(t1 + $offui)			\
	ldr_ui	%R0 %V0					\
	str_ui	%V1 %R0					\
	movi_p	%V0 $(t0 + $offl)			\
	movi_p	%V1 $(t1 + $offl)			\
	ldr_l	%R0 %V0					\
	str_l	%V1 %R0					\
	movi_p	%V0 $(t0 + $offul)			\
	movi_p	%V1 $(t1 + $offul)			\
	ldr_ul	%R0 %V0					\
	str_ul	%V1 %R0					\
	movi_p	%V0 $(t0 + $offp)			\
	movi_p	%V1 $(t1 + $offp)			\
	ldr_p	%R0 %V0					\
	str_p	%V1 %R0					\
	movi_p	%V0 $(t0 + $offf)			\
	movi_p	%V1 $(t1 + $offf)			\
	ldr_f	%F0 %V0					\
	str_f	%V1 %F0					\
	movi_p	%V0 $(t0 + $offd)			\
	movi_p	%V1 $(t1 + $offd)			\
	ldr_d	%F0 %V0					\
	str_d	%V1 %F0					\
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

#define MOVR_2(V0, V1, R0, R1, F0, F1)			\
	MOVR_1(V0, V1, R0, R1, F0, F1)			\
	MOVR_1(V0, V1, R1, R0, F0, F1)

#define MOVR_3(V0, V1, R0, R1, F0, F1)			\
	MOVR_2(V0, V1, R0, R1, F0, F1)			\
	MOVR_2(V0, R0, V1, R1, F0, F1)			\
	MOVR_2(V0, R1, R0, V1, F0, F1)

#define MOVR_4(V0, V1, R0, R1, F0, F1)			\
	MOVR_3(V0, V1, R0, R1, F0, F1)			\
	MOVR_3(V1, V0, R0, R1, F0, F1)			\
	MOVR_3(R0, V1, V0, R1, F0, F1)			\
	MOVR_3(R1, V1, R0, V0, F0, F1)

#define   MOVR(v0, v1, v2, r0, r1, r2,	f0, f1)		\
	MOVR_4(v0, v1,	   v2, r0,	f0, f1)		\
	MOVR_4(v0, v1,	   v2, r1,	f0, f1)		\
	MOVR_4(v0, v1,	   v2, r2,	f0, f1)		\
	MOVR_4(v0, v1,	   r0, r1,	f0, f1)		\
	MOVR_4(v0, v1,	   r0, r2,	f0, f1)		\
	MOVR_4(v0, v2,	   r0, r1,	f0, f1)		\
	MOVR_4(v0, v2,	   r0, r2,	f0, f1)		\
	MOVR_4(v0, r0,	   r1, r2,	f0, f1)		\
	MOVR_4(v1, r0,	   r1, r2,	f0, f1)		\
	MOVR_4(v2, r0,	   r1, r2,	f0, f1)

	MOVR(v0, v1, v2, r0, r1, r2, f0, f1)

	ret
