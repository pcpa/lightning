/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (mips version)
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
 * Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/

#ifndef __lightning_core_mips_h
#define __lightning_core_mips_h

#define JIT_FP				_FP
#define JIT_SP				_SP
#define JIT_RET				_V0
#define JIT_RZERO			_ZERO
#define JIT_TEMP			_AT

#define JIT_R_NUM			8
static const jit_gpr_t
jit_r_order[JIT_R_NUM] = {
    _T0, _T1, _T2, _T3, _T4, _T5, _T6, _T7
};
#define JIT_R(i)			jit_r_order[i]

#define JIT_V_NUM			8
static const jit_gpr_t
jit_v_order[JIT_V_NUM] = {
    _S0, _S1, _S2, _S3, _S4, _S5, _S6, _S7
};
#define JIT_V(i)			jit_v_order[i]

#define jit_movr_i(r0, r1)		mips_movr_i(_jit, r0, r1)
__jit_inline void
mips_movr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1) {
	/* FIXME verify */
	if (r1 == _V0)
	    mips_coder(_jit, MIPS_MFLO, r0);
	else if (r1 == _V1)
	    mips_coder(_jit, MIPS_MFHI, r0);
	else
	    mips_alurrr(_jit, MIPS_OR, r0, JIT_RZERO, r1);
    }
}

#define jit_movi_i(r0, i0)		mips_movi_i(_jit, r0, i0)
__jit_inline void
mips_movi_i(jit_state_t _jit, jit_gpr_t r0, int i0)
{
    mips_coderi(_jit, MIPS_LUI, r0, (unsigned)i0 >> 16);
    mips_coderi(_jit, MIPS_ORI, r0, i0 & 0xffff);
}

#define jit_negr_i(r0, r1)		mips_negr_i(_jit, r0, r1)
__jit_inline void
mips_negr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mips_alurrr(_jit, MIPS_SUBU, r0, JIT_RZERO, r1);
}

#define jit_addr_i(r0, r1, r2)		mips_addr_i(_jit, r0, r1, r2)
__jit_inline void
mips_addr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_ADDU, r0, r1, r2);
}

#define jit_addi_i(r0, r1, i0)		mips_addi_i(_jit, r0, r1, i0)
__jit_inline void
mips_addi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mips_alurri(_jit, MIPS_ADDIU, r0, r1, i0);
    else {
	jit_movi_i(JIT_TEMP, i0);
	jit_addr_i(r0, r1, JIT_TEMP);
    }
}

#define jit_subr_i(r0, r1, r2)		mips_subr_i(_jit, r0, r1, r2)
__jit_inline void
mips_subr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_SUBU, r0, r1, r2);
}

#define jit_subi_i(r0, r1, i0)		mips_subi_i(_jit, r0, r1, i0)
__jit_inline void
mips_subi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mips_alurri(_jit, MIPS_ADDIU, r0, r1, -i0);
    else {
	jit_movi_i(JIT_TEMP, i0);
	jit_subr_i(r0, r1, JIT_TEMP);
    }
}

#define jit_mulr_i(r0, r1, r2)		mips_mulr_i(_jit, r0, r1, r2)
__jit_inline void
mips_mulr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_MULU, JIT_RZERO, r1, r2);
    /* FIXME correct? */
    mips_coder(_jit, MIPS_MFLO, r0);
}

#define jit_muli_i(r0, r1, i0)		mips_muli_i(_jit, r0, r1, i0)
__jit_inline void
mips_muli_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(_AT, i0);
    jit_mulr_i(r0, r1, _AT);
}

#define jit_divr_i(r0, r1, r2)		mips_divr_i(_jit, r0, r1, r2)
__jit_inline void
mips_divr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_DIV, JIT_RZERO, r1, r2);
    /* FIXME correct? */
    mips_coder(_jit, MIPS_MFLO, r0);
}

#define jit_divi_i(r0, r1, i0)		mips_divi_i(_jit, r0, r1, i0)
__jit_inline void
mips_divi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(_AT, i0);
    jit_divr_i(r0, r1, _AT);
}

#define jit_divr_ui(r0, r1, r2)		mips_divr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_divr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_DIVU, JIT_RZERO, r1, r2);
    /* FIXME correct? */
    mips_coder(_jit, MIPS_MFLO, r0);
}

#define jit_divi_ui(r0, r1, i0)		mips_divi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_divi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(_AT, i0);
    jit_divr_ui(r0, r1, _AT);
}


#define jit_modr_i(r0, r1, r2)		mips_modr_i(_jit, r0, r1, r2)
__jit_inline void
mips_modr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_DIV, JIT_RZERO, r1, r2);
    /* FIXME correct? */
    mips_coder(_jit, MIPS_MFHI, r0);
}

#define jit_modi_i(r0, r1, i0)		mips_modi_i(_jit, r0, r1, i0)
__jit_inline void
mips_modi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(_AT, i0);
    jit_modr_i(r0, r1, _AT);
}

#define jit_modr_ui(r0, r1, r2)		mips_modr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_modr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_DIVU, JIT_RZERO, r1, r2);
    /* FIXME correct? */
    mips_coder(_jit, MIPS_MFHI, r0);
}

#define jit_modi_ui(r0, r1, i0)		mips_modi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_modi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(_AT, i0);
    jit_modr_ui(r0, r1, _AT);
}

#define jit_andr_i(r0, r1, r2)		mips_andr_i(_jit, r0, r1, r2)
__jit_inline void
mips_andr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_AND, r0, r1, r2);
}

#define jit_andi_i(r0, r1, i0)		mips_andi_i(_jit, r0, r1, i0)
__jit_inline void
mips_andi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mips_alurri(_jit, MIPS_ANDI, r0, r1, i0);
    else {
	jit_movi_i(JIT_TEMP, i0);
	jit_andr_i(r0, r1, JIT_TEMP);
    }
}

#define jit_orr_i(r0, r1, r2)		mips_orr_i(_jit, r0, r1, r2)
__jit_inline void
mips_orr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_OR, r0, r1, r2);
}

#define jit_ori_i(r0, r1, i0)		mips_ori_i(_jit, r0, r1, i0)
__jit_inline void
mips_ori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mips_alurri(_jit, MIPS_ORI, r0, r1, i0);
    else {
	jit_movi_i(JIT_TEMP, i0);
	jit_orr_i(r0, r1, JIT_TEMP);
    }
}

#define jit_xorr_i(r0, r1, r2)		mips_xorr_i(_jit, r0, r1, r2)
__jit_inline void
mips_xorr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alurrr(_jit, MIPS_XOR, r0, r1, r2);
}

#define jit_xori_i(r0, r1, i0)		mips_xori_i(_jit, r0, r1, i0)
__jit_inline void
mips_xori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mips_alurri(_jit, MIPS_XORI, r0, r1, i0);
    else {
	jit_movi_i(JIT_TEMP, i0);
	jit_xorr_i(r0, r1, JIT_TEMP);
    }
}

#define jit_lshr_i(r0, r1, r2)		mips_lshr_i(_jit, r0, r1, r2)
__jit_inline void
mips_lshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_shiftrrr(_jit, MIPS_SLLV, r0, r1, r2);
}

#define jit_lshi_i(r0, r1, i0)		mips_lshi_i(_jit, r0, r1, i0)
__jit_inline void
mips_lshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    mips_shiftrri(_jit, MIPS_SLL, r0, r1, i0);
}

/* FIXME jit_rshr_i */

#define jit_rshi_i(r0, r1, i0)		mips_rshi_i(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    mips_shiftrri(_jit, MIPS_SRA, r0, r1, i0);
}

#define jit_rshr_ui(r0, r1, r2)		mips_rshr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_shiftrrr(_jit, MIPS_SRLV, r0, r1, r2);
}

#define jit_rshi_ui(r0, r1, i0)		mips_rshi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    mips_shiftrri(_jit, MIPS_SRL, r0, r1, i0);
}

#endif /* __lightning_core_mips_h */
