/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (arm version)
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

#ifndef __lightning_core_arm_h
#define __lightning_core_arm_h

#define JIT_PC			_R15
#define JIT_SP			_R13
#define JIT_TMP			_R12

#define jit_movr_i(r0, r1)		arm_movr_i(_jit, r0, r1)
__jit_inline void
arm_movr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _MOV(r0, r1);
}

#define jit_movi_i(r0, i0)		arm_movi_i(_jit, r0, i0)
__jit_inline void
arm_movi_i(jit_state_t _jit, jit_gpr_t r0, int i0)
{
    int		i;
    if ((i = encode_arm_immediate(i0)) != -1)
	_MOVI(r0, i);
    else if ((i = encode_arm_immediate(~i0)) != -1)
	_MVNI(r0, i);
    else {
	int	q0, q1, q2, q3;
	q0 = i0 & 0x000000ff;		q1 = i0 & 0x0000ff00;
	q2 = i0 & 0x00ff0000;		q3 = i0 & 0xff000000;
	if (!q0 + !q1 + !q2 + !q3 >= 2) {
	    /* prefer no inversion on tie */
	    if (q3) {
		_MOVI(r0, encode_arm_immediate(q3));
		if (q2)	_ORI(r0, r0, encode_arm_immediate(q2));
		if (q1)	_ORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_ORI(r0, r0, q0);
	    }
	    else if (q2) {
		_MOVI(r0, encode_arm_immediate(q2));
		if (q1)	_ORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_ORI(r0, r0, q0);
	    }
	    else if (q1) {
		_MOVI(r0, encode_arm_immediate(q1));
		_ORI(r0, r0, q0);
	    }
	}
	else {
	    i0 = ~i0;
	    q0 = i0 & 0x000000ff;	q1 = i0 & 0x0000ff00;
	    q2 = i0 & 0x00ff0000;	q3 = i0 & 0xff000000;
	    if (q3) {
		_MVNI(r0, encode_arm_immediate(q3));
		if (q2)	_XORI(r0, r0, encode_arm_immediate(q2));
		if (q1)	_XORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_XORI(r0, r0, q0);
	    }
	    else if (q2) {
		_MVNI(r0, encode_arm_immediate(q2));
		if (q1)	_XORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_XORI(r0, r0, q0);
	    }
	    else if (q1) {
		_MVNI(r0, encode_arm_immediate(q1));
		_XORI(r0, r0, q0);
	    }
	}
    }
}

#define jit_negr_i(r0, r1)		arm_negr_i(_jit, r0, r1)
__jit_inline void
arm_negr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _RSBI(r0, r1, 0);
}

#define jit_addr_i(r0, r1, r2)		arm_addr_i(_jit, r0, r1, r2)
__jit_inline void
arm_addr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADD(r0, r1, r2);
}

#define jit_addi_i(r0, r1, i0)		arm_addi_i(_jit, r0, r1, i0)
__jit_inline void
arm_addi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ADDI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_SUBI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_ADD(r0, r1, reg);
    }
}

#define jit_addcr_ui(r0, r1, r2)	arm_addcr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_addcr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADDS(r0, r1, r2);
}

#define jit_addci_ui(r0, r1, i0)	arm_addci_ui(_jit, r0, r1, i0)
__jit_inline void
arm_addci_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ADDSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_SUBSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_ADDS(r0, r1, reg);
    }
}

#define jit_addxr_ui(r0, r1, r2)	arm_addxr_i(_jit, r0, r1, r2)
__jit_inline void
arm_addxr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* keep setting carry because don't know last ADC */
    _ADCS(r0, r1, r2);
}

#define jit_addxi_ui(r0, r1, i0)	arm_addxi_ui(_jit, r0, r1, i0)
__jit_inline void
arm_addxi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ADCSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_SBCSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_ADCS(r0, r1, reg);
    }
}

#define jit_subr_i(r0, r1, r2)		arm_subr_i(_jit, r0, r1, r2)
__jit_inline void
arm_subr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SUB(r0, r1, r2);
}

#define jit_subi_i(r0, r1, i0)		arm_subi_i(_jit, r0, r1, i0)
__jit_inline void
arm_subi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SUBI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADDI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SUB(r0, r1, reg);
    }
}

#define jit_subcr_ui(r0, r1, r2)	arm_subcr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_subcr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SUBS(r0, r1, r2);
}

#define jit_subci_ui(r0, r1, i0)	arm_subci_ui(_jit, r0, r1, i0)
__jit_inline void
arm_subci_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SUBSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADDSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SUBS(r0, r1, reg);
    }
}

#define jit_subxr_ui(r0, r1, r2)	arm_subxr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_subxr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* keep setting carry because don't know last SBC */
    _SBCS(r0, r1, r2);
}

#define jit_subxi_ui(r0, r1, i0)	arm_subxi_ui(_jit, r0, r1, i0)
__jit_inline void
arm_subxi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SBCSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADCSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SBCS(r0, r1, reg);
    }
}

#define jit_rsbr_i(r0, r1, r2)		arm_rsbr_i(_jit, r0, r1, r2)
__jit_inline void
arm_rsbr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _RSB(r0, r1, r2);
}

#define jit_rsbi_i(r0, r1, i0)		arm_rsbi_i(_jit, r0, r1, i0)
__jit_inline void
arm_rsbi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_RSBI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_RSB(r0, r1, reg);
    }
}

#define jit_mulr_i(r0, r1, r2)		arm_mulr_i(_jit, r0, r1, r2)
__jit_inline void
arm_mulr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1 && !jit_cpu.armv6) {
	_MOV(JIT_TMP, r1);
	_MUL(r0, JIT_TMP, r2);
    }
    else
	_MUL(r0, r1, r2);
}

#define jit_muli_i(r0, r1, i0)		arm_muli_i(_jit, r0, r1, i0)
__jit_inline void
arm_muli_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    reg = r0 != r1 ? r0 : JIT_TMP;
    jit_movi_i(reg, i0);
    if (r0 == r1 && !jit_cpu.armv6)
	_MUL(r0, reg, r1);
    else
	_MUL(r0, r1, reg);
}


#define jit_notr_i(r0, r1)		arm_notr_i(_jit, r0, r1)
__jit_inline void
arm_notr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _NOT(r0, r1);
}

#define jit_andr_i(r0, r1, r2)		arm_andr_i(_jit, r0, r1, r2)
__jit_inline void
arm_andr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _AND(r0, r1, r2);
}

#define jit_andi_i(r0, r1, i0)		arm_andi_i(_jit, r0, r1, i0)
__jit_inline void
arm_andi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ANDI(r0, r1, i0);
    else if ((i = encode_arm_immediate(~i0)) != -1)
	_BICI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_AND(r0, r1, reg);
    }
}

#define jit_orr_i(r0, r1, r2)		arm_orr_i(_jit, r0, r1, r2)
__jit_inline void
arm_orr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _OR(r0, r1, r2);
}

#define jit_ori_i(r0, r1, i0)		arm_ori_i(_jit, r0, r1, i0)
__jit_inline void
arm_ori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ORI(r0, r1, i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_OR(r0, r1, reg);
    }
}

#define jit_xorr_i(r0, r1, r2)		arm_xorr_i(_jit, r0, r1, r2)
__jit_inline void
arm_orr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _XOR(r0, r1, r2);
}

#define jit_xori_i(r0, r1, i0)		arm_xori_i(_jit, r0, r1, i0)
__jit_inline void
arm_xori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_XORI(r0, r1, i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_XOR(r0, r1, reg);
    }
}

#define jit_lshr_i(r0, r1, r2)		arm_lshr_i(_jit, r0, r1, r2)
__jit_inline void
arm_lshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LSL(r0, r1, r2);
}

#define jit_lshi_i(r0, r1, i0)		arm_lshi_i(_jit, r0, r1, i0)
__jit_inline void
arm_lshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 31);
    _LSLI(r0, r1, i0);
}

#define jit_rshr_i(r0, r1, r2)		arm_rshr_i(_jit, r0, r1, r2)
__jit_inline void
arm_rshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ASR(r0, r1, r2);
}

#define jit_rshi_i(r0, r1, i0)		arm_rshi_i(_jit, r0, r1, i0)
__jit_inline void
arm_rshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 1 && i0 <= 32);
    _ASRI(r0, r1, i0);
}

#define jit_rshr_ui(r0, r1, r2)		arm_rshr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_rshr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LSR(r0, r1, r2);
}

#define jit_rshi_ui(r0, r1, i0)		arm_rshi_ui(_jit, r0, r1, i0)
__jit_inline void
arm_rshi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 1 && i0 <= 32);
    _LSRI(r0, r1, i0);
}

#define jit_ldxr_c(r0, r1, r2)		arm_ldxr_c(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRSB(r0, r1, r2);
}

#define jit_ldxi_c(r0, r1, i0)		arm_ldxi_c(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRSBI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRSBIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRSB(r0, r1, reg);
    }
}

#define jit_ldxr_uc(r0, r1, r2)		arm_ldxr_uc(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRB(r0, r1, r2);
}

#define jit_ldxi_uc(r0, r1, i0)		arm_ldxi_uc(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_LDRBI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_LDRBIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRB(r0, r1, reg);
    }
}

#define jit_ldxr_s(r0, r1, r2)		arm_ldxr_s(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRSH(r0, r1, r2);
}

#define jit_ldxi_s(r0, r1, i0)		arm_ldxi_s(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRSHI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRSHIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRSH(r0, r1, reg);
    }
}

#define jit_ldxr_us(r0, r1, r2)		arm_ldxr_us(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRH(r0, r1, r2);
}

#define jit_ldxi_us(r0, r1, i0)		arm_ldxi_us(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRHI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRHIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRH(r0, r1, reg);
    }
}

#define jit_ldxr_i(r0, r1, r2)		arm_ldxr_i(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDR(r0, r1, r2);
}

#define jit_ldxi_i(r0, r1, i0)		arm_ldxi_i(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_LDRI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_LDRIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDR(r0, r1, reg);
    }
}

#define jit_stxr_c(r0, r1, r2)		arm_stxr_c(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STRB(r0, r1, r2);
}

#define jit_stxi_c(r0, r1, i0)		arm_stxi_c(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_c(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_STRBI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_STRBIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRB(r0, r1, reg);
    }
}

#define jit_stxr_s(r0, r1, r2)		arm_stxr_s(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STRH(r0, r1, r2);
}

#define jit_stxi_s(r0, r1, i0)		arm_stxi_s(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_s(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_STRHI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_STRHIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRH(r0, r1, reg);
    }
}

#define jit_stxr_i(r0, r1, r2)		arm_stxr_i(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STR(r0, r1, r2);
}

#define jit_stxi_i(r0, r1, i0)		arm_stxi_i(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_i(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_STRI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_STRIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STR(r0, r1, reg);
    }
}

#endif /* __lightning_core_arm_h */
