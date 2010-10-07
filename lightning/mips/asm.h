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

typedef enum {
    _F0		= 0x00,
    _F1		= 0x01,
    _F2		= 0x02,
    _F3		= 0x03,
    _F4		= 0x04,
    _F5		= 0x05,
    _F6		= 0x06,
    _F7		= 0x07,
    _F8		= 0x08,
    _F9		= 0x09,
    _F10	= 0x0a,
    _F11	= 0x0b,
    _F12	= 0x0c,
    _F13	= 0x0d,
    _F14	= 0x0e,
    _F15	= 0x0f,
    _F16	= 0x10,
    _F17	= 0x11,
    _F18	= 0x12,
    _F19	= 0x13,
    _F20	= 0x14,
    _F21	= 0x15,
    _F22	= 0x16,
    _F23	= 0x17,
    _F24	= 0x18,
    _F25	= 0x19,
    _F26	= 0x1a,
    _F27	= 0x1b,
    _F28	= 0x1c,
    _F29	= 0x1d,
    _F30	= 0x1e,
    _F31	= 0x1f,
} jit_fpr_t;

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
	_ui	tc : 11;
	_ui	rd : 5;
	_ui	rt : 5;
	_ui	rs : 5;
	_ui	hc : 6;
    } h_rrt;
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
    struct {
	_ui	tc : 6;
	_ui	fd : 5;
	_ui	fs : 5;
	_ui	ft : 5;
	_ui	fm : 5;
	_ui	hc : 6;
    } fp;
    int		op;
} mips_code_t;

typedef enum {
    MIPS_J		= 0x02,
    MIPS_SRL		= 0x02,
    MIPS_JAL		= 0x03,
    MIPS_SRA		= 0x03,
    MIPS_BEQ		= 0x04,
    MIPS_ADDI		= 0x08,
    MIPS_ADDIU		= 0x09,
    MIPS_SLTI		= 0x0a,
    MIPS_SLTIU		= 0x0b,
    MIPS_ANDI		= 0x0c,
    MIPS_ORI		= 0x0d,
    MIPS_XORI		= 0x0e,
    MIPS_LUI		= 0x0f,
    MIPS_COP1		= 0x11,
    MIPS_COP1X		= 0x13,
    MIPS_HMUL		= 0x1c,
    MIPS_DADDI		= 0x18,
    MIPS_DADDIU		= 0x19,
    MIPS_HINS		= 0x1f,
    MIPS_HDINS		= 0x1f,
    MIPS_LB		= 0x20,
    MIPS_HSEB		= 0x1f,
    MIPS_HSEH		= 0x1f,
    MIPS_LH		= 0x21,
    MIPS_LW		= 0x23,
    MIPS_LBU		= 0x24,
    MIPS_LHU		= 0x25,
    MIPS_LWU		= 0x27,
    MIPS_SB		= 0x28,
    MIPS_SH		= 0x29,
    MIPS_SW		= 0x2b,
    MIPS_LWC1		= 0x31,
    MIPS_LDC1		= 0x35,
    MIPS_LD		= 0x37,
    MIPS_SDC1		= 0x3d,
    MIPS_SWC1		= 0x39,
    MIPS_SD		= 0x3f,
} mips_hcode_t;

typedef enum {
    MIPS_SLL		= 0x00,
    MIPS_TMUL		= 0x02,
    MIPS_SLLV		= 0x04,
    MIPS_TINS		= 0x04,
    MIPS_SRLV		= 0x06,
    MIPS_SRAV		= 0x07,
    MIPS_TDINS		= 0x07,
    MIPS_JR		= 0x08,
    MIPS_JALR		= 0x09,
    MIPS_MOVZ		= 0x0a,
    MIPS_MOVN		= 0x0b,
    MIPS_MFHI		= 0x10,
    MIPS_MTHI		= 0x11,
    MIPS_MFLO		= 0x12,
    MIPS_MTLO		= 0x13,
    MIPS_DSLLV		= 0x14,
    MIPS_DSRLV		= 0x16,
    MIPS_DSRAV		= 0x17,
    MIPS_MULT		= 0x18,
    MIPS_MULTU		= 0x19,
    MIPS_DIV		= 0x1a,
    MIPS_DIVU		= 0x1b,
    MIPS_DMULT		= 0x1c,
    MIPS_DMULTU		= 0x1d,
    MIPS_DDIV		= 0x1e,
    MIPS_DDIVU		= 0x1f,
    MIPS_ADD		= 0x20,
    MIPS_ADDU		= 0x21,
    MIPS_SUB		= 0x22,
    MIPS_SUBU		= 0x23,
    MIPS_AND		= 0x24,
    MIPS_OR		= 0x25,
    MIPS_XOR		= 0x26,
    MIPS_NOR		= 0x27,
    MIPS_SLT		= 0x2a,
    MIPS_SLTU		= 0x2b,
    MIPS_DADD		= 0x2c,
    MIPS_DADDU		= 0x2d,
    MIPS_DSUB		= 0x2e,
    MIPS_DSUBU		= 0x2f,
    MIPS_DSLL		= 0x38,
    MIPS_DSRL		= 0x3a,
    MIPS_DSRA		= 0x3b,
    MIPS_DSLL32		= 0x3c,
    MIPS_DSRL32		= 0x3e,
    MIPS_DSRA32		= 0x3f,
    MIPS_TSEB		= 0x420,
    MIPS_TSEH		= 0x620,
} mips_tcode_t;

typedef enum {
    MIPS_ADD_fmt	= 0x00,
    MIPS_LWXC1		= 0x00,
    MIPS_SUB_fmt	= 0x01,
    MIPS_LDXC1		= 0x01,
    MIPS_MUL_fmt	= 0x02,
    MIPS_DIV_fmt	= 0x03,
    MIPS_SQRT_fmt	= 0x04,
    MIPS_ABS_fmt	= 0x05,
    MIPS_MOV_fmt	= 0x06,
    MIPS_NEG_fmt	= 0x07,
    MIPS_SWXC1		= 0x08,
    MIPS_TRUNC_fmt_L	= 0x09,
    MIPS_SDXC1		= 0x09,
    MIPS_CEIL_fmt_L	= 0x0a,
    MIPS_FLOOR_fmt_L	= 0x0b,
    MIPS_TRUNC_fmt_W	= 0x0d,
    MIPS_CEIL_fmt_W	= 0x0e,
    MIPS_FLOOR_fmt_W	= 0x0f,
    MIPS_CVT_fmt_S	= 0x20,
    MIPS_CVT_fmt_D	= 0x21,
    MIPS_CVT_fmt_W	= 0x24,
    MIPS_CVT_fmt_L	= 0x25,
} mips_fcode_t;

typedef enum {
    MIPS_fmt_l_fi	= 0x00,		/* int32 <- uninterp float32 */
    MIPS_fmt_dl		= 0x01,		/* int64 <- uninterp float64 */
    MIPS_fmt_h_fi	= 0x03,		/* int32 <- top word of float64 */
    MIPS_fmt_l_if	= 0x04,		/* int32 -> uninterp float32 */
    MIPS_fmt_ld		= 0x05,		/* int64 -> uninterp float64 */
    MIPS_fmt_h_if	= 0x07,		/* int32 -> top word of float64 */
    MIPS_fmt_S		= 0x10,		/* float32 */
    MIPS_fmt_D		= 0x11,		/* float64 */
    MIPS_fmt_W		= 0x14,		/* int32 */
    MIPS_fmt_L		= 0x15,		/* int64 */
    MIPS_fmt_PS		= 0x16,		/* 2 x float32 */
} mips_fmt_t;

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
mipsh_rrt(jit_state_t _jit, mips_hcode_t hc,
	  jit_gpr_t rt, jit_gpr_t rd, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc.h_rrt.tc = tc;
    cc.h_rrt.rd = rd;
    cc.h_rrt.rt = rt;
    cc.h_rrt.rs = 0;
    cc.h_rrt.hc = hc;

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

__jit_inline void
mipshrriit(jit_state_t _jit, mips_hcode_t hc,
	   jit_gpr_t rs, jit_gpr_t rt, int rd, int im, mips_tcode_t tc)
{
    mips_code_t		cc;

    cc.hrrrit.tc = tc;
    cc.hrrrit.im = im;
    cc.hrrrit.rd = rd;
    cc.hrrrit.rt = rt;
    cc.hrrrit.rs = rs;
    cc.hrrrit.hc = hc;

    _jit_I(cc.op);
}

__jit_inline void
mips_fp1(jit_state_t _jit, mips_fmt_t fm,
	 jit_fpr_t fs, jit_fpr_t fd, mips_fcode_t tc)
{
    mips_code_t		cc;

    cc.fp.tc = tc;
    cc.fp.fd = fd;
    cc.fp.fs = fs;
    cc.fp.ft = 0;
    cc.fp.fm = fm;
    cc.fp.hc = MIPS_COP1;

    _jit_I(cc.op);
}

__jit_inline void
mips_fp2(jit_state_t _jit, mips_fmt_t fm,
	 jit_fpr_t ft, jit_fpr_t fs, jit_fpr_t fd, mips_fcode_t tc)
{
    mips_code_t		cc;

    cc.fp.tc = tc;
    cc.fp.fd = fd;
    cc.fp.fs = fs;
    cc.fp.ft = ft;
    cc.fp.fm = fm;
    cc.fp.hc = MIPS_COP1;

    _jit_I(cc.op);
}

__jit_inline void
mips_xrf(jit_state_t _jit, mips_fmt_t fm, jit_gpr_t rt, jit_gpr_t fs)
{
    mips_code_t		cc;

    cc.fp.tc = 0;
    cc.fp.fd = 0;
    cc.fp.fs = fs;
    cc.fp.ft = rt;
    cc.fp.fm = fm;
    cc.fp.hc = MIPS_COP1;

    _jit_I(cc.op);
}

__jit_inline void
mipshrfi(jit_state_t _jit, mips_hcode_t hc,
	 mips_fmt_t fm, jit_gpr_t rs, jit_gpr_t ft, int im)
{
    mips_code_t		cc;

    cc.hrri.im = im;
    cc.hrri.rs = rs;
    cc.hrri.rt = ft;
    cc.hrri.hc = hc;

    _jit_I(cc.op);
}

__jit_inline void
mips_rrf_(jit_state_t _jit,
	 jit_gpr_t rs, jit_gpr_t rt, jit_fpr_t fs, mips_fcode_t tc)
{
    mips_code_t		cc;

    cc.hrrrit.tc = tc;
    cc.hrrrit.im = 0;
    cc.hrrrit.rd = fs;
    cc.hrrrit.rt = rt;
    cc.hrrrit.rs = rs;
    cc.hrrrit.hc = MIPS_COP1X;

    _jit_I(cc.op);
}

__jit_inline void
mips_rr_f(jit_state_t _jit,
	 jit_gpr_t rs, jit_gpr_t rt, jit_fpr_t fd, mips_fcode_t tc)
{
    mips_code_t		cc;

    cc.hrrrit.tc = tc;
    cc.hrrrit.im = fd;
    cc.hrrrit.rd = 0;
    cc.hrrrit.rt = rt;
    cc.hrrrit.rs = rs;
    cc.hrrrit.hc = MIPS_COP1X;

    _jit_I(cc.op);
}

/* Reference:
 *	http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
 *	MIPS32(r) Architecture Volume II: The MIPS32(r) Instrunction Set
 *	MIPS64(r) Architecture Volume II: The MIPS64(r) Instrunction Set
 */

#endif /* __lightning_asm_h */
