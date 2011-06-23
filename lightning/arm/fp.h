/******************************** -*- C -*- ****************************
 *
 *	Floating-point support (arm)
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2011 Free Software Foundation, Inc.
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
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/

#ifndef __lightning_fp_arm_h
#define __lightning_fp_arm_h

#include <math.h>

#define JIT_FPR_NUM			6
static const jit_fpr_t
jit_f_order[JIT_FPR_NUM] = {
    _F0, _F1, _F2, _F3, _F4, _F5
};
#define JIT_FPR(n)			jit_f_order[n]

extern float	__addsf3(float, float);
extern double	__adddf3(double, double);
extern float	__aeabi_fsub(float, float);
extern double	__aeabi_dsub(double, double);
extern float	__aeabi_fmul(float, float);
extern double	__aeabi_dmul(double, double);
extern float	__aeabi_fdiv(float, float);
extern double	__aeabi_ddiv(double, double);
extern float	__aeabi_i2f(int);
extern double	__aeabi_i2d(int);
extern float	__aeabi_d2f(double);
extern double	__aeabi_f2d(float);
extern int	__aeabi_f2iz(double);
extern int	__aeabi_d2iz(float);
extern int	__aeabi_fcmplt(float, float);
extern int	__aeabi_dcmplt(double, double);
extern int	__aeabi_fcmple(float, float);
extern int	__aeabi_dcmple(double, double);
extern int	__aeabi_fcmpeq(float, float);
extern int	__aeabi_dcmpeq(double, double);
extern int	__aeabi_fcmpge(float, float);
extern int	__aeabi_dcmpge(double, double);
extern int	__aeabi_fcmpgt(float, float);
extern int	__aeabi_dcmpgt(double, double);
extern int	__aeabi_fcmpun(float, float);
extern int	__aeabi_dcmpun(double, double);

static void
arm_if(jit_state_t _jit, float (*i0)(float), jit_gpr_t r0, jit_fpr_t r1)
{
    int			 d;
    int			 l;
    jit_insn		*is_nan;
    jit_insn		*fast_not_nan;
    jit_insn		*slow_not_nan;
    assert(r1 != JIT_FPRET);
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    /* >> based on fragment of __aeabi_fcmpun */
    _LSLI(JIT_FTMP, _R0, 1);
    arm_cc_srrri(_jit,ARM_CC_AL,ARM_MVN|ARM_S|ARM_ASR,JIT_TMP,0,JIT_FTMP,24);
    fast_not_nan = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    arm_cc_shift(_jit,ARM_CC_AL,ARM_S|ARM_LSL,JIT_TMP,_R0,0,9);
    slow_not_nan = _jit->x.pc;
    _CC_B(ARM_CC_EQ, 0);
    _MOVI(r0, encode_arm_immediate(0x80000000));
    is_nan = _jit->x.pc;
    _CC_B(ARM_CC_AL, 0);
    jit_patch(fast_not_nan);
    jit_patch(slow_not_nan);
    /* << based on fragment of __aeabi_fcmpun */
    if (i0) {
	d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
	if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	    _BL(d & 0x00ffffff);
	else {
	    jit_movi_i(JIT_FTMP, (int)i0);
	    _BLX(JIT_FTMP);
	}
    }
    d = (((int)__aeabi_f2iz - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_f2iz);
	_BLX(JIT_FTMP);
    }
    jit_movr_i(r0, _R0);
    jit_patch(is_nan);
    _POP(l);
}

static void
arm_id(jit_state_t _jit, double (*i0)(double), jit_gpr_t r0, jit_fpr_t r1)
{
    int			 d;
    int			 l;
    jit_insn		*is_nan;
    jit_insn		*fast_not_nan;
    jit_insn		*slow_not_nan;
    assert(r1 != JIT_FPRET);
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    if (jit_armv5_p())
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
    }
    /* >> based on fragment of __aeabi_dcmpun */
    _LSLI(JIT_TMP, _R1, 1);
    arm_cc_srrri(_jit,ARM_CC_AL,ARM_MVN|ARM_S|ARM_ASR,JIT_TMP,0,JIT_TMP,21);
    fast_not_nan = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    arm_cc_srrri(_jit,ARM_CC_AL,ARM_ORR|ARM_S|ARM_LSL,JIT_TMP,_R0,_R1,12);
    slow_not_nan = _jit->x.pc;
    _CC_B(ARM_CC_EQ, 0);
    _MOVI(r0, encode_arm_immediate(0x80000000));
    is_nan = _jit->x.pc;
    _CC_B(ARM_CC_AL, 0);
    jit_patch(fast_not_nan);
    jit_patch(slow_not_nan);
    /* << based on fragment of __aeabi_dcmpun */
    if (i0) {
	d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
	if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	    _BL(d & 0x00ffffff);
	else {
	    jit_movi_i(JIT_FTMP, (int)i0);
	    _BLX(JIT_FTMP);
	}
    }
    d = (((int)__aeabi_d2iz - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_d2iz);
	_BLX(JIT_FTMP);
    }
    jit_movr_i(r0, _R0);
    jit_patch(is_nan);
    _POP(l);
}

static void
arm_ff(jit_state_t _jit, float (*i0)(float), jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET);
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    _STRIN(_R0, JIT_FP, (r0 << 3) + 8);
    _POP(0xf);
}

static void
arm_dd(jit_state_t _jit, double (*i0)(double), jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5_p())
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
    }
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    if (jit_armv5_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

static void
arm_fff(jit_state_t _jit, float (*i0)(float, float),
	jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    _STRIN(_R0, JIT_FP, (r0 << 3) + 8);
    _POP(0xf);
}

static void
arm_ddd(jit_state_t _jit, double (*i0)(double, double),
	jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    if (jit_armv5_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

static void
arm_iff(jit_state_t _jit, int (*i0)(float, float),
	jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
    int			l;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    jit_movr_i(r0, _R0);
    _POP(l);
}

static void
arm_idd(jit_state_t _jit, int (*i0)(double, double),
	jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			l;
    int			d;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    jit_movr_i(r0, _R0);
    _POP(l);
}

static void
arm_iunff(jit_state_t _jit, int (*i0)(float, float),
	  jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    int			 l;
    jit_insn		*i;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    d = (((int)__aeabi_fcmpun - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_fcmpun);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    _CC_MOVI(ARM_CC_NE, r0, 1);
    i = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    jit_movr_i(r0, _R0);
    jit_patch(i);
    _POP(l);
}

static void
arm_iundd(jit_state_t _jit, int (*i0)(double, double),
	  jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    int			 l;
    jit_insn		*i;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    d = (((int)__aeabi_dcmpun - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_dcmpun);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    _CC_MOVI(ARM_CC_NE, r0, 1);
    i = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    jit_movr_i(r0, _R0);
    jit_patch(i);
    _POP(l);
}

static jit_insn *
arm_bff(jit_state_t _jit, int (*i0)(float, float), int cc,
	void *i1, jit_fpr_t r1, jit_fpr_t r2)
{
    jit_insn		*l;
    int			 d;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    _POP(0xf);
    l = _jit->x.pc;
    d = (((int)i1 - (int)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}

static jit_insn *
arm_bdd(jit_state_t _jit, int (*i0)(double, double), int cc,
	void *i1, jit_fpr_t r1, jit_fpr_t r2)
{
    jit_insn		*l;
    int			 d;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    d = (((int)i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)i0);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    _POP(0xf);
    l = _jit->x.pc;
    d = (((int)i1 - (int)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}

static jit_insn *
arm_bunff(jit_state_t _jit, int eq, void *i1, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    jit_insn		*l;
    jit_insn		*j0;
    jit_insn		*j1;
    int			 i0;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    i0 = (int)__aeabi_fcmpun;
    d = ((i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, i0);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    /* if unordered */
    j0 = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R1, JIT_FP, (r2 << 3) + 8);
    i0 = (int)__aeabi_fcmpeq;
    d = ((i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, i0);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    j1 = _jit->x.pc;
    if (eq) {
	_CC_B(ARM_CC_EQ, 0);
	jit_patch(j0);
    }
    else
	_CC_B(ARM_CC_NE, 0);
    _POP(0xf);
    l = _jit->x.pc;
    d = (((int)i1 - (int)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_AL, d & 0x00ffffff);
    if (!eq)
	jit_patch(j0);
    jit_patch(j1);
    _POP(0xf);
    return (l);
}

static jit_insn *
arm_bundd(jit_state_t _jit, int eq, void *i1, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    jit_insn		*l;
    jit_insn		*j0;
    jit_insn		*j1;
    int			 i0;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    i0 = (int)__aeabi_dcmpun;
    d = ((i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, i0);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    /* if unordered */
    j0 = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 4);
    }
    i0 = (int)__aeabi_dcmpeq;
    d = ((i0 - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, i0);
	_BLX(JIT_FTMP);
    }
    _CMPI(_R0, 0);
    j1 = _jit->x.pc;
    if (eq) {
	_CC_B(ARM_CC_EQ, 0);
	jit_patch(j0);
    }
    else
	_CC_B(ARM_CC_NE, 0);
    _POP(0xf);
    l = _jit->x.pc;
    d = (((int)i1 - (int)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_AL, d & 0x00ffffff);
    if (!eq)
	jit_patch(j0);
    jit_patch(j1);
    _POP(0xf);
    return (l);
}

#define jit_extr_i_f(r0, r1)		arm_extr_i_f(_jit, r0, r1)
__jit_inline void
arm_extr_i_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    int			d;
    assert(r0 != JIT_FPRET);
    _PUSH(0xf);
    if (r1 != _R0)
	jit_movr_i(_R0, r1);
    d = (((int)__aeabi_i2f - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_i2f);
	_BLX(JIT_FTMP);
    }
    _STRIN(_R0, JIT_FP, (r0 << 3) + 8);
    _POP(0xf);
}

#define jit_extr_i_d(r0, r1)		arm_extr_i_d(_jit, r0, r1)
__jit_inline void
arm_extr_i_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    int			d;
    assert(r0 != JIT_FPRET);
    _PUSH(0xf);
    if (r1 != _R0)
	jit_movr_i(_R0, r1);
    d = (((int)__aeabi_i2d - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_i2d);
	_BLX(JIT_FTMP);
    }
    if (jit_armv5_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

#define jit_extr_d_f(r0, r1)		arm_extr_d_f(_jit, r0, r1)
static void
arm_extr_d_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    assert(r0 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5_p())
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
    }
    d = (((int)__aeabi_d2f - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_d2f);
	_BLX(JIT_FTMP);
    }
    _STRIN(_R0, JIT_FP, (r0 << 3) + 8);
    _POP(0xf);
}

#define jit_extr_f_d(r0, r1)		arm_extr_f_d(_jit, r0, r1)
static void
arm_extr_f_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    assert(r0 != JIT_FPRET);
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    d = (((int)__aeabi_f2d - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_f2d);
	_BLX(JIT_FTMP);
    }
    if (jit_armv5_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

#define jit_rintr_f_i(r0, r1)		arm_rintr_f_i(_jit, r0, r1)
__jit_inline void
arm_rintr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_if(_jit, rintf, r0, r1);
}

#define jit_rintr_d_i(r0, r1)		arm_truncr_d_i(_jit, r0, r1)
__jit_inline void
arm_rintr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_id(_jit, rint, r0, r1);
}

#define jit_roundr_f_i(r0, r1)		arm_roundr_f_i(_jit, r0, r1)
__jit_inline void
arm_roundr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_if(_jit, roundf, r0, r1);
}

#define jit_roundr_d_i(r0, r1)		arm_roundr_d_i(_jit, r0, r1)
__jit_inline void
arm_roundr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_id(_jit, round, r0, r1);
}

#define jit_truncr_f_i(r0, r1)		arm_truncr_f_i(_jit, r0, r1)
__jit_inline void
arm_truncr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_if(_jit, (void *)0, r0, r1);
}

#define jit_truncr_d_i(r0, r1)		arm_truncr_d_i(_jit, r0, r1)
__jit_inline void
arm_truncr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_id(_jit, (void *)0, r0, r1);
}

#define jit_ceilr_f_i(r0, r1)		arm_ceilr_f_i(_jit, r0, r1)
__jit_inline void
arm_ceilr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_if(_jit, ceilf, r0, r1);
}

#define jit_ceilr_d_i(r0, r1)		arm_ceilr_d_i(_jit, r0, r1)
__jit_inline void
arm_ceilr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_id(_jit, ceil, r0, r1);
}

#define jit_floorr_f_i(r0, r1)		arm_floorr_f_i(_jit, r0, r1)
__jit_inline void
arm_floorr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_if(_jit, floorf, r0, r1);
}

#define jit_floorr_d_i(r0, r1)		arm_floorr_d_i(_jit, r0, r1)
__jit_inline void
arm_floorr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    arm_id(_jit, floor, r0, r1);
}

#define jit_movr_f(r0, r1)		arm_movr_f(_jit, r0, r1)
__jit_inline void
arm_movr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	if (r0 == JIT_FPRET)
	    /* jit_ret() must follow! */
	    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	else {
	    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
	}
    }
}

#define jit_movr_d(r0, r1)		arm_movr_d(_jit, r0, r1)
__jit_inline void
arm_movr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	if (jit_armv5_p()) {
	    if (r0 == JIT_FPRET)
		/* jit_ret() must follow! */
		_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	    else {
		_LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
		_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	    }
	}
	else {
	    if (r0 == JIT_FPRET) {
		/* jit_ret() must follow! */
		_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
		_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	    }
	    else {
		_LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
		_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
		_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
		_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
	    }
	}
    }
}

#define jit_movi_f(r0, i0)		arm_movi_f(_jit, r0, i0)
__jit_inline void
arm_movi_f(jit_state_t _jit, jit_fpr_t r0, float i0)
{
    union {
	int	i;
	float	f;
    } u;
    u.f = i0;
    if (r0 == JIT_FPRET)
	/* jit_ret() must follow! */
	jit_movi_i(_R0, u.i);
    else {
	jit_movi_i(JIT_FTMP, u.i);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    }
}

#define jit_movi_d(r0, i0)		arm_movi_d(_jit, r0, i0)
__jit_inline void
arm_movi_d(jit_state_t _jit, jit_fpr_t r0, double i0)
{
    union {
	int	i[2];
	double	d;
    } u;
    u.d = i0;
    if (r0 == JIT_FPRET) {
	/* jit_ret() must follow! */
	jit_movi_i(_R0, u.i[0]);
	jit_movi_i(_R1, u.i[1]);
    }
    else {
	jit_movi_i(JIT_FTMP, u.i[0]);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
	jit_movi_i(JIT_FTMP, u.i[1]);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

#define jit_absr_f(r0, r1)		arm_absr_f(_jit, r0, r1)
__jit_inline void
arm_absr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    _BICI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_absr_d(r0, r1)		arm_absr_d(_jit, r0, r1)
__jit_inline void
arm_absr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET);
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
    _BICI(JIT_FTMP, JIT_FTMP,  encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    if (r0 != r1) {
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    }
}

#define jit_negr_f(r0, r1)		arm_negr_f(_jit, r0, r1)
__jit_inline void
arm_negr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET);
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    _XORI(JIT_FTMP, JIT_FTMP,  encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_negr_d(r0, r1)		arm_negr_d(_jit, r0, r1)
__jit_inline void
arm_negr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    assert(r0 != JIT_FPRET && r1 != JIT_FPRET);
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
    _XORI(JIT_FTMP, JIT_FTMP,  encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    if (r0 != r1) {
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    }
}

#define jit_sqrtr_f(r0, r1)		arm_sqrtr_f(_jit, r0, r1)
__jit_inline void
arm_sqrtr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    arm_ff(_jit, sqrtf, r0, r1);
}

#define jit_sqrtr_d(r0, r1)		arm_sqrtr_d(_jit, r0, r1)
__jit_inline void
arm_sqrtr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    arm_dd(_jit, sqrt, r0, r1);
}

#define jit_addr_f(r0, r1, r2)		arm_addr_f(_jit, r0, r1, r2)
__jit_inline void
arm_addr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_fff(_jit, __addsf3, r0, r1, r2);
}

#define jit_addr_d(r0, r1, r2)		arm_addr_d(_jit, r0, r1, r2)
__jit_inline void
arm_addr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_ddd(_jit, __adddf3, r0, r1, r2);
}

#define jit_subr_f(r0, r1, r2)		arm_subr_f(_jit, r0, r1, r2)
__jit_inline void
arm_subr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_fff(_jit, __aeabi_fsub, r0, r1, r2);
}

#define jit_subr_d(r0, r1, r2)		arm_subr_d(_jit, r0, r1, r2)
__jit_inline void
arm_subr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_ddd(_jit, __aeabi_dsub, r0, r1, r2);
}

#define jit_mulr_f(r0, r1, r2)		arm_mulr_f(_jit, r0, r1, r2)
__jit_inline void
arm_mulr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_fff(_jit, __aeabi_fmul, r0, r1, r2);
}

#define jit_mulr_d(r0, r1, r2)		arm_mulr_d(_jit, r0, r1, r2)
__jit_inline void
arm_mulr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_ddd(_jit, __aeabi_dmul, r0, r1, r2);
}

#define jit_divr_f(r0, r1, r2)		arm_divr_f(_jit, r0, r1, r2)
__jit_inline void
arm_divr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_fff(_jit, __aeabi_fdiv, r0, r1, r2);
}

#define jit_divr_d(r0, r1, r2)		arm_divr_d(_jit, r0, r1, r2)
__jit_inline void
arm_divr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_ddd(_jit, __aeabi_ddiv, r0, r1, r2);
}

#define jit_ltr_f(r0, r1, r2)		arm_ltr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ltr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmplt, r0, r1, r2);
}

#define jit_ltr_d(r0, r1, r2)		arm_ltr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ltr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmplt, r0, r1, r2);
}

#define jit_ler_f(r0, r1, r2)		arm_ler_f(_jit, r0, r1, r2)
__jit_inline void
arm_ler_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmple, r0, r1, r2);
}

#define jit_ler_d(r0, r1, r2)		arm_ler_d(_jit, r0, r1, r2)
__jit_inline void
arm_ler_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmple, r0, r1, r2);
}

#define jit_eqr_f(r0, r1, r2)		arm_eqr_f(_jit, r0, r1, r2)
__jit_inline void
arm_eqr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmpeq, r0, r1, r2);
}

#define jit_eqr_d(r0, r1, r2)		arm_eqr_d(_jit, r0, r1, r2)
__jit_inline void
arm_eqr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmpeq, r0, r1, r2);
}

#define jit_ger_f(r0, r1, r2)		arm_ger_f(_jit, r0, r1, r2)
__jit_inline void
arm_ger_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmpge, r0, r1, r2);
}

#define jit_ger_d(r0, r1, r2)		arm_ger_d(_jit, r0, r1, r2)
__jit_inline void
arm_ger_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmpge, r0, r1, r2);
}

#define jit_gtr_f(r0, r1, r2)		arm_gtr_f(_jit, r0, r1, r2)
__jit_inline void
arm_gtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmpgt, r0, r1, r2);
}

#define jit_gtr_d(r0, r1, r2)		arm_gtr_d(_jit, r0, r1, r2)
__jit_inline void
arm_gtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmpgt, r0, r1, r2);
}

#define jit_ner_f(r0, r1, r2)		arm_ner_f(_jit, r0, r1, r2)
__jit_inline void
arm_ner_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define jit_ner_d(r0, r1, r2)		arm_ner_d(_jit, r0, r1, r2)
__jit_inline void
arm_ner_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define jit_unltr_f(r0, r1, r2)		arm_unltr_f(_jit, r0, r1, r2)
__jit_inline void
arm_unltr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iunff(_jit, __aeabi_fcmplt, r0, r1, r2);
}

#define jit_unltr_d(r0, r1, r2)		arm_unltr_d(_jit, r0, r1, r2)
__jit_inline void
arm_unltr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iundd(_jit, __aeabi_dcmplt, r0, r1, r2);
}

#define jit_unler_f(r0, r1, r2)		arm_unler_f(_jit, r0, r1, r2)
__jit_inline void
arm_unler_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iunff(_jit, __aeabi_fcmple, r0, r1, r2);
}

#define jit_unler_d(r0, r1, r2)		arm_unler_d(_jit, r0, r1, r2)
__jit_inline void
arm_unler_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iundd(_jit, __aeabi_dcmple, r0, r1, r2);
}

#define jit_uneqr_f(r0, r1, r2)		arm_uneqr_f(_jit, r0, r1, r2)
__jit_inline void
arm_uneqr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iunff(_jit, __aeabi_fcmpeq, r0, r1, r2);
}

#define jit_uneqr_d(r0, r1, r2)		arm_uneqr_d(_jit, r0, r1, r2)
__jit_inline void
arm_uneqr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iundd(_jit, __aeabi_dcmpeq, r0, r1, r2);
}

#define jit_unger_f(r0, r1, r2)		arm_unger_f(_jit, r0, r1, r2)
__jit_inline void
arm_unger_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iunff(_jit, __aeabi_fcmpge, r0, r1, r2);
}

#define jit_unger_d(r0, r1, r2)		arm_unger_d(_jit, r0, r1, r2)
__jit_inline void
arm_unger_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iundd(_jit, __aeabi_dcmpge, r0, r1, r2);
}

#define jit_ungtr_f(r0, r1, r2)		arm_ungtr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ungtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iunff(_jit, __aeabi_fcmpgt, r0, r1, r2);
}

#define jit_ungtr_d(r0, r1, r2)		arm_ungtr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ungtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iundd(_jit, __aeabi_dcmpgt, r0, r1, r2);
}

#define jit_ltgtr_f(r0, r1, r2)		arm_ltgtr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ltgtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iunff(_jit, __aeabi_fcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define jit_ltgtr_d(r0, r1, r2)		arm_ltgtr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ltgtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iundd(_jit, __aeabi_dcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define jit_ordr_f(r0, r1, r2)		arm_ordr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmpun, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define jit_ordr_d(r0, r1, r2)		arm_ordr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmpun, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define jit_unordr_f(r0, r1, r2)	arm_unordr_f(_jit, r0, r1, r2)
__jit_inline void
arm_unordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_iff(_jit, __aeabi_fcmpun, r0, r1, r2);
}

#define jit_unordr_d(r0, r1, r2)	arm_unordr_d(_jit, r0, r1, r2)
__jit_inline void
arm_unordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    arm_idd(_jit, __aeabi_dcmpun, r0, r1, r2);
}

#define jit_bltr_f(i0, r0, r1)		arm_bltr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bltr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmplt, ARM_CC_NE, i0, r0, r1));
}

#define jit_bltr_d(i0, r0, r1)		arm_bltr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bltr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmplt, ARM_CC_NE, i0, r0, r1));
}

#define jit_bler_f(i0, r0, r1)		arm_bler_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bler_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmple, ARM_CC_NE, i0, r0, r1));
}

#define jit_bler_d(i0, r0, r1)		arm_bler_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bler_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmple, ARM_CC_NE, i0, r0, r1));
}

#define jit_beqr_f(i0, r0, r1)		arm_beqr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_beqr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpeq, ARM_CC_NE, i0, r0, r1));
}

#define jit_beqr_d(i0, r0, r1)		arm_beqr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_beqr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpeq, ARM_CC_NE, i0, r0, r1));
}

#define jit_bger_f(i0, r0, r1)		arm_bger_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bger_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpge, ARM_CC_NE, i0, r0, r1));
}

#define jit_bger_d(i0, r0, r1)		arm_bger_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bger_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpge, ARM_CC_NE, i0, r0, r1));
}

#define jit_bgtr_f(i0, r0, r1)		arm_bgtr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bgtr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpgt, ARM_CC_NE, i0, r0, r1));
}

#define jit_bgtr_d(i0, r0, r1)		arm_bgtr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bgtr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpgt, ARM_CC_NE, i0, r0, r1));
}

#define jit_bner_f(i0, r0, r1)		arm_bner_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bner_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpeq, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bner_d(i0, r0, r1)		arm_bner_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bner_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpeq, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bunltr_f(i0, r0, r1)	arm_bunltr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunltr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpge, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bunltr_d(i0, r0, r1)	arm_bunltr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunltr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpge, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bunler_f(i0, r0, r1)	arm_bunler_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunler_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpgt, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bunler_d(i0, r0, r1)	arm_bunler_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunler_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpgt, ARM_CC_EQ, i0, r0, r1));
}

#define jit_buneqr_f(i0, r0, r1)	arm_buneqr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_buneqr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bunff(_jit, 1, i0, r0, r1));
}

#define jit_buneqr_d(i0, r0, r1)	arm_buneqr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_buneqr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bundd(_jit, 1, i0, r0, r1));
}

#define jit_bunger_f(i0, r0, r1)	arm_bunger_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunger_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmplt, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bunger_d(i0, r0, r1)	arm_bunger_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunger_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmplt, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bungtr_f(i0, r0, r1)	arm_bungtr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bungtr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmple, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bungtr_d(i0, r0, r1)	arm_bungtr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bungtr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmple, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bltgtr_f(i0, r0, r1)	arm_bltgtr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bltgtr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bunff(_jit, 0, i0, r0, r1));
}

#define jit_bltgtr_d(i0, r0, r1)	arm_bltgtr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bltgtr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bundd(_jit, 0, i0, r0, r1));
}

#define jit_bordr_f(i0, r0, r1)		arm_bordr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bordr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpun, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bordr_d(i0, r0, r1)		arm_bordr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bordr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpun, ARM_CC_EQ, i0, r0, r1));
}

#define jit_bunordr_f(i0, r0, r1)	arm_bunordr_f(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunordr_f(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bff(_jit, __aeabi_fcmpun, ARM_CC_NE, i0, r0, r1));
}

#define jit_bunordr_d(i0, r0, r1)	arm_bunordr_d(_jit, i0, r0, r1)
__jit_inline jit_insn *
arm_bunordr_d(jit_state_t _jit, void *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    return (arm_bdd(_jit, __aeabi_dcmpun, ARM_CC_NE, i0, r0, r1));
}

#define jit_ldr_f(r0, r1)		arm_ldr_f(_jit, r0, r1)
__jit_inline void
arm_ldr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    assert(r0 != JIT_FPRET);
    jit_ldr_i(JIT_FTMP, r1);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldr_d(r0, r1)		arm_ldr_d(_jit, r0, r1)
__jit_inline void
arm_ldr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    assert(r0 != JIT_FPRET);
    if (jit_armv5_p()) {
	_LDRDI(JIT_TMP, r1, 0);
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	_LDRI(JIT_TMP, r1, 0);
	_LDRI(JIT_FTMP, r1, 4);
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

#define jit_ldi_f(r0, i0)		arm_ldi_f(_jit, r0, i0)
__jit_inline void
arm_ldi_f(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    assert(r0 != JIT_FPRET);
    jit_ldi_i(JIT_FTMP, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldi_d(r0, i0)		arm_ldi_d(_jit, r0, i0)
__jit_inline void
arm_ldi_d(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    assert(r0 != JIT_FPRET);
    jit_movi_i(JIT_TMP, (int)i0);
    if (jit_armv5_p()) {
	_LDRDI(JIT_TMP, JIT_TMP, 0);
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	_LDRI(JIT_FTMP, JIT_TMP, 4);
	_LDRI(JIT_TMP, JIT_TMP, 0);
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

#define jit_ldxr_f(r0, r1, r2)		arm_ldxr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(r0 != JIT_FPRET);
    _LDR(JIT_TMP, r1, r2);
    _STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldxr_d(r0, r1, r2)		arm_ldxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(r0 != JIT_FPRET);
    if (jit_armv5_p()) {
	_LDRD(JIT_TMP, r1, r2);
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	jit_addr_i(JIT_TMP, r1, r2);
	_LDRI(JIT_FTMP, JIT_TMP, 4);
	_LDRI(JIT_TMP, JIT_TMP, 0);
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

#define jit_ldxi_f(r0, r1, i0)		arm_ldxi_f(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    assert(r0 != JIT_FPRET);
    jit_ldxi_i(JIT_FTMP, r1, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldxi_d(r0, r1, i0)		arm_ldxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    assert(r0 != JIT_FPRET);
    if (jit_armv5_p()) {
	if (i0 >= 0 && i0 <= 255)
	    _LDRDI(JIT_TMP, r1, i0);
	else if (i0 < 0 && i0 >= -255)
	    _LDRDIN(JIT_TMP, r1, -i0);
	else {
	    jit_addi_i(JIT_TMP, r1, i0);
	    _LDRDI(JIT_TMP, JIT_TMP, 0);
	}
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	if (i0 >= -255 && i0 + 4 <= 255) {
	    if (i0 >= 0 && i0 <= 255)
		_LDRI(JIT_TMP, r1, i0);
	    else if (i0 < 0 && i0 >= -255)
		_LDRIN(JIT_TMP, r1, -i0);
	    i0 += 4;
	    if (i0 >= 0 && i0 <= 255)
		_LDRI(JIT_FTMP, r1, i0);
	    else if (i0 < 0 && i0 >= -255)
		_LDRIN(JIT_FTMP, r1, -i0);
	}
	else {
	    jit_addi_i(JIT_TMP, r1, i0);
	    _LDRI(JIT_FTMP, JIT_TMP, 4);
	    _LDRI(JIT_TMP, JIT_TMP, 0);
	}
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

#define jit_str_f(r0, r1)		arm_str_f(_jit, r0, r1)
__jit_inline void
arm_str_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(r1 != JIT_FPRET);
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    _STRI(JIT_FTMP, r0, 0);
}

#define jit_str_d(r0, r1)		arm_str_d(_jit, r0, r1)
__jit_inline void
arm_str_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(r1 != JIT_FPRET);
    if (jit_armv5_p()) {
	_LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	_STRDI(JIT_TMP, r0, 0);
    }
    else {
	_LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
	_STRI(JIT_TMP, r0, 0);
	_STRI(JIT_FTMP, r0, 4);
    }
}

#define jit_sti_f(r0, i0)		arm_sti_f(_jit, r0, i0)
__jit_inline void
arm_sti_f(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    assert(r0 != JIT_FPRET);
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _STRI(JIT_FTMP, JIT_TMP, 0);
}

#define jit_sti_d(r0, i0)		arm_sti_d(_jit, r0, i0)
__jit_inline void
arm_sti_d(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    assert(r0 != JIT_FPRET);
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _STRI(JIT_FTMP, JIT_TMP, 0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    _STRI(JIT_FTMP, JIT_TMP, 4);
}

#define jit_stxr_f(r0, r1, r2)		arm_stxr_f(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_f(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    assert(r2 != JIT_FPRET);
    _LDRIN(JIT_TMP, JIT_FP, (r2 << 3) + 8);
    _STR(JIT_TMP, r0, r1);
}

#define jit_stxr_d(r0, r1, r2)		arm_stxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    assert(r2 != JIT_FPRET);
    if (jit_armv5_p()) {
	_LDRDIN(JIT_TMP, JIT_FP, (r2 << 3) + 8);
	_STRD(JIT_TMP, r0, r1);
    }
    else {
	jit_addr_i(JIT_TMP, r0, r1);
	_LDRIN(JIT_FTMP, JIT_FP, (r2 << 3) + 8);
	_STRI(JIT_FTMP, JIT_TMP, 0);
	_LDRIN(JIT_FTMP, JIT_FP, (r2 << 3) + 4);
	_STRI(JIT_FTMP, JIT_TMP, 4);
    }
}

#define jit_stxi_f(r0, r1, i0)		arm_stxi_f(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_f(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(r1 != JIT_FPRET);
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    jit_stxi_i(i0, r0, JIT_FTMP);
}

#define jit_stxi_d(r0, r1, i0)		arm_stxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(r1 != JIT_FPRET);
    if (jit_armv5_p()) {
	if (i0 >= 0 && i0 <= 255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDI(JIT_TMP, r0, i0);
	}
	else if (i0 < 0 && i0 >= -255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDIN(JIT_TMP, r0, -i0);
	}
	else {
	    jit_addi_i(JIT_TMP, r1, i0);
	    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	    _STRI(JIT_FTMP, JIT_TMP, 0);
	    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
	    _STRI(JIT_FTMP, JIT_TMP, 4);
	}
    }
    else {
	jit_addi_i(JIT_TMP, r1, i0);
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	_STRI(JIT_FTMP, JIT_TMP, 0);
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
	_STRI(JIT_FTMP, JIT_TMP, 4);
    }
}

#define jit_prolog_f(i0)		do {} while (0)
#define jit_prolog_d(i0)		do {} while (0)

#define jit_prepare_f(i0)		arm_prepare_f(_jit, i0)
__jit_inline void
arm_prepare_f(jit_state_t _jit, int i0)
{
    assert(i0 >= 0);
    _jitl.stack_offset += i0 << 2;
}

#define jit_prepare_d(i0)		arm_prepare_d(_jit, i0)
__jit_inline void
arm_prepare_d(jit_state_t _jit, int i0)
{
    assert(i0 >= 0);
    _jitl.stack_offset += i0 << 3;
}

#define jit_arg_f()			arm_arg_f(_jit)
__jit_inline int
arm_arg_f(jit_state_t _jit)
{
    int		ofs = _jitl.nextarg_get++;
    if (ofs > 3) {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(int);
    }
    return (ofs);
}

#define jit_arg_d()			arm_arg_d(_jit)
__jit_inline int
arm_arg_d(jit_state_t _jit)
{
    int		ofs;
    if (_jitl.nextarg_get & 1)
	++_jitl.nextarg_get;
    ofs = _jitl.nextarg_get;
    if (ofs > 3) {
	if (_jitl.framesize & 7)
	    _jitl.framesize += 4;
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(double);
    }
    _jitl.nextarg_get += 2;
    return (ofs);
}

#define jit_getarg_f(r0, i0)		arm_getarg_f(_jit, r0, i0)
__jit_inline void
arm_getarg_f(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    assert(r0 != JIT_FPRET);
    if (i0 < 4)
	i0 <<= 2;
    jit_ldxi_i(JIT_FTMP, JIT_FP, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_getarg_d(r0, i0)		arm_getarg_d(_jit, r0, i0)
__jit_inline void
arm_getarg_d(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (i0 < 4)
	i0 <<= 2;
    assert(r0 != JIT_FPRET);
    if (jit_armv5_p()) {
	if (i0 < 255)
	    _LDRDI(JIT_TMP, JIT_FP, i0);
	else {
	    jit_addi_i(JIT_TMP, JIT_FP, i0);
	    _LDRI(JIT_FTMP, JIT_TMP, 4);
	    _LDRI(JIT_TMP, JIT_TMP, 0);
	}
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	if (i0 + 4 < 255) {
	    _LDRI(JIT_TMP, JIT_FP, i0);
	    _LDRI(JIT_FTMP, JIT_FP, i0 + 4);
	}
	else {
	    jit_addi_i(JIT_TMP, JIT_FP, i0);
	    _LDRI(JIT_FTMP, JIT_TMP, 4);
	    _LDRI(JIT_TMP, JIT_TMP, 0);
	}
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

#define jit_pusharg_f(r0)		arm_pusharg_f(_jit, r0)
__jit_inline void
arm_pusharg_f(jit_state_t _jit, jit_fpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;
    assert(r0 != JIT_FPRET);
    assert(ofs < 256);
    _jitl.stack_offset -= sizeof(float);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] &= ~(1 << (ofs & 31));
    jit_stxi_i(0, JIT_SP, JIT_FTMP);
}

#define jit_pusharg_d(r0)		arm_pusharg_d(_jit, r0)
__jit_inline void
arm_pusharg_d(jit_state_t _jit, jit_fpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;
    assert(r0 != JIT_FPRET);
    assert(ofs < 256);
    _jitl.stack_offset -= sizeof(double);
    if (jit_armv5_p())
	_LDRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    else {
	_LDRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] |= 1 << (ofs & 31);
    jit_stxi_i(0, JIT_SP, JIT_TMP);
    jit_stxi_i(0, JIT_SP, JIT_FTMP);
}

#define jit_retval_f(r0)		arm_retval_f(_jit, r0)
__jit_inline void
arm_retval_f(jit_state_t _jit, jit_fpr_t r0)
{
    _STRIN(_R0, JIT_FP, (r0 << 3) + 8);
}

#define jit_retval_d(r0)		arm_retval_d(_jit, r0)
__jit_inline void
arm_retval_d(jit_state_t _jit, jit_fpr_t r0)
{
    if (jit_armv5_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
}

#endif /* __lightning_fp_arm_h */
