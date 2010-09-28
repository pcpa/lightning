/* do not bother about result of operations, only ensure valid arguments
 * and that registers not modified by the operation are not clobbered  */

.data	256
#define DEFREG(reg)		reg:	.c	#reg
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
tbl:
.p	r0 r1 r2 v0 v1 v2 f0 f1 f2 f3 f4 f5
fmt:
.c	"register %%%s clobbered at line %d\n"
#define DEFDATA(name, type)		name:	.type	0
DEFDATA(c,  c)
DEFDATA(uc, c)
.align	2
DEFDATA(s,  s)
DEFDATA(us, s)
.align	4
DEFDATA(i,  i)
DEFDATA(ui, i)
.align	$(__WORDSIZE >> 3)
DEFDATA(l,  l)
DEFDATA(ul, l)
DEFDATA(p,  p)
DEFDATA(f,  f)
.align	8
DEFDATA(d,  d)
#define offc		1
#define offuc		offc
#define offs		2
#define offus		offs
#define offi		4
#define offui		offi
#if __WORDSIZE == 32
#  define offl		8
#else
#  define offl		offi
#endif
#define offul		offl
#define offp		offul
#define offf		4
#define offd		8

.code	$(4 * 1024 * 1024)
	jmpi main

#define IV0	0x10000
#define IV1	0x10001
#define IV2	0x10002
#define IV3	0x10003
#define IV4	0x10004
#define IV5	0x10005
#define FV0	100.0
#define FV1	101.0
#define FV2	102.0
#define FV3	103.0
#define FV4	104.0
#define FV5	105.0

#define IR0	r0
#define IR1	r1
#define IR2	r2
#define IR3	v0
#define IR4	v1
#define IR5	v2
#define FR0	f0
#define FR1	f1
#define FR2	f2
#define FR3	f3
#define FR4	f4
#define FR5	f5

/*--------------------------------------------------------------------*/
/* setup registers with unlikely values, for clobber detection
 * just don't use these values on tests :-); calee save registers
 * setting must be done inline */
#define setup()						\
	movi_i %v0 IV3					\
	movi_i %v1 IV4					\
	movi_i %v2 IV5					\
	calli setup
/* this indirectly also ensures that calee save registers are
 * properly restored */

setup:
	prolog 0
	movi_i %r0 IV0
	movi_i %r1 IV1
	movi_i %r2 IV2
	movi_d %f0 FV0
	movi_d %f1 FV1
	movi_d %f2 FV2
	movi_d %f3 FV3
	movi_d %f4 FV4
	movi_d %f5 FV5
	ret

/*--------------------------------------------------------------------*/
error:
.flags	push_pop	1
/* not a function - to jump on errors, with r0 being the offset in tbl,
 * of the clobbered register; stops immediately on clobber detection */
	movi_p %v0 tbl
	muli_l %r0 %r0 $(__WORDSIZE >> 3)
	ldxr_p %r0 %v0 %r0
	prepare 3
		pusharg_p %r1
		pusharg_p %r0
		movi_p %r0 fmt
		pusharg_p %r0
	finish @printf
	prepare 1
		movi_i %r0 -1
		pusharg_i %r0
	finish @exit
	calli @abort
	ret
.flags	push_pop	0

/*--------------------------------------------------------------------*/
main:
	prolog 0

	setup()

#define check(line, label, rn)				\
	beqi_i label %IR##rn IV##rn			\
	movi_i %r0 rn					\
	movi_i %r1 line					\
	jmpi error					\
label:

/* require clobbering a register for comparison */
#define checkf(line, label, rn, fr)			\
	movi_d %FR##fr FV##rn				\
	beqr_d label %FR##rn %FR##fr			\
	movi_i %r0 $(rn + 6)				\
	movi_i %r1 line					\
	jmpi error					\
label:

#define check1(line, kind, i0)			\
	 check(line, i##line##kind##i0##_0, i0)
#define check2(line, kind, i0, i1)			\
	check(line, i##line##kind##i0##i1##_0, i0)	\
	check(line, i##line##kind##i0##i1##_1, i1)
#define check3(line, kind, i0, i1, i2)			\
	check(line, i##line##kind##i0##i1##i2##_0, i0)	\
	check(line, i##line##kind##i0##i1##i2##_1, i1)	\
	check(line, i##line##kind##i0##i1##i2##_2, i2)
#define check4(line, kind, i0, i1, i2, i3)			\
	check(line, i##line##kind##i0##i1##i2##i3##_0, i0)	\
	check(line, i##line##kind##i0##i1##i2##i3##_1, i1)	\
	check(line, i##line##kind##i0##i1##i2##i3##_2, i2)	\
	check(line, i##line##kind##i0##i1##i2##i3##_3, i3)
#define check5(line, kind, i0, i1, i2, i3, i4)			\
	check(line, i##line##kind##i0##i1##i2##i3##i4##_0, i0)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##_1, i1)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##_2, i2)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##_3, i3)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##_4, i4)
#define check6(line, kind, i0, i1, i2, i3, i4, i5)			\
	check(line, i##line##kind##i0##i1##i2##i3##i4##i5##_0, i0)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##i5##_1, i1)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##i5##_2, i2)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##i5##_3, i3)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##i5##_4, i4)	\
	check(line, i##line##kind##i0##i1##i2##i3##i4##i5##_5, i5)
#define checkf1(line, kind, t, f0, fr)			\
	checkf(line, t, t##line##kind##f0##_0, f0, fr)
#define  checkf2(line, kind, t, fr, f0, f1)			\
	checkf(line, t##line##kind##f0##f1##_0, f0, fr)		\
	checkf(line, t, t##line##kind##f0##f1##_1, f1, fr)
#define  checkf3(line, kind, t, fr, f0, f1, f2)			\
	checkf(line, t##line##kind##f0##f1##f2##_0, f0, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##_1, f1, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##_2, f2, fr)
#define  checkf4(line, kind, t, fr, f0, f1, f2, f3)		\
	checkf(line, t##line##kind##f0##f1##f2##f3##_0, f0, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##_1, f1, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##_2, f2, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##_3, f3, fr)
#define  checkf5(line, kind, t, fr, f0, f1, f2, f3, f4)			\
	checkf(line, t##line##kind##f0##f1##f2##f3##f4##_0, f0, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##f4##_1, f1, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##f4##_2, f2, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##f4##_3, f3, fr)	\
	checkf(line, t##line##kind##f0##f1##f2##f3##f4##_4, f4, fr)

#define alui(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_##i %IR##i0 1				\
	op##i_##i %IR##i1 %IR##i0 1			\
	check4(line, i, i2, i3, i4, i5)
#define aluic(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_##i %IR##i0 1				\
	op##i_##i %IR##i0 %IR##i0 1			\
	check5(line, ic, i1, i2, i3, i4, i5)
#define alur(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_##i %IR##i0 1				\
	movi_##i %IR##i1 1				\
	op##r_##i %IR##i2 %IR##i0 %IR##i1		\
	check3(line, r, i3, i4, i5)
#define alurc0(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_##i %IR##i0 1				\
	movi_##i %IR##i1 1				\
	op##r_##i %IR##i0 %IR##i0 %IR##i1		\
	check4(line, rc0, i2, i3, i4, i5)
#define alurc1(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_##i %IR##i0 1				\
	movi_##i %IR##i1 1				\
	op##r_##i %IR##i1 %IR##i0 %IR##i1		\
	check4(line, rc1, i2, i3, i4, i5)
#define alurc2(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_##i %IR##i0 1				\
	op##r_##i %IR##i0 %IR##i0 %IR##i0		\
	check5(line, rc2, i1, i2, i3, i4, i5)
#define fopr(line, op, f, f0, f1, f2, f3, f4, f5)	\
	setup()						\
	movi_##f %FR##f0 1.0				\
	movi_##f %FR##f1 1.0				\
	op##r_##f %FR##f2 %FR##f0 %FR##f1		\
	check6(line, f##op##r##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf3(line, r, f, f0, f3, f4, f5)
#define foprc0(line, op, f, f0, f1, f2, f3, f4, f5)	\
	setup()						\
	movi_##f %FR##f0 1.0				\
	movi_##f %FR##f1 1.0				\
	op##r_##f %FR##f0 %FR##f0 %FR##f1		\
	check6(line, f##op##r0##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf4(line, r0, f, f0, f2, f3, f4, f5)
#define foprc1(line, op, f, f0, f1, f2, f3, f4, f5)	\
	setup()						\
	movi_##f %FR##f0 1.0				\
	movi_##f %FR##f1 1.0				\
	op##r_##f %FR##f1 %FR##f0 %FR##f1		\
	check6(line, f##op##r1##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf4(line, r1, f, f0, f2, f3, f4, f5)
#define foprc2(line, op, f, f0, f1, f2, f3, f4, f5)	\
	setup()						\
	movi_##f %FR##f0 1.0				\
	op##r_##f %FR##f0 %FR##f0 %FR##f0		\
	check6(line, f##op##r2##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, r2, f, f0, f1, f2, f3, f4, f5)
#define aluxii(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_u##i %IR##i0 1				\
	op##ci_u##i %IR##i1 %IR##i0 1			\
	op##xi_u##i %IR##i2 %IR##i0 1			\
	check3(line, i, i3, i4, i5)
#define aluxir(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_u##i %IR##i0 1				\
	op##ci_u##i %IR##i1 %IR##i0 1			\
	op##xr_u##i %IR##i2 %IR##i0 %IR##i1		\
	check3(line, ir, i3, i4, i5)
#define aluxri(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_u##i %IR##i0 1				\
	movi_u##i %IR##i1 1				\
	op##cr_u##i %IR##i2 %IR##i0 %IR##i1		\
	op##xi_u##i %IR##i0 %IR##i1 1			\
	check3(line, ri, i3, i4, i5)
#define aluxrr(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_u##i %IR##i0 1				\
	movi_u##i %IR##i1 1				\
	op##cr_u##i %IR##i2 %IR##i0 %IR##i1		\
	op##xr_u##i %IR##i2 %IR##i0 %IR##i1		\
	check3(line, rr, i3, i4, i5)
#define unyr(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_u##i %IR##i0 1				\
	op##r_##i %IR##i1 %IR##i0			\
	check4(line, rr, i2, i3, i4, i5)
#define unyrc(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	movi_u##i %IR##i0 1				\
	op##r_##i %IR##i0 %IR##i0			\
	check5(line, rr, i1, i2, i3, i4, i5)
#define unfr(line, op, f, f0, f1, f2, f3, f4, f5)	\
	setup()						\
	movi_##f %FR##f0 1.0				\
	op##r_##f %FR##f1 %FR##f0			\
	check6(line, f##op##r##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf4(line, r, f, f0, f2, f3, f4, f5)
#define unfrc(line, op, f, f0, f1, f2, f3, f4, f5)	\
	setup()						\
	movi_##f %FR##f0 1.0				\
	op##r_##f %FR##f0 %FR##f0			\
	check6(line, f##op##rc##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, r, f, f0, f1, f2, f3, f4, f5)
#define  fcpr(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	setup()								   \
	movi_##f %FR##f0 1.0						   \
	movi_##f %FR##f1 1.0						   \
	op##r_##f %IR##r0 %FR##f0 %FR##f1				   \
	check5(line, f##op##r##r0##f0##f1##f2##f3##f4##f5,		   \
	       r1, r2, r3, r4, r5)					   \
	checkf4(line, op##c##r0, f, f0, f2, f3, f4, f5)
#define fcprc(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	setup()								   \
	movi_##f %FR##f0 1.0						   \
	op##r_##f %IR##r0 %FR##f0 %FR##f0				   \
	check5(line, f##op##rc##r0##f0##f1##f2##f3##f4##f5,		   \
	       r1, r2, r3, r4, r5)					   \
	checkf5(line, op##cc##r0, f, f0, f1, f2, f3, f4, f5)
#define imvi(line, i, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_##i %IR##i0 1				\
	check5(line, mvi, i1, i2, i3, i4, i5)
#define imvr(line, i, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movr_##i %IR##i0 %IR##i1			\
	check4(line, mvr, i2, i3, i4, i5)
#define imvrc(line, i, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movr_##i %IR##i0 %IR##i1			\
	check5(line, mvrc, i1, i2, i3, i4, i5)
#define fmvi(line, f, f0, f1, f2, f3, f4, f5)		\
	setup()						\
	movi_##f %FR##f0 1				\
	check6(line, mvi##f##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, mvi, f, f0, f1, f2, f3, f4, f5)
#define fmvr(line, f, f0, f1, f2, f3, f4, f5)		\
	setup()						\
	movr_##f %FR##f0 %FR##f1			\
	check6(line, mvr##f##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, mvr, f, f0, f1, f2, f3, f4, f5)
#define fmvrc(line, f, f0, f1, f2, f3, f4, f5)		\
	setup()						\
	movr_##f %FR##f0 %FR##f0			\
	check6(line, mvc##f##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, mvc, f, f0, f1, f2, f3, f4, f5)
#define exti(line, op, t, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	op##_##t##_##i %IR##i0 %IR##i1			\
	check4(line, x, i2, i3, i4, i5)
#define extic(line, op, t, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	op##_##t##_##i %IR##i0 %IR##i0			\
	check5(line, xc, i1, i2, i3, i4, i5)
#define extfi(line, i, f, i0,i1,i2,i3,i4,i5, f0,f1,f2,f3,f4,f5)		\
	setup()								\
	movi_##i %IR##i0 1						\
	extr_##i##_##f %FR##f0 %IR##i0					\
	check5(line, x##i0##f0##f1##f2##f3##f4##f5, i1,i2,i3,i4,i5)	\
	checkf5(line, x##i0, f, f0, f1, f2, f3, f4, f5)
#define extif(line, op, f, i, i0,i1,i2,i3,i4,i5,f0,f1,f2,f3,f4,f5)	\
	setup()								\
	movi_##f %FR##f0 1.5						\
	op##r_##f##_##i %IR##i0 %FR##f0					\
	check5(line, x##i0##f0##f1##f2##f3##f4##f5, i1,i2,i3,i4,i5)	\
	checkf5(line, x##i0, f, f0, f1, f2, f3, f4, f5)
#define htni(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	op##_##i %IR##i0 %IR##i1			\
	check4(line, op, i2, i3, i4, i5)
#define htnic(line, op, i, i0, i1, i2, i3, i4, i5)	\
	setup()						\
	op##_##i %IR##i0 %IR##i0			\
	check5(line, op##c, i1, i2, i3, i4, i5)
#define ldri(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 t				\
	ldr_##t %IR##i1 %IR##i0				\
	check4(line, r, i2, i3, i4, i5)
#define ldric(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 t				\
	ldr_##t %IR##i0 %IR##i0				\
	check5(line, rc, i1, i2, i3, i4, i5)
#define ldii(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	ldi_##t %IR##i0 t				\
	check5(line, i, i1, i2, i3, i4, i5)
#define ldrf(line,t,i0,i1,i2,i3,i4,i5,f0,f1,f2,f3,f4,f5)\
	setup()						\
	movi_p %IR##i0 t				\
	ldr_##t %FR##f0 %IR##i0				\
	check5(line, f##i0##f0##f1##f2##f3##f4##f5,	\
	       i1, i2, i3, i4, i5)			\
	checkf5(line, f##i0, t, f0, f1, f2, f3, f4, f5)
#define ldif(line, t, f0, f1, f2, f3, f4, f5)		\
	setup()						\
	ldi_##t %FR##f0 t				\
	check6(line, f##i##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, f, t, f0, f1, f2, f3, f4, f5)
#define ldxii(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	ldxi_##t %IR##i1 %IR##i0 off##t			\
	check4(line, xi, i2, i3, i4, i5)
#define ldxiic(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	ldxi_##t %IR##i0 %IR##i0 off##t			\
	check5(line, xic, i1, i2, i3, i4, i5)
#define ldxri(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	movi_i %IR##i1 off##t				\
	ldxr_##t %IR##i2 %IR##i0 %IR##i1		\
	check3(line, xri, i3, i4, i5)
#define ldxri0(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	movi_i %IR##i1 off##t				\
	ldxr_##t %IR##i0 %IR##i0 %IR##i1		\
	check4(line, xri0, i2, i3, i4, i5)
#define ldxri1(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	movi_i %IR##i1 off##t				\
	ldxr_##t %IR##i1 %IR##i0 %IR##i1		\
	check4(line, xri1, i2, i3, i4, i5)
#define ldxrf(line,t,i0,i1,i2,i3,i4,i5,f0,f1,f2,f3,f4,f5)	\
	setup()							\
	movi_p %IR##i0 $(t - off##t)				\
	movi_i %IR##i1 off##t					\
	ldxr_##t %FR##f0 %IR##i0 %IR##i1			\
	check4(line, t##f##i0##i1##f0##f1##f2##f3##f4##f5,	\
	       i2, i3, i4, i5)					\
	checkf5(line, f##i0##i1, t, f0, f1, f2, f3, f4, f5)
#define stri(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 t				\
	str_##t %IR##i0 %IR##i1				\
	check4(line, ldr, i2, i3, i4, i5)
#define stric(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 t				\
	str_##t %IR##i0 %IR##i0				\
	check5(line, ldrc, i1, i2, i3, i4, i5)
#define stii(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	sti_##t t %IR##i0				\
	check5(line, ldi, i1, i2, i3, i4, i5)
#define strf(line,t,i0,i1,i2,i3,i4,i5,f0,f1,f2,f3,f4,f5)\
	setup()						\
	movi_p %IR##i0 t				\
	str_##t %IR##i0 %FR##f0				\
	check5(line, f##i0##f0##f1##f2##f3##f4##f5,	\
	       i1, i2, i3, i4, i5)			\
	checkf5(line, f##i0, t, f0, f1, f2, f3, f4, f5)
#define stif(line, t, f0, f1, f2, f3, f4, f5)		\
	setup()						\
	sti_##t t %FR##f0				\
	check6(line, f##i##f0##f1##f2##f3##f4##f5,	\
	       0, 1, 2, 3, 4, 5)			\
	checkf5(line, f, t, f0, f1, f2, f3, f4, f5)
#define stxii(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	stxi_##t off##t %IR##i0 %IR##i1			\
	check4(line, xi, i2, i3, i4, i5)
#define stxiic(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	stxi_##t off##t %IR##i0 %IR##i0			\
	check5(line, xic, i1, i2, i3, i4, i5)
#define stxri(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	movi_i %IR##i1 off##t				\
	stxr_##t %IR##i1 %IR##i0 %IR##i2		\
	check3(line, xri, i3, i4, i5)
#define stxri0(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	movi_i %IR##i1 off##t				\
	stxr_##t %IR##i1 %IR##i0 %IR##i1		\
	check4(line, xri0, i2, i3, i4, i5)
#define stxri1(line, t, i0, i1, i2, i3, i4, i5)		\
	setup()						\
	movi_p %IR##i0 $(t - off##t)			\
	movi_i %IR##i1 off##t				\
	stxr_##t %IR##i1 %IR##i0 %IR##i0		\
	check4(line, xri1, i2, i3, i4, i5)
#define stxrf(line,t,i0,i1,i2,i3,i4,i5,f0,f1,f2,f3,f4,f5)	\
	setup()							\
	movi_p %IR##i0 $(t - off##t)				\
	movi_i %IR##i1 off##t					\
	stxr_##t %IR##i1 %IR##i0 %FR##f0			\
	check4(line, t##f##i0##i1##f0##f1##f2##f3##f4##f5,	\
	       i2, i3, i4, i5)					\
	checkf5(line, f##i0##i1, t, f0, f1, f2, f3, f4, f5)
#define bri(line, op, i, i0, i1, i2, i3, i4, i5)			\
	setup()								\
	movi_##i %IR##i0 1						\
	b##op##i_##i i##line##op##i0##i1##i2##i3##i4##i5 %IR##i0 1	\
i##line##op##i0##i1##i2##i3##i4##i5:					\
	check5(line, i, i1, i2, i3, i4, i5)
#define brr(line, op, i, i0, i1, i2, i3, i4, i5)			\
	setup()								\
	movi_##i %IR##i0 1						\
	movr_##i %IR##i1 %IR##i0					\
	b##op##r_##i r##line##op##i0##i1##i2##i3##i4##i5 %IR##i1 %IR##i0\
 r##line##op##i0##i1##i2##i3##i4##i5:					\
	check4(line, r, i2, i3, i4, i5)
#define brf(line, op, f, f0, f1, f2, f3, f4, f5)			\
	setup()								\
	movi_##f %FR##f0 1.0						\
	movr_##f %FR##f1 %FR##f0					\
	b##op##r_##f r##line##op##f0##f1##f2##f3##f4##f5 %FR##f1 %FR##f0\
 r##line##op##f0##f1##f2##f3##f4##f5:					\
	check6(line, f##r##f0##f1##f2##f3##f4##f5,			\
	       0, 1, 2, 3, 4, 5)					\
	checkf3(line, r, f, f0, f3, f4, f5)

#define   xalu(line, op, i, i0, i1, i2, i3, i4, i5)		\
	  alui(line, op, i, i0, i1,	i2, i3, i4, i5)		\
	 aluic(line, op, i, i0,		i1, i2, i3, i4, i5)	\
	  alur(line, op, i, i0, i1, i2,	i3, i4, i5)		\
	alurc0(line, op, i, i0, i1,	i2, i3, i4, i5)		\
	alurc1(line, op, i, i0, i1,	i2, i3, i4, i5)		\
	alurc2(line, op, i, i0,		i1, i2, i3, i4, i5)
#define  xalux(line, op, i, i0, i1, i2, i3, i4, i5)		\
	aluxii(line, op, i, i0, i1, i2,	i3, i4, i5)		\
	aluxir(line, op, i, i0, i1, i2,	i3, i4, i5)		\
	aluxri(line, op, i, i0, i1, i2,	i3, i4, i5)		\
	aluxrr(line, op, i, i0, i1, i2,	i3, i4, i5)
#define   xuny(line, op, i, i0, i1, i2, i3, i4, i5)		\
	  unyr(line, op, i, i0, i1,	i2, i3, i4, i5)		\
	 unyrc(line, op, i, i0,		i1, i2, i3, i4, i5)
#define   xfop(line, op, f, f0, f1, f2, f3, f4, f5)		\
	  fopr(line, op, f, f0, f1, f2,		f3, f4, f5)	\
	foprc0(line, op, f, f0, f1,		f2, f3, f4, f5)	\
	foprc1(line, op, f, f0, f1,		f2, f3, f4, f5)	\
	foprc2(line, op, f, f0, f1,		f2, f3, f4, f5)
#define   xunf(line, op, f, f0, f1, f2, f3, f4, f5)		\
	  unfr(line, op, f, f0, f1,	f2, f3, f4, f5)		\
	 unfrc(line, op, f, f0,		f1, f2, f3, f4, f5)
#define  ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	 fcpr(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	fcprc(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	 fcpr(line, op, f, r1, r2, r3, r4, r5, r0, f0, f1, f2, f3, f4, f5) \
	fcprc(line, op, f, r1, r2, r3, r4, r5, r0, f0, f1, f2, f3, f4, f5) \
	 fcpr(line, op, f, r2, r3, r4, r5, r0, r1, f0, f1, f2, f3, f4, f5) \
	fcprc(line, op, f, r2, r3, r4, r5, r0, r1, f0, f1, f2, f3, f4, f5) \
	 fcpr(line, op, f, r3, r4, r5, r0, r1, r2, f0, f1, f2, f3, f4, f5) \
	fcprc(line, op, f, r3, r4, r5, r0, r1, r2, f0, f1, f2, f3, f4, f5) \
	 fcpr(line, op, f, r4, r5, r0, r1, r2, r3, f0, f1, f2, f3, f4, f5) \
	fcprc(line, op, f, r4, r5, r0, r1, r2, r3, f0, f1, f2, f3, f4, f5) \
	 fcpr(line, op, f, r5, r0, r1, r2, r3, r4, f0, f1, f2, f3, f4, f5) \
	fcprc(line, op, f, r5, r0, r1, r2, r3, r4, f0, f1, f2, f3, f4, f5)
#define  xfcp(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	 ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f0, f1, f2, f3, f4, f5) \
	 ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f1, f2, f3, f4, f5, f0) \
	 ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f2, f3, f4, f5, f0, f1) \
	 ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f3, f4, f5, f0, f1, f2) \
	 ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f4, f5, f0, f1, f2, f3) \
	 ifcp(line, op, f, r0, r1, r2, r3, r4, r5, f5, f0, f1, f2, f3, f4)
#define  xmvi(line, i, i0, i1, i2, i3, i4, i5)		\
	 imvi(line, i, i0,	i1, i2, i3, i4, i5)	\
	 imvr(line, i, i0, i1,	i2, i3, i4, i5)		\
	imvrc(line, i, i0,	i1, i2, i3, i4, i5)
#define  xmvf(line, f, f0, f1, f2, f3, f4, f5)		\
	 fmvi(line, f, f0,	f1, f2, f3, f4, f5)	\
	 fmvr(line, f, f0, f1,	f2, f3, f4, f5)		\
	fmvrc(line, f, f0,	f1, f2, f3, f4, f5)
#define  xext(line, t, i, i0, i1, i2, i3, i4, i5)		\
	 exti(line, extr, t, i, i0, i1,	i2, i3, i4, i5)		\
	extic(line, extr, t, i, i0,	i1, i2, i3, i4, i5)
#define  xi2f(line, i, f, f0, f1, f2, f3, f4, f5) 			\
	extfi(line, i, f, 0, 1, 2, 3, 4, 5, f0, f1, f2, f3, f4, f5)	\
	extfi(line, i, f, 1, 2, 3, 4, 5, 0, f1, f2, f3, f4, f5, f0)	\
	extfi(line, i, f, 2, 3, 4, 5, 0, 1, f2, f3, f4, f5, f0, f1)	\
	extfi(line, i, f, 3, 4, 5, 0, 1, 2, f3, f4, f5, f0, f1, f2)	\
	extfi(line, i, f, 4, 5, 0, 1, 2, 3, f4, f5, f0, f1, f2, f3)	\
	extfi(line, i, f, 5, 0, 1, 2, 3, 4, f5, f0, f1, f2, f3, f4)
#define  xf2i(line, op, f, i, f0, f1, f2, f3, f4, f5) 			\
	extif(line, op, f, i, 0, 1, 2, 3, 4, 5, f0, f1, f2, f3, f4, f5)	\
	extif(line, op, f, i, 1, 2, 3, 4, 5, 0, f1, f2, f3, f4, f5, f0)	\
	extif(line, op, f, i, 2, 3, 4, 5, 0, 1, f2, f3, f4, f5, f0, f1)	\
	extif(line, op, f, i, 3, 4, 5, 0, 1, 2, f3, f4, f5, f0, f1, f2)	\
	extif(line, op, f, i, 4, 5, 0, 1, 2, 3, f4, f5, f0, f1, f2, f3)	\
	extif(line, op, f, i, 5, 0, 1, 2, 3, 4, f5, f0, f1, f2, f3, f4)
#define  xh2n(line, i, i0, i1, i2, i3, i4, i5)			\
	 htni(line, hton, i, i0, i1,	i2, i3, i4, i5)		\
	htnic(line, hton, i, i0,	i1, i2, i3, i4, i5)	\
	 htni(line, ntoh, i, i0, i1,	i2, i3, i4, i5)		\
	htnic(line, ntoh, i, i0,	i1, i2, i3, i4, i5)
#define   xldi(line, t, i0, i1, i2, i3, i4, i5)	\
	  ldri(line, t, i0, i1, i2, i3, i4, i5)	\
	 ldric(line, t, i0, i1, i2, i3, i4, i5)	\
	  ldii(line, t, i0, i1, i2, i3, i4, i5)	\
	 ldxii(line, t, i0, i1, i2, i3, i4, i5)	\
	ldxiic(line, t, i0, i1, i2, i3, i4, i5)	\
	 ldxri(line, t, i0, i1, i2, i3, i4, i5)	\
	ldxri0(line, t, i0, i1, i2, i3, i4, i5)	\
	ldxri1(line, t, i0, i1, i2, i3, i4, i5)
#define  xldrf(line, t, f0, f1, f2, f3, f4, f5)				\
	  ldrf(line, t, 0, 1, 2, 3, 4, 5, f0, f1, f2, f3, f4, f5)	\
	  ldrf(line, t, 1, 2, 3, 4, 5, 0, f0, f1, f2, f3, f4, f5)	\
	  ldrf(line, t, 2, 3, 4, 5, 0, 1, f0, f1, f2, f3, f4, f5)	\
	  ldrf(line, t, 3, 4, 5, 0, 1, 2, f0, f1, f2, f3, f4, f5)	\
	  ldrf(line, t, 4, 5, 0, 1, 2, 3, f0, f1, f2, f3, f4, f5)	\
	  ldrf(line, t, 5, 0, 1, 2, 3, 4, f0, f1, f2, f3, f4, f5)	\
  	 ldxrf(line, t, 0, 1, 2, 3, 4, 5, f0, f1, f2, f3, f4, f5)	\
	 ldxrf(line, t, 1, 2, 3, 4, 5, 0, f0, f1, f2, f3, f4, f5)	\
	 ldxrf(line, t, 2, 3, 4, 5, 0, 1, f0, f1, f2, f3, f4, f5)	\
	 ldxrf(line, t, 3, 4, 5, 0, 1, 2, f0, f1, f2, f3, f4, f5)	\
	 ldxrf(line, t, 4, 5, 0, 1, 2, 3, f0, f1, f2, f3, f4, f5)	\
	 ldxrf(line, t, 5, 0, 1, 2, 3, 4, f0, f1, f2, f3, f4, f5)
#define   xsti(line, t, i0, i1, i2, i3, i4, i5)	\
	  stri(line, t, i0, i1, i2, i3, i4, i5)	\
	 stric(line, t, i0, i1, i2, i3, i4, i5)	\
	  stii(line, t, i0, i1, i2, i3, i4, i5)	\
	 stxii(line, t, i0, i1, i2, i3, i4, i5)	\
	stxiic(line, t, i0, i1, i2, i3, i4, i5)	\
	 stxri(line, t, i0, i1, i2, i3, i4, i5)	\
	stxri0(line, t, i0, i1, i2, i3, i4, i5)	\
	stxri1(line, t, i0, i1, i2, i3, i4, i5)
#define  xstrf(line, t, f0, f1, f2, f3, f4, f5)				\
	  strf(line, t, 0, 1, 2, 3, 4, 5, f0, f1, f2, f3, f4, f5)	\
	  strf(line, t, 1, 2, 3, 4, 5, 0, f0, f1, f2, f3, f4, f5)	\
	  strf(line, t, 2, 3, 4, 5, 0, 1, f0, f1, f2, f3, f4, f5)	\
	  strf(line, t, 3, 4, 5, 0, 1, 2, f0, f1, f2, f3, f4, f5)	\
	  strf(line, t, 4, 5, 0, 1, 2, 3, f0, f1, f2, f3, f4, f5)	\
	  strf(line, t, 5, 0, 1, 2, 3, 4, f0, f1, f2, f3, f4, f5)	\
  	 stxrf(line, t, 0, 1, 2, 3, 4, 5, f0, f1, f2, f3, f4, f5)	\
	 stxrf(line, t, 1, 2, 3, 4, 5, 0, f0, f1, f2, f3, f4, f5)	\
	 stxrf(line, t, 2, 3, 4, 5, 0, 1, f0, f1, f2, f3, f4, f5)	\
	 stxrf(line, t, 3, 4, 5, 0, 1, 2, f0, f1, f2, f3, f4, f5)	\
	 stxrf(line, t, 4, 5, 0, 1, 2, 3, f0, f1, f2, f3, f4, f5)	\
	 stxrf(line, t, 5, 0, 1, 2, 3, 4, f0, f1, f2, f3, f4, f5)
#define  xjmpi(line, t, op, i0, i1, i2, i3, i4, i5)	\
	   bri(line, t, op, i0, i1, i2, i3, i4, i5)	\
	   brr(line, t, op, i0, i1, i2, i3, i4, i5)

#define   alu(line, op, i)			\
	 xalu(line, op, i, 0, 1, 2, 3, 4, 5)	\
	 xalu(line, op, i, 1, 2, 3, 4, 5, 0)	\
	 xalu(line, op, i, 2, 3, 4, 5, 0, 1)	\
	 xalu(line, op, i, 3, 4, 5, 0, 1, 2)	\
	 xalu(line, op, i, 4, 5, 0, 1, 2, 3)	\
	 xalu(line, op, i, 5, 0, 1, 2, 3, 4)
#define  alux(line, op, i)			\
	xalux(line, op, i, 0, 1, 2, 3, 4, 5)	\
	xalux(line, op, i, 1, 2, 3, 4, 5, 0)	\
	xalux(line, op, i, 2, 3, 4, 5, 0, 1)	\
	xalux(line, op, i, 3, 4, 5, 0, 1, 2)	\
	xalux(line, op, i, 4, 5, 0, 1, 2, 3)	\
	xalux(line, op, i, 5, 0, 1, 2, 3, 4)
#define   uny(line, op, i)			\
	 xuny(line, op, i, 0, 1, 2, 3, 4, 5)	\
	 xuny(line, op, i, 1, 2, 3, 4, 5, 0)	\
	 xuny(line, op, i, 2, 3, 4, 5, 0, 1)	\
	 xuny(line, op, i, 3, 4, 5, 0, 1, 2)	\
	 xuny(line, op, i, 4, 5, 0, 1, 2, 3)	\
	 xuny(line, op, i, 5, 0, 1, 2, 3, 4)
#define   fop(line, op, f)			\
	 xfop(line, op, f, 0, 1, 2, 3, 4, 5)	\
	 xfop(line, op, f, 1, 2, 3, 4, 5, 0)	\
	 xfop(line, op, f, 2, 3, 4, 5, 0, 1)	\
	 xfop(line, op, f, 3, 4, 5, 0, 1, 2)	\
	 xfop(line, op, f, 4, 5, 0, 1, 2, 3)	\
	 xfop(line, op, f, 5, 0, 1, 2, 3, 4)
#define   unf(line, op, f)			\
	 xunf(line, op, f, 0, 1, 2, 3, 4, 5)	\
	 xunf(line, op, f, 1, 2, 3, 4, 5, 0)	\
	 xunf(line, op, f, 2, 3, 4, 5, 0, 1)	\
	 xunf(line, op, f, 3, 4, 5, 0, 1, 2)	\
	 xunf(line, op, f, 4, 5, 0, 1, 2, 3)	\
	 xunf(line, op, f, 5, 0, 1, 2, 3, 4)
#define  fcmp(line, op, f)					\
	 xfcp(line, op, f, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5)
#define   mvi(line, i)				\
	 xmvi(line, i, 0, 1, 2, 3, 4, 5)	\
	 xmvi(line, i, 1, 2, 3, 4, 5, 0)	\
	 xmvi(line, i, 2, 3, 4, 5, 0, 1)	\
	 xmvi(line, i, 3, 4, 5, 0, 1, 2)	\
	 xmvi(line, i, 4, 5, 0, 1, 2, 3)	\
	 xmvi(line, i, 5, 0, 1, 2, 3, 4)
#define   mvf(line, f)				\
	 xmvf(line, f, 0, 1, 2, 3, 4, 5)	\
	 xmvf(line, f, 1, 2, 3, 4, 5, 0)	\
	 xmvf(line, f, 2, 3, 4, 5, 0, 1)	\
	 xmvf(line, f, 3, 4, 5, 0, 1, 2)	\
	 xmvf(line, f, 4, 5, 0, 1, 2, 3)	\
	 xmvf(line, f, 5, 0, 1, 2, 3, 4)
#define   ext(line, t, i)			\
	 xext(line, t, i, 0, 1, 2, 3, 4, 5)	\
	 xext(line, t, i, 1, 2, 3, 4, 5, 0)	\
	 xext(line, t, i, 2, 3, 4, 5, 0, 1)	\
	 xext(line, t, i, 3, 4, 5, 0, 1, 2)	\
	 xext(line, t, i, 4, 5, 0, 1, 2, 3)	\
	 xext(line, t, i, 5, 0, 1, 2, 3, 4)
#define   i2f(line, i, f)			\
	 xi2f(line, i, f, 0, 1, 2, 3, 4, 5)	\
	 xi2f(line, i, f, 1, 2, 3, 4, 5, 0)	\
	 xi2f(line, i, f, 2, 3, 4, 5, 0, 1)	\
	 xi2f(line, i, f, 3, 4, 5, 0, 1, 2)	\
	 xi2f(line, i, f, 4, 5, 0, 1, 2, 3)	\
	 xi2f(line, i, f, 5, 0, 1, 2, 3, 4)
#define   f2i(line, op, f, i)			\
	 xf2i(line, op, f, i, 0, 1, 2, 3, 4, 5)	\
	 xf2i(line, op, f, i, 1, 2, 3, 4, 5, 0)	\
	 xf2i(line, op, f, i, 2, 3, 4, 5, 0, 1)	\
	 xf2i(line, op, f, i, 3, 4, 5, 0, 1, 2)	\
	 xf2i(line, op, f, i, 4, 5, 0, 1, 2, 3)	\
	 xf2i(line, op, f, i, 5, 0, 1, 2, 3, 4)
#define   h2n(line, i)				\
	 xh2n(line, i, 0, 1, 2, 3, 4, 5)	\
	 xh2n(line, i, 1, 2, 3, 4, 5, 0)	\
	 xh2n(line, i, 2, 3, 4, 5, 0, 1)	\
	 xh2n(line, i, 3, 4, 5, 0, 1, 2)	\
	 xh2n(line, i, 4, 5, 0, 1, 2, 3)	\
	 xh2n(line, i, 5, 0, 1, 2, 3, 4)
#define   ldi(line, t)				\
	 xldi(line, t, 0, 1, 2, 3, 4, 5)	\
	 xldi(line, t, 1, 2, 3, 4, 5, 0)	\
	 xldi(line, t, 2, 3, 4, 5, 0, 1)	\
	 xldi(line, t, 3, 4, 5, 0, 1, 2)	\
	 xldi(line, t, 4, 5, 0, 1, 2, 3)	\
	 xldi(line, t, 5, 0, 1, 2, 3, 4)
#define   ldf(line, t)				\
	xldrf(line, t, 0, 1, 2, 3, 4, 5)	\
	xldrf(line, t, 1, 2, 3, 4, 5, 0)	\
	xldrf(line, t, 2, 3, 4, 5, 0, 1)	\
	xldrf(line, t, 3, 4, 5, 0, 1, 2)	\
	xldrf(line, t, 4, 5, 0, 1, 2, 3)	\
	xldrf(line, t, 5, 0, 1, 2, 3, 4)	\
	 ldif(line, t, 0, 1, 2, 3, 4, 5)	\
	 ldif(line, t, 1, 2, 3, 4, 5, 0)	\
	 ldif(line, t, 2, 3, 4, 5, 0, 1)	\
	 ldif(line, t, 3, 4, 5, 0, 1, 2)	\
	 ldif(line, t, 4, 5, 0, 1, 2, 3)	\
	 ldif(line, t, 5, 0, 1, 2, 3, 4)
#define   sti(line, t)				\
	 xsti(line, t, 0, 1, 2, 3, 4, 5)	\
	 xsti(line, t, 1, 2, 3, 4, 5, 0)	\
	 xsti(line, t, 2, 3, 4, 5, 0, 1)	\
	 xsti(line, t, 3, 4, 5, 0, 1, 2)	\
	 xsti(line, t, 4, 5, 0, 1, 2, 3)	\
	 xsti(line, t, 5, 0, 1, 2, 3, 4)
#define   stf(line, t)				\
	xstrf(line, t, 0, 1, 2, 3, 4, 5)	\
	xstrf(line, t, 1, 2, 3, 4, 5, 0)	\
	xstrf(line, t, 2, 3, 4, 5, 0, 1)	\
	xstrf(line, t, 3, 4, 5, 0, 1, 2)	\
	xstrf(line, t, 4, 5, 0, 1, 2, 3)	\
	xstrf(line, t, 5, 0, 1, 2, 3, 4)	\
	 stif(line, t, 0, 1, 2, 3, 4, 5)	\
	 stif(line, t, 1, 2, 3, 4, 5, 0)	\
	 stif(line, t, 2, 3, 4, 5, 0, 1)	\
	 stif(line, t, 3, 4, 5, 0, 1, 2)	\
	 stif(line, t, 4, 5, 0, 1, 2, 3)	\
	 stif(line, t, 5, 0, 1, 2, 3, 4)
#define  jmpi(line, t, op)			\
	xjmpi(line, t, op, 0, 1, 2, 3, 4, 5)	\
	xjmpi(line, t, op, 1, 2, 3, 4, 5, 0)	\
	xjmpi(line, t, op, 2, 3, 4, 5, 0, 1)	\
	xjmpi(line, t, op, 3, 4, 5, 0, 1, 2)	\
	xjmpi(line, t, op, 4, 5, 0, 1, 2, 3)	\
	xjmpi(line, t, op, 5, 0, 1, 2, 3, 4)
#define  jmpf(line, t, op)			\
	  brf(line, t, op, 0, 1, 2, 3, 4, 5)	\
	  brf(line, t, op, 1, 2, 3, 4, 5, 0)	\
	  brf(line, t, op, 2, 3, 4, 5, 0, 1)	\
	  brf(line, t, op, 3, 4, 5, 0, 1, 2)	\
	  brf(line, t, op, 4, 5, 0, 1, 2, 3)	\
	  brf(line, t, op, 5, 0, 1, 2, 3, 4)

	 alu(__LINE__, add,	i)
	 alu(__LINE__, add,	l)
	 fop(__LINE__, add,	f)
	 fop(__LINE__, add,	d)
	alux(__LINE__, add,	i)
	alux(__LINE__, add,	l)
	 alu(__LINE__, sub,	i)
	 alu(__LINE__, sub,	l)
	 fop(__LINE__, sub,	f)
	 fop(__LINE__, sub,	d)
	alux(__LINE__, sub,	i)
	alux(__LINE__, sub,	l)
	 alu(__LINE__, rsb,	i)
	 alu(__LINE__, rsb,	l)
	 fop(__LINE__, rsb,	f)
	 fop(__LINE__, rsb,	d)
	 alu(__LINE__, mul,	i)
	 alu(__LINE__, mul,	l)
	 fop(__LINE__, mul,	f)
	 fop(__LINE__, mul,	d)
	 alu(__LINE__, hmul,	i)
	 alu(__LINE__, hmul,	l)
	 alu(__LINE__, div,	i)
	 alu(__LINE__, div,	l)
	 fop(__LINE__, div,	f)
	 fop(__LINE__, div,	d)
	 alu(__LINE__, mod,	i)
	 alu(__LINE__, mod,	l)
	 alu(__LINE__, and,	i)
	 alu(__LINE__, and,	l)
	 alu(__LINE__, or,	i)
	 alu(__LINE__, or,	l)
	 alu(__LINE__, xor,	i)
	 alu(__LINE__, xor,	l)
	 alu(__LINE__, lsh,	i)
	 alu(__LINE__, rsh,	l)
	 uny(__LINE__, neg,	i)
	 uny(__LINE__, neg,	l)
	 unf(__LINE__, neg,	f)
	 unf(__LINE__, neg,	d)
	 uny(__LINE__, not,	i)
	 uny(__LINE__, not,	l)
	 alu(__LINE__, lt,	i)
	 alu(__LINE__, lt,	l)
	fcmp(__LINE__, lt,	f)
	fcmp(__LINE__, lt,	d)
	 alu(__LINE__, le,	i)
	 alu(__LINE__, le,	l)
	fcmp(__LINE__, le,	f)
	fcmp(__LINE__, le,	d)
	 alu(__LINE__, eq,	i)
	 alu(__LINE__, eq,	l)
	fcmp(__LINE__, eq,	f)
	fcmp(__LINE__, eq,	d)
	 alu(__LINE__, gt,	i)
	 alu(__LINE__, gt,	l)
	fcmp(__LINE__, gt,	f)
	fcmp(__LINE__, gt,	d)
	 alu(__LINE__, ge,	i)
	 alu(__LINE__, ge,	l)
	 alu(__LINE__, ne,	i)
	 alu(__LINE__, ne,	l)
	fcmp(__LINE__, ne,	f)
	fcmp(__LINE__, ne,	d)
	fcmp(__LINE__, unlt,	f)
	fcmp(__LINE__, unlt,	d)
	fcmp(__LINE__, unle,	f)
	fcmp(__LINE__, unle,	d)
	fcmp(__LINE__, uneq,	f)
	fcmp(__LINE__, uneq,	d)
	fcmp(__LINE__, unge,	f)
	fcmp(__LINE__, unge,	d)
	fcmp(__LINE__, ungt,	f)
	fcmp(__LINE__, ungt,	d)
	fcmp(__LINE__, ltgt,	f)
	fcmp(__LINE__, ltgt,	d)
	fcmp(__LINE__, ord,	f)
	fcmp(__LINE__, ord,	d)
	fcmp(__LINE__, unord,	f)
	fcmp(__LINE__, unord, 	d)
	 mvi(__LINE__,		i)
	 mvi(__LINE__,		l)
	 mvf(__LINE__,		f)
	 mvf(__LINE__,		d)
	 ext(__LINE__, c,	i)
	 ext(__LINE__, c,	ui)
	 ext(__LINE__, c,	l)
	 ext(__LINE__, c,	ul)
	 ext(__LINE__, uc,	i)
	 ext(__LINE__, uc,	ui)
	 ext(__LINE__, uc,	l)
	 ext(__LINE__, uc,	ul)
	 ext(__LINE__, s,	i)
	 ext(__LINE__, s,	ui)
	 ext(__LINE__, s,	l)
	 ext(__LINE__, s,	ul)
	 ext(__LINE__, us,	i)
	 ext(__LINE__, us,	ui)
	 ext(__LINE__, us,	l)
	 ext(__LINE__, us,	ul)
	 ext(__LINE__, i,	l)
	 ext(__LINE__, i,	ul)
	 ext(__LINE__, ui,	l)
	 ext(__LINE__, ui,	ul)
	 i2f(__LINE__, i,	f)
	 i2f(__LINE__, i,	d)
	 i2f(__LINE__, l,	f)
	 i2f(__LINE__, l,	d)
	 f2i(__LINE__, round,	f,	i)
	 f2i(__LINE__, round,	f,	l)
	 f2i(__LINE__, round,	d,	i)
	 f2i(__LINE__, round,	d,	l)
	 f2i(__LINE__, trunc,	f,	i)
	 f2i(__LINE__, trunc,	f,	l)
	 f2i(__LINE__, trunc,	d,	i)
	 f2i(__LINE__, trunc,	d,	l)
	 f2i(__LINE__, floor,	f,	i)
	 f2i(__LINE__, floor,	f,	l)
	 f2i(__LINE__, floor,	d,	i)
	 f2i(__LINE__, floor,	d,	l)
	 f2i(__LINE__, ceil,	f,	i)
	 f2i(__LINE__, ceil,	f,	l)
	 f2i(__LINE__, ceil,	d,	i)
	 f2i(__LINE__, ceil,	d,	l)
	 h2n(__LINE__, us)
	 h2n(__LINE__, ui)
	 ldi(__LINE__, c)
	 ldi(__LINE__, uc)
	 ldi(__LINE__, s)
	 ldi(__LINE__, us)
	 ldi(__LINE__, i)
	 ldi(__LINE__, ui)
	 ldi(__LINE__, l)
	 ldi(__LINE__, ul)
	 ldi(__LINE__, p)
	 ldf(__LINE__, f)
	 ldf(__LINE__, d)
	 sti(__LINE__, c)
	 sti(__LINE__, uc)
	 sti(__LINE__, s)
	 sti(__LINE__, us)
	 sti(__LINE__, i)
	 sti(__LINE__, ui)
	 sti(__LINE__, l)
	 sti(__LINE__, ul)
	 sti(__LINE__, p)
	 stf(__LINE__, f)
	 stf(__LINE__, d)
	jmpi(__LINE__, lt,	i)
	jmpi(__LINE__, lt,	ui)
	jmpi(__LINE__, lt,	l)
	jmpi(__LINE__, lt,	ul)
	jmpf(__LINE__, lt,	f)
	jmpf(__LINE__, lt,	d)
	jmpi(__LINE__, le,	i)
	jmpi(__LINE__, le,	ui)
	jmpi(__LINE__, le,	l)
	jmpi(__LINE__, le,	ul)
	jmpf(__LINE__, le,	f)
	jmpf(__LINE__, le,	d)
	jmpi(__LINE__, eq,	i)
	jmpi(__LINE__, eq,	ui)
	jmpi(__LINE__, eq,	l)
	jmpi(__LINE__, eq,	ul)
	jmpf(__LINE__, eq,	f)
	jmpf(__LINE__, eq,	d)
	jmpi(__LINE__, ge,	i)
	jmpi(__LINE__, ge,	ui)
	jmpi(__LINE__, ge,	l)
	jmpi(__LINE__, ge,	ul)
	jmpf(__LINE__, ge,	f)
	jmpf(__LINE__, ge,	d)
	jmpi(__LINE__, gt,	i)
	jmpi(__LINE__, gt,	ui)
	jmpi(__LINE__, gt,	l)
	jmpi(__LINE__, gt,	ul)
	jmpf(__LINE__, gt,	f)
	jmpf(__LINE__, gt,	d)
	jmpi(__LINE__, ne,	i)
	jmpi(__LINE__, ne,	ui)
	jmpi(__LINE__, ne,	l)
	jmpi(__LINE__, ne,	ul)
	jmpf(__LINE__, ne,	f)
	jmpf(__LINE__, ne,	d)
	jmpf(__LINE__, unlt,	f)
	jmpf(__LINE__, unlt,	d)
	jmpf(__LINE__, unle,	f)
	jmpf(__LINE__, unle,	d)
	jmpf(__LINE__, uneq,	f)
	jmpf(__LINE__, uneq,	d)
	jmpf(__LINE__, unge,	f)
	jmpf(__LINE__, unge,	d)
	jmpf(__LINE__, ungt,	f)
	jmpf(__LINE__, ungt,	d)
	jmpf(__LINE__, ltgt,	f)
	jmpf(__LINE__, ltgt,	d)
	jmpf(__LINE__, ord,	f)
	jmpf(__LINE__, ord,	d)
	jmpf(__LINE__, unord,	f)
	jmpf(__LINE__, unord,	d)
	jmpi(__LINE__, ms,	ui)
	jmpi(__LINE__, ms,	ul)
	jmpi(__LINE__, mc,	ui)
	jmpi(__LINE__, mc,	ul)
	jmpi(__LINE__, oadd,	i)
	jmpi(__LINE__, oadd,	ui)
	jmpi(__LINE__, oadd,	l)
	jmpi(__LINE__, oadd,	ul)
	jmpi(__LINE__, osub,	i)
	jmpi(__LINE__, osub,	ui)
	jmpi(__LINE__, osub,	l)
	jmpi(__LINE__, osub,	ul)

	ret
