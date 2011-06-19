/******************************** -*- C -*- ****************************
 *
 *	Floating-point support (arm)
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2010 Free Software Foundation, Inc.
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
extern float	__aeabi_d2f(double);
extern double	__aeabi_f2d(float);
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
arm_fff(jit_state_t _jit, float (*i0)(float, float),
	jit_fpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    int			d;
    _PUSH(0xf);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
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
    _PUSH(0xf);
    if (jit_armv5_p()) {
	_LDRDIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRDIN(_R2, JIT_FP, (r2 << 3) + 8);
    }
    else {
	_LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
	_LDRIN(_R1, JIT_FP, (r1 << 3) + 4);
	_LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
	_LDRIN(_R3, JIT_FP, (r2 << 3) + 8);
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
    l = 0xf;
    if ((int)r0 < 4)
	/* bogus extra push to align at 8 bytes */
	l = (l & ~(1 << r0)) | 0x10;
    _PUSH(l);
    _LDRIN(_R0, JIT_FP, (r1 << 3) + 8);
    _LDRIN(_R2, JIT_FP, (r2 << 3) + 8);
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

#define jit_extr_d_f(r0, r1)		arm_extr_d_f(_jit, r0, r1)
static void
arm_extr_d_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    int			d;
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

#define jit_movr_f(r0, r1)		arm_movr_f(_jit, r0, r1)
__jit_inline void
arm_movr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	_LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    }
}

#define jit_movr_d(r0, r1)		arm_movr_d(_jit, r0, r1)
__jit_inline void
arm_movr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	if (jit_armv5_p()) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	}
	else {
	    _LDRIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 4);
	    _STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
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
    jit_movi_i(JIT_FTMP, u.i);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
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
    jit_movi_i(JIT_FTMP, u.i[0]);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    jit_movi_i(JIT_FTMP, u.i[1]);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
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

#define jit_ldr_f(r0, r1)		arm_ldr_f(_jit, r0, r1)
__jit_inline void
arm_ldr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    jit_ldr_i(JIT_FTMP, r1);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldr_d(r0, r1)		arm_ldr_d(_jit, r0, r1)
__jit_inline void
arm_ldr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
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
#if 0
    _LDRDI(r0, r1, 0);
#endif
}

#define jit_ldi_f(r0, i0)		arm_ldi_f(_jit, r0, i0)
__jit_inline void
arm_ldi_f(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    jit_ldi_i(JIT_FTMP, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldi_d(r0, i0)		arm_ldi_d(_jit, r0, i0)
__jit_inline void
arm_ldi_d(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
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
#if 0
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRDI(r0, JIT_TMP, 0);
#endif
}

#define jit_ldxr_f(r0, r1, r2)		arm_ldxr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDR(JIT_TMP, r1, r2);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldxr_d(r0, r1, r2)		arm_ldxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (jit_armv5_p()) {
	_LDRD(JIT_TMP, r1, r2);
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	jit_addi_i(JIT_TMP, r1, r2);
	_LDR(JIT_FTMP, JIT_TMP, 4);
	_LDR(JIT_TMP, JIT_TMP, 0);
	_STRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	_STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    }
#if 0
    _LDRD(r0, r1, r2);
#endif
}

#define jit_ldxi_f(r0, r1, i0)		arm_ldxi_f(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    jit_ldxi_i(JIT_FTMP, r1, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_ldxi_d(r0, r1, i0)		arm_ldxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
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
#if 0
    jit_fpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRDI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRDIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRD(r0, r1, reg);
    }
#endif
}

#define jit_str_f(r0, r1)		arm_str_f(_jit, r0, r1)
__jit_inline void
arm_str_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    jit_str_i(r0, JIT_FTMP);
}

#define jit_str_d(r0, r1)		arm_str_d(_jit, r0, r1)
__jit_inline void
arm_str_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
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
#if 0
    _STRDI(r1, r0, 0);
#endif
}

#define jit_sti_f(r0, i0)		arm_sti_f(_jit, r0, i0)
__jit_inline void
arm_sti_f(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    jit_sti_i(i0, JIT_FTMP);
}

#define jit_sti_d(r0, i0)		arm_sti_d(_jit, r0, i0)
__jit_inline void
arm_sti_d(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    _STRI(JIT_FTMP, JIT_TMP, 0);
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
    _STRI(JIT_FTMP, JIT_TMP, 4);
#if 0
    jit_movi_i(JIT_TMP, (int)i0);
    _STRDI(r0, JIT_TMP, 0);
#endif
}

#define jit_stxr_f(r0, r1, r2)		arm_stxr_f(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_f(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r2 << 3) + 8);
    jit_stxr_i(r0, r1, JIT_FTMP);
}

#define jit_stxr_d(r0, r1, r2)		arm_stxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
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
#if 0
    _STRD(r2, r0, r1);
#endif
}

#define jit_stxi_f(r0, r1, i0)		arm_stxi_f(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_f(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    _LDRIN(JIT_FTMP, JIT_FP, (r1 << 3) + 8);
    jit_stxi_i(i0, r0, JIT_FTMP);
}

#define jit_stxi_d(r0, r1, i0)		arm_stxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    if (jit_armv5_p()) {
	if (i0 >= 0 && i0 <= 255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDI(JIT_TMP, r0, i0);
	}
	else if (i0 < 0 && i0 >= -255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r1 << 3) + 8);
	    _STRDIN(JIT_TMP, r1, -i0);
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
#if 0
    jit_fpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_STRDI(r1, r0, i0);
    else if (i0 < 0 && i0 >= -255)
	_STRDIN(r1, r0, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRD(r1, r0, reg);
    }
#endif
}

#define jit_prolog_f(i0)		do {} while (0)
#define jit_prolog_d(i0)		do {} while (0)

#define jit_prepare_f(i0)		arm_prepare_f(_jit, i0)
__jit_inline void
arm_prepare_f(jit_state_t _jit, int i0)
{
    assert(i0 >= 0);
    _jitl.nextarg_put += i0;
    arm_prepare_adjust(_jit);
}

#define jit_prepare_d(i0)		arm_prepare_d(_jit, i0)
__jit_inline void
arm_prepare_d(jit_state_t _jit, int i0)
{
    assert(i0 >= 0);
    _jitl.nextarg_put += i0 << 1;
    arm_prepare_adjust(_jit);
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
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(double);
    }
    _jitl.nextarg_get += 2;
    return (ofs);
}

#define jit_getarg_f(f0, i0)		arm_getarg_f(_jit, f0, i0)
__jit_inline void
arm_getarg_f(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (i0 < 4)
	jit_ldxi_i(JIT_FTMP, JIT_FP, i0 << 2);
    else
	jit_ldxi_i(JIT_FTMP, JIT_FP, i0);
    _STRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
}

#define jit_getarg_d(f0, i0)		arm_getarg_d(_jit, f0, i0)
__jit_inline void
arm_getarg_d(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (jit_armv5_p()) {
	if (i0 < 4)
	    _LDRDI(JIT_TMP, JIT_FP, i0 << 2);
	else if (i0 < 255)
	    _LDRDI(JIT_TMP, JIT_FP, i0);
	else {
	    jit_addi_i(JIT_TMP, JIT_FP, i0);
	    _LDRI(JIT_FTMP, JIT_TMP, 4);
	    _LDRI(JIT_TMP, JIT_TMP, 0);
	}
	_STRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
    }
    else {
	if (i0 < 4) {
	    _LDRI(JIT_TMP, JIT_FP, i0 << 2);
	    _LDRI(JIT_FTMP, JIT_FP, (i0 << 2) + 4);
	}
	else if (i0 + 4 < 255) {
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
    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
    if (_jitl.alignhack == 1)
	--_jitl.nextarg_put;
    if (--_jitl.nextarg_put < 4)
	jit_stxi_i(_jitl.nextarg_put << 2, JIT_FP, JIT_FTMP);
    else {
	_jitl.stack_offset -= sizeof(float);
	jit_stxi_i(_jitl.stack_offset, JIT_SP, JIT_FTMP);
    }
    _jitl.alignhack = 2;
}

#define jit_pusharg_d(r0)		arm_pusharg_d(_jit, r0)
__jit_inline void
arm_pusharg_d(jit_state_t _jit, jit_fpr_t r0)
{
    if (_jitl.nextarg_put & 1) {
	if (_jitl.alignhack)
	    assert(!"only one misalignment patched!\n");
	/* ugly hack */
	++_jitl.nextarg_put;
	arm_prepare_adjust(_jit);
	_jitl.alignhack = 1;
    }
    _jitl.nextarg_put -= 2;
    if (_jitl.nextarg_put < 4) {
	if (jit_armv5_p()) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	    _STRDI(JIT_TMP, JIT_FP, _jitl.nextarg_put << 2);
	}
	else {
	    _LDRIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
	    _STRI(JIT_TMP, JIT_FP, _jitl.nextarg_put << 2);
	    _STRI(JIT_FTMP, JIT_FP, (_jitl.nextarg_put << 2) + 4);
	}
    }
    else {
	_jitl.stack_offset -= sizeof(double);
	if (jit_armv5_p() && _jitl.stack_offset <= 255) {
	    _LDRDIN(JIT_TMP, JIT_FP, (r0 << 3) + 8);
	    _STRDI(JIT_TMP, JIT_SP, _jitl.stack_offset);
	}
	else {
	    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 8);
	    jit_stxi_i(_jitl.stack_offset, JIT_SP, JIT_FTMP);
	    _LDRIN(JIT_FTMP, JIT_FP, (r0 << 3) + 4);
	    jit_stxi_i(_jitl.stack_offset + 4, JIT_SP, JIT_FTMP);
	}
    }
}

#endif /* __lightning_fp_arm_h */
