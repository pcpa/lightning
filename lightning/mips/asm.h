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
    struct {	_ui _:26;	_ui	b :  6; } hc;
    struct {	_ui _:21;	_ui	b :  5; } rs;
    struct {	_ui _:21;	_ui	b :  5; } fm;
    struct {	_ui _:16;	_ui	b :  5; } rt;
    struct {	_ui _:16;	_ui	b :  5; } ft;
    struct {	_ui _:11;	_ui	b :  5; } rd;
    struct {	_ui _:11;	_ui	b :  5; } fs;
    struct {	_ui _: 6;	_ui	b :  5; } ic;
    struct {	_ui _: 6;	_ui	b :  5; } fd;
    struct {	_ui _: 6;	_ui	b : 20; } br;
    struct {			_ui	b :  6; } tc;
    struct {			_ui	b : 11; } cc;
    struct {			_ui	b : 16; } is;
    struct {			_ui	b : 26; } ii;
    int						  op;
} mips_code_t;

typedef enum {
    MIPS_REGIMM		= 0x01,
    MIPS_J		= 0x02,
    MIPS_SRL		= 0x02,
    MIPS_JAL		= 0x03,
    MIPS_SRA		= 0x03,
    MIPS_BEQ		= 0x04,
    MIPS_BNE		= 0x05,
    MIPS_BLEZ		= 0x06,
    MIPS_BGTZ		= 0x07,
    MIPS_ADDI		= 0x08,
    MIPS_ADDIU		= 0x09,
    MIPS_SLTI		= 0x0a,
    MIPS_SLTIU		= 0x0b,
    MIPS_ANDI		= 0x0c,
    MIPS_ORI		= 0x0d,
    MIPS_XORI		= 0x0e,
    MIPS_LUI		= 0x0f,
    MIPS_COP1		= 0x11,
    MIPS_COP2		= 0x12,
    MIPS_COP1X		= 0x13,
    MIPS_BEQL		= 0x14,
    MIPS_BNEL		= 0x15,
    MIPS_BLEZL		= 0x16,
    MIPS_BGTZL		= 0x17,
    MIPS_DADDI		= 0x18,
    MIPS_DADDIU		= 0x19,
    MIPS_SPECIAL2	= 0x1c,
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
    MIPS_CACHE		= 0x2f,
    MIPS_LWC1		= 0x31,
    MIPS_LDC1		= 0x35,
    MIPS_LD		= 0x37,
    MIPS_SDC1		= 0x3d,
    MIPS_SWC1		= 0x39,
    MIPS_SD		= 0x3f,
} mips_hc_t;

typedef enum {
    MIPS_CF		= 0x02,
    MIPS_BC		= 0x08,
    MIPS_BGZAL		= 0x11,
} mips_r1_t;

typedef enum {
    MIPS_BC1F		= 0x00,
    MIPS_BLTZ		= 0x00,
    MIPS_BC1T		= 0x01,
    MIPS_BGEZ		= 0x01,
    MIPS_BC1FL		= 0x02,
    MIPS_BLTZL		= 0x02,
    MIPS_BC1TL		= 0x03,
    MIPS_BGEZL		= 0x03,
    MIPS_BGEZAL		= 0x11,
    MIPS_BLTZALL	= 0x12,
    MIPS_BGEZALL	= 0x13,
} mips_r2_t;

typedef enum {
    MIPS_SLL		= 0x00,
    MIPS_MUL		= 0x02,
    MIPS_SLLV		= 0x04,
    MIPS_TINS		= 0x04,
    MIPS_SRLV		= 0x06,
    MIPS_SRAV		= 0x07,
    MIPS_TDINS		= 0x07,
    MIPS_JR		= 0x08,
    MIPS_JALR		= 0x09,
    MIPS_MOVZ		= 0x0a,
    MIPS_MOVN		= 0x0b,
    MIPS_BREAK		= 0x0d,
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
    MIPS_CLZ		= 0x20,
    MIPS_ADDU		= 0x21,
    MIPS_CLO		= 0x21,
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
} mips_tc_t;

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
    MIPS_ALNV_PS	= 0x1e,
    MIPS_CVT_fmt_S	= 0x20,
    MIPS_CVT_fmt_D	= 0x21,
    MIPS_CVT_fmt_W	= 0x24,
    MIPS_CVT_fmt_L	= 0x25,
} mips_fc_t;

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

typedef enum {
    MIPS_cond_F		= 0x30,
    MIPS_cond_UN	= 0x31,
    MIPS_cond_EQ	= 0x32,
    MIPS_cond_UEQ	= 0x33,
    MIPS_cond_OLT	= 0x34,
    MIPS_cond_ULT	= 0x35,
    MIPS_cond_OLE	= 0x36,
    MIPS_cond_ULE	= 0x37,
    MIPS_cond_SF	= 0x38,
    MIPS_cond_NGLE	= 0x39,
    MIPS_cond_SEQ	= 0x3a,
    MIPS_cond_NGL	= 0x3b,
    MIPS_cond_LT	= 0x3c,
    MIPS_cond_NGE	= 0x3d,
    MIPS_cond_LE	= 0x3e,
    MIPS_cond_UGT	= 0x3f,
} mips_cond_t;

__jit_inline void
mips___r_t(jit_state_t _jit, jit_gpr_t rd, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.tc.b = tc;
    c.rd.b = rd;

    _jit_I(c.op);
}

__jit_inline void
mips_r___t(jit_state_t _jit, jit_gpr_t rs, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.rs.b = rs;
    c.tc.b = tc;

    _jit_I(c.op);
}

__jit_inline void
mipsh_ri(jit_state_t _jit, mips_hc_t hc, jit_gpr_t rt, int im)
{
    mips_code_t		c;

    c.is.b = _s16(im);
    c.rt.b = rt;
    c.rs.b = 0;		/* ignored */
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mipshrri(jit_state_t _jit, mips_hc_t hc, jit_gpr_t rt, jit_gpr_t rs, int im)
{
    mips_code_t		c;

    c.is.b = _s16(im);
    c.rt.b = rt;
    c.rs.b = rs;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mipshrrr_t(jit_state_t _jit, mips_hc_t hc,
	   jit_gpr_t rs, jit_gpr_t rt, jit_gpr_t rd, mips_tc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.ic.b = 0;
    c.rd.b = rd;
    c.rt.b = rt;
    c.rs.b = rs;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mipsh_rrt(jit_state_t _jit, mips_hc_t hc,
	  jit_gpr_t rt, jit_gpr_t rd, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.tc.b = tc;
    c.rd.b = rd;
    c.rt.b = rt;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mips_rrr_t(jit_state_t _jit, jit_gpr_t rs,
	   jit_gpr_t rt, jit_gpr_t rd, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.tc.b = tc;
    c.rd.b = rd;
    c.rt.b = rt;
    c.rs.b = rs;

    _jit_I(c.op);
}

__jit_inline void
mips__rrit(jit_state_t _jit, jit_gpr_t rt, jit_gpr_t rd, int im, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.tc.b = tc;
    c.ic.b = _u5(im);
    c.rd.b = rd;
    c.rt.b = rt;

    _jit_I(c.op);
}

__jit_inline void
mips_r_it(jit_state_t _jit, jit_gpr_t rs, int im, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.tc.b = tc;
    c.ic.b = im;	/* hint */
    c.rs.b = rs;

    _jit_I(c.op);
}


__jit_inline void
mips_r_rit(jit_state_t _jit, jit_gpr_t rs, jit_gpr_t rd, int im, mips_tc_t tc)
{
    mips_code_t		c;

    c.op = 0;
    c.tc.b = tc;
    c.ic.b = im;
    c.rd.b = rd;
    c.rs.b = rs;

    _jit_I(c.op);
}

__jit_inline void
mipshi(jit_state_t _jit, mips_hc_t hc, int im)
{
    mips_code_t		c;

    c.ii.b = _s26(im);
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mipshrriit(jit_state_t _jit, mips_hc_t hc, jit_gpr_t rs,
	   jit_gpr_t rt, int rd, int im, mips_tc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.ic.b = im;
    c.rd.b = rd;
    c.rt.b = rt;
    c.rs.b = rs;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mips_fp1(jit_state_t _jit, mips_fmt_t fm,
	 jit_fpr_t fs, jit_fpr_t fd, mips_fc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.fd.b = fd;
    c.fs.b = fs;
    c.ft.b = 0;
    c.fm.b = fm;
    c.hc.b = MIPS_COP1;

    _jit_I(c.op);
}

__jit_inline void
mips_fp2(jit_state_t _jit, mips_fmt_t fm,
	 jit_fpr_t ft, jit_fpr_t fs, jit_fpr_t fd, mips_fc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.fd.b = fd;
    c.fs.b = fs;
    c.ft.b = ft;
    c.fm.b = fm;
    c.hc.b = MIPS_COP1;

    _jit_I(c.op);
}

__jit_inline void
mips_xrf(jit_state_t _jit, mips_fmt_t fm, jit_gpr_t rt, jit_gpr_t fs)
{
    mips_code_t		c;

    c.op = 0;
    c.fs.b = fs;
    c.ft.b = rt;
    c.fm.b = fm;
    c.hc.b = MIPS_COP1;

    _jit_I(c.op);
}

__jit_inline void
mipshrfi(jit_state_t _jit, mips_hc_t hc,
	 mips_fmt_t fm, jit_gpr_t rs, jit_gpr_t ft, int im)
{
    mips_code_t		c;

    c.is.b = im;
    c.rs.b = rs;
    c.rt.b = ft;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mips_rrf_(jit_state_t _jit, jit_gpr_t rs,
	  jit_gpr_t rt, jit_fpr_t fs, mips_fc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.ic.b = 0;
    c.rd.b = fs;
    c.rt.b = rt;
    c.rs.b = rs;
    c.hc.b = MIPS_COP1X;

    _jit_I(c.op);
}

__jit_inline void
mips_rr_f(jit_state_t _jit, jit_gpr_t rs,
	  jit_gpr_t rt, jit_fpr_t fd, mips_fc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.is.b = fd;
    c.rd.b = 0;
    c.rt.b = rt;
    c.rs.b = rs;
    c.hc.b = MIPS_COP1X;

    _jit_I(c.op);
}

__jit_inline void
mips_rffft(jit_state_t _jit, jit_gpr_t rs,
	   jit_fpr_t ft, jit_fpr_t fs, jit_fpr_t fd, mips_fc_t tc)
{
    mips_code_t		c;

    c.tc.b = tc;
    c.fd.b = fd;
    c.fs.b = fs;
    c.ft.b = ft;
    c.fm.b = rs;
    c.hc.b = MIPS_COP1X;

    _jit_I(c.op);
}

__jit_inline void
mips_hxxs(jit_state_t _jit, mips_hc_t hc, mips_r1_t r1, mips_r2_t r2, int im)
{
    mips_code_t		c;

    c.is.b = _s16(im);
    c.rt.b = r2;
    c.rs.b = r1;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mips_hrxs(jit_state_t _jit, mips_hc_t hc, jit_gpr_t rs, mips_r2_t r2, int im)
{
    mips_code_t		c;

    c.is.b = _s16(im);
    c.rt.b = r2;
    c.rs.b = rs;
    c.hc.b = hc;

    _jit_I(c.op);
}

__jit_inline void
mips_c_cond_fmt(jit_state_t _jit, mips_fmt_t fm,
		jit_fpr_t ft, jit_fpr_t fs, mips_cond_t cc)
{
    mips_code_t		c;

    c.cc.b = cc;
    c.fs.b = fs;
    c.ft.b = ft;
    c.fm.b = fm;
    c.hc.b = MIPS_COP1;

    _jit_I(c.op);
}

#define _ADD(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_ADD)
#define _ADD_S(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_S,ft,fs,fd,MIPS_ADD_fmt)
#define _ADD_D(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_D,ft,fs,fd,MIPS_ADD_fmt)
#define _ADD_PS(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_PS,ft,fs,fd,MIPS_ADD_fmt)
#define _ADDI(rt,rs,im)		mipshrri(_jit,MIPS_ADDI,rs,rt,im)
#define _ADDIU(rt,rs,im)	mipshrri(_jit,MIPS_ADDIU,rs,rt,im)
#define _ADDU(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_ADDU)
#define _ALNV_PS(fd,fs,ft,rs)	mips_rffft(_jit,rs,ft,fs,fd,MIPS_ALNV_PS)
#define _AND(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_AND)
#define _ANDI(rt,rs,im)		mipshrri(_jit,MIPS_ANDI,rs,rt,im)
#define _B(im)			mipshrri(_jit, MIPS_BEQ, _ZERO, _ZERO,im)
#define _BAL(im)		mipshrri(_jit,REGIMM,_ZERO,(jit_gpr_t)MIPS_BGZALL,im)
#define _BC1F(im)		mips_hxxs(_jit,MIPS_COP1,MIPS_BC,MIPS_BC1F,im)
#define _BC1FL(im)		mips_hxxs(_jit,MIPS_COP1,MIPS_BC,MIPS_BC1FL,im)
#define _BC1T(im)		mips_hxxs(_jit,MIPS_COP1,MIPS_BC,MIPS_BC1T,im)
#define _BC1TL(im)		mips_hxxs(_jit,MIPS_COP1,MIPS_BC,MIPS_BC1TL,im)
#define _BC2F(im)		mips_hxxs(_jit,MIPS_COP2,MIPS_BC,MIPS_BC1F,im)
#define _BC2FL(im)		mips_hxxs(_jit,MIPS_COP2,MIPS_BC,MIPS_BC1FL,im)
#define _BC2T(im)		mips_hxxs(_jit,MIPS_COP2,MIPS_BC,MIPS_BC1T,im)
#define _BC2TL(im)		mips_hxxs(_jit,MIPS_COP2,MIPS_BC,MIPS_BC1TL,im)
#define _BEQ(rs,rt,im)		mipshrri(_jit,MIPS_BEQ,r0,r1,im)
#define _BEQL(rs,rt,im)		mipshrri(_jit,MIPS_BEQL,r0,r1,im)
#define _BGEZ(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BGEZ,im)
#define _BGEZAL(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BGEZAL,im)
#define _BGEZALL(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BGEZALL,im)
#define _BGEZL(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BGEZL,im)
#define _BGTZ(rs,im)		mipshrri(_jit,MIPS_BGTZ,rs,_ZERO,im)
#define _BGTZL(rs,im)		mipshrri(_jit,MIPS_BGTZL,rs,_ZERO,im)
#define _BLEZ(rs,im)		mipshrri(_jit,MIPS_BLEZ,rs,_ZERO,im)
#define _BLEZL(rs,im)		mipshrri(_jit,MIPS_BLEZL,rs,_ZERO,im)
#define _BLTZ(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BLTZ,im)
#define _BLTZAL(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BLTZAL,im)
#define _BLTZALL(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BLTZALL,im)
#define _BLTZL(rs,im)		mips_hrxs(_jit,MIPS_REGIMM,rs,MIPS_BLTZL,im)
#define _BNE(rs,rt,im)		mipshrri(_jit,MIPS_BNE,r0,r1,im)
#define _BNEL(rs,rt,im)		mipshrri(_jit,MIPS_BNEL,r0,r1,im)
__jit_inline void
mips_break(jit_state_t _jit, int code)
{
    mips_code_t		c;

    c.tc.b = MIPS_BREAK;
    c.br.b = _s20(code);
    c.hc.b = 0;

    _jit_I(c.op);
}
#define _BREAK(code)		mips_break(_jit, code)
#define _C_F_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_F)
#define _C_F_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_F)
#define _C_F_PS(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_F)
#define _C_UN_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_UN)
#define _C_UN_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_UN)
#define _C_UN_PS(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_UN)
#define _C_EQ_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_EQ)
#define _C_EQ_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_EQ)
#define _C_EQ_PS(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_EQ)
#define _C_UEQ_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_UEQ)
#define _C_UEQ_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_UEQ)
#define _C_UEQ_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_UEQ)
#define _C_OLT_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_OLT)
#define _C_OLT_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_OLT)
#define _C_OLT_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_OLT)
#define _C_ULT_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_ULT)
#define _C_ULT_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_ULT)
#define _C_ULT_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_ULT)
#define _C_OLE_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_OLE)
#define _C_OLE_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_OLE)
#define _C_OLE_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_OLE)
#define _C_ULE_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_ULE)
#define _C_ULE_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_ULE)
#define _C_ULE_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_ULE)
#define _C_SF_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_SF)
#define _C_SF_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_SF)
#define _C_SF_PS(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_SF)
#define _C_NGLE_S(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_NGLE)
#define _C_NGLE_D(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_NGLE)
#define _C_NGLE_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_NGLE)
#define _C_SEQ_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_SEQ)
#define _C_SEQ_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_SEQ)
#define _C_SEQ_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_SEQ)
#define _C_NGL_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_NGL)
#define _C_NGL_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_NGL)
#define _C_NGL_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_NGL)
#define _C_NLT_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_NLT)
#define _C_NLT_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_NLT)
#define _C_NLT_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_NLT)
#define _C_NGE_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_NGE)
#define _C_NGE_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_NGE)
#define _C_NGE_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_NGE)
#define _C_NLE_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_NLE)
#define _C_NLE_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_NLE)
#define _C_NLE_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_NLE)
#define _C_UGT_S(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_S,jit_cond_UGT)
#define _C_UGT_D(fs,ft)		mips_c_cond_fmt(_jit,jit_fmt_D,jit_cond_UGT)
#define _C_UGT_PS(fs,ft)	mips_c_cond_fmt(_jit,jit_fmt_PS,jit_cond_UGT)
#define _CACHE(op,offset,base)	mips_cache(_jit,op,base,offset)
__jit_inline void
mips_cache(jit_state_t _jit, jit_gpr_t base, int op, int offset)
{
    mips_code_t		c;

    c.is.b = _s16(offset);
    c.rt.b = op;
    c.rs.b = base;
    c.hc.b = MIPS_CACHE;

    _jit_I(c.op);
}
#define _CEIL_L_S(fd,fs)	mips_fp1(_jit,MIPS_fmt_S,fs,fd,MIPS_CEIL_fmt_L)
#define _CEIL_L_D(fd,fs)	mips_fp1(_jit,MIPS_fmt_D,fs,fd,MIPS_CEIL_fmt_L)
#define _CEIL_W_S(fd,fs)	mips_fp1(_jit,MIPS_fmt_S,fs,fd,MIPS_CEIL_fmt_W)
#define _CEIL_W_D(fd,fs)	mips_fp1(_jit,MIPS_fmt_D,fs,fd,MIPS_CEIL_fmt_W)
#define _CFC1(rt,fs)		mips_cfc1(_jit,rt,fs)
__jit_inline void
mips_cfc1(jit_state_t _jit, jit_gpr_t rt, jit_fpr_t fs)
{
    mips_code_t		c;

    c.cc.b = 0;
    c.fs.b = fs;
    c.rt.b = rt;
    c.rs.b = MIPS_CF;
    c.hc.b = MIPS_COP1;

    _jit_I(c.op);
}
#define _CFC2(rt,impl)		mips_cfc2(_jit,rt,impl)
__jit_inline void
mips_cfc2(jit_state_t _jit, jit_gpr_t rt, int impl)
{
    mips_code_t		c;

    c.is.b = _s16(impl);
    c.rs.b = MIPS_CF;
    c.hc.b = MIPS_COP2;

    _jit_I(c.op);
}
#define _CLO(rd,rs)		mipshrrr_t(_jit,MIPS_SPECIAL2,rs,rd,rd,MIPS_CLO)
#define _COP2(func)
__jit_inline void
mips_cop2(jit_state_t _jit, int func)
{
    mips_code_t		c;

    c.hc.b = MIPS_COP2;
    c.ii.b = (1 << 25) | _u25(func);

    _jit_I(c.op);
}
#define _CLZ(rd,rs)		mipshrrr_t(_jit,MIPS_SPECIAL2,rs,rd,rd,MIPS_CLZ)


#define _DADD(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_DADD)
#define _DADDI(rt,rs,im)	mipshrri(_jit,MIPS_DADDI,rs,rt,im)
#define _DADDU(rd,rs,rt)	mips_rrr_t(_jit,rs,rt,rd,MIPS_DADDU)
#define _DADDIU(rt,rs,im)	mipshrri(_jit,MIPS_DADDIU,rs,rt,im)

#define _DDIV(rs,rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_DDIV)
#define _DDIVU(rs,rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_DDIVU)

#define _DIV(rs,rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_DIV)
#define _DIVU(rs,rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_DIVU)

#define _DMULT(rs, rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_DMULT)
#define _DMULTU(rs, rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_DMULTU)

#define _DSUB(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_DSUB)
#define _DSUBU(rd,rs,rt)	mips_rrr_t(_jit,rs,rt,rd,MIPS_DSUBU)

#define _LUI(rt,im)		mipsh_ri(_jit,MIPS_LUI,rt,im)

#define _MFHI(rd)		mips___r_t(_jit, rd, MIPS_MFHI)
#define _MFLO(rd)		mips___r_t(_jit, rd, MIPS_MFLO)

#define _MUL(rd,rs,rt)		mipshrrr_t(_jit,MIPS_SPECIAL2,rs,rt,rd,MIPS_MUL)
#define _MUL_S(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_S,ft,fs,fd,MIPS_MUL_fmt)
#define _MUL_D(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_D,ft,fs,fd,MIPS_MUL_fmt)
#define _MUL_PS(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_PS,ft,fs,fd,MIPS_MUL_fmt)
#define _MULT(rs, rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_MULT)
#define _MULTU(rs, rt)		mips_rrr_t(_jit,rs,rt,JIT_RZERO,MIPS_MULTU)

#define _OR(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_OR)
#define _ORI(rt,rs,im)		mipshrri(_jit,MIPS_ORI,rs,rt,im)

#define _SLT(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_SLT)
#define _SLTI(rt,rs,im)		mipshrri(_jit,MIPS_SLTI,rs,rt,im)
#define _SLTU(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_SLTU)
#define _SLTIU(rt,rs,im)	mipshrri(_jit,MIPS_SLTIU,rs,rt,im)

#define _SUB(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_SUB)
#define _SUB_S(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_S,ft,fs,fd,MIPS_SUB_fmt)
#define _SUB_D(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_D,ft,fs,fd,MIPS_SUB_fmt)
#define _SUB_PS(fd,fs,ft)	mips_fp2(_jit,MIPS_fmt_PS,ft,fs,fd,MIPS_SUB_fmt)
#define _SUBU(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_SUBU)
#define _XOR(rd,rs,rt)		mips_rrr_t(_jit,rs,rt,rd,MIPS_XOR)

/* Reference:
 *	http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
 *	MIPS32(r) Architecture Volume II: The MIPS32(r) Instrunction Set
 *	MIPS64(r) Architecture Volume II: The MIPS64(r) Instrunction Set
 */

#endif /* __lightning_asm_h */
