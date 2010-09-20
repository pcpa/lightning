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

#if 0
#  define _r0P(R)		((int)(R) == (int)_NOREG)
#  define _rIP(R)		((int)(R) == (int)_RIP)
#  define _rspP(R)		(_rR(R) == _rR(_RSP))
#  define _rsp12P(R)		(_rN(R) == _rN(_RSP))

#  define _x86_RIP_addressing_possible(D,O)	(X86_RIP_RELATIVE_ADDR && \
						((unsigned long)x86_get_target() + 4 + (O) - (D) <= 0xffffffff))

#  define _r_X(   R, D,B,I,S,O)	(_r0P(I) ? (_r0P(B)    ? (!X86_TARGET_64BIT ? _r_D(R,D) : \
					                 (_x86_RIP_addressing_possible(D, O) ? \
				                          _r_D(R, (D) - ((unsigned long)x86_get_target() + 4 + (O))) : \
				                          _r_DSIB(R,D))) : \
					                 _r_DSIB(R,D                ))  : \
				           (_rIP(B)    ? _r_D   (R,D                )   : \
				           (_rsp12P(B) ? _r_DBIS(R,D,_RSP,_RSP,1)   : \
						         _r_DB  (R,D,     B       ))))  : \
				 (_r0P(B)	       ? _r_4IS (R,D,	         I,S)   : \
				 (!_rspP(I)            ? _r_DBIS(R,D,     B,     I,S)   : \
						         JITFAIL("illegal index register: %esp"))))
#endif

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
#define _r1e8lP(R)		((int)(R) >= _RSP && (int)(R) <= _RDX)

__jit_inline void
__REXwrxb(int l, int w, int r, int x, int b)
{
    int		rex = (w << 3) | (r << 2) | (x << 1) | b;

    if (rex || l)
	_jit_B(0x40 | rex);
}

__jit_inline void
__REXwrx_(int l, int w, int r, int x, int mr)
{
    int		b = mr == _RIP ? 0 : _rXP(mr);

    __REXwrxb(l, w, r, x, _BIT(b));
}

__jit_inline void
__REXw_x_(int l, int w, int r, int x, int mr)
{
    __REXwrx_(l, w, _BIT(_rXP(r)), x, mr);
}

__jit_inline void
__REX_reg(int rr)
{
    __REXwrxb(0, 0, 0, 0, _BIT(_rXP(rr)));
}

__jit_inline void
__REX_mem(int mb, int mi)
{
    __REXwrxb(0, 0, 0, _BIT(_rXP(mi)), _BIT(_rXP(mb)));
}

__jit_inline void
_rex_b_rr(int rr, int mr)
{
    __REXw_x_(_r1e8lP(rr) || _r1e8lP(mr), 0, rr, 0, mr);
}

__jit_inline void
_rex_b_mr(int rb, int ri, int rd)
{
    __REXw_x_(_r1e8lP(rd) || _r1e8lP(rb), 0, rd, _BIT(_rXP(ri)), rb);
}

__jit_inline void
_rex_b_rm(int rs, int rb, int ri)
{
    _rex_b_mr(rb, ri, rs);
}

__jit_inline void
_rex_bl_rr(int rr, int mr)
{
    __REXw_x_(_r1e8lP(mr), 0, rr, 0, mr);
}

__jit_inline void
_rex_l_r(int rr)
{
    __REX_reg(rr);
}

__jit_inline void
_rex_l_m(int rb, int ri)
{
    __REX_mem(rb, ri);
}

__jit_inline void
_rex_l_rr(int rr, int mr)
{
    __REXw_x_(0, 0, rr, 0, mr);
}

__jit_inline void
_rex_l_mr(int rb, int ri, int rd)
{
    __REXw_x_(0, 0, rd, _BIT(_rXP(ri)), rb);
}

__jit_inline void
_rex_l_rm(int rs, int rb, int ri)
{
    _rex_l_mr(rb, ri, rs);
}

__jit_inline void
_rex_q_rr(int rr, int mr)
{
    __REXw_x_(0, 1, rr, 0, mr);
}

__jit_inline void
_rex_q_mr(int rb, int ri, int rd)
{
    __REXw_x_(0, 1, rd, _BIT(_rXP(ri)), rb);
}

__jit_inline void
_rex_q_rm(int rs, int rb, int ri)
{
    _rex_q_mr(rb, ri, rs);
}

__jit_inline void
_rex_q_r(int rr)
{
    __REX_reg(rr);
}

__jit_inline void
_rex_q_m(int rb, int ri)
{
    __REX_mem(rb, ri);
}

__jit_inline void
_REXBrr(jit_gpr_t rr, jit_gpr_t mr)
{
    _rex_b_rr((int)rr, (int)mr);
}

__jit_inline void
_REXBmr(jit_gpr_t rb, jit_gpr_t ri, jit_gpr_t rd)
{
    _rex_b_mr((int)rb, (int)ri, (int)rd);
}

__jit_inline void
_REXBrm(jit_gpr_t rs, jit_gpr_t rb, jit_gpr_t ri)
{
    _rex_b_rm((int)rs, (int)rb, (int)ri);
}

__jit_inline void
_REXBLrr(jit_gpr_t rr, jit_gpr_t mr)
{
    _rex_bl_rr((int)rr, (int)mr);
}

__jit_inline void
_REXLr(jit_gpr_t rr)
{
    _rex_l_r((int)rr);
}

__jit_inline void
_REXLm(jit_gpr_t rb, jit_gpr_t ri)
{
    _rex_l_m((int)rb, (int)ri);
}

__jit_inline void
_REXLrr(jit_gpr_t rr, jit_gpr_t mr)
{
    _rex_l_rr((int)rr, (int)mr);
}

__jit_inline void
_REXLmr(jit_gpr_t rb, jit_gpr_t ri, jit_gpr_t rd)
{
    _rex_l_mr((int)rb, (int)ri, (int)rd);
}

__jit_inline void
_REXLrm(jit_gpr_t rs, jit_gpr_t rb, jit_gpr_t ri)
{
    _rex_l_rm((int)rs, (int)rb, (int)ri);
}

__jit_inline void
_REXQrr(jit_gpr_t rr, jit_gpr_t mr)
{
    _rex_q_rr((int)rr, (int)mr);
}

__jit_inline void
_REXQmr(jit_gpr_t rb, jit_gpr_t ri, jit_gpr_t rd)
{
    _rex_q_mr((int)rb, (int)ri, (int)rd);
}

__jit_inline void
_REXQrm(jit_gpr_t rs, jit_gpr_t rb, jit_gpr_t ri)
{
    _rex_q_rm((int)rs, (int)rb, (int)ri);
}

__jit_inline void
_REXQr(jit_gpr_t rr)
{
    _rex_q_r((int)rr);
}

__jit_inline void
_REXQm(jit_gpr_t rb, jit_gpr_t ri)
{
    _rex_q_m((int)rb, (int)ri);
}

__jit_inline void
_REXQFrr(jit_gpr_t rr, jit_fpr_t mr)
{
    _rex_q_rr((int)rr, (int)mr);
}

__jit_inline void
_REXLFrr(jit_gpr_t rr, jit_fpr_t mr)
{
    _rex_l_rr((int)rr, (int)mr);
}

__jit_inline void
_REXFrr(jit_fpr_t rr, jit_fpr_t mr)
{
    _rex_l_rr((int)rr, (int)mr);
}

__jit_inline void
_REXFLrr(jit_fpr_t rr, jit_gpr_t mr)
{
    _rex_l_rr((int)rr, (int)mr);
}

__jit_inline void
_REXFmr(jit_gpr_t rb, jit_gpr_t ri, jit_fpr_t rd)
{
    _rex_l_mr((int)rb, (int)ri, (int)rd);
}

__jit_inline void
_REXFrm(jit_fpr_t rs, jit_gpr_t rb, jit_gpr_t ri)
{
    _rex_l_rm((int)rs, (int)rb, (int)ri);
}

__jit_inline void
_REXFQrr(jit_fpr_t rr, jit_gpr_t mr)
{
    _rex_q_rr((int)rr, (int)mr);
}

/* --- ALU instructions ---------------------------------------------------- */
__jit_inline void
_ALUQrr(x86_alu_t op,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _alu_sil_rr(op, rs, rd);
}

__jit_inline void
_ALUQmr(x86_alu_t op,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _alu_sil_mr(op, md, rb, ri, ms, rd);
}

__jit_inline void
_ALUQrm(x86_alu_t op,
	jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _alu_sil_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_ALUQir(x86_alu_t op,
	long im, jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _alu_il_ir(op, im, rd);
}

__jit_inline void
_ALUQim(x86_alu_t op,
	long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _alu_il_im(op, im, md, rb, ri, ms);
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
_ROTSHIQir(x86_rotsh_t op,
	   long im, jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _rotsh_sil_ir(op, im, rd);
}

__jit_inline void
_ROTSHIQim(x86_rotsh_t op,
	   long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _rotsh_sil_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_ROTSHIQrr(x86_rotsh_t op,
	   jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _rotsh_sil_rr(op, rs, rd);
}

__jit_inline void
_ROTSHIQrm(x86_rotsh_t op,
	   jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _rotsh_sil_rm(op, rs, md, rb, ri, ms);
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
_BTQir(x86_bt_t op,
       long im, jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _bt_sil_ir(op, im, rd);
}

__jit_inline void
_BTQim(x86_bt_t op,
       long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _bt_sil_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_BTQrr(x86_bt_t op,
       jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _bt_sil_rr(op, rs, rd);
}

__jit_inline void
_BTQrm(x86_bt_t op,
       jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _bt_sil_rm(op, rs, md, rb, ri, ms);
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
MOVQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _mov_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVQrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _mov_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
MOVQir(long im, jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _Or(0xb8, _r8(rd));
    _jit_L(im);
}

__jit_inline void
MOVQim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _O(0xc7);
    _i_X(_b000, md, rb, ri, ms);
    _jit_I(_s32(im));
}

/* --- Unary and Multiply/Divide instructions ------------------------------ */
__jit_inline void
_UNARYQr(x86_unary_t op,
	 jit_gpr_t rs)
{
    _REXQrr(_NOREG, rs);
    _unary_sil_r(op, rs);
}

__jit_inline void
_UNARYQm(x86_unary_t op,
	 long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQmr(rb, ri, _NOREG);
    _unary_sil_m(op, md, rb, ri, ms);
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
IMULQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _imul_sil_mr(md, rb, ri, ms, rd);
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
    _REXQrr(_NOREG, rs);
    _call_il_sr(rs);
}

__jit_inline void
JMPQsr(jit_gpr_t rs)
{
    _REXQrr(_NOREG, rs);
    _jmp_il_sr(rs);
}

__jit_inline void
CMOVQrr(x86_cc_t cc,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _cmov_sil_rr(cc, rs, rd);
}

__jit_inline void
CMOVQmr(x86_cc_t cc,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _cmov_sil_mr(cc, md, rb, ri, ms, rd);
}

/* --- Push/Pop instructions ----------------------------------------------- */
__jit_inline void
POPQr(jit_gpr_t rd)
{
    _REXQr(rd);
    _pop_sil_r(rd);
}

__jit_inline void
POPQm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQm(rb, ri);
    _pop_sil_m(md, rb, ri, ms);
}

__jit_inline void
PUSHQr(jit_gpr_t rs)
{
    _REXQr(rs);
    _push_sil_r(rs);
}

__jit_inline void
PUSHQm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQm(rb, ri);
    _push_sil_m(md, rb, ri, ms);
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
TESTQrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _test_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
TESTQir(long im, jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _test_il_ir(im, rd);
}

__jit_inline void
TESTQim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _test_il_im(im, md, rb, ri, ms);
}

/* --- Exchange instructions ----------------------------------------------- */
__jit_inline void
CMPXCHGQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _cmpxchg_sil_rr(rs, rd);
}

__jit_inline void
CMPXCHGQrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _cmpxchg_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XADDQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _xadd_sil_rr(rs, rd);
}

__jit_inline void
XADDQrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _xadd_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XCHGQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rs, rd);
    _xchg_sil_rr(rs, rd);
}

__jit_inline void
XCHGQrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(rs, rb, ri);
    _xchg_sil_rm(rs, md, rb, ri, ms);
}

/* --- Increment/Decrement instructions ------------------------------------ */
__jit_inline void
DECQr(jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _dec_sil_r(rd);
}

__jit_inline void
DECQm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _dec_sil_m(md, rb, ri, ms);
}

__jit_inline void
INCQr(jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _inc_sil_r(rd);
}

__jit_inline void
INCQm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXQrm(_NOREG, rb, ri);
    _inc_sil_m(md, rb, ri, ms);
}

/* --- Misc instructions --------------------------------------------------- */
__jit_inline void
BSFQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _bsf_sil_rr(rs, rd);
}

__jit_inline void
BSFQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _bsf_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
BSRQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _bsr_sil_rr(rs, rd);
}

__jit_inline void
BSRQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _bsr_sil_mr(md, rb, ri, ms, rd);
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
_movsd_l_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x63);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
MOVSBQrr(jit_gpr_t rs, jit_gpr_t rd)		
{
    _REXQrr(rd, rs);
    _movsb_sil_rr(rs, rd);
}

__jit_inline void
MOVSBQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _movsb_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVSWQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _movsw_il_rr(rs, rd);
}

__jit_inline void
MOVSWQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _movsw_il_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVSLQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _movsd_l_rr(rs, rd);
}

__jit_inline void
MOVSLQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _movsd_l_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVZBQrr(jit_gpr_t rs, jit_gpr_t rd)		
{
    _REXQrr(rd, rs);
    _movzb_sil_rr(rs, rd);
}

__jit_inline void
MOVZBQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _movzb_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVZWQrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXQrr(rd, rs);
    _movzw_il_rr(rs, rd);
}

__jit_inline void
MOVZWQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _movzw_il_mr(md, rb, ri, ms, rd);
}

__jit_inline void
LEAQmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXQmr(rb, ri, rd);
    _lea_il_mr(md, rb, ri, ms, rd);
}

__jit_inline void
BSWAPQr(jit_gpr_t rd)
{
    _REXQrr(_NOREG, rd);
    _bswap_il_r(rd);
}

/* long rax = (int)eax */
#define CLTQ_				CDQE_
__jit_inline void
CDQE_(void)
{
    _REXQrr(_NOREG, _NOREG);
    _sign_extend_rax();
}

#define CQTO_()				CQO_()
/* long rdx:rax = rax */
__jit_inline void
CQO_(void)
{
    _REXQrr(_NOREG, _NOREG);
    _sign_extend_rdx_rax();
}

#define __SSEQrr(OP, RS, RD)						\
    (_REXQrr(RD, RS),							\
     _OO(0x0f00 | (OP)),						\
     _Mrm(_b11, _rX(RD), _rX(RS)))

#define __SSEQFrr(OP, RS, RD)						\
    (_REXQFrr(RD, RS),							\
     _OO(0x0f00 | (OP)),						\
     _Mrm(_b11, _rA(RD), _rX(RS)))

#define __SSEFQrr(OP, RS, RD)						\
    (_REXFQrr(RD, RS),							\
     _OO(0x0f00 | (OP)),						\
     _Mrm(_b11, _rX(RD), _rA(RS)))

#define __SSEQmr(OP,MD,MB,MI,MS,RD)					\
    (_REXQmr(MB, MI, RD),						\
     _OO(0x0f00 | (OP)),						\
     _r_X(RD, MD, MB, MI, MS))

#define __SSEQrm(OP,RS,MD,MB,MI,MS)					\
    (_REXQrm(RS, MB, MI),						\
     _OO(0x0f00 | (OP)),						\
     _r_X(RS, MD, MB, MI, MS))

#define __SSEQ1rm(OP,RS,MD,MB,MI,MS)					\
    (_REXQrm(RS, MB, MI),						\
     _OO(0x0f01 | (OP)),						\
     _r_X(RS, MD, MB, MI, MS))

#define _SSEQrr(PX, OP, RS, RD)						\
    (_jit_B(PX), __SSEQrr(OP, RS, RD))
#define _SSEQFrr(PX, OP, RS, RD)					\
    (_jit_B(PX), __SSEQFrr(OP, RS, RD))
#define _SSEFQrr(PX, OP, RS, RD)					\
    (_jit_B(PX), __SSEFQrr(OP, RS, RD))
#define _SSEQmr(PX, OP, MD, MB, MI, MS, RD)				\
    (_jit_B(PX), __SSEQmr(OP, MD, MB, MI, MS, RD))
#define _SSEQrm(PX, OP, RS, MD, MB, MI, MS)				\
    (_jit_B(PX), __SSEQrm(OP, RS, MD, MB, MI, MS))
#define _SSEQ1rm(PX, OP, RS, MD, MB, MI, MS)				\
    (_jit_B(PX), __SSEQ1rm(OP, RS, MD, MB, MI, MS))

#define CVTTSS2SIQrr(RS, RD)		 _SSEQFrr(0xf3, X86_SSE_CVTTSI, RS, RD)
#define CVTTSS2SIQmr(MD, MB, MI, MS, RD) _SSEQmr(0xf3, X86_SSE_CVTTSI, MD, MB, MI, MS, RD)
#define CVTTSD2SIQrr(RS, RD)		 _SSEQFrr(0xf2, X86_SSE_CVTTSI, RS, RD)
#define CVTTSD2SIQmr(MD, MB, MI, MS, RD) _SSEQmr(0xf2, X86_SSE_CVTTSI, MD, MB, MI, MS, RD)

#define CVTSS2SIQrr(RS, RD)		 _SSEQFrr(0xf3, X86_SSE_CVTSI, RS, RD)
#define CVTSS2SIQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf3, X86_SSE_CVTSI, MD, MB, MI, MS, RD)
#define CVTSD2SIQrr(RS, RD)		 _SSEQFrr(0xf2, X86_SSE_CVTSI, RS, RD)
#define CVTSD2SIQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf2, X86_SSE_CVTSI, MD, MB, MI, MS, RD)

#define CVTSI2SSQrr(RS, RD)		 _SSEFQrr(0xf3, X86_SSE_CVTIS, RS, RD)
#define CVTSI2SSQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf3, X86_SSE_CVTIS, MD, MB, MI, MS, RD)
#define CVTSI2SDQrr(RS, RD)		 _SSEFQrr(0xf2, X86_SSE_CVTIS, RS, RD)
#define CVTSI2SDQmr(MD, MB, MI, MS, RD)	 _SSEQmr(0xf2, X86_SSE_CVTIS, MD, MB, MI, MS, RD)

#define MOVDQXrr(RS, RD)		 _SSEFQrr(0x66, 0x6e, RS, RD)
#define MOVDQXmr(MD, MB, MI, MS, RD)	 _SSEQmr(0x66, 0x6e, MD, MB, MI, MS, RD)

#define MOVDXQrr(RS, RD)		 _SSEQrr(0x66, 0x7e, RS, RD)
#define MOVDXQrm(RS, MD, MB, MI, MS)	 _SSEQrm(0x66, 0x7e, RS, MD, MB, MI, MS)
#define MOVDQMrr(RS, RD)		__SSEQrr(      0x6e, RS, RD)
#define MOVDQMmr(MD, MB, MI, MS, RD)	__SSEQmr(      0x6e, MD, MB, MI, MS, RD)
#define MOVDMQrr(RS, RD)		__SSEQrr(      0x7e, RS, RD)
#define MOVDMQrm(RS, MD, MB, MI, MS)	__SSEQrm(      0x7e, RS, MD, MB, MI, MS)

#endif	/* LIGHTNING_DEBUG */
#endif	/* __lightning_asm_h */
