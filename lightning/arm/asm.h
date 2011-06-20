/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the arm
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2011 Free Software Foundation
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

#ifndef __lightning_asm_h
#define __lightning_asm_h

typedef enum {
    _R0,	/* argument / result */
    _R1,	/* argument */
    _R2,	/* argument */
    _R3,	/* argument */
    _R4,	/* variable */
    _R5,	/* variable */
    _R6,	/* variable */
    _R7,	/* variable */
    _R8,	/* variable */
    _R9,	/* variable */
    _R10,	/* sl - stack limit */
    _R11,	/* fp - frame pointer */
    _R12,	/* ip - temporary */
    _R13,	/* sp - stack pointer */
    _R14,	/* lr - link register */
    _R15,	/* pc - program counter */
} jit_gpr_t;

#define JIT_RET		_R0
#define JIT_PC		_R15
#define JIT_LR		_R14
#define JIT_SP		_R13
#define JIT_FP		_R11
#define JIT_TMP		_R8
#define JIT_FTMP	_R9
#define JIT_FPRET	_F0

typedef enum {
    _F0,	/* result */
    _F1,	/* scratch */
    _F2,	/* scratch */
    _F3,	/* scratch */
    _F4,	/* variable */
    _F5,	/* variable */
    _F6,	/* variable */
    _F7,	/* variable */
} jit_fpr_t;

#define ARM_CC_EQ	0x00000000	/* Z=1 */
#define ARM_CC_NE	0x10000000	/* Z=0 */
#define ARM_CC_HS	0x20000000	/* C=1 */
#define ARM_CC_LO	0x30000000	/* C=0 */
#define ARM_CC_MI	0x40000000	/* N=1 */
#define ARM_CC_PL	0x50000000	/* N=0 */
#define ARM_CC_VS	0x60000000	/* V=1 */
#define ARM_CC_VC	0x70000000	/* V=0 */
#define ARM_CC_HI	0x80000000	/* C=1 && Z=0 */
#define ARM_CC_LS	0x90000000	/* C=0 || Z=1 */
#define ARM_CC_GE	0xa0000000	/* (N=1 && V=1) || (N=0 && V=0) */
#define ARM_CC_LT	0xb0000000	/* (N=1 && V=0) || (N=0 && V=1) */
#define ARM_CC_GT	0xc0000000	/* Z=0 && ((N=1 && V=1) || (N=0 && V=1)) XXX */
#define ARM_CC_LE	0xd0000000	/* Z=1 || (N=1 && V=0) || (N=1 && V=0) */
#define ARM_CC_AL	0xe0000000	/* always */
#define ARM_CC_NV	0xf0000000	/* reserved */

#define ARM_MOV		0x01a00000
#define ARM_MVN		0x01e00000

#define ARM_I		0x02000000	/* immediate */
#define ARM_S		0x00100000	/* set flags */
#define ARM_ADD		0x00800000
#define ARM_ADC		0x00a00000
#define ARM_SUB		0x00400000
#define ARM_SBC		0x00c00000
#define ARM_RSB		0x00600000
#define ARM_RSC		0x00e00000	/* ARMV7M */
#define ARM_MUL		0x00000090
#define ARM_MLA		0x00200090
#define ARM_UMULL	0x00800090
#define ARM_UMLAL	0x00a00090
#define ARM_SMULL	0x00c00090
#define ARM_SMLAL	0x00e00090

#define ARM_AND		0x00000000
#define ARM_BIC		0x01c00000
#define ARM_ORR		0x01800000
#define ARM_EOR		0x00200000

#define ARM_SHIFT	0x01a00000
#define ARM_R		0x00000010	/* register shift */
#define ARM_LSL		0x00000000
#define ARM_LSR		0x00000020
#define ARM_ASR		0x00000040
#define ARM_ROR		0x00000060

#define ARM_CMP		0x01500000
#define ARM_CMN		0x01700000
#define ARM_TST		0x01100000	/* THUMB */
#define ARM_TEQ		0x01300000	/* ARMV6T2 */

/* branch */
#define ARM_BX		0x012fff10
#define ARM_BLX		0x012fff30
#define ARM_B		0x0a000000
#define ARM_BL		0x0b000000

/* ldr/str */
#define ARM_P		0x00800000	/* positive offset */
#define ARM_LDRSB	0x011000d0
#define ARM_LDRSBI	0x015000d0
#define ARM_LDRB	0x07500000
#define ARM_LDRBI	0x05500000
#define ARM_LDRSH	0x011000f0
#define ARM_LDRSHI	0x015000f0
#define ARM_LDRH	0x011000b0
#define ARM_LDRHI	0x015000b0
#define ARM_LDR		0x07100000
#define ARM_LDRI	0x05100000
#define ARM_LDRD	0x010000d0
#define ARM_LDRDI	0x014000d0
#define ARM_STRB	0x07400000
#define ARM_STRBI	0x05400000
#define ARM_STRH	0x010000b0
#define ARM_STRHI	0x014000b0
#define ARM_STR		0x07000000
#define ARM_STRI	0x05000000
#define ARM_STRD	0x010000f0
#define ARM_STRDI	0x014000f0

/* ldm/stm */
#define ARM_M		0x08000000
#define ARM_M_L		0x00100000	/* load; store if not set */
#define ARM_M_I		0x00800000	/* inc; dec if not set */
#define ARM_M_B		0x01000000	/* before; after if not set */
#define ARM_M_U		0x00200000	/* update Rn */

/* from binutils */
#define rotate_left(v, n)	(v << n | v >> (32 - n))
static int
encode_arm_immediate(unsigned int v)
{
    unsigned int	a, i;

    for (i = 0; i < 32; i += 2)
	if ((a = rotate_left(v, i)) <= 0xff)
	    return (a | (i << 7));

    return (-1);
}

__jit_inline void
arm_cc_orrr(jit_state_t _jit, int cc, int o, int r0, int r1, int r2)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fff0f));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u4(r2));
}

__jit_inline void
arm_cc_orri(jit_state_t _jit, int cc, int o, int r0, int r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fffff));
    assert(!(i0 & 0xfffff000));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u12(i0));
}

__jit_inline void
arm_cc_orri8(jit_state_t _jit, int cc, int o, int r0, int r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fff0f));
    assert(!(i0 & 0xffffff00));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|((i0&0xf0)<<4)|(i0&0x0f));
}

__jit_inline void
arm_cc_orrrr(jit_state_t _jit, int cc, int o, int r0, int r1, int r2, int r3)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fff0f));
    _jit_I(cc|o|(_u4(r0)<<16)|(_u4(r1)<<12)|(_u4(r2)<<8)|_u4(r3));
}

__jit_inline void
arm_cc_srrri(jit_state_t _jit, int cc, int o, int r0, int r1, int r2, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0000ff8f));
    _jit_I(cc|o|(_u4(r0)<<12)|(_u4(r1)<<16)|(i0<<7)|_u4(r2));
}

__jit_inline void
arm_cc_shift(jit_state_t _jit, int cc, int o, int r0, int r1, int r2, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0fe0ff8f));
    assert(((_u4(r2)<<8)&(i0<<7)) == 0);
    _jit_I(cc|ARM_SHIFT|o|(_u4(r0)<<12)|(_u4(r2)<<8)|(i0<<7)|_u4(r1));
}

__jit_inline void
arm_cc_b(jit_state_t _jit, int cc, int o, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x00ffffff));
    _jit_I(cc|o|_u24(i0));
}

__jit_inline void
arm_cc_bx(jit_state_t _jit, int cc, int o, int r0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0000000f));
    _jit_I(cc|o|_u4(r0));
}

__jit_inline void
arm_cc_orl(jit_state_t _jit, int cc, int o, int r0, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fffff));
    _jit_I(cc|o|(_u4(r0)<<16)|_u16(i0));
}

#define _CC_MOV(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_MOV,r0,0,r1)
#define _MOV(r0,r1)		_CC_MOV(ARM_CC_AL,r0,r1)
#define _CC_MOVI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_MOV|ARM_I,r0,0,i0)
#define _MOVI(r0,i0)		_CC_MOVI(ARM_CC_AL,r0,i0)
#define _CC_MVN(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_MVN,r0,0,r1)
#define _MVN(r0, r1)		_CC_MVN(ARM_CC_AL,r0,r1)
#define _CC_MVNI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_MVN|ARM_I,r0,0,i0)
#define _MVNI(r0,i0)		_CC_MVNI(ARM_CC_AL,r0,i0)
#define _CC_NOT(cc,r0,r1)	_CC_MVN(cc,r0,r1)
#define _NOT(r0, r1)		_MVN(r0, r1)
#define _NOP()			_MOV(_R0, _R0)

#define _CC_ADD(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADD,r0,r1,r2)
#define _ADD(r0,r1,r2)		_CC_ADD(ARM_CC_AL,r0,r1,r2)
#define _CC_ADDI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADD|ARM_I,r0,r1,i0)
#define _ADDI(r0,r1,i0)		_CC_ADDI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADDS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADD|ARM_S,r0,r1,r2)
#define _ADDS(r0,r1,r2)		_CC_ADDS(ARM_CC_AL,r0,r1,r2)
#define _CC_ADDSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADD|ARM_S|ARM_I,r0,r1,i0)
#define _ADDSI(r0,r1,i0)	_CC_ADDSI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADC,r0,r1,r2)
#define _ADC(r0,r1,r2)		_CC_ADC(ARM_CC_AL,r0,r1,r2)
#define _CC_ADCI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADC|ARM_I,r0,r1,i0)
#define _ADCI(r0,r1,i0)		_CC_ADCI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADCS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADC|ARM_S,r0,r1,r2)
#define _ADCS(r0,r1,r2)		_CC_ADCS(ARM_CC_AL,r0,r1,r2)
#define _CC_ADCSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADC|ARM_S|ARM_I,r0,r1,i0)
#define _ADCSI(r0,r1,i0)	_CC_ADCSI(ARM_CC_AL,r0,r1,i0)
#define _CC_SUB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SUB,r0,r1,r2)
#define _SUB(r0,r1,r2)		_CC_SUB(ARM_CC_AL,r0,r1,r2)
#define _CC_SUBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SUB|ARM_I,r0,r1,i0)
#define _SUBI(r0,r1,i0)		_CC_SUBI(ARM_CC_AL,r0,r1,i0)
#define _CC_SUBS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SUB|ARM_S,r0,r1,r2)
#define _SUBS(r0,r1,r2)		_CC_SUBS(ARM_CC_AL,r0,r1,r2)
#define _CC_SUBSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SUB|ARM_S|ARM_I,r0,r1,i0)
#define _SUBSI(r0,r1,i0)	_CC_SUBSI(ARM_CC_AL,r0,r1,i0)
#define _CC_SBC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SBC,r0,r1,r2)
#define _SBC(r0,r1,r2)		_CC_SBC(ARM_CC_AL,r0,r1,r2)
#define _CC_SBCI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SBC|ARM_I,r0,r1,i0)
#define _SBCI(r0,r1,i0)		_CC_SBCI(ARM_CC_AL,r0,r1,i0)
#define _CC_SBCS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SBC|ARM_S,r0,r1,r2)
#define _SBCS(r0,r1,r2)		_CC_SBCS(ARM_CC_AL,r0,r1,r2)
#define _CC_SBCSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SBC|ARM_S|ARM_I,r0,r1,i0)
#define _SBCSI(r0,r1,i0)	_CC_SBCSI(ARM_CC_AL,r0,r1,i0)
#define _CC_RSB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_RSB,r0,r1,r2)
#define _RSB(r0,r1,r2)		_CC_RSB(ARM_CC_AL,r0,r1,r2)
#define _CC_RSBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_RSB|ARM_I,r0,r1,i0)
#define _RSBI(r0,r1,i0)		_CC_RSBI(ARM_CC_AL,r0,r1,i0)

/* >> ARMV7M */
#define _CC_RSC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_RSC,r0,r1,r2)
#define _RSC(r0,r1,r2)		_CC_RSC(ARM_CC_AL,r0,r1,r2)
#define _CC_RSCI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_RSC|ARM_I,r0,r1,i0)
#define _RSCI(r0,r1,i0)		_CC_RSCI(ARM_CC_AL,r0,r1,i0)
/* << ARMV7M */

#define _CC_MUL(cc,r0,r1,r2)	  arm_cc_orrrr(_jit,cc,ARM_MUL,r0,0,r2,r1)
#define _MUL(r0,r1,r2)		  _CC_MUL(ARM_CC_AL,r0,r1,r2)
#define _CC_MLA(cc,r0,r1,r2,r3)	  arm_cc_orrrr(_jit,cc,ARM_MLA,r0,r3,r2,r1)
#define _MLA(r0,r1,r2,r3)	  _CC_MLA(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_UMULL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_UMULL,r1,r0,r3,r2)
#define _UMULL(r0,r1,r2,r3)	  _CC_UMULL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_UMLAL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_UMLAL,r1,r0,r3,r2)
#define _UMLAL(r0,r1,r2,r3)	  _CC_UMLAL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_SMULL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_SMULL,r1,r0,r3,r2)
#define _SMULL(r0,r1,r2,r3)	  _CC_SMULL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_SMLAL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_SMLAL,r1,r0,r3,r2)
#define _SMLAL(r0,r1,r2,r3)	  _CC_SMLAL(ARM_CC_AL,r0,r1,r2,r3)

#define _CC_AND(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_AND,r0,r1,r2)
#define _AND(r0,r1,r2)		_CC_AND(ARM_CC_AL,r0,r1,r2)
#define _CC_AND_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(_jit,cc,ARM_AND|sh,r0,r1,r2,im)
#define _AND_SI(r0,r1,r2,sh,im)	_CC_AND_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_ANDI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_AND|ARM_I,r0,r1,i0)
#define _ANDI(r0,r1,i0)		_CC_ANDI(ARM_CC_AL,r0,r1,i0)
#define _CC_ANDS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_AND|ARM_S,r0,r1,r2)
#define _ANDS(r0,r1,r2)		_CC_ANDS(ARM_CC_AL,r0,r1,r2)
#define _CC_ANDSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_AND|ARM_S|ARM_I,r0,r1,i0)
#define _ANDSI(r0,r1,i0)	_CC_ANDSI(ARM_CC_AL,r0,r1,i0)
#define _CC_BIC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_BIC,r0,r1,r2)
#define _BIC(r0,r1,r2)		_CC_BIC(ARM_CC_AL,r0,r1,r2)
#define _CC_BIC_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(_jit,cc,ARM_BIC|sh,r0,r1,r2,im)
#define _BIC_SI(r0,r1,r2,sh,im)	_CC_BIC_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_BICI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_BIC|ARM_I,r0,r1,i0)
#define _BICI(r0,r1,i0)		_CC_BICI(ARM_CC_AL,r0,r1,i0)
#define _CC_BICS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_BIC|ARM_S,r0,r1,r2)
#define _BICS(r0,r1,r2)		_CC_BICS(ARM_CC_AL,r0,r1,r2)
#define _CC_BICSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_BIC|ARM_S|ARM_I,r0,r1,i0)
#define _BICSI(r0,r1,i0)	_CC_BICSI(ARM_CC_AL,r0,r1,i0)
#define _CC_OR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ORR,r0,r1,r2)
#define _OR(r0,r1,r2)		_CC_OR(ARM_CC_AL,r0,r1,r2)
#define _CC_OR_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(_jit,cc,ARM_ORR|sh,r0,r1,r2,im)
#define _OR_SI(r0,r1,r2,sh,im)	_CC_OR_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_ORI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ORR|ARM_I,r0,r1,i0)
#define _ORI(r0,r1,i0)		_CC_ORI(ARM_CC_AL,r0,r1,i0)
#define _CC_XOR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_EOR,r0,r1,r2)
#define _XOR(r0,r1,r2)		_CC_XOR(ARM_CC_AL,r0,r1,r2)
#define _CC_XOR_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(_jit,cc,ARM_EOR|sh,r0,r1,r2,im)
#define _XOR_SI(r0,r1,r2,sh,im)	_CC_XOR_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_XORI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_EOR|ARM_I,r0,r1,i0)
#define _XORI(r0,r1,i0)		_CC_XORI(ARM_CC_AL,r0,r1,i0)

#define _CC_SHIFT(cc,o,r0,r1,r2,i0) arm_cc_shift(_jit,cc,o,r0,r1,r2,i0)
#define _CC_LSL(cc,r0,r1,r2)	_CC_SHIFT(cc,ARM_LSL|ARM_R,r0,r1,r2,0)
#define _LSL(r0,r1,r2)		_CC_LSL(ARM_CC_AL,r0,r1,r2)
#define _CC_LSLI(cc,r0,r1,i0)	_CC_SHIFT(cc,ARM_LSL,r0,r1,0,i0)
#define _LSLI(r0,r1,i0)		_CC_LSLI(ARM_CC_AL,r0,r1,i0)
#define _CC_LSR(cc,r0,r1,r2)	_CC_SHIFT(cc,ARM_LSR|ARM_R,r0,r1,r2,0)
#define _LSR(r0,r1,r2)		_CC_LSR(ARM_CC_AL,r0,r1,r2)
#define _CC_LSRI(cc,r0,r1,i0)	_CC_SHIFT(cc,ARM_LSR,r0,r1,0,i0)
#define _LSRI(r0,r1,i0)		_CC_LSRI(ARM_CC_AL,r0,r1,i0)
#define _CC_ASR(cc,r0,r1,r2)	_CC_SHIFT(cc,ARM_ASR|ARM_R,r0,r1,r2,0)
#define _ASR(r0,r1,r2)		_CC_ASR(ARM_CC_AL,r0,r1,r2)
#define _CC_ASRI(cc,r0,r1,i0)	_CC_SHIFT(cc,ARM_ASR,r0,r1,0,i0)
#define _ASRI(r0,r1,i0)		_CC_ASRI(ARM_CC_AL,r0,r1,i0)
#define _CC_ROR(cc,r0,r1,r2)	_CC_SHIFT(cc,ARM_ROR|ARM_R,r0,r1,r2,0)
#define _ROR(r0,r1,r2)		_CC_ROR(ARM_CC_AL,r0,r1,r2)
#define _CC_RORI(cc,r0,r1,i0)	_CC_SHIFT(cc,ARM_ROR,r0,r1,0,i0)
#define _RORI(r0,r1,i0)		_CC_RORI(ARM_CC_AL,r0,r1,i0)

#define _CC_CMP(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_CMP,0,r0,r1)
#define _CMP(r0,r1)		_CC_CMP(ARM_CC_AL,r0,r1)
#define _CC_CMPI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_CMP|ARM_I,0,r0,i0)
#define _CMPI(r0,i0)		_CC_CMPI(ARM_CC_AL,r0,i0)
#define _CC_CMN(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_CMN,0,r0,r1)
#define _CMN(r0,r1)		_CC_CMN(ARM_CC_AL,r0,r1)
#define _CC_CMNI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_CMN|ARM_I,0,r0,i0)
#define _CMNI(r0,i0)		_CC_CMNI(ARM_CC_AL,r0,i0)

/* >> THUMB */
#define _CC_TST(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_TST,0,r0,r1)
#define _TST(r0,r1)		_CC_TST(ARM_CC_AL,r0,r1)
#define _CC_TSTI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_TST|ARM_I,0,r0,i0)
#define _TSTI(r0,i0)		_CC_TSTI(ARM_CC_AL,r0,i0)
/* << THUMB */

/* >> ARVM6T2 */
#define _CC_TEQ(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_TEQ,0,r0,r1)
#define _TEQ(r0,r1)		_CC_TEQ(ARM_CC_AL,r0,r1)
#define _CC_TEQI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_TEQ|ARM_I,0,r0,i0)
#define _TEQI(r0,i0)		_CC_TEQI(ARM_CC_AL,r0,i0)
/* << ARVM6T2 */

#define _CC_BX(cc,r0)		arm_cc_bx(_jit,cc,ARM_BX,r0)
#define _BX(i0)			_CC_BX(ARM_CC_AL,r0)
#define _CC_BLX(cc,r0)		arm_cc_bx(_jit,cc,ARM_BLX,r0)
#define _BLX(r0)		_CC_BLX(ARM_CC_AL,r0)
#define _CC_B(cc,i0)		arm_cc_b(_jit,cc,ARM_B,i0)
#define _B(i0)			_CC_B(ARM_CC_AL,i0)
#define _CC_BL(cc,i0)		arm_cc_b(_jit,cc,ARM_BL,i0)
#define _BL(i0)			_CC_BL(ARM_CC_AL,i0)

#define _CC_LDRSB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSB|ARM_P,r0,r1,r2)
#define _LDRSB(r0,r1,r2)	_CC_LDRSB(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSBN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSB,r0,r1,r2)
#define _LDRSBN(r0,r1,r2)	_CC_LDRSBN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSBI(cc,r0,r1,i0)	 arm_cc_orri8(_jit,cc,ARM_LDRSBI|ARM_P,r0,r1,i0)
#define _LDRSBI(r0,r1,i0)	 _CC_LDRSBI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSBIN(cc,r0,r1,i0) arm_cc_orri8(_jit,cc,ARM_LDRSBI,r0,r1,i0)
#define _LDRSBIN(r0,r1,i0)	 _CC_LDRSBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRB|ARM_P,r0,r1,r2)
#define _LDRB(r0,r1,r2)		_CC_LDRB(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRBN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRB,r0,r1,r2)
#define _LDRBN(r0,r1,r2)	_CC_LDRBN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRBI|ARM_P,r0,r1,i0)
#define _LDRBI(r0,r1,i0)	_CC_LDRBI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRBIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRBI,r0,r1,i0)
#define _LDRBIN(r0,r1,i0)	_CC_LDRBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSH(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSH|ARM_P,r0,r1,r2)
#define _LDRSH(r0,r1,r2)	_CC_LDRSH(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSHN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSH,r0,r1,r2)
#define _LDRSHN(r0,r1,r2)	_CC_LDRSHN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSHI(cc,r0,r1,i0)	 arm_cc_orri8(_jit,cc,ARM_LDRSHI|ARM_P,r0,r1,i0)
#define _LDRSHI(r0,r1,i0)	 _CC_LDRSHI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSHIN(cc,r0,r1,i0) arm_cc_orri8(_jit,cc,ARM_LDRSHI,r0,r1,i0)
#define _LDRSHIN(r0,r1,i0)	 _CC_LDRSHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRH(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRH|ARM_P,r0,r1,r2)
#define _LDRH(r0,r1,r2)		_CC_LDRH(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRHN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRH,r0,r1,r2)
#define _LDRHN(r0,r1,r2)	_CC_LDRHN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRHI(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_LDRHI|ARM_P,r0,r1,i0)
#define _LDRHI(r0,r1,i0)	_CC_LDRHI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRHIN(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_LDRHI,r0,r1,i0)
#define _LDRHIN(r0,r1,i0)	_CC_LDRHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDR|ARM_P,r0,r1,r2)
#define _LDR(r0,r1,r2)		_CC_LDR(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDR,r0,r1,r2)
#define _LDRN(r0,r1,r2)		_CC_LDRN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRI|ARM_P,r0,r1,i0)
#define _LDRI(r0,r1,i0)		_CC_LDRI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRI,r0,r1,i0)
#define _LDRIN(r0,r1,i0)	_CC_LDRIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRD(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRD|ARM_P,r0,r1,r2)
#define _LDRD(r0,r1,r2)		_CC_LDRD(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRDN(cc,r0,r1,r2)  arm_cc_orrr(_jit,cc,ARM_LDRD,r0,r1,r2)
#define _LDRDN(r0,r1,r2)	_CC_LDRDN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRDI(cc,r0,r1,i0)  arm_cc_orri8(_jit,cc,ARM_LDRDI|ARM_P,r0,r1,i0)
#define _LDRDI(r0,r1,i0)	_CC_LDRDI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRDIN(cc,r0,r1,i0) arm_cc_orri8(_jit,cc,ARM_LDRDI,r0,r1,i0)
#define _LDRDIN(r0,r1,i0)	_CC_LDRDIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRB|ARM_P,r0,r1,r2)
#define _STRB(r0,r1,r2)		_CC_STRB(ARM_CC_AL,r0,r1,r2)
#define _CC_STRBN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRB,r0,r1,r2)
#define _STRBN(r0,r1,r2)	_CC_STRBN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRBI|ARM_P,r0,r1,i0)
#define _STRBI(r0,r1,i0)	_CC_STRBI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRBIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRBI,r0,r1,i0)
#define _STRBIN(r0,r1,i0)	_CC_STRBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRH(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRH|ARM_P,r0,r1,r2)
#define _STRH(r0,r1,r2)		_CC_STRH(ARM_CC_AL,r0,r1,r2)
#define _CC_STRHN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRH,r0,r1,r2)
#define _STRHN(r0,r1,r2)	_CC_STRHN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRHI(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_STRHI|ARM_P,r0,r1,i0)
#define _STRHI(r0,r1,i0)	_CC_STRHI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRHIN(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_STRHI,r0,r1,i0)
#define _STRHIN(r0,r1,i0)	_CC_STRHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STR|ARM_P,r0,r1,r2)
#define _STR(r0,r1,r2)		_CC_STR(ARM_CC_AL,r0,r1,r2)
#define _CC_STRN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STR,r0,r1,r2)
#define _STRN(r0,r1,r2)		_CC_STRN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRI|ARM_P,r0,r1,i0)
#define _STRI(r0,r1,i0)		_CC_STRI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRI,r0,r1,i0)
#define _STRIN(r0,r1,i0)	_CC_STRIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRD(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRD|ARM_P,r0,r1,r2)
#define _STRD(r0,r1,r2)		_CC_STRD(ARM_CC_AL,r0,r1,r2)
#define _CC_STRDN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRD,r0,r1,r2)
#define _STRDN(r0,r1,r2)	_CC_STRDN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRDI(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_STRDI|ARM_P,r0,r1,i0)
#define _STRDI(r0,r1,i0)	_CC_STRDI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRDIN(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_STRDI,r0,r1,i0)
#define _STRDIN(r0,r1,i0)	_CC_STRDIN(ARM_CC_AL,r0,r1,i0)

#define _CC_LDMIA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I,r0,i0)
#define _LDMIA(r0,i0)		_CC_LDMIA(ARM_CC_AL,r0,i0)
#define _LDM(r0,i0)		_LDMIA(r0,i0)
#define _CC_LDMIA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_U,r0,i0)
#define _LDMIA_U(r0,i0)		_CC_LDMIA_U(ARM_CC_AL,r0,i0)
#define _LDM_U(r0,i0)		_LDMIA_U(r0,i0)
#define _CC_LDMIB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_B,r0,i0)
#define _LDMIB(r0,i0)		_CC_LDMIB(ARM_CC_AL,r0,i0)
#define _CC_LDMIB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_B|ARM_M_U,r0,i0)
#define _LDMIB_U(r0,i0)		_CC_LDMIB_U(ARM_CC_AL,r0,i0)
#define _CC_LDMDA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L,r0,i0)
#define _LDMDA(r0,i0)		_CC_LDMDA(ARM_CC_AL,r0,i0)
#define _CC_LDMDA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_U,r0,i0)
#define _LDMDA_U(r0,i0)		_CC_LDMDA_U(ARM_CC_AL,r0,i0)
#define _CC_LDMDB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_B,r0,i0)
#define _LDMDB(r0,i0)		_CC_LDMDB(ARM_CC_AL,r0,i0)
#define _CC_LDMDB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_B|ARM_M_U,r0,i0)
#define _LDMDB_U(r0,i0)		_CC_LDMDB_U(ARM_CC_AL,r0,i0)
#define _CC_STMIA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I,r0,i0)
#define _STMIA(r0,i0)		_CC_STMIA(ARM_CC_AL,r0,i0)
#define _STM(r0,i0)		_STMIA(r0,i0)
#define _CC_STMIA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I|ARM_M_U,r0,i0)
#define _STMIA_U(r0,i0)		_CC_STMIA_U(ARM_CC_AL,r0,i0)
#define _STM_U(r0,i0)		_STMIA_U(r0,i0)
#define _CC_STMIB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I|ARM_M_B,r0,i0)
#define _STMIB(r0,i0)		_CC_STMIB(ARM_CC_AL,r0,i0)
#define _CC_STMIB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I|ARM_M_B|ARM_M_U,r0,i0)
#define _STMIB_U(r0,i0)		_CC_STMIB_U(ARM_CC_AL,r0,i0)
#define _CC_STMDA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M,r0,i0)
#define _STMDA(r0,i0)		_CC_STMDA(ARM_CC_AL,r0,i0)
#define _CC_STMDA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_U,r0,i0)
#define _STMDA_U(r0,i0)		_CC_STMDA_U(ARM_CC_AL,r0,i0)
#define _CC_STMDB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_B,r0,i0)
#define _STMDB(r0,i0)		_CC_STMDB(ARM_CC_AL,r0,i0)
#define _CC_STMDB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_B|ARM_M_U,r0,i0)
#define _STMDB_U(r0,i0)		_CC_STMDB_U(ARM_CC_AL,r0,i0)
#define _CC_PUSH(cc,i0)		_CC_STMDB_U(cc,JIT_SP,i0)
#define _PUSH(i0)		_CC_PUSH(ARM_CC_AL,i0)
#define _CC_POP(cc,i0)		_CC_LDMIA_U(cc,JIT_SP,i0)
#define _POP(i0)		_CC_POP(ARM_CC_AL,i0)

#endif /* __lightning_asm_h */
