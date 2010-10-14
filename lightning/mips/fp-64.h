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
    _DMTC1(r0, JIT_FPTMP);
    _CVT_S_L(f0, JIT_FPTMP);
}

#define jit_extr_l_d(f0, r0)		mips_extr_l_d(_jit, f0, r0)
__jit_inline void
mips_extr_l_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    _DMTC1(r0, JIT_FPTMP);
    _CVT_D_L(f0, JIT_FPTMP);
}

#define jit_rintr_f_l(r0, f0)		mips_rintr_f_l(_jit, r0, f0)
__jit_inline void
mips_rintr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CVT_S_L(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_rintr_d_l(r0, f0)		mips_rintr_d_l(_jit, r0, f0)
__jit_inline void
mips_rintr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CVT_D_L(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_roundr_f_l(r0, f0)		mips_roundr_f_l(_jit, r0, f0)
__jit_inline void
mips_roundr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    /* FIXME round to nearest and even on ties... */
    _ROUND_L_S(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_roundr_d_l(r0, f0)		mips_roundr_d_l(_jit, r0, f0)
__jit_inline void
mips_roundr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    /* FIXME round to nearest and even on ties... */
    _ROUND_L_D(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_truncr_f_l(r0, f0)		mips_truncr_f_l(_jit, r0, f0)
__jit_inline void
mips_truncr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _TRUNC_L_S(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_truncr_d_l(r0, f0)		mips_truncr_d_l(_jit, r0, f0)
__jit_inline void
mips_truncr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _TRUNC_L_D(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_ceilr_f_l(r0, f0)		mips_ceilr_f_l(_jit, r0, f0)
__jit_inline void
mips_ceilr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CEIL_L_S(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_ceilr_d_l(r0, f0)		mips_ceilr_d_l(_jit, r0, f0)
__jit_inline void
mips_ceilr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CEIL_L_D(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_floorr_f_l(r0, f0)		mips_floorr_f_l(_jit, r0, f0)
__jit_inline void
mips_floorr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _FLOOR_L_S(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#define jit_floorr_d_l(r0, f0)		mips_floorr_d_l(_jit, r0, f0)
__jit_inline void
mips_floorr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _FLOOR_D_S(JIT_FPTMP, f0);
    _DMFC1(r0, JIT_FPTMP);
}

#endif /* __lightning_fp_h */
