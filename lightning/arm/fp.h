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

/*
 * FIXME should use another register for JIT_TMP (_r11?) and _R8/_R9
 * for softfp?
 */
#define jit_movi_f(r0, i0)		arm_movi_f(_jit, r0, i0)
__jit_inline void
arm_movi_f(jit_state_t _jit, jit_fpr_t r0, float i0)
{
    union {
	int	i;
	float	f;
    } u;
    assert(jit_cpu.softfp);
    u.f = i0;
    jit_movi_i(JIT_FTMP, u.i);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
}

#define jit_ldr_f(r0, r1)		arm_ldr_f(_jit, r0, r1)
__jit_inline void
arm_ldr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    assert(jit_cpu.softfp);
    jit_ldr_i(JIT_FTMP, r1);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
}

#define jit_ldi_f(r0, i0)		arm_ldi_f(_jit, r0, i0)
__jit_inline void
arm_ldi_f(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    assert(jit_cpu.softfp);
    jit_ldi_i(JIT_FTMP, i0);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
}

#define jit_ldxr_f(r0, r1, r2)		arm_ldxr_f(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(jit_cpu.softfp);
    jit_ldxr_i(JIT_FTMP, r1, r2);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
}

#define jit_ldxi_f(r0, r1, i0)		arm_ldxi_f(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, r1, i0);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
}

#define jit_str_f(r0, r1)		arm_str_f(_jit, r0, r1)
__jit_inline void
arm_str_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4));
    jit_str_i(r0, JIT_FTMP);
}

#define jit_sti_f(r0, i0)		arm_sti_f(_jit, r0, i0)
__jit_inline void
arm_sti_f(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r0 << 4));
    jit_sti_i(i0, JIT_FTMP);
}

#define jit_stxr_f(r0, r1, r2)		arm_stxr_f(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_f(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r2 << 4));
    jit_stxr_i(r0, r1, JIT_FTMP);
}

#define jit_stxi_f(r0, r1, i0)		arm_stxi_f(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_f(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4));
    jit_stxi_i(i0, r0, JIT_FTMP);
}

#define jit_movi_d(r0, i0)		arm_movi_d(_jit, r0, i0)
__jit_inline void
arm_movi_d(jit_state_t _jit, jit_fpr_t r0, double i0)
{
    union {
	int	i[2];
	double	d;
    } u;
    assert(jit_cpu.softfp);
    u.d = i0;
    jit_movi_i(JIT_FTMP, u.i[0]);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
    jit_movi_i(JIT_FTMP, u.i[1]);
    jit_stxi_i(-(r0 << 4) + 4, JIT_FP, JIT_FTMP);
}

#define jit_ldr_d(r0, r1)		arm_ldr_d(_jit, r0, r1)
__jit_inline void
arm_ldr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    assert(jit_cpu.softfp);
    jit_ldr_i(JIT_FTMP, r1);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
    jit_ldxi_i(JIT_FTMP, r1, 4);
    jit_stxi_i(-(r0 << 4) + 4, JIT_FP, JIT_FTMP);
#if 0
    _LDRDI(r0, r1, 0);
#endif
}

#define jit_ldi_d(r0, i0)		arm_ldi_d(_jit, r0, i0)
__jit_inline void
arm_ldi_d(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    assert(jit_cpu.softfp);
    jit_movi_i(JIT_TMP, (int)i0);
    jit_ldr_i(JIT_FTMP, JIT_TMP);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
    jit_ldxi_i(JIT_FTMP, JIT_TMP, 4);
    jit_stxi_i(-(r0 << 4) + 4, JIT_FP, JIT_FTMP);
#if 0
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRDI(r0, JIT_TMP, 0);
#endif
}

#define jit_ldxr_d(r0, r1, r2)		arm_ldxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(jit_cpu.softfp);
    jit_ldxr_i(JIT_FTMP, r1, r2);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
    jit_addi_i(JIT_TMP, r2, 4);
    jit_ldxr_i(JIT_FTMP, r1, JIT_TMP);
    jit_stxi_i(-(r0 << 4) + 4, JIT_FP, JIT_FTMP);
#if 0
    _LDRD(r0, r1, r2);
#endif
}

#define jit_ldxi_d(r0, r1, i0)		arm_ldxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, r1, i0);
    jit_stxi_i(-(r0 << 4), JIT_FP, JIT_FTMP);
    jit_ldxr_i(JIT_FTMP, r1, i0 + 4);
    jit_stxi_i(-(r0 << 4) + 4, JIT_FP, JIT_FTMP);
#if 0
    jit_fpr_t		reg;
    assert(jit_cpu.softfp);
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

#define jit_str_d(r0, r1)		arm_str_d(_jit, r0, r1)
__jit_inline void
arm_str_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4));
    jit_str_i(r0, JIT_FTMP);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4) + 4);
    jit_stxi_i(4, r0, JIT_FTMP);
#if 0
    _STRDI(r0, r1, 0);
#endif
}

#define jit_sti_d(r0, i0)		arm_sti_d(_jit, r0, i0)
__jit_inline void
arm_sti_d(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r0 << 4));
    jit_movi_i(JIT_TMP, (int)i0);
    jit_str_i(JIT_TMP, JIT_FTMP);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r0 << 4) + 4);
    jit_stxr_i(4, JIT_TMP, JIT_FTMP);
#if 0
    jit_movi_i(JIT_TMP, (int)i0);
    _STRDI(JIT_TMP, r0, 0);
#endif
}

#define jit_stxr_d(r0, r1, r2)		arm_stxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4));
    jit_stxr_i(r0, r1, JIT_FTMP);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4) + 4);
    jit_addi_i(JIT_TMP, r1, 4);
    jit_stxr_i(r0, JIT_TMP, JIT_FTMP);
#if 0
    _STRD(r0, r1, r2);
#endif
}

#define jit_stxi_d(r0, r1, i0)		arm_stxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    assert(jit_cpu.softfp);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4));
    jit_stxi_i(i0, r0, JIT_FTMP);
    jit_ldxi_i(JIT_FTMP, JIT_FP, -(r1 << 4) + 4);
    jit_stxi_i(i0 + 4, r0, JIT_FTMP);
#if 0
    jit_fpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_STRDI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_STRDIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRD(r0, r1, reg);
    }
#endif
}

#endif /* __lightning_fp_arm_h */
