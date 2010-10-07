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
	mips_rrr_t(_jit, r1, JIT_RZERO, r0, MIPS_OR);
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
	    mipshrri(_jit, MIPS_ORI, r0, r0, i0 & 0xffff);
    }
}

#define jit_movi_p(r0, i0)		mips_movi_p(_jit, r0, i0)
__jit_inline void
mips_movi_p(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    mipsh_ri(_jit, MIPS_LUI, r0, (unsigned)i0 >> 16);
    mipshrri(_jit, MIPS_ORI, r0, r0, (unsigned)i0 & 0xffff);
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

#define jit_jmpr(i0)			mips_jmpr(_jit, i0)
__jit_inline void
mips_jmpr(jit_state_t _jit, jit_gpr_t r0)
{
    mips_r_it(_jit, r0, 0, MIPS_JR);
    jit_nop(1);
}

#define jit_jmpi(i0)			mips_jmpi(_jit, i0)
__jit_inline jit_insn *
mips_jmpi(jit_state_t _jit, void *i0)
{
    long	pc = (long)_jit->x.pc;
    long	lb = (long)i0;

    /* FIXME how to patch efficiently? */

    /* FIXME return an address that can be patched so, should always
     * jump to register, to not be limited to same 256Mb segment */

    if ((pc & 0xf0000000) == (lb & 0xf0000000))
	mipshi(_jit, MIPS_J, ((long)i0) >> 2);
    else {
	jit_movi_i(JIT_RTEMP, lb);
	mips_r_it(_jit, JIT_RTEMP, 0, MIPS_JR);
    }
    jit_nop(1);
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

#define jit_ldr_c(r0, r1)		mips_ldr_x(_jit, MIPS_LB,  r0, r1)
#define jit_ldr_uc(r0, r1)		mips_ldr_x(_jit, MIPS_LBU, r0, r1)
#define jit_ldr_s(r0, r1)		mips_ldr_x(_jit, MIPS_LH,  r0, r1)
#define jit_ldr_us(r0, r1)		mips_ldr_x(_jit, MIPS_LHU, r0, r1)
#define jit_ldr_i(r0, r1)		mips_ldr_x(_jit, MIPS_LW,  r0, r1)
__jit_inline void
mips_ldr_x(jit_state_t _jit, mips_hcode_t hc, jit_gpr_t r0, jit_gpr_t r1)
{
    mipshrri(_jit, hc, r0, r1, 0);
}

#define jit_ldi_c(r0, i0)		mips_ldi_x(_jit, MIPS_LB,  r0, i0)
#define jit_ldi_uc(r0, i0)		mips_ldi_x(_jit, MIPS_LBU, r0, i0)
#define jit_ldi_s(r0, i0)		mips_ldi_x(_jit, MIPS_LH,  r0, i0)
#define jit_ldi_us(r0, i0)		mips_ldi_x(_jit, MIPS_LHU, r0, i0)
#define jit_ldi_i(r0, i0)		mips_ldi_x(_jit, MIPS_LW,  r0, i0)
__jit_inline void
mips_ldi_x(jit_state_t _jit, mips_hcode_t hc, jit_gpr_t r0, void *i0)
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
mips_ldxr_x(jit_state_t _jit, mips_hcode_t hc,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_l(JIT_RTEMP, r1, r2);
    mipshrri(_jit, hc, r0, JIT_RTEMP, 0);
}

#define jit_ldxi_c(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LB,  r0, r1, i0)
#define jit_ldxi_uc(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LBU, r0, r1, i0)
#define jit_ldxi_s(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LH,  r0, r1, i0)
#define jit_ldxi_us(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LHU, r0, r1, i0)
#define jit_ldxi_i(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LW,  r0, r1, i0)
__jit_inline void
mips_ldxi_x(jit_state_t _jit, mips_hcode_t hc,
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
mips_str_x(jit_state_t _jit, mips_hcode_t hc, jit_gpr_t r0, jit_gpr_t r1)
{
    mipshrri(_jit, hc, r1, r0, 0);
}

#define jit_sti_c(i0, r0)		mips_sti_x(_jit, MIPS_SB, i0, r0)
#define jit_sti_s(i0, r0)		mips_sti_x(_jit, MIPS_SH, i0, r0)
#define jit_sti_i(i0, r0)		mips_sti_x(_jit, MIPS_SW, i0, r0)
__jit_inline void
mips_sti_x(jit_state_t _jit, mips_hcode_t hc, void *i0, jit_gpr_t r0)
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
mips_stxr_x(jit_state_t _jit, mips_hcode_t hc,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    mipshrri(_jit, hc, JIT_RTEMP, r0, 0);
}

#define jit_stxi_c(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SB, i0, r0, r1)
#define jit_stxi_s(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SH, i0, r0, r1)
#define jit_stxi_i(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SW, i0, r0, r1)
__jit_inline void
mips_stxi_x(jit_state_t _jit, mips_hcode_t hc,
	    int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (_s16P(i0))
	mipshrri(_jit, hc, r1, r0, i0);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	mipshrri(_jit, hc, JIT_RTEMP, r0, 0);
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

#define jit_prolog(n)			mips_prolog(_jit, n)
__jit_inline void
mips_prolog(jit_state_t _jit, int n)
{
    _jitl.framesize = 40;
    _jitl.nextarg_geti = 0;

    jit_subi_i(JIT_SP, JIT_SP, 40);
    jit_stxi_i(36, JIT_SP, _RA);
    jit_stxi_i(32, JIT_SP, _FP);
    jit_stxi_i(28, JIT_SP, _S7);
    jit_stxi_i(24, JIT_SP, _S6);
    jit_stxi_i(20, JIT_SP, _S5);
    jit_stxi_i(16, JIT_SP, _S4);
    jit_stxi_i(12, JIT_SP, _S3);
    jit_stxi_i( 8, JIT_SP, _S2);
    jit_stxi_i( 4, JIT_SP, _S1);
    jit_stxi_i( 0, JIT_SP, _S0);
    jit_movr_i(JIT_FP, JIT_SP);

    /* patch alloca and stack adjustment */
    jit_subi_i(JIT_SP, JIT_SP, 0);
    _jitl.stack = ((short *)_jit->x.pc) - 1;
    _jitl.alloca_offset = _jitl.stack_offset = _jitl.stack_length = 0;
}

#define jit_prepare_i(count)		mips_prepare_i(_jit, count)
__jit_inline void
mips_prepare_i(jit_state_t _jit, int count)
{
    assert(count		>= 0 &&
	   _jitl.stack_offset	== 0 &&
	   _jitl.nextarg_puti	== 0);

    _jitl.nextarg_puti = count;
    if (_jitl.nextarg_puti > JIT_A_NUM) {
	_jitl.stack_offset = (_jitl.nextarg_puti - JIT_A_NUM) << 2;
	if (_jitl.stack_length < _jitl.stack_offset) {
	    _jitl.stack_length = _jitl.stack_offset;
	    *_jitl.stack = (_jitl.alloca_offset +
			    _jitl.stack_length + 7) & ~7;
	}
    }
}

#define jit_pusharg_i(r0)		mips_pusharg_i(_jit, r0)
__jit_inline void
mips_pusharg_i(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.nextarg_puti > 0);
    if (--_jitl.nextarg_puti >= JIT_A_NUM) {
	_jitl.stack_offset -= sizeof(int);
	assert(_jitl.stack_offset >= 0);
	jit_stxi_i(_jitl.stack_offset, JIT_SP, r0);
    }
    else
	jit_movr_i(jit_a_order[_jitl.nextarg_puti], r0);
}

#define jit_arg_i()			mips_arg_i(_jit)
#define jit_arg_c()			mips_arg_i(_jit)
#define jit_arg_uc()			mips_arg_i(_jit)
#define jit_arg_s()			mips_arg_i(_jit)
#define jit_arg_us()			mips_arg_i(_jit)
#define jit_arg_ui()			mips_arg_i(_jit)
__jit_inline int
mips_arg_i(jit_state_t _jit)
{
    int		ofs;

    if (_jitl.nextarg_geti < JIT_A_NUM) {
	ofs = _jitl.nextarg_geti;
	++_jitl.nextarg_geti;
    }
    else {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(int);
    }

    return (ofs);
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

#define jit_calli(i0)			mips_calli(_jit, i0)
__jit_inline jit_insn *
mips_calli(jit_state_t _jit, void *i0)
{
#if 0
    /* FIXME still usable to call jit functions that are not really
     * position independent code */

    long	pc = (long)_jit->x.pc;
    long	lb = (long)i0;

    /* FIXME return an address that can be patched so, should always
     * call register to not be limited to same 256Mb segment */
    if ((pc & 0xf0000000) == (lb & 0xf0000000))
	mipshi(_jit, MIPS_JAL, ((long)i0) >> 2);
    else {
	jit_movi_i(JIT_RTEMP, lb);
	mips_r_rit(_jit, JIT_RTEMP, _RA, 0, MIPS_JALR);
    }
    jit_nop(1);
#else
    /* if calling a pic function, _T9 *must* hold the function pointer */

    jit_movi_i(_T9, (long)i0);
    jit_callr(_T9);
#endif
    return (_jit->x.pc);
}

#define jit_finish(i0)			mips_finish(_jit, i0)
__jit_inline jit_insn *
mips_finish(jit_state_t _jit, void *i0)
{
    assert(_jitl.stack_offset	== 0 &&
	   _jitl.nextarg_puti	== 0);
    jit_calli(i0);
    /* FIXME return patchable address */
    return (_jit->x.pc);
}

#define jit_finishr(rs)			x86_finishr(_jit, rs)
__jit_inline void
mips_finishr(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.stack_offset	== 0 &&
	   _jitl.nextarg_puti	== 0);
    jit_callr(r0);
}

#define jit_retval_i(r0)		mips_retval_i(_jit, r0)
__jit_inline void
mips_retval_i(jit_state_t _jit, jit_gpr_t r0)
{
    jit_movr_i(r0, JIT_RET);
}

#define jit_ret()			mips_ret(_jit)
__jit_inline void
mips_ret(jit_state_t jit)
{
    jit_movr_i(JIT_SP, JIT_FP);
    jit_ldxi_i(_S0, JIT_SP,  0);
    jit_ldxi_i(_S1, JIT_SP,  4);
    jit_ldxi_i(_S2, JIT_SP,  8);
    jit_ldxi_i(_S3, JIT_SP, 12);
    jit_ldxi_i(_S4, JIT_SP, 16);
    jit_ldxi_i(_S5, JIT_SP, 20);
    jit_ldxi_i(_S6, JIT_SP, 24);
    jit_ldxi_i(_S7, JIT_SP, 28);
    jit_ldxi_i(_FP, JIT_SP, 32);
    jit_ldxi_i(_RA, JIT_SP, 36);
    jit_jmpr(_RA);
    /* restore sp in delay slot */
    jit_addi_i(JIT_SP, JIT_SP, 40);
}

#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_MIPS64 \
	: defined (__mips64)
#  include "core-64.h"
#endif

#endif /* __lightning_core_mips_h */
