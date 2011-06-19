#if BRANCH_DIRECT
#  define FBRANCH(OP, LABEL, R0, F0, F1)		\
	b##OP LABEL %F0 %F1
# define IBRANCHR(OP, LABEL, R0, R1, R2)		\
	b##OP LABEL %R0 %R1
# define IBRANCHI(OP, LABEL, R0, R1, IM)		\
	b##OP LABEL %R0 IM
#else
#  define FBRANCH(OP, LABEL, R0, F0, F1)		\
	OP %R0 %F0 %F1					\
	bnei_i LABEL %R0 0
# define IBRANCHR(OP, LABEL, R0, R1, R2)		\
	OP %R2 %R0 %R1					\
	bnei_i LABEL %R2 0
# define IBRANCHI(OP, LABEL, R0, R1, IM)		\
	OP %R1 %R0 IM					\
	bnei_i LABEL %R1 0
#endif

#define I7f			0x7fffffff
#define I80			0x80000000
#define I81			0x80000001
#define Iff			0xffffffff

#if __WORDSIZE == 32
#  define L7f			I7f
#  define L80			I80
#  define L81			I81
#  define Lff			Iff
#else
#  define L7f			0x7fffffffffffffff
#  define L80			0x8000000000000000
#  define L81			0x8000000000000001
#  define Lff			0xffffffffffffffff
#endif

#define BOPR_X_1(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	movi_##X %R1 VR					\
	IBRANCHR(OP##r_##X, OP##r##X##N, R0, R1, R2)	\
	prepare 3					\
		pusharg_##X %R1				\
		pusharg_##X %R0				\
		movi_p %R0 OP##r			\
		pusharg_p %R0				\
	finish @printf					\
 OP##r##X##N:
#define BOPI_X_1(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	IBRANCHI(OP##i_##X, OP##i##X##N, R0, R1, VR)	\
	movi_##X %R1 VR					\
	prepare 3					\
		pusharg_##X %R1				\
		pusharg_##X %R0				\
		movi_p %R0 OP##i			\
		pusharg_p %R0				\
	finish @printf					\
 OP##i##X##N:
#define BOPR_X_2(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	movi_##X %R1 VR					\
	IBRANCHR(OP##r_##X, OP##r0##X##N, R0, R1, R2)	\
	jmpi OP##r1##X##N				\
 OP##r0##X##N:						\
	prepare 3					\
		pusharg_##X %R1				\
		pusharg_##X %R0				\
		movi_p %R0 OP##r			\
		pusharg_p %R0				\
	finish @printf					\
 OP##r1##X##N:
#define BOPI_X_2(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	IBRANCHI(OP##i_##X, OP##i0##X##N, R0, R1, VR)	\
	jmpi OP##i1##X##N				\
	movi_##X %R1 VR					\
 OP##i0##X##N:						\
	prepare 3					\
		pusharg_##X %R1				\
		pusharg_##X %R0				\
		movi_p %R0 OP##i			\
		pusharg_p %R0				\
	finish @printf					\
 OP##i1##X##N:
#define BASR_X_1(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	movi_##X %R1 VR					\
	movr_##X %R2 %R0				\
	b##OP##r_##X OP##r##X##N %R0 %R1		\
	prepare 4					\
		pusharg_##X %R0				\
		pusharg_##X %R1				\
		pusharg_##X %R2				\
		movi_p %R0 OP##r			\
		pusharg_p %R0				\
	finish @printf					\
 OP##r##X##N:
#define BASI_X_1(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	movr_##X %R2 %R0				\
	b##OP##i_##X OP##i##X##N %R0 VR			\
	movi_##X %R1 VR					\
	prepare 4					\
		pusharg_##X %R0				\
		pusharg_##X %R1				\
		pusharg_##X %R2				\
		movi_p %R0 OP##i			\
		pusharg_p %R0				\
	finish @printf					\
 OP##i##X##N:
#define BASR_X_2(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	movi_##X %R1 VR					\
	movr_##X %R2 %R0				\
	b##OP##r_##X OP##r0##X##N %R0 %R1		\
	jmpi OP##r1##X##N				\
 OP##r0##X##N:						\
	prepare 4					\
		pusharg_##X %R0				\
		pusharg_##X %R1				\
		pusharg_##X %R2				\
		movi_p %R0 OP##r			\
		pusharg_p %R0				\
	finish @printf					\
 OP##r1##X##N:
#define BASI_X_2(N, OP, X, VL, VR, R0, R1, R2)		\
	movi_##X %R0 VL					\
	movr_##X %R2 %R0				\
	b##OP##i_##X OP##i0##X##N %R0 VR		\
	jmpi OP##i1##X##N				\
	movi_##X %R1 VR					\
 OP##i0##X##N:						\
	prepare 4					\
		pusharg_##X %R0				\
		pusharg_##X %R1				\
		pusharg_##X %R2				\
		movi_p %R0 OP##i			\
		pusharg_p %R0				\
	finish @printf					\
 OP##i1##X##N:

#define PRINTF_F1_F0(OP, R0, F0, F1)			\
	prepare 1					\
	prepare_d 2					\
		extr_f_d %F1 %F1			\
		pusharg_d %F1				\
		extr_f_d %F0 %F0			\
		pusharg_d %F0				\
		movi_p %R0 OP				\
		pusharg_p %R0				\
	finish @printf
#define PRINTF_F1_F0_F1_F0(OP, R0, F0, F1)		\
	prepare 1					\
	prepare_d 4					\
		extr_f_d %F1 %F1			\
		pusharg_d %F1				\
		extr_f_d %F0 %F0			\
		pusharg_d %F0				\
		pusharg_d %F1				\
		pusharg_d %F0				\
		movi_p %R0 OP				\
		pusharg_p %R0				\
	finish @printf
#define PRINTF_F1_F1_F0_F0(OP, R0, F0, F1)		\
	prepare 1					\
	prepare_d 4					\
		extr_f_d %F1 %F1			\
		pusharg_d %F1				\
		pusharg_d %F1				\
		extr_f_d %F0 %F0			\
		pusharg_d %F0				\
		pusharg_d %F0				\
		movi_p %R0 OP				\
		pusharg_p %R0				\
	finish @printf
#define PRINTD_F1_F0(OP, R0, F0, F1)			\
	prepare 1					\
	prepare_d 2					\
		pusharg_d %F1				\
		pusharg_d %F0				\
		movi_p %R0 OP				\
		pusharg_p %R0				\
	finish @printf
#define PRINTD_F1_F0_F1_F0(OP, R0, F0, F1)		\
	prepare 1					\
	prepare_d 4					\
		pusharg_d %F1				\
		pusharg_d %F0				\
		pusharg_d %F1				\
		pusharg_d %F0				\
		movi_p %R0 OP				\
		pusharg_p %R0				\
	finish @printf
#define PRINTD_F1_F1_F0_F0(OP, R0, F0, F1)		\
	prepare 1					\
	prepare_d 4					\
		pusharg_d %F1				\
		pusharg_d %F1				\
		pusharg_d %F0				\
		pusharg_d %F0				\
		movi_p %R0 OP				\
		pusharg_p %R0				\
	finish @printf
#define BOPF_1(N, OP, VL, VR, R0, F0, F1)		\
	movi_f %F0 VL					\
	movi_f %F1 VR					\
	FBRANCH(OP##r_f, OP##rf##N, R0, F0, F1)		\
	PRINTF_F1_F0(OP, R0, F0, F1)			\
 OP##rf##N:
#define BOPF_2(N, OP, VL, VR, R0, F0, F1)		\
	movi_f %F0 VL					\
	movi_f %F1 VR					\
	FBRANCH(OP##r_f, OP##rf0##N, R0, F0, F1)	\
	jmpi OP##rf1##N					\
 OP##rf0##N:						\
	PRINTF_F1_F0(OP, R0, F0, F1)			\
 OP##rf1##N:
#define BOPD_1(N, OP, VL, VR, R0, F0, F1)		\
	movi_d %F0 VL					\
	movi_d %F1 VR					\
	FBRANCH(OP##r_d, OP##rd##N, R0, F0, F1)		\
	PRINTD_F1_F0(OP, R0, F0, F1)			\
 OP##rd##N:
#define BOPD_2(N, OP, VL, VR, R0, F0, F1)		\
	movi_d %F0 VL					\
	movi_d %F1 VR					\
	FBRANCH(OP##r_d, OP##rd0##N, R0, F0, F1)	\
	jmpi OP##rd1##N					\
 OP##rd0##N:						\
	PRINTD_F1_F0(OP, R0, F0, F1)			\
 OP##rd1##N:
#define BOPF_3(N, OP, VL, VR, R0, F0, F1)		\
	movi_f %F0 VL					\
	movi_f %F1 VR					\
	FBRANCH(OP##r_f, OP##rf##N, R0, F0, F1)		\
	PRINTF_F1_F0_F1_F0(OP, R0, F0, F1)		\
 OP##rf##N:
#define BOPF_4(N, OP, VL, VR, R0, F0, F1)		\
	movi_f %F0 VL					\
	movi_f %F1 VR					\
	FBRANCH(OP##r_f, OP##rf0##N, R0, F0, F1)	\
	jmpi OP##rf1##N					\
 OP##rf0##N:						\
	PRINTF_F1_F0_F1_F0(OP, R0, F0, F1)		\
 OP##rf1##N:
#define BOPD_3(N, OP, VL, VR, R0, F0, F1)		\
	movi_d %F0 VL					\
	movi_d %F1 VR					\
	FBRANCH(OP##r_d, OP##rd##N, R0, F0, F1)		\
	PRINTD_F1_F0_F1_F0(OP, R0, F0, F1)		\
 OP##rd##N:
#define BOPD_4(N, OP, VL, VR, R0, F0, F1)		\
	movi_d %F0 VL					\
	movi_d %F1 VR					\
	FBRANCH(OP##r_d, OP##rd0##N, R0, F0, F1)	\
	jmpi OP##rd1##N					\
 OP##rd0##N:						\
	PRINTD_F1_F0_F1_F0(OP, R0, F0, F1)		\
 OP##rd1##N:
#define BOPF_5(N, OP, VL, VR, R0, F0, F1)		\
	movi_f %F0 VL					\
	movi_f %F1 VR					\
	FBRANCH(OP##r_f, OP##rf##N, R0, F0, F1)		\
	PRINTF_F1_F1_F0_F0(OP, R0, F0, F1)		\
 OP##rf##N:
#define BOPF_6(N, OP, VL, VR, R0, F0, F1)		\
	movi_f %F0 VL					\
	movi_f %F1 VR					\
	FBRANCH(OP##r_f, OP##rf0##N, R0, F0, F1)	\
	jmpi OP##rf1##N					\
 OP##rf0##N:						\
	PRINTF_F1_F1_F0_F0(OP, R0, F0, F1)		\
 OP##rf1##N:
#define BOPD_5(N, OP, VL, VR, R0, F0, F1)		\
	movi_d %F0 VL					\
	movi_d %F1 VR					\
	FBRANCH(OP##r_d, OP##rd##N, R0, F0, F1)		\
	PRINTD_F1_F1_F0_F0(OP, R0, F0, F1)		\
 OP##rd##N:
#define BOPD_6(N, OP, VL, VR, R0, F0, F1)		\
	movi_d %F0 VL					\
	movi_d %F1 VR					\
	FBRANCH(OP##r_d, OP##rd0##N, R0, F0, F1)	\
	jmpi OP##rd1##N					\
 OP##rd0##N:						\
	PRINTD_F1_F1_F0_F0(OP, R0, F0, F1)		\
 OP##rd1##N:

#define    BLTR_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   lt, i,VL,VR,R0,R1,R2)
#define    BLTR_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   lt, i,VL,VR,R0,R1,R2)
#define    BLTI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   lt, i,VL,VR,R0,R1,R2)
#define    BLTI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   lt, i,VL,VR,R0,R1,R2)
#define    BLTR_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   lt, l,VL,VR,R0,R1,R2)
#define    BLTR_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   lt, l,VL,VR,R0,R1,R2)
#define    BLTI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   lt, l,VL,VR,R0,R1,R2)
#define    BLTI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   lt, l,VL,VR,R0,R1,R2)
#define   BLTR_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   lt,ui,VL,VR,R0,R1,R2)
#define   BLTR_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   lt,ui,VL,VR,R0,R1,R2)
#define   BLTI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   lt,ui,VL,VR,R0,R1,R2)
#define   BLTI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   lt,ui,VL,VR,R0,R1,R2)
#define   BLTR_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   lt,ul,VL,VR,R0,R1,R2)
#define   BLTR_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   lt,ul,VL,VR,R0,R1,R2)
#define   BLTI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   lt,ul,VL,VR,R0,R1,R2)
#define   BLTI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   lt,ul,VL,VR,R0,R1,R2)
#define    BLTR_P_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   lt, p,VL,VR,R0,R1,R2)
#define    BLTR_P_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   lt, p,VL,VR,R0,R1,R2)
#define    BLTI_P_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   lt, p,VL,VR,R0,R1,R2)
#define    BLTI_P_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   lt, p,VL,VR,R0,R1,R2)
#define    BLER_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   le, i,VL,VR,R0,R1,R2)
#define    BLER_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   le, i,VL,VR,R0,R1,R2)
#define    BLEI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   le, i,VL,VR,R0,R1,R2)
#define    BLEI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   le, i,VL,VR,R0,R1,R2)
#define    BLER_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   le, l,VL,VR,R0,R1,R2)
#define    BLER_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   le, l,VL,VR,R0,R1,R2)
#define    BLEI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   le, l,VL,VR,R0,R1,R2)
#define    BLEI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   le, l,VL,VR,R0,R1,R2)
#define   BLER_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   le,ui,VL,VR,R0,R1,R2)
#define   BLER_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   le,ui,VL,VR,R0,R1,R2)
#define   BLEI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   le,ui,VL,VR,R0,R1,R2)
#define   BLEI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   le,ui,VL,VR,R0,R1,R2)
#define   BLER_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   le,ul,VL,VR,R0,R1,R2)
#define   BLER_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   le,ul,VL,VR,R0,R1,R2)
#define   BLEI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   le,ul,VL,VR,R0,R1,R2)
#define   BLEI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   le,ul,VL,VR,R0,R1,R2)
#define    BLER_P_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   le, p,VL,VR,R0,R1,R2)
#define    BLEI_P_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   le, p,VL,VR,R0,R1,R2)
#define    BLER_P_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   le, p,VL,VR,R0,R1,R2)
#define    BLEI_P_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   le, p,VL,VR,R0,R1,R2)
#define    BEQR_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   eq, i,VL,VR,R0,R1,R2)
#define    BEQR_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   eq, i,VL,VR,R0,R1,R2)
#define    BEQI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   eq, i,VL,VR,R0,R1,R2)
#define    BEQI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   eq, i,VL,VR,R0,R1,R2)
#define    BEQR_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   eq, l,VL,VR,R0,R1,R2)
#define    BEQR_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   eq, l,VL,VR,R0,R1,R2)
#define    BEQI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   eq, l,VL,VR,R0,R1,R2)
#define    BEQI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   eq, l,VL,VR,R0,R1,R2)
#define   BEQR_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   eq,ui,VL,VR,R0,R1,R2)
#define   BEQR_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   eq,ui,VL,VR,R0,R1,R2)
#define   BEQI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   eq,ui,VL,VR,R0,R1,R2)
#define   BEQI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   eq,ui,VL,VR,R0,R1,R2)
#define   BEQR_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   eq,ul,VL,VR,R0,R1,R2)
#define   BEQR_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   eq,ul,VL,VR,R0,R1,R2)
#define   BEQI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   eq,ul,VL,VR,R0,R1,R2)
#define   BEQI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   eq,ul,VL,VR,R0,R1,R2)
#define    BEQR_P_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   eq, p,VL,VR,R0,R1,R2)
#define    BEQI_P_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   eq, p,VL,VR,R0,R1,R2)
#define    BEQR_P_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   eq, p,VL,VR,R0,R1,R2)
#define    BEQI_P_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   eq, p,VL,VR,R0,R1,R2)
#define    BGER_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ge, i,VL,VR,R0,R1,R2)
#define    BGER_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ge, i,VL,VR,R0,R1,R2)
#define    BGEI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ge, i,VL,VR,R0,R1,R2)
#define    BGEI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ge, i,VL,VR,R0,R1,R2)
#define    BGER_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ge, l,VL,VR,R0,R1,R2)
#define    BGER_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ge, l,VL,VR,R0,R1,R2)
#define    BGEI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ge, l,VL,VR,R0,R1,R2)
#define    BGEI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ge, l,VL,VR,R0,R1,R2)
#define   BGER_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ge,ui,VL,VR,R0,R1,R2)
#define   BGER_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ge,ui,VL,VR,R0,R1,R2)
#define   BGEI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ge,ui,VL,VR,R0,R1,R2)
#define   BGEI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ge,ui,VL,VR,R0,R1,R2)
#define   BGER_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ge,ul,VL,VR,R0,R1,R2)
#define   BGER_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ge,ul,VL,VR,R0,R1,R2)
#define   BGEI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ge,ul,VL,VR,R0,R1,R2)
#define   BGEI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ge,ul,VL,VR,R0,R1,R2)
#define    BGER_P_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ge, p,VL,VR,R0,R1,R2)
#define    BGEI_P_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ge, p,VL,VR,R0,R1,R2)
#define    BGER_P_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ge, p,VL,VR,R0,R1,R2)
#define    BGEI_P_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ge, p,VL,VR,R0,R1,R2)
#define    BGTR_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   gt, i,VL,VR,R0,R1,R2)
#define    BGTR_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   gt, i,VL,VR,R0,R1,R2)
#define    BGTI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   gt, i,VL,VR,R0,R1,R2)
#define    BGTI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   gt, i,VL,VR,R0,R1,R2)
#define    BGTR_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   gt, l,VL,VR,R0,R1,R2)
#define    BGTR_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   gt, l,VL,VR,R0,R1,R2)
#define    BGTI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   gt, l,VL,VR,R0,R1,R2)
#define    BGTI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   gt, l,VL,VR,R0,R1,R2)
#define   BGTR_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   gt,ui,VL,VR,R0,R1,R2)
#define   BGTR_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   gt,ui,VL,VR,R0,R1,R2)
#define   BGTI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   gt,ui,VL,VR,R0,R1,R2)
#define   BGTI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   gt,ui,VL,VR,R0,R1,R2)
#define   BGTR_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   gt,ul,VL,VR,R0,R1,R2)
#define   BGTR_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   gt,ul,VL,VR,R0,R1,R2)
#define   BGTI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   gt,ul,VL,VR,R0,R1,R2)
#define   BGTI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   gt,ul,VL,VR,R0,R1,R2)
#define    BGTR_P_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   gt, p,VL,VR,R0,R1,R2)
#define    BGTI_P_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   gt, p,VL,VR,R0,R1,R2)
#define    BGTR_P_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   gt, p,VL,VR,R0,R1,R2)
#define    BGTI_P_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   gt, p,VL,VR,R0,R1,R2)
#define    BNER_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ne, i,VL,VR,R0,R1,R2)
#define    BNER_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ne, i,VL,VR,R0,R1,R2)
#define    BNEI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ne, i,VL,VR,R0,R1,R2)
#define    BNEI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ne, i,VL,VR,R0,R1,R2)
#define    BNER_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ne, l,VL,VR,R0,R1,R2)
#define    BNER_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ne, l,VL,VR,R0,R1,R2)
#define    BNEI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ne, l,VL,VR,R0,R1,R2)
#define    BNEI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ne, l,VL,VR,R0,R1,R2)
#define   BNER_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ne,ui,VL,VR,R0,R1,R2)
#define   BNER_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ne,ui,VL,VR,R0,R1,R2)
#define   BNEI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ne,ui,VL,VR,R0,R1,R2)
#define   BNEI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ne,ui,VL,VR,R0,R1,R2)
#define   BNER_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ne,ul,VL,VR,R0,R1,R2)
#define   BNER_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ne,ul,VL,VR,R0,R1,R2)
#define   BNEI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ne,ul,VL,VR,R0,R1,R2)
#define   BNEI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ne,ul,VL,VR,R0,R1,R2)
#define    BNER_P_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ne, p,VL,VR,R0,R1,R2)
#define    BNEI_P_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ne, p,VL,VR,R0,R1,R2)
#define    BNER_P_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ne, p,VL,VR,R0,R1,R2)
#define    BNEI_P_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ne, p,VL,VR,R0,R1,R2)
#define    BMSR_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ms, i,VL,VR,R0,R1,R2)
#define    BMSR_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ms, i,VL,VR,R0,R1,R2)
#define    BMSI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ms, i,VL,VR,R0,R1,R2)
#define    BMSI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ms, i,VL,VR,R0,R1,R2)
#define    BMSR_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ms, l,VL,VR,R0,R1,R2)
#define    BMSR_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ms, l,VL,VR,R0,R1,R2)
#define    BMSI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ms, l,VL,VR,R0,R1,R2)
#define    BMSI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ms, l,VL,VR,R0,R1,R2)
#define   BMSR_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ms,ui,VL,VR,R0,R1,R2)
#define   BMSR_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ms,ui,VL,VR,R0,R1,R2)
#define   BMSI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ms,ui,VL,VR,R0,R1,R2)
#define   BMSI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ms,ui,VL,VR,R0,R1,R2)
#define   BMSR_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   ms,ul,VL,VR,R0,R1,R2)
#define   BMSR_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   ms,ul,VL,VR,R0,R1,R2)
#define   BMSI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   ms,ul,VL,VR,R0,R1,R2)
#define   BMSI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   ms,ul,VL,VR,R0,R1,R2)
#define    BMCR_I_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   mc, i,VL,VR,R0,R1,R2)
#define    BMCR_I_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   mc, i,VL,VR,R0,R1,R2)
#define    BMCI_I_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   mc, i,VL,VR,R0,R1,R2)
#define    BMCI_I_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   mc, i,VL,VR,R0,R1,R2)
#define    BMCR_L_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   mc, l,VL,VR,R0,R1,R2)
#define    BMCR_L_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   mc, l,VL,VR,R0,R1,R2)
#define    BMCI_L_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   mc, l,VL,VR,R0,R1,R2)
#define    BMCI_L_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   mc, l,VL,VR,R0,R1,R2)
#define   BMCR_UI_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   mc,ui,VL,VR,R0,R1,R2)
#define   BMCR_UI_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   mc,ui,VL,VR,R0,R1,R2)
#define   BMCI_UI_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   mc,ui,VL,VR,R0,R1,R2)
#define   BMCI_UI_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   mc,ui,VL,VR,R0,R1,R2)
#define   BMCR_UL_t(N,VL,VR,R0,R1,R2)	BOPR_X_1(N,   mc,ul,VL,VR,R0,R1,R2)
#define   BMCR_UL_f(N,VL,VR,R0,R1,R2)	BOPR_X_2(N,   mc,ul,VL,VR,R0,R1,R2)
#define   BMCI_UL_t(N,VL,VR,R0,R1,R2)	BOPI_X_1(N,   mc,ul,VL,VR,R0,R1,R2)
#define   BMCI_UL_f(N,VL,VR,R0,R1,R2)	BOPI_X_2(N,   mc,ul,VL,VR,R0,R1,R2)
#define  BOADDR_I_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, oadd, i,VL,VR,R0,R1,R2)
#define  BOADDR_I_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, oadd, i,VL,VR,R0,R1,R2)
#define  BOADDI_I_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, oadd, i,VL,VR,R0,R1,R2)
#define  BOADDI_I_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, oadd, i,VL,VR,R0,R1,R2)
#define  BOADDR_L_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, oadd, l,VL,VR,R0,R1,R2)
#define  BOADDR_L_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, oadd, l,VL,VR,R0,R1,R2)
#define  BOADDI_L_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, oadd, l,VL,VR,R0,R1,R2)
#define  BOADDI_L_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, oadd, l,VL,VR,R0,R1,R2)
#define BOADDR_UI_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, oadd,ui,VL,VR,R0,R1,R2)
#define BOADDR_UI_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, oadd,ui,VL,VR,R0,R1,R2)
#define BOADDI_UI_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, oadd,ui,VL,VR,R0,R1,R2)
#define BOADDI_UI_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, oadd,ui,VL,VR,R0,R1,R2)
#define BOADDR_UL_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, oadd,ul,VL,VR,R0,R1,R2)
#define BOADDR_UL_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, oadd,ul,VL,VR,R0,R1,R2)
#define BOADDI_UL_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, oadd,ul,VL,VR,R0,R1,R2)
#define BOADDI_UL_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, oadd,ul,VL,VR,R0,R1,R2)
#define  BOSUBR_I_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, osub, i,VL,VR,R0,R1,R2)
#define  BOSUBR_I_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, osub, i,VL,VR,R0,R1,R2)
#define  BOSUBI_I_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, osub, i,VL,VR,R0,R1,R2)
#define  BOSUBI_I_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, osub, i,VL,VR,R0,R1,R2)
#define  BOSUBR_L_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, osub, l,VL,VR,R0,R1,R2)
#define  BOSUBR_L_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, osub, l,VL,VR,R0,R1,R2)
#define  BOSUBI_L_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, osub, l,VL,VR,R0,R1,R2)
#define  BOSUBI_L_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, osub, l,VL,VR,R0,R1,R2)
#define BOSUBR_UI_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, osub,ui,VL,VR,R0,R1,R2)
#define BOSUBR_UI_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, osub,ui,VL,VR,R0,R1,R2)
#define BOSUBI_UI_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, osub,ui,VL,VR,R0,R1,R2)
#define BOSUBI_UI_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, osub,ui,VL,VR,R0,R1,R2)
#define BOSUBR_UL_t(N,VL,VR,R0,R1,R2)	BASR_X_1(N, osub,ul,VL,VR,R0,R1,R2)
#define BOSUBR_UL_f(N,VL,VR,R0,R1,R2)	BASR_X_2(N, osub,ul,VL,VR,R0,R1,R2)
#define BOSUBI_UL_t(N,VL,VR,R0,R1,R2)	BASI_X_1(N, osub,ul,VL,VR,R0,R1,R2)
#define BOSUBI_UL_f(N,VL,VR,R0,R1,R2)	BASI_X_2(N, osub,ul,VL,VR,R0,R1,R2)
#define      BLTF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N,   lt,   VL,VR,R0,F0,F1)
#define      BLTD_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N,   lt,   VL,VR,R0,F0,F1)
#define      BLTF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N,   lt,   VL,VR,R0,F0,F1)
#define      BLTD_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N,   lt,   VL,VR,R0,F0,F1)
#define      BLEF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N,   le,   VL,VR,R0,F0,F1)
#define      BLED_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N,   le,   VL,VR,R0,F0,F1)
#define      BLEF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N,   le,   VL,VR,R0,F0,F1)
#define      BLED_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N,   le,   VL,VR,R0,F0,F1)
#define      BEQF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N,   eq,   VL,VR,R0,F0,F1)
#define      BEQD_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N,   eq,   VL,VR,R0,F0,F1)
#define      BEQF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N,   eq,   VL,VR,R0,F0,F1)
#define      BEQD_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N,   eq,   VL,VR,R0,F0,F1)
#define      BGEF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N,   ge,   VL,VR,R0,F0,F1)
#define      BGED_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N,   ge,   VL,VR,R0,F0,F1)
#define      BGEF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N,   ge,   VL,VR,R0,F0,F1)
#define      BGED_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N,   ge,   VL,VR,R0,F0,F1)
#define      BGTF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N,   gt,   VL,VR,R0,F0,F1)
#define      BGTD_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N,   gt,   VL,VR,R0,F0,F1)
#define      BGTF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N,   gt,   VL,VR,R0,F0,F1)
#define      BGTD_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N,   gt,   VL,VR,R0,F0,F1)
#define      BNEF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N,   ne,   VL,VR,R0,F0,F1)
#define      BNED_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N,   ne,   VL,VR,R0,F0,F1)
#define      BNEF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N,   ne,   VL,VR,R0,F0,F1)
#define      BNED_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N,   ne,   VL,VR,R0,F0,F1)
#define    BUNLTF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N, unlt,   VL,VR,R0,F0,F1)
#define    BUNLTD_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N, unlt,   VL,VR,R0,F0,F1)
#define    BUNLTF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N, unlt,   VL,VR,R0,F0,F1)
#define    BUNLTD_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N, unlt,   VL,VR,R0,F0,F1)
#define    BUNLEF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N, unle,   VL,VR,R0,F0,F1)
#define    BUNLED_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N, unle,   VL,VR,R0,F0,F1)
#define    BUNLEF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N, unle,   VL,VR,R0,F0,F1)
#define    BUNLED_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N, unle,   VL,VR,R0,F0,F1)
#define    BUNEQF_t(N,VL,VR,R0,F0,F1)	  BOPF_3(N, uneq,   VL,VR,R0,F0,F1)
#define    BUNEQD_t(N,VL,VR,R0,F0,F1)	  BOPD_3(N, uneq,   VL,VR,R0,F0,F1)
#define    BUNEQF_f(N,VL,VR,R0,F0,F1)	  BOPF_4(N, uneq,   VL,VR,R0,F0,F1)
#define    BUNEQD_f(N,VL,VR,R0,F0,F1)	  BOPD_4(N, uneq,   VL,VR,R0,F0,F1)
#define    BUNGEF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N, unge,   VL,VR,R0,F0,F1)
#define    BUNGED_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N, unge,   VL,VR,R0,F0,F1)
#define    BUNGEF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N, unge,   VL,VR,R0,F0,F1)
#define    BUNGED_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N, unge,   VL,VR,R0,F0,F1)
#define    BUNGTF_t(N,VL,VR,R0,F0,F1)	  BOPF_1(N, ungt,   VL,VR,R0,F0,F1)
#define    BUNGTD_t(N,VL,VR,R0,F0,F1)	  BOPD_1(N, ungt,   VL,VR,R0,F0,F1)
#define    BUNGTF_f(N,VL,VR,R0,F0,F1)	  BOPF_2(N, ungt,   VL,VR,R0,F0,F1)
#define    BUNGTD_f(N,VL,VR,R0,F0,F1)	  BOPD_2(N, ungt,   VL,VR,R0,F0,F1)
#define    BLTGTF_t(N,VL,VR,R0,F0,F1)	  BOPF_3(N, ltgt,   VL,VR,R0,F0,F1)
#define    BLTGTD_t(N,VL,VR,R0,F0,F1)	  BOPD_3(N, ltgt,   VL,VR,R0,F0,F1)
#define    BLTGTF_f(N,VL,VR,R0,F0,F1)	  BOPF_4(N, ltgt,   VL,VR,R0,F0,F1)
#define    BLTGTD_f(N,VL,VR,R0,F0,F1)	  BOPD_4(N, ltgt,   VL,VR,R0,F0,F1)
#define     BORDF_t(N,VL,VR,R0,F0,F1)	  BOPF_5(N,  ord,   VL,VR,R0,F0,F1)
#define     BORDD_t(N,VL,VR,R0,F0,F1)	  BOPD_5(N,  ord,   VL,VR,R0,F0,F1)
#define     BORDF_f(N,VL,VR,R0,F0,F1)	  BOPF_6(N,  ord,   VL,VR,R0,F0,F1)
#define     BORDD_f(N,VL,VR,R0,F0,F1)	  BOPD_6(N,  ord,   VL,VR,R0,F0,F1)
#define   BUNORDF_t(N,VL,VR,R0,F0,F1)	  BOPF_5(N,unord,   VL,VR,R0,F0,F1)
#define   BUNORDD_t(N,VL,VR,R0,F0,F1)	  BOPD_5(N,unord,   VL,VR,R0,F0,F1)
#define   BUNORDF_f(N,VL,VR,R0,F0,F1)	  BOPF_6(N,unord,   VL,VR,R0,F0,F1)
#define   BUNORDD_f(N,VL,VR,R0,F0,F1)	  BOPD_6(N,unord,   VL,VR,R0,F0,F1)

/*--------------------------------------------------------------------*/
#define BRANCHF1(BRANCH, N, R0, F0, F1)				\
	TESTF(BRANCH##F, N, R0, F0, F1)				\
	TESTF(BRANCH##D, N, R0, F0, F1)

#define BRANCHF2(BRANCH, R0, F0,F1,F2,F3,F4,F5)			\
	BRANCHF1(BRANCH, R0##F0##F1, R0, F0, F1)		\
	BRANCHF1(BRANCH, R0##F0##F2, R0, F0, F2)		\
	BRANCHF1(BRANCH, R0##F0##F3, R0, F0, F3)		\
	BRANCHF1(BRANCH, R0##F0##F4, R0, F0, F4)		\
	BRANCHF1(BRANCH, R0##F0##F5, R0, F0, F5)		\
	BRANCHF1(BRANCH, R0##F1##F2, R0, F1, F2)		\
	BRANCHF1(BRANCH, R0##F1##F3, R0, F1, F3)		\
	BRANCHF1(BRANCH, R0##F1##F4, R0, F1, F4)		\
	BRANCHF1(BRANCH, R0##F1##F5, R0, F1, F5)		\
	BRANCHF1(BRANCH, R0##F2##F3, R0, F2, F3)		\
	BRANCHF1(BRANCH, R0##F2##F4, R0, F2, F4)		\
	BRANCHF1(BRANCH, R0##F2##F5, R0, F2, F5)		\
	BRANCHF1(BRANCH, R0##F3##F4, R0, F3, F4)		\
	BRANCHF1(BRANCH, R0##F3##F5, R0, F3, F5)		\
	BRANCHF1(BRANCH, R0##F4##F5, R0, F4, F5)

#define BRANCHF( BRANCH, V0,V1,V2,R0,R1,R2,F0,F1,F2,F3,F4,F5)	\
	BRANCHF2(BRANCH, V0, F0,F1,F2,F3,F4,F5)			\
	BRANCHF2(BRANCH, V1, F0,F1,F2,F3,F4,F5)			\
	BRANCHF2(BRANCH, V2, F0,F1,F2,F3,F4,F5)			\
	BRANCHF2(BRANCH, R0, F0,F1,F2,F3,F4,F5)			\
	BRANCHF2(BRANCH, R1, F0,F1,F2,F3,F4,F5)			\
	BRANCHF2(BRANCH, R2, F0,F1,F2,F3,F4,F5)

/*--------------------------------------------------------------------*/
#if BRANCH_DIRECT
#  define BRANCHI1(BRANCH, N, R0, R1, R2)			\
	TESTI(BRANCH##R, N, R0, R1, R2)				\
	TESTI(BRANCH##I, N, R0, R1, R2)
#else
#  define BRANCHI1(BRANCH, N, R0, R1, R2)			\
	TESTI(BRANCH##R, N, R0, R1, R2)				\
	TESTI(BRANCH##R, N##0, R0, R1, R0)			\
	TESTI(BRANCH##R, N##1, R0, R1, R1)			\
	TESTI(BRANCH##I, N, R0, R1, R2)				\
	TESTI(BRANCH##I, N##0, R0, R0, R1)
#endif

#define BRANCHI2(BRANCH, R0, R1, R2)				\
	BRANCHI1(BRANCH, R0##R1##R2, R0, R1, R2)		\
	BRANCHI1(BRANCH, R0##R2##R1, R0, R2, R1)

#define BRANCHI3(BRANCH, R0, R1, R2)				\
	BRANCHI2(BRANCH, R0, R1, R2)				\
	BRANCHI2(BRANCH, R1, R0, R2)				\
	BRANCHI2(BRANCH, R2, R1, R0)

#define BRANCHI( BRANCH, V0, V1, V2, R0, R1, R2)		\
	BRANCHI3(BRANCH, V0, V1, V2)				\
	BRANCHI3(BRANCH, V0, V1, R0)				\
	BRANCHI3(BRANCH, V0, V1, R1)				\
	BRANCHI3(BRANCH, V0, V1, R2)				\
	BRANCHI3(BRANCH, V1, R0, R1)				\
	BRANCHI3(BRANCH, V1, R0, R2)				\
	BRANCHI3(BRANCH, V1, R1, R2)				\
	BRANCHI3(BRANCH, V2, R0, R1)				\
	BRANCHI3(BRANCH, V2, R0, R2)				\
	BRANCHI3(BRANCH, R0, R1, R2)

/*====================================================================*/
.data	1024
ltr:
.c	"ltr: %lx < %lx\n"
lti:
.c	"lti: %lx < %lx\n"
ler:
.c	"ler: %lx <= %lx\n"
lei:
.c	"lei: %lx <= %lx\n"
eqr:
.c	"eqr: %lx == %lx\n"
eqi:
.c	"eqi: %lx == %lx\n"
ger:
.c	"ger: %lx >= %lx\n"
gei:
.c	"gei: %lx >= %lx\n"
gtr:
.c	"gtr: %lx > %lx\n"
gti:
.c	"gti: %lx > %lx\n"
ner:
.c	"ner: %lx != %lx\n"
nei:
.c	"nei: %lx != %lx\n"
msr:
.c	"msr: %lx & %lx\n"
msi:
.c	"msi: %lx & %lx\n"
mcr:
.c	"mcr: !(%lx & %lx)\n"
mci:
.c	"mci: !(%lx & %lx)\n"
oaddr:
.c	"oaddr: %lx + %lx = %lx\n"
oaddi:
.c	"oaddi: %lx + %lx = %lx\n"
osubr:
.c	"osubr: %lx - %lx = %lx\n"
osubi:
.c	"osubi: %lx - %lx = %lx\n"

lt:
.c	"lt: %f < %f\n"
le:
.c	"le: %f <= %f\n"
eq:
.c	"eq: %f == %f\n"
ge:
.c	"ge: %f >= %f\n"
gt:
.c	"gt: %f > %f\n"
ne:
.c	"ne: %f != %f\n"
unlt:
.c	"unlt: !(%f >= %f)\n"
unle:
.c	"unle: !(%f > %f)\n"
uneq:
.c	"uneq: !(%f < %f) && !(%f > %f)\n"
unge:
.c	"unge: !(%f < %f)\n"
ungt:
.c	"ungt: !(%f <= %f)\n"
ltgt:
.c	"ltgt: !(%f >= %f) || !(%f <= %f)\n"
ord:
.c	"ord: (%f == %f) && (%f == %f)\n"
unord:
.c	"unord: (%f != %f) || (%f != %f)\n"
.	$($NaN  =  0.0 / 0.0)
.	$($pInf =  1.0 / 0.0)
.	$($nInf = -1.0 / 0.0)

/*====================================================================*/
.code	$(24 * 1024 * 1024)
	prolog 0

#define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_f(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_I_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_L_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_f(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_f(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_f(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_L_f(N##4, L81, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_t(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##4, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_f(N##7, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_t(N##7, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##8, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_t(N##8, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##9, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##9, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_f(N##10, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_f(N##11, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##11, L80, L81, R0, R1, R2)	\
	BRANCH##_P_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_P_t(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_P_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_P_t(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_P_t(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_P_f(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_P_f(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_P_f(N##4, L81, L80, R0, R1, R2)	\
	BRANCH##_P_t(N##4, L80, L81, R0, R1, R2)
	BRANCHI(BLT, v0, v1, v2, r0, r1, r2)
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##0, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##2, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##3, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##4, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##4, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##5, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##5, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##6, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##7, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##8, $NaN, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##9, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##10, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BLT, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTI
#define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_f(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_I_t(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##4, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##5, I81, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_L_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_f(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_f(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_t(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_L_f(N##3, L81, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_t(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_t(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_t(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_t(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_t(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_f(N##3, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_P_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_P_t(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_P_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_P_t(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_P_t(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_P_f(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_P_t(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_P_f(N##3, L81, L80, R0, R1, R2)	\
	BRANCH##_P_t(N##4, L80, L81, R0, R1, R2)
	BRANCHI(BLE, v0, v1, v2, r0, r1, r2)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##0, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##2, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##3, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##4, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##4, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##5, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##5, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##6, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##7, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##8, $NaN, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##9, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BLE, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTI
#define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##2, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##3, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##4, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_f(N##5, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##5, I81, I81, R0, R1, R2)	\
	BRANCH##_I_t(N##6, 0, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##6, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##7, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##8, I81, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##9, I80, I81, R0, R1, R2)	\
	BRANCH##_L_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_f(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_t(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_L_f(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##7, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##3, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##4, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_f(N##5, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##5, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_t(N##6, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##7, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##8, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##9, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_f(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_t(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_f(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##7, L80, L81, R0, R1, R2)	\
	BRANCH##_P_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_P_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_P_f(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_P_f(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_P_f(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_P_f(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_P_t(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_P_f(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_P_f(N##7, L80, L81, R0, R1, R2)
	BRANCHI(BEQ, v0, v1, v2, r0, r1, r2)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##3, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##4, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##5, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##6, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##7, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##8, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##9, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##10, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##11, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##12, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##13, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##13, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BEQ, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTI
#define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_t(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_I_t(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_L_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_t(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_t(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_t(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_L_t(N##4, L81, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_f(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_t(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##4, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_f(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_f(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_t(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_t(N##4, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_P_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_P_f(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_P_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_P_f(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_P_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_P_t(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_P_t(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_P_t(N##4, L81, L80, R0, R1, R2)	\
	BRANCH##_P_f(N##4, L80, L81, R0, R1, R2)
	BRANCHI(BGE, v0, v1, v2, r0, r1, r2)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##0, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##2, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##3, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##3, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##4, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##4, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##5, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##5, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##6, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##7, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##8, $NaN, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##9, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BGE, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTI
#define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_t(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_I_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##4, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##5, I81, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_L_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_t(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_t(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_f(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_L_t(N##3, L81, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_f(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_f(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_f(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_f(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_t(N##3, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##4, L80, L81, R0, R1, R2)	\
	BRANCH##_P_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_P_f(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_P_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_P_f(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_P_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_P_t(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_P_f(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_P_t(N##3, L81, L80, R0, R1, R2)	\
	BRANCH##_P_f(N##4, L80, L81, R0, R1, R2)
	BRANCHI(BGT, v0, v1, v2, r0, r1, r2)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##0, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##2, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##3, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##3, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##4, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##4, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##5, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##5, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##6, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##7, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##8, $NaN, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##9, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##10, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BGT, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTI
#define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##2, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##3, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##4, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_t(N##5, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##5, I81, I81, R0, R1, R2)	\
	BRANCH##_I_f(N##6, 0, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##6, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##7, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##8, I81, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##9, I80, I81, R0, R1, R2)	\
	BRANCH##_L_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_t(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_t(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_f(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_L_t(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##7, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##2, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##3, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##4, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_t(N##5, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##5, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_f(N##6, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##6, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##7, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##8, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##9, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_t(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_t(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_f(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_t(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##7, L80, L81, R0, R1, R2)	\
	BRANCH##_P_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_P_t(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_P_t(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_P_t(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_P_t(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_P_t(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_P_f(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_P_t(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_P_t(N##7, L80, L81, R0, R1, R2)
	BRANCHI(BNE, v0, v1, v2, r0, r1, r2)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##3, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##4, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##5, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##6, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##7, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##8, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##9, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##10, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##11, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##12, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##13, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##14, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BNE, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##0, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##2, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##3, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##4, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##4, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##5, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##5, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##6, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##7, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##8, $NaN, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##9, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##10, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BUNLT, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##0, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##2, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##3, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##4, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##4, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##5, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##5, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##6, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##7, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##8, $NaN, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##9, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BUNLE, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##2, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##3, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##4, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##5, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##6, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##7, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##8, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##9, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##10, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##11, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##12, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##13, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##14, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##15, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BUNEQ, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##2, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##4, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##5, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##6, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##7, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##8, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##9, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##11, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##12, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##13, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##14, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##15, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BUNGE, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##4, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##5, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##6, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##7, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##8, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##9, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##11, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##12, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##13, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##14, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##15, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BUNGT, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##4, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##5, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##6, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##7, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##8, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##9, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##11, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##12, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##13, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##14, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##15, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BLTGT, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_t(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_t(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##2, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##3, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_t(N##4, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_t(N##5, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##6, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##7, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##8, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##9, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##10, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##11, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##12, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##13, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##14, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_t(N##15, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BORD, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)
#undef TESTF
#define TESTF(BRANCH, N, R0, F0, F1)			\
	BRANCH##_f(N##0, 0.0, 0.5, R0, F0, F1)		\
	BRANCH##_f(N##1, 0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##2, 0.0, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##3, 0.0, -0.5, R0, F0, F1)		\
	BRANCH##_f(N##4, -0.5, 0.0, R0, F0, F1)		\
	BRANCH##_f(N##5, $nInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##6, 0.0, $nInf, R0, F0, F1)	\
	BRANCH##_f(N##7, $pInf, 0.0, R0, F0, F1)	\
	BRANCH##_f(N##8, 0.0, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##9, $nInf, $pInf, R0, F0, F1)	\
	BRANCH##_f(N##10, $pInf, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##11, $NaN, $pInf, R0, F0, F1)	\
	BRANCH##_t(N##12, $NaN, $nInf, R0, F0, F1)	\
	BRANCH##_t(N##13, $NaN, 0.0, R0, F0, F1)	\
	BRANCH##_t(N##14, $NaN, $NaN, R0, F0, F1)	\
	BRANCH##_f(N##15, $pInf, $pInf, R0, F0, F1)
	BRANCHF(BUNORD, v0, v1, v2, r0, r1, r2, f0, f1, f2, f3, f4, f5)

#if BRANCH_DIRECT
#  undef TESTI
#  define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##2, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_f(N##3, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_I_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##6, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##7, I80, I81, R0, R1, R2)	\
	BRANCH##_L_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_t(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_f(N##2, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##3, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##4, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_t(N##4, L81, L81, R0, R1, R2)	\
	BRANCH##_L_t(N##5, L81, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##6, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##2, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_f(N##3, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##7, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_t(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_f(N##2, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##3, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##4, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_t(N##4, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_t(N##5, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##6, L80, L81, R0, R1, R2)
	BRANCHI(BMS, v0, v1, v2, r0, r1, r2)
#  undef TESTI
#  define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_t(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##2, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_t(N##3, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_I_t(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##6, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##7, I80, I81, R0, R1, R2)	\
	BRANCH##_L_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_t(N##2, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##3, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##4, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_f(N##4, L81, L81, R0, R1, R2)	\
	BRANCH##_L_f(N##5, L81, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##6, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_t(N##3, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_t(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_t(N##6, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##7, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_t(N##2, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##3, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_t(N##4, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_f(N##4, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_f(N##5, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##6, L80, L81, R0, R1, R2)
	BRANCHI(BMC, v0, v1, v2, r0, r1, r2)
#  undef TESTI
#  define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##2, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##3, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##4, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_f(N##5, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##5, I81, I81, R0, R1, R2)	\
	BRANCH##_I_f(N##6, 0, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##7, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##8, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_t(N##8, I81, I80, R0, R1, R2)	\
	BRANCH##_I_t(N##9, I80, I81, R0, R1, R2)	\
	BRANCH##_L_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##2, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_f(N##3, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##4, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##5, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_t(N##5, L81, L81, R0, R1, R2)	\
	BRANCH##_L_t(N##6, L81, L80, R0, R1, R2)	\
	BRANCH##_L_t(N##7, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##2, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_f(N##3, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##5, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##7, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_t(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_t(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_f(N##2, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_f(N##3, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##4, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_t(N##4, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_t(N##5, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##6, L80, L81, R0, R1, R2)
	BRANCHI(BOADD, v0, v1, v2, r0, r1, r2)
#  undef TESTI
#  define TESTI(BRANCH, N, R0, R1, R2)			\
	BRANCH##_I_f(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##1, 1, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##2, Iff, 1, R0, R1, R2)		\
	BRANCH##_I_f(N##3, 1, Iff, R0, R1, R2)		\
	BRANCH##_I_t(N##3, I80, I7f, R0, R1, R2)	\
	BRANCH##_I_t(N##4, I7f, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##4, I81, I81, R0, R1, R2)	\
	BRANCH##_I_f(N##5, 0, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##6, 0, Iff, R0, R1, R2)		\
	BRANCH##_I_f(N##7, Iff, 0, R0, R1, R2)		\
	BRANCH##_I_f(N##8, I81, I80, R0, R1, R2)	\
	BRANCH##_I_f(N##9, I80, I81, R0, R1, R2)	\
	BRANCH##_L_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_L_f(N##1, 1, Lff, R0, R1, R2)		\
	BRANCH##_L_t(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_L_t(N##2, L7f, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_L_f(N##3, Lff, 0, R0, R1, R2)		\
	BRANCH##_L_f(N##4, L81, L81, R0, R1, R2)	\
	BRANCH##_L_f(N##5, L81, L80, R0, R1, R2)	\
	BRANCH##_L_f(N##6, L80, L81, R0, R1, R2)	\
	BRANCH##_UI_t(N##0, 0, 1, R0, R1, R2)		\
	BRANCH##_UI_f(N##0, 1, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##1, Iff, 1, R0, R1, R2)		\
	BRANCH##_UI_t(N##1, 1, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##2, I80, I7f, R0, R1, R2)	\
	BRANCH##_UI_t(N##2, I7f, I80, R0, R1, R2)	\
	BRANCH##_UI_f(N##3, I81, I81, R0, R1, R2)	\
	BRANCH##_UI_f(N##4, 0, 0, R0, R1, R2)		\
	BRANCH##_UI_t(N##4, 0, Iff, R0, R1, R2)		\
	BRANCH##_UI_f(N##5, Iff, 0, R0, R1, R2)		\
	BRANCH##_UI_f(N##6, I81, I80, R0, R1, R2)	\
	BRANCH##_UI_t(N##6, I80, I81, R0, R1, R2)	\
	BRANCH##_UL_f(N##0, Lff, 1, R0, R1, R2)		\
	BRANCH##_UL_t(N##0, 1, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##1, L80, L7f, R0, R1, R2)	\
	BRANCH##_UL_t(N##1, L7f, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##2, 0, Lff, R0, R1, R2)		\
	BRANCH##_UL_f(N##2, Lff, 0, R0, R1, R2)		\
	BRANCH##_UL_f(N##3, L81, L81, R0, R1, R2)	\
	BRANCH##_UL_f(N##4, L81, L80, R0, R1, R2)	\
	BRANCH##_UL_t(N##4, L80, L81, R0, R1, R2)
	BRANCHI(BOSUB, v0, v1, v2, r0, r1, r2)
#endif

	ret
