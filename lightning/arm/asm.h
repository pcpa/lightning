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
    _R9,	/* variable (real frame pointer) */
    _R10,	/* sl - stack limit */
    _R11,	/* fp - frame pointer */
    _R12,	/* ip - temporary */
    _R13,	/* sp - stack pointer */
    _R14,	/* lr - link register */
    _R15,	/* pc - program counter */
} jit_gpr_t;

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

#define ARM_I		0x02000000	/* immediate */
#define ARM_S		0x00100000	/* set flags */
#define ARM_ADD		0x00800000
#define ARM_ADC		0x00a00000
#define ARM_SUB		0x00400000
#define ARM_SBC		0x00c00000
#define ARM_RSB		0x00600000
#define ARM_RSC		0x00e00000
#define ARM_MUL		0x00000090
#define ARM_MLA		0x00200090

#define ARM_AND		0x00000000
#define ARM_BIC		0x01c00000
#define ARM_ORR		0x01800000
#define ARM_EOR		0x00200000

#define ARM_ASR		0x01a00050
#define ARM_LSR		0x01a00030
#define ARM_LSL		0x01a00010
#define ARM_ROR		0x01a00070
#define ARM_ASRI	0x01a00040
#define ARM_LSRI	0x01a00020
#define ARM_LSLI	0x01a00000
#define ARM_RORI	0x01a00060

#define ARM_MOV		0x01a00000
#define ARM_MVN		0x01e00000

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
#define ARM_STRB	0x07400000
#define ARM_STRBI	0x05400000
#define ARM_STRH	0x010000b0
#define ARM_STRHI	0x014000b0
#define ARM_STR		0x07000000
#define ARM_STRI	0x05000000

/* from binutils */
#define rotate_left(v, n)	(v << n | v >> (32 - n))
static int
encode_arm_immediate(int v)
{
    unsigned int	a, i;

    for (i = 0; i < 32; i += 2)
	if ((a = rotate_left (v, i)) <= 0xff)
	    return (a | (i << 7));

    return (-1);
}

__jit_inline void
_orr(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r0)<<12)|_u4(r1));
}

__jit_inline void
_ori(jit_state_t _jit, int o, jit_gpr_t r0, int i0)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r0)<<12)|_u12(i0));
}

__jit_inline void
_orrr(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u4(r2));
}

__jit_inline void
_orri(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u12(i0));
}

__jit_inline void
_orri8(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u8(i0));
}

__jit_inline void
_orrrr(jit_state_t _jit, int o,
       jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, jit_gpr_t r3)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r0)<<16)|(_u4(r1)<<12)|(_u4(r2)<<8)|_u4(r3));
}

__jit_inline void
_srrr(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_I(o|(_u4(r0)<<12)|(_u4(r2)<<8)|_u4(r1));
}

__jit_inline void
_srri(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 1 && i0 <= 31);
    _jit_I(o|(_u4(r0)<<12)|(i0<<7)|(_u4(r1)));
}

#define _ADD(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADD, r0, r1, r2)
#define _ADDI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADD|ARM_I, r0, r1, i0)
#define _ADDS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADD|ARM_S, r0, r1, r2)
#define _ADDSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADD|ARM_S|ARM_I, r0, r1, i0)
#define _ADC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADC, r0, r1, r2)
#define _ADCI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADC|ARM_I, r0, r1, i0)
#define _ADCS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADC|ARM_S, r0, r1, r2)
#define _ADCSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADC|ARM_S|ARM_I, r0, r1, i0)
#define _SUB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SUB, r0, r1, r2)
#define _SUBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SUB|ARM_I, r0, r1, i0)
#define _SUBS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SUB|ARM_S, r0, r1, r2)
#define _SUBSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SUB|ARM_S|ARM_I, r0, r1, i0)
#define _SBC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SBC, r0, r1, r2)
#define _SBCI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SBC|ARM_I, r0, r1, i0)
#define _SBCS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SBC|ARM_S, r0, r1, r2)
#define _SBCSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SBC|ARM_S|ARM_I, r0, r1, i0)
#define _RSB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_RSB, r0, r1, r2)
#define _RSBI(r0, r1, i0)	_orrr(_jit,ARM_CC_AL|ARM_RSB|ARM_I, r0, r1, i0)
#define _RSC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_RSC, r0, r1, r2)
#define _RSCI(r0, r1, i0)	_orrr(_jit,ARM_CC_AL|ARM_RSC|ARM_I, r0, r1, i0)
#define _MUL(r0, r1, r2)	_orrrr(_jit, ARM_CC_AL|ARM_MUL, r0, 0, r2, r1)
#define _MLA(r0, r1, r2, r3)	_orrrr(_jit, ARM_CC_AL|ARM_MLA, r0, r3, r2, r1)

#define _AND(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_AND, r0, r1, r2)
#define _ANDI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_AND|ARM_I, r0, r1, i0)
#define _BIC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_BIC, r0, r1, r2)
#define _BICI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_BIC|ARM_I, r0, r1, i0)
#define _OR(r0, r1, r2)		_orrr(_jit,ARM_CC_AL|ARM_ORR, r0, r1, r2)
#define _ORI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ORR|ARM_I, r0, r1, i0)
#define _XOR(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_EOR, r0, r1, r2)
#define _XORI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_EOR|ARM_I, r0, r1, i0)
#define _MOV(r0, r1)		_orr(_jit,ARM_CC_AL|ARM_MOV, r0, r1)
#define _MOVI(r0, i0)		_ori(_jit,ARM_CC_AL|ARM_MOV|ARM_I, r0, i0)
#define _MVN(r0, r1)		_orr(_jit,ARM_CC_AL|ARM_MVN, r0, r1)
#define _MVNI(r0, i0)		_ori(_jit,ARM_CC_AL|ARM_MVN|ARM_I, r0, i0)

#define _NOT(r0, r1)		_orr(_jit,ARM_CC_AL|ARM_MVN, r0, r1)

#define _ASR(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_ASR, r0, r1, r2)
#define _LSR(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_LSL, r0, r1, r2)
#define _LSL(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_LSL, r0, r1, r2)
#define _ROR(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_ROR, r0, r1, r2)
#define _ASRI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_ASRI, r0, r1, i0)
#define _LSRI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_LSRI, r0, r1, i0)
#define _LSLI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_LSLI, r0, r1, i0)
#define _RORI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_RORI, r0, r1, i0)

#define _LDRSB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSB|ARM_P, r0, r1, r2)
#define _LDRSBN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSB, r0, r1, r2)
#define _LDRSBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSBI|ARM_P, r0, r1, i0)
#define _LDRSBIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSBI, r0, r1, i0)
#define _LDRB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRB|ARM_P, r0, r1, r2)
#define _LDRBN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRB, r0, r1, r2)
#define _LDRBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRBI|ARM_P, r0, r1, i0)
#define _LDRBIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRBI, r0, r1, i0)
#define _LDRSH(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSH|ARM_P, r0, r1, r2)
#define _LDRSHN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSH, r0, r1, r2)
#define _LDRSHI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSHI|ARM_P, r0, r1, i0)
#define _LDRSHIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSHI, r0, r1, i0)
#define _LDRH(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRH|ARM_P, r0, r1, r2)
#define _LDRHN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRH, r0, r1, r2)
#define _LDRHI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRHI|ARM_P, r0, r1, i0)
#define _LDRHIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRHI, r0, r1, i0)
#define _LDR(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDR|ARM_P, r0, r1, r2)
#define _LDRN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDR, r0, r1, r2)
#define _LDRI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRI|ARM_P, r0, r1, i0)
#define _LDRIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRI, r0, r1, i0)
#define _STRB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRB|ARM_P, r2, r1, r0)
#define _STRBN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRB, r2, r1, r0)
#define _STRBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRBI|ARM_P, r1, r0, i0)
#define _STRBIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRBI, r1, r0, i0)
#define _STRH(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRH|ARM_P, r2, r1, r0)
#define _STRHN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRH, r2, r1, r0)
#define _STRHI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRHI|ARM_P, r1, r0, i0)
#define _STRHIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRHI, r1, r0, i0)
#define _STR(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STR|ARM_P, r2, r1, r0)
#define _STRN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STR, r2, r1, r0)
#define _STRI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRI|ARM_P, r1, r0, i0)
#define _STRIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRI, r1, r0, i0)

/*
*ADC, ADD	Add with Carry, Add page 3-50 All
ADR		Load program or register-relative address (short range) page 3-24 All
ADRL		pseudo-instruction Load program or register-relative address (medium range) page 3-155 x6M
*AND		Logical AND page 3-56 All
*ASR		Arithmetic Shift Right page 3-71 All
B		Branch page 3-116 All
BFC, BFI	Bit Field Clear and Insert page 3-109 T2
*BIC		Bit Clear page 3-56 All
BKPT		Breakpoint page 3-134 5
BL		Branch with Link page 3-116 All
BLX		Branch with Link, change instruction set page 3-116 T
BX		Branch, change instruction set page 3-116 T
BXJ		Branch, change to Jazelle® page 3-116 J, x7M
CBZ, CBNZ	Compare and Branch if {Non}Zero page 3-122 T2
CDP		Coprocessor Data Processing operation page 3-125 x6M
CDP2		Coprocessor Data Processing operation page 3-125 5, x6M
CHKA		Check array page 3-152 EE
CLREX		Clear Exclusive page 3-42 K, x6M
CLZ		Count leading zeros page 3-58 5, x6M
CMN, CMP	Compare Negative, Compare page 3-59 All
CPS		Change Processor State page 3-140 6
DBG		Debug page 3-146 7
DMB, DSB	Data Memory Barrier, Data Synchronization Barrier page 3-147 7, 6M
ENTERX, LEAVEX	Change state to or from ThumbEE page 3-151 EE
*EOR		Exclusive OR page 3-56 All
HB,HBL,HBLP,HBP	Handler Branch, branches to a specified handler page 3-153 EE
ISB		Instruction Synchronization Barrier page 3-147 7, 6M
IT		If-Then page 3-119 T2
LDC		Load Coprocessor page 3-131 x6M
LDC2		Load Coprocessor page 3-131 5, x6M
LDM		Load Multiple registers page 3-30 All
*LDR		Load Register with word page 3-9 All
LDR		pseudo-instruction Load Register pseudo-instruction page 3-158 All
*LDRB		Load Register with byte page 3-9 All
LDRBT		Load Register with byte, user mode page 3-9 x6M
LDRD		Load Registers with two words page 3-9 5E, x6M
LDREX		Load Register Exclusive page 3-39 6, x6M
LDREXB, LDREXH	Load Register Exclusive Byte, Halfword page 3-39 K, x6M
LDREXD		Load Register Exclusive Doubleword page 3-39 K, x7M
*LDRH		Load Register with halfword page 3-9 All
LDRHT		Load Register with halfword, user mode page 3-9 T2
*LDRSB		Load Register with signed byte page 3-9 All
LDRSBT		Load Register with signed byte, user mode page 3-9 T2
*LDRSH		Load Register with signed halfword page 3-9 All
LDRSHT		Load Register with signed halfword, user mode page 3-9 T2
LDRT		Load Register with word, user mode page 3-9 x6M
*LSL, LSR	Logical Shift Left, Logical Shift Right page 3-71 All
MAR		Move from Registers to 40-bit Accumulator page 3-149 XScale
MCR		Move from Register to Coprocessor page 3-126 x6M
MCR2		Move from Register to Coprocessor page 3-126 5, x6M
MCRR		Move from Registers to Coprocessor page 3-126 5E, x6M
MCRR2		Move from Registers to Coprocessor page 3-126 6, x6M
MIA,MIAPH,MIAxy	Multiply with Internal 40-bit Accumulate page 3-94 XScale
*MLA		Multiply Accumulate page 3-76 x6M
MLS		Multiply and Subtract page 3-76 T2
*MOV		Move page 3-61 All
MOVT		Move Top page 3-64 T2
MOV32		pseudo-instruction Move 32-bit immediate to register page 3-157 T2
MRA		Move from 40-bit Accumulator to Registers page 3-149 XScale
MRC		Move from Coprocessor to Register page 3-127 x6M
MRC2		Move from Coprocessor to Register page 3-127 5, x6M
MRRC		Move from Coprocessor to Registers page 3-127 5E, x6M
MRRC2		Move from Coprocessor to Registers page 3-127 6, x6M
MRS		Move from PSR to register page 3-136 All
MRS		Move from system Coprocessor to Register page 3-129 7A, 7R
MSR		Move from register to PSR page 3-138 All
MSR		Move from Register to system Coprocessor page 3-128 7A, 7R
*MUL		Multiply page 3-76 All
*MVN		Move Not page 3-61 All
*NOP		No Operation page 3-143 All
ORN		Logical OR NOT page 3-56 T2
*ORR		Logical OR page 3-56 All
PKHBT,PKHTB	Pack Halfwords page 3-113 6, 7EM
PLD		Preload Data page 3-28 5E, x6M
PLDW		Preload Data with intent to Write page 3-28 7MP
PLI		Preload Instruction page 3-28 7
PUSH, POP	PUSH registers to stack, POP registers from stack page 3-33 All
QADD, QDADD,	Saturating Arithmetic page 3-97 5E, 7EM
QDSUB, QSUB
QADD8, QADD16,	Parallel signed Saturating Arithmetic page 3-102 6, 7EM
QASX, QSUB8,
QSUB16, QSAX 
RBIT		Reverse Bits page 3-69 T2
REV,REV16,REVSH	Reverse byte order page 3-69 6
RFE		Return From Exception page 3-35 T2, x7M
*ROR		Rotate Right Register page 3-71 All
RRX		Rotate Right with Extend page 3-71 x6M
*RSB		Reverse Subtract page 3-50 All
*RSC		Reverse Subtract with Carry page 3-50 x7M
SADD8, SADD16,	Parallel signed arithmetic page 3-102 6, 7EM
SASX
*SBC		Subtract with Carry page 3-50 All
SBFX, UBFX	Signed, Unsigned Bit Field eXtract page 3-110 T2
SDIV		Signed divide page 3-74 7M, 7R
SEL		Select bytes according to APSR GE flags page 3-67 6, 7EM
SETEND		Set Endianness for memory accesses page 3-142 6, x7M
SEV		Set Event page 3-144 K, 6M
SHADD8,SHADD16,	Parallel signed Halving arithmetic page 3-102 6, 7EM
SHASX, SHSUB8,
SHSUB16, SHSAX 
SMC		Secure Monitor Call page 3-141 Z
SMLAxy		Signed Multiply with Accumulate (32 <= 16 x 16 + 32) page 3-80 5E, 7EM
SMLAD		Dual Signed Multiply Accumulate page 3-89 6, 7EM
		(32 <= 32 + 16 x 16 + 16 x 16)
SMLAL		Signed Multiply Accumulate (64 <= 64 + 32 x 32) page 3-78 x6M
SMLALxy		Signed Multiply Accumulate (64 <= 64 + 16 x 16) page 3-83 5E, 7EM
SMLALD		Dual Signed Multiply Accumulate Long page 3-91 6, 7EM
		(64 <= 64 + 16 x 16 + 16 x 16)
SMLAWy		Signed Multiply with Accumulate (32 <= 32 x 16 + 32) page 3-82 5E, 7EM
SMLSD		Dual Signed Multiply Subtract Accumulate page 3-89 6, 7EM
		(32 <= 32 + 16 x 16 - 16 x 16)
SMLSLD		Dual Signed Multiply Subtract Accumulate Long page 3-91 6, 7EM
		(64 <= 64 + 16 x 16 - 16 x 16)
SMMLA		Signed top word Multiply with Accumulate (32 <= page 3-87 6, 7EM
		TopWord(32 x 32 + 32)) 
SMMLS		Signed top word Multiply with Subtract (32 <= page 3-87 6, 7EM
		TopWord(32 - 32 x 32)) 
SMMUL		Signed top word Multiply (32 <= TopWord(32 x 32)) page 3-87 6, 7EM
SMUAD, SMUSD	Dual Signed Multiply, and Add or Subtract products page 3-85 6, 7EM
SMULxy		Signed Multiply (32 <= 16 x 16) page 3-80 5E, 7EM
SMULL		Signed Multiply (64 <= 32 x 32) page 3-78 x6M
SMULWy		Signed Multiply (32 <= 32 x 16) page 3-82 5E, 7EM
SRS		Store Return State page 3-37 T2, x7M
SSAT		Signed Saturate page 3-99 6, x6M
SSAT16		Signed Saturate, parallel halfwords page 3-106 6, 7EM
SSUB8, SSUB16,	Parallel signed arithmetic page 3-102 6, 7EM
SSAX
STC		Store Coprocessor page 3-131 x6M
STC2		Store Coprocessor page 3-131 5, x6M
STM		Store Multiple registers page 3-30 All
*STR		Store Register with word page 3-9 All
*STRB		Store Register with byte page 3-9 All
STRBT		Store Register with byte, user mode page 3-9 x6M
STRD		Store Registers with two words page 3-9 5E, x6M
STREX		Store Register Exclusive page 3-39 6, x6M
STREXB, STREXH	Store Register Exclusive Byte, Halfword page 3-39 K, x6M
STREXD		Store Register Exclusive Doubleword page 3-39 K, x7M
*STRH		Store Register with halfword page 3-9 All
STRHT		Store Register with halfword, user mode page 3-9 T2
STRT		Store Register with word, user mode page 3-9 x6M
*SUB		Subtract page 3-50 All
SUBS		pc, lr Exception return, no stack page 3-54 T2, x7M
SVC		SuperVisor Call page 3-135 All
(formerly SWI)
SWP, SWPB	Swap registers and memory (ARM only) page 3-43 All, x7M
SXTAB, SXTAB16,	Signed extend, with Addition page 3-111 6, 7EM
SXTAH
SXTB, SXTH	Signed extend page 3-111 6
SXTB16		Signed extend page 3-111 6, 7EM
SYS		Execute system coprocessor instruction page 3-130 7A, 7R
TBB, TBH	Table Branch Byte, Halfword page 3-123 T2
TEQ		Test Equivalence page 3-65 x6M
TST		Test page 3-65 All
UADD8, UADD16,	Parallel Unsigned Arithmetic page 3-102 6, 7EM
UASX
UDIV		Unsigned divide page 3-74 7M, 7R
UHADD8,UHADD16,	Parallel Unsigned Halving Arithmetic page 3-102 6, 7EM
UHASX, UHSUB8,
UHSUB16, UHSAX 
UMAAL		Unsigned Multiply Accumulate Accumulate Long page 3-93 6, 7EM
		(64 <= 32 + 32 + 32 x 32)
UMLAL, UMULL	Unsigned Multiply Accumulate, Unsigned Multiply page 3-78 x6M
		(64 <= 32 x 32 + 64), (64 <= 32 x 32)
UQADD8,UQADD16,	Parallel Unsigned Saturating Arithmetic page 3-102 6, 7EM
UQASX, UQSUB8,
UQSUB16, UQSAX 
USAD8		Unsigned Sum of Absolute Differences page 3-104 6, 7EM
USADA8		Accumulate Unsigned Sum of Absolute Differences page 3-104 6, 7EM
USAT		Unsigned Saturate page 3-99 6, x6M
USAT16		Unsigned Saturate, parallel halfwords page 3-106 6, 7EM
USUB8, USUB16,	Parallel unsigned arithmetic page 3-102 6, 7EM
USAX
UXTAB,UXTAB16,	Unsigned extend with Addition page 3-111 6, 7EM
UXTAH
UXTB, UXTH	Unsigned extend page 3-111 6
UXTB16		Unsigned extend page 3-111 6, 7EM
V*		See Chapter 4 NEON and VFP Programming
FE, WFI, YIELD	Wait For Event, Wait For Interrupt, Yield page 3-144 T2, 6M

All	These instructions are available in all versions of the ARM architecture.
5	These instructions are available in the ARMv5T*, ARMv6*, and ARMv7 architectures.
5E	These instructions are available in the ARMv5TE, ARMv6*, and ARMv7 architectures.
6	These instructions are available in the ARMv6* and ARMv7 architectures.
6M	These instructions are available in the ARMv6-M and ARMv7 architectures.
x6M	These instructions are not available in the ARMv6-M architecture.
7	These instructions are available in the ARMv7 architectures.
7M	These instructions are available in the ARMv7-M architecture, including ARMv7E-M implementations.
x7M	These instructions are not available in the ARMv6-M or ARMv7-M architecture, or any ARMv7E-M
	implementation.
7EM	These instructions are available in ARMv7E-M implementations but not in the ARMv7-M or ARMv6-M
	architecture.
7R	These instructions are available in the ARMv7-R architecture.
7MP	These instructions are available in the ARMv7 architectures that implement the Multiprocessing Extensions.
EE	These instructions are available in ThumbEE variants of the ARM architecture.
J	This instruction is available in the ARMv5TEJ, ARMv6*, and ARMv7 architectures.
K	These instructions are available in the ARMv6K, and ARMv7 architectures.
T	These instructions are available in ARMv4T, ARMv5T*, ARMv6*, and ARMv7 architectures.
T2	These instructions are available in the ARMv6T2 and above architectures.
XScale	These instructions are available in XScale versions of the ARM architecture.
Z	This instruction is available if Security Extensions are implemented.
*/

#endif /* __lightning_asm_h */
