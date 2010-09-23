#include "consts.inc"

/*====================================================================*/
.data	256
cifmt:
.c	"ci: %lx %lx\n"
clfmt:
.c	"cl: %lx %lx\n"
ucifmt:
.c	"u:ci: %lx %lx\n"
uclfmt:
.c	"u:cl: %lx %lx\n"
sifmt:
.c	"si: %lx %lx\n"
slfmt:
.c	"sl: %lx %lx\n"
usifmt:
.c	"u:si: %lx %lx\n"
uslfmt:
.c	"u:sl: %lx %lx\n"
iifmt:
.c	"ii: %lx %lx\n"
ilfmt:
.c	"il: %lx %lx\n"
uiifmt:
.c	"u:ii: %lx %lx\n"
uilfmt:
.c	"u:il: %lx %lx\n"
lifmt:
.c	"li: %lx %lx\n"
llfmt:
.c	"ll: %lx %lx\n"
ulifmt:
.c	"u:li: %lx %lx\n"
ullfmt:
.c	"u:ll: %lx %lx\n"
pifmt:
.c	"pi: %p %p\n"
plfmt:
.c	"pl: %p %p\n"

/*====================================================================*/
.code	$(1024 * 1024)
	prolog 0

#define iMOVI(N, T, V, R0, R1)			\
	movi_i %R0 is##T##V			\
	movi_l %R1 ls##T##V			\
	/* check for same int value  */		\
	beqr_i N##T##i##V %R0 %R1		\
	prepare 3				\
		pusharg_l %R1			\
		pusharg_i %R0			\
		movi_p %R0 T##ifmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##i##V:					\
	/* check for same long value  */	\
	beqr_l N##T##l##V %R0 %R1		\
	prepare 3				\
		pusharg_l %R1			\
		pusharg_i %R0			\
		movi_p %R0 T##lfmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##l##V:

#define uMOVI(N, T, V, R0, R1)			\
	movi_ui %R0 T##v##V			\
	movi_ul %R1 T##v##V			\
	/* check for same uint value  */	\
	beqr_ui N##T##ui##V %R0 %R1		\
	prepare 3				\
		pusharg_ul %R1			\
		pusharg_ui %R0			\
		movi_p %R0 u##T##ifmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##ui##V:					\
	/* check for same ulong value  */	\
	beqr_ul N##T##ul##V %R0 %R1		\
	prepare 3				\
		pusharg_l %R1			\
		pusharg_i %R0			\
		movi_p %R0 u##T##lfmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##ul##V:

#define IMOVI(N, T, V, R0, R1)			\
	movi_##T %R0 T##v##V			\
	movi_##T %R1 T##v##V			\
	/* check for same int value  */		\
	beqr_i N##T##i##V %R0 %R1		\
	prepare 3				\
		pusharg_##T %R1			\
		pusharg_##T %R0			\
		movi_p %R0 T##ifmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##i##V:					\
	/* check for same long value  */	\
	beqr_l N##T##l##V %R0 %R1		\
	prepare 3				\
		pusharg_##T %R1			\
		pusharg_##T %R0			\
		movi_p %R0 T##lfmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##l##V:

#define UMOVI(N, T, V, R0, R1)			\
	movi_u##T %R0 T##v##V			\
	movi_u##T %R1 T##v##V			\
	/* check for same uint value  */	\
	beqr_ui N##T##ui##V %R0 %R1		\
	prepare 3				\
		pusharg_u##T %R1		\
		pusharg_u##T %R0		\
		movi_p %R0 u##T##ifmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##ui##V:					\
	/* check for same ulong value  */	\
	beqr_ul N##T##ul##V %R0 %R1		\
	prepare 3				\
		pusharg_u##T %R1		\
		pusharg_u##T %R0		\
		movi_p %R0 ullfmt		\
		pusharg_p %R0			\
	finish @printf				\
N##T##ul##V:

#define pMOVI(N, V, R0, R1)			\
	movi_p %R0 lv##V			\
	movi_p %R1 lv##V			\
	/* check for same int value  */		\
	beqr_i N##pi##V %R0 %R1			\
	prepare 3				\
		pusharg_p %R1			\
		pusharg_p %R0			\
		movi_p %R0 pifmt		\
		pusharg_p %R0			\
	finish @printf				\
N##pi##V:					\
	/* check for same long value  */	\
	beqr_l N##pl##V %R0 %R1			\
	prepare 3				\
		pusharg_p %R1			\
		pusharg_p %R0			\
		movi_p %R0 plfmt		\
		pusharg_p %R0			\
	finish @printf				\
N##pl##V:

#define MOVI_1(N, R0, R1, V)			\
	iMOVI(N, c, V, R0, R1)			\
	uMOVI(N, c, V, R0, R1)			\
	iMOVI(N, s, V, R0, R1)			\
	uMOVI(N, s, V, R0, R1)			\
	IMOVI(N, i, V, R0, R1)			\
	UMOVI(N, i, V, R0, R1)			\
	IMOVI(N, l, V, R0, R1)			\
	UMOVI(N, l, V, R0, R1)			\
	pMOVI(N,    V, R0, R1)

#define MOVI_2(N, R0, R1)			\
	MOVI_1(N, R0, R1, 0)			\
	MOVI_1(N, R0, R1, 1)			\
	MOVI_1(N, R0, R1, 2)			\
	MOVI_1(N, R0, R1, 3)

#define MOVI(R0, R1)				\
	MOVI_2(R0##R1, R0, R1)			\
	MOVI_2(R1##R0, R1, R0)

	MOVI(v0, v1)
	MOVI(v0, v2)
	MOVI(v0, r0)
	MOVI(v0, r1)
	MOVI(v0, r2)
	MOVI(v1, v2)
	MOVI(v1, r0)
	MOVI(v1, r1)
	MOVI(v1, r2)
	MOVI(v2, r0)
	MOVI(v2, r1)
	MOVI(v2, r2)
	MOVI(r0, r1)
	MOVI(r0, r2)
	MOVI(r1, r2)

	ret

