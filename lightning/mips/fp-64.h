/******************************** -*- C -*- ****************************
 *
 *	Floating-point support (mips64)
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

#ifndef __lightning_fp_h
#define __lightning_fp_h

#define jit_extr_l_f(f0, r0)		mips_extr_l_f(_jit, f0, r0)
__jit_inline void
mips_extr_l_f(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    mips_xrf(_jit, MIPS_fmt_l_if, r0, JIT_FPTMP);
    mips_fp1(_jit, MIPS_fmt_S, f0, JIT_FPTMP, MIPS_CVT_fmt_L);
}

#define jit_extr_l_d(f0, r0)		mips_extr_l_d(_jit, f0, r0)
__jit_inline void
mips_extr_l_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    mips_xrf(_jit, MIPS_fmt_l_if, r0, JIT_FPTMP);
    mips_fp1(_jit, MIPS_fmt_D, f0, JIT_FPTMP, MIPS_CVT_fmt_L);
}

#define jit_roundr_f_l(r0, f0)		mips_roundr_f_l(_jit, r0, f0)
__jit_inline void
mips_roundr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_L, f0, JIT_FPTMP, MIPS_CVT_fmt_S);
    mips_xrf(_jit, MIPS_fmt_dl, r0, JIT_FPTMP);
}

#define jit_roundr_d_l(r0, f0)		mips_roundr_d_l(_jit, r0, f0)
__jit_inline void
mips_roundr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_L, f0, JIT_FPTMP, MIPS_CVT_fmt_D);
    mips_xrf(_jit, MIPS_fmt_dl, r0, JIT_FPTMP);
}

#define jit_truncr_f_l(r0, f0)		mips_truncr_f_l(_jit, r0, f0)
__jit_inline void
mips_truncr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_S, f0, JIT_FPTMP, MIPS_TRUNC_fmt_L);
    mips_xrf(_jit, MIPS_fmt_dl, r0, JIT_FPTMP);
}

#define jit_truncr_d_l(r0, f0)		mips_truncr_d_l(_jit, r0, f0)
__jit_inline void
mips_truncr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_D, f0, JIT_FPTMP, MIPS_TRUNC_fmt_L);
    mips_xrf(_jit, MIPS_fmt_ld, r0, JIT_FPTMP);
}

#define jit_ceilr_f_l(r0, f0)		mips_ceilr_f_l(_jit, r0, f0)
__jit_inline void
mips_ceilr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CEIL_L_S(JIT_FPTMP, f0);
    mips_xrf(_jit, MIPS_fmt_dl, r0, JIT_FPTMP);
}

#define jit_ceilr_d_l(r0, f0)		mips_ceilr_d_l(_jit, r0, f0)
__jit_inline void
mips_ceilr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CEIL_L_D(JIT_FPTMP, f0);
    mips_xrf(_jit, MIPS_fmt_ld, r0, JIT_FPTMP);
}

#define jit_floorr_f_l(r0, f0)		mips_floorr_f_l(_jit, r0, f0)
__jit_inline void
mips_floorr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_S, f0, JIT_FPTMP, MIPS_FLOOR_fmt_L);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_floorr_d_l(r0, f0)		mips_floorr_d_l(_jit, r0, f0)
__jit_inline void
mips_floorr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_D, f0, JIT_FPTMP, MIPS_FLOOR_fmt_L);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#endif /* __lightning_fp_h */
