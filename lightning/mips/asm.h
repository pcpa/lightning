/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the mips
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2010 Free Software Foundation
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
    _ZERO	= 0x00,		/* constant 0 */
    _AT		= 0x01,		/* assembly temporary */
    _V0		= 0x02,		/* function return and expression evaluation */
    _V1		= 0x03,
    _A0		= 0x04,		/* function arguments */
    _A1		= 0x05,
    _A2		= 0x06,
    _A3		= 0x07,
    _T0		= 0x08,		/* temporaries */
    _T1		= 0x09,
    _T2		= 0x0a,
    _T3		= 0x0b,
    _T4		= 0x0c,
    _T5		= 0x0d,
    _T6		= 0x0e,
    _T7		= 0x0f,
    _S0		= 0x10,		/* (callee saved) temporaries */
    _S1		= 0x11,
    _S2		= 0x12,
    _S3		= 0x13,
    _S4		= 0x14,
    _S5		= 0x15,
    _S6		= 0x16,
    _S7		= 0x17,
    _T8		= 0x18,		/* temporaries */
    _T9		= 0x19,
    _K0		= 0x1a,		/* reserved for OS kernel */
    _K1		= 0x1b,
    _GP		= 0x1c,		/* (callee saved) global pointer */
    _SP		= 0x1d,		/* (callee saved) stack pointer */
    _FP		= 0x1e,		/* (callee saved) frame pointer */
    _RA		= 0x1f,		/* return address */
} jit_gpr_t;

typedef union {
    struct {
	_ui	tc : 11;
	_ui	rd : 5;
	_ui	hc : 16;
    } ___r_t;
    struct {
	_ui	tc : 21;
	_ui	rs : 5;
	_ui	hc : 6;
    } _r___t;
    struct {
	_ui	tc : 11;
	_ui	rd : 5;
	_ui	rt : 5;
	_ui	rs : 5;
	_ui	hc : 6;
    } _rrr_t;
    struct {
	_ui	im : 16;
	_ui	rt : 5;
	_ui	rs : 5;
	_ui	hc : 6;
    } hrri;
    struct {
	_ui	tc : 6;
	_ui	im : 5;
	_ui	rd : 5;
	_ui	rt : 5;
	_ui	rs : 5;
	_ui	hc : 6;
    } hrrrit;
    struct {
	_ui	tc : 6;
	_ui	im : 5;
	_ui	rd : 5;
	_ui	rt : 5;
	_ui	hc : 11;
    } __rrit;
    struct {
	_ui	tc : 6;
	_ui	im : 5;
	_ui	rt : 10;
	_ui	rs : 5;
	_ui	hc : 6;
    } _r_it;
    struct  {
	_ui	im : 26;
	_ui	hc : 6;
    } hi;
    int		op;
} mips_code_t;

typedef enum {
    MIPS_TMUL	= 0x02,		/* pair to HMUL */
    MIPS_SLLV	= 0x04,
    MIPS_SRLV	= 0x06,
    MIPS_SRAV	= 0x06,
    MIPS_JR	= 0x08,
    MIPS_JALR	= 0x09,
    MIPS_MFHI	= 0x10,
    MIPS_MTHI	= 0x11,
    MIPS_MFLO	= 0x12,
    MIPS_MTLO	= 0x13,
    MIPS_ADD	= 0x20,
    MIPS_ADDU	= 0x21,
    MIPS_AND	= 0x24,
    MIPS_MULT	= 0x18,
    MIPS_MULTU	= 0x19,
    MIPS_DIV	= 0x1a,
    MIPS_DIVU	= 0x1b,
    MIPS_SUB	= 0x22,
    MIPS_SUBU	= 0x23,
    MIPS_OR	= 0x25,
    MIPS_XOR	= 0x26,
    MIPS_NOR	= 0x27,
    MIPS_SLT	= 0x2a,
    MIPS_SLTU	= 0x2b,
} mips_tcode_t;

typedef enum {
    MIPS_SLL	= 0x00,
    MIPS_J	= 0x02,
    MIPS_SRL	= 0x02,
    MIPS_JAL	= 0x03,
    MIPS_SRA	= 0x03,
    MIPS_BEQ	= 0x04,
    MIPS_ADDI	= 0x08,
    MIPS_ADDIU	= 0x09,
    MIPS_ANDI	= 0x0c,
    MIPS_ORI	= 0x0d,
    MIPS_XORI	= 0x0e,
    MIPS_LUI	= 0x0f,
    MIPS_HMUL	= 0x1c,		/* pair to TMUL */
    MIPS_LB	= 0x20,
    MIPS_LH	= 0x21,
    MIPS_LW	= 0x23,
    MIPS_LBU	= 0x24,
    MIPS_LHU	= 0x25,
    MIPS_SB	= 0x28,
    MIPS_SH	= 0x29,
    MIPS_SW	= 0x2b,
} mips_hcode_t;

__jit_inline void
mips___r_t(jit_state_t _jit, jit_gpr_t r0, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc.___r_t.tc = tc;
    cc.___r_t.rd = r0;
    cc.___r_t.hc = 0;

    _jit_I(cc.op);
}

__jit_inline void
mips_r___t(jit_state_t _jit, jit_gpr_t r0, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc._r___t.tc = tc;
    cc._r___t.rs = r0;
    cc._r___t.hc = 0;

    _jit_I(cc.op);
}

__jit_inline void
mipsh_ri(jit_state_t _jit, mips_hcode_t hc, jit_gpr_t rt, int im)
{
    mips_code_t		cc;

    cc.hrri.im = _s16(im);
    cc.hrri.rt = rt;
    cc.hrri.rs = 0;	/* ignored */
    cc.hrri.hc = hc;

    _jit_I(cc.op);
}

__jit_inline void
mipshrri(jit_state_t _jit, mips_hcode_t hc, jit_gpr_t rt, jit_gpr_t rs, int im)
{
    mips_code_t		cc;

    cc.hrri.im = _s16(im);
    cc.hrri.rt = rt;
    cc.hrri.rs = rs;
    cc.hrri.hc = hc;

    _jit_I(cc.op);
}

__jit_inline void
mipshrrr_t(jit_state_t _jit, mips_hcode_t hc,
	   jit_gpr_t rs, jit_gpr_t rt, jit_gpr_t rd, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc.hrrrit.tc = tc;
    cc.hrrrit.im = 0;
    cc.hrrrit.rd = rd;
    cc.hrrrit.rt = rt;
    cc.hrrrit.rs = rs;
    cc.hrrrit.hc = hc;

    _jit_I(cc.op);
}

__jit_inline void
mips_rrr_t(jit_state_t _jit,
	   jit_gpr_t rs, jit_gpr_t rt, jit_gpr_t rd, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc._rrr_t.tc = tc;
    cc._rrr_t.rd = rd;
    cc._rrr_t.rt = rt;
    cc._rrr_t.rs = rs;
    cc._rrr_t.hc = 0;

    _jit_I(cc.op);
}

__jit_inline void
mips__rrit(jit_state_t _jit,
	   jit_gpr_t rt, jit_gpr_t rd, int im, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc.__rrit.tc = tc;
    cc.__rrit.im = _u5(im);
    cc.__rrit.rd = rd;
    cc.__rrit.rt = rt;
    cc.__rrit.hc = 0;

    _jit_I(cc.op);
}

__jit_inline void
mips_r_it(jit_state_t _jit, jit_gpr_t rs, int im, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc._r_it.tc = tc;
    cc._r_it.im = im;	/* hint */
    cc._r_it.rt = 0;
    cc._r_it.rs = rs;
    cc._r_it.hc = 0;

    _jit_I(cc.op);
}


__jit_inline void
mips_r_rit(jit_state_t _jit,
	   jit_gpr_t rs, jit_gpr_t rd, int im, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc.hrrrit.tc = tc;
    cc.hrrrit.im = im;
    cc.hrrrit.rd = rd;
    cc.hrrrit.rt = 0;
    cc.hrrrit.rs = rs;
    cc.hrrrit.hc = 0;

    _jit_I(cc.op);
}

__jit_inline void
mipshi(jit_state_t _jit, mips_hcode_t hc, int im)
{
    mips_code_t		cc;

    cc.hi.im = _s26(im);
    cc.hi.hc = hc;

    _jit_I(cc.op);
}

/* Reference:
 *	http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
 *	MIPS32(r) Architecture Volume II: The MIPS32(r) Instrunction Set
 */

#endif /* __lightning_asm_h */
