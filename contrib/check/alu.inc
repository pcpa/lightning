.data	1024
strand:
.c	"&"
stror:
.c	"|"
strxor:
.c	"^"
stradd:
.c	"+"
strsub:
strneg:
.c	"-"
strmul:
.c	"*"
strdiv:
.c	"/"
strmod:
.c	"%"
strlsh:
.c	"<<"
strrsh:
.c	">>"
strrsb:
.c	"<rsb>"
strhmul:
.c	"<hmul>"
strnot:
.c	"~"
strabs:
.c	"abs"
strsqrt:
.c	"sqrt"
strsin:
.c	"sin"
strcos:
.c	"cos"
strtan:
.c	"tan"
fmti:
.c	" i: %lx -> %lx %s %lx = %lx\n"
fmtui:
.c	"ui: %lx -> %lx %s %lx = %lx\n"
fmtl:
.c	" l: %lx -> %lx %s %lx = %lx\n"
fmtul:
.c	"ul: %lx -> %lx %s %lx = %lx\n"
fmtp:
.c	" p: %p -> %p %s %p = %p\n"
fmtxi:
.c	" i: %lx -> %lx %s %lx -> %lx\n"
fmtxui:
.c	"ui: %lx -> %lx %s %lx -> %lx\n"
fmtxl:
.c	" l: %lx -> %lx %s %lx -> %lx\n"
fmtxul:
.c	"ul: %lx -> %lx %s %lx -> %lx\n"
ufmti:
.c	" i: %lx -> %s %lx = %lx\n"
ufmtui:
.c	"ui: %lx -> %s %lx = %lx\n"
ufmtl:
.c	" l: %lx -> %s %lx = %lx\n"
ufmtul:
.c	"ul: %lx -> %s %lx = %lx\n"
/* share some float operations */
fmtf:
.c	" f: %f -> %f %s %f = %f\n"
fmtd:
.c	" d: %f -> %f %s %f = %f\n"
ufmtf:
.c	" f: %f -> %s %f = %f\n"
ufmtd:
.c	" d: %f -> %s %f = %f\n"

.	$( $NaN =  0.0/0.0)
.	$( $Inf =  1.0/0.0)
.	$($nInf = -1.0/0.0)

#if DEBUG_REGISTERS
#define DEFREG(reg)	str##reg:	.c	#reg
DEFREG(r0)
DEFREG(r1)
DEFREG(r2)
DEFREG(v0)
DEFREG(v1)
DEFREG(v2)
DEFREG(f0)
DEFREG(f1)
DEFREG(f2)
DEFREG(f3)
DEFREG(f4)
DEFREG(f5)
fmtri:
.c	"%s %s %s\n"
ufmtri:
.c	"%s %s\n"
fmtrf:
.c	"%s %s %s %s\n"
ufmtrf:
.c	"%s %s %s\n"

#  define EXTRAI(R0, R1, R2)		\
	movi_p %R0 str##R0		\
	movi_p %R1 str##R1		\
	movi_p %R2 str##R2		\
	prepare 4			\
		pusharg_p %R2		\
		pusharg_p %R1		\
		pusharg_p %R0		\
		movi_p %R0 fmtri	\
		pusharg_p %R0		\
	finish @printf
#  define EXTRAF(R0, F0, F1, F2, F3)	\
	prepare 5			\
		movi_p %R0 str##F3	\
		pusharg_p %R0		\
		movi_p %R0 str##F2	\
		pusharg_p %R0		\
		movi_p %R0 str##F1	\
		pusharg_p %R0		\
		movi_p %R0 str##F0	\
		pusharg_p %R0		\
		movi_p %R0 fmtrf	\
		pusharg_p %R0		\
	finish @printf
#  define EXTRAUI(R0, R1)		\
	movi_p %R0 str##R0		\
	movi_p %R1 str##R1		\
	prepare 3			\
		pusharg_p %R1		\
		pusharg_p %R0		\
		movi_p %R0 ufmtri	\
		pusharg_p %R0		\
	finish @printf
#  define EXTRAUF(R0, F0, F1, F2)	\
	prepare 4			\
		movi_p %R0 str##F2	\
		pusharg_p %R0		\
		movi_p %R0 str##F1	\
		pusharg_p %R0		\
		movi_p %R0 str##F0	\
		pusharg_p %R0		\
		movi_p %R0 ufmtrf	\
		pusharg_p %R0		\
	finish @printf
#else
#  define EXTRAI(R0, R1, R2)		/**/
#  define EXTRAF(R0, F0, F1, F2, F3)	/**/
#  define EXTRAUI(R0, R1)		/**/
#  define EXTRAUF(R0, F0, F1, F2)	/**/
#endif

/*====================================================================*/
#define ERRORI(I, OP, R0, R1, R2, V)	\
	prepare 6			\
		pusharg_##I %R2		\
		pusharg_##I %R1		\
		movi_p %R1 str##OP	\
		pusharg_p %R1		\
		pusharg_##I %R0		\
		movi_##I %R0 V		\
		pusharg_##I %R0		\
		movi_p %R0 fmt##I	\
		pusharg_p %R0		\
	finish @printf			\
	EXTRAI(R0, R1, R2)

#define  ERRORi(OP, R0, R1, R2, V)	ERRORI(i,  OP, R0, R1, R2, V)
#define ERRORui(OP, R0, R1, R2, V)	ERRORI(ui, OP, R0, R1, R2, V)
#define  ERRORl(OP, R0, R1, R2, V)	ERRORI(l,  OP, R0, R1, R2, V)
#define ERRORul(OP, R0, R1, R2, V)	ERRORI(ul, OP, R0, R1, R2, V)
#define  ERRORp(OP, R0, R1, R2, V)	ERRORI(p,  OP, R0, R1, R2, V)

/* reg2 = reg0 op reg1 */
#define ALUR(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R1 I1					\
	OP##r_##T %R2 %R0 %R1				\
	beqi_##T OP##T##N##r_##R0##R1##R2 %R2 V		\
	ERROR##T(OP, R0, R1, R2, V)			\
OP##T##N##r_##R0##R1##R2:

/* reg2 = reg0 op im */
#define ALUI(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	OP##i_##T %R2 %R0 I1				\
	beqi_##T OP##T##N##i_##R0##R1##R2 %R2 V		\
	movi_##T %R1 I1					\
	ERROR##T(OP, R0, R1, R2, V)			\
OP##T##N##i_##R0##R1##R2:

/* reg0 = reg0 op reg1 */
#define ALURC0(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R1 I1					\
	movr_##T %R2 %R0				\
	OP##r_##T %R0 %R0 %R1				\
	bnei_##T OP##T##N##rc0_0##R0##R1##R2 %R0 V	\
	beqi_##T OP##T##N##rc0_1##R0##R1##R2 %R2 I0	\
OP##T##N##rc0_0##R0##R1##R2:				\
	ERROR##T(OP, R2, R1, R0, V)			\
OP##T##N##rc0_1##R0##R1##R2:

/* reg1 = reg0 op reg1 */
#define ALURC1(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R1 I1					\
	movr_##T %R2 %R1				\
	OP##r_##T %R1 %R0 %R1				\
	bnei_##T OP##T##N##rc1_0##R0##R1##R2 %R1 V	\
	beqi_##T OP##T##N##rc1_1##R0##R1##R2 %R2 I1	\
OP##T##N##rc1_0##R0##R1##R2:				\
	ERROR##T(OP, R0, R2, R1, V)			\
OP##T##N##rc1_1##R0##R1##R2:

/* reg0 = reg0 op reg0 */
#define ALURC2(N, T, OP, I0, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movr_##T %R1 %R0				\
	movr_##T %R2 %R1				\
	OP##r_##T %R0 %R0 %R0				\
	bnei_##T OP##T##N##rc2_0##R0##R1##R2 %R0 V	\
	beqr_##T OP##T##N##rc2_1##R0##R1##R2 %R1 %R2	\
OP##T##N##rc2_0##R0##R1##R2:				\
	ERROR##T(OP, R2, R1, R0, V)			\
OP##T##N##rc2_1##R0##R1##R2:

/* reg0 = reg0 op im */
#define ALUIC(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movr_##T %R2 %R0				\
	OP##i_##T %R0 %R0 I1				\
	beqi_##T OP##T##N##ic_##R0##R1##R2 %R0 V	\
	movi_##T %R1 I1					\
	ERROR##T(OP, R2, R1, R0, V)			\
OP##T##N##ic_##R0##R1##R2:

#define   ALU1(N, T, OP, I0, I1, V, R0, R1, R2)		\
	  ALUR(N, T, OP, I0, I1, V, R0, R1, R2)		\
	ALURC0(N, T, OP, I0, I1, V, R0, R1, R2)		\
	ALURC1(N, T, OP, I0, I1, V, R0, R1, R2)		\
	  ALUI(N, T, OP, I0, I1, V, R0, R1, R2)		\
	 ALUIC(N, T, OP, I0, I1, V, R0, R1, R2)

#define ALU2(N, T, OP, I0, I1, V, R0, R1, R2)		\
	ALU1(N, T, OP, I0, I1, V, R0, R1, R2)		\
	ALU1(N, T, OP, I0, I1, V, R0, R2, R1)

#define ALU3(N, T, OP, I0, I1, V, R0, R1, R2)		\
	ALU2(N, T, OP, I0, I1, V, R0, R1, R2)		\
	ALU2(N, T, OP, I0, I1, V, R1, R0, R2)		\
	ALU2(N, T, OP, I0, I1, V, R2, R1, R0)

#define  ALU(N, T, OP, I0, I1, V)			\
	ALU3(N, T, OP, I0, I1, V, v0, v1, v2)		\
	ALU3(N, T, OP, I0, I1, V, v0, v1, r0)		\
	ALU3(N, T, OP, I0, I1, V, v0, v1, r1)		\
	ALU3(N, T, OP, I0, I1, V, v0, v1, r2)		\
	ALU3(N, T, OP, I0, I1, V, v1, v2, r0)		\
	ALU3(N, T, OP, I0, I1, V, v1, v2, r1)		\
	ALU3(N, T, OP, I0, I1, V, v1, v2, r2)		\
	ALU3(N, T, OP, I0, I1, V, v2, r0, r1)		\
	ALU3(N, T, OP, I0, I1, V, v2, r0, r2)		\
	ALU3(N, T, OP, I0, I1, V, r0, r1, r2)

/*====================================================================*/
#define  ALUC1(N, T, OP, I0, V, R0, R1, R2)		\
	ALURC2(N, T, OP, I0, V, R0, R1, R2)		\
	ALURC2(N, T, OP, I0, V, R0, R2, R1)

#define ALUC2(N, T, OP, I0, V, R0, R1, R2)		\
	ALUC1(N, T, OP, I0, V, R0, R1, R2)		\
	ALUC1(N, T, OP, I0, V, R1, R0, R2)		\
	ALUC1(N, T, OP, I0, V, R2, R1, R0)

#define  ALUC(N, T, OP, I0, V)				\
	ALUC2(N, T, OP, I0, V, v0, v1, v2)		\
	ALUC2(N, T, OP, I0, V, v0, v1, r0)		\
	ALUC2(N, T, OP, I0, V, v0, v1, r1)		\
	ALUC2(N, T, OP, I0, V, v0, v1, r2)		\
	ALUC2(N, T, OP, I0, V, v1, v2, r0)		\
	ALUC2(N, T, OP, I0, V, v1, v2, r1)		\
	ALUC2(N, T, OP, I0, V, v1, v2, r2)		\
	ALUC2(N, T, OP, I0, V, v2, r0, r1)		\
	ALUC2(N, T, OP, I0, V, v2, r0, r2)		\
	ALUC2(N, T, OP, I0, V, r0, r1, r2)

/*====================================================================*/
#define ERRORF(R0, OP, F0, F1, F2, F3)	\
	prepare 2			\
	prepare_d 4			\
		extr_d_f %F2 %F2	\
		pusharg_d %F2		\
		extr_d_f %F1 %F1	\
		pusharg_d %F1		\
		movi_p %R0 str##OP	\
		pusharg_p %R0		\
		extr_d_f %F0 %F0	\
		pusharg_d %F0		\
		extr_d_f %F3 %F3	\
		pusharg_d %F3		\
		movi_p %R0 fmtf		\
		pusharg_p %R0		\
	finish @printf			\
	EXTRAF(R0, F0, F1, F2, F3)

#define  ERRORf(OP, F0, F1, F2, F3)	ERRORF(r0, OP, F0, F1, F2, F3)
#define  ERRORd(OP, F0, F1, F2, F3)	ERRORD(r0, OP, F0, F1, F2, F3)

#define ERRORD(R0, OP, F0, F1, F2, F3)	\
	prepare 2			\
	prepare_d 4			\
		pusharg_d %F2		\
		pusharg_d %F1		\
		movi_p %R0 str##OP	\
		pusharg_p %R0		\
		pusharg_d %F0		\
		pusharg_d %F3		\
		movi_p %R0 fmtd		\
		pusharg_p %R0		\
	finish @printf			\
	EXTRAF(R0, F0, F1, F2, F3)

/* reg2 = reg0 op reg1 */
#define FOPR(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	movi_##T %F0 I0					\
	movi_##T %F1 I1					\
	movi_##T %F3 V					\
	OP##r_##T %F2 %F0 %F1				\
	beqr_##T OP##T##N##F0##F1##F2##F3 %F2 %F3	\
	ERROR##T(OP, F0, F1, F2, F3)			\
OP##T##N##F0##F1##F2##F3:

/* reg0 = reg0 op reg1 */
#define FOPRC0(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	movi_##T %F0 I0					\
	movi_##T %F1 I1					\
	movr_##T %F2 %F0				\
	OP##r_##T %F0 %F0 %F1				\
	movi_##T %F3 I0					\
	bner_##T OP##T##N##c0_0##F0##F1##F2##F3 %F2 %F3	\
	movi_##T %F3 I1					\
	bner_##T OP##T##N##c0_0##F0##F1##F2##F3 %F1 %F3	\
	movi_##T %F3 V					\
	beqr_##T OP##T##N##c0_1##F0##F1##F2##F3 %F0 %F3	\
OP##T##N##c0_0##F0##F1##F2##F3:				\
	ERROR##T(OP, F2, F1, F0, F3)			\
OP##T##N##c0_1##F0##F1##F2##F3:

/* reg1 = reg0 op reg1 */
#define FOPRC1(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	movi_##T %F0 I0					\
	movi_##T %F1 I1					\
	movr_##T %F2 %F1				\
	OP##r_##T %F1 %F0 %F1				\
	movi_##T %F3 I0					\
	bner_##T OP##T##N##c1_0##F0##F1##F2##F3 %F0 %F3	\
	movi_##T %F3 I1					\
	bner_##T OP##T##N##c1_0##F0##F1##F2##F3 %F2 %F3	\
	movi_##T %F3 V					\
	beqr_##T OP##T##N##c1_1##F0##F1##F2##F3 %F1 %F3	\
OP##T##N##c1_0##F0##F1##F2##F3:				\
	ERROR##T(OP, F0, F2, F1, F3)			\
OP##T##N##c1_1##F0##F1##F2##F3:

#define   FOP1(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	  FOPR(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOPRC0(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOPRC1(N, T, OP, I0, I1, V, F0, F1, F2, F3)

#define FOP2(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOP1(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOP1(N, T, OP, I0, I1, V, F0, F1, F3, F2)

#define FOP3(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOP2(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOP2(N, T, OP, I0, I1, V, F0, F2, F1, F3)	\
	FOP2(N, T, OP, I0, I1, V, F0, F3, F2, F1)

#define FOP4(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOP3(N, T, OP, I0, I1, V, F0, F1, F2, F3)	\
	FOP3(N, T, OP, I0, I1, V, F1, F0, F2, F3)	\
	FOP3(N, T, OP, I0, I1, V, F2, F1, F0, F3)	\
	FOP3(N, T, OP, I0, I1, V, F3, F1, F2, F0)

#define  FOP(N, T, OP, I0, I1, V)			\
	FOP4(N, T, OP, I0, I1, V, f0, f1, f2, f3)	\
	FOP4(N, T, OP, I0, I1, V, f0, f1, f2, f4)	\
	FOP4(N, T, OP, I0, I1, V, f0, f1, f2, f5)	\
	FOP4(N, T, OP, I0, I1, V, f1, f2, f3, f4)	\
	FOP4(N, T, OP, I0, I1, V, f1, f2, f3, f5)	\
	FOP4(N, T, OP, I0, I1, V, f2, f3, f4, f5)

/*====================================================================*/
#define ERRORX(I, OP, R0, R1, R2, I0, I1, V)	\
	prepare 6				\
		pusharg_##I %R2			\
		movi_p %R1 I1			\
		pusharg_##I %R1			\
		movi_p %R1 str##OP		\
		pusharg_p %R1			\
		movi_p %R0 I0			\
		pusharg_##I %R0			\
		movi_##I %R0 V			\
		pusharg_##I %R0			\
		movi_p %R0 fmtx##I		\
		pusharg_p %R0			\
	finish @printf				\
	EXTRAI(R0, R1, R2)

#define  ERRORXi(OP, R0,R1,R2, I0,I1,V)	ERRORX(i,  OP, R0, R1, R2, I0, I1, V)
#define ERRORXui(OP, R0,R1,R2, I0,I1,V)	ERRORX(ui, OP, R0, R1, R2, I0, I1, V)
#define  ERRORXl(OP, R0,R1,R2, I0,I1,V)	ERRORX(l,  OP, R0, R1, R2, I0, I1, V)
#define ERRORXul(OP, R0,R1,R2, I0,I1,V)	ERRORX(ul, OP, R0, R1, R2, I0, I1, V)

/*
 * r0 = i0
 * r1 = i1
 * r2 = 0
 * r0 = r0 opc r1	<only want carry>
 * r2 = r2 opx r2	<r2 must match v>
 */
#define ALUXII(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R2 0					\
	OP##ci_##T %R0 %R0 I1				\
	OP##xi_##T %R2 %R2 0				\
	beqi_##T OP##T##N##ii##R0##R1##R2 %R2 V		\
	ERRORX##T(OP, R0, R1, R2, I0, I1, V)		\
OP##T##N##ii##R0##R1##R2:

#define ALUXIR(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R2 0					\
	OP##ci_##T %R0 %R0 I1				\
	OP##xr_##T %R2 %R2 %R2				\
	beqi_##T OP##T##N##ir##R0##R1##R2 %R2 V		\
	ERRORX##T(OP, R0, R1, R2, I0, I1, V)		\
OP##T##N##ir##R0##R1##R2:

#define ALUXRI(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R1 I1					\
	movi_##T %R2 0					\
	OP##cr_##T %R0 %R0 %R1				\
	OP##xi_##T %R2 %R2 0				\
	beqi_##T OP##T##N##ri##R0##R1##R2 %R2 V		\
	ERRORX##T(OP, R0, R1, R2, I0, I1, V)		\
OP##T##N##ri##R0##R1##R2:

#define ALUXRR(N, T, OP, I0, I1, V, R0, R1, R2)		\
	movi_##T %R0 I0					\
	movi_##T %R1 I1					\
	movi_##T %R2 0					\
	OP##cr_##T %R0 %R0 %R1				\
	OP##xr_##T %R2 %R2 %R2				\
	beqi_##T OP##T##N##rr##R0##R1##R2 %R2 V		\
	ERRORX##T(OP, R0, R1, R2, I0, I1, V)		\
OP##T##N##rr##R0##R1##R2:

#define ALUXRR1(N, T, OP, I0, I1, V, R0, R1, R2)	\
	 ALUXII(N, T, OP, I0, I1, V, R0, R1, R2)	\
	 ALUXIR(N, T, OP, I0, I1, V, R0, R1, R2)	\
	 ALUXRI(N, T, OP, I0, I1, V, R0, R1, R2)	\
	 ALUXRR(N, T, OP, I0, I1, V, R0, R1, R2)

#define ALUXRR2(N, T, OP, I0, I1, V, R0, R1, R2)	\
	ALUXRR1(N, T, OP, I0, I1, V, R0, R1, R2)	\
	ALUXRR1(N, T, OP, I0, I1, V, R0, R2, R1)

#define ALUXRR3(N, T, OP, I0, I1, V, R0, R1, R2)	\
	ALUXRR2(N, T, OP, I0, I1, V, R0, R1, R2)	\
	ALUXRR2(N, T, OP, I0, I1, V, R1, R0, R2)	\
	ALUXRR2(N, T, OP, I0, I1, V, R2, R1, R0)

#define    ALUX(N, T, OP, I0, I1, V)			\
	ALUXRR3(N, T, OP, I0, I1, V, v0, v1, v2)	\
	ALUXRR3(N, T, OP, I0, I1, V, v0, v1, r0)	\
	ALUXRR3(N, T, OP, I0, I1, V, v0, v1, r1)	\
	ALUXRR3(N, T, OP, I0, I1, V, v0, v1, r2)	\
	ALUXRR3(N, T, OP, I0, I1, V, v1, v2, r0)	\
	ALUXRR3(N, T, OP, I0, I1, V, v1, v2, r1)	\
	ALUXRR3(N, T, OP, I0, I1, V, v1, v2, r2)	\
	ALUXRR3(N, T, OP, I0, I1, V, v2, r0, r1)	\
	ALUXRR3(N, T, OP, I0, I1, V, v2, r0, r2)	\
	ALUXRR3(N, T, OP, I0, I1, V, r0, r1, r2)

/* Test operations only defined for signed/unsigned? */
#if TEST_ILLOGICAL
#  define IALUX(N, T, OP, I0, I1, V)			\
	ALUX(N, T, OP, I0, I1, V)
#  define UALUX(N, T, OP, I0, I1, V)			\
	ALUX(N, T, OP, I0, I1, V)
#else
#  define IALUX(N, T, OP, I0, I1, V)			/**/
#  define UALUX(N, T, OP, I0, I1, V)			/**/
#endif

/*====================================================================*/
#define ERRORUI(I, OP, R0, R1, V)	\
	prepare 5			\
		pusharg_##I %R0		\
		pusharg_##I %R1		\
		movi_p %R0 str##OP	\
		pusharg_p %R0		\
		movi_##I %R0 V		\
		pusharg_##I %R0		\
		movi_p %R0 ufmt##I	\
		pusharg_p %R0		\
	finish @printf			\
	EXTRAUI(R0, R1)

#define  ERRORUi(OP, R0, R1, V)		ERRORUI(i,  OP, R0, R1, V)
#define ERRORUui(OP, R0, R1, V)		ERRORUI(ui, OP, R0, R1, V)
#define  ERRORUl(OP, R0, R1, V)		ERRORUI(l,  OP, R0, R1, V)
#define ERRORUul(OP, R0, R1, V)		ERRORUI(ul, OP, R0, R1, V)

#define ALUUR(N, T, OP, I, V, R0, R1)			\
	movi_##T %R1 I					\
	OP##r_##T %R0 %R1				\
	beqi_##T OP##T##N##R0##R1 %R0 V			\
	ERRORU##T(OP, R0, R1, V)			\
OP##T##N##R0##R1:

#define ALUURC(N, T, OP, I, V, R0, R1)			\
	movi_##T %R0 I					\
	OP##r_##T %R0 %R0				\
	beqi_##T OP##T##N##c##R0##R1 %R0 V		\
	movi_##T %R1 I					\
	ERRORU##T(OP, R0, R1, V)			\
OP##T##N##c##R0##R1:

#define ALUU1(N, T, OP, I, V, R0, R1)			\
	ALUUR(N, T, OP, I, V, R0, R1)			\
	ALUURC(N, T, OP, I, V, R0, R1)

#define ALUU2(N, T, OP, I, V, R0, R1)			\
	ALUU1(N, T, OP, I, V, R0, R1)			\
	ALUU1(N, T, OP, I, V, R1, R0)

#define  ALUU(N, T, OP, I, V)				\
	ALUU2(N, T, OP, I, V, v0, v1)			\
	ALUU2(N, T, OP, I, V, v0, v2)			\
	ALUU2(N, T, OP, I, V, v0, r0)			\
	ALUU2(N, T, OP, I, V, v0, r1)			\
	ALUU2(N, T, OP, I, V, v0, r2)			\
	ALUU2(N, T, OP, I, V, v1, v2)			\
	ALUU2(N, T, OP, I, V, v1, r0)			\
	ALUU2(N, T, OP, I, V, v1, r1)			\
	ALUU2(N, T, OP, I, V, v1, r2)			\
	ALUU2(N, T, OP, I, V, v2, r0)			\
	ALUU2(N, T, OP, I, V, v2, r1)			\
	ALUU2(N, T, OP, I, V, v2, r2)			\
	ALUU2(N, T, OP, I, V, r0, r1)			\
	ALUU2(N, T, OP, I, V, r0, r2)			\
	ALUU2(N, T, OP, I, V, r1, r2)

/*====================================================================*/
#define ERRORUF(R0, OP, F0, F1, F2)	\
	prepare 2			\
	prepare_d 3			\
		extr_d_f %F0 %F0	\
		pusharg_d %F0		\
		extr_d_f %F1 %F1	\
		pusharg_d %F1		\
		movi_p %R0 str##OP	\
		pusharg_p %R0		\
		extr_d_f %F2 %F2	\
		pusharg_d %F2		\
		movi_p %R0 ufmtf	\
		pusharg_p %R0		\
	finish @printf			\
	EXTRAUF(R0, F0, F1, F2)

#define ERRORUD(R0, OP, F0, F1, F2)	\
	prepare 2			\
	prepare_d 3			\
		pusharg_d %F0		\
		pusharg_d %F1		\
		movi_p %R0 str##OP	\
		pusharg_p %R0		\
		pusharg_d %F2		\
		movi_p %R0 ufmtd	\
		pusharg_p %R0		\
	finish @printf			\
	EXTRAUF(R0, F0, F1, F2)

#define  ERRORUf(OP, F0, F1, F2)	ERRORUF(r0,  OP, F0, F1, F2)
#define  ERRORUd(OP, F0, F1, F2)	ERRORUD(r0,  OP, F0, F1, F2)

#define FOPUR(N, T, OP, I, V, F0, F1, F2)		\
	movi_##T %F2 V					\
	movi_##T %F1 I					\
	OP##r_##T %F0 %F1				\
	beqr_##T OP##T##N##F0##F1##F2 %F0 %F2		\
	ERRORU##T(OP, F0, F1, F2)			\
OP##T##N##F0##F1##F2:

#define FOPURC(N, T, OP, I, V, F0, F1, F2)		\
	movi_##T %F2 V					\
	movi_##T %F0 I					\
	movr_##T %F1 %F0				\
	OP##r_##T %F0 %F0				\
	beqr_##T OP##T##N##c##F0##F1##F2 %F0 %F2	\
	ERRORU##T(OP, F0, F1, F2)			\
OP##T##N##c##F0##F1##F2:

#define  FOPU1(N, T, OP, I, V, F0, F1, F2)		\
	 FOPUR(N, T, OP, I, V, F0, F1, F2)		\
	FOPURC(N, T, OP, I, V, F0, F1, F2)

#define FOPU2(N, T, OP, I, V, F0, F1, F2)		\
	FOPU1(N, T, OP, I, V, F0, F1, F2)		\
	FOPU1(N, T, OP, I, V, F0, F2, F1)

#define FOPU3(N, T, OP, I, V, F0, F1, F2)		\
	FOPU2(N, T, OP, I, V, F0, F1, F2)		\
	FOPU2(N, T, OP, I, V, F1, F0, F2)		\
	FOPU2(N, T, OP, I, V, F2, F1, F0)

#define  FOPU(N, T, OP, I, V)				\
	FOPU3(N, T, OP, I, V, f0, f1, f2)		\
	FOPU3(N, T, OP, I, V, f0, f1, f3)		\
	FOPU3(N, T, OP, I, V, f0, f1, f4)		\
	FOPU3(N, T, OP, I, V, f0, f1, f5)		\
	FOPU3(N, T, OP, I, V, f1, f2, f3)		\
	FOPU3(N, T, OP, I, V, f1, f2, f4)		\
	FOPU3(N, T, OP, I, V, f1, f2, f5)		\
	FOPU3(N, T, OP, I, V, f2, f3, f4)		\
	FOPU3(N, T, OP, I, V, f2, f4, f5)		\
	FOPU3(N, T, OP, I, V, f3, f4, f5)

/*====================================================================*/
#define UFOPUR(N, T, OP, I, V, F0, F1, F2)		\
	movi_##T %F2 V					\
	movi_##T %F1 I					\
	OP##r_##T %F0 %F1				\
	buneqr_##T OP##T##N##u##F0##F1##F2 %F0 %F2	\
	ERRORU##T(OP, F0, F1, F2)			\
OP##T##N##u##F0##F1##F2:

#define UFOPURC(N, T, OP, I, V, F0, F1, F2)		\
	movi_##T %F2 V					\
	movi_##T %F0 I					\
	movr_##T %F1 %F0				\
	OP##r_##T %F0 %F0				\
	buneqr_##T OP##T##N##uc##F0##F1##F2 %F0 %F2	\
	ERRORU##T(OP, F0, F1, F2)			\
OP##T##N##uc##F0##F1##F2:

#define  UFOPU1(N, T, OP, I, V, F0, F1, F2)		\
	 UFOPUR(N, T, OP, I, V, F0, F1, F2)		\
	UFOPURC(N, T, OP, I, V, F0, F1, F2)

#define UFOPU2(N, T, OP, I, V, F0, F1, F2)		\
	UFOPU1(N, T, OP, I, V, F0, F1, F2)		\
	UFOPU1(N, T, OP, I, V, F0, F2, F1)

#define UFOPU3(N, T, OP, I, V, F0, F1, F2)		\
	UFOPU2(N, T, OP, I, V, F0, F1, F2)		\
	UFOPU2(N, T, OP, I, V, F1, F0, F2)		\
	UFOPU2(N, T, OP, I, V, F2, F1, F0)

#define  UFOPU(N, T, OP, I, V)				\
	UFOPU3(N, T, OP, I, V, f0, f1, f2)		\
	UFOPU3(N, T, OP, I, V, f0, f1, f3)		\
	UFOPU3(N, T, OP, I, V, f0, f1, f4)		\
	UFOPU3(N, T, OP, I, V, f0, f1, f5)		\
	UFOPU3(N, T, OP, I, V, f1, f2, f3)		\
	UFOPU3(N, T, OP, I, V, f1, f2, f4)		\
	UFOPU3(N, T, OP, I, V, f1, f2, f5)		\
	UFOPU3(N, T, OP, I, V, f2, f3, f4)		\
	UFOPU3(N, T, OP, I, V, f2, f4, f5)		\
	UFOPU3(N, T, OP, I, V, f3, f4, f5)
