/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the x86-64
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2003 Gwenole Beauchesne
 * Copyright 2006 Free Software Foundation, Inc.
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
 ***********************************************************************/

#ifndef __lightning_asm_h
#define __lightning_asm_h
#ifndef LIGHTNING_DEBUG

/*	OPCODE	+ i		= immediate operand
 *		+ r		= register operand
 *		+ m		= memory operand (disp,base,index,scale)
 *		+ sr/sm		= a star preceding a register or memory
 */

#define CALLsr(R)			CALLQsr(R)
#define JMPsr(R)			JMPQsr(R)

/* --- Increment/Decrement instructions ------------------------------------ */
__jit_inline void
_dec_sil_r(jit_gpr_t rd)
{
    _O(0xff);
    _Mrm(_b11, _b001, _rA(rd));
}

__jit_inline void
_inc_sil_r(jit_gpr_t rd)
{
    _O(0xff);
    _Mrm(_b11, _b000, _rA(rd));
}

/* --- REX prefixes -------------------------------------------------------- */
#define _BIT(X)			(!!(X))
#define _d64(W,R,X,B)		(_jit_B(0x40|(W)<<3|(R)<<2|(X)<<1|(B)))

#define __REXwrxb(L,W,R,X,B)	((W|R|X|B) || (L) ? (void)_d64(W,R,X,B) : ((void)0))
#define __REXwrx_(L,W,R,X,MR)	(__REXwrxb(L,W,R,X,_BIT(_rIP(MR)?0:_rXP(MR))))
#define __REXw_x_(L,W,R,X,MR)	(__REXwrx_(L,W,_BIT(_rXP(R)),X,MR))
#define __REX_reg(RR)		(__REXwrxb(0,0,0,00,_BIT(_rXP(RR))))
#define __REX_mem(MB,MI)	(__REXwrxb(0,0,0,_BIT(_rXP(MI)),_BIT(_rXP(MB))))

#define _r1e8lP(R)	((int)(R) >= _RSP && (int)(R) <= _RDX)

__jit_inline void
_REXBrr(jit_gpr_t rr, jit_gpr_t mr)
{
    __REXw_x_(_r1e8lP(rr) || _r1e8lP(mr), 0, rr, 0, mr);
}

__jit_inline void
_REXBmr(int mb, int mi, jit_gpr_t rd)
{
    __REXw_x_(_r1e8lP(rd) || _r1e8lP(mb), 0, rd, _BIT(_rXP(mi)), mb);
}

__jit_inline void
_REXBrm(jit_gpr_t rs, int mb, int mi)
{
    _REXBmr(mb, mi, rs);
}

__jit_inline void
_REXBLrr(jit_gpr_t rr, jit_gpr_t mr)
{
    __REXw_x_(_r1e8lP(mr), 0, rr, 0, mr);
}

__jit_inline void
_REXLr(jit_gpr_t rr)
{
    __REX_reg(rr);
}

__jit_inline void
_REXLm(int mb, int mi)
{
    __REX_mem(mb, mi);
}

__jit_inline void
_REXLrr(jit_gpr_t rr, jit_gpr_t mr)
{
    __REXw_x_(0, 0, rr, 0, mr);
}

__jit_inline void
_REXLmr(int mb, int mi, jit_gpr_t rd)
{
    __REXw_x_(0, 0, rd, _BIT(_rXP(mi)), mb);
}

__jit_inline void
_REXLrm(jit_gpr_t rs, int mb, int mi)
{
    _REXLmr(mb, mi, rs);
}

__jit_inline void
_REXQrr(jit_gpr_t rr, jit_gpr_t mr)
{
    __REXw_x_(0, 1, rr, 0, mr);
}

__jit_inline void
_REXQmr(int mb, int mi, jit_gpr_t rd)
{
    __REXw_x_(0, 1, rd, _BIT(_rXP(mi)), mb);
}

__jit_inline void
_REXQrm(jit_gpr_t rs, int mb, int mi)
{
    _REXQmr(mb, mi, rs);
}

__jit_inline void
_REXQr(jit_gpr_t rr)
{
    __REX_reg(rr);
}

__jit_inline void
_REXQm(int mb, int mi)
{
    __REX_mem(mb, mi);
}

/* --- ALU instructions ---------------------------------------------------- */
__jit_inline void
_ALUQrr(int op, jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _alu_sil_rr(op, rs, rd);
}

__jit_inline void
_ALUQmr(int op, int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _alu_sil_mr(op, md, mb, mi, ms, rd);
}

__jit_inline void
_ALUQrm(int op, jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _alu_sil_rm(op, rs, md, mb, mi, ms);
}

__jit_inline void
_ALUQir(int op, long im, jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _alu_il_ir(op, im, rd);
}

__jit_inline void
_ALUQim(int op, long im, int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _alu_il_im(op, im, md, mb, mi, ms);
}

#define ADCQrr(RS, RD)			_ALUQrr(X86_ADC, RS, RD)
#define ADCQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCQir(IM, RD)			_ALUQir(X86_ADC, IM, RD)
#define ADCQim(IM, MD, MB, MI, MS)	_ALUQim(X86_ADC, IM, MD, MB, MI, MS)

#define ADDQrr(RS, RD)			_ALUQrr(X86_ADD, RS, RD)
#define ADDQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDQir(IM, RD)			_ALUQir(X86_ADD, IM, RD)
#define ADDQim(IM, MD, MB, MI, MS)	_ALUQim(X86_ADD, IM, MD, MB, MI, MS)

#define ANDQrr(RS, RD)			_ALUQrr(X86_AND, RS, RD)
#define ANDQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDQir(IM, RD)			_ALUQir(X86_AND, IM, RD)
#define ANDQim(IM, MD, MB, MI, MS)	_ALUQim(X86_AND, IM, MD, MB, MI, MS)

#define CMPQrr(RS, RD)			_ALUQrr(X86_CMP, RS, RD)
#define CMPQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPQir(IM, RD)			_ALUQir(X86_CMP, IM, RD)
#define CMPQim(IM, MD, MB, MI, MS)	_ALUQim(X86_CMP, IM, MD, MB, MI, MS)

#define ORQrr(RS, RD)			_ALUQrr(X86_OR, RS, RD)
#define ORQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_OR, MD, MB, MI, MS, RD)
#define ORQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_OR, RS, MD, MB, MI, MS)
#define ORQir(IM, RD)			_ALUQir(X86_OR, IM, RD)
#define ORQim(IM, MD, MB, MI, MS)	_ALUQim(X86_OR, IM, MD, MB, MI, MS)

#define SBBQrr(RS, RD)			_ALUQrr(X86_SBB, RS, RD)
#define SBBQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBQir(IM, RD)			_ALUQir(X86_SBB, IM, RD)
#define SBBQim(IM, MD, MB, MI, MS)	_ALUQim(X86_SBB, IM, MD, MB, MI, MS)

#define SUBQrr(RS, RD)			_ALUQrr(X86_SUB, RS, RD)
#define SUBQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBQir(IM, RD)			_ALUQir(X86_SUB, IM, RD)
#define SUBQim(IM, MD, MB, MI, MS)	_ALUQim(X86_SUB, IM, MD, MB, MI, MS)

#define XORQrr(RS, RD)			_ALUQrr(X86_XOR, RS, RD)
#define XORQmr(MD, MB, MI, MS, RD)	_ALUQmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORQrm(RS, MD, MB, MI, MS)	_ALUQrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORQir(IM, RD)			_ALUQir(X86_XOR, IM, RD)
#define XORQim(IM, MD, MB, MI, MS)	_ALUQim(X86_XOR, IM, MD, MB, MI, MS)

/* --- Shift/Rotate instructions ------------------------------------------- */
__jit_inline void
_ROTSHIQir(int op, long im, jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _rotsh_sil_ir(op, im, rd);
}

__jit_inline void
_ROTSHIQim(int op, long im, int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _rotsh_sil_im(op, im, md, mb, mi, ms);
}

__jit_inline void
_ROTSHIQrr(int op, jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _rotsh_sil_rr(op, rs, rd);
}

__jit_inline void
_ROTSHIQrm(int op, jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _rotsh_sil_rm(op, rs, md, mb, mi, ms);
}

#define ROLQir(IM, RD)			_ROTSHIQir(X86_ROL, IM, RD)
#define ROLQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLQrr(RS, RD)			_ROTSHIQrr(X86_ROL, RS, RD)
#define ROLQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_ROL, RS, MD, MB, MI, MS)

#define RORQir(IM, RD)			_ROTSHIQir(X86_ROR, IM, RD)
#define RORQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_ROR, IM, MD, MB, MI, MS)
#define RORQrr(RS, RD)			_ROTSHIQrr(X86_ROR, RS, RD)
#define RORQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_ROR, RS, MD, MB, MI, MS)

#define RCLQir(IM, RD)			_ROTSHIQir(X86_RCL, IM, RD)
#define RCLQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLQrr(RS, RD)			_ROTSHIQrr(X86_RCL, RS, RD)
#define RCLQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCRQir(IM, RD)			_ROTSHIQir(X86_RCR, IM, RD)
#define RCRQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRQrr(RS, RD)			_ROTSHIQrr(X86_RCR, RS, RD)
#define RCRQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_RCR, RS, MD, MB, MI, MS)

#define SHLQir(IM, RD)			_ROTSHIQir(X86_SHL, IM, RD)
#define SHLQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLQrr(RS, RD)			_ROTSHIQrr(X86_SHL, RS, RD)
#define SHLQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHRQir(IM, RD)			_ROTSHIQir(X86_SHR, IM, RD)
#define SHRQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRQrr(RS, RD)			_ROTSHIQrr(X86_SHR, RS, RD)
#define SHRQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_SHR, RS, MD, MB, MI, MS)

#define SALQir				SHLQir
#define SALQim				SHLQim
#define SALQrr				SHLQrr
#define SALQrm				SHLQrm

#define SARQir(IM, RD)			_ROTSHIQir(X86_SAR, IM, RD)
#define SARQim(IM, MD, MB, MI, MS)	_ROTSHIQim(X86_SAR, IM, MD, MB, MI, MS)
#define SARQrr(RS, RD)			_ROTSHIQrr(X86_SAR, RS, RD)
#define SARQrm(RS, MD, MB, MI, MS)	_ROTSHIQrm(X86_SAR, RS, MD, MB, MI, MS)

/* --- Bit test instructions ----------------------------------------------- */
__jit_inline void
_BTQir(int op, long im, jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _bt_sil_ir(op, im, rd);
}

__jit_inline void
_BTQim(int op, long im, int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _bt_sil_im(op, im, md, mb, mi, ms);
}

__jit_inline void
_BTQrr(int op, jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _bt_sil_rr(op, rs, rd);
}

__jit_inline void
_BTQrm(int op, jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _bt_sil_rm(op, rs, md, mb, mi, ms);
}

#define BTQir(IM, RD)			_BTQir(X86_BT, IM, RD)
#define BTQim(IM, MD, MB, MI, MS)	_BTQim(X86_BT, IM, MD, MB, MI, MS)
#define BTQrr(RS, RD)			_BTQrr(X86_BT, RS, RD)
#define BTQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BT, RS, MD, MB, MI, MS)

#define BTCQir(IM, RD)			_BTQir(X86_BTC, IM, RD)
#define BTCQim(IM, MD, MB, MI, MS)	_BTQim(X86_BTC, IM, MD, MB, MI, MS)
#define BTCQrr(RS, RD)			_BTQrr(X86_BTC, RS, RD)
#define BTCQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BTC, RS, MD, MB, MI, MS)

#define BTRQir(IM, RD)			_BTQir(X86_BTR, IM, RD)
#define BTRQim(IM, MD, MB, MI, MS)	_BTQim(X86_BTR, IM, MD, MB, MI, MS)
#define BTRQrr(RS, RD)			_BTQrr(X86_BTR, RS, RD)
#define BTRQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BTR, RS, MD, MB, MI, MS)

#define BTSQir(IM, RD)			_BTQir(X86_BTS, IM, RD)
#define BTSQim(IM, MD, MB, MI, MS)	_BTQim(X86_BTS, IM, MD, MB, MI, MS)
#define BTSQrr(RS, RD)			_BTQrr(X86_BTS, RS, RD)
#define BTSQrm(RS, MD, MB, MI, MS)	_BTQrm(X86_BTS, RS, MD, MB, MI, MS)

/* --- Move instructions --------------------------------------------------- */
__jit_inline void
MOVQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _mov_sil_rr(rs, rd);
}

__jit_inline void
MOVQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _mov_sil_mr(md, mb, mi, ms, rd);
}

__jit_inline void
MOVQrm(jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _mov_sil_rm(rs, md, mb, mi, ms);
}

__jit_inline void
MOVQir(long im, jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _Or(0xb8, _r8(rd));
    _jit_L(im);
}

__jit_inline void
MOVQim(long im, int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _O(0xc7);
    _r_X(0, md, mb, mi, ms, 0);
    _jit_I(_s32(im));
}

/* --- Unary and Multiply/Divide instructions ------------------------------ */
__jit_inline void
_UNARYQr(int op, jit_gpr_t rs)
{
    _REXQrr(0, rs);
    _unary_sil_r(op, rs);
}

__jit_inline void
_UNARYQm(int op, int md, int mb, int mi, int ms)
{
    _REXQmr(mb, mi, 0);
    _unary_sil_m(op, md, mb, mi, ms);
}

#define NOTQr(RS)			_UNARYQr(X86_NOT, RS)
#define NOTQm(MD, MB, MI, MS)		_UNARYQm(X86_NOT, MD, MB, MI, MS)

#define NEGQr(RS)			_UNARYQr(X86_NEG, RS)
#define NEGQm(MD, MB, MI, MS)		_UNARYQm(X86_NEG, MD, MB, MI, MS)

#define MULQr(RS)			_UNARYQr(X86_MUL, RS)
#define MULQm(MD, MB, MI, MS)		_UNARYQm(X86_MUL, MD, MB, MI, MS)

#define IMULQr(RS)			_UNARYQr(X86_IMUL, RS)
#define IMULQm(MD, MB, MI, MS)		_UNARYQm(X86_IMUL, MD, MB, MI, MS)

#define DIVQr(RS)			_UNARYQr(X86_DIV, RS)
#define DIVQm(MD, MB, MI, MS)		_UNARYQm(X86_DIV, MD, MB, MI, MS)

#define IDIVQr(RS)			_UNARYQr(X86_IDIV, RS)
#define IDIVQm(MD, MB, MI, MS)		_UNARYQm(X86_IDIV, MD, MB, MI, MS)

__jit_inline void
IMULQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _imul_sil_rr(rs, rd);
}

__jit_inline void
IMULQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _imul_sil_mr(md, mb, mi, ms, rd);
}

#define IMULQir(im, rd)			IMULQirr(im, rd, rd)
__jit_inline void
IMULQirr(long im, jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _imul_il_irr(im, rs, rd);
}

/* --- Control Flow related instructions ----------------------------------- */
__jit_inline void
CALLQsr(jit_gpr_t rs)
{
    _REXQrr(0, rs);
    _call_il_sr(rs);
}

__jit_inline void
JMPQsr(jit_gpr_t rs)
{
    _REXQrr(0, rs);
    _jmp_il_sr(rs);
}

__jit_inline void
CMOVQrr(int cc,jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _cmov_sil_rr(cc, rs, rd);
}

__jit_inline void
CMOVQmr(int cc, int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _cmov_sil_mr(cc, md, mb, mi, ms, rd);
}

/* --- Push/Pop instructions ----------------------------------------------- */
__jit_inline void
POPQr(jit_gpr_t rd)
{
    _REXQr(rd);
    _pop_sil_r(rd);
}

__jit_inline void
POPQm(int md, int mb, int mi, int ms)
{
    _REXQm(mb, mi);
    _pop_sil_m(md, mb, mi, ms);
}

__jit_inline void
PUSHQr(jit_gpr_t rs)
{
    _REXQr(rs);
    _push_sil_r(rs);
}

__jit_inline void
PUSHQm(int md, int mb, int mi, int ms)
{
    _REXQm(mb, mi);
    _push_sil_m(md, mb, mi, ms);
}

__jit_inline void
PUSHQi(long im)
{
    _push_il_i(im);
}

/* --- Test instructions --------------------------------------------------- */
__jit_inline void
TESTQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _test_sil_rr(rs, rd);
}

__jit_inline void
TESTQrm(jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _test_sil_rm(rs, md, mb, mi, ms);
}

__jit_inline void
TESTQir(long im, jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _test_il_ir(im, rd);
}

__jit_inline void
TESTQim(long im, int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _test_il_im(im, md, mb, mi, ms);
}

/* --- Exchange instructions ----------------------------------------------- */
__jit_inline void
CMPXCHGQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _cmpxchg_sil_rr(rs, rd);
}

__jit_inline void
CMPXCHGQrm(jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _cmpxchg_sil_rm(rs, md, mb, mi, ms);
}

__jit_inline void
XADDQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _xadd_sil_rr(rs, rd);
}

__jit_inline void
XADDQrm(jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _xadd_sil_rm(rs, md, mb, mi, ms);
}

__jit_inline void
XCHGQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _xchg_sil_rr(rs, rd);
}

__jit_inline void
XCHGQrm(jit_gpr_t rs, int md, int mb, int mi, int ms)
{
    _REXQrm(rs, mb, mi);
    _xchg_sil_rm(rs, md, mb, mi, ms);
}

/* --- Increment/Decrement instructions ------------------------------------ */
__jit_inline void
DECQr(jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _dec_sil_r(rd);
}

__jit_inline void
DECQm(int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _dec_sil_m(md, mb, mi, ms);
}

__jit_inline void
INCQr(jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _inc_sil_r(rd);
}

__jit_inline void
INCQm(int md, int mb, int mi, int ms)
{
    _REXQrm(0, mb, mi);
    _inc_sil_m(md, mb, mi, ms);
}

/* --- Misc instructions --------------------------------------------------- */
__jit_inline void
BSFQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _bsf_sil_rr(rs, rd);
}

__jit_inline void
BSFQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _bsf_sil_mr(md, mb, mi, ms, rd);
}

__jit_inline void
BSRQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _bsr_sil_rr(rs, rd);
}

__jit_inline void
BSRQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _bsr_sil_mr(md, mb, mi, ms, rd);
}

/* long rd = (int)rs */
__jit_inline void
_movsd_l_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x63);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

/* long rd = (int)*rs */
__jit_inline void
_movsd_l_mr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _O(0x63);
    _r_X(_rA(rd), md, mb, mi, ms, 0);
}

__jit_inline void
MOVSBQrr(jit_gpr_t rs, jit_gpr_t rd)		
{
    _REXQrr(rd, rs);
    _movsb_sil_rr(rs, rd);
}

__jit_inline void
MOVSBQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _movsb_sil_mr(md, mb, mi, ms, rd);
}

__jit_inline void
MOVSWQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _movsw_il_rr(rs, rd);
}

__jit_inline void
MOVSWQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _movsw_il_mr(md, mb, mi, ms, rd);
}

__jit_inline void
MOVSLQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _movsd_l_rr(rs, rd);
}

__jit_inline void
MOVSLQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _movsd_l_mr(md, mb, mi, ms, rd);
}

__jit_inline void
MOVZBQrr(jit_gpr_t rs, jit_gpr_t rd)		
{
    _REXQrr(rd, rs);
    _movzb_sil_rr(rs, rd);
}

__jit_inline void
MOVZBQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _movzb_sil_mr(md, mb, mi, ms, rd);
}

__jit_inline void
MOVZWQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _movzw_il_rr(rs, rd);
}

__jit_inline void
MOVZWQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _movzw_il_mr(md, mb, mi, ms, rd);
}

__jit_inline void
LEAQmr(int md, int mb, int mi, int ms, jit_gpr_t rd)
{
    _REXQmr(mb, mi, rd);
    _lea_il_mr(md, mb, mi, ms, rd);
}

__jit_inline void
BSWAPQr(jit_gpr_t rd)
{
    _REXQrr(0, rd);
    _bswap_il_r(rd);
}

/* long rax = (int)eax */
#define CLTQ_				CDQE_
__jit_inline void
CDQE_(void)
{
    _REXQrr(0, 0);
    _sign_extend_rax();
}

#define CQTO_()				CQO_()
/* long rdx:rax = rax */
__jit_inline void
CQO_(void)
{
    _REXQrr(0, 0);
    _sign_extend_rdx_rax();
}


#define __SSEQrr(OP,RS,RSA,RD,RDA)		(_REXQrr(RD, RS),		_OO_Mrm		(0x0f00|(OP)	,_b11,RDA(RD),RSA(RS)				))
#define __SSEQmr(OP,MD,MB,MI,MS,RD,RDA)		(_REXQmr(MB, MI, RD),		_OO_r_X		(0x0f00|(OP)	     ,RDA(RD)		,MD,MB,MI,MS		))
#define __SSEQrm(OP,RS,RSA,MD,MB,MI,MS)		(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0f00|(OP)	     ,RSA(RS)		,MD,MB,MI,MS		))
#define __SSEQ1rm(OP,RS,RSA,MD,MB,MI,MS)	(_REXQrm(RS, MB, MI),		_OO_r_X		(0x0f01|(OP)	     ,RSA(RS)		,MD,MB,MI,MS		))

#define _SSEQrr(PX,OP,RS,RSA,RD,RDA)					(_jit_B(PX), __SSEQrr(OP, RS, RSA, RD, RDA))
#define _SSEQmr(PX,OP,MD,MB,MI,MS,RD,RDA)				(_jit_B(PX), __SSEQmr(OP, MD, MB, MI, MS, RD, RDA))
#define _SSEQrm(PX,OP,RS,RSA,MD,MB,MI,MS)				(_jit_B(PX), __SSEQrm(OP, RS, RSA, MD, MB, MI, MS))
#define _SSEQ1rm(PX,OP,RS,RSA,MD,MB,MI,MS)				(_jit_B(PX), __SSEQ1rm(OP, RS, RSA, MD, MB, MI, MS))

#define CVTTSS2SIQrr(RS, RD)		 _SSEQrr(0xf3, X86_SSE_CVTTSI, RS,_rX, RD,_r8)
#define CVTTSS2SIQmr(MD, MB, MI, MS, RD) _SSEQmr(0xf3, X86_SSE_CVTTSI, MD, MB, MI, MS, RD,_r8)
#define CVTTSD2SIQrr(RS, RD)		 _SSEQrr(0xf2, X86_SSE_CVTTSI, RS,_rX, RD,_r8)
#define CVTTSD2SIQmr(MD, MB, MI, MS, RD) _SSEQmr(0xf2, X86_SSE_CVTTSI, MD, MB, MI, MS, RD,_r8)

#define CVTSS2SIQrr(RS, RD)		 _SSEQrr(0xf3, X86_SSE_CVTSI, RS,_rX, RD,_r8)
#define CVTSS2SIQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf3, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_r8)
#define CVTSD2SIQrr(RS, RD)		 _SSEQrr(0xf2, X86_SSE_CVTSI, RS,_rX, RD,_r8)
#define CVTSD2SIQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf2, X86_SSE_CVTSI, MD, MB, MI, MS, RD,_r8)

#define CVTSI2SSQrr(RS, RD)		 _SSEQrr(0xf3, X86_SSE_CVTIS, RS,_r8, RD,_rX)
#define CVTSI2SSQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf3, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)
#define CVTSI2SDQrr(RS, RD)		 _SSEQrr(0xf2, X86_SSE_CVTIS, RS,_r8, RD,_rX)
#define CVTSI2SDQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf2, X86_SSE_CVTIS, MD, MB, MI, MS, RD,_rX)

#define MOVDQXrr(RS, RD)		 _SSEQrr(0x66, 0x6e, RS,_r8, RD,_rX)
#define MOVDQXmr(MD, MB, MI, MS, RD)	 _SSEQmr(0x66, 0x6e, MD, MB, MI, MS, RD,_rX)

#define MOVDXQrr(RS, RD)		 _SSEQrr(0x66, 0x7e, RS,_rX, RD,_r8)
#define MOVDXQrm(RS, MD, MB, MI, MS)	 _SSEQrm(0x66, 0x7e, RS,_rX, MD, MB, MI, MS)
#define MOVDQMrr(RS, RD)		__SSEQrr(      0x6e, RS,_r8, RD,_rM)
#define MOVDQMmr(MD, MB, MI, MS, RD)	__SSEQmr(      0x6e, MD, MB, MI, MS, RD,_rM)
#define MOVDMQrr(RS, RD)		__SSEQrr(      0x7e, RS,_rM, RD,_r8)
#define MOVDMQrm(RS, MD, MB, MI, MS)	__SSEQrm(      0x7e, RS,_rM, MD, MB, MI, MS)

#endif	/* LIGHTNING_DEBUG */
#endif	/* __lightning_asm_h */
