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

#define jit_noop(n)			mips_noop(_jit, n)
__jit_inline void
mips_noop(jit_state_t _jit, int n)
{
    while (n--)
	_jit_I(0);
}

#define jit_movr_i(r0, r1)		mips_movr_i(_jit, r0, r1)
__jit_inline void
mips_movr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1) {
	if (r1 == _V0)
	    mips___r_t(_jit, r0, MIPS_MFLO);
	else if (r1 == _V1)
	    mips___r_t(_jit, r0, MIPS_MFHI);
	else if (r0 == _V0)
	    mips_r___t(_jit, r0, MIPS_MTLO);
	else if (r0 == _V1)
	    mips_r___t(_jit, r0, MIPS_MTHI);
	else
	    mips_rrr_t(_jit, r1, JIT_RZERO, r0, MIPS_OR);
    }
}

#define jit_movi_i(r0, i0)		mips_movi_i(_jit, r0, i0)
__jit_inline void
mips_movi_i(jit_state_t _jit, jit_gpr_t r0, int i0)
{
    if (i0 == 0)
	mips_rrr_t(_jit, r0, r0, r0, MIPS_XOR);
    else {
	mipsh_ri(_jit, MIPS_LUI, r0, (unsigned)i0 >> 16);
	if (i0 & 0xffff)
	    mipsh_ri(_jit, MIPS_ORI, r0, i0 & 0xffff);
    }
}

#define jit_movi_p(r0, i0)		mips_movi_p(_jit, r0, i0)
__jit_inline void
mips_movi_p(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    mipsh_ri(_jit, MIPS_LUI, r0, (unsigned)i0 >> 16);
    mipsh_ri(_jit, MIPS_ORI, r0, (unsigned)i0 & 0xffff);
}

#define jit_negr_i(r0, r1)		mips_negr_i(_jit, r0, r1)
__jit_inline void
mips_negr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mips_rrr_t(_jit, JIT_RZERO, r1, r0, MIPS_SUBU);
}

#define jit_addr_i(r0, r1, r2)		mips_addr_i(_jit, r0, r1, r2)
__jit_inline void
mips_addr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, r0, MIPS_ADDU);
}

#define jit_addi_i(r0, r1, i0)		mips_addi_i(_jit, r0, r1, i0)
__jit_inline void
mips_addi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_ADDIU, r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_addr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_subr_i(r0, r1, r2)		mips_subr_i(_jit, r0, r1, r2)
__jit_inline void
mips_subr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, r0, MIPS_SUBU);
}

#define jit_subi_i(r0, r1, i0)		mips_subi_i(_jit, r0, r1, i0)
__jit_inline void
mips_subi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_ADDIU, r0, r1, -i0);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_MULTU);
    mips___r_t(_jit, r0, MIPS_MFLO);
#else
    /* FIXME matches bits in MIPS32 documentation but not
     * disassembled by gdb in loongson mips64 */
    mipshrrr_t(_jit, MIPS_HMUL, r1, r2, r0, MIPS_TMUL);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_MULT);
    mips___r_t(_jit, r0, MIPS_MFHI);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_MULTU);
    mips___r_t(_jit, r0, MIPS_MFHI);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DIV);
    mips___r_t(_jit, r0, MIPS_MFLO);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DIVU);
    mips___r_t(_jit, r0, MIPS_MFLO);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DIV);
    mips___r_t(_jit, r0, MIPS_MFHI);
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
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DIVU);
    mips___r_t(_jit, r0, MIPS_MFHI);
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
    mips_rrr_t(_jit, r1, r2, r0, MIPS_AND);
}

#define jit_andi_i(r0, r1, i0)		mips_andi_i(_jit, r0, r1, i0)
__jit_inline void
mips_andi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_ANDI, r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_andr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_orr_i(r0, r1, r2)		mips_orr_i(_jit, r0, r1, r2)
__jit_inline void
mips_orr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, r0, MIPS_OR);
}

#define jit_ori_i(r0, r1, i0)		mips_ori_i(_jit, r0, r1, i0)
__jit_inline void
mips_ori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_ORI, r0, r1, i0);
    else {
	jit_movi_i(JIT_RTEMP, i0);
	jit_orr_i(r0, r1, JIT_RTEMP);
    }
}

#define jit_xorr_i(r0, r1, r2)		mips_xorr_i(_jit, r0, r1, r2)
__jit_inline void
mips_xorr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, r0, MIPS_XOR);
}

#define jit_xori_i(r0, r1, i0)		mips_xori_i(_jit, r0, r1, i0)
__jit_inline void
mips_xori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_XORI, r0, r1, i0);
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
    mips__rrit(_jit, r1, r0, i0, MIPS_SLL);
}

#define jit_rshr_i(r0, r1, i0)		mips_rshr_i(_jit, r0, r1, i0)
__jit_inline void
mips_rshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_SRAV);
}

#define jit_rshi_i(r0, r1, i0)		mips_rshi_i(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
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
    mips__rrit(_jit, r1, r0, i0, MIPS_SRL);
}

#define jit_jmpr(i0)			mips_jmpr(_jit, i0)
__jit_inline void
mips_jmpr(jit_state_t _jit, jit_gpr_t r0)
{
    mips_r_it(_jit, MIPS_JR, 0, r0);
}

#define jit_jmpi(i0)			mips_jmpi(_jit, i0)
__jit_inline jit_insn *
mips_jmpi(jit_state_t _jit, void *i0)
{
    long	pc = (long)_jit->x.pc;
    long	lb = (long)i0;

    /* FIXME how to patch efficiently? */

    if ((pc & 0xf0000000) == (lb & 0xf0000000))
	mipshi(_jit, MIPS_J, ((long)i0) >> 2);
    else {
	jit_movi_i(JIT_RTEMP, lb);
	mips_r_it(_jit, MIPS_JR, 0, JIT_RTEMP);
    }
    /* FIXME else jump to register */
    return (_jit->x.pc);
}

#define jit_beqr_i(i0, r0, r1)		mips_beqr_i(_jit, i0, r0, r1)
__jit_inline void
mips_beqr_i(jit_state_t _jit, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    long	ds = (long)i0 - (long)_jit->x.pc + 5;

    if (_siP(18, ds))
	mipshrri(_jit, MIPS_BEQ, r0, r1, ds);
    /* FIXME else jump to register */
}

#define jit_ldxr_c(r0, r1, r2)		mips_ldxr_c(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_LB, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_c(r0, r1, i0)		mips_ldxi_c(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_LB, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_LB, r0, JIT_RTEMP, 0);
    }
}

#define jit_ldxr_uc(r0, r1, r2)		mips_ldxr_uc(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_LBU, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_uc(r0, r1, i0)		mips_ldxi_uc(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_LBU, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_LBU, r0, JIT_RTEMP, 0);
    }
}

#define jit_ldxr_s(r0, r1, r2)		mips_ldxr_s(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_LH, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_s(r0, r1, i0)		mips_ldxi_s(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_LH, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_LH, r0, JIT_RTEMP, 0);
    }
}

#define jit_ldxr_us(r0, r1, r2)		mips_ldxr_us(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_LHU, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_us(r0, r1, i0)		mips_ldxi_us(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_LHU, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_LHU, r0, JIT_RTEMP, 0);
    }
}

#define jit_ldxr_i(r0, r1, r2)		mips_ldxr_i(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_LW, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_i(r0, r1, i0)		mips_ldxi_i(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_LW, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_LW, r0, JIT_RTEMP, 0);
    }
}

#define jit_stxr_c(r0, r1, r2)		mips_stxr_c(_jit, r0, r1, r2)
__jit_inline void
mips_stxr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_SB, r0, JIT_RTEMP, 0);
}

#define jit_stxi_c(i0, r0, r1)		mips_stxi_c(_jit, i0, r0, r1)
__jit_inline void
mips_stxi_c(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_SB, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_SB, r0, JIT_RTEMP, 0);
    }
}

#define jit_stxr_s(r0, r1, r2)		mips_stxr_s(_jit, r0, r1, r2)
__jit_inline void
mips_stxr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_SH, r0, JIT_RTEMP, 0);
}

#define jit_stxi_s(i0, r0, r1)		mips_stxi_s(_jit, i0, r0, r1)
__jit_inline void
mips_stxi_s(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_SH, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_SH, r0, JIT_RTEMP, 0);
    }
}

#define jit_stxr_i(r0, r1, r2)		mips_stxr_i(_jit, r0, r1, r2)
__jit_inline void
mips_stxr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, MIPS_SW, r0, JIT_RTEMP, 0);
}

#define jit_stxi_i(i0, r0, r1)		mips_stxi_i(_jit, i0, r0, r1)
__jit_inline void
mips_stxi_i(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_SW, r0, r1, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, MIPS_SW, r0, JIT_RTEMP, 0);
    }
}

#define jit_prolog(n)			mips_prolog(_jit, n)
__jit_inline void
mips_prolog(jit_state_t _jit, int n)
{
    _jitl.framesize = 40;
    /* jit_subi_p */
    jit_subi_i(_SP, _SP, 40);

    jit_stxi_i(36, _SP, _RA);
    jit_stxi_i(32, _SP, _FP);
    jit_stxi_i(28, _SP, _S7);
    jit_stxi_i(24, _SP, _S6);
    jit_stxi_i(20, _SP, _S5);
    jit_stxi_i(16, _SP, _S4);
    jit_stxi_i(12, _SP, _S3);
    jit_stxi_i( 8, _SP, _S2);
    jit_stxi_i( 4, _SP, _S1);
    jit_stxi_i( 0, _SP, _S0);

    /* jit_movr_p */
    jit_movr_i(_FP, _SP);
}

#define jit_ret()			mips_ret(_jit)
__jit_inline void
mips_ret(jit_state_t jit)
{
    /* jit_movr_p */
    jit_movr_i(_SP, _FP);

    jit_ldxi_i(_S0, _SP,  0);
    jit_ldxi_i(_S1, _SP,  4);
    jit_ldxi_i(_S2, _SP,  8);
    jit_ldxi_i(_S3, _SP, 12);
    jit_ldxi_i(_S4, _SP, 16);
    jit_ldxi_i(_S5, _SP, 20);
    jit_ldxi_i(_S6, _SP, 24);
    jit_ldxi_i(_S7, _SP, 28);
    jit_ldxi_i(_FP, _SP, 32);
    jit_ldxi_i(_RA, _SP, 36);

    /* jit_addi_p */
    jit_addi_i(_SP, _SP, 40);

    jit_jmpr(_RA);
    jit_noop(1);
}

#endif /* __lightning_core_mips_h */
