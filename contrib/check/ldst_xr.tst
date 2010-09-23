/* mostly a cut&paste of ldst_xi.tst */

#include "ldst_consts.inc"

/*------------------------------------------------------------------------*/
.code	$(4 * 1024 * 1024)
	jmpi main

#define X2Y(C, N, X, Y, R0, R1, V0)			\
	movi_i			%R1 $off##X		\
	ldxr_##X		%R0 %V0		%R1	\
	movi_l			%R1 Y##s##X##N		\
	beqr_##Y L##X##2##Y##C	%R0 %R1			\
	prepare	3					\
		pusharg_##X	%R0			\
		pusharg_##X	%R1			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##2##Y##C:						\

#define UX2UY(C, N, X, Y, R0, R1, V0)			\
	movi_i			%R1 $offu##X		\
	ldxr_u##X		%R0 %V0		%R1	\
	movi_ul			%R1 X##v##N		\
	beqr_u##Y Lu##X##2u##Y##C	%R0	%R1	\
	prepare	3					\
		pusharg_u##X	%R0			\
		pusharg_u##X	%R1			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
Lu##X##2u##Y##C:					\

#define F2F(C, N, R0, F0, F1, V0)			\
	movi_i			%R0 $offf		\
	ldxr_f			%F0 %V0		%R0	\
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

#define D2D(C, N, R0, F0, F1, V0)			\
	movi_i			%R0 $offd		\
	ldxr_d			%F0 %V0		%R0	\
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

#define F2D(C, N, R0, F0, F1, V0)			\
	movi_i			%R0 $offf		\
	ldxr_f			%F0 %V0		%R0	\
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

#define D2F(C, N, R0, F0, F1, V0)			\
	movi_i			%R0 $offd		\
	ldxr_d			%F0 %V0		%R0	\
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

#define STXR_0(C, N, V0, R0, R1, F0, F1)		\
L##C:							\
	movi_p		%V0 t0				\
	movi_i		%R0 cv##N			\
	movi_i		%R1 $offc			\
	stxr_c	%R1	%V0 %R0				\
	movi_i		%R1 $offuc			\
	stxr_uc	%R1	%V0 %R0				\
	movi_i		%R0 sv##N			\
	movi_i		%R1 $offs			\
	stxr_s	%R1	%V0 %R0				\
	movi_i		%R1 $offus			\
	stxr_us	%R1	%V0 %R0				\
	movi_i		%R0 iv##N			\
	movi_i		%R1 $offi			\
	stxr_i	%R1	%V0 %R0				\
	movi_i		%R1 $offui			\
	stxr_ui	%R1	%V0 %R0				\
	movi_l		%R0 lv##N			\
	movi_i		%R1 $offl			\
	stxr_l	%R1	%V0 %R0				\
	movi_i		%R1 $offul			\
	stxr_ul	%R1	%V0 %R0				\
	movi_p		%R0 pv##N			\
	movi_i		%R1 $offp			\
	stxr_p	%R1	%V0 %R0				\
	movi_f		%F0 fv##N			\
	movi_i		%R1 $offf			\
	stxr_f	%R1	%V0 %F0				\
	movi_d		%F0 dv##N			\
	movi_i		%R1 $offd			\
	stxr_d	%R1	%V0 %F0				\
	X2Y	(C, N, c, i, R0, R1, V0)		\
	X2Y	(C, N, c, l, R0, R1, V0)		\
	UX2UY	(C, N, c, i, R0, R1, V0)		\
	UX2UY	(C, N, c, l, R0, R1, V0)		\
	X2Y	(C, N, s, i, R0, R1, V0)		\
	X2Y	(C, N, s, l, R0, R1, V0)		\
	UX2UY	(C, N, s, i, R0, R1, V0)		\
	UX2UY	(C, N, s, l, R0, R1, V0)		\
	X2Y	(C, N, i, l, R0, R1, V0)		\
	UX2UY	(C, N, i, l, R0, R1, V0)		\
	movi_i		%R1 $offp			\
	ldxr_p		%R0 %V0	%R1			\
	movi_p		%R1 pv##N			\
	beqr_p	Lp##C	%R0 %R1				\
	prepare 3					\
		pusharg_p	%R0			\
		pusharg_p	%R1			\
		movi_p		%R0 fmt_p		\
		pusharg_p	%R0			\
	finish @printf					\
Lp##C:							\
	F2F(C, N, R0, F0, F1, V0)			\
	D2D(C, N, R0, F0, F1, V0)			\
	F2D(C, N, R0, F0, F1, V0)			\
	D2F(C, N, R0, F0, F1, V0)			\

#define	STXR_1(v0, r0, r1, f0, f1)			\
	STXR_0(0_##v0##_##r0##_##r1##_##f0##_##f1,0,v0,r0,r1,f0,f1)	\
	STXR_0(1_##v0##_##r0##_##r1##_##f0##_##f1,1,v0,r0,r1,f0,f1)	\
	STXR_0(2_##v0##_##r0##_##r1##_##f0##_##f1,2,v0,r0,r1,f0,f1)     \
	STXR_0(3_##v0##_##r0##_##r1##_##f0##_##f1,3,v0,r0,r1,f0,f1)

#define STXR_2(V0, R0, R1, F0, F1)			\
	STXR_1(V0, R0, R1, F0, F1)			\
	STXR_1(V0, R1, R0, F0, F1)

#define STXR_3(V0, R0, R1, F0, F1)			\
	STXR_2(V0, R0, R1, F0, F1)			\
	STXR_2(R0, V0, R1, F0, F1)			\
	STXR_2(R1, R0, V0, F0, F1)			\

#define   STXR(v0, v1, v2, r0, r1, r2, f0, f1)		\
	STXR_3(v0, v1, v2, f0, f1)			\
	STXR_3(v0, v1, r0, f0, f1)			\
	STXR_3(v0, v1, r1, f0, f1)			\
	STXR_3(v0, v1, r2, f0, f1)			\
	STXR_3(v0, v2, r0, f0, f1)			\
	STXR_3(v0, v2, r1, f0, f1)			\
	STXR_3(v0, v2, r2, f0, f1)			\
	STXR_3(v0, r0, r1, f0, f1)			\
	STXR_3(v0, r0, r2, f0, f1)			\
	STXR_3(v0, r1, r2, f0, f1)

main:
	prolog 0

	STXR(v0, v1, v2, r0, r1, r2, f0, f1)

#define CMPX(X, Y, V0, V1, R0, R1, R2, F0, F1)		\
	movi_i		%R2 $off##X			\
	ldxr_##X	%R0 %V0 %R2			\
	ldxr_##X	%R1 %V1 %R2			\
	beqr_##Y L##X##Y##_##V0##_##V1##_##R0##_##R1##_##R2##_##F0##_##F1 %R0 %R1 \
	prepare	3					\
		pusharg_##X	%R0			\
		pusharg_##X	%R1			\
		movi_p		%R2 fmt_##X		\
		pusharg_p	%R2			\
	finish @printf					\
L##X##Y##_##V0##_##V1##_##R0##_##R1##_##R2##_##F0##_##F1:

#define CMPF(V0, V1, R0, R1, R2, F0, F1)		\
	movi_i		%R0 $offf			\
	ldxr_f		%F0 %V0 %R0			\
	ldxr_f		%F1 %V1 %R0			\
	float_cmp_branch_true(Lff_##V0##_##V1##_##R0##_##R1##_##R2##_##F0##_##F1, R0, F0, F1) \
	prepare 1					\
	prepare_d 2					\
		extr_f_d	%F0 %F0			\
		pusharg_d	%F0			\
		extr_f_d	%F1 %F1			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_f		\
		pusharg_p	%R0			\
	finish @printf					\
Lff_##V0##_##V1##_##R0##_##R1##_##R2##_##F0##_##F1:

#define CMPD(V0, V1, R0, R1, R2, F0, F1)		\
	movi_i		%R0 $offd			\
	ldxr_d		%F0 %V0 %R0			\
	ldxr_d		%F1 %V1 %R0			\
	double_cmp_branch_true(Ldd_##V0##_##V1##_##R0##_##R1##_##R2##_##F0##_##F1, R0, F0, F1) \
	prepare 1					\
	prepare_d 2					\
		pusharg_d	%F0			\
		pusharg_d	%F1			\
		movi_p		%R0 fmt_d		\
		pusharg_p	%R0			\
	finish @printf					\
Ldd_##V0##_##V1##_##R0##_##R1##_##R2##_##F0##_##F1:

#define MOVXR_1(V0, V1, R0, R1, R2, F0, F1)		\
	movi_p		%V0 t0				\
	movi_p		%V1 t1				\
	movi_i		%R1 $offc			\
	ldxr_c		%R0 %V0	%R1			\
	stxr_c	%R1	%V1 %R0				\
	movi_i		%R1 $offuc			\
	ldxr_uc		%R0 %V0	%R1			\
	stxr_uc	%R1	%V1 %R0				\
	movi_i		%R1 $offs			\
	ldxr_s		%R0 %V0	%R1			\
	stxr_s	%R1	%V1 %R0				\
	movi_i		%R1 $offus			\
	ldxr_us		%R0 %V0	%R1			\
	stxr_us	%R1	%V1 %R0				\
	movi_i		%R1 $offi			\
	ldxr_i		%R0 %V0	%R1			\
	stxr_i	%R1	%V1 %R0				\
	movi_i		%R1 $offui			\
	ldxr_ui		%R0 %V0	%R1			\
	stxr_ui	%R1	%V1 %R0				\
	movi_i		%R1 $offl			\
	ldxr_l		%R0 %V0	%R1			\
	stxr_l	%R1	%V1 %R0				\
	movi_i		%R1 $offul			\
	ldxr_ul		%R0 %V0	%R1			\
	stxr_ul	%R1	%V1 %R0				\
	movi_i		%R1 $offp			\
	ldxr_p		%R0 %V0	%R1			\
	stxr_p	%R1	%V1 %R0				\
	movi_i		%R1 $offf			\
	ldxr_f		%F0 %V0	%R1			\
	stxr_f	%R1	%V1 %F0				\
	movi_i		%R1 $offd			\
	ldxr_d		%F0 %V0	%R1			\
	stxr_d	%R1	%V1 %F0				\
	CMPX( c,  i, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( c,  l, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(uc, ui, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(uc, ul, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( s,  i, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( s,  l, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(us, ui, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(us, ul, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( i,  i, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( i,  l, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(ui, ui, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(ui, ul, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( l,  l, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX(ul, ul, V0, V1, R0, R1, R2, F0, F1)	\
	CMPX( p,  p, V0, V1, R0, R1, R2, F0, F1)	\
	CMPF(	     V0, V1, R0, R1, R2, F0, F1)	\
	CMPD(	     V0, V1, R0, R1, R2, F0, F1)

#define MOVXR_2(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_1(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_1(V0, V1, R0, R2, R1, F0, F1)

#define MOVXR_3(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_2(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_2(V0, V1, R1, R0, R2, F0, F1)		\
	MOVXR_2(V0, V1, R2, R1, R0, F0, F1)

#define MOVXR_4(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_3(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_3(V0, R0, V1, R1, R2, F0, F1)		\
	MOVXR_3(V0, R1, R0, V1, R2, F0, F1)		\
	MOVXR_3(V0, R2, R0, R1, V1, F0, F1)		\

#define MOVXR_5(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_4(V0, V1, R0, R1, R2, F0, F1)		\
	MOVXR_4(V1, V0, R0, R1, R2, F0, F1)		\
	MOVXR_4(R0, V1, V0, R1, R2, F0, F1)		\
	MOVXR_4(R1, V1, R0, V0, R2, F0, F1)		\
	MOVXR_4(R2, V1, R0, R1, V0, F0, F1)

#define   MOVXR(v0, v1, v2, r0, r1, r2, f0, f1)		\
	MOVXR_5(v0, v1, v2, r0, r1, f0, f1)		\
	MOVXR_5(v0, v1, v2, r0, r2, f0, f1)		\
	MOVXR_5(v0, v1, v2, r1, r2, f0, f1)		\
	MOVXR_5(v0, v1, r0, r1, r2, f0, f1)		\
	MOVXR_5(v1, v2, r0, r1, r2, f0, f1)

	MOVXR(v0, v1, v2, r0, r1, r2, f0, f1)

	ret
