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

/* _VxxxQ macros mean _Q0=_F0, _Q1=_F2, ... */
typedef enum {
    _F0,	/* result */
    _F1,	/* scratch */
    _F2,	/* scratch */
    _F3,	/* scratch */
    _F4,	/* variable */
    _F5,	/* variable */
    _F6,	/* variable */
    _F7,	/* variable */
    JIT_FPRET,	/* for abstraction of returning a float/double result */
} jit_fpr_t;

#define ARM_CC_EQ	0x00000000	/* Z=1 */
#define ARM_CC_NE	0x10000000	/* Z=0 */
#define ARM_CC_HS	0x20000000	/* C=1 */
#  define ARM_CC_CS	ARM_CC_HS
#define ARM_CC_LO	0x30000000	/* C=0 */
#  define ARM_CC_CC	ARM_CC_LO
#define ARM_CC_MI	0x40000000	/* N=1 */
#define ARM_CC_PL	0x50000000	/* N=0 */
#define ARM_CC_VS	0x60000000	/* V=1 */
#define ARM_CC_VC	0x70000000	/* V=0 */
#define ARM_CC_HI	0x80000000	/* C=1 && Z=0 */
#define ARM_CC_LS	0x90000000	/* C=0 || Z=1 */
#define ARM_CC_GE	0xa0000000	/* N=V */
#define ARM_CC_LT	0xb0000000	/* N!=V */
#define ARM_CC_GT	0xc0000000	/* Z=0 && N=V */
#define ARM_CC_LE	0xd0000000	/* Z=1 || N!=V */
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
/* << ARMv6* */
#define ARM_REV		0x06b00f30
#define ARM_REV16	0x06b00fb0
#define ARM_SXTB	0x06af0070
#define ARM_UXTB	0x06ef0070
#define ARM_SXTH	0x06bf0070
#define ARM_UXTH	0x06ff0070
#define ARM_XTR8	0x00000400	/* ?xt? rotate 8 bits */
#define ARM_XTR16	0x00000800	/* ?xt? rotate 16 bits */
#define ARM_XTR24	0x00000c00	/* ?xt? rotate 24 bits */
/* >> ARMv6* */

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

#define ARM_V_Q		0x00000040
#define FPSCR_N		0x80000000/* Negative condition code flag */
#define FPSCR_Z		0x40000000/* Zero condition code flag */
#define FPSCR_C		0x20000000/* Carry condition code flag */
#define FPSCR_V		0x10000000/* Overflow condition code flag */
#define FPSCR_QC	0x08000000/* Cumulative saturation flag */
#define FPSCR_AHP	0x04000000/* Alternative half-precision (unset is IEEE format) */
#define FPSCR_DN	0x02000000/* Default NaN mode */
#define FPSCR_FZ	0x01000000/* Flush to zero (unset is fully IEEE-754 compliant) */
#define FPSCR_RMASK	0x00c00000
#  define FPSCR_RN	0x00000000	/* Round to Nearest */
#  define FPSCR_RP	0x00400000	/* Round towards Plus Infinity */
#  define FPSCR_RM	0x00800000	/* Round towards Minus Infinity */
#  define FPSCR_RZ	0x00c00000	/* Round towards Zero */
#define FPSCR_STRIDE	0x00300000
#define FPSCR_RES1	0x00080000/* Reserved, UNK/SBZP */
#define FPSCR_LEN	0x00070000
#define FPSCR_IDE	0x00008000/* Input Denormal exception trap enable */
#define FPSCR_IXE	0x00001000/* Inexact exception trap enable */
#define FPSCR_UFE	0x00000800/* Underflow exception trap enable */
#define FPSCR_OFE	0x00000400/* Overflow exception trap enable */
#define FPSCR_DZE	0x00000200/* Division by zero exception trap enable */
#define FPSCR_IOE	0x00000100/* Invalid Operation exception trap enable */
#define FPSCR_IDC	0x00000080/* Input Denormal cumulative exception flag */
#define FPSCR_RES0	0x00000060/* Reserved, UNK/SBZP */
#define FPSCR_IXC	0x00000010/* Inexact cumulative exception flag */
#define FPSCR_UFC	0x00000008/* Underflow cumulative exception flag */
#define FPSCR_OFC	0x00000004/* Overflow cumulative exception flag */
#define FPSCR_DZC	0x00000002/* Division by zero cumulative exception flag */
#define FPSCR_IOC	0x00000001/* Invalid Operation cumulative exception flag */

/***********************************************************************
 * VFPv2 and VFPv3 (encoding T2/A2) instructions
 ***********************************************************************/
#define ARM_V_E		0x00000080	/* ARM_VCMP exception if NaN arg(s) */
#define ARM_V_Z		0x00010000	/* ARM_VCMP with zero */
#define ARM_V_F64	0x00000100	/* Undefined in single precision only variant */
#define ARM_VADD_F	0x0e300a00
#define ARM_VSUB_F	0x0e300a40
#define ARM_VMUL_F	0x0e200a00
#define ARM_VDIV_F	0x0e800a00
#define ARM_VABS_F	0x0eb00ac0
#define ARM_VNEG_F	0x0eb10a40
#define ARM_VSQRT_F	0x0eb10ac0
#define ARM_VMOV_F	0x0eb00a40
#define ARM_VMOV_A_S	0x0e100b10	/* vmov rn, sn */
#define ARM_VMOV_S_A	0x0e000a10	/* vmov sn, rn */
#define ARM_VMOV_AA_D	0x0c500b10	/* vmov rn,rn, dn */
#define ARM_VMOV_D_AA	0x0c400b10	/* vmov dn, rn,rn */
#define ARM_VCMP	0x0eb40a40
#define ARM_VMRS	0x0ef10a10
#define ARM_VMSR	0x0ee10a10
#define ARM_VCVT_2I		0x00040000	/* to integer */
#define ARM_VCVT_2S		0x00010000	/* to signed */
#define ARM_VCVT_RS		0x00000080	/* round to zero or signed */
#define ARM_VCVT		0x0eb80a40
#define ARM_VCVT_S32_F32	ARM_VCVT|ARM_VCVT_2I|ARM_VCVT_2S|ARM_VCVT_RS
#define ARM_VCVT_U32_F32	ARM_VCVT|ARM_VCVT_2I|ARM_VCVT_RS
#define ARM_VCVT_S32_F64	ARM_VCVT|ARM_VCVT_2I|ARM_VCVT_2S|ARM_VCVT_RS|ARM_V_F64
#define ARM_VCVT_U32_F64	ARM_VCVT|ARM_VCVT_2I|ARM_VCVT_RS|ARM_V_F64
#define ARM_VCVT_F32_S32	ARM_VCVT|ARM_VCVT_RS
#define ARM_VCVT_F32_U32	ARM_VCVT
#define ARM_VCVT_F64_S32	ARM_VCVT|ARM_VCVT_RS|ARM_V_F64
#define ARM_VCVT_F64_U32	ARM_VCVT|ARM_V_F64
#define ARM_VCVT_F		0x0eb70ac0
#define ARM_VCVT_F32_F64	ARM_VCVT_F
#define ARM_VCVT_F64_F32	ARM_VCVT_F|ARM_V_F64

/* does not set bit 7, meaning to use rounding mode of FPSCR */
#define ARM_VCVTR_S32_F32	ARM_VCVT|ARM_VCVT_2I|ARM_VCVT_2S
#define ARM_VCVTR_U32_F32	ARM_VCVT|ARM_VCVT_2I
#define ARM_VCVTR_S32_F64	ARM_VCVT|ARM_VCVT_2I|ARM_VCVT_2S|ARM_V_F64
#define ARM_VCVTR_U32_F64	ARM_VCVT|ARM_VCVT_2I|ARM_V_F64

/***********************************************************************
 * NEON instructions (encoding T1/A1) (condition must always be ARM_CC_NV)
 ***********************************************************************/
#define ARM_V_U		0x01000000
#define ARM_V_I16	0x00100000
#define ARM_V_I32	0x00200000
#define ARM_V_I64	0x00300000
#define ARM_V_S16	0x00040000
#define ARM_V_S32	0x00080000
#define ARM_VADD_I	0x02800800
#define ARM_VQADD_I	0x02000010	/* sets flag on overflow/carry */
#define ARM_VADDL_I	0x02800000	/* q=d+d */
#define ARM_VADDW_I	0x02800100	/* q=q+d */
#define ARM_VSUB_I	0x03000800
#define ARM_VQSUB_I	0x02000210	/* sets flag on overflow/carry */
#define ARM_VSUBL_I	0x02800200
#define ARM_VSUBW_I	0x02800300
#define ARM_VMUL_I	0x02000910
#define ARM_VMULL_I	0x02800c00

#define ARM_VABS_I	0x03b10300
#define ARM_VQABS_I	0x03b00700	/* sets flag on overflow */
#define ARM_VNEG_I	0x03b10380
#define ARM_VQNEG_I	0x03b00780	/* sets flag on overflow */

#define ARM_VAND	0x02000110
#define ARM_VBIC	0x02100110
#define ARM_VORR	0x02200110
#define ARM_VORN	0x02300110
#define ARM_VEOR	0x03000110
#define ARM_VMOVL_S8	0x00080000
#define ARM_VMOVL_S16	0x00100000
#define ARM_VMOVL_S32	0x00200000
#define ARM_VMOVL_I	0x02800a10

#define ARM_VMOVI	0x02800010
#define ARM_VMVNI	0x02800030

#define ARM_VLDR	0x0d100a00
#define ARM_VSTR	0x0d000a00
#define ARM_VM		0x0c000a00

/***********************************************************************
 * Advanced SIMD (encoding T2/A2) instructions
 ***********************************************************************/
#define ARM_VMOV_ADV_U	0x00800000 	/* zero extend, sign extend if unset */
#define ARM_VMOV_ADV_8	0x00400000
#define ARM_VMOV_ADV_16	0x00000020
#define ARM_VMOV_A_D	0x0e100b10
#define ARM_VMOV_D_A	0x0e000b10

/*
 * FIXME add (T2/A2) encoding for VFPv3 conditional VMOV with different
 * position of imm8 (split in imm8H and imm8L)
 */
static int
encode_vfp_immediate(int code, unsigned lo, unsigned hi)
{
    int		mode, imm, mask;

    if (hi != lo) {
	if (code == ARM_VMOVI) {
	    /* (I64)
	     *	aaaaaaaabbbbbbbbccccccccddddddddeeeeeeeeffffffffgggggggghhhhhhhh
	     */
	    for (mode = 0, mask = 0xff; mode < 4; mask <<= 8, mode++) {
		imm = lo & mask;
		if (imm != mask && imm != 0)
		    goto fail;
		imm = hi & mask;
		if (imm != mask && imm != 0)
		    goto fail;
	    }
	    mode = 0xe20;
	    imm = (((hi & 0x80000000) >> 24) | ((hi & 0x00800000) >> 17) |
		   ((hi & 0x00008000) >> 10) | ((hi & 0x00000080) >>  3) |
		   ((lo & 0x80000000) >> 28) | ((lo & 0x00800000) >> 21) |
		   ((lo & 0x00008000) >> 14) | ((lo & 0x00000080) >>  7));
	    goto success;
	}
	goto fail;
    }
    if (code == ARM_VMOVI || code == ARM_VMVNI) {
	/*  (I32)
	 *  00000000 00000000 00000000 abcdefgh
	 *  00000000 00000000 abcdefgh 00000000
	 *  00000000 abcdefgh 00000000 00000000
	 *  abcdefgh 00000000 00000000 00000000 */
	for (mode = 0, mask = 0xff; mode < 4; mask <<= 8, mode++) {
	    if ((lo & mask) == lo) {
		imm = lo >> (mode << 3);
		mode <<= 9;
		goto success;
	    }
	}
	/*  (I16)
	 *  00000000 abcdefgh 00000000 abcdefgh
	 *  abcdefgh 00000000 abcdefgh 00000000 */
	for (mode = 0, mask = 0xff; mode < 2; mask <<= 8, mode++) {
	    if ((lo & mask) && ((lo & (mask << 16)) >> 16) == (lo & mask)) {
		imm = lo >> (mode << 3);
		mode = 0x800 | (mode << 9);
		goto success;
	    }
	}
	/*  (I32)
	 *  00000000 00000000 abcdefgh 11111111
	 *  00000000 abcdefgh 11111111 11111111 */
	for (mode = 0, mask = 0xff; mode < 2;
	     mask = (mask << 8) | 0xff, mode++) {
	    if ((lo & mask) == mask &&
		!((lo & ~mask) >> 8) &&
		(imm = lo >> (8 + (mode << 8)))) {
		mode = 0xc00 | (mode << 8);
		goto success;
	    }
	}
	if (code == ARM_VMOVI) {
	    /* (F32)
	     *  aBbbbbbc defgh000 00000000 00000000
	     *  from the ARM Architecture Reference Manual:
	     *  In this entry, B = NOT(b). The bit pattern represents the
	     *  floating-point number (-1)^s* 2^exp * mantissa, where
	     *  S = UInt(a),
	     *  exp = UInt(NOT(b):c:d)-3 and
	     *  mantissa = (16+UInt(e:f:g:h))/16. */
	    if ((lo & 0x7ffff) == 0 &&
		(((lo & 0x7e000000) == 0x3e000000) ||
		 ((lo & 0x7e000000) == 0x40000000))) {
		mode = 0xf00;
		imm = ((lo >> 24) & 0x80) | ((lo >> 19) & 0x7f);
		goto success;
	    }
	}
    }

fail:
    /* need another approach (load from memory, move from arm register, etc) */
    return (-1);

success:
    imm = ((imm & 0x80) << 17) | ((imm & 0x70) << 12) | (imm & 0x0f);
    return (code | mode | imm);
}

#define arm_voir(oi,r0)		_arm_voir(_jit,oi,r0)
__jit_inline void
_arm_voir(jit_state_t _jit, int oi, int r0)
{
    assert(!(oi  & 0x0000f000));
    _jit_I(ARM_CC_NV|oi|(_u4(r0)<<12));
}

#define arm_vo_rr(o,r0,r1)	 _arm_cc_vo_rr(_jit,ARM_CC_NV,o,r0,r1)
#define arm_cc_vo_rr(cc,o,r0,r1) _arm_cc_vo_rr(_jit,cc,o,r0,r1)
__jit_inline void
_arm_cc_vo_rr(jit_state_t _jit, int cc, int o, int r0, int r1)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf000f00f));
    _jit_I(cc|o|(_u4(r0)<<12)|_u4(r1));
}

#define arm_vorr_(o,r0,r1)	 _arm_cc_vorr_(_jit,ARM_CC_NV,o,r0,r1)
#define arm_cc_vorr_(cc,o,r0,r1) _arm_cc_vorr_(_jit,cc,o,r0,r1)
__jit_inline void
_arm_cc_vorr_(jit_state_t _jit, int cc, int o, int r0, int r1)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf000f00f));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12));
}

#define arm_vorrr(o,r0,r1,r2)	    _arm_cc_vorrr(_jit,ARM_CC_NV,o,r0,r1,r2)
#define arm_cc_vorrr(cc,o,r0,r1,r2) _arm_cc_vorrr(_jit,cc,o,r0,r1,r2)
__jit_inline void
_arm_cc_vorrr(jit_state_t _jit, int cc, int o, int r0, int r1, int r2)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00ff00f));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u4(r2));
}

#define arm_cc_vldst(cc,o,r0,r1,i0) _arm_cc_vldst(_jit,cc,o,r0,r1,i0)
__jit_inline void
_arm_cc_vldst(jit_state_t _jit, int cc, int o, int r0, int r1, int i0)
{
    /* i0 << 2 is byte offset */
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00ff0ff));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u8(i0));
}

#define arm_cc_vorrl(cc,o,r0,r1,i0) _arm_cc_vorrl(_jit,cc,o,r0,r1,i0)
__jit_inline void
_arm_cc_vorrl(jit_state_t _jit, int cc, int o, int r0, int r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00ff0ff));
    assert(i0 && !(i0 & 1) && r1 + i0 <= 32);
    _jit_I(cc|o|(_u4(r0)<<16)|(_u4(r1)<<12)|_u8(i0));
}

/***********************************************************************
 * VFPv2 and VFPv3 (encoding T2/A2) instructions
 ***********************************************************************/
#define _CC_VADD_F32(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VADD_F,r0,r1,r2)
#define _VADD_F32(r0,r1,r2)		_CC_VADD_F32(ARM_CC_AL,r0,r1,r2)
#define _CC_VADD_F64(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VADD_F|ARM_V_F64,r0,r1,r2)
#define _VADD_F64(r0,r1,r2)		_CC_VADD_F64(ARM_CC_AL,r0,r1,r2)
#define _CC_VSUB_F32(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VSUB_F,r0,r1,r2)
#define _VSUB_F32(r0,r1,r2)		_CC_VSUB_F32(ARM_CC_AL,r0,r1,r2)
#define _CC_VSUB_F64(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VSUB_F|ARM_V_F64,r0,r1,r2)
#define _VSUB_F64(r0,r1,r2)		_CC_VSUB_F64(ARM_CC_AL,r0,r1,r2)
#define _CC_VMUL_F32(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VMUL_F,r0,r1,r2)
#define _VMUL_F32(r0,r1,r2)		_CC_VMUL_F32(ARM_CC_AL,r0,r1,r2)
#define _CC_VMUL_F64(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VMUL_F|ARM_V_F64,r0,r1,r2)
#define _VMUL_F64(r0,r1,r2)		_CC_VMUL_F64(ARM_CC_AL,r0,r1,r2)
#define _CC_VDIV_F32(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VDIV_F,r0,r1,r2)
#define _VDIV_F32(r0,r1,r2)		_CC_VDIV_F32(ARM_CC_AL,r0,r1,r2)
#define _CC_VDIV_F64(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VDIV_F|ARM_V_F64,r0,r1,r2)
#define _VDIV_F64(r0,r1,r2)		_CC_VDIV_F64(ARM_CC_AL,r0,r1,r2)
#define _CC_VABS_F32(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VABS_F,r0,r1)
#define _VABS_F32(r0,r1)		_CC_VABS_F32(ARM_CC_AL,r0,r1)
#define _CC_VABS_F64(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VABS_F|ARM_V_F64,r0,r1)
#define _VABS_F64(r0,r1)		_CC_VABS_F64(ARM_CC_AL,r0,r1)
#define _CC_VNEG_F32(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VNEG_F,r0,r1)
#define _VNEG_F32(r0,r1)		_CC_VNEG_F32(ARM_CC_AL,r0,r1)
#define _CC_VNEG_F64(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VNEG_F|ARM_V_F64,r0,r1)
#define _VNEG_F64(r0,r1)		_CC_VNEG_F64(ARM_CC_AL,r0,r1)
#define _CC_VSQRT_F32(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VSQRT_F,r0,r1)
#define _VSQRT_F32(r0,r1)		_CC_VSQRT_F32(ARM_CC_AL,r0,r1)
#define _CC_VSQRT_F64(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VSQRT_F|ARM_V_F64,r0,r1)
#define _VSQRT_F64(r0,r1)		_CC_VSQRT_F64(ARM_CC_AL,r0,r1)
#define _CC_VMOV_F32(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VMOV_F,r0,r1)
#define _VMOV_F32(r0,r1)		_CC_VMOV_F32(ARM_CC_AL,r0,r1)
#define _CC_VMOV_F64(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VMOV_F|ARM_V_F64,r0,r1)
#define _VMOV_F64(r0,r1)		_CC_VMOV_F64(ARM_CC_AL,r0,r1)
#define _CC_VMOV_AA_D(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VMOV_AA_D,r0,r1,r2)
#define _VMOV_AA_D(r0,r1,r2)		_CC_VMOV_AA_D(ARM_CC_AL,r0,r1,r2)
#define _CC_VMOV_D_AA(cc,r0,r1,r2)	arm_cc_vorrr(cc,ARM_VMOV_D_AA,r1,r2,r0)
#define _VMOV_D_AA(r0,r1,r2)		_CC_VMOV_D_AA(ARM_CC_AL,r0,r1,r2)
#define _CC_VMOV_A_S(cc,r0,r1)		arm_cc_vorr_(cc,ARM_VMOV_A_S,r0,r1)
#define _VMOV_A_S(r0,r1)		_CC_VMOV_A_S(ARM_CC_AL,r0,r1)
#define _CC_VMOV_S_A(cc,r0,r1)		arm_cc_vorr_(cc,ARM_VMOV_S_A,r1,r0)
#define _VMOV_S_A(r0,r1)		_CC_VMOV_S_A(ARM_CC_AL,r0,r1)
#define _CC_VCMP_F32(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VCMP,r0,r1)
#define _VCMP_F32(r0,r1)		_CC_VCMP_F32(ARM_CC_AL,r0,r1)
#define _CC_VCMP_F64(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_F64,r0,r1)
#define _VCMP_F64(r0,r1)		_CC_VCMP_F64(ARM_CC_AL,r0,r1)
#define _CC_VCMPE_F32(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_E,r0,r1)
#define _VCMPE_F32(r0,r1)		_CC_VCMPE_F32(ARM_CC_AL,r0,r1)
#define _CC_VCMPE_F64(cc,r0,r1)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_E|ARM_V_F64,r0,r1)
#define _VCMPE_F64(r0,r1)		_CC_VCMPE_F64(ARM_CC_AL,r0,r1)
#define _CC_VCMPZ_F32(cc,r0)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_Z,r0,0)
#define _VCMPZ_F32(r0)			_CC_VCMPZ_F32(ARM_CC_AL,r0)
#define _CC_VCMPZ_F64(cc,r0)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_Z|ARM_V_F64,r0,0)
#define _VCMPZ_F64(r0)			_CC_VCMPZ_F64(ARM_CC_AL,r0)
#define _CC_VCMPEZ_F32(cc,r0)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_Z|ARM_V_E,r0,0)
#define _VCMPEZ_F32(r0)			_CC_VCMPEZ_F32(ARM_CC_AL,r0)
#define _CC_VCMPEZ_F64(cc,r0)		arm_cc_vo_rr(cc,ARM_VCMP|ARM_V_Z|ARM_V_E|ARM_V_F64,r0,0)
#define _VCMPEZ_F64(r0)			_CC_VCMPEZ_F64(ARM_CC_AL,r0)
#define _CC_VMRS(cc,r0)			arm_cc_vorr_(cc,ARM_VMRS,r0,0)
#define _VMRS(r0)			_CC_VMRS(ARM_CC_AL,r0)
#define _CC_VMSR(cc,r0)			arm_cc_vorr_(cc,ARM_VMSR,r0,0)
#define _VMSR(r0)			_CC_VMSR(ARM_CC_AL,r0)
#define _CC_VCVT_S32_F32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_S32_F32,r0,r1)
#define _VCVT_S32_F32(r0,r1)		_CC_VCVT_S32_F32(ARM_CC_AL,r0,r1)
#define _CC_VCVT_U32_F32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_U32_F32,r0,r1)
#define _VCVT_U32_F32(r0,r1)		_CC_VCVT_U32_F32(ARM_CC_AL,r0,r1)
#define _CC_VCVT_S32_F64(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_S32_F64,r0,r1)
#define _VCVT_S32_F64(r0,r1)		_CC_VCVT_S32_F64(ARM_CC_AL,r0,r1)
#define _CC_VCVT_U32_F64(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_U32_F64,r0,r1)
#define _VCVT_U32_F64(r0,r1)		_CC_VCVT_U32_F64(ARM_CC_AL,r0,r1)
#define _CC_VCVT_F32_S32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_F32_S32,r0,r1)
#define _VCVT_F32_S32(r0,r1)		_CC_VCVT_F32_S32(ARM_CC_AL,r0,r1)
#define _CC_VCVT_F32_U32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_F32_U32,r0,r1)
#define _VCVT_F32_U32(r0,r1)		_CC_VCVT_F32_U32(ARM_CC_AL,r0,r1)
#define _CC_VCVT_F64_S32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_F64_S32,r0,r1)
#define _VCVT_F64_S32(r0,r1)		_CC_VCVT_F64_S32(ARM_CC_AL,r0,r1)
#define _CC_VCVT_F64_U32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_F64_U32,r0,r1)
#define _VCVT_F64_U32(r0,r1)		_CC_VCVT_F64_U32(ARM_CC_AL,r0,r1)
#define _CC_VCVT_F32_F64(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_F32_F64,r0,r1)
#define _VCVT_F32_F64(r0,r1)		_CC_VCVT_F32_F64(ARM_CC_AL,r0,r1)
#define _CC_VCVT_F64_F32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVT_F64_F32,r0,r1)
#define _VCVT_F64_F32(r0,r1)		_CC_VCVT_F64_F32(ARM_CC_AL,r0,r1)
/* use rounding mode in fpscr (intended for floor, ceil, etc) */
#define _CC_VCVTR_S32_F32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVTR_S32_F32,r0,r1)
#define _VCVTR_S32_F32(r0,r1)		_CC_VCVTR_S32_F32(ARM_CC_AL,r0,r1)
#define _CC_VCVTR_U32_F32(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVTR_U32_F32,r0,r1)
#define _VCVTR_U32_F32(r0,r1)		_CC_VCVTR_U32_F32(ARM_CC_AL,r0,r1)
#define _CC_VCVTR_S32_F64(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVTR_S32_F64,r0,r1)
#define _VCVTR_S32_F64(r0,r1)		_CC_VCVTR_S32_F64(ARM_CC_AL,r0,r1)
#define _CC_VCVTR_U32_F64(cc,r0,r1)	arm_cc_vo_rr(cc,ARM_VCVTR_U32_F64,r0,r1)
#define _VCVTR_U32_F64(r0,r1)		_CC_VCVTR_U32_F64(ARM_CC_AL,r0,r1)

/***********************************************************************
 * NEON instructions (encoding T1/A1) (condition must always be ARM_CC_NV)
 ***********************************************************************/
#define _CC_VLDMIA_F32(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_L|ARM_M_I,r0,r1,i0)
#define _VLDMIA_F32(r0,r1,i0)		_CC_VLDMIA_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDMIA_F64(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_L|ARM_M_I|ARM_V_F64,r0,r1,i0)
#define _VLDMIA_F64(r0,r1,i0)		_CC_VLDMIA_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTMIA_F32(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_I,r0,r1,i0)
#define _VSTMIA_F32(r0,r1,i0)		_CC_VSTMIA_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTMIA_F64(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_I|ARM_V_F64,r0,r1,i0)
#define _VSTMIA_F64(r0,r1,i0)		_CC_VSTMIA_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDMIA_U_F32(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_L|ARM_M_I|ARM_M_U,r0,r1,i0)
#define _VLDMIA_U_F32(r0,r1,i0)		_CC_VLDMIA_U_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDMIA_U_F64(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_L|ARM_M_I|ARM_M_U|ARM_V_F64,r0,r1,i0)
#define _VLDMIA_U_F64(r0,r1,i0)		_CC_VLDMIA_U_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTMIA_U_F32(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_I|ARM_M_U,r0,r1,i0)
#define _VSTMIA_U_F32(r0,r1,i0)		_CC_VSTMIA_U_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTMIA_U_F64(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_I|ARM_M_U|ARM_V_F64,r0,r1,i0)
#define _VSTMIA_U_F64(r0,r1,i0)		_CC_VSTMIA_U_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDMDB_U_F32(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_B|ARM_M_U,r0,r1,i0)
#define _VLDMDB_U_F32(r0,r1,i0)		_CC_VLDMDB_U_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDMDB_U_F64(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_B|ARM_M_U|ARM_V_F64,r0,r1,i0)
#define _VLDMDB_U_F64(r0,r1,i0)		_CC_VLDMDB_U_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTMDB_U_F32(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_B|ARM_M_U,r0,r1,i0)
#define _VSTMDB_U_F32(r0,r1,i0)		_CC_VSTMDB_U_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTMDB_U_F64(cc,r0,r1,i0)	arm_cc_vorrl(cc,ARM_VM|ARM_M_B|ARM_M_U|ARM_V_F64,r0,r1,i0)
#define _VSTMDB_U_F64(r0,r1,i0)		_CC_VSTMDB_U_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VPUSH_F32(cc,r0,i0)		_CC_VSTMDB_U_F32(cc,JIT_SP,r0,i0)
#define _VPUSH_F32(r0,i0)		_CC_VPUSH_F32(ARM_CC_AL,r0,i0)
#define _CC_VPUSH_F64(cc,r0,i0)		_CC_VSTMDB_U_F64(cc,JIT_SP,r0,i0)
#define _VPUSH_F64(r0,i0)		_CC_VPUSH_F64(ARM_CC_AL,r0,i0)
#define _CC_VPOP_F32(cc,r0,i0)		_CC_VLDMIA_U_F32(cc,JIT_SP,r0,i0)
#define _VPOP_F32(r0,i0)		_CC_VPOP_F32(ARM_CC_AL,r0,i0)
#define _CC_VPOP_F64(cc,r0,i0)		_CC_VLDMIA_U_F64(cc,JIT_SP,r0,i0)
#define _VPOP_F64(r0,i0)		_CC_VPOP_F64(ARM_CC_AL,r0,i0)

/***********************************************************************
 * Advanced SIMD (encoding T2/A2) instructions
 ***********************************************************************/
#define _CC_VMOV_A_S8(cc,r0,r1)		arm_cc_vorr_(cc,ARM_VMOV_A_D|ARM_VMOV_ADV_8,r0,r1)
#define _VMOV_A_S8(r0,r1)		_CC_VMOV_A_S8(ARM_CC_AL,r0,r1)
#define _CC_VMOV_A_U8(cc,r0,r1)		arm_cc_vorr_(cc,ARM_VMOV_A_D|ARM_VMOV_ADV_8|ARM_VMOV_ADV_U,r0,r1)
#define _VMOV_A_U8(r0,r1)		_CC_VMOV_A_U8(ARM_CC_AL,r0,r1)
#define _CC_VMOV_A_S16(cc,r0,r1)	arm_cc_vorr_(cc,ARM_VMOV_A_D|ARM_VMOV_ADV_16,r0,r1)
#define _VMOV_A_S16(r0,r1)		_CC_VMOV_A_S16(ARM_CC_AL,r0,r1)
#define _CC_VMOV_A_U16(cc,r0,r1)	arm_cc_vorr_(cc,ARM_VMOV_A_D|ARM_VMOV_ADV_16|ARM_VMOV_ADV_U,r0,r1)
#define _VMOV_A_U16(r0,r1)		_CC_VMOV_A_U16(ARM_CC_AL,r0,r1)
#define _CC_VMOV_A_S32(cc,r0,r1)	arm_cc_vorr_(cc,ARM_VMOV_A_D,r0,r1)
#define _VMOV_A_S32(r0,r1)		_CC_VMOV_A_S32(ARM_CC_AL,r0,r1)
#define _CC_VMOV_A_U32(cc,r0,r1)	arm_cc_vorr_(cc,ARM_VMOV_A_D|ARM_VMOV_ADV_U,r0,r1)
#define _VMOV_A_U32(r0,r1)		_CC_VMOV_A_U32(ARM_CC_AL,r0,r1)
#define _CC_VMOV_V_I8(cc,r0,r1)		arm_cc_vorr_(cc,ARM_VMOV_D_A|ARM_VMOV_ADV_8,r1,r0)
#define _VMOV_V_I8(r0,r1)		_CC_VMOV_V_I8(ARM_CC_AL,r0,r1)
#define _CC_VMOV_V_I16(cc,r0,r1)	arm_cc_vorr_(cc,ARM_VMOV_D_A|ARM_VMOV_ADV_16,r1,r0)
#define _VMOV_V_I16(r0,r1)		_CC_VMOV_V_I16(ARM_CC_AL,r0,r1)
#define _CC_VMOV_V_I32(cc,r0,r1)	arm_cc_vorr_(cc,ARM_VMOV_D_A,r1,r0)
#define _VMOV_V_I32(r0,r1)		_CC_VMOV_V_I32(ARM_CC_AL,r0,r1)
#define _VADD_I8(r0,r1,r2)		arm_vorrr(ARM_VADD_I,r0,r1,r2)
#define _VADDQ_I8(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_Q,r0,r1,r2)
#define _VADD_I16(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_I16,r0,r1,r2)
#define _VADDQ_I16(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_I16|ARM_V_Q,r0,r1,r2)
#define _VADD_I32(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_I32,r0,r1,r2)
#define _VADDQ_I32(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_I32|ARM_V_Q,r0,r1,r2)
#define _VADD_I64(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_I64,r0,r1,r2)
#define _VADDQ_I64(r0,r1,r2)		arm_vorrr(ARM_VADD_I|ARM_V_I64|ARM_V_Q,r0,r1,r2)
#define _VQADD_S8(r0,r1,r2)		arm_vorrr(ARM_VQADD_I,r0,r1,r2)
#define _VQADDQ_S8(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_Q,r0,r1,r2)
#define _VQADD_U8(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_U,r0,r1,r2)
#define _VQADDQ_U8(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VQADD_S16(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I16,r0,r1,r2)
#define _VQADDQ_S16(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I16|ARM_V_Q,r0,r1,r2)
#define _VQADD_U16(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I16|ARM_V_U,r0,r1,r2)
#define _VQADDQ_U16(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I16|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VQADD_S32(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I32,r0,r1,r2)
#define _VQADDQ_S32(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I32|ARM_V_Q,r0,r1,r2)
#define _VQADD_U32(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I32|ARM_V_U,r0,r1,r2)
#define _VQADDQ_U32(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I32|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VQADD_S64(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I64,r0,r1,r2)
#define _VQADDQ_S64(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I64|ARM_V_Q,r0,r1,r2)
#define _VQADD_U64(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I64|ARM_V_U,r0,r1,r2)
#define _VQADDQ_U64(r0,r1,r2)		arm_vorrr(ARM_VQADD_I|ARM_V_I64|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VADDL_S8(r0,r1,r2)		arm_vorrr(ARM_VADDL_I,r0,r1,r2)
#define _VADDL_U8(r0,r1,r2)		arm_vorrr(ARM_VADDL_I|ARM_V_U,r0,r1,r2)
#define _VADDL_S16(r0,r1,r2)		arm_vorrr(ARM_VADDL_I|ARM_V_I16,r0,r1,r2)
#define _VADDL_U16(r0,r1,r2)		arm_vorrr(ARM_VADDL_I|ARM_V_I16|ARM_V_U,r0,r1,r2)
#define _VADDL_S32(r0,r1,r2)		arm_vorrr(ARM_VADDL_I|ARM_V_I32,r0,r1,r2)
#define _VADDL_U32(r0,r1,r2)		arm_vorrr(ARM_VADDL_I|ARM_V_I32|ARM_V_U,r0,r1,r2)
#define _VADDW_S8(r0,r1,r2)		arm_vorrr(ARM_VADDW_I,r0,r1,r2)
#define _VADDW_U8(r0,r1,r2)		arm_vorrr(ARM_VADDW_I|ARM_V_U,r0,r1,r2)
#define _VADDW_S16(r0,r1,r2)		arm_vorrr(ARM_VADDW_I|ARM_V_I16,r0,r1,r2)
#define _VADDW_U16(r0,r1,r2)		arm_vorrr(ARM_VADDW_I|ARM_V_I16|ARM_V_U,r0,r1,r2)
#define _VADDW_S32(r0,r1,r2)		arm_vorrr(ARM_VADDW_I|ARM_V_I32,r0,r1,r2)
#define _VADDW_U32(r0,r1,r2)		arm_vorrr(ARM_VADDW_I|ARM_V_I32|ARM_V_U,r0,r1,r2)
#define _VSUB_I8(r0,r1,r2)		arm_vorrr(ARM_VSUB_I,r0,r1,r2)
#define _VSUBQ_I8(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_Q,r0,r1,r2)
#define _VSUB_I16(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_I16,r0,r1,r2)
#define _VSUBQ_I16(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_I16|ARM_V_Q,r0,r1,r2)
#define _VSUB_I32(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_I32,r0,r1,r2)
#define _VSUBQ_I32(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_I32|ARM_V_Q,r0,r1,r2)
#define _VSUB_I64(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_I64,r0,r1,r2)
#define _VSUBQ_I64(r0,r1,r2)		arm_vorrr(ARM_VSUB_I|ARM_V_I64|ARM_V_Q,r0,r1,r2)
#define _VQSUB_S8(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I,r0,r1,r2)
#define _VQSUBQ_S8(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_Q,r0,r1,r2)
#define _VQSUB_U8(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_U,r0,r1,r2)
#define _VQSUBQ_U8(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VQSUB_S16(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I16,r0,r1,r2)
#define _VQSUBQ_S16(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I16|ARM_V_Q,r0,r1,r2)
#define _VQSUB_U16(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I16|ARM_V_U,r0,r1,r2)
#define _VQSUBQ_U16(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I16|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VQSUB_S32(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I32,r0,r1,r2)
#define _VQSUBQ_S32(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I32|ARM_V_Q,r0,r1,r2)
#define _VQSUB_U32(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I32|ARM_V_U,r0,r1,r2)
#define _VQSUBQ_U32(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I32|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VQSUB_S64(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I64,r0,r1,r2)
#define _VQSUBQ_S64(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I64|ARM_V_Q,r0,r1,r2)
#define _VQSUB_U64(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I64|ARM_V_U,r0,r1,r2)
#define _VQSUBQ_U64(r0,r1,r2)		arm_vorrr(ARM_VQSUB_I|ARM_V_I64|ARM_V_U|ARM_V_Q,r0,r1,r2)
#define _VSUBL_S8(r0,r1,r2)		arm_vorrr(ARM_VSUBL_I,r0,r1,r2)
#define _VSUBL_U8(r0,r1,r2)		arm_vorrr(ARM_VSUBL_I|ARM_V_U,r0,r1,r2)
#define _VSUBL_S16(r0,r1,r2)		arm_vorrr(ARM_VSUBL_I|ARM_V_I16,r0,r1,r2)
#define _VSUBL_U16(r0,r1,r2)		arm_vorrr(ARM_VSUBL_I|ARM_V_I16|ARM_V_U,r0,r1,r2)
#define _VSUBL_S32(r0,r1,r2)		arm_vorrr(ARM_VSUBL_I|ARM_V_I32,r0,r1,r2)
#define _VSUBL_U32(r0,r1,r2)		arm_vorrr(ARM_VSUBL_I|ARM_V_I32|ARM_V_U,r0,r1,r2)
#define _VSUBW_S8(r0,r1,r2)		arm_vorrr(ARM_VSUBW_I,r0,r1,r2)
#define _VSUBW_U8(r0,r1,r2)		arm_vorrr(ARM_VSUBW_I|ARM_V_U,r0,r1,r2)
#define _VSUBW_S16(r0,r1,r2)		arm_vorrr(ARM_VSUBW_I|ARM_V_I16,r0,r1,r2)
#define _VSUBW_U16(r0,r1,r2)		arm_vorrr(ARM_VSUBW_I|ARM_V_I16|ARM_V_U,r0,r1,r2)
#define _VSUBW_S32(r0,r1,r2)		arm_vorrr(ARM_VSUBW_I|ARM_V_I32,r0,r1,r2)
#define _VSUBW_U32(r0,r1,r2)		arm_vorrr(ARM_VSUBW_I|ARM_V_I32|ARM_V_U,r0,r1,r2)
#define _VMUL_I8(r0,r1,r2)		arm_vorrr(ARM_VMUL_I,r0,r1,r2)
#define _VMULQ_I8(r0,r1,r2)		arm_vorrr(ARM_VMUL_I|ARM_V_Q,r0,r1,r2)
#define _VMUL_I16(r0,r1,r2)		arm_vorrr(ARM_VMUL_I|ARM_V_I16,r0,r1,r2)
#define _VMULQ_I16(r0,r1,r2)		arm_vorrr(ARM_VMUL_I|ARM_V_Q|ARM_V_I16,r0,r1,r2)
#define _VMUL_I32(r0,r1,r2)		arm_vorrr(ARM_VMUL_I|ARM_V_I32,r0,r1,r2)
#define _VMULQ_I32(r0,r1,r2)		arm_vorrr(ARM_VMUL_I|ARM_V_Q|ARM_V_I32,r0,r1,r2)
#define _VMULL_S8(r0,r1,r2)		arm_vorrr(ARM_VMULL_I,r0,r1,r2)
#define _VMULL_U8(r0,r1,r2)		arm_vorrr(ARM_VMULL_I|ARM_V_U,r0,r1,r2)
#define _VMULL_S16(r0,r1,r2)		arm_vorrr(ARM_VMULL_I|ARM_V_I16,r0,r1,r2)
#define _VMULL_U16(r0,r1,r2)		arm_vorrr(ARM_VMULL_I|ARM_V_U|ARM_V_I16,r0,r1,r2)
#define _VMULL_S32(r0,r1,r2)		arm_vorrr(ARM_VMULL_I|ARM_V_I32,r0,r1,r2)
#define _VMULL_U32(r0,r1,r2)		arm_vorrr(ARM_VMULL_I|ARM_V_U|ARM_V_I32,r0,r1,r2)
#define _VABS_S8(r0,r1)			arm_vo_rr(ARM_VABS_I,r0,r1)
#define _VABSQ_S8(r0,r1)		arm_vo_rr(ARM_VABS_I|ARM_V_Q,r0,r1)
#define _VABS_S16(r0,r1)		arm_vo_rr(ARM_VABS_I|ARM_V_S16,r0,r1)
#define _VABSQ_S16(r0,r1)		arm_vo_rr(ARM_VABS_I|ARM_V_S16|ARM_V_Q,r0,r1)
#define _VABS_S32(r0,r1)		arm_vo_rr(ARM_VABS_I|ARM_V_S32,r0,r1)
#define _VABSQ_S32(r0,r1)		arm_vo_rr(ARM_VABS_I|ARM_V_S32|ARM_V_Q,r0,r1)
#define _VQABS_S8(r0,r1)		arm_vo_rr(ARM_VQABS_I,r0,r1)
#define _VQABSQ_S8(r0,r1)		arm_vo_rr(ARM_VQABS_I|ARM_V_Q,r0,r1)
#define _VQABS_S16(r0,r1)		arm_vo_rr(ARM_VQABS_I|ARM_V_S16,r0,r1)
#define _VQABSQ_S16(r0,r1)		arm_vo_rr(ARM_VQABS_I|ARM_V_S16|ARM_V_Q,r0,r1)
#define _VQABS_S32(r0,r1)		arm_vo_rr(ARM_VQABS_I|ARM_V_S32,r0,r1)
#define _VQABSQ_S32(r0,r1)		arm_vo_rr(ARM_VQABS_I|ARM_V_S32|ARM_V_Q,r0,r1)
#define _VNEG_S8(r0,r1)			arm_vo_rr(ARM_VNEG_I,r0,r1)
#define _VNEGQ_S8(r0,r1)		arm_vo_rr(ARM_VNEG_I|ARM_V_Q,r0,r1)
#define _VNEG_S16(r0,r1)		arm_vo_rr(ARM_VNEG_I|ARM_V_S16,r0,r1)
#define _VNEGQ_S16(r0,r1)		arm_vo_rr(ARM_VNEG_I|ARM_V_S16|ARM_V_Q,r0,r1)
#define _VNEG_S32(r0,r1)		arm_vo_rr(ARM_VNEG_I|ARM_V_S32,r0,r1)
#define _VNEGQ_S32(r0,r1)		arm_vo_rr(ARM_VNEG_I|ARM_V_S32|ARM_V_Q,r0,r1)
#define _VQNEG_S8(r0,r1)		arm_vo_rr(ARM_VQNEG_I,r0,r1)
#define _VQNEGQ_S8(r0,r1)		arm_vo_rr(ARM_VQNEG_I|ARM_V_Q,r0,r1)
#define _VQNEG_S16(r0,r1)		arm_vo_rr(ARM_VQNEG_I|ARM_V_S16,r0,r1)
#define _VQNEGQ_S16(r0,r1)		arm_vo_rr(ARM_VQNEG_I|ARM_V_S16|ARM_V_Q,r0,r1)
#define _VQNEG_S32(r0,r1)		arm_vo_rr(ARM_VQNEG_I|ARM_V_S32,r0,r1)
#define _VQNEGQ_S32(r0,r1)		arm_vo_rr(ARM_VQNEG_I|ARM_V_S32|ARM_V_Q,r0,r1)
#define _VAND(r0,r1,r2)			arm_vorrr(ARM_VAND,r0,r1,r2)
#define _VANDQ(r0,r1,r2)		arm_vorrr(ARM_VAND|ARM_V_Q,r0,r1,r2)
#define _VBIC(r0,r1,r2)			arm_vorrr(ARM_VBIC,r0,r1,r2)
#define _VBICQ(r0,r1,r2)		arm_vorrr(ARM_VBIC|ARM_V_Q,r0,r1,r2)
#define _VORR(r0,r1,r2)			arm_vorrr(ARM_VORR,r0,r1,r2)
#define _VORRQ(r0,r1,r2)		arm_vorrr(ARM_VORR|ARM_V_Q,r0,r1,r2)
#define _VORN(r0,r1,r2)			arm_vorrr(ARM_VORN,r0,r1,r2)
#define _VORNQ(r0,r1,r2)		arm_vorrr(ARM_VORN|ARM_V_Q,r0,r1,r2)
#define _VEOR(r0,r1,r2)			arm_vorrr(ARM_VEOR,r0,r1,r2)
#define _VEORQ(r0,r1,r2)		arm_vorrr(ARM_VEOR|ARM_V_Q,r0,r1,r2)
#define _VMOV(r0,r1)			_VORR(r0,r1,r1)
#define _VMOVQ(r0,r1)			_VORRQ(r0,r1,r1)
#define _VMOVL_S8(r0,r1)		arm_vo_rr(ARM_VMOVL_I|ARM_VMOVL_S8,r0,r1)
#define _VMOVL_U8(r0,r1)		arm_vo_rr(ARM_VMOVL_I|ARM_V_U|ARM_VMOVL_S8,r0,r1)
#define _VMOVL_S16(r0,r1)		arm_vo_rr(ARM_VMOVL_I|ARM_VMOVL_S16,r0,r1)
#define _VMOVL_U16(r0,r1)		arm_vo_rr(ARM_VMOVL_I|ARM_V_U|ARM_VMOVL_S16,r0,r1)
#define _VMOVL_S32(r0,r1)		arm_vo_rr(ARM_VMOVL_I|ARM_VMOVL_S32,r0,r1)
#define _VMOVL_U32(r0,r1)		arm_vo_rr(ARM_VMOVL_I|ARM_V_U|ARM_VMOVL_S32,r0,r1)
/* oi should be the result of en	code_vfp_immediate with code being one
 * of ARM_V{ORR,BIC,MOV,MVN}I */
#define _VIMM(oi,r0)			arm_voir(oi,r0)
#define _VIMMQ(oi,r0)			arm_voir(oi|ARM_V_Q,r0)
#define _VMOVI(oi,r0)			_VIMM(oi,r0)
#define _VMVNI(oi,r0)			_VIMM(oi,r0)

/* index is multipled by four */
#define _CC_VLDRN_F32(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VLDR,r0,r1,i0)
#define _VLDRN_F32(r0,r1,i0)		_CC_VLDRN_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDR_F32(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VLDR|ARM_P,r0,r1,i0)
#define _VLDR_F32(r0,r1,i0)		_CC_VLDR_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDRN_F64(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VLDR|ARM_V_F64,r0,r1,i0)
#define _VLDRN_F64(r0,r1,i0)		_CC_VLDRN_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VLDR_F64(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VLDR|ARM_V_F64|ARM_P,r0,r1,i0)
#define _VLDR_F64(r0,r1,i0)		_CC_VLDR_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTRN_F32(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VSTR,r0,r1,i0)
#define _VSTRN_F32(r0,r1,i0)		_CC_VSTRN_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTR_F32(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VSTR|ARM_P,r0,r1,i0)
#define _VSTR_F32(r0,r1,i0)		_CC_VSTR_F32(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTRN_F64(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VSTR|ARM_V_F64,r0,r1,i0)
#define _VSTRN_F64(r0,r1,i0)		_CC_VSTRN_F64(ARM_CC_AL,r0,r1,i0)
#define _CC_VSTR_F64(cc,r0,r1,i0)	arm_cc_vldst(cc,ARM_VSTR|ARM_V_F64|ARM_P,r0,r1,i0)
#define _VSTR_F64(r0,r1,i0)		_CC_VSTR_F64(ARM_CC_AL,r0,r1,i0)

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

#define arm_cc_orrr(cc,o,r0,r1,r2)  _arm_cc_orrr(_jit,cc,o,r0,r1,r2)
__jit_inline void
_arm_cc_orrr(jit_state_t _jit, int cc, int o, int r0, int r1, int r2)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00fff0f));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u4(r2));
}

#define arm_cc_orri(cc,o,r0,r1,i0)  _arm_cc_orri(_jit,cc,o,r0,r1,i0)
__jit_inline void
_arm_cc_orri(jit_state_t _jit, int cc, int o, int r0, int r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00fffff));
    assert(!(i0 & 0xfffff000));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u12(i0));
}

#define arm_cc_orri8(cc,o,r0,r1,i0) _arm_cc_orri8(_jit,cc,o,r0,r1,i0)
__jit_inline void
_arm_cc_orri8(jit_state_t _jit, int cc, int o, int r0, int r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00fff0f));
    assert(!(i0 & 0xffffff00));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|((i0&0xf0)<<4)|(i0&0x0f));
}

#define arm_cc_orrrr(cc,o,r0,r1,r2,r3) _arm_cc_orrrr(_jit,cc,o,r0,r1,r2,r3)
__jit_inline void
_arm_cc_orrrr(jit_state_t _jit, int cc, int o, int r0, int r1, int r2, int r3)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00fff0f));
    _jit_I(cc|o|(_u4(r0)<<16)|(_u4(r1)<<12)|(_u4(r2)<<8)|_u4(r3));
}

#define arm_cc_srrri(cc,o,r0,r1,r2,i0) _arm_cc_srrri(_jit,cc,o,r0,r1,r2,i0)
__jit_inline void
_arm_cc_srrri(jit_state_t _jit, int cc, int o, int r0, int r1, int r2, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf000ff8f));
    _jit_I(cc|o|(_u4(r0)<<12)|(_u4(r1)<<16)|(i0<<7)|_u4(r2));
}

#define arm_cc_shift(cc,o,r0,r1,r2,i0) _arm_cc_shift(_jit,cc,o,r0,r1,r2,i0)
__jit_inline void
_arm_cc_shift(jit_state_t _jit, int cc, int o, int r0, int r1, int r2, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xffe0ff8f));
    assert(((_u4(r2)<<8)&(i0<<7)) == 0);
    _jit_I(cc|ARM_SHIFT|o|(_u4(r0)<<12)|(_u4(r2)<<8)|(i0<<7)|_u4(r1));
}

#define arm_cc_b(cc,o,i0)	_arm_cc_b(_jit,cc,o,i0)
__jit_inline void
_arm_cc_b(jit_state_t _jit, int cc, int o, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf0ffffff));
    _jit_I(cc|o|_u24(i0));
}

#define arm_cc_bx(cc,o,r0)	_arm_cc_bx(_jit,cc,o,r0)
__jit_inline void
_arm_cc_bx(jit_state_t _jit, int cc, int o, int r0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf000000f));
    _jit_I(cc|o|_u4(r0));
}

#define arm_cc_orl(cc,o,r0,i0)	_arm_cc_orl(_jit,cc,o,r0,i0)
__jit_inline void
_arm_cc_orl(jit_state_t _jit, int cc, int o, int r0, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf00fffff));
    _jit_I(cc|o|(_u4(r0)<<16)|_u16(i0));
}

#define arm6_cc_orr(cc,o,r0,r1)	_arm6_cc_orr(_jit,cc,o,r0,r1)
__jit_inline void
_arm6_cc_orr(jit_state_t _jit, int cc, int o, int r0, int r1)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0xf000f00f));
    _jit_I(cc|o|(_u4(r0)<<12)|_u4(r1));
}

#define _CC_MOV(cc,r0,r1)	arm_cc_orrr(cc,ARM_MOV,r0,0,r1)
#define _MOV(r0,r1)		_CC_MOV(ARM_CC_AL,r0,r1)
#define _CC_MOVI(cc,r0,i0)	arm_cc_orri(cc,ARM_MOV|ARM_I,r0,0,i0)
#define _MOVI(r0,i0)		_CC_MOVI(ARM_CC_AL,r0,i0)
#define _CC_MVN(cc,r0,r1)	arm_cc_orrr(cc,ARM_MVN,r0,0,r1)
#define _MVN(r0, r1)		_CC_MVN(ARM_CC_AL,r0,r1)
#define _CC_MVNI(cc,r0,i0)	arm_cc_orri(cc,ARM_MVN|ARM_I,r0,0,i0)
#define _MVNI(r0,i0)		_CC_MVNI(ARM_CC_AL,r0,i0)
#define _CC_NOT(cc,r0,r1)	_CC_MVN(cc,r0,r1)
#define _NOT(r0, r1)		_MVN(r0, r1)
#define _NOP()			_MOV(_R0, _R0)

#define _CC_ADD(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_ADD,r0,r1,r2)
#define _ADD(r0,r1,r2)		_CC_ADD(ARM_CC_AL,r0,r1,r2)
#define _CC_ADDI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_ADD|ARM_I,r0,r1,i0)
#define _ADDI(r0,r1,i0)		_CC_ADDI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADDS(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_ADD|ARM_S,r0,r1,r2)
#define _ADDS(r0,r1,r2)		_CC_ADDS(ARM_CC_AL,r0,r1,r2)
#define _CC_ADDSI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_ADD|ARM_S|ARM_I,r0,r1,i0)
#define _ADDSI(r0,r1,i0)	_CC_ADDSI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADC(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_ADC,r0,r1,r2)
#define _ADC(r0,r1,r2)		_CC_ADC(ARM_CC_AL,r0,r1,r2)
#define _CC_ADCI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_ADC|ARM_I,r0,r1,i0)
#define _ADCI(r0,r1,i0)		_CC_ADCI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADCS(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_ADC|ARM_S,r0,r1,r2)
#define _ADCS(r0,r1,r2)		_CC_ADCS(ARM_CC_AL,r0,r1,r2)
#define _CC_ADCSI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_ADC|ARM_S|ARM_I,r0,r1,i0)
#define _ADCSI(r0,r1,i0)	_CC_ADCSI(ARM_CC_AL,r0,r1,i0)
#define _CC_SUB(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_SUB,r0,r1,r2)
#define _SUB(r0,r1,r2)		_CC_SUB(ARM_CC_AL,r0,r1,r2)
#define _CC_SUBI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_SUB|ARM_I,r0,r1,i0)
#define _SUBI(r0,r1,i0)		_CC_SUBI(ARM_CC_AL,r0,r1,i0)
#define _CC_SUBS(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_SUB|ARM_S,r0,r1,r2)
#define _SUBS(r0,r1,r2)		_CC_SUBS(ARM_CC_AL,r0,r1,r2)
#define _CC_SUBSI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_SUB|ARM_S|ARM_I,r0,r1,i0)
#define _SUBSI(r0,r1,i0)	_CC_SUBSI(ARM_CC_AL,r0,r1,i0)
#define _CC_SBC(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_SBC,r0,r1,r2)
#define _SBC(r0,r1,r2)		_CC_SBC(ARM_CC_AL,r0,r1,r2)
#define _CC_SBCI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_SBC|ARM_I,r0,r1,i0)
#define _SBCI(r0,r1,i0)		_CC_SBCI(ARM_CC_AL,r0,r1,i0)
#define _CC_SBCS(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_SBC|ARM_S,r0,r1,r2)
#define _SBCS(r0,r1,r2)		_CC_SBCS(ARM_CC_AL,r0,r1,r2)
#define _CC_SBCSI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_SBC|ARM_S|ARM_I,r0,r1,i0)
#define _SBCSI(r0,r1,i0)	_CC_SBCSI(ARM_CC_AL,r0,r1,i0)
#define _CC_RSB(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_RSB,r0,r1,r2)
#define _RSB(r0,r1,r2)		_CC_RSB(ARM_CC_AL,r0,r1,r2)
#define _CC_RSBI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_RSB|ARM_I,r0,r1,i0)
#define _RSBI(r0,r1,i0)		_CC_RSBI(ARM_CC_AL,r0,r1,i0)

/* >> ARMV7M */
#define _CC_RSC(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_RSC,r0,r1,r2)
#define _RSC(r0,r1,r2)		_CC_RSC(ARM_CC_AL,r0,r1,r2)
#define _CC_RSCI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_RSC|ARM_I,r0,r1,i0)
#define _RSCI(r0,r1,i0)		_CC_RSCI(ARM_CC_AL,r0,r1,i0)
/* << ARMV7M */

#define _CC_MUL(cc,r0,r1,r2)	  arm_cc_orrrr(cc,ARM_MUL,r0,0,r2,r1)
#define _MUL(r0,r1,r2)		  _CC_MUL(ARM_CC_AL,r0,r1,r2)
#define _CC_MLA(cc,r0,r1,r2,r3)	  arm_cc_orrrr(cc,ARM_MLA,r0,r3,r2,r1)
#define _MLA(r0,r1,r2,r3)	  _CC_MLA(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_UMULL(cc,r0,r1,r2,r3) arm_cc_orrrr(cc,ARM_UMULL,r1,r0,r3,r2)
#define _UMULL(r0,r1,r2,r3)	  _CC_UMULL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_UMLAL(cc,r0,r1,r2,r3) arm_cc_orrrr(cc,ARM_UMLAL,r1,r0,r3,r2)
#define _UMLAL(r0,r1,r2,r3)	  _CC_UMLAL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_SMULL(cc,r0,r1,r2,r3) arm_cc_orrrr(cc,ARM_SMULL,r1,r0,r3,r2)
#define _SMULL(r0,r1,r2,r3)	  _CC_SMULL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_SMLAL(cc,r0,r1,r2,r3) arm_cc_orrrr(cc,ARM_SMLAL,r1,r0,r3,r2)
#define _SMLAL(r0,r1,r2,r3)	  _CC_SMLAL(ARM_CC_AL,r0,r1,r2,r3)

#define _CC_AND(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_AND,r0,r1,r2)
#define _AND(r0,r1,r2)		_CC_AND(ARM_CC_AL,r0,r1,r2)
#define _CC_AND_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(cc,ARM_AND|sh,r0,r1,r2,im)
#define _AND_SI(r0,r1,r2,sh,im)	_CC_AND_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_ANDI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_AND|ARM_I,r0,r1,i0)
#define _ANDI(r0,r1,i0)		_CC_ANDI(ARM_CC_AL,r0,r1,i0)
#define _CC_ANDS(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_AND|ARM_S,r0,r1,r2)
#define _ANDS(r0,r1,r2)		_CC_ANDS(ARM_CC_AL,r0,r1,r2)
#define _CC_ANDSI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_AND|ARM_S|ARM_I,r0,r1,i0)
#define _ANDSI(r0,r1,i0)	_CC_ANDSI(ARM_CC_AL,r0,r1,i0)
#define _CC_BIC(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_BIC,r0,r1,r2)
#define _BIC(r0,r1,r2)		_CC_BIC(ARM_CC_AL,r0,r1,r2)
#define _CC_BIC_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(cc,ARM_BIC|sh,r0,r1,r2,im)
#define _BIC_SI(r0,r1,r2,sh,im)	_CC_BIC_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_BICI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_BIC|ARM_I,r0,r1,i0)
#define _BICI(r0,r1,i0)		_CC_BICI(ARM_CC_AL,r0,r1,i0)
#define _CC_BICS(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_BIC|ARM_S,r0,r1,r2)
#define _BICS(r0,r1,r2)		_CC_BICS(ARM_CC_AL,r0,r1,r2)
#define _CC_BICSI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_BIC|ARM_S|ARM_I,r0,r1,i0)
#define _BICSI(r0,r1,i0)	_CC_BICSI(ARM_CC_AL,r0,r1,i0)
#define _CC_ORR(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_ORR,r0,r1,r2)
#define _ORR(r0,r1,r2)		_CC_ORR(ARM_CC_AL,r0,r1,r2)
#define _CC_ORR_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(cc,ARM_ORR|sh,r0,r1,r2,im)
#define _ORR_SI(r0,r1,r2,sh,im)	_CC_ORR_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_ORRI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_ORR|ARM_I,r0,r1,i0)
#define _ORRI(r0,r1,i0)		_CC_ORRI(ARM_CC_AL,r0,r1,i0)
#define _CC_EOR(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_EOR,r0,r1,r2)
#define _EOR(r0,r1,r2)		_CC_EOR(ARM_CC_AL,r0,r1,r2)
#define _CC_EOR_SI(cc,r0,r1,r2,sh,im)					\
    arm_cc_srrri(cc,ARM_EOR|sh,r0,r1,r2,im)
#define _EOR_SI(r0,r1,r2,sh,im)	_CC_EOR_SI(ARM_CC_AL,r0,r1,r2,sh,im)
#define _CC_EORI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_EOR|ARM_I,r0,r1,i0)
#define _EORI(r0,r1,i0)		_CC_EORI(ARM_CC_AL,r0,r1,i0)

/* >> ARVM6* */
#define _CC_REV(cc,r0,r1)	arm6_cc_orr(cc,ARM_REV,r0,r1)
#define _REV(r0,r1)		_CC_REV(ARM_CC_AL,r0,r1)
#define _CC_REV16(cc,r0,r1)	arm6_cc_orr(cc,ARM_REV16,r0,r1)
#define _REV16(r0,r1)		_CC_REV16(ARM_CC_AL,r0,r1)
#define _CC_SXTB_R(cc,r0,r1,i0)	arm6_cc_orr(cc,ARM_SXTB|i0,r0,r1)
#define _SXTB_R(r0,r1,i0)	_CC_SXTB_R(ARM_CC_AL,r0,r1,i0)
#define _SXTB(r0,r1)		_SXTB_R(r0,r1,0)
#define _CC_UXTB_R(cc,r0,r1,i0)	arm6_cc_orr(cc,ARM_UXTB|i0,r0,r1)
#define _UXTB_R(r0,r1,i0)	_CC_UXTB_R(ARM_CC_AL,r0,r1,i0)
#define _UXTB(r0,r1)		_UXTB_R(r0,r1,0)
#define _CC_SXTH_R(cc,r0,r1,i0)	arm6_cc_orr(cc,ARM_SXTH|i0,r0,r1)
#define _SXTH_R(r0,r1,i0)	_CC_SXTH_R(ARM_CC_AL,r0,r1,i0)
#define _SXTH(r0,r1)		_SXTH_R(r0,r1,0)
#define _CC_UXTH_R(cc,r0,r1,i0)	arm6_cc_orr(cc,ARM_UXTH|i0,r0,r1)
#define _UXTH_R(r0,r1,i0)	_CC_UXTH_R(ARM_CC_AL,r0,r1,i0)
#define _UXTH(r0,r1)		_UXTH_R(r0,r1,0)
/* << ARVM6* */

#define _CC_SHIFT(cc,o,r0,r1,r2,i0) arm_cc_shift(cc,o,r0,r1,r2,i0)
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

#define _CC_CMP(cc,r0,r1)	arm_cc_orrr(cc,ARM_CMP,0,r0,r1)
#define _CMP(r0,r1)		_CC_CMP(ARM_CC_AL,r0,r1)
#define _CC_CMPI(cc,r0,i0)	arm_cc_orri(cc,ARM_CMP|ARM_I,0,r0,i0)
#define _CMPI(r0,i0)		_CC_CMPI(ARM_CC_AL,r0,i0)
#define _CC_CMN(cc,r0,r1)	arm_cc_orrr(cc,ARM_CMN,0,r0,r1)
#define _CMN(r0,r1)		_CC_CMN(ARM_CC_AL,r0,r1)
#define _CC_CMNI(cc,r0,i0)	arm_cc_orri(cc,ARM_CMN|ARM_I,0,r0,i0)
#define _CMNI(r0,i0)		_CC_CMNI(ARM_CC_AL,r0,i0)

/* >> THUMB */
#define _CC_TST(cc,r0,r1)	arm_cc_orrr(cc,ARM_TST,0,r0,r1)
#define _TST(r0,r1)		_CC_TST(ARM_CC_AL,r0,r1)
#define _CC_TSTI(cc,r0,i0)	arm_cc_orri(cc,ARM_TST|ARM_I,0,r0,i0)
#define _TSTI(r0,i0)		_CC_TSTI(ARM_CC_AL,r0,i0)
/* << THUMB */

/* >> ARVM6T2 */
#define _CC_TEQ(cc,r0,r1)	arm_cc_orrr(cc,ARM_TEQ,0,r0,r1)
#define _TEQ(r0,r1)		_CC_TEQ(ARM_CC_AL,r0,r1)
#define _CC_TEQI(cc,r0,i0)	arm_cc_orri(cc,ARM_TEQ|ARM_I,0,r0,i0)
#define _TEQI(r0,i0)		_CC_TEQI(ARM_CC_AL,r0,i0)
/* << ARVM6T2 */

#define _CC_BX(cc,r0)		arm_cc_bx(cc,ARM_BX,r0)
#define _BX(i0)			_CC_BX(ARM_CC_AL,r0)
#define _CC_BLX(cc,r0)		arm_cc_bx(cc,ARM_BLX,r0)
#define _BLX(r0)		_CC_BLX(ARM_CC_AL,r0)
#define _CC_B(cc,i0)		arm_cc_b(cc,ARM_B,i0)
#define _B(i0)			_CC_B(ARM_CC_AL,i0)
#define _CC_BL(cc,i0)		arm_cc_b(cc,ARM_BL,i0)
#define _BL(i0)			_CC_BL(ARM_CC_AL,i0)

#define _CC_LDRSB(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRSB|ARM_P,r0,r1,r2)
#define _LDRSB(r0,r1,r2)	_CC_LDRSB(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSBN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRSB,r0,r1,r2)
#define _LDRSBN(r0,r1,r2)	_CC_LDRSBN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSBI(cc,r0,r1,i0)	 arm_cc_orri8(cc,ARM_LDRSBI|ARM_P,r0,r1,i0)
#define _LDRSBI(r0,r1,i0)	 _CC_LDRSBI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSBIN(cc,r0,r1,i0) arm_cc_orri8(cc,ARM_LDRSBI,r0,r1,i0)
#define _LDRSBIN(r0,r1,i0)	 _CC_LDRSBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRB(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRB|ARM_P,r0,r1,r2)
#define _LDRB(r0,r1,r2)		_CC_LDRB(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRBN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRB,r0,r1,r2)
#define _LDRBN(r0,r1,r2)	_CC_LDRBN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRBI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_LDRBI|ARM_P,r0,r1,i0)
#define _LDRBI(r0,r1,i0)	_CC_LDRBI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRBIN(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_LDRBI,r0,r1,i0)
#define _LDRBIN(r0,r1,i0)	_CC_LDRBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSH(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRSH|ARM_P,r0,r1,r2)
#define _LDRSH(r0,r1,r2)	_CC_LDRSH(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSHN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRSH,r0,r1,r2)
#define _LDRSHN(r0,r1,r2)	_CC_LDRSHN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSHI(cc,r0,r1,i0)	 arm_cc_orri8(cc,ARM_LDRSHI|ARM_P,r0,r1,i0)
#define _LDRSHI(r0,r1,i0)	 _CC_LDRSHI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSHIN(cc,r0,r1,i0) arm_cc_orri8(cc,ARM_LDRSHI,r0,r1,i0)
#define _LDRSHIN(r0,r1,i0)	 _CC_LDRSHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRH(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRH|ARM_P,r0,r1,r2)
#define _LDRH(r0,r1,r2)		_CC_LDRH(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRHN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRH,r0,r1,r2)
#define _LDRHN(r0,r1,r2)	_CC_LDRHN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRHI(cc,r0,r1,i0)	arm_cc_orri8(cc,ARM_LDRHI|ARM_P,r0,r1,i0)
#define _LDRHI(r0,r1,i0)	_CC_LDRHI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRHIN(cc,r0,r1,i0)	arm_cc_orri8(cc,ARM_LDRHI,r0,r1,i0)
#define _LDRHIN(r0,r1,i0)	_CC_LDRHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDR(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDR|ARM_P,r0,r1,r2)
#define _LDR(r0,r1,r2)		_CC_LDR(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDR,r0,r1,r2)
#define _LDRN(r0,r1,r2)		_CC_LDRN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_LDRI|ARM_P,r0,r1,i0)
#define _LDRI(r0,r1,i0)		_CC_LDRI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRIN(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_LDRI,r0,r1,i0)
#define _LDRIN(r0,r1,i0)	_CC_LDRIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRD(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_LDRD|ARM_P,r0,r1,r2)
#define _LDRD(r0,r1,r2)		_CC_LDRD(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRDN(cc,r0,r1,r2)  arm_cc_orrr(cc,ARM_LDRD,r0,r1,r2)
#define _LDRDN(r0,r1,r2)	_CC_LDRDN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRDI(cc,r0,r1,i0)  arm_cc_orri8(cc,ARM_LDRDI|ARM_P,r0,r1,i0)
#define _LDRDI(r0,r1,i0)	_CC_LDRDI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRDIN(cc,r0,r1,i0) arm_cc_orri8(cc,ARM_LDRDI,r0,r1,i0)
#define _LDRDIN(r0,r1,i0)	_CC_LDRDIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRB(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STRB|ARM_P,r0,r1,r2)
#define _STRB(r0,r1,r2)		_CC_STRB(ARM_CC_AL,r0,r1,r2)
#define _CC_STRBN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STRB,r0,r1,r2)
#define _STRBN(r0,r1,r2)	_CC_STRBN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRBI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_STRBI|ARM_P,r0,r1,i0)
#define _STRBI(r0,r1,i0)	_CC_STRBI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRBIN(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_STRBI,r0,r1,i0)
#define _STRBIN(r0,r1,i0)	_CC_STRBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRH(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STRH|ARM_P,r0,r1,r2)
#define _STRH(r0,r1,r2)		_CC_STRH(ARM_CC_AL,r0,r1,r2)
#define _CC_STRHN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STRH,r0,r1,r2)
#define _STRHN(r0,r1,r2)	_CC_STRHN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRHI(cc,r0,r1,i0)	arm_cc_orri8(cc,ARM_STRHI|ARM_P,r0,r1,i0)
#define _STRHI(r0,r1,i0)	_CC_STRHI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRHIN(cc,r0,r1,i0)	arm_cc_orri8(cc,ARM_STRHI,r0,r1,i0)
#define _STRHIN(r0,r1,i0)	_CC_STRHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STR(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STR|ARM_P,r0,r1,r2)
#define _STR(r0,r1,r2)		_CC_STR(ARM_CC_AL,r0,r1,r2)
#define _CC_STRN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STR,r0,r1,r2)
#define _STRN(r0,r1,r2)		_CC_STRN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRI(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_STRI|ARM_P,r0,r1,i0)
#define _STRI(r0,r1,i0)		_CC_STRI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRIN(cc,r0,r1,i0)	arm_cc_orri(cc,ARM_STRI,r0,r1,i0)
#define _STRIN(r0,r1,i0)	_CC_STRIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRD(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STRD|ARM_P,r0,r1,r2)
#define _STRD(r0,r1,r2)		_CC_STRD(ARM_CC_AL,r0,r1,r2)
#define _CC_STRDN(cc,r0,r1,r2)	arm_cc_orrr(cc,ARM_STRD,r0,r1,r2)
#define _STRDN(r0,r1,r2)	_CC_STRDN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRDI(cc,r0,r1,i0)	arm_cc_orri8(cc,ARM_STRDI|ARM_P,r0,r1,i0)
#define _STRDI(r0,r1,i0)	_CC_STRDI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRDIN(cc,r0,r1,i0)	arm_cc_orri8(cc,ARM_STRDI,r0,r1,i0)
#define _STRDIN(r0,r1,i0)	_CC_STRDIN(ARM_CC_AL,r0,r1,i0)

#define _CC_LDMIA(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_I,r0,i0)
#define _LDMIA(r0,i0)		_CC_LDMIA(ARM_CC_AL,r0,i0)
#define _LDM(r0,i0)		_LDMIA(r0,i0)
#define _CC_LDMIA_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_U,r0,i0)
#define _LDMIA_U(r0,i0)		_CC_LDMIA_U(ARM_CC_AL,r0,i0)
#define _LDM_U(r0,i0)		_LDMIA_U(r0,i0)
#define _CC_LDMIB(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_B,r0,i0)
#define _LDMIB(r0,i0)		_CC_LDMIB(ARM_CC_AL,r0,i0)
#define _CC_LDMIB_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_B|ARM_M_U,r0,i0)
#define _LDMIB_U(r0,i0)		_CC_LDMIB_U(ARM_CC_AL,r0,i0)
#define _CC_LDMDA(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L,r0,i0)
#define _LDMDA(r0,i0)		_CC_LDMDA(ARM_CC_AL,r0,i0)
#define _CC_LDMDA_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_U,r0,i0)
#define _LDMDA_U(r0,i0)		_CC_LDMDA_U(ARM_CC_AL,r0,i0)
#define _CC_LDMDB(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_B,r0,i0)
#define _LDMDB(r0,i0)		_CC_LDMDB(ARM_CC_AL,r0,i0)
#define _CC_LDMDB_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_L|ARM_M_B|ARM_M_U,r0,i0)
#define _LDMDB_U(r0,i0)		_CC_LDMDB_U(ARM_CC_AL,r0,i0)
#define _CC_STMIA(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_I,r0,i0)
#define _STMIA(r0,i0)		_CC_STMIA(ARM_CC_AL,r0,i0)
#define _STM(r0,i0)		_STMIA(r0,i0)
#define _CC_STMIA_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_I|ARM_M_U,r0,i0)
#define _STMIA_U(r0,i0)		_CC_STMIA_U(ARM_CC_AL,r0,i0)
#define _STM_U(r0,i0)		_STMIA_U(r0,i0)
#define _CC_STMIB(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_I|ARM_M_B,r0,i0)
#define _STMIB(r0,i0)		_CC_STMIB(ARM_CC_AL,r0,i0)
#define _CC_STMIB_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_I|ARM_M_B|ARM_M_U,r0,i0)
#define _STMIB_U(r0,i0)		_CC_STMIB_U(ARM_CC_AL,r0,i0)
#define _CC_STMDA(cc,r0,i0)	arm_cc_orl(cc,ARM_M,r0,i0)
#define _STMDA(r0,i0)		_CC_STMDA(ARM_CC_AL,r0,i0)
#define _CC_STMDA_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_U,r0,i0)
#define _STMDA_U(r0,i0)		_CC_STMDA_U(ARM_CC_AL,r0,i0)
#define _CC_STMDB(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_B,r0,i0)
#define _STMDB(r0,i0)		_CC_STMDB(ARM_CC_AL,r0,i0)
#define _CC_STMDB_U(cc,r0,i0)	arm_cc_orl(cc,ARM_M|ARM_M_B|ARM_M_U,r0,i0)
#define _STMDB_U(r0,i0)		_CC_STMDB_U(ARM_CC_AL,r0,i0)
#define _CC_PUSH(cc,i0)		_CC_STMDB_U(cc,JIT_SP,i0)
#define _PUSH(i0)		_CC_PUSH(ARM_CC_AL,i0)
#define _CC_POP(cc,i0)		_CC_LDMIA_U(cc,JIT_SP,i0)
#define _POP(i0)		_CC_POP(ARM_CC_AL,i0)

#endif /* __lightning_asm_h */
