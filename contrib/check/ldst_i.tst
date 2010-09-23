#include "consts.inc"

.data	512

#define DEFSTRUCT(N)		\
.align	$(__WORDSIZE >> 3)	\
c##N:	.c	0	;	\
uc##N:	.c	0	;	\
s##N:	.s	0	;	\
us##N:	.s	0	;	\
i##N:	.i	0	;	\
ui##N:	.i	0	;	\
.align	$(__WORDSIZE >> 3)	\
l##N:	.l	0	;	\
ul##N:	.l	0	;	\
p##N:	.p	0	;	\
f##N:	.f	0.0	;	\
.align	8			\
d##N:	.d	0.0

DEFSTRUCT(0)
DEFSTRUCT(1)
DEFSTRUCT(2)
DEFSTRUCT(3)
fmtc:
.c " c: %lx != %lx\n"
fmtuc:
.c "uc: %lx != %lx\n"
fmts:
.c " s: %lx != %lx\n"
fmtus:
.c "us: %lx != %lx\n"
fmti:
.c " i: %lx != %lx\n"
fmtui:
.c "ui: %lx != %lx\n"
fmtl:
.c " l: %lx != %lx\n"
fmtul:
.c "ul: %lx != %lx\n"
fmtp:
.c " p: %p != %p\n"
fmtf:
.c " f: %f != %f\n"
fmtd:
.c " d: %d != %d\n"

.code	$(16 * 1024 * 1024)
	prolog 0

#define STI0(N, V, I, ST, T, R)	\
	movi_##I %R ST##v##V	\
	sti_##ST T##N %R

#define STI(N, V, R, F)			\
	STI0(N, V, i,  c, c,  R)	\
	STI0(N, V, ui, c, uc, R)	\
	STI0(N, V, i,  s, s,  R)	\
	STI0(N, V, ui, s, us, R)	\
	STI0(N, V, i,  i, i,  R)	\
	STI0(N, V, ui, i, ui, R)	\
	STI0(N, V, l,  l, l,  R)	\
	STI0(N, V, ul, l, ul, R)	\
	STI0(N, V, p,  p, p,  R)	\
	STI0(N, V, f,  f, f,  F)	\
	STI0(N, V, d,  d, d,  F)

#define iLDI(L, N, V, I, T, R0, R1)	\
	ldi_##T %R0 T##N		\
	movi_##I %R1 I##s##T##V		\
	beqr_##I L %R0 %R1		\
	prepare 3			\
		pusharg_##T %R1		\
		pusharg_##T %R0		\
		movi_p %R0 fmt##T	\
		pusharg_p %R0		\
	finish @printf			\
L:

#define xLDI(L, N, V, I, R0, R1)	\
	ldi_##I %R0 I##N		\
	movi_##I %R1 I##v##V		\
	beqr_##I L %R0 %R1		\
	prepare 3			\
		pusharg_##I %R1		\
		pusharg_##I %R0		\
		movi_p %R0 fmt##I	\
		pusharg_p %R0		\
	finish @printf			\
L:

#define uiLDI(L, N, V, I, T, R0, R1)	\
	ldi_u##T %R0 T##N		\
	movi_u##I %R1 T##v##V		\
	beqr_u##I L %R0 %R1		\
	prepare 3			\
		pusharg_u##T %R1	\
		pusharg_u##T %R0	\
		movi_p %R0 fmtu##T	\
		pusharg_p %R0		\
	finish @printf			\
L:

#define fLDI(L, N, V, R0, F0, F1)	\
	ldi_f %F0 f##N			\
	movi_f %F1 fv##V		\
	beqr_f L %F0 %F1		\
	prepare 1			\
	prepare_d 2			\
		extr_f_d %F1 %F1	\
		pusharg_d %F1		\
		extr_f_d %F0 %F0	\
		pusharg_d %F0		\
		movi_p %R0 fmtf		\
		pusharg_p %R0		\
	finish @printf			\
L:

#define dLDI(L, N, V, R0, F0, F1)	\
	ldi_d %F0 d##N			\
	movi_d %F1 dv##V		\
	beqr_d L %F0 %F1		\
	prepare 1			\
	prepare_d 2			\
		pusharg_d %F1		\
		pusharg_d %F0		\
		movi_p %R0 fmtd		\
		pusharg_p %R0		\
	finish @printf			\
L:

#define iLDI0(L,     N, V,	R0, R1)		\
	 iLDI(L##c,  N, V, i, c, R0, R1)	\
	uiLDI(L##uc, N, V, i, c, R0, R1)	\
	 iLDI(L##s,  N, V, i, s, R0, R1)	\
	uiLDI(L##us, N, V, i, s, R0, R1)	\
	 xLDI(L##i,  N, V, i,    R0, R1)	\
	uiLDI(L##ui, N, V, i, i, R0, R1)	\
	 xLDI(L##l,  N, V, l,    R0, R1)	\
	uiLDI(L##ul, N, V, l, l, R0, R1)	\
	 xLDI(L##p,  N, V, p,    R0, R1)

#define fLDI0(L, N, V, R0, F0, F1)	\
	 fLDI(L##f, N, V, R0, F0, F1)	\
	 dLDI(L##d, N, V, R0, F0, F1)

#define EXPAND(N, V, r0, r1, f0, f1)			\
	   STI(N, V, r0, f0)				\
	 iLDI0(r0##r1##f0##f1##N##V, N, V, r0, r1)	\
	 fLDI0(r0##r1##f0##f1##N##V, N, V, r0, f0, f1)

#define  TEST1(	     R0, R1, F0, F1)	\
	EXPAND(0, 1, R0, R1, F0, F1)	\
	EXPAND(0, 2, R0, R1, F0, F1)	\
	EXPAND(0, 3, R0, R1, F0, F1)	\
	EXPAND(1, 0, R0, R1, F0, F1)	\
	EXPAND(1, 2, R0, R1, F0, F1)	\
	EXPAND(1, 3, R0, R1, F0, F1)	\
	EXPAND(2, 0, R0, R1, F0, F1)	\
	EXPAND(2, 1, R0, R1, F0, F1)	\
	EXPAND(2, 3, R0, R1, F0, F1)	\
	EXPAND(3, 0, R0, R1, F0, F1)	\
	EXPAND(3, 1, R0, R1, F0, F1)	\
	EXPAND(3, 2, R0, R1, F0, F1)

#define TEST2F(R0, R1, F0, F1)		\
	 TEST1(R0, R1, F0, F1)		\
	 TEST1(R0, R1, F1, F0)

#define  TESTI(R0, R1, F0, F1)		\
	TEST2F(R0, R1, F0, F1)		\
	TEST2F(R1, R0, F0, F1)

#define  TEST(R0, R1)		\
	TESTI(R0, R1, f0, f1)	\
	TESTI(R0, R1, f0, f2)	\
	TESTI(R0, R1, f0, f3)	\
	TESTI(R0, R1, f0, f4)	\
	TESTI(R0, R1, f0, f5)	\
	TESTI(R0, R1, f1, f2)	\
	TESTI(R0, R1, f1, f3)	\
	TESTI(R0, R1, f1, f4)	\
	TESTI(R0, R1, f1, f5)	\
	TESTI(R0, R1, f2, f3)	\
	TESTI(R0, R1, f2, f4)	\
	TESTI(R0, R1, f2, f5)	\
	TESTI(R0, R1, f3, f4)	\
	TESTI(R0, R1, f3, f5)	\
	TESTI(R0, R1, f4, f5)

	TEST(v0, v1)
	TEST(v0, v2)
	TEST(v0, r0)
	TEST(v0, r1)
	TEST(v0, r2)
	TEST(v1, v2)
	TEST(v1, r0)
	TEST(v1, r1)
	TEST(v1, r2)
	TEST(v2, r0)
	TEST(v2, r1)
	TEST(v2, r2)
	TEST(r0, r1)
	TEST(r0, r2)
	TEST(r1, r2)

	ret
