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
#define JIT_RTEMP			_AT

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

#define JIT_A_NUM			4
static const jit_gpr_t
jit_a_order[JIT_A_NUM] = {
    _A0, _A1, _A2, _A3
};

#define jit_nop(n)			mips_nop(_jit, n)
__jit_inline void
mips_nop(jit_state_t _jit, int n)
{
    while (n--)
	_jit_I(0);
}

#define jit_movr_i(r0, r1)		mips_movr_i(_jit, r0, r1)
__jit_inline void
mips_movr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1)
	_OR(r0, r1, JIT_RZERO);
}

#define jit_movi_i(r0, i0)		mips_movi_i(_jit, r0, i0)
__jit_inline void
mips_movi_i(jit_state_t _jit, jit_gpr_t r0, int i0)
{
    if (i0 == 0)
	_XOR(r0, r0, r0);
    else if (_s16P(i0))
	_ADDIU(r0, JIT_RZERO, i0);
    else {
	_LUI(r0, (unsigned)i0 >> 16);
	if (i0 & 0xffff)
	    _ORI(r0, r0, i0 & 0xffff);
    }
}

#define jit_movi_p(r0, i0)		mips_movi_p(_jit, r0, i0)
__jit_inline void
mips_movi_p(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    unsigned int	im = (unsigned int)i0;

    _LUI(r0, im >> 16);
    _ORI(r0, r0, im & 0xffff);
}

#define jit_negr_i(r0, r1)		mips_negr_i(_jit, r0, r1)
__jit_inline void
mips_negr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _SUBU(r0, JIT_RZERO, r1);
}

#define jit_addr_i(r0, r1, r2)		mips_addr_i(_jit, r0, r1, r2)
__jit_inline void
mips_addr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADDU(r0, r1, r2);
}

#define jit_addi_i(r0, r1, i0)		mips_addi_i(_jit, r0, r1, i0)
__jit_inline void
mips_addi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	_ADDIU(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_addr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_subr_i(r0, r1, r2)		mips_subr_i(_jit, r0, r1, r2)
__jit_inline void
mips_subr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SUBU(r0, r1, r2);
}

#define jit_subi_i(r0, r1, i0)		mips_subi_i(_jit, r0, r1, i0)
__jit_inline void
mips_subi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	_ADDIU(r0, r1, -i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_subr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_mulr_i(r0, r1, r2)		mips_mulr_i(_jit, r0, r1, r2)
__jit_inline void
mips_mulr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
#if 1
    _MULTU(r1, r2);
    _MFLO(r0);
#else
    /* FIXME matches bits in MIPS32 documentation but not
     * disassembled by gdb in loongson mips64 */
    _MUL(r0, r1, r2);
#endif
}

#define jit_muli_i(r0, r1, i0)		mips_muli_i(_jit, r0, r1, i0)
__jit_inline void
mips_muli_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_mulr_i(r0, r1, JIT_RTEMP);
}

#define jit_hmulr_i(r0, r1, r2)		mips_hmulr_i(_jit, r0, r1, r2)
__jit_inline void
mips_hmulr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _MULT(r1, r2);
    _MFHI(r0);
}

#define jit_hmuli_i(r0, r1, i0)		mips_hmuli_i(_jit, r0, r1, i0)
__jit_inline void
mips_hmuli_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_hmulr_i(r0, r1, JIT_RTEMP);
}

#define jit_hmulr_ui(r0, r1, r2)	mips_hmulr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_hmulr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _MULTU(r1, r2);
    _MFHI(r0);
}

#define jit_hmuli_ui(r0, r1, i0)	mips_hmuli_ui(_jit, r0, r1, i0)
__jit_inline void
mips_hmuli_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_hmulr_ui(r0, r1, JIT_RTEMP);
}

#define jit_divr_i(r0, r1, r2)		mips_divr_i(_jit, r0, r1, r2)
__jit_inline void
mips_divr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DIV(r1, r2);
    _MFLO(r0);
}

#define jit_divi_i(r0, r1, i0)		mips_divi_i(_jit, r0, r1, i0)
__jit_inline void
mips_divi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_divr_i(r0, r1, JIT_RTEMP);
}

#define jit_divr_ui(r0, r1, r2)		mips_divr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_divr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DIVU(r1, r2);
    _MFLO(r0);
}

#define jit_divi_ui(r0, r1, i0)		mips_divi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_divi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_divr_ui(r0, r1, JIT_RTEMP);
}

#define jit_modr_i(r0, r1, r2)		mips_modr_i(_jit, r0, r1, r2)
__jit_inline void
mips_modr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DIV(r1, r2);
    _MFHI(r0);
}

#define jit_modi_i(r0, r1, i0)		mips_modi_i(_jit, r0, r1, i0)
__jit_inline void
mips_modi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_modr_i(r0, r1, JIT_RTEMP);
}

#define jit_modr_ui(r0, r1, r2)		mips_modr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_modr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DIVU(r1, r2);
    _MFHI(r0);
}

#define jit_modi_ui(r0, r1, i0)		mips_modi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_modi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_movi_i(JIT_RTEMP, i0);
    jit_modr_ui(r0, r1, JIT_RTEMP);
}

#define jit_andr_i(r0, r1, r2)		mips_andr_i(_jit, r0, r1, r2)
__jit_inline void
mips_andr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _AND(r0, r1, r2);
}

#define jit_andi_i(r0, r1, i0)		mips_andi_i(_jit, r0, r1, i0)
__jit_inline void
mips_andi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_u16P(i0))
	_ANDI(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_andr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_orr_i(r0, r1, r2)		mips_orr_i(_jit, r0, r1, r2)
__jit_inline void
mips_orr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _OR(r0, r1, r2);
}

#define jit_ori_i(r0, r1, i0)		mips_ori_i(_jit, r0, r1, i0)
__jit_inline void
mips_ori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_u16P(i0))
	_ORI(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_orr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_xorr_i(r0, r1, r2)		mips_xorr_i(_jit, r0, r1, r2)
__jit_inline void
mips_xorr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _XOR(r0, r1, r2);
}

#define jit_xori_i(r0, r1, i0)		mips_xori_i(_jit, r0, r1, i0)
__jit_inline void
mips_xori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_u16P(i0))
	_XORI(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_xorr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_lshr_i(r0, r1, r2)		mips_lshr_i(_jit, r0, r1, r2)
__jit_inline void
mips_lshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_SLLV);
}

#define jit_lshi_i(r0, r1, i0)		mips_lshi_i(_jit, r0, r1, i0)
__jit_inline void
mips_lshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 31);
    mips__rrit(_jit, r1, r0, i0, MIPS_SLL);
}

#define jit_rshr_i(r0, r1, r2)		mips_rshr_i(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_SRAV);
}

#define jit_rshi_i(r0, r1, i0)		mips_rshi_i(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 31);
    mips__rrit(_jit, r1, r0, i0, MIPS_SRA);
}

#define jit_rshr_ui(r0, r1, r2)		mips_rshr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_SRLV);
}

#define jit_rshi_ui(r0, r1, i0)		mips_rshi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 31);
    mips__rrit(_jit, r1, r0, i0, MIPS_SRL);
}

#define jit_ltr_i(r0, r1, r2)		mips_ltr_i(_jit, r0, r1, r2)
__jit_inline void
mips_ltr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SLT(r0, r1, r2);
}

#define jit_lti_i(r0, r1, i0)		mips_lti_i(_jit, r0, r1, i0)
__jit_inline void
mips_lti_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	_SLTI(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_ltr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_ltr_ui(r0, r1, i0)		mips_ltr_ui(_jit, r0, r1, i0)
__jit_inline void
mips_ltr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SLTU(r0, r1, r2);
}

#define jit_lti_ui(r0, r1, i0)		mips_lti_ui(_jit, r0, r1, i0)
__jit_inline void
mips_lti_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    /* value is sign extended */
    if (i0 <= 0x7fff || (unsigned)i0 >= 0xffff8000)
	_SLTIU(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_ltr_ui(r0, r1, JIT_RTEMP);
    }
}

#define jit_ler_i(r0, r1, r2)		mips_ler_i(_jit, r0, r1, r2)
__jit_inline void
mips_ler_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* _at = r1 - r2; */
    jit_subr_i(JIT_RTEMP, r1, r2);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_lei_i(r0, r1, i0)		mips_lei_i(_jit, r0, r1, i0)
__jit_inline void
mips_lei_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    /* _at = r1 - i0; */
    jit_subi_i(JIT_RTEMP, r1, i0);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_ler_ui(r0, r1, r2)		mips_ler_ui(_jit, r0, r1, r2)
__jit_inline void
mips_ler_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* _at = r1 - r2; */
    jit_subr_ui(JIT_RTEMP, r1, r2);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_lei_ui(r0, r1, i0)		mips_ler_ui(_jit, r0, r1, i0)
__jit_inline void
mips_lei_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    /* _at = r1 - i0; */
    jit_subi_ui(JIT_RTEMP, r1, i0);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_eqr_i(r0, r1, r2)		mips_eqr_i(_jit, r0, r1, r2)
__jit_inline void
mips_eqr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* r0 = r1 - r2; */
    jit_subr_i(r0, r1, r2);
    /* _at = 1; */
    _ORI(JIT_RTEMP, JIT_RZERO, 1);
    /* if (r0) r0 = _at; */
    mips_rrr_t(_jit, JIT_RTEMP, r0, r0, MIPS_MOVN);
}

#define jit_eqi_i(r0, r1, i0)		mips_eqi_i(_jit, r0, r1, i0)
__jit_inline void
mips_eqi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0)
	/* r0 = r1 - i0; */
	jit_subi_i(r0, r1, i0);
    else
	/* r0 = 0 < (uint32_t)r0; */
	_SLTU(r0, JIT_RZERO, r0);
    /* _at = 1; */
    _ORI(JIT_RTEMP, JIT_RZERO, 1);
    /* if (r0) r0 = _at; */
    mips_rrr_t(_jit, JIT_RTEMP, r0, r0, MIPS_MOVN);
}

#define jit_ger_i(r0, r1, r2)		mips_ger_i(_jit, r0, r1, r2)
__jit_inline void
mips_ger_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SLT(r0, r1, r2);
}

#define jit_gei_i(r0, r1, i0)		mips_gei_i(_jit, r0, r1, i0)
__jit_inline void
mips_gei_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	_SLTI(r0, r1, -i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_ger_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_ger_ui(r0, r1, i0)		mips_ger_ui(_jit, r0, r1, i0)
__jit_inline void
mips_ger_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SLTU(r0, r1, r2);
}

#define jit_lti_ui(r0, r1, i0)		mips_lti_ui(_jit, r0, r1, i0)
__jit_inline void
mips_gei_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    i0 = -i0;
    /* value is sign extended */
    if (i0 <= 0x7fff || (unsigned)i0 >= 0xffff8000)
	_SLTIU(r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_ger_ui(r0, r1, JIT_RTEMP);
    }
}

#define jit_gtr_i(r0, r1, r2)		mips_gtr_i(_jit, r0, r1, r2)
__jit_inline void
mips_gtr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* _at = r1 - r2; */
    jit_subr_i(JIT_RTEMP, r1, r2);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_gti_i(r0, r1, i0)		mips_gti_i(_jit, r0, r1, i0)
__jit_inline void
mips_gti_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    /* _at = r1 - i0; */
    jit_subi_i(JIT_RTEMP, r1, i0);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_gtr_ui(r0, r1, r2)		mips_gtr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_gtr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* _at = r1 - r2; */
    jit_subr_ui(JIT_RTEMP, r1, r2);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_gti_ui(r0, r1, i0)		mips_gtr_ui(_jit, r0, r1, i0)
__jit_inline void
mips_gti_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    /* _at = r1 - i0; */
    jit_subi_ui(JIT_RTEMP, r1, i0);
    /* r0 = 0 < at */
    _SLT(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_ner_i(r0, r1, r2)		mips_ner_i(_jit, r0, r1, r2)
__jit_inline void
mips_ner_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* r0 = r1 - r2; */
    jit_subr_i(r0, r1, r2);
    /* r0 = 0 < (uint32_t)at */
    _SLTU(r0, JIT_RZERO, JIT_RTEMP);
}

#define jit_nei_i(r0, r1, i0)		mips_nei_i(_jit, r0, r1, i0)
__jit_inline void
mips_nei_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0) {
	/* r0 = r1 - i0; */
	jit_subi_i(r0, r1, i0);
	/* r0 = 0 < (uint32_t)at */
	_SLTU(r0, JIT_RZERO, JIT_RTEMP);
    }
    else
	/* r0 = 0 < (uint32_t)r0; */
	_SLTU(r0, JIT_RZERO, r0);
}

#define jit_jmpr(i0)			mips_jmpr(_jit, i0)
__jit_inline void
mips_jmpr(jit_state_t _jit, jit_gpr_t r0)
{
    mips_r_it(_jit, r0, 0, MIPS_JR);
    jit_nop(1);
}

#define jit_bltr_i(i0, r0, r1)		mips_bltr_i(_jit, i0, r0, r1)
__jit_inline void
mips_bltr_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_i(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BLTZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_blti_i(i0, r0, i1)		mips_blti_i(_jit, i0, r0, i1)
__jit_inline void
mips_blti_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bltr_i(i0, r0, JIT_RTEMP);
}

#define jit_bltr_ui(i0, r0, r1)		mips_bltr_ui(_jit, i0, r0, r1)
__jit_inline void
mips_bltr_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_ui(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BLTZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_blti_ui(i0, r0, i1)		mips_blti_ui(_jit, i0, r0, i1)
__jit_inline void
mips_blti_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bltr_ui(i0, r0, JIT_RTEMP);
}

#define jit_bler_i(i0, r0, r1)		mips_bler_i(_jit, i0, r0, r1)
__jit_inline void
mips_bler_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_i(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BLEZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_blei_i(i0, r0, i1)		mips_blei_i(_jit, i0, r0, i1)
__jit_inline void
mips_blei_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bler_i(i0, r0, JIT_RTEMP);
}

#define jit_bler_ui(i0, r0, r1)		mips_bler_ui(_jit, i0, r0, r1)
__jit_inline void
mips_bler_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_ui(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BLEZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_blei_ui(i0, r0, i1)		mips_blei_ui(_jit, i0, r0, i1)
__jit_inline void
mips_blei_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bler_ui(i0, r0, JIT_RTEMP);
}

#define jit_beqr_i(i0, r0, r1)		mips_beqr_i(_jit, i0, r0, r1)
__jit_inline void
mips_beqr_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    if (_siP(18, ds))
	_BEQ(r0, r1, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_beqi_i(i0, r0, i1)		mips_beqi_i(_jit, i0, r0, i1)
__jit_inline void
mips_beqi_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_beqr_i(i0, r0, JIT_RTEMP);
}

#define jit_bger_i(i0, r0, r1)		mips_bger_i(_jit, i0, r0, r1)
__jit_inline void
mips_bger_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_i(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BGEZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_bgei_i(i0, r0, i1)		mips_bgei_i(_jit, i0, r0, i1)
__jit_inline void
mips_bgei_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bger_i(i0, r0, JIT_RTEMP);
}

#define jit_bger_ui(i0, r0, r1)		mips_bger_ui(_jit, i0, r0, r1)
__jit_inline void
mips_bger_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_ui(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BGEZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_bgei_ui(i0, r0, i1)		mips_bgei_ui(_jit, i0, r0, i1)
__jit_inline void
mips_bgei_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bger_ui(i0, r0, JIT_RTEMP);
}

#define jit_bgtr_i(i0, r0, r1)		mips_bgtr_i(_jit, i0, r0, r1)
__jit_inline void
mips_bgtr_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_i(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BGTZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_bgti_i(i0, r0, i1)		mips_bgti_i(_jit, i0, r0, i1)
__jit_inline void
mips_bgti_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bgtr_i(i0, r0, JIT_RTEMP);
}

#define jit_bgtr_ui(i0, r0, r1)		mips_bgtr_ui(_jit, i0, r0, r1)
__jit_inline void
mips_bgtr_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    jit_subr_ui(JIT_RTEMP, r0, r1);
    if (_siP(18, ds))
	_BGTZ(JIT_RTEMP, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_bgti_ui(i0, r0, i1)		mips_bgti_ui(_jit, i0, r0, i1)
__jit_inline void
mips_bgti_ui(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bgtr_ui(i0, r0, JIT_RTEMP);
}

#define jit_bner_i(i0, r0, r1)		mips_bner_i(_jit, i0, r0, r1)
__jit_inline void
mips_bner_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    if (_siP(18, ds))
	_BNE(r0, r1, ds >> 2);
    /* FIXME else jump to register */
}

#define jit_bnei_i(i0, r0, i1)		mips_bnei_i(_jit, i0, r0, i1)
__jit_inline void
mips_bnei_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_movi_i(JIT_RTEMP, i1);
    jit_bner_i(i0, r0, JIT_RTEMP);
}

#define jit_ldr_c(r0, r1)		mips_ldr_x(_jit, MIPS_LB,  r0, r1)
#define jit_ldr_uc(r0, r1)		mips_ldr_x(_jit, MIPS_LBU, r0, r1)
#define jit_ldr_s(r0, r1)		mips_ldr_x(_jit, MIPS_LH,  r0, r1)
#define jit_ldr_us(r0, r1)		mips_ldr_x(_jit, MIPS_LHU, r0, r1)
#define jit_ldr_i(r0, r1)		mips_ldr_x(_jit, MIPS_LW,  r0, r1)
__jit_inline void
mips_ldr_x(jit_state_t _jit, mips_hc_t hc, jit_gpr_t r0, jit_gpr_t r1)
{
    mipshrri(_jit, hc, r0, r1, 0);
}

#define jit_ldi_c(r0, i0)		mips_ldi_x(_jit, MIPS_LB,  r0, i0)
#define jit_ldi_uc(r0, i0)		mips_ldi_x(_jit, MIPS_LBU, r0, i0)
#define jit_ldi_s(r0, i0)		mips_ldi_x(_jit, MIPS_LH,  r0, i0)
#define jit_ldi_us(r0, i0)		mips_ldi_x(_jit, MIPS_LHU, r0, i0)
#define jit_ldi_i(r0, i0)		mips_ldi_x(_jit, MIPS_LW,  r0, i0)
__jit_inline void
mips_ldi_x(jit_state_t _jit, mips_hc_t hc, jit_gpr_t r0, void *i0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	mipshrri(_jit, hc, r0, JIT_RZERO, ds);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	mipshrri(_jit, hc, r0, JIT_RTEMP, 0);
    }
}

#define jit_ldxr_c(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LB,  r0, r1, r2)
#define jit_ldxr_uc(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LBU, r0, r1, r2)
#define jit_ldxr_s(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LH,  r0, r1, r2)
#define jit_ldxr_us(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LHU, r0, r1, r2)
#define jit_ldxr_i(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LW,  r0, r1, r2)
__jit_inline void
mips_ldxr_x(jit_state_t _jit, mips_hc_t hc,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, hc, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_c(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LB,  r0, r1, i0)
#define jit_ldxi_uc(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LBU, r0, r1, i0)
#define jit_ldxi_s(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LH,  r0, r1, i0)
#define jit_ldxi_us(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LHU, r0, r1, i0)
#define jit_ldxi_i(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LW,  r0, r1, i0)
__jit_inline void
mips_ldxi_x(jit_state_t _jit, mips_hc_t hc,
	    jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	mipshrri(_jit, hc, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, hc, r0, JIT_RTEMP, 0);
    }
}

#define jit_str_c(r0, r1)		mips_str_x(_jit, MIPS_SB, r0, r1)
#define jit_str_s(r0, r1)		mips_str_x(_jit, MIPS_SH, r0, r1)
#define jit_str_i(r0, r1)		mips_str_x(_jit, MIPS_SW, r0, r1)
__jit_inline void
mips_str_x(jit_state_t _jit, mips_hc_t hc, jit_gpr_t r0, jit_gpr_t r1)
{
    mipshrri(_jit, hc, r1, r0, 0);
}

#define jit_sti_c(i0, r0)		mips_sti_x(_jit, MIPS_SB, i0, r0)
#define jit_sti_s(i0, r0)		mips_sti_x(_jit, MIPS_SH, i0, r0)
#define jit_sti_i(i0, r0)		mips_sti_x(_jit, MIPS_SW, i0, r0)
__jit_inline void
mips_sti_x(jit_state_t _jit, mips_hc_t hc, void *i0, jit_gpr_t r0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	mipshrri(_jit, hc, r0, JIT_RZERO, ds);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	mipshrri(_jit, hc, r0, JIT_RTEMP, 0);
    }
}

#define jit_stxr_c(r0, r1, r2)		mips_stxr_x(_jit, MIPS_SB, r0, r1, r2)
#define jit_stxr_s(r0, r1, r2)		mips_stxr_x(_jit, MIPS_SH, r0, r1, r2)
#define jit_stxr_i(r0, r1, r2)		mips_stxr_x(_jit, MIPS_SW, r0, r1, r2)
__jit_inline void
mips_stxr_x(jit_state_t _jit, mips_hc_t hc,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r0, r1);
    mipshrri(_jit, hc, JIT_RTEMP, r2, 0);
}

#define jit_stxi_c(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SB, i0, r0, r1)
#define jit_stxi_s(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SH, i0, r0, r1)
#define jit_stxi_i(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SW, i0, r0, r1)
__jit_inline void
mips_stxi_x(jit_state_t _jit, mips_hc_t hc,
	    int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (_s16P(i0))
	mipshrri(_jit, hc, r1, r0, i0);
    else {
	jit_addi_i(JIT_RTEMP, r0, i0);
	mipshrri(_jit, hc, JIT_RTEMP, r1, 0);
    }
}

#define jit_extr_c_i(r0, r1)		mips_extr_c_i(_jit, r0, r1)
__jit_inline void
mips_extr_c_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mipsh_rrt(_jit, MIPS_HSEB, r1, r0, MIPS_TSEB);
}

#define jit_extr_c_ui(r0, r1)		mips_extr_c_ui(_jit, r0, r1)
__jit_inline void
mips_extr_c_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mipshrri(_jit, MIPS_ANDI, r0, r1, 0xff);
}

#define jit_extr_s_i(r0, r1)		mips_extr_s_i(_jit, r0, r1)
__jit_inline void
mips_extr_s_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mipsh_rrt(_jit, MIPS_HSEH, r1, r0, MIPS_TSEH);
}

#define jit_extr_s_ui(r0, r1)		mips_extr_s_ui(_jit, r0, r1)
__jit_inline void
mips_extr_s_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mipshrri(_jit, MIPS_ANDI, r0, r1, 0xffff);
}

#define jit_getarg_c(r0, ofs)		mips_getarg_c(_jit, r0, ofs)
__jit_inline void
mips_getarg_c(jit_state_t _jit, jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_A_NUM)
	jit_extr_c_i(r0, jit_a_order[ofs]);
    else
	jit_ldxi_c(r0, JIT_FP, ofs + sizeof(int) - sizeof(char));
}

#define jit_getarg_uc(r0, ofs)		mips_getarg_uc(_jit, r0, ofs)
__jit_inline void
mips_getarg_uc(jit_state_t _jit, jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_A_NUM)
	jit_extr_c_ui(r0, jit_a_order[ofs]);
    else
	jit_ldxi_uc(r0, JIT_FP, ofs + sizeof(int) - sizeof(char));
}

#define jit_getarg_s(r0, ofs)		mips_getarg_s(_jit, r0, ofs)
__jit_inline void
mips_getarg_s(jit_state_t _jit, jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_A_NUM)
	jit_extr_s_i(r0, jit_a_order[ofs]);
    else
	jit_ldxi_s(r0, JIT_FP, ofs + sizeof(int) - sizeof(short));
}

#define jit_getarg_us(r0, ofs)		mips_getarg_us(_jit, r0, ofs)
__jit_inline void
mips_getarg_us(jit_state_t _jit, jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_A_NUM)
	jit_extr_s_ui(r0, jit_a_order[ofs]);
    else
	jit_ldxi_us(r0, JIT_FP, ofs + sizeof(int) - sizeof(short));
}

#define jit_getarg_i(r0, ofs)		mips_getarg_i(_jit, r0, ofs)
__jit_inline void
mips_getarg_i(jit_state_t _jit, jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_A_NUM)
	jit_movr_i(r0, jit_a_order[ofs]);
    else
	jit_ldxi_i(r0, JIT_FP, ofs);
}

#define jit_callr(r0)			mips_callr(_jit, r0)
__jit_inline void
mips_callr(jit_state_t _jit, jit_gpr_t r0)
{
    /* other registers can be used instead of _RA */
    mips_r_rit(_jit, r0, _RA, 0, MIPS_JALR);
    jit_nop(1);
}

#define jit_finishr(rs)			x86_finishr(_jit, rs)
__jit_inline void
mips_finishr(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.stack_offset	== 0 &&
	   _jitl.nextarg_puti	== 0);
    jit_callr(r0);
}

#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_MIPS64 \
	: defined (__mips64)
#  include "core-64.h"
#else
#  include "core-32.h"
#endif

#endif /* __lightning_core_mips_h */
