/******************************** -*- C -*- ****************************
 *
 *	Support macros for the i386 math coprocessor
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2004, 2008 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
 *
 * This file is part of GNU lightning.
 *
 * GNU lightning is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU lightning is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GNU lightning; see the file COPYING.LESSER; if not, write to the
 * Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ***********************************************************************/


#ifndef __lightning_fp_h
#define __lightning_fp_h

/* We really must map the x87 stack onto a flat register file.  In practice,
   we can provide something sensible and make it work on the x86 using the
   stack like a file of eight registers.

   We use six or seven registers so as to have some freedom
   for floor, ceil, round, (and log, tan, atn and exp).

   Not hard at all, basically play with FXCH.  FXCH is mostly free,
   so the generated code is not bad.  Of course we special case when one
   of the operands turns out to be ST0.  */

/* - moves:

	move FPR0 to FPR3
		FST  ST3

	move FPR3 to FPR0
		FXCH ST3
		FST  ST3

	move FPR3 to FPR1
                FLD  ST3
                FSTP ST2   Stack is rotated, so FPRn becomes STn+1 */

/* - loads:

	load into FPR0
		FSTP ST0
		FLD  [FUBAR]

	load into FPR3
		FSTP ST3     Save old st0 into destination register
		FLD  [FUBAR]
		FXCH ST3     Get back old st0

   (and similarly for immediates, using the stack) */

#define JIT_FPR_NUM			6
#define JIT_FPRET			0
#define JIT_FPR(i)			(i)

/* FIXME should be a runtime flags */

/* FISTTP, FCOMI and FUCOMI are p6 or newer */
#define jit_i686()			1

/* assume always round to nearest */
/* not assuming round to nearest averages more than 3 times slower than
 * fastest version, and slightly less then 3 times slower than pre p6
 * version, but works regardless of rounding mode */
#define jit_always_round_to_nearest()	1

#define jit_absr_d(f0, f1)		jit_absr_d(f0, f1)
__jit_inline void
jit_absr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 == f1) {
	if (f0 == _ST0)
	    FABS_();
	else {
	    FXCHr(f0);
	    FABS_();
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FABS_();
	FSTPr(f0 + 1);
    }
}

#define jit_negr_d(f0, f1)		jit_negr_d(f0, f1)
__jit_inline void
jit_negr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 == f1) {
	if (f0 == _ST0)
	    FCHS_();
	else {
	    FXCHr(f0);
	    FCHS_();
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FCHS_();
	FSTPr(f0 + 1);
    }
}

#define jit_sqrtr_d(f0, f1)		jit_sqrtr_d(f0, f1)
__jit_inline void
jit_sqrtr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 == f1) {
	if (f0 == _ST0)
	    FSQRT_();
	else {
	    FXCHr(f0);
	    FSQRT_();
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FSQRT_();
	FSTPr(f0 + 1);
    }
}

#define jit_addr_d(f0, f1, f2)		jit_addr_d(f0, f1, f2)
__jit_inline void
jit_addr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1) {
	if (f2 == _ST0)
	    FADDrr(_ST0, f0);
	else if (f0 == _ST0)
	    FADDrr(f2, _ST0);
	else {
	    FXCHr(f0);
	    FADDrr(f2, _ST0);
	    FXCHr(f0);
	}
    }
    else if (f0 == f2) {
	if (f1 == _ST0)
	    FADDrr(_ST0, f0);
	else if (f0 == _ST0)
	    FADDrr(f1, _ST0);
	else {
	    FXCHr(f0);
	    FADDrr(f1, _ST0);
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FADDrr(f2 + 1, _ST0);
	FSTPr(f0 + 1);
    }
}

#define jit_subr_d(f0, f1, f2)		jit_subr_d(f0, f1, f2)
__jit_inline void
jit_subr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1) {
	if (f2 == _ST0)
	    FSUBRrr(_ST0, f0);
	else if (f0 == _ST0)
	    FSUBrr(f2, _ST0);
	else {
	    FXCHr(f0);
	    FSUBrr(f2, _ST0);
	    FXCHr(f0);
	}
    }
    else if (f0 == f2) {
	if (f1 == _ST0)
	    FSUBrr(_ST0, f0);
	else if (f0 == _ST0)
	    FSUBRrr(f1, _ST0);
	else {
	    FXCHr(f0);
	    FSUBRrr(f1, _ST0);
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FSUBrr(f2 + 1, _ST0);
	FSTPr(f0 + 1);
    }
}

#define jit_mulr_d(f0, f1, f2)		jit_mulr_d(f0, f1, f2)
__jit_inline void
jit_mulr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1) {
	if (f2 == _ST0)
	    FMULrr(_ST0, f0);
	else if (f0 == _ST0)
	    FMULrr(f2, _ST0);
	else {
	    FXCHr(f0);
	    FMULrr(f2, _ST0);
	    FXCHr(f0);
	}
    }
    else if (f0 == f2) {
	if (f1 == _ST0)
	    FMULrr(_ST0, f0);
	else if (f0 == _ST0)
	    FMULrr(f1, _ST0);
	else {
	    FXCHr(f0);
	    FMULrr(f1, _ST0);
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FMULrr(f2 + 1, _ST0);
	FSTPr(f0 + 1);
    }
}

#define jit_divr_d(f0, f1, f2)		jit_divr_d(f0, f1, f2)
__jit_inline void
jit_divr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1) {
	if (f2 == _ST0)
	    FDIVRrr(_ST0, f0);
	else if (f0 == _ST0)
	    FDIVrr(f2, _ST0);
	else {
	    FXCHr(f0);
	    FDIVrr(f2, _ST0);
	    FXCHr(f0);
	}
    }
    else if (f0 == f2) {
	if (f1 == _ST0)
	    FDIVrr(_ST0, f0);
	else if (f0 == _ST0)
	    FDIVRrr(f1, _ST0);
	else {
	    FXCHr(f0);
	    FDIVRrr(f1, _ST0);
	    FXCHr(f0);
	}
    }
    else {
	FLDr(f1);
	FDIVrr(f2 + 1, _ST0);
	FSTPr(f0 + 1);
    }
}

#define jit_ldi_f(f0, i0)		jit_ldi_f(f0, i0)
__jit_inline void
jit_ldi_f(jit_fpr_t f0, void *i0)
{
    FLDSm((long)i0, 0, 0, 0);
    FSTPr(f0 + 1);
}

#define jit_ldr_f(f0, r0)		jit_ldr_f(f0, r0)
__jit_inline void
jit_ldr_f(jit_fpr_t f0, jit_gpr_t r0)
{
    FLDSm(0, r0, 0, 0);
    FSTPr(f0 + 1);
}

#define jit_ldxi_f(f0, r0, i0)		jit_ldxi_f(f0, r0, i0)
__jit_inline void
jit_ldxi_f(jit_fpr_t f0, jit_gpr_t r0, int i0)
{
    FLDSm(i0, r0, 0, 0);
    FSTPr(f0 + 1);
}

#define jit_ldxr_f(f0, r0, i0)		jit_ldxr_f(f0, r0, i0)
__jit_inline void
jit_ldxr_f(jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    FLDSm(0, r0, r1, 1);
    FSTPr(f0 + 1);
}

#define jit_ldi_d(f0, i0)		jit_ldi_d(f0, i0)
__jit_inline void
jit_ldi_d(jit_fpr_t f0, void *i0)
{
    FLDLm((long)i0, 0, 0, 0);
    FSTPr(f0 + 1);
}

#define jit_ldr_d(f0, r0)		jit_ldr_d(f0, r0)
__jit_inline void
jit_ldr_d(jit_fpr_t f0, jit_gpr_t r0)
{
    FLDLm(0, r0, 0, 0);
    FSTPr(f0 + 1);
}

#define jit_ldxi_d(f0, r0, i0)		jit_ldxi_d(f0, r0, i0)
__jit_inline void
jit_ldxi_d(jit_fpr_t f0, jit_gpr_t r0, int i0)
{
    FLDLm(i0, r0, 0, 0);
    FSTPr(f0 + 1);
}

#define jit_ldxr_d(f0, r0, i0)		jit_ldxr_d(f0, r0, i0)
__jit_inline void
jit_ldxr_d(jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    FLDLm(0, r0, r1, 1);
    FSTPr(f0 + 1);
}

#define jit_sti_f(i0, f0)		jit_sti_f(i0, f0)
__jit_inline void
jit_sti_f(void *i0, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTSm((long)i0, 0, 0, 0);
    else {
	FXCHr(f0);
	FSTSm((long)i0, 0, 0, 0);
	FXCHr(f0);
    }
}

#define jit_str_f(i0, f0)		jit_str_f(i0, f0)
__jit_inline void
jit_str_f(jit_gpr_t r0, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTSm(0, r0, 0, 0);
    else {
	FXCHr(f0);
	FSTSm(0, r0, 0, 0);
	FXCHr(f0);
    }
}

#define jit_stxi_f(i0, r0, f0)		jit_stxi_f(i0, r0, f0)
__jit_inline void
jit_stxi_f(int i0, jit_gpr_t r0, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTSm(i0, r0, 0, 0);
    else {
	FXCHr(f0);
	FSTSm(i0, r0, 0, 0);
	FXCHr(f0);
    }
}

#define jit_stxr_f(r0, r1, f0)		jit_stxr_f(r0, r1, f0)
__jit_inline void
jit_stxr_f(jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTSm(0, r0, r1, 1);
    else {
	FXCHr(f0);
	FSTSm(0, r0, r1, 1);
	FXCHr(f0);
    }
}

#define jit_sti_d(i0, f0)		jit_sti_d(i0, f0)
__jit_inline void
jit_sti_d(void *i0, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTLm((long)i0, 0, 0, 0);
    else {
	FXCHr(f0);
	FSTLm((long)i0, 0, 0, 0);
	FXCHr(f0);
    }
}

#define jit_str_d(i0, f0)		jit_str_d(i0, f0)
__jit_inline void
jit_str_d(jit_gpr_t r0, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTLm(0, r0, 0, 0);
    else {
	FXCHr(f0);
	FSTLm(0, r0, 0, 0);
	FXCHr(f0);
    }
}

#define jit_stxi_d(i0, r0, f0)		jit_stxi_d(i0, r0, f0)
__jit_inline void
jit_stxi_d(int i0, jit_gpr_t r0, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTLm(i0, r0, 0, 0);
    else {
	FXCHr(f0);
	FSTLm(i0, r0, 0, 0);
	FXCHr(f0);
    }
}

#define jit_stxr_d(r0, r1, f0)		jit_stxr_d(r0, r1, f0)
__jit_inline void
jit_stxr_d(jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    if (f0 == _ST0)
	FSTLm(0, r0, r1, 1);
    else {
	FXCHr(f0);
	FSTLm(0, r0, r1, 1);
	FXCHr(f0);
    }
}

#define jit_movi_f(f0, i0)		jit_movi_f(f0, i0)
__jit_inline void
jit_movi_f(jit_fpr_t f0, float i0)
{
    union {
	int	i;
	float	f;
    } data;
    int		c;

    c = 1;
    data.f = i0;
    if (data.f == 0.0) {
	if (data.i & 0x80000000)
	    c = 0;
	else
	    FLDZ_();
    }
    else if (data.f == 1.0)
	FLD1_();
    /* these should be optional for reproducibly tests
     * that rely on load of truncated values */
    else if (data.f == 3.3219280948873623478703195458468f)
	FLDL2T_();
    else if (data.f == 1.4426950408889634073599246886656f)
	FLDL2E_();
    else if (data.f == 3.1415926535897932384626421096161f)
	FLDPI_();
    else if (data.f == 0.3010299956639811952137387498515f)
	FLDLG2_();
    else if (data.f == 0.6931471805599453094172323683399f)
	FLDLN2_();
    else
	c = 0;

    if (c)
	FSTPr(f0 + 1);
    else {
	jit_pushi_i(data.i);
	jit_ldr_f(f0, _RSP);
	ADDLir(sizeof(long), _RSP);
    }
}

#define jit_movi_d(f0, i0)		jit_movi_d(f0, i0)
__jit_inline void
jit_movi_d(jit_fpr_t f0, double i0)
{
    union {
	int	i[2];
	long	l;
	double	d;
    } data;
    int		c;

    c = 1;
    data.d = i0;
    if (data.d == 0.0) {
#if __WORDSIZE == 64
	if (data.l & 0x8000000000000000)
	    c = 0;
#else
	if (data.i[1] & 0x80000000)
	    c = 0;
#endif
	else
	    FLDZ_();
    }
    else if (data.d == 1.0)
	FLD1_();
    /* these should be optional for reproducibly tests
     * that rely on load of truncated values */
    else if (data.d == 3.3219280948873623478703195458468)
	FLDL2T_();
    else if (data.d == 1.4426950408889634073599246886656)
	FLDL2E_();
    else if (data.d == 3.1415926535897932384626421096161)
	FLDPI_();
    else if (data.d == 0.3010299956639811952137387498515)
	FLDLG2_();
    else if (data.d == 0.6931471805599453094172323683399)
	FLDLN2_();
    else
	c = 0;

    if (c)
	FSTPr(f0 + 1);
    else {
#if __WORDSIZE == 64
	PUSHQi(data.l);
#else
	PUSHLi(data.i[1]);
	PUSHLi(data.i[0]);
#endif
	jit_ldr_d(f0, _RSP);
	ADDLir(8, _RSP);
    }
}

#define jit_movr_d(f0, f1)		jit_movr_d(f0, f1)
__jit_inline void
jit_movr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 != f1) {
	if (f1 == _ST0)
	    FSTr(f0);
	else if (f0 == _ST0) {
	    FXCHr(f1);
	    FSTr(f1);
	}
	else {
	    FLDr(f1);
	    FSTPr(f0 + 1);
	}
    }
}

#define jit_extr_i_d(f0, r0)		jit_extr_i_d(f0, r0)
__jit_inline void
jit_extr_i_d(jit_fpr_t f0, jit_gpr_t r0)
{
    jit_pushr_i(r0);
    FILDLm(0, _RSP, 0, 0);
    FSTPr(f0 + 1);
    jit_popr_i(r0);
}

#define jit_rintr_d_i(r0, f0)		jit_rintr_d_i(r0, f0)
__jit_inline void
jit_rintr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_pushr_i(_RAX);
    /* store integer using current rounding mode */
    if (f0 == _ST0)
	FISTLm(0, _RSP, 0, 0);
    else {
	FXCHr(f0);
	FISTLm(0, _RSP, 0, 0);
	FXCHr(f0);
    }
    jit_popr_i(r0);
}

/* This looks complex/slow, but actually is quite faster than
 * adjusting the rounding mode as done in _safe_roundr_d_i
 */
__jit_inline void
_i386_roundr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;

    /* make room on stack */
    jit_pushr_i(_RAX);
    if (r0 != _RAX)
	MOVLrr(_RAX, r0);

    FLDr(f0);

    /* status test(st(0), 0.0) in %ax to know if positive */
    FTST_();
    FNSTSWr(_RAX);

    /*	Assuming default, round to nearest:
     *	f(n) = n - rint(n)
     *	f(0.3) =  0.3	f(-0.3) = -0.3
     *	f(0.5) =  0.5	f(-0.5) = -0.5	(wrong round down to even)
     *	f(0.7) = -0.3	f(-0.7) =  0.3
     *	f(1.3) =  0.3	f(-1.3) = -0.3
     *	f(1.5) = -0.5	f(-1.5) =  0.5	(correct round up to even)
     *	f(1.7) = -0.3	f(-1.7) =  0.3
     *
     *	Logic used is:
     *	-0.5 * sgn(n) + (n - rint(n))
     *
     *	If result of above is not zero, round to nearest (even on ties)
     *	will round away from zero as expected, otherwise:
     *	rint(n - -0.5 * sgn(n))
     *
     *	Example:
     *	round_to_nearest(0.5) = 0, what is wrong, following above:
     *		0.5 - rint(0.5) = 0.5
     *		-0.5 * 1 + 0.5 = 0
     *		rint(0.5 - -0.5 * 1) = 1
     *	with negative value:
     *	round_to_nearest(-2.5) = -2, what is wrong, following above:
     *		-2.5 - rint(-2.5) = -0.5
     *		-0.5 * -1 + -0.5 = 0.0
     *		rint(-2.5 - -0.5 * -1) = -3
     */

    /* st(0) = rint(st(0)) */
    FRNDINT_();

    /* st(0) = st(f0+1)-st(0) */
    FSUBRrr(f0 + 1, _ST0);

    /* st(0) = -0.5, st(1) = fract */
    FLD1_();
    FCHS_();
    F2XM1_();

    /* if (st(f0+2) is positive, do not change sign of -0.5 */
    ANDWir(FPSW_COND, _RAX);
    TESTWrr(_RAX, _RAX);
    JZSm((long)(_jit.x.pc + 2));
    label = _jit.x.pc;
    FCHS_();
    jit_patch_rel_char_at(label, _jit.x.pc);

    /* st(0) = *0.5 + fract, st(1) = *0.5 */
    FXCHr(_ST1);
    FADDrr(_ST1, _ST0);

    /* status of test(st(0), 0.0) in %ax to know if zero
     * (tie round to nearest, that was even, and was round towards zero) */
    FTST_();
    FNSTSWr(_RAX);

    /* replace top of x87 stack with jit register argument */
    FFREEr(_ST0);
    FINCSTP_();
    FLDr(f0 + 1);

    /* if operation did not result in zero, can round to near */
    ANDWir(FPSW_COND, _RAX);
    CMPWir(FPSW_EQ, _RAX);
    JNESm((long)(_jit.x.pc + 2));
    label = _jit.x.pc;

    /* adjust for round, st(0) = st(0) - *0.5 */
    FSUBrr(_ST1, _ST0);

    jit_patch_rel_char_at(label, _jit.x.pc);

    /* overwrite *0.5 with (possibly adjusted) value */
    FSTPr(_ST1);

    /* store value and pop x87 stack */
    FISTPLm(0, _RSP, 0, 0);

    if (r0 != _RAX)
	XCHGLrr(_RAX, r0);
    jit_popr_i(r0);
}

__jit_inline void
_safe_roundr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;

    /* make room on stack and save %rax */
    SUBLir(sizeof(long) << 1, _RSP);
    if (r0 != _RAX)
	MOVLrr(_RAX, r0);

    /* load copy of value */
    FLDr(f0);

    /* store control word */
    FSTCWm(0, _RSP, 0, 0);
    /* load control word */
    jit_ldr_s(_RAX, _RSP);
    /* make copy */
    jit_stxi_s(sizeof(long), _RSP, _RAX);

    /* clear top bits and select chop (truncate mode) */
    MOVZBLrr(_RAX, _RAX);
#if __WORDSIZE == 32
    ORWir(FPCW_CHOP, _RAX);
#else
    ORLir(FPCW_CHOP, _RAX);
#endif

    /* load new control word */
    jit_str_s(_RSP, _RAX);
    FLDCWm(0, _RSP, 0, 0);

    /* compare with 0 */
    FTST_();
    FNSTSWr(_RAX);

    /* load -0.5 */
    FLD1_();
    FCHS_();
    F2XM1_();

    /* if negative keep sign of -0.5 */
    ANDWir(FPSW_COND, _RAX);
    CMPWir(FPSW_LT, _RAX);
    JESm((long)(_jit.x.pc + 2));
    label = _jit.x.pc;
    FCHS_();
    jit_patch_rel_char_at(label, _jit.x.pc);

    /* add/sub 0.5 */
    FADDPr(_ST1);

    /* round adjusted value using truncation */
    FISTPLm(0, _RSP, 0, 0);

    /* load result and restore state */
    FLDCWm(sizeof(long), _RSP, 0, 0);
    if (r0 != _RAX)
	XCHGLrr(_RAX, r0);
    jit_ldr_i(r0, _RSP);
    ADDLir(sizeof(long) << 1, _RSP);
}

#define jit_roundr_d_i(r0, f0)		jit_roundr_d_i(r0, f0)
__jit_inline void
jit_roundr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_always_round_to_nearest())
	_i386_roundr_d_i(r0, f0);
    else
	_safe_roundr_d_i(r0, f0);
}

__jit_inline void
_i386_truncr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    /* make room, store control word and copy */
    SUBLir(sizeof(long) << 1, _RSP);
    FSTCWm(0, _RSP, 0, 0);
    jit_ldr_s(r0, _RSP);
    jit_stxi_s(sizeof(long), _RSP, r0);

    /* clear top bits and select chop (round towards zero) */
    if (jit_check8(r0))
	/* always the path in 64-bit mode */
	MOVZBLrr(r0, r0);
    else
	/* 32-bit mode only */
	ANDWir(0xff, r0);
#if __WORDSIZE == 32
    ORWir(FPCW_CHOP, r0);
#else
    ORLir(FPCW_CHOP, r0);
#endif

    /* load new control word and convert integer */
    jit_str_s(_RSP, r0);
    FLDCWm(0, _RSP, 0, 0);
    if (f0 == _ST0)
	FISTLm(0, _RSP, 0, 0);
    else {
	FXCHr(f0);
	FISTLm(0, _RSP, 0, 0);
	FXCHr(f0);
    }

    /* load result and restore state */
    FLDCWm(sizeof(long), _RSP, 0, 0);
    jit_ldr_i(r0, _RSP);
    ADDLir(sizeof(long) << 1, _RSP);
}

__jit_inline void
_i686_truncr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_pushr_i(_RAX);
    FLDr(f0);
    FISTTPLm(0, _RSP, 0, 0);
    jit_popr_i(r0);
}

#define jit_truncr_d_i(r0, f0)		jit_truncr_d_i(r0, f0)
__jit_inline void
jit_truncr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_i686())
	_i686_truncr_d_i(r0, f0);
    else
	/* also "safe" version */
	_i386_truncr_d_i(r0, f0);
}

__jit_inline void
_safe_floorr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    /* make room, store control word and copy */
    SUBLir(sizeof(long) << 1, _RSP);
    FSTCWm(0, _RSP, 0, 0);
    jit_ldr_s(r0, _RSP);
    jit_stxi_s(sizeof(long), _RSP, r0);

    /* clear top bits and select down (round towards minus infinity) */
    if (jit_check8(r0))
	MOVZBLrr(r0, r0);
    else
	ANDWir(0xff, r0);
#if __WORDSIZE == 32
    ORWir(FPCW_DOWN, r0);
#else
    ORLir(FPCW_DOWN, r0);
#endif

    /* load new control word and convert integer */
    jit_str_s(_RSP, r0);
    FLDCWm(0, _RSP, 0, 0);

    if (f0 == _ST0)
	FISTLm(0, _RSP, 0, 0);
    else {
	FXCHr(f0);
	FISTLm(0, _RSP, 0, 0);
	FXCHr(f0);
    }

    /* load integer and restore state */
    FLDCWm(sizeof(long), _RSP, 0, 0);
    jit_ldr_i(r0, _RSP);
    ADDLir(sizeof(long) << 1, _RSP);
}

__jit_inline void
_i386_floorr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;

    jit_pushr_i(_RAX);
    if (r0 != _RAX)
	MOVLrr(_RAX, r0);
    FLDr(f0);
    FRNDINT_();
    FUCOMr(f0 + 1);
    FNSTSWr(_RAX);
    ANDWir(FPSW_COND, _RAX);
    TESTWrr(_RAX, _RAX);
    JNESm((long)(_jit.x.pc + 4));
    label = _jit.x.pc;
    FLD1_();
    FSUBRPr(_ST1);
    jit_patch_rel_char_at(label, _jit.x.pc);
    FISTPLm(0, _RSP, 0, 0);
    if (r0 != _RAX)
	XCHGLrr(_RAX, r0);
    jit_popr_i(r0);
}

__jit_inline void
_i686_floorr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;

    /* make room */
    jit_pushr_i(_RAX);
    /* push value */
    FLDr(f0);
    /* round to nearest */
    FRNDINT_();
    /* compare and set flags */
    FXCHr(f0 + 1);
    FCOMIr(f0 + 1);
    FXCHr(f0 + 1);
    /* store integer */
    FISTPLm(0, _RSP, 0, 0);
    jit_popr_i(r0);
    JPESm((long)(_jit.x.pc + 3));
    label = _jit.x.pc;
    /* subtract 1 if carry */
    SBBLir(0, r0);
    jit_patch_rel_char_at(label, _jit.x.pc);
}

#define jit_floorr_d_i(r0, f0)		jit_floorr_d_i(r0, f0)
__jit_inline void
jit_floorr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_always_round_to_nearest()) {
	if (jit_i686())
	    _i686_floorr_d_i(r0, f0);
	else
	    _i386_floorr_d_i(r0, f0);
    }
    else
	_safe_floorr_d_i(r0, f0);
}

__jit_inline void
_safe_ceilr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    /* make room, store control word and copy */
    SUBLir(sizeof(long) << 1, _RSP);
    FSTCWm(0, _RSP, 0, 0);
    jit_ldr_s(r0, _RSP);
    jit_stxi_s(sizeof(long), _RSP, r0);

    /* clear top bits and select up (round towards positive infinity) */
    if (jit_check8(r0))
	MOVZBLrr(r0, r0);
    else
	ANDWir(0xff, r0);
#if __WORDSIZE == 32
    ORWir(FPCW_UP, r0);
#else
    ORLir(FPCW_UP, r0);
#endif

    /* load new control word and convert integer */
    jit_str_s(_RSP, r0);
    FLDCWm(0, _RSP, 0, 0);
    if (f0 == _ST0)
	FISTLm(0, _RSP, 0, 0);
    else {
	FXCHr(f0);
	FISTLm(0, _RSP, 0, 0);
	FXCHr(f0);
    }

    /* load integer and restore state */
    FLDCWm(sizeof(long), _RSP, 0, 0);
    jit_ldr_i(r0, _RSP);
    ADDLir(sizeof(long) << 1, _RSP);
}

__jit_inline void
_i386_ceilr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;

    jit_pushr_i(_RAX);
    if (r0 != _RAX)
	MOVLrr(_RAX, r0);
    FLDr(f0);
    FRNDINT_();
    FUCOMr(f0 + 1);
    FNSTSWr(_RAX);
    ANDWir(FPSW_COND, _RAX);
    CMPWir(FPSW_LT, _RAX);
    JNESm((long)(_jit.x.pc + 4));
    label = _jit.x.pc;
    FLD1_();
    FADDPr(_ST1);
    jit_patch_rel_char_at(label, _jit.x.pc);
    FISTPLm(0, _RSP, 0, 0);
    if (r0 != _RAX)
	XCHGLrr(_RAX, r0);
    jit_popr_i(r0);
}

__jit_inline void
_i686_ceilr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;

    /* make room */
    jit_pushr_i(_RAX);
    /* push value */
    FLDr(f0);
    /* round to nearest */
    FRNDINT_();
    /* compare and set flags */
    FCOMIr(f0 + 1);
    /* store integer */
    FISTPLm(0, _RSP, 0, 0);
    jit_popr_i(r0);
    JPESm((long)(_jit.x.pc + 4));
    label = _jit.x.pc;
    /* add 1 if carry */
    ADCLir(0, r0);
    jit_patch_rel_char_at(label, _jit.x.pc);
}

#define jit_ceilr_d_i(r0, f0)		jit_ceilr_d_i(r0, f0)
__jit_inline void
jit_ceilr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_always_round_to_nearest()) {
	if (jit_i686())
	    _i686_ceilr_d_i(r0, f0);
	else
	    _i386_ceilr_d_i(r0, f0);
    }
    else
	_safe_ceilr_d_i(r0, f0);
}

/* After FNSTSW we have 1 if <, 40 if =, 0 if >, 45 if unordered.  Here
   is how to map the values of the status word's high byte to the
   conditions.

         <     =     >     unord    valid values    condition
  gt     no    no    yes   no       0               STSW & 45 == 0
  lt     yes   no    no    no       1               STSW & 45 == 1
  eq     no    yes   no    no       40              STSW & 45 == 40
  unord  no    no    no    yes      45              bit 2 == 1

  ge     no    yes   no    no       0, 40           bit 0 == 0
  unlt   yes   no    no    yes      1, 45           bit 0 == 1
  ltgt   yes   no    yes   no       0, 1            bit 6 == 0
  uneq   no    yes   no    yes      40, 45          bit 6 == 1
  le     yes   yes   no    no       1, 40           odd parity for STSW & 41
  ungt   no    no    yes   yes      0, 45           even parity for STSW & 41

  unle   yes   yes   no    yes      1, 40, 45       STSW & 45 != 0
  unge   no    yes   yes   yes      0, 40, 45       STSW & 45 != 1
  ne     yes   no    yes   yes      0, 1, 45        STSW & 45 != 40
  ord    yes   yes   yes   no       0, 1, 40        bit 2 == 0

  lt, le, ungt, unge are actually computed as gt, ge, unlt, unle with
  the operands swapped; it is more efficient this way.  */

__jit_inline void
_i386_fp_cmp(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1,
	     int shift, int and, int code)
{
    if (f0 == _ST0)
	FUCOMr(f1);
    else {
	FLDr(f0);
	FUCOMPr(f1 + 1);
    }
    if (r0 != _RAX)
	MOVLrr(_RAX, r0);
    FNSTSWr(_RAX);
    SHRLir(shift, _RAX);
    if (and)
	ANDLir(and, _RAX);
    else
	MOVLir(0, _RAX);
    if (shift == 8) {
	if (code < 0) {
	    CMPBir(0x40, _RAX);
	    code = -code;
	}
	SETCCir(code, _RAX);
    }
    else if (code == 0)
	ADCBir(0, _RAX);
    else
	SBBBir(-1, _RAX);
    if (r0 != _RAX)
	XCHGLrr(r0, _RAX);
}

__jit_inline void
_i686_fp_cmp(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1, int code)
{
    int		 rc;
    jit_gpr_t	 reg;

    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }

    XORLrr(reg, reg);
    if (f0 == _ST0)
	FUCOMIr(f1);
    else {
	FLDr(f0);
	FUCOMIPr(f1 + 1);
    }
    SETCCir(code, reg);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

#define jit_ltr_d(r0, f0, f1)		jit_ltr_d(r0, f0, f1)
__jit_inline void
jit_ltr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f1, f0,	X86_CC_A);
    else
	_i386_fp_cmp(r0, f1, f0,	8, 0x45, X86_CC_Z);
}

#define jit_ler_d(r0, f0, f1)		jit_ler_d(r0, f0, f1)
__jit_inline void
jit_ler_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f1, f0,	X86_CC_AE);
    else
	_i386_fp_cmp(r0, f1, f0,	9, 0, -1);
}

#define jit_eqr_d(r0, f0, f1)		jit_eqr_d(r0, f0, f1)
__jit_inline void
jit_eqr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t			fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686()) {
	int		 rc;
	jit_gpr_t	 reg;
	jit_insn	*label;
	if ((rc = jit_check8(r0)))
	    reg = r0;
	else {
	    MOVLrr(_RAX, r0);
	    reg = _RAX;
	}
	XORLrr(reg, reg);
	if (f0 == _ST0)
	    FUCOMIr(f1);
	else {
	    FLDr(f0);
	    FUCOMIPr(f1 + 1);
	}
	JPESm((long)(_jit.x.pc + 3));
	label = _jit.x.pc;
	SETCCir(X86_CC_E, reg);
	jit_patch_rel_char_at(label, _jit.x.pc);
	if (!rc)
	    XCHGLrr(_RAX, r0);
    }
    else
	_i386_fp_cmp(r0, fr0, fr1,	8, 0x45, -X86_CC_E);
}

#define jit_ger_d(r0, f0, f1)		jit_ger_d(r0, f0, f1)
__jit_inline void
jit_ger_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f0, f1,	X86_CC_AE);
    else
	_i386_fp_cmp(r0, f0, f1,	9, 0, -1);
}

#define jit_gtr_d(r0, f0, f1)		jit_gtr_d(r0, f0, f1)
__jit_inline void
jit_gtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f0, f1,	X86_CC_A);
    else
	_i386_fp_cmp(r0, f0, f1,	8, 0x45, X86_CC_Z);
}

#define jit_ner_d(r0, f0, f1)		jit_ner_d(r0, f0, f1)
__jit_inline void
jit_ner_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686()) {
	int		 rc;
	jit_gpr_t	 reg;
	jit_insn	*label;
	if ((rc = jit_check8(r0)))
	    reg = r0;
	else {
	    MOVLrr(_RAX, r0);
	    reg = _RAX;
	}
	MOVLir(1, reg);
	if (f0 == _ST0)
	    FUCOMIr(f1);
	else {
	    FLDr(f0);
	    FUCOMIPr(f1 + 1);
	}
	JPESm((long)(_jit.x.pc + 3));
	label = _jit.x.pc;
	SETCCir(X86_CC_NE, reg);
	jit_patch_rel_char_at(label, _jit.x.pc);
	if (!rc)
	    XCHGLrr(_RAX, r0);
    }
    else
	_i386_fp_cmp(r0, fr0, fr1,	8, 0x45, -X86_CC_NE);
}

#define jit_unltr_d(r0, f0, f1)		jit_unltr_d(r0, f0, f1)
__jit_inline void
jit_unltr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f0, f1,	X86_CC_NAE);
    else
	_i386_fp_cmp(r0, f0, f1,	9, 0, 0);
}

#define jit_unler_d(r0, f0, f1)		jit_unler_d(r0, f0, f1)
__jit_inline void
jit_unler_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f0, f1,	X86_CC_NA);
    else
	_i386_fp_cmp(r0, f0, f1,	8, 0x45, X86_CC_NZ);
}

#define jit_ltgtr_d(r0, f0, f1)		jit_ltgtr_d(r0, f0, f1)
__jit_inline void
jit_ltgtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_cmp(r0, fr0, fr1,	X86_CC_NE);
    else
	_i386_fp_cmp(r0, fr0, fr1,	15, 0, -1);
}

#define jit_uneqr_d(r0, f0, f1)		jit_uneqr_d(r0, f0, f1)
__jit_inline void
jit_uneqr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_cmp(r0, fr0, fr1,	X86_CC_E);
    else
	_i386_fp_cmp(r0, fr0, fr1,	15, 0, 0);
}

#define jit_unger_d(r0, f0, f1)		jit_unger_d(r0, f0, f1)
__jit_inline void
jit_unger_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f1, f0,	X86_CC_NA);
    else
	_i386_fp_cmp(r0, f1, f0,	8, 0x45, X86_CC_NZ);
}

#define jit_ungtr_d(r0, f0, f1)		jit_ungtr_d(r0, f0, f1)
__jit_inline void
jit_ungtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_cmp(r0, f1, f0,	X86_CC_NAE);
    else
	_i386_fp_cmp(r0, f1, f0,	9, 0, 0);
}

#define jit_ordr_d(r0, f0, f1)		jit_ordr_d(r0, f0, f1)
__jit_inline void
jit_ordr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_cmp(r0, fr0, fr1,	X86_CC_NP);
    else
	_i386_fp_cmp(r0, fr0, fr1,	11, 0, -1);
}

#define jit_unordr_d(r0, f0, f1)	jit_unordr_d(r0, f0, f1)
__jit_inline void
jit_unordr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_cmp(r0, fr0, fr1,	X86_CC_P);
    else
	_i386_fp_cmp(r0, fr0, fr1,	11, 0, 0);
}

__jit_inline void
_i386_fp_bcmp(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1,
	      int shift, int and, int cmp, int code)
{
    if (f0 == _ST0)
	FUCOMr(f1);
    else {
	FLDr(f0);
	FUCOMPr(f1 + 1);
    }
    jit_pushr_i(_RAX);
    FNSTSWr(_RAX);
    SHRLir(shift, _RAX);
    if (and)
	ANDLir(and, _RAX);
    if (cmp)
	CMPLir(cmp, _RAX);
    jit_popr_i(_RAX);
    JCCim(code, label);
}

__jit_inline void
_i686_fp_bcmp(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1, int code)
{
    if (f0 == _ST0)
	FUCOMIr(f1);
    else {
	FLDr(f0);
	FUCOMIPr(f1 + 1);
    }
    JCCim(code, label);
}

#define jit_bltr_d(label, f0, f1)	jit_bltr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bltr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f1, f0,	X86_CC_A);
    else
	_i386_fp_bcmp(label, f1, f0,	8, 0x45, 0, X86_CC_Z);
    return (_jit.x.pc);
}

#define jit_bler_d(label, f0, f1)	jit_bler_d(label, f0, f1)
__jit_inline jit_insn *
jit_bler_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f1, f0,	X86_CC_AE);
    else
	_i386_fp_bcmp(label, f1, f0,	9, 0, 0, X86_CC_NC);
    return (_jit.x.pc);
}

#define jit_beqr_d(label, f0, f1)	jit_beqr_d(label, f0, f1)
__jit_inline jit_insn *
jit_beqr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686()) {
	jit_insn	*jp_label;
	if (fr0 == _ST0)
	    FUCOMIr(fr1);
	else {
	    FLDr(fr0);
	    FUCOMIPr(fr1 + 1);
	}
	/* jump past user jump if unordered */
	JPESm((long)(_jit.x.pc + 6));
	jp_label = _jit.x.pc;
	JCCim(X86_CC_E, label);
	jit_patch_rel_char_at(jp_label, _jit.x.pc);
    }
    else
	_i386_fp_bcmp(label, fr0, fr1,	8, 0x45, 0x40, X86_CC_Z);
    return (_jit.x.pc);
}

#define jit_bger_d(label, f0, f1)	jit_bger_d(label, f0, f1)
__jit_inline jit_insn *
jit_bger_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f0, f1,	X86_CC_AE);
    else
	_i386_fp_bcmp(label, f0, f1,	9, 0, 0, X86_CC_NC);
    return (_jit.x.pc);
}

#define jit_bgtr_d(label, f0, f1)	jit_bgtr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bgtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f0, f1,	X86_CC_A);
    else
	_i386_fp_bcmp(label, f0, f1,	8, 0x45, 0, X86_CC_Z);
    return (_jit.x.pc);
}

#define jit_bner_d(label, f0, f1)	jit_bner_d(label, f0, f1)
__jit_inline jit_insn *
jit_bner_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686()) {
	jit_insn	*jp_label;
	jit_insn	*jz_label;
	if (fr0 == _ST0)
	    FUCOMIr(fr1);
	else {
	    FLDr(fr0);
	    FUCOMIPr(fr1 + 1);
	}
	/* jump to user jump if unordered */
	JPESm((long)(_jit.x.pc + 2));
	jp_label = _jit.x.pc;
	/* jump past user jump if equal */
	JZSm((long)(_jit.x.pc + 5));
	jz_label = _jit.x.pc;
	jit_patch_rel_char_at(jp_label, _jit.x.pc);
	JMPm((long)label);
	jit_patch_rel_char_at(jz_label, _jit.x.pc);
    }
    else
	_i386_fp_bcmp(label, fr0, fr1,	8, 0x45, 0x40, X86_CC_NZ);
    return (_jit.x.pc);
}

#define jit_bunltr_d(label, f0, f1)	jit_bunltr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunltr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f0, f1,	X86_CC_NAE);
    else
	_i386_fp_bcmp(label, f0, f1,	9, 0, 0, X86_CC_C);
    return (_jit.x.pc);
}

#define jit_bunler_d(label, f0, f1)	jit_bunler_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunler_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f0, f1,	X86_CC_NA);
    else
	_i386_fp_bcmp(label, f0, f1,	8, 0x45, 0, X86_CC_NZ);
    return (_jit.x.pc);
}

#define jit_bltgtr_d(label, f0, f1)	jit_bltgtr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bltgtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_bcmp(label, fr0, fr1,	X86_CC_NE);
    else
	_i386_fp_bcmp(label, fr0, fr1,	15, 0, 0, X86_CC_NC);
    return (_jit.x.pc);
}

#define jit_buneqr_d(label, f0, f1)	jit_buneqr_d(label, f0, f1)
__jit_inline jit_insn *
jit_buneqr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_bcmp(label, fr0, fr1,	X86_CC_E);
    else
	_i386_fp_bcmp(label, fr0, fr1,	15, 0, 0, X86_CC_C);
    return (_jit.x.pc);
}

#define jit_bunger_d(label, f0, f1)	jit_bunger_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunger_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f1, f0,	X86_CC_NA);
    else
	_i386_fp_bcmp(label, f1, f0,	8, 0x45, 0, X86_CC_NZ);
    return (_jit.x.pc);
}

#define jit_bungtr_d(label, f0, f1)	jit_bungtr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bungtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    if (jit_i686())
	_i686_fp_bcmp(label, f1, f0,	X86_CC_NAE);
    else
	_i386_fp_bcmp(label, f1, f0,	9, 0, 0, X86_CC_C);
    return (_jit.x.pc);
}

#define jit_bordr_d(label, f0, f1)	jit_bordr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bordr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_bcmp(label, fr0, fr1,	X86_CC_NP);
    else
	_i386_fp_bcmp(label, fr0, fr1,	11, 0, 0, X86_CC_NC);
    return (_jit.x.pc);
}

#define jit_bunordr_d(label, f0, f1)	jit_bunordr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunordr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_fpr_t		fr0, fr1;
    if (f1 == _ST0)	fr0 = f1, fr1 = f0;
    else		fr0 = f0, fr1 = f1;
    if (jit_i686())
	_i686_fp_bcmp(label, fr0, fr1,	X86_CC_P);
    else
	_i386_fp_bcmp(label, fr0, fr1,	11, 0, 0, X86_CC_C);
    return (_jit.x.pc);
}

/* ABI */
#define jit_retval_d(f0)		jit_retval_d(f0)
__jit_inline void
jit_retval_d(jit_fpr_t f0)
{
    FSTPr(f0 + 1);
}

#define jit_pusharg_f(f0)		jit_pusharg_f(f0)
__jit_inline void
jit_pusharg_f(jit_fpr_t f0)
{
    jit_subi_i(JIT_SP, JIT_SP, sizeof(float));
    jit_str_f(JIT_SP, f0);
}

#define jit_pusharg_d(f0)		jit_pusharg_d(f0)
__jit_inline void
jit_pusharg_d(jit_fpr_t f0)
{
    jit_subi_i(JIT_SP, JIT_SP, sizeof(double));
    jit_str_d(JIT_SP, f0);
}

#define jit_prepare_f(nf)		jit_prepare_f(nf)
__jit_inline void
jit_prepare_f(int nf)
{
    _jitl.argssize += nf;
}

#define jit_prepare_d(nd)		jit_prepare_d(nd)
__jit_inline void
jit_prepare_d(int nd)
{
    _jitl.argssize += nd << 1;
}

#define jit_arg_f			jit_arg_f
__jit_inline int
jit_arg_f(void)
{
    int		off;
    off = _jitl.framesize;
    _jitl.framesize += sizeof(float);
    return (off);
}

#define jit_arg_d			jit_arg_d
__jit_inline int
jit_arg_d(void)
{
    int		off;
    off = _jitl.framesize;
    _jitl.framesize += sizeof(double);
    return (off);
}

#if 0
#define jit_sin()	FSIN_()				/* fsin */
#define jit_cos()	FCOS_()				/* fcos */
#define jit_tan()	(FPTAN_(), 			/* fptan */ \
			 FSTPr(_ST0))			/* fstp st */
#define jit_atn()	(FLD1_(), 			/* fld1 */ \
			 FPATAN_())			/* fpatan */
#define jit_exp()	(FLDL2E_(), 			/* fldl2e */ \
			 FMULPr(_ST1), 			/* fmulp */ \
			 FLDr(_ST0),			/* fld st */ \
			 FRNDINT(),		 	/* frndint */ \
			 FSUBRrr(_ST0, _ST1),		/* fsubr */ \
			 FXCHr(_ST1), 			/* fxch st(1) */ \
			 F2XM1_(), 			/* f2xm1 */ \
			 FLD1_(), 			/* fld1 */ \
			 FADDPr(_ST1), 			/* faddp */ \
			 FSCALE_(), 			/* fscale */ \
			 FSTPr(_ST1))			/* fstp st(1) */
#define jit_log()	(FLDLN2_(), 			/* fldln2 */ \
			 FXCHr(_ST1), 			/* fxch st(1) */ \
			 FYL2X_())			/* fyl2x */
#endif

#endif /* __lightning_fp_h */
