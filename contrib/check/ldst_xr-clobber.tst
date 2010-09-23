/* ldxr_* and stxr_* test clobbering on purpose pointer or index register */

#include "ldst_consts.inc"

/*------------------------------------------------------------------------*/
.code	$(4 * 1024 * 1024)
	prolog 0

#define X2Y_R(C, N, X, Y, R0, V0)			\
	movi_i			%R0 $off##X		\
	ldxr_##X		%R0 %V0	%R0		\
	beqi_##Y L##X##2##Y##C	%R0 Y##s##X##N		\
	prepare	3					\
		pusharg_##X	%R0			\
		movi_i		%R0 Y##s##X##N		\
		pusharg_##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##2##Y##C:						\

#define UX2UY_R(C, N, X, Y, R0, V0)			\
	movi_i			%R0 $offu##X		\
	ldxr_u##X		%R0 %V0	%R0		\
	beqi_##Y Lu##X##2u##Y##C	%R0 X##v##N	\
	prepare	3					\
		pusharg_u##X	%R0			\
		movi_i		%R0 X##v##N		\
		pusharg_u##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
Lu##X##2u##Y##C:					\

#define STXR_R_0(C, N, V0, R0)				\
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
	X2Y_R	(C, N, c, i, R0, V0)			\
	X2Y_R	(C, N, c, l, R0, V0)			\
	UX2UY_R	(C, N, c, i, R0, V0)			\
	UX2UY_R	(C, N, c, l, R0, V0)			\
	X2Y_R	(C, N, s, i, R0, V0)			\
	X2Y_R	(C, N, s, l, R0, V0)			\
	UX2UY_R	(C, N, s, i, R0, V0)			\
	UX2UY_R	(C, N, s, l, R0, V0)			\
	X2Y_R	(C, N, i, l, R0, V0)			\
	UX2UY_R	(C, N, i, l, R0, V0)			\
	movi_i		%R0 $offp			\
	ldxr_p		%R0 %V0	%R0			\
	beqi_p	Lp##C	%R0 pv##N			\
	prepare 3					\
		pusharg_p	%R0			\
		movi_p		%R0 pv##N		\
		pusharg_p	%R0			\
		movi_p		%R0 fmt_p		\
		pusharg_p	%R0			\
	finish @printf					\
Lp##C:							\

#define	STXR_R_1(v0, r0)				\
	STXR_R_0(R0_##v0##_##r0, 0, v0, r0)		\
	STXR_R_0(R1_##v0##_##r0, 1, v0, r0)		\
	STXR_R_0(R2_##v0##_##r0, 2, v0, r0)		\
	STXR_R_0(R3_##v0##_##r0, 3, v0, r0)

#define STXR_R_2(V0, R0)				\
	STXR_R_1(V0, R0)				\
	STXR_R_1(R0, V0)

#define   STXR_R(v0, v1, v2, r0, r1, r2)		\
	STXR_R_2(v0, v1)				\
	STXR_R_2(v0, v2)				\
	STXR_R_2(v0, r0)				\
	STXR_R_2(v0, r1)				\
	STXR_R_2(v0, r2)				\
	STXR_R_2(v1, r0)				\
	STXR_R_2(v1, r2)				\
	STXR_R_2(v2, r0)				\
	STXR_R_2(v2, r1)				\
	STXR_R_2(v2, r2)				\

	STXR_R(v0, v1, v2, r0, r1, r2)




#define X2Y_V(C, N, X, Y, R0, V0)			\
	movi_p			%V0 t0			\
	movi_i			%R0 $off##X		\
	ldxr_##X		%V0 %V0	%R0		\
	beqi_##Y L##X##2##Y##C	%V0 Y##s##X##N		\
	prepare	3					\
		pusharg_##X	%V0			\
		movi_i		%R0 Y##s##X##N		\
		pusharg_##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
L##X##2##Y##C:						\

#define UX2UY_V(C, N, X, Y, R0, V0)			\
	movi_p			%V0 t0			\
	movi_i			%R0 $offu##X		\
	ldxr_u##X		%V0 %V0	%R0		\
	beqi_##Y Lu##X##2u##Y##C	%V0 X##v##N	\
	prepare	3					\
		pusharg_u##X	%V0			\
		movi_i		%R0 X##v##N		\
		pusharg_u##X	%R0			\
		movi_p		%R0 fmt_##X		\
		pusharg_p	%R0			\
	finish @printf					\
Lu##X##2u##Y##C:					\

#define STXR_V_0(C, N, V0, R0)				\
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
	X2Y_V	(C, N, c, i, R0, V0)			\
	X2Y_V	(C, N, c, l, R0, V0)			\
	UX2UY_V	(C, N, c, i, R0, V0)			\
	UX2UY_V	(C, N, c, l, R0, V0)			\
	X2Y_V	(C, N, s, i, R0, V0)			\
	X2Y_V	(C, N, s, l, R0, V0)			\
	UX2UY_V	(C, N, s, i, R0, V0)			\
	UX2UY_V	(C, N, s, l, R0, V0)			\
	X2Y_V	(C, N, i, l, R0, V0)			\
	UX2UY_V	(C, N, i, l, R0, V0)			\
	movi_p		%V0 t0				\
	movi_i		%R0 $offp			\
	ldxr_p		%V0 %V0	%R0			\
	beqi_p	Lp##C	%V0 pv##N			\
	prepare 3					\
		pusharg_p	%V0			\
		movi_p		%R0 pv##N		\
		pusharg_p	%R0			\
		movi_p		%R0 fmt_p		\
		pusharg_p	%R0			\
	finish @printf					\
Lp##C:							\

#define	STXR_V_1(v0, r0)				\
	STXR_V_0(V0_##v0##_##r0, 0, v0, r0)		\
	STXR_V_0(V1_##v0##_##r0, 1, v0, r0)		\
	STXR_V_0(V2_##v0##_##r0, 2, v0, r0)		\
	STXR_V_0(V3_##v0##_##r0, 3, v0, r0)

#define STXR_V_2(V0, R0)				\
	STXR_V_1(V0, R0)				\
	STXR_V_1(R0, V0)

#define   STXR_V(v0, v1, v2, r0, r1, r2)		\
	STXR_V_2(v0, v1)				\
	STXR_V_2(v0, v2)				\
	STXR_V_2(v0, r0)				\
	STXR_V_2(v0, r1)				\
	STXR_V_2(v0, r2)				\
	STXR_V_2(v1, r0)				\
	STXR_V_2(v1, r2)				\
	STXR_V_2(v2, r0)				\
	STXR_V_2(v2, r1)				\
	STXR_V_2(v2, r2)				\

	STXR_V(v0, v1, v2, r0, r1, r2)


	ret
