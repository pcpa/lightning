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

/*
 * FIXME Just the basic logic for now; if not using a vfp, need to
 * also assert the registers are consecutive for load/store.
 * Probably should map float registers to stack offsets, and use
 * _R8/_R9 when a "double" is required.
 */
#define jit_ldr_d(r0, r1)		arm_ldr_d(_jit, r0, r1)
__jit_inline void
arm_ldr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LDRDI(r0, r1, 0);
}

#define jit_ldi_d(r0, i0)		arm_ldi_d(_jit, r0, i0)
__jit_inline void
arm_ldi_d(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRDI(r0, JIT_TMP, 0);
}

#define jit_ldxr_d(r0, r1, r2)		arm_ldxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRD(r0, r1, r2);
}

#define jit_ldxi_d(r0, r1, i0)		arm_ldxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRDI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRDIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRD(r0, r1, reg);
    }
}

#define jit_str_d(r0, r1)		arm_str_d(_jit, r0, r1)
__jit_inline void
arm_str_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _STRDI(r0, r1, 0);
}

#define jit_sti_d(r0, i0)		arm_sti_d(_jit, r0, i0)
__jit_inline void
arm_sti_d(jit_state_t _jit, void *i0, jit_gpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _STRDI(JIT_TMP, r0, 0);
}

#define jit_stxr_d(r0, r1, r2)		arm_stxr_d(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STRD(r0, r1, r2);
}

#define jit_stxi_d(r0, r1, i0)		arm_stxi_d(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_STRDI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_STRDIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRD(r0, r1, reg);
    }
}

#endif /* __lightning_fp_arm_h */
