/******************************** -*- C -*- ****************************
 *
 *	Support macros for arm software floating-point math
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


#ifndef __lightning_fp_soft_h
#define __lightning_fp_soft_h

#include <math.h>

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

__jit_inline void
soft_movr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
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

__jit_inline void
soft_movr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	if (jit_armv5e_p()) {
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

__jit_inline void
soft_movi_f(jit_state_t _jit, jit_fpr_t r0, float i0)
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

__jit_inline void
soft_movi_d(jit_state_t _jit, jit_fpr_t r0, double i0)
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

__jit_inline void
soft_extr_i_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    int			d;
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

__jit_inline void
soft_extr_i_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    int			d;
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
    if (jit_armv5e_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

static void
soft_extr_d_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    _PUSH(0xf);
    if (jit_armv5e_p())
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

static void
soft_extr_f_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    d = (((int)__aeabi_f2d - (int)_jit->x.pc) >> 2) - 2;
    if ((d & 0xff800000) == 0xff800000 || (d & 0xff000000) == 0x00000000)
	_BL(d & 0x00ffffff);
    else {
	jit_movi_i(JIT_FTMP, (int)__aeabi_f2d);
	_BLX(JIT_FTMP);
    }
    if (jit_armv5e_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

static void
soft_if(jit_state_t _jit, float (*i0)(float), jit_gpr_t r0, jit_fpr_t r1)
{
    int			 d;
    int			 l;
    jit_insn		*is_nan;
    jit_insn		*fast_not_nan;
    jit_insn		*slow_not_nan;
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    /* >> based on fragment of __aeabi_fcmpun */
    _LSLI(JIT_FTMP, _R0, 1);
    arm_cc_srrri(ARM_CC_AL,ARM_MVN|ARM_S|ARM_ASR,JIT_TMP,0,JIT_FTMP,24);
    fast_not_nan = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    arm_cc_shift(ARM_CC_AL,ARM_S|ARM_LSL,JIT_TMP,_R0,0,9);
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
soft_id(jit_state_t _jit, double (*i0)(double), jit_gpr_t r0, jit_fpr_t r1)
{
    int			 d;
    int			 l;
    jit_insn		*is_nan;
    jit_insn		*fast_not_nan;
    jit_insn		*slow_not_nan;
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    if (jit_armv5e_p())
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
    }
    /* >> based on fragment of __aeabi_dcmpun */
    _LSLI(JIT_TMP, _R1, 1);
    arm_cc_srrri(ARM_CC_AL,ARM_MVN|ARM_S|ARM_ASR,JIT_TMP,0,JIT_TMP,21);
    fast_not_nan = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    arm_cc_srrri(ARM_CC_AL,ARM_ORR|ARM_S|ARM_LSL,JIT_TMP,_R0,_R1,12);
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

#define soft_rintr_f_i(_jit, r0, r1)	soft_if(_jit, rintf, r0, r1)
#define soft_rintr_d_i(_jit, r0, r1)	soft_id(_jit, rint, r0, r1)
#define soft_roundr_f_i(_jit, r0, r1)	soft_if(_jit, roundf, r0, r1)
#define soft_roundr_d_i(_jit, r0, r1)	soft_id(_jit, round, r0, r1)
#define soft_truncr_f_i(_jit, r0, r1)	soft_if(_jit, (void *)0, r0, r1)
#define soft_truncr_d_i(_jit, r0, r1)	soft_id(_jit, (void *)0, r0, r1)
#define soft_ceilr_f_i(_jit, r0, r1)	soft_if(_jit, ceilf, r0, r1)
#define soft_ceilr_d_i(_jit, r0, r1)	soft_id(_jit, ceil, r0, r1)
#define soft_floorr_f_i(_jit, r0, r1)	soft_if(_jit, floorf, r0, r1)
#define soft_floorr_d_i(_jit, r0, r1)	soft_id(_jit, floor, r0, r1)

__jit_inline void
soft_absr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    _BICI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_absr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
    _BICI(JIT_FTMP, JIT_FTMP,  encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    if (r0 != r1) {
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    }
}

__jit_inline void
soft_negr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    _XORI(JIT_FTMP, JIT_FTMP,  encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_negr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
    _XORI(JIT_FTMP, JIT_FTMP,  encode_arm_immediate(0x80000000));
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    if (r0 != r1) {
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    }
}

static void
soft_ff(jit_state_t _jit, float (*i0)(float), jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
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
soft_dd(jit_state_t _jit, double (*i0)(double), jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
    _PUSH(0xf);
    if (jit_armv5e_p())
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
    if (jit_armv5e_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

#define soft_sqrtr_f(_jit, r0, r1)	soft_ff(_jit, sqrtf, r0, r1)
#define soft_sqrtr_d(_jit, r0, r1)	soft_dd(_jit, sqrt, r0, r1)

static void
soft_fff(jit_state_t _jit, float (*i0)(float, float),
	 jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
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
soft_ddd(jit_state_t _jit, double (*i0)(double, double),
	 jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
    _PUSH(0xf);
    if (jit_armv5e_p()) {
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
    if (jit_armv5e_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
    _POP(0xf);
}

#define soft_addr_f(_jit, r0, r1, r2)	soft_fff(_jit, __addsf3, r0, r1, r2)
#define soft_addr_d(_jit, r0, r1, r2)	soft_ddd(_jit, __adddf3, r0, r1, r2)
#define soft_subr_f(_jit, r0, r1, r2)	soft_fff(_jit, __aeabi_fsub, r0, r1, r2)
#define soft_subr_d(_jit, r0, r1, r2)	soft_ddd(_jit, __aeabi_dsub, r0, r1, r2)
#define soft_mulr_f(_jit, r0, r1, r2)	soft_fff(_jit, __aeabi_fmul, r0, r1, r2)
#define soft_mulr_d(_jit, r0, r1, r2)	soft_ddd(_jit, __aeabi_dmul, r0, r1, r2)
#define soft_divr_f(_jit, r0, r1, r2)	soft_fff(_jit, __aeabi_fdiv, r0, r1, r2)
#define soft_divr_d(_jit, r0, r1, r2)	soft_ddd(_jit, __aeabi_ddiv, r0, r1, r2)

static void
soft_iff(jit_state_t _jit, int (*i0)(float, float),
	 jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
    int			l;
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
soft_idd(jit_state_t _jit, int (*i0)(double, double),
	 jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			l;
    int			d;
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    if (jit_armv5e_p()) {
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

#define soft_ltr_f(_jit, r0, r1, r2)	soft_iff(_jit, __aeabi_fcmplt,r0,r1,r2)
#define soft_ltr_d(_jit, r0, r1, r2)	soft_idd(_jit, __aeabi_dcmplt,r0,r1,r2)
#define soft_ler_f(_jit, r0, r1, r2)	soft_iff(_jit, __aeabi_fcmple,r0,r1,r2)
#define soft_ler_d(_jit, r0, r1, r2)	soft_idd(_jit, __aeabi_dcmple,r0,r1,r2)
#define soft_eqr_f(_jit, r0, r1, r2)	soft_iff(_jit, __aeabi_fcmpeq,r0,r1,r2)
#define soft_eqr_d(_jit, r0, r1, r2)	soft_idd(_jit, __aeabi_dcmpeq,r0,r1,r2)
#define soft_ger_f(_jit, r0, r1, r2)	soft_iff(_jit, __aeabi_fcmpge,r0,r1,r2)
#define soft_ger_d(_jit, r0, r1, r2)	soft_idd(_jit, __aeabi_dcmpge,r0,r1,r2)
#define soft_gtr_f(_jit, r0, r1, r2)	soft_iff(_jit, __aeabi_fcmpgt,r0,r1,r2)
#define soft_gtr_d(_jit, r0, r1, r2)	soft_idd(_jit, __aeabi_dcmpgt,r0,r1,r2)

__jit_inline void
soft_ner_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    soft_iff(_jit, __aeabi_fcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

__jit_inline void
soft_ner_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    soft_idd(_jit, __aeabi_dcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

static void
soft_iunff(jit_state_t _jit, int (*i0)(float, float),
	   jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    int			 l;
    jit_insn		*i;
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
soft_iundd(jit_state_t _jit, int (*i0)(double, double),
	   jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    int			 l;
    jit_insn		*i;
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    if (jit_armv5e_p()) {
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
    if (jit_armv5e_p()) {
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

#define soft_unltr_f(_jit, r0, r1, r2)	soft_iunff(_jit,__aeabi_fcmplt,r0,r1,r2)
#define soft_unltr_d(_jit, r0, r1, r2)	soft_iundd(_jit,__aeabi_dcmplt,r0,r1,r2)
#define soft_unler_f(_jit, r0, r1, r2)	soft_iunff(_jit,__aeabi_fcmple,r0,r1,r2)
#define soft_unler_d(_jit, r0, r1, r2)	soft_iundd(_jit,__aeabi_dcmple,r0,r1,r2)
#define soft_uneqr_f(_jit, r0, r1, r2)	soft_iunff(_jit,__aeabi_fcmpeq,r0,r1,r2)
#define soft_uneqr_d(_jit, r0, r1, r2)	soft_iundd(_jit,__aeabi_dcmpeq,r0,r1,r2)
#define soft_unger_f(_jit, r0, r1, r2)	soft_iunff(_jit,__aeabi_fcmpge,r0,r1,r2)
#define soft_unger_d(_jit, r0, r1, r2)	soft_iundd(_jit,__aeabi_dcmpge,r0,r1,r2)
#define soft_ungtr_f(_jit, r0, r1, r2)	soft_iunff(_jit,__aeabi_fcmpgt,r0,r1,r2)
#define soft_ungtr_d(_jit, r0, r1, r2)	soft_iundd(_jit,__aeabi_dcmpgt,r0,r1,r2)

__jit_inline void
soft_ltgtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    soft_iunff(_jit, __aeabi_fcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

__jit_inline void
soft_ltgtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    soft_iundd(_jit, __aeabi_dcmpeq, r0, r1, r2);
    _XORI(r0, r0, 1);
}

__jit_inline void
soft_ordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    soft_iff(_jit, __aeabi_fcmpun, r0, r1, r2);
    _XORI(r0, r0, 1);
}

__jit_inline void
soft_ordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    soft_idd(_jit, __aeabi_dcmpun, r0, r1, r2);
    _XORI(r0, r0, 1);
}

#define soft_unordr_f(_jit, r0, r1, r2)	soft_iunff(_jit,__aeabi_fcmpun,r0,r1,r2)
#define soft_unordr_d(_jit, r0, r1, r2)	soft_iundd(_jit,__aeabi_dcmpun,r0,r1,r2)

static jit_insn *
soft_bff(jit_state_t _jit, int (*i0)(float, float), int cc,
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
soft_bdd(jit_state_t _jit, int (*i0)(double, double), int cc,
	 void *i1, jit_fpr_t r1, jit_fpr_t r2)
{
    jit_insn		*l;
    int			 d;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5e_p()) {
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

#define soft_bltr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmplt,ARM_CC_NE,i0,r0,r1)
#define soft_bltr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmplt,ARM_CC_NE,i0,r0,r1)
#define soft_bler_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmple,ARM_CC_NE,i0,r0,r1)
#define soft_bler_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmple,ARM_CC_NE,i0,r0,r1)
#define soft_beqr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpeq,ARM_CC_NE,i0,r0,r1)
#define soft_beqr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpeq,ARM_CC_NE,i0,r0,r1)
#define soft_bger_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpge,ARM_CC_NE,i0,r0,r1)
#define soft_bger_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpge,ARM_CC_NE,i0,r0,r1)
#define soft_bgtr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpgt,ARM_CC_NE,i0,r0,r1)
#define soft_bgtr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpgt,ARM_CC_NE,i0,r0,r1)
#define soft_bner_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpeq,ARM_CC_EQ,i0,r0,r1)
#define soft_bner_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpeq,ARM_CC_EQ,i0,r0,r1)
#define soft_bunltr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpge,ARM_CC_EQ,i0,r0,r1)
#define soft_bunltr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpge,ARM_CC_EQ,i0,r0,r1)
#define soft_bunler_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpgt,ARM_CC_EQ,i0,r0,r1)
#define soft_bunler_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpgt,ARM_CC_EQ,i0,r0,r1)

static jit_insn *
soft_bunff(jit_state_t _jit, int eq, void *i1, jit_fpr_t r1, jit_fpr_t r2)
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
soft_bundd(jit_state_t _jit, int eq, void *i1, jit_fpr_t r1, jit_fpr_t r2)
{
    int			 d;
    jit_insn		*l;
    jit_insn		*j0;
    jit_insn		*j1;
    int			 i0;
    assert(r1 != JIT_FPRET && r2 != JIT_FPRET);
    _PUSH(0xf);
    if (jit_armv5e_p()) {
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
    if (jit_armv5e_p()) {
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

#define soft_buneqr_f(_jit,i0,r0,r1)	soft_bunff(_jit,1,i0,r0,r1)
#define soft_buneqr_d(_jit,i0,r0,r1)	soft_bundd(_jit,1,i0,r0,r1)
#define soft_bunger_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmplt,ARM_CC_EQ,i0,r0,r1)
#define soft_bunger_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmplt,ARM_CC_EQ,i0,r0,r1)
#define soft_bungtr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmple,ARM_CC_EQ,i0,r0,r1)
#define soft_bungtr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmple,ARM_CC_EQ,i0,r0,r1)
#define soft_bltgtr_f(_jit,i0,r0,r1)	soft_bunff(_jit,0,i0,r0,r1)
#define soft_bltgtr_d(_jit,i0,r0,r1)	soft_bundd(_jit,0,i0,r0,r1)
#define soft_bordr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpun,ARM_CC_EQ,i0,r0,r1)
#define soft_bordr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpun,ARM_CC_EQ,i0,r0,r1)
#define soft_bunordr_f(_jit,i0,r0,r1)	soft_bff(_jit,__aeabi_fcmpun,ARM_CC_NE,i0,r0,r1)
#define soft_bunordr_d(_jit,i0,r0,r1)	soft_bdd(_jit,__aeabi_dcmpun,ARM_CC_NE,i0,r0,r1)

__jit_inline void
soft_ldr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    jit_ldr_i(JIT_FTMP, r1);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_ldr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    if (jit_armv5e_p()) {
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

__jit_inline void
soft_ldi_f(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    jit_ldi_i(JIT_FTMP, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_ldi_d(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    if (jit_armv5e_p()) {
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

__jit_inline void
soft_ldxr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDR(JIT_TMP, r1, r2);
    _STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_ldxr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (jit_armv5e_p()) {
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

__jit_inline void
soft_ldxi_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    jit_ldxi_i(JIT_FTMP, r1, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_ldxi_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    if (jit_armv5e_p()) {
	if (i0 >= 0 && i0 <= 255)
	    _LDRDI(JIT_TMP, r1, i0);
	else if (i0 < 0 && i0 >= -255)
	    _LDRDIN(JIT_TMP, r1, -i0);
	else {
	    jit_addi_i(JIT_FTMP, r1, i0);
	    _LDRDI(JIT_TMP, JIT_FTMP, 0);
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
	    jit_addi_i(JIT_FTMP, r1, i0);
	    _LDRI(JIT_TMP, JIT_FTMP, 0);
	    _LDRI(JIT_FTMP, JIT_FTMP, 4);
	}
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

__jit_inline void
soft_str_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    _STRI(JIT_FTMP, r0, 0);
}

__jit_inline void
soft_str_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    if (jit_armv5e_p()) {
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

__jit_inline void
soft_sti_f(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _STRI(JIT_FTMP, JIT_TMP, 0);
}

__jit_inline void
soft_sti_d(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _STRI(JIT_FTMP, JIT_TMP, 0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    _STRI(JIT_FTMP, JIT_TMP, 4);
}

__jit_inline void
soft_stxr_f(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    _LDRIN(JIT_TMP, JIT_FP, (r2 << 3) + 8);
    _STR(JIT_TMP, r0, r1);
}

__jit_inline void
soft_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    if (jit_armv5e_p()) {
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

__jit_inline void
soft_stxi_f(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    jit_stxi_i(i0, r0, JIT_FTMP);
}

__jit_inline void
soft_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    if (jit_armv5e_p()) {
	if (i0 >= 0 && i0 <= 255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDI(JIT_TMP, r0, i0);
	}
	else if (i0 < 0 && i0 >= -255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDIN(JIT_TMP, r0, -i0);
	}
	else {
	    jit_addi_i(JIT_FTMP, r0, i0);
	    _LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRI(JIT_TMP, JIT_FTMP, 0);
	    _LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 4);
	    _STRI(JIT_TMP, JIT_FTMP, 4);
	}
    }
    else {
	jit_addi_i(JIT_FTMP, r0, i0);
	_LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	_STRI(JIT_TMP, JIT_FTMP, 0);
	_LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 4);
	_STRI(JIT_TMP, JIT_FTMP, 4);
    }
}

__jit_inline void
soft_getarg_f(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (i0 < 4)
	i0 <<= 2;
    jit_ldxi_i(JIT_FTMP, JIT_FP, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

__jit_inline void
soft_getarg_d(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (i0 < 4)
	i0 <<= 2;
    if (jit_armv5e_p()) {
	if (i0 < 255)
	    _LDRDI(JIT_TMP, JIT_FP, i0);
	else {
	    jit_addi_i(JIT_FTMP, JIT_FP, i0);
	    _LDRI(JIT_TMP, JIT_FTMP, 0);
	    _LDRI(JIT_FTMP, JIT_FTMP, 4);
	}
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	if (i0 + 4 < 255) {
	    _LDRI(JIT_TMP, JIT_FP, i0);
	    _LDRI(JIT_FTMP, JIT_FP, i0 + 4);
	}
	else {
	    jit_addi_i(JIT_FTMP, JIT_FP, i0);
	    _LDRI(JIT_TMP, JIT_FTMP, 0);
	    _LDRI(JIT_FTMP, JIT_FTMP, 4);
	}
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
}

__jit_inline void
soft_pusharg_f(jit_state_t _jit, jit_fpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;
    assert(ofs < 256);
    _jitl.stack_offset -= sizeof(float);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] &= ~(1 << (ofs & 31));
    jit_stxi_i(0, JIT_SP, JIT_FTMP);
}

__jit_inline void
soft_pusharg_d(jit_state_t _jit, jit_fpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;
    assert(ofs < 256);
    _jitl.stack_offset -= sizeof(double);
    if (jit_armv5e_p())
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

#define soft_retval_f(_jit, r0)		_STRIN(_R0, JIT_FP, (r0 << 3) + 8)
__jit_inline void
soft_retval_d(jit_state_t _jit, jit_fpr_t r0)
{
    if (jit_armv5e_p())
	_STRDIN(_R0, JIT_FP, (r0 << 3) + 8);
    else {
	_STRIN(_R0, JIT_FP, (r0 << 3) + 8);
	_STRIN(_R1, JIT_FP, (r0 << 3) + 4);
    }
}

#endif /* __lightning_fp_soft_h */
