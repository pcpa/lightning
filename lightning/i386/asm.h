/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the i386
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 1999, 2000, 2001, 2002 Ian Piumarta
 * Copyright 2003 Gwenole Beauchesne
 * Copyright 2006 Free Software Foundation
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
 ***********************************************************************/

#ifndef __lightning_asm_i386_h
#define __lightning_asm_i386_h
#ifndef LIGHTNING_DEBUG

/*	OPCODE	+ i		= immediate operand
 *		+ r		= register operand
 *		+ m		= memory operand (disp,base,index,scale)
 *		+ sr/sm		= a star preceding a register or memory
 */

#define _b00		0
#define _b01		1
#define _b10		2
#define _b11		3

#define _b000		0
#define _b001		1
#define _b010		2
#define _b011		3
#define _b100		4
#define _b101		5
#define _b110		6
#define _b111		7

typedef enum {
    _RIP	= -2,
    _NOREG	= 0,
    _RAX	= 0x50,
    _RCX	= 0x51,
    _RDX	= 0x52,
    _RBX	= 0x53,
    _RSP	= 0x54,
    _RBP	= 0x55,
    _RSI	= 0x56,
    _RDI	= 0x57,
    _R8		= 0x58,
    _R9		= 0x59,
    _R10	= 0x5a,
    _R11	= 0x5b,
    _R12	= 0x5c,
    _R13	= 0x5d,
    _R14	= 0x5e,
    _R15	= 0x5f,
} jit_gpr_t;

typedef enum {
    _ST0	= 0,
    _ST1	= 1,
    _ST2	= 2,
    _ST3	= 3,
    _ST4	= 4,
    _ST5	= 5,
    _ST6	= 6,
    _ST7	= 7,
    _XMM0	= 0x60,
    _XMM1	= 0x61,
    _XMM2	= 0x62,
    _XMM3	= 0x63,
    _XMM4	= 0x64,
    _XMM5	= 0x65,
    _XMM6	= 0x66,
    _XMM7	= 0x67,
    _XMM8	= 0x68,
    _XMM9	= 0x69,
    _XMM10	= 0x6a,
    _XMM11	= 0x6b,
    _XMM12	= 0x6c,
    _XMM13	= 0x6d,
    _XMM14	= 0x6e,
    _XMM15	= 0x6f,
} jit_fpr_t;

typedef enum {
    _SCL1	= _b00,
    _SCL2	= _b01,
    _SCL4	= _b10,
    _SCL8	= _b11,
} jit_scl_t;

#if __WORDSIZE == 32
#  define _RMAX			_RDI
#  define _XMAX			_XMM7
#  define _rA(R)		_r4(R)
#  define jit_reg8_p(rs)	((rs) >= _RAX && (rs) <= _RBX)
#else
#  define _RMAX			_R15
#  define _XMAX			_XMM15
#  define _rA(R)		_r8(R)
#  define jit_reg8_p(rs)	1
#endif

#if !_ASM_SAFETY
#  define _r1(R)		_rN(R)
#  define _r2(R)		_rN(R)
#  define _r4(R)		_rN(R)
#  define _r8(R)		_rN(R)
#  define _rM(R)		_rN(R)
#  define _rX(R)		_rN(R)
#else
#  define _r1(R)							\
    (((R) >= _RAX && (R) <= _RMAX)					\
	? _rN(R)							\
	: JITFAIL("bad 8-bit register " #R))
#  define _r2(R)							\
    (((R) >= _RAX && (R) <= _RMAX)					\
	? _rN(R)							\
	: JITFAIL("bad 16-bit register " #R))
#  define _r4(R)							\
    (((R) >= _RAX && (R) <= _RMAX)					\
	? _rN(R)							\
	: JITFAIL("bad 32-bit register " #R))
#  if __WORDSIZE == 32
#    define _r8(R)							\
	JITFAIL("bad 64-bit register " #R)
#  else
#    define _r8(R)							\
    (((R) >= _RAX && (R) <= _R15)					\
	? _rN(R)							\
	: JITFAIL("bad 64-bit register " #R))
#  endif
#  define _rM(R)							\
    (((R) >= _XMM0 && (R) <= _XMM7)					\
	? _rN(R)							\
	: JITFAIL("bad MMX register " #R))
#  define _rX(R)							\
    (((R) >= _XMM0 && (R) <= _XMAX)					\
	? _rN(R)							\
	: JITFAIL("bad SSE register " #R))
#endif

#define _sign_extend_rax()	_O(0x98)
#define _sign_extend_rdx_rax()	_O(0x99)

#define _rC(R)			((R) & 0xf0)
#define _rR(R)			((R) & 0x0f)
#define _rN(R)			((R) & 0x07)
#define _rXP(R)			((R) > 0 && _rR(R) > 7)

#define _OFF4(D)		(_jit_SL(D) - _jit_SL(_jit.x.pc))
#define _CKD8(D)		_s8(_OFF4(D))
#define _CKD32(D)		_s32(_OFF4(D))

#define _D8(D)								\
    (_jit_B(0), ((*(_PUC(_jit.x.pc)-1))= _CKD8(D)))
#define _D32(D)								\
    (_jit_I(0), ((*(_PUI(_jit.x.pc)-1))= _CKD32(D)))

#ifndef _ASM_SAFETY
# define _M(M)			(M)
# define _r(R)			(R)
# define _m(M)			(M)
# define _s(S)			(S)
# define _i(I)			(I)
# define _b(B)			(B)
#else
# define _M(M)								\
    (((M) & ~3) ? JITFAIL("internal error: mod = " #M) : (M))
# define _r(R)								\
    (((R) & ~7) ? JITFAIL("internal error: reg = " #R) : (R))
# define _m(M)								\
    (((M) & ~7) ? JITFAIL("internal error: r/m = " #M) : (M))
# define _s(S)								\
    (((S) & ~3) ? JITFAIL("internal error: memory scale = " #S) : (S))
# define _i(I)								\
    (((I) & ~7) ? JITFAIL("internal error: memory index = " #I) : (I))
# define _b(B)								\
    (((B) & ~7) ? JITFAIL("internal error: memory base = "  #B) : (B))
#endif

#define _Mrm(Md, R, M)							\
    _jit_B((_M(Md) << 6) | (_r(R) << 3) | _m(M))

#define _SIB(Sc, I, B)							\
    _jit_B((_s(Sc) << 6) | (_i(I) << 3) | _b(B))

/* _r_D() is RIP addressing mode if X86_TARGET_64BIT, use _r_DSIB() instead */
__jit_inline void
_r_D(int rd, long md)
{
    _Mrm(_b00, rd, _b101);
    _jit_I(_s32(md));
}

__jit_inline void
_r_DSIB(int rd, long md)
{
    _Mrm(_b00, rd, _b100);
    _SIB(_SCL1, _b100, _b101);
    _jit_I(_s32(md));
}

__jit_inline void
_r_0B(int rd, jit_gpr_t rb)
{
    _Mrm(_b00, rd, _rA(rb));
}

__jit_inline void
_r_0BIS(int rd, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _Mrm(_b00, rd, _b100);
    _SIB(ms, _rA(ri), _rA(rb));
}

__jit_inline void
_r_1B(int rd, long md, jit_gpr_t rb)
{
    _Mrm(_b01, rd, _rA(rb));
    _jit_B(md);
}

__jit_inline void
_r_1BIS(int rd, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _Mrm(_b01, rd, _b100);
    _SIB(ms, _rA(ri), _rA(rb));
    _jit_B(md);
}

__jit_inline void
_r_4B(int rd, long md, jit_gpr_t rb)
{
    _Mrm(_b10, rd, _rA(rb));
    _jit_I(_s32(md));
}

__jit_inline void
_r_4IS(int rd, long md, jit_gpr_t ri, jit_scl_t ms)
{
    _Mrm(_b00, rd, _b100);
    _SIB(ms, _rA(ri), _b101);
    _jit_I(_s32(md));
}

__jit_inline void
_r_4BIS(int rd, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _Mrm(_b10, rd, _b100);
    _SIB(ms, _rA(ri), _rA(rb));
    _jit_I(_s32(md));
}

__jit_inline void
_r_DB(int rd, long md, jit_gpr_t rb)
{
    if (md == 0 && _rN(rb) != _rN(_RBP))
	_r_0B(rd, rb);
    else if (_s8P(md))
	_r_1B(rd, md, rb);
    else
	_r_4B(rd, md, rb);
}

__jit_inline void
_r_DBIS(int rd, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (md == 0 && _rN(rb) != _rN(_RBP))
	_r_0BIS(rd, rb, ri, ms);
    else if (_s8P(md))
	_r_1BIS(rd, md, rb, ri, ms);
    else
	_r_4BIS(rd, md, rb, ri, ms);
}

/* Use RIP-addressing in 64-bit mode, if possible */
__jit_inline void
_i_X(int op, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (ri == _NOREG) {
	if (rb == _NOREG) {
#if __WORDSIZE == 32
	    _r_D(op, md);
#else
	    _r_DSIB(op, md);
#endif
	}
#if __WORDSIZE == 64
	else if (rb == _RIP)
	    _r_D(op, md);
#endif
	else if (_rN(rb) == _rN(_RSP))
	    _r_DBIS(op, md, _RSP, _RSP, ms);
	else
	    _r_DB(op, md, rb);
    }
    else if (rb == _NOREG)
	_r_4IS(op, md, ri, ms);
    else if (_rR(ri) != _rR(_RSP))
	_r_DBIS(op, md, rb, ri, ms);
    else
	JITFAIL("illegal index register: %esp");
}

__jit_inline void
_r_X(jit_gpr_t rd, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _i_X((int)_rA(rd), md, rb, ri, ms);
}

__jit_inline void
_f_X(jit_fpr_t rd, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _i_X((int)_rX(rd), md, rb, ri, ms);
}

/* --- Instruction formats ------------------------------------------------- */

#define _d16()			(_jit_B(0x66))
#define _O(OP) 			(_jit_B(OP))
#define _Or(OP, R)		(_jit_B((OP) | _r(R)))
#define _OO(OP)			(_jit_B((OP) >> 8), _jit_B((OP)))

/* --- ALU instructions ---------------------------------------------------- */
typedef enum {
    X86_ADD	= 0,
    X86_OR	= 1,
    X86_ADC	= 2,
    X86_SBB	= 3,
    X86_AND	= 4,
    X86_SUB	= 5,
    X86_XOR	= 6,
    X86_CMP	= 7,
} x86_alu_t;

__jit_inline void
_alu_c_rr(x86_alu_t op, jit_gpr_t rs, jit_gpr_t rd)
{
    _O((int)op << 3);
    _Mrm(_b11, _r1(rs), _r1(rd));
}

__jit_inline void
_alu_c_mr(x86_alu_t op,
	  long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(((int)op << 3) + 2);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_alu_c_rm(x86_alu_t op,
	  jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O((int)op << 3);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_alu_c_ir(x86_alu_t op,
	  long im, jit_gpr_t rd)
{
    if (rd == _RAX)
	_O(((int)op << 3) + 4);
    else {
	_O(0x80);
	_Mrm(_b11, (int)op, _r1(rd));
    }
    _jit_B(_s8(im));
}

__jit_inline void
_alu_c_im(x86_alu_t op,
	  long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x80);
    _i_X((int)op, md, rb, ri, ms);
    _jit_B(_s8(im));
}

__jit_inline void
_alu_s_ir(x86_alu_t op,
	  long im, jit_gpr_t rd)
{
    if (rd == _RAX) {
	_O(((int)op << 3) + 5);
	_jit_W(_s16(im));
    }
    else if (_s8P(im)) {
	_O(0x83);
	_Mrm(_b11, (int)op, _r2(rd));
	_jit_B(im);
    }
    else {
	_O(0x81);
	_Mrm(_b11, (int)op, _r2(rd));
	_jit_W(_s16(im));
    }
}

__jit_inline void
_alu_s_im(x86_alu_t op,
	  long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (_s8P(im)) {
	_O(0x83);
	_i_X((int)op, md, rb, ri, ms);
	_jit_B(im);
    }
    else {
	_O(0x81);
	_i_X((int)op, md, rb, ri, ms);
	_jit_W(_s16(im));
    }
}

__jit_inline void
_alu_sil_rr(x86_alu_t op,
	    jit_gpr_t rs, jit_gpr_t rd)
{
    _O(((int)op << 3) + 1);
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_alu_sil_mr(x86_alu_t op,
	    long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(((int)op << 3) + 3);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_alu_sil_rm(x86_alu_t op,
	    jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(((int)op << 3) + 1);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_alu_il_ir(x86_alu_t op,
	   long im, jit_gpr_t rd)
{
    if (_s8P(im)) {
	_O(0x83);
	_Mrm(_b11, (int)op, _rA(rd));
	_jit_B(im);
    }
    else {
	if (rd == _RAX)
	    _O(((int)op << 3) + 5);
	else {
	    _O(0x81);
	    _Mrm(_b11, (int)op, _rA(rd));
	}
	_jit_I(_s32(im));
    }
}

__jit_inline void
_alu_il_im(x86_alu_t op,
	   long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (_s8P(im)) {
	_O(0x83);
	_i_X((int)op, md, rb, ri, ms);
	_jit_B(im);
    }
    else {
	_O(0x81);
	_i_X((int)op, md, rb, ri, ms);
	_jit_I(_s32(im));
    }
}

/* --- Shift/Rotate instructions ------------------------------------------- */
typedef enum {
    X86_ROL	= 0,
    X86_ROR	= 1,
    X86_RCL	= 2,
    X86_RCR	= 3,
    X86_SHL	= 4,
    X86_SHR	= 5,
    X86_SAR	= 7,
} x86_rotsh_t;

__jit_inline void
_rotsh_c_rr(x86_rotsh_t op,
	    jit_gpr_t rs, jit_gpr_t rd)
{
    if (rs != _RCX)
	JITFAIL("source register must be RCX");
    _O(0xd2);
    _Mrm(_b11, (int)op, _rA(rd));
}

__jit_inline void
_rotsh_sil_rr(x86_rotsh_t op,
	      jit_gpr_t rs, jit_gpr_t rd)
{
    if (rs != _RCX)
	JITFAIL("source register must be RCX");
    _O(0xd3);
    _Mrm(_b11, (int)op, _rA(rd));
}

__jit_inline void
_rotsh_c_ir(x86_rotsh_t op,
	    long im, jit_gpr_t rd)
{
    if (im == 1) {
	_O(0xd0);
	_Mrm(_b11, (int)op, _rA(rd));
    }
    else {
	_O(0xc0);
	_Mrm(_b11, (int)op, _rA(rd));
	_jit_B(_u8(im));
    }
}

__jit_inline void
_rotsh_sil_ir(x86_rotsh_t op,
	      long im, jit_gpr_t rd)
{
    if (im == 1) {
	_O(0xd1);
	_Mrm(_b11, (int)op, _rA(rd));
    }
    else {
	_O(0xc1);
	_Mrm(_b11, (int)op, _rA(rd));
	_jit_B(_u8(im));
    }
}

__jit_inline void
_rotsh_c_im(x86_rotsh_t op,
	     long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (im == 1) {
	_O(0xd0);
	_i_X((int)op, md, rb, ri, ms);
    }
    else {
	_O(0xc0);
	_i_X((int)op, md, rb, ri, ms);
	_jit_B(_u8(im));
    }
}

__jit_inline void
_rotsh_sil_im(x86_rotsh_t op,
	       long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (im == 1) {
	_O(0xd1);
	_i_X((int)op, md, rb, ri, ms);
    }
    else {
	_O(0xc1);
	_i_X((int)op, md, rb, ri, ms);
	_jit_B(_u8(im));
    }
}

__jit_inline void
_rotsh_c_rm(x86_rotsh_t op,
	    jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (rs != _RCX)
	JITFAIL("source register must be RCX");
    _O(0xd2);
    _i_X((int)op, md, rb, ri, ms);
}

__jit_inline void
_rotsh_sil_rm(x86_rotsh_t op,
	      jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    if (rs != _RCX)
	JITFAIL("source register must be RCX");
    _O(0xd3);
    _i_X((int)op, md, rb, ri, ms);
}

/* --- Bit test instructions ----------------------------------------------- */
typedef enum {
  X86_BT	= 4,
  X86_BTS	= 5,
  X86_BTR	= 6,
  X86_BTC	= 7,
} x86_bt_t;

__jit_inline void
_bt_sil_ir(x86_bt_t op,
	   long im, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xba);
    _Mrm(_b11, (int)op, _rA(rd));
    _jit_B(_u8(im));
}

__jit_inline void
_bt_sil_im(x86_bt_t op,
	   long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x0f);
    _O(0xba);
    _i_X((int)op, md, rb, ri, ms);
    _jit_B(_u8(im));
}

__jit_inline void
_bt_sil_rr(x86_bt_t op,
	   jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0x83 | ((int)op << 3));
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_bt_sil_rm(x86_bt_t op,
	   jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x0f);
    _O(0x83 | ((int)op << 3));
    _r_X(rs, md, rb, ri, ms);
}

/* --- Move instructions --------------------------------------------------- */
__jit_inline void
_mov_c_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x88);
    _Mrm(_b11, _r1(rs), _r1(rd));
}

__jit_inline void
_mov_c_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x8a);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_mov_c_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x88);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_mov_c_ir(long im, jit_gpr_t rd)
{
    _Or(0xb0, _r1(rd));
    _jit_B(_s8(im));
}

__jit_inline void
_mov_c_im(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _i_X(0, md, rb, ri, ms);
    _jit_B(_s8(im));
}

__jit_inline void
_mov_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x89);
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_mov_sil_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x8b);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_mov_sil_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x89);
    _r_X(rs, md, rb, ri, ms);
}

/* --- Unary and Multiply/Divide instructions ------------------------------ */
typedef enum {
    X86_NOT	= 2,
    X86_NEG	= 3,
    X86_MUL	= 4,
    X86_IMUL	= 5,
    X86_DIV	= 6,
    X86_IDIV	= 7,
} x86_unary_t;

__jit_inline void
_unary_c_r(x86_unary_t op,
	   jit_gpr_t rs)
{
    _O(0xf6);
    _Mrm(_b11, (int)op, _r1(rs));
}

__jit_inline void
_unary_c_m(x86_unary_t op,
	   long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xf6);
    _i_X((int)op, md, rb, ri, ms);
}

__jit_inline void
_unary_sil_r(x86_unary_t op,
	     jit_gpr_t rs)
{
    _O(0xf7);
    _Mrm(_b11, (int)op, _rA(rs));
}

__jit_inline void
_unary_sil_m(x86_unary_t op,
	     long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xf7);
    _i_X((int)op, md, rb, ri, ms);
}

__jit_inline void
_imul_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xaf);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

__jit_inline void
_imul_sil_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xaf);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_imul_s_irr(long im, jit_gpr_t rs, jit_gpr_t rd)
{
    if (_s8P(im)) {
	_O(0x6b);
	_Mrm(_b11, _rA(rd), _r4(rs));
	_jit_B(im);
    }
    else {
	_O(0x69);
	_Mrm(_b11, _rA(rd), _r4(rs));
	_jit_W(_s16(im));
    }
}

__jit_inline void
_imul_il_irr(long im, jit_gpr_t rs, jit_gpr_t rd)
{
    if (_s8P(im)) {
	_O(0x6b);
	_Mrm(_b11, _rA(rd), _r4(rs));
	_jit_B(im);
    }
    else {
	_O(0x69);
	_Mrm(_b11, _rA(rd), _r4(rs));
	_jit_I(_s32(im));
    }
}

/* --- Control Flow related instructions ----------------------------------- */
typedef enum {
    X86_CC_O	= 0x0,
    X86_CC_NO	= 0x1,
    X86_CC_NAE	= 0x2,
    X86_CC_B	= 0x2,
    X86_CC_C	= 0x2,
    X86_CC_AE	= 0x3,
    X86_CC_NB	= 0x3,
    X86_CC_NC	= 0x3,
    X86_CC_E	= 0x4,
    X86_CC_Z	= 0x4,
    X86_CC_NE	= 0x5,
    X86_CC_NZ	= 0x5,
    X86_CC_BE	= 0x6,
    X86_CC_NA	= 0x6,
    X86_CC_A	= 0x7,
    X86_CC_NBE	= 0x7,
    X86_CC_S	= 0x8,
    X86_CC_NS	= 0x9,
    X86_CC_P	= 0xa,
    X86_CC_PE	= 0xa,
    X86_CC_NP	= 0xb,
    X86_CC_PO	= 0xb,
    X86_CC_L	= 0xc,
    X86_CC_NGE	= 0xc,
    X86_CC_GE	= 0xd,
    X86_CC_NL	= 0xd,
    X86_CC_LE	= 0xe,
    X86_CC_NG	= 0xe,
    X86_CC_G	= 0xf,
    X86_CC_NLE	= 0xf,
} x86_cc_t;

__jit_inline void
_call_il_sr(jit_gpr_t rs)
{
    _O(0xff);
    _Mrm(_b11, _b010, _rA(rs));
}

__jit_inline void
_jmp_il_sr(jit_gpr_t rs)
{
    _O(0xff);
    _Mrm(_b11, _b100, _rA(rs));
}

__jit_inline void
_cmov_sil_rr(x86_cc_t cc,
	     jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0x40 | (int)cc);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

__jit_inline void
_cmov_sil_mr(x86_cc_t cc,
	     long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _O(0x0f);
    _O(0x40 | (int)cc);
    _r_X(rd, md, rb, ri, ms);
}

/* --- Push/Pop instructions ----------------------------------------------- */
__jit_inline void
_pop_sil_r(jit_gpr_t rd)
{
    _Or(0x58, _rA(rd));
}

__jit_inline void
_pop_sil_m(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x8f);
    _i_X(_b000, md, rb, ri, ms);
}

__jit_inline void
_push_sil_r(jit_gpr_t rs)
{
    _Or(0x50, _rA(rs));
}

__jit_inline void
_push_sil_m(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xff);
    _i_X(_b110, md, rb, ri, ms);
}

__jit_inline void
_push_c_i(long im)
{
    _O(0x6a);
    _jit_B(im);
}

__jit_inline void
_push_il_i(long im)
{
    if (_s8P(im))
	_push_c_i(im);
    else {
	_O(0x68);
	_jit_I(_s32(im));
    }
}

/* --- Test instructions --------------------------------------------------- */
__jit_inline void
_test_c_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x84);
    _Mrm(_b11, _r1(rs), _r1(rd));
}

__jit_inline void
_test_c_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x84);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_test_c_ir(long im, jit_gpr_t rd)
{
    if (rd == _RAX)
	_O(0xa8);
    else {
	_O(0xf6);
	_Mrm(_b11, _b000, _r1(rd));
    }
    _jit_B(_s8(im));
}

__jit_inline void
_test_c_im(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xf6);
    _i_X(_b000, md, rb, ri, ms);
    _jit_B(_s8(im));
}

__jit_inline void
_test_s_ir(long im, jit_gpr_t rd)
{
    if (rd == _RAX)
	_O(0xa9);
    else {
	_O(0xf7);
	_Mrm(_b11, _b000, _r2(rd));
    }
    _jit_W(_s16(im));
}

__jit_inline void
_test_s_im(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xf7);
    _i_X(_b000, md, rb, ri, ms);
    _jit_W(_s16(im));
}

__jit_inline void
_test_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x85);
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_test_sil_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x85);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_test_il_ir(long im, jit_gpr_t rd)
{
    if (rd == _RAX)
	_O(0xa9);
    else {
	_O(0xf7);
	_Mrm(_b11, _b000, _rA(rd));
    }
    _jit_I(_s32(im));
}

__jit_inline void
_test_il_im(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xf7);
    _i_X(_b000, md, rb, ri, ms);
    _jit_I(_s32(im));
}

/* --- Exchange instructions ----------------------------------------------- */
__jit_inline void
_cmpxchg_c_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xb0);
    _Mrm(_b11, _r1(rs), _r1(rd));
}

__jit_inline void
_cmpxchg_c_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x0f);
    _O(0xb0);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_cmpxchg_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xb1);
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_cmpxchg_sil_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x0f);
    _O(0xb1);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_xadd_c_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xc0);
    _Mrm(_b11, _r1(rs), _r1(rd));
}

__jit_inline void
_xadd_c_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x0f);
    _O(0xc0);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_xadd_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xc1);
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_xadd_sil_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x0f);
    _O(0xc1);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_xchg_c_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x86);
    _Mrm(_b11, _r1(rs), _r1(rd));
}

__jit_inline void
_xchg_c_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x86);
    _r_X(rs, md, rb, ri, ms);
}

__jit_inline void
_xchg_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x87);
    _Mrm(_b11, _rA(rs), _rA(rd));
}

__jit_inline void
_xchg_sil_rm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0x87);
    _r_X(rs, md, rb, ri, ms);
}

/* --- Increment/Decrement instructions ------------------------------------ */
__jit_inline void
_dec_c_r(jit_gpr_t rd)
{
    _O(0xfe);
    _Mrm(_b11, _b001, _r1(rd));
}

__jit_inline void
_inc_c_r(jit_gpr_t rd)
{
    _O(0xfe);
    _Mrm(_b11, _b000, _r1(rd));
}

__jit_inline void
_dec_c_m(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xfe);
    _i_X(_b001, md, rb, ri, ms);
}

__jit_inline void
_inc_c_m(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xfe);
    _i_X(_b000, md, rb, ri, ms);
}

__jit_inline void
_dec_sil_m(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xff);
    _i_X(_b001, md, rb, ri, ms);
}

__jit_inline void
_inc_sil_m(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _O(0xff);
    _i_X(_b000, md, rb, ri, ms);
}

/* --- Misc instructions --------------------------------------------------- */
__jit_inline void
_bsf_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbc);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

__jit_inline void
_bsf_sil_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbc);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_bsr_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbd);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

__jit_inline void
_bsr_sil_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbd);
    _r_X(rd, md, rb, ri, ms);
}

/* short|int|long rd = (char)rs */
__jit_inline void
_movsb_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbe);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

/* short|int|long rd = (char)*rs */
__jit_inline void
_movsb_sil_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbe);
    _r_X(rd, md, rb, ri, ms);
}

/* int|long rd = (short)rs */
__jit_inline void
_movsw_il_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbf);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

/* int|long rd = (short)*rs */
__jit_inline void
_movsw_il_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xbf);
    _r_X(rd, md, rb, ri, ms);
}

/* unsigned short|int|long rd = (unsigned char)rs */
__jit_inline void
_movzb_sil_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xb6);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

/* unsigned short|int|long rd = (unsigned char)*rs */
__jit_inline void
_movzb_sil_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xb6);
    _r_X(rd, md, rb, ri, ms);
}

/* unsigned int|long rd = (unsigned short)rs */
__jit_inline void
_movzw_il_rr(jit_gpr_t rs, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xb7);
    _Mrm(_b11, _rA(rd), _rA(rs));
}

/* unsigned int|long rd = (unsigned short)*rs */
__jit_inline void
_movzw_il_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x0f);
    _O(0xb7);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_lea_il_mr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _O(0x8d);
    _r_X(rd, md, rb, ri, ms);
}

__jit_inline void
_bswap_il_r(jit_gpr_t rd)
{
    _O(0x0f);
    _Or(0xc8, _rA(rd));
}

#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_X86_64 \
	: defined (__x86_64__)
#  include "asm-64.h"
#else
#  include "asm-32.h"
#endif

/* ========================================================================= */
/* --- Fully-qualified intrinsic instructions ------------------------------ */
/* ========================================================================= */

/*	OPCODE	+ i	= immediate operand
 *		+ r	= register operand
 *		+ m	= memory operand (disp,base,index,scale)
 *		+ sr/sm	= a star preceding a register or memory
 *		+ 0	= top of stack register (for FPU instructions)
 *
 *	NOTE in x86-64 mode: a memory operand with only a valid
 *	displacement value will lead to the expect absolute mode. If
 *	RIP addressing is necessary, X86_RIP shall be used as the base
 *	register argument.
 */

/* --- ALU instructions ---------------------------------------------------- */
__jit_inline void
_ALUBrr(x86_alu_t op,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _alu_c_rr(op, rs, rd);
}

__jit_inline void
_ALUBmr(x86_alu_t op,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXBmr(rb, ri, rd);
    _alu_c_mr(op, md, rb, ri, ms, rd);
}

__jit_inline void
_ALUBrm(x86_alu_t op,
	jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _alu_c_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_ALUBir(x86_alu_t op,
	long im, jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _alu_c_ir(op, im, rd);
}

__jit_inline void
_ALUBim(x86_alu_t op,
	long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _alu_c_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_ALUWrr(x86_alu_t op,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _alu_sil_rr(op, rs, rd);
}

__jit_inline void
_ALUWmr(x86_alu_t op,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _alu_sil_mr(op, md, rb, ri, ms, rd);
}

__jit_inline void
_ALUWrm(x86_alu_t op,
	jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _alu_sil_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_ALUWir(x86_alu_t op,
	long im, jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _alu_s_ir(op, im, rd);
}

__jit_inline void
_ALUWim(x86_alu_t op,
	long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _alu_s_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_ALULrr(x86_alu_t op,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _alu_sil_rr(op, rs, rd);
}

__jit_inline void
_ALULmr(x86_alu_t op,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _alu_sil_mr(op, md, rb, ri, ms, rd);
}

__jit_inline void
_ALULrm(x86_alu_t op,
	jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _alu_sil_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_ALULir(x86_alu_t op,
	long im, jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _alu_il_ir(op, im, rd);
}

__jit_inline void
_ALULim(x86_alu_t op,
	long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _alu_il_im(op, im, md, rb, ri, ms);
}

#define ADCBrr(RS, RD)			_ALUBrr(X86_ADC, RS, RD)
#define ADCBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCBir(IM, RD)			_ALUBir(X86_ADC, IM, RD)
#define ADCBim(IM, MD, MB, MI, MS)	_ALUBim(X86_ADC, IM, MD, MB, MI, MS)

#define ADCWrr(RS, RD)			_ALUWrr(X86_ADC, RS, RD)
#define ADCWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCWir(IM, RD)			_ALUWir(X86_ADC, IM, RD)
#define ADCWim(IM, MD, MB, MI, MS)	_ALUWim(X86_ADC, IM, MD, MB, MI, MS)

#define ADCLrr(RS, RD)			_ALULrr(X86_ADC, RS, RD)
#define ADCLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_ADC, MD, MB, MI, MS, RD)
#define ADCLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_ADC, RS, MD, MB, MI, MS)
#define ADCLir(IM, RD)			_ALULir(X86_ADC, IM, RD)
#define ADCLim(IM, MD, MB, MI, MS)	_ALULim(X86_ADC, IM, MD, MB, MI, MS)

#define ADDBrr(RS, RD)			_ALUBrr(X86_ADD, RS, RD)
#define ADDBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDBir(IM, RD)			_ALUBir(X86_ADD, IM, RD)
#define ADDBim(IM, MD, MB, MI, MS)	_ALUBim(X86_ADD, IM, MD, MB, MI, MS)

#define ADDWrr(RS, RD)			_ALUWrr(X86_ADD, RS, RD)
#define ADDWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDWir(IM, RD)			_ALUWir(X86_ADD, IM, RD)
#define ADDWim(IM, MD, MB, MI, MS)	_ALUWim(X86_ADD, IM, MD, MB, MI, MS)

#define ADDLrr(RS, RD)			_ALULrr(X86_ADD, RS, RD)
#define ADDLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_ADD, MD, MB, MI, MS, RD)
#define ADDLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_ADD, RS, MD, MB, MI, MS)
#define ADDLir(IM, RD)			_ALULir(X86_ADD, IM, RD)
#define ADDLim(IM, MD, MB, MI, MS)	_ALULim(X86_ADD, IM, MD, MB, MI, MS)

#define ANDBrr(RS, RD)			_ALUBrr(X86_AND, RS, RD)
#define ANDBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDBir(IM, RD)			_ALUBir(X86_AND, IM, RD)
#define ANDBim(IM, MD, MB, MI, MS)	_ALUBim(X86_AND, IM, MD, MB, MI, MS)

#define ANDWrr(RS, RD)			_ALUWrr(X86_AND, RS, RD)
#define ANDWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDWir(IM, RD)			_ALUWir(X86_AND, IM, RD)
#define ANDWim(IM, MD, MB, MI, MS)	_ALUWim(X86_AND, IM, MD, MB, MI, MS)

#define ANDLrr(RS, RD)			_ALULrr(X86_AND, RS, RD)
#define ANDLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_AND, MD, MB, MI, MS, RD)
#define ANDLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_AND, RS, MD, MB, MI, MS)
#define ANDLir(IM, RD)			_ALULir(X86_AND, IM, RD)
#define ANDLim(IM, MD, MB, MI, MS)	_ALULim(X86_AND, IM, MD, MB, MI, MS)

#define CMPBrr(RS, RD)			_ALUBrr(X86_CMP, RS, RD)
#define CMPBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPBir(IM, RD)			_ALUBir(X86_CMP, IM, RD)
#define CMPBim(IM, MD, MB, MI, MS)	_ALUBim(X86_CMP, IM, MD, MB, MI, MS)

#define CMPWrr(RS, RD)			_ALUWrr(X86_CMP, RS, RD)
#define CMPWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPWir(IM, RD)			_ALUWir(X86_CMP, IM, RD)
#define CMPWim(IM, MD, MB, MI, MS)	_ALUWim(X86_CMP, IM, MD, MB, MI, MS)

#define CMPLrr(RS, RD)			_ALULrr(X86_CMP, RS, RD)
#define CMPLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_CMP, MD, MB, MI, MS, RD)
#define CMPLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_CMP, RS, MD, MB, MI, MS)
#define CMPLir(IM, RD)			_ALULir(X86_CMP, IM, RD)
#define CMPLim(IM, MD, MB, MI, MS)	_ALULim(X86_CMP, IM, MD, MB, MI, MS)

#define ORBrr(RS, RD)			_ALUBrr(X86_OR, RS, RD)
#define ORBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_OR, MD, MB, MI, MS, RD)
#define ORBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_OR, RS, MD, MB, MI, MS)
#define ORBir(IM, RD)			_ALUBir(X86_OR, IM, RD)
#define ORBim(IM, MD, MB, MI, MS)	_ALUBim(X86_OR, IM, MD, MB, MI, MS)

#define ORWrr(RS, RD)			_ALUWrr(X86_OR, RS, RD)
#define ORWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_OR, MD, MB, MI, MS, RD)
#define ORWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_OR, RS, MD, MB, MI, MS)
#define ORWir(IM, RD)			_ALUWir(X86_OR, IM, RD)
#define ORWim(IM, MD, MB, MI, MS)	_ALUWim(X86_OR, IM, MD, MB, MI, MS)

#define ORLrr(RS, RD)			_ALULrr(X86_OR, RS, RD)
#define ORLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_OR, MD, MB, MI, MS, RD)
#define ORLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_OR, RS, MD, MB, MI, MS)
#define ORLir(IM, RD)			_ALULir(X86_OR, IM, RD)
#define ORLim(IM, MD, MB, MI, MS)	_ALULim(X86_OR, IM, MD, MB, MI, MS)

#define SBBBrr(RS, RD)			_ALUBrr(X86_SBB, RS, RD)
#define SBBBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBBir(IM, RD)			_ALUBir(X86_SBB, IM, RD)
#define SBBBim(IM, MD, MB, MI, MS)	_ALUBim(X86_SBB, IM, MD, MB, MI, MS)

#define SBBWrr(RS, RD)			_ALUWrr(X86_SBB, RS, RD)
#define SBBWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBWir(IM, RD)			_ALUWir(X86_SBB, IM, RD)
#define SBBWim(IM, MD, MB, MI, MS)	_ALUWim(X86_SBB, IM, MD, MB, MI, MS)

#define SBBLrr(RS, RD)			_ALULrr(X86_SBB, RS, RD)
#define SBBLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_SBB, MD, MB, MI, MS, RD)
#define SBBLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_SBB, RS, MD, MB, MI, MS)
#define SBBLir(IM, RD)			_ALULir(X86_SBB, IM, RD)
#define SBBLim(IM, MD, MB, MI, MS)	_ALULim(X86_SBB, IM, MD, MB, MI, MS)

#define SUBBrr(RS, RD)			_ALUBrr(X86_SUB, RS, RD)
#define SUBBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBBir(IM, RD)			_ALUBir(X86_SUB, IM, RD)
#define SUBBim(IM, MD, MB, MI, MS)	_ALUBim(X86_SUB, IM, MD, MB, MI, MS)

#define SUBWrr(RS, RD)			_ALUWrr(X86_SUB, RS, RD)
#define SUBWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBWir(IM, RD)			_ALUWir(X86_SUB, IM, RD)
#define SUBWim(IM, MD, MB, MI, MS)	_ALUWim(X86_SUB, IM, MD, MB, MI, MS)

#define SUBLrr(RS, RD)			_ALULrr(X86_SUB, RS, RD)
#define SUBLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_SUB, MD, MB, MI, MS, RD)
#define SUBLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_SUB, RS, MD, MB, MI, MS)
#define SUBLir(IM, RD)			_ALULir(X86_SUB, IM, RD)
#define SUBLim(IM, MD, MB, MI, MS)	_ALULim(X86_SUB, IM, MD, MB, MI, MS)

#define XORBrr(RS, RD)			_ALUBrr(X86_XOR, RS, RD)
#define XORBmr(MD, MB, MI, MS, RD)	_ALUBmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORBrm(RS, MD, MB, MI, MS)	_ALUBrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORBir(IM, RD)			_ALUBir(X86_XOR, IM, RD)
#define XORBim(IM, MD, MB, MI, MS)	_ALUBim(X86_XOR, IM, MD, MB, MI, MS)

#define XORWrr(RS, RD)			_ALUWrr(X86_XOR, RS, RD)
#define XORWmr(MD, MB, MI, MS, RD)	_ALUWmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORWrm(RS, MD, MB, MI, MS)	_ALUWrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORWir(IM, RD)			_ALUWir(X86_XOR, IM, RD)
#define XORWim(IM, MD, MB, MI, MS)	_ALUWim(X86_XOR, IM, MD, MB, MI, MS)

#define XORLrr(RS, RD)			_ALULrr(X86_XOR, RS, RD)
#define XORLmr(MD, MB, MI, MS, RD)	_ALULmr(X86_XOR, MD, MB, MI, MS, RD)
#define XORLrm(RS, MD, MB, MI, MS)	_ALULrm(X86_XOR, RS, MD, MB, MI, MS)
#define XORLir(IM, RD)			_ALULir(X86_XOR, IM, RD)
#define XORLim(IM, MD, MB, MI, MS)	_ALULim(X86_XOR, IM, MD, MB, MI, MS)

/* --- Shift/Rotate instructions ------------------------------------------- */
__jit_inline void
_ROTSHIBir(x86_rotsh_t op,
	   long im, jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _rotsh_c_ir(op, im, rd);
}

__jit_inline void
_ROTSHIBim(x86_rotsh_t op,
	   long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _rotsh_c_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_ROTSHIBrr(x86_rotsh_t op,
	   jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _rotsh_c_rr(op, rs, rd);
}

__jit_inline void
_ROTSHIBrm(x86_rotsh_t op,
	   jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _rotsh_c_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_ROTSHIWir(x86_rotsh_t op,
	   long im, jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _rotsh_sil_ir(op, im, rd);
}

__jit_inline void
_ROTSHIWim(x86_rotsh_t op,
	   long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _rotsh_sil_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_ROTSHIWrr(x86_rotsh_t op,
	   jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _rotsh_sil_rr(op, rs, rd);
}

__jit_inline void
_ROTSHIWrm(x86_rotsh_t op,
	   jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _rotsh_sil_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_ROTSHILir(x86_rotsh_t op,
	   long im, jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _rotsh_sil_ir(op, im, rd);
}

__jit_inline void
_ROTSHILim(x86_rotsh_t op,
	   long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _rotsh_sil_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_ROTSHILrr(x86_rotsh_t op,
	   jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _rotsh_sil_rr(op, rs, rd);
}

__jit_inline void
_ROTSHILrm(x86_rotsh_t op,
	   jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _rotsh_sil_rm(op, rs, md, rb, ri, ms);
}

#define ROLBir(IM, RD)			_ROTSHIBir(X86_ROL, IM, RD)
#define ROLBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLBrr(RS, RD)			_ROTSHIBrr(X86_ROL, RS, RD)
#define ROLBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_ROL, RS, MD, MB, MI, MS)

#define ROLWir(IM, RD)			_ROTSHIWir(X86_ROL, IM, RD)
#define ROLWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLWrr(RS, RD)			_ROTSHIWrr(X86_ROL, RS, RD)
#define ROLWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_ROL, RS, MD, MB, MI, MS)

#define ROLLir(IM, RD)			_ROTSHILir(X86_ROL, IM, RD)
#define ROLLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_ROL, IM, MD, MB, MI, MS)
#define ROLLrr(RS, RD)			_ROTSHILrr(X86_ROL, RS, RD)
#define ROLLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_ROL, RS, MD, MB, MI, MS)

#define RORBir(IM, RD)			_ROTSHIBir(X86_ROR, IM, RD)
#define RORBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_ROR, IM, MD, MB, MI, MS)
#define RORBrr(RS, RD)			_ROTSHIBrr(X86_ROR, RS, RD)
#define RORBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_ROR, RS, MD, MB, MI, MS)

#define RORWir(IM, RD)			_ROTSHIWir(X86_ROR, IM, RD)
#define RORWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_ROR, IM, MD, MB, MI, MS)
#define RORWrr(RS, RD)			_ROTSHIWrr(X86_ROR, RS, RD)
#define RORWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_ROR, RS, MD, MB, MI, MS)

#define RORLir(IM, RD)			_ROTSHILir(X86_ROR, IM, RD)
#define RORLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_ROR, IM, MD, MB, MI, MS)
#define RORLrr(RS, RD)			_ROTSHILrr(X86_ROR, RS, RD)
#define RORLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_ROR, RS, MD, MB, MI, MS)

#define RCLBir(IM, RD)			_ROTSHIBir(X86_RCL, IM, RD)
#define RCLBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLBrr(RS, RD)			_ROTSHIBrr(X86_RCL, RS, RD)
#define RCLBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCLWir(IM, RD)			_ROTSHIWir(X86_RCL, IM, RD)
#define RCLWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLWrr(RS, RD)			_ROTSHIWrr(X86_RCL, RS, RD)
#define RCLWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCLLir(IM, RD)			_ROTSHILir(X86_RCL, IM, RD)
#define RCLLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_RCL, IM, MD, MB, MI, MS)
#define RCLLrr(RS, RD)			_ROTSHILrr(X86_RCL, RS, RD)
#define RCLLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_RCL, RS, MD, MB, MI, MS)

#define RCRBir(IM, RD)			_ROTSHIBir(X86_RCR, IM, RD)
#define RCRBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRBrr(RS, RD)			_ROTSHIBrr(X86_RCR, RS, RD)
#define RCRBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_RCR, RS, MD, MB, MI, MS)

#define RCRWir(IM, RD)			_ROTSHIWir(X86_RCR, IM, RD)
#define RCRWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRWrr(RS, RD)			_ROTSHIWrr(X86_RCR, RS, RD)
#define RCRWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_RCR, RS, MD, MB, MI, MS)

#define RCRLir(IM, RD)			_ROTSHILir(X86_RCR, IM, RD)
#define RCRLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_RCR, IM, MD, MB, MI, MS)
#define RCRLrr(RS, RD)			_ROTSHILrr(X86_RCR, RS, RD)
#define RCRLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_RCR, RS, MD, MB, MI, MS)

#define SHLBir(IM, RD)			_ROTSHIBir(X86_SHL, IM, RD)
#define SHLBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLBrr(RS, RD)			_ROTSHIBrr(X86_SHL, RS, RD)
#define SHLBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHLWir(IM, RD)			_ROTSHIWir(X86_SHL, IM, RD)
#define SHLWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLWrr(RS, RD)			_ROTSHIWrr(X86_SHL, RS, RD)
#define SHLWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHLLir(IM, RD)			_ROTSHILir(X86_SHL, IM, RD)
#define SHLLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_SHL, IM, MD, MB, MI, MS)
#define SHLLrr(RS, RD)			_ROTSHILrr(X86_SHL, RS, RD)
#define SHLLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_SHL, RS, MD, MB, MI, MS)

#define SHRBir(IM, RD)			_ROTSHIBir(X86_SHR, IM, RD)
#define SHRBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRBrr(RS, RD)			_ROTSHIBrr(X86_SHR, RS, RD)
#define SHRBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_SHR, RS, MD, MB, MI, MS)

#define SHRWir(IM, RD)			_ROTSHIWir(X86_SHR, IM, RD)
#define SHRWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRWrr(RS, RD)			_ROTSHIWrr(X86_SHR, RS, RD)
#define SHRWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_SHR, RS, MD, MB, MI, MS)

#define SHRLir(IM, RD)			_ROTSHILir(X86_SHR, IM, RD)
#define SHRLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_SHR, IM, MD, MB, MI, MS)
#define SHRLrr(RS, RD)			_ROTSHILrr(X86_SHR, RS, RD)
#define SHRLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_SHR, RS, MD, MB, MI, MS)

#define SALBir				SHLBir
#define SALBim				SHLBim
#define SALBrr				SHLBrr
#define SALBrm				SHLBrm

#define SALWir				SHLWir
#define SALWim				SHLWim
#define SALWrr				SHLWrr
#define SALWrm				SHLWrm

#define SALLir				SHLLir
#define SALLim				SHLLim
#define SALLrr				SHLLrr
#define SALLrm				SHLLrm

#define SARBir(IM, RD)			_ROTSHIBir(X86_SAR, IM, RD)
#define SARBim(IM, MD, MB, MI, MS)	_ROTSHIBim(X86_SAR, IM, MD, MB, MI, MS)
#define SARBrr(RS, RD)			_ROTSHIBrr(X86_SAR, RS, RD)
#define SARBrm(RS, MD, MB, MI, MS)	_ROTSHIBrm(X86_SAR, RS, MD, MB, MI, MS)

#define SARWir(IM, RD)			_ROTSHIWir(X86_SAR, IM, RD)
#define SARWim(IM, MD, MB, MI, MS)	_ROTSHIWim(X86_SAR, IM, MD, MB, MI, MS)
#define SARWrr(RS, RD)			_ROTSHIWrr(X86_SAR, RS, RD)
#define SARWrm(RS, MD, MB, MI, MS)	_ROTSHIWrm(X86_SAR, RS, MD, MB, MI, MS)

#define SARLir(IM, RD)			_ROTSHILir(X86_SAR, IM, RD)
#define SARLim(IM, MD, MB, MI, MS)	_ROTSHILim(X86_SAR, IM, MD, MB, MI, MS)
#define SARLrr(RS, RD)			_ROTSHILrr(X86_SAR, RS, RD)
#define SARLrm(RS, MD, MB, MI, MS)	_ROTSHILrm(X86_SAR, RS, MD, MB, MI, MS)

/* --- Bit test instructions ----------------------------------------------- */
__jit_inline void
_BTWir(x86_bt_t op,
       long im, jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _bt_sil_ir(op, im, rd);
}

__jit_inline void
_BTWim(x86_bt_t op,
       long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _bt_sil_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_BTWrr(x86_bt_t op,
       jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _bt_sil_rr(op, rs, rd);
}

__jit_inline void
_BTWrm(x86_bt_t op,
       jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _bt_sil_rm(op, rs, md, rb, ri, ms);
}

__jit_inline void
_BTLir(x86_bt_t op,
       long im, jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _bt_sil_ir(op, im, rd);
}

__jit_inline void
_BTLim(x86_bt_t op,
       long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _bt_sil_im(op, im, md, rb, ri, ms);
}

__jit_inline void
_BTLrr(x86_bt_t op,
       jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _bt_sil_rr(op, rs, rd);
}

__jit_inline void
_BTLrm(x86_bt_t op,
       jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _bt_sil_rm(op, rs, md, rb, ri, ms);
}

#define BTWir(IM, RD)			_BTWir(X86_BT, IM, RD)
#define BTWim(IM, MD, MB, MI, MS)	_BTWim(X86_BT, IM, MD, MI, MS)
#define BTWrr(RS, RD)			_BTWrr(X86_BT, RS, RD)
#define BTWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BT, RS, MD, MB, MI, MS)

#define BTLir(IM, RD)			_BTLir(X86_BT, IM, RD)
#define BTLim(IM, MD, MB, MI, MS)	_BTLim(X86_BT, IM, MD, MB, MI, MS)
#define BTLrr(RS, RD)			_BTLrr(X86_BT, RS, RD)
#define BTLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BT, RS, MD, MB, MI, MS)

#define BTCWir(IM, RD)			_BTWir(X86_BTC, IM, RD)
#define BTCWim(IM, MD, MB, MI, MS)	_BTWim(X86_BTC, IM, MD, MI, MS)
#define BTCWrr(RS, RD)			_BTWrr(X86_BTC, RS, RD)
#define BTCWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BTC, RS, MD, MB, MI, MS)

#define BTCLir(IM, RD)			_BTLir(X86_BTC, IM, RD)
#define BTCLim(IM, MD, MB, MI, MS)	_BTLim(X86_BTC, IM, MD, MB, MI, MS)
#define BTCLrr(RS, RD)			_BTLrr(X86_BTC, RS, RD)
#define BTCLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BTC, RS, MD, MB, MI, MS)

#define BTRWir(IM, RD)			_BTWir(X86_BTR, IM, RD)
#define BTRWim(IM, MD, MB, MI, MS)	_BTWim(X86_BTR, IM, MD, MI, MS)
#define BTRWrr(RS, RD)			_BTWrr(X86_BTR, RS, RD)
#define BTRWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BTR, RS, MD, MB, MI, MS)

#define BTRLir(IM, RD)			_BTLir(X86_BTR, IM, RD)
#define BTRLim(IM, MD, MB, MI, MS)	_BTLim(X86_BTR, IM, MD, MB, MI, MS)
#define BTRLrr(RS, RD)			_BTLrr(X86_BTR, RS, RD)
#define BTRLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BTR, RS, MD, MB, MI, MS)

#define BTSWir(IM, RD)			_BTWir(X86_BTS, IM, RD)
#define BTSWim(IM, MD, MB, MI, MS)	_BTWim(X86_BTS, IM, MD, MI, MS)
#define BTSWrr(RS, RD)			_BTWrr(X86_BTS, RS, RD)
#define BTSWrm(RS, MD, MB, MI, MS)	_BTWrm(X86_BTS, RS, MD, MB, MI, MS)

#define BTSLir(IM, RD)			_BTLir(X86_BTS, IM, RD)
#define BTSLim(IM, MD, MB, MI, MS)	_BTLim(X86_BTS, IM, MD, MB, MI, MS)
#define BTSLrr(RS, RD)			_BTLrr(X86_BTS, RS, RD)
#define BTSLrm(RS, MD, MB, MI, MS)	_BTLrm(X86_BTS, RS, MD, MB, MI, MS)

/* --- Move instructions --------------------------------------------------- */
__jit_inline void
MOVBrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _mov_c_rr(rs, rd);
}

__jit_inline void
MOVBmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXBmr(rb, ri, rd);
    _mov_c_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVBrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _mov_c_rm(rs, md, rb, ri, ms);
}

__jit_inline void
MOVBir(long im, jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _mov_c_ir(im, rd);
}

__jit_inline void
MOVBim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _mov_c_im(im, md, rb, ri, ms);
}

__jit_inline void
MOVWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _mov_sil_rr(rs, rd);
}

__jit_inline void
MOVWmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _mov_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVWrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _mov_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
MOVWir(long im, jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _Or(0xb8, _r2(rd));
    _jit_W(_s16(im));
}

__jit_inline void
MOVWim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _O(0xc7);
    _i_X(_b000, md, rb, ri, ms);
    _jit_W(_s16(im));
}

__jit_inline void
MOVLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _mov_sil_rr(rs, rd);
}

__jit_inline void
MOVLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _mov_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVLrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _mov_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
MOVLir(long im, jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _Or(0xb8, _r4(rd));
    _jit_I(_u32(im));
}

__jit_inline void
MOVLim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _O(0xc7);
    _i_X(0, md, rb, ri, ms);
    _jit_I(_s32(im));
}

/* --- Unary and Multiply/Divide instructions ------------------------------ */
__jit_inline void
_UNARYBr(x86_unary_t op,
	 jit_gpr_t rs)
{
    _REXBrr(_NOREG, rs);
    _unary_c_r(op, rs);
}

__jit_inline void
_UNARYBm(x86_unary_t op,
	 long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _unary_c_m(op, md, rb, ri, ms);
}

__jit_inline void
_UNARYWr(x86_unary_t op,
	 jit_gpr_t rs)
{
    _d16();
    _REXLrr(_NOREG, rs);
    _unary_sil_r(op, rs);
}

__jit_inline void
_UNARYWm(x86_unary_t op,
	 long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLmr(rb, ri, _NOREG);
    _unary_sil_m(op, md, rb, ri, ms);
}

__jit_inline void
_UNARYLr(x86_unary_t op,
	 jit_gpr_t rs)
{
    _REXLrr(_NOREG, rs);
    _unary_sil_r(op, rs);
}

__jit_inline void
_UNARYLm(x86_unary_t op,
	 long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLmr(rb, ri, _NOREG);
    _unary_sil_m(op, md, rb, ri, ms);
}

#define NOTBr(RS)			_UNARYBr(X86_NOT, RS)
#define NOTBm(MD, MB, MI, MS)		_UNARYBm(X86_NOT, MD, MB, MI, MS)
#define NOTWr(RS)			_UNARYWr(X86_NOT, RS)
#define NOTWm(MD, MB, MI, MS)		_UNARYWm(X86_NOT, MD, MB, MI, MS)
#define NOTLr(RS)			_UNARYLr(X86_NOT, RS)
#define NOTLm(MD, MB, MI, MS)		_UNARYLm(X86_NOT, MD, MB, MI, MS)

#define NEGBr(RS)			_UNARYBr(X86_NEG, RS)
#define NEGBm(MD, MB, MI, MS)		_UNARYBm(X86_NEG, MD, MB, MI, MS)
#define NEGWr(RS)			_UNARYWr(X86_NEG, RS)
#define NEGWm(MD, MB, MI, MS)		_UNARYWm(X86_NEG, MD, MB, MI, MS)
#define NEGLr(RS)			_UNARYLr(X86_NEG, RS)
#define NEGLm(MD, MB, MI, MS)		_UNARYLm(X86_NEG, MD, MB, MI, MS)

#define MULBr(RS)			_UNARYBr(X86_MUL, RS)
#define MULBm(MD, MB, MI, MS)		_UNARYBm(X86_MUL, MD, MB, MI, MS)
#define MULWr(RS)			_UNARYWr(X86_MUL, RS)
#define MULWm(MD, MB, MI, MS)		_UNARYWm(X86_MUL, MD, MB, MI, MS)
#define MULLr(RS)			_UNARYLr(X86_MUL, RS)
#define MULLm(MD, MB, MI, MS)		_UNARYLm(X86_MUL, MD, MB, MI, MS)

#define IMULBr(RS)			_UNARYBr(X86_IMUL, RS)
#define IMULBm(MD, MB, MI, MS)		_UNARYBm(X86_IMUL, MD, MB, MI, MS)
#define IMULWr(RS)			_UNARYWr(X86_IMUL, RS)
#define IMULWm(MD, MB, MI, MS)		_UNARYWm(X86_IMUL, MD, MB, MI, MS)
#define IMULLr(RS)			_UNARYLr(X86_IMUL, RS)
#define IMULLm(MD, MB, MI, MS)		_UNARYLm(X86_IMUL, MD, MB, MI, MS)

#define DIVBr(RS)			_UNARYBr(X86_DIV, RS)
#define DIVBm(MD, MB, MI, MS)		_UNARYBm(X86_DIV, MD, MB, MI, MS)
#define DIVWr(RS)			_UNARYWr(X86_DIV, RS)
#define DIVWm(MD, MB, MI, MS)		_UNARYWm(X86_DIV, MD, MB, MI, MS)
#define DIVLr(RS)			_UNARYLr(X86_DIV, RS)
#define DIVLm(MD, MB, MI, MS)		_UNARYLm(X86_DIV, MD, MB, MI, MS)

#define IDIVBr(RS)			_UNARYBr(X86_IDIV, RS)
#define IDIVBm(MD, MB, MI, MS)		_UNARYBm(X86_IDIV, MD, MB, MI, MS)
#define IDIVWr(RS)			_UNARYWr(X86_IDIV, RS)
#define IDIVWm(MD, MB, MI, MS)		_UNARYWm(X86_IDIV, MD, MB, MI, MS)
#define IDIVLr(RS)			_UNARYLr(X86_IDIV, RS)
#define IDIVLm(MD, MB, MI, MS)		_UNARYLm(X86_IDIV, MD, MB, MI, MS)

__jit_inline void
IMULWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rd, rs);
    _imul_sil_rr(rs, rd);
}

__jit_inline void
IMULWmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _imul_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
IMULWirr(long im, jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rd, rs);
    _imul_il_irr(im, rs, rd);
}

__jit_inline void
IMULLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rd, rs);
    _imul_sil_rr(rs, rd);
}

__jit_inline void
IMULLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _imul_sil_mr(md, rb, ri, ms, rd);
}

#define IMULLir(im, rd)			IMULLirr(im, rd, rd)
__jit_inline void
IMULLirr(long im, jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rd, rs);
    _imul_il_irr(im, rs, rd);
}

/* --- Control Flow related instructions ----------------------------------- */
/* 32 bit displacement from %rip */
__jit_inline void
CALLm(void *im)
{
    _O(0xe8);
    _D32(im);
}

/* call absolute value */
__jit_inline void
CALLLsr(jit_gpr_t rs)
{
    _REXLrr(_NOREG, rs);
    _call_il_sr(rs);
}

__jit_inline void
CALLsm(jit_gpr_t rs, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _O(0xff);
    _i_X(_b010, rs, rb, ri, ms);
}

__jit_inline void
JMPSm(void *im)
{
    _O(0xeb);
    _D8(im);
}

__jit_inline void
JMPm(void *im)
{
    _O(0xe9);
    _D32(im);
}

__jit_inline void
JMPLsr(jit_gpr_t rs)
{
    _REXLrr(_NOREG, rs);
    _jmp_il_sr(rs);
}

__jit_inline void
JMPsm(jit_gpr_t rs, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _O(0xff);
    _i_X(_b100, rs, rb, ri, ms);
}

__jit_inline void
JCCSim(x86_cc_t cc,
       void *im)
{
    _O(0x70 | (int)cc);
    _D8(im);
}

#define JOSm(D)				JCCSim(X86_CC_O,   D)
#define JNOSm(D)			JCCSim(X86_CC_NO,  D)
#define JNAESm(D)			JCCSim(X86_CC_NAE, D)
#define JBSm(D)				JCCSim(X86_CC_B,   D)
#define JCSm(D)				JCCSim(X86_CC_C,   D)
#define JAESm(D)			JCCSim(X86_CC_AE,  D)
#define JNBSm(D)			JCCSim(X86_CC_NB,  D)
#define JNCSm(D)			JCCSim(X86_CC_NC,  D)
#define JESm(D)				JCCSim(X86_CC_E,   D)
#define JZSm(D)				JCCSim(X86_CC_Z,   D)
#define JNESm(D)			JCCSim(X86_CC_NE,  D)
#define JNZSm(D)			JCCSim(X86_CC_NZ,  D)
#define JBESm(D)			JCCSim(X86_CC_BE,  D)
#define JNASm(D)			JCCSim(X86_CC_NA,  D)
#define JASm(D)				JCCSim(X86_CC_A,   D)
#define JNBESm(D)			JCCSim(X86_CC_NBE, D)
#define JSSm(D)				JCCSim(X86_CC_S,   D)
#define JNSSm(D)			JCCSim(X86_CC_NS,  D)
#define JPSm(D)				JCCSim(X86_CC_P,   D)
#define JPESm(D)			JCCSim(X86_CC_PE,  D)
#define JNPSm(D)			JCCSim(X86_CC_NP,  D)
#define JPOSm(D)			JCCSim(X86_CC_PO,  D)
#define JLSm(D)				JCCSim(X86_CC_L,   D)
#define JNGESm(D)			JCCSim(X86_CC_NGE, D)
#define JNLSm(D)			JCCSim(X86_CC_GE,  D)
#define JGESm(D)			JCCSim(X86_CC_NL,  D)
#define JLESm(D)			JCCSim(X86_CC_LE,  D)
#define JNGSm(D)			JCCSim(X86_CC_NG,  D)
#define JGSm(D)				JCCSim(X86_CC_G,   D)
#define JNLESm(D)			JCCSim(X86_CC_NLE, D)

__jit_inline void
JCCim(x86_cc_t cc,
      void *im)
{
    _O(0x0f);
    _O(0x80 | (int)cc);
    _D32(im);
}

#define JOm(D)				JCCim(X86_CC_O,   D)
#define JNOm(D)				JCCim(X86_CC_NO,  D)
#define JNAEm(D)			JCCim(X86_CC_NAE, D)
#define JBm(D)				JCCim(X86_CC_B,   D)
#define JCm(D)				JCCim(X86_CC_C,   D)
#define JAEm(D)				JCCim(X86_CC_AE,  D)
#define JNBm(D)				JCCim(X86_CC_NB,  D)
#define JNCm(D)				JCCim(X86_CC_NC,  D)
#define JEm(D)				JCCim(X86_CC_E,   D)
#define JZm(D)				JCCim(X86_CC_Z,   D)
#define JNEm(D)				JCCim(X86_CC_NE,  D)
#define JNZm(D)				JCCim(X86_CC_NZ,  D)
#define JBEm(D)				JCCim(X86_CC_BE,  D)
#define JNAm(D)				JCCim(X86_CC_NA,  D)
#define JAm(D)				JCCim(X86_CC_A,   D)
#define JNBEm(D)			JCCim(X86_CC_NBE, D)
#define JSm(D)				JCCim(X86_CC_S,   D)
#define JNSm(D)				JCCim(X86_CC_NS,  D)
#define JPm(D)				JCCim(X86_CC_P,   D)
#define JPEm(D)				JCCim(X86_CC_PE,  D)
#define JNPm(D)				JCCim(X86_CC_NP,  D)
#define JPOm(D)				JCCim(X86_CC_PO,  D)
#define JLm(D)				JCCim(X86_CC_L,   D)
#define JNGEm(D)			JCCim(X86_CC_NGE, D)
#define JGEm(D)				JCCim(X86_CC_GE,  D)
#define JNLm(D)				JCCim(X86_CC_NL,  D)
#define JLEm(D)				JCCim(X86_CC_LE,  D)
#define JNGm(D)				JCCim(X86_CC_NG,  D)
#define JGm(D)				JCCim(X86_CC_G,   D)
#define JNLEm(D)			JCCim(X86_CC_NLE, D)

__jit_inline void
SETCCir(x86_cc_t cc,
	jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _O(0x0f);
    _O(0x90 | (int)cc);
    _Mrm(_b11, _b000, _r1(rd));
}

#define SETOr(RD)			SETCCir(X86_CC_O,   RD)
#define SETNOr(RD)			SETCCir(X86_CC_NO,  RD)
#define SETBr(RD)			SETCCir(X86_CC_B,   RD)
#define SETCr(RD)			SETCCir(X86_CC_C,   RD)
#define SETNAEr(RD)			SETCCir(X86_CC_NAE, RD)
#define SETNBr(RD)			SETCCir(X86_CC_NB,  RD)
#define SETAEr(RD)			SETCCir(X86_CC_AE,  RD)
#define SETEr(RD)			SETCCir(X86_CC_E,   RD)
#define SETZr(RD)			SETCCir(X86_CC_Z,   RD)
#define SETNEr(RD)			SETCCir(X86_CC_NE,  RD)
#define SETNZr(RD)			SETCCir(X86_CC_NZ,  RD)
#define SETBEr(RD)			SETCCir(X86_CC_BE,  RD)
#define SETNAr(RD)			SETCCir(X86_CC_NA,  RD)
#define SETNBEr(RD)			SETCCir(X86_CC_NBE, RD)
#define SETAr(RD)			SETCCir(X86_CC_A,   RD)
#define SETSr(RD)			SETCCir(X86_CC_S,   RD)
#define SETNSr(RD)			SETCCir(X86_CC_NS,  RD)
#define SETPr(RD)			SETCCir(X86_CC_P,   RD)
#define SETPEr(RD)			SETCCir(X86_CC_PE,  RD)
#define SETNPr(RD)			SETCCir(X86_CC_NP,  RD)
#define SETPOr(RD)			SETCCir(X86_CC_NO,  RD)
#define SETLr(RD)			SETCCir(X86_CC_L,   RD)
#define SETNGEr(RD)			SETCCir(X86_CC_NGE, RD)
#define SETGEr(RD)			SETCCir(X86_CC_GE,  RD)
#define SETNLr(RD)			SETCCir(X86_CC_NL,  RD)
#define SETLEr(RD)			SETCCir(X86_CC_LE,  RD)
#define SETNGr(RD)			SETCCir(X86_CC_NG,  RD)
#define SETGr(RD)			SETCCir(X86_CC_G,   RD)
#define SETNLEr(RD)			SETCCir(X86_CC_NLE, RD)

__jit_inline void
SETCCim(x86_cc_t cc,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _O(0x0f);
    _O(0x90 | (int)cc);
    _i_X(_b000, md, rb, ri, ms);
}

#define SETOm(D, B, I, S)		SETCCim(X86_CC_O,   D, B, I, S)
#define SETNOm(D, B, I, S)		SETCCim(X86_CC_NO,  D, B, I, S)
#define SETNAEm(D, B, I, S)		SETCCim(X86_CC_NAE, D, B, I, S)
#define SETBm(D, B, I, S)		SETCCim(X86_CC_B,   D, B, I, S)
#define SETCm(D, B, I, S)		SETCCim(X86_CC_C,   D, B, I, S)
#define SETAEm(D, B, I, S)		SETCCim(X86_CC_AE,  D, B, I, S)
#define SETNBm(D, B, I, S)		SETCCim(X86_CC_NB,  D, B, I, S)
#define SETEm(D, B, I, S)		SETCCim(X86_CC_E,   D, B, I, S)
#define SETZm(D, B, I, S)		SETCCim(X86_CC_Z,   D, B, I, S)
#define SETNEm(D, B, I, S)		SETCCim(X86_CC_NE,  D, B, I, S)
#define SETNZm(D, B, I, S)		SETCCim(X86_CC_NZ,  D, B, I, S)
#define SETBEm(D, B, I, S)		SETCCim(X86_CC_BE,  D, B, I, S)
#define SETNAm(D, B, I, S)		SETCCim(X86_CC_NA,  D, B, I, S)
#define SETAm(D, B, I, S)		SETCCim(X86_CC_A,   D, B, I, S)
#define SETNBEm(D, B, I, S)		SETCCim(X86_CC_NBE, D, B, I, S)
#define SETSm(D, B, I, S)		SETCCim(X86_CC_S,   D, B, I, S)
#define SETNSm(D, B, I, S)		SETCCim(X86_CC_NS,  D, B, I, S)
#define SETPm(D, B, I, S)		SETCCim(X86_CC_P,   D, B, I, S)
#define SETPEm(D, B, I, S)		SETCCim(X86_CC_PE,  D, B, I, S)
#define SETNPm(D, B, I, S)		SETCCim(X86_CC_NP,  D, B, I, S)
#define SETPOm(D, B, I, S)		SETCCim(X86_CC_PO,  D, B, I, S)
#define SETLm(D, B, I, S)		SETCCim(X86_CC_L,   D, B, I, S)
#define SETNGEm(D, B, I, S)		SETCCim(X86_CC_NGE, D, B, I, S)
#define SETGEm(D, B, I, S)		SETCCim(X86_CC_GE,  D, B, I, S)
#define SETNLm(D, B, I, S)		SETCCim(X86_CC_NL,  D, B, I, S)
#define SETLEm(D, B, I, S)		SETCCim(X86_CC_LE,  D, B, I, S)
#define SETNGm(D, B, I, S)		SETCCim(X86_CC_NG,  D, B, I, S)
#define SETGm(D, B, I, S)		SETCCim(X86_CC_G,   D, B, I, S)
#define SETNLEm(D, B, I, S)		SETCCim(X86_CC_NLE, D, B, I, S)

__jit_inline void
CMOVWrr(x86_cc_t cc,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rd, rs);
    _cmov_sil_rr(cc, rs, rd);
}

__jit_inline void
CMOVWmr(x86_cc_t cc,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _cmov_sil_mr(cc, md, rb, ri, ms, rd);
}

__jit_inline void
CMOVLrr(x86_cc_t cc,
	jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rd, rs);
    _cmov_sil_rr(cc, rs, rd);
}

__jit_inline void
CMOVLmr(x86_cc_t cc,
	long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _cmov_sil_mr(cc, md, rb, ri, ms, rd);
}

/* --- Push/Pop instructions ----------------------------------------------- */
#define POPAD_					POPA_
__jit_inline void
POPA_(void)
{
    _d16();
    _O(0x61);
}

#define PUSHAD_					PUSHA_
__jit_inline void
PUSHA_(void)
{
    _d16();
    _O(0x60);
}
    
__jit_inline void
POPF_(void)
{
    _O(0x9d);
}

__jit_inline void
PUSHF_(void)
{
    _O(0x9c);
}

/* --- Test instructions --------------------------------------------------- */
__jit_inline void
TESTBrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _test_c_rr(rs, rd);
}

__jit_inline void
TESTBrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _test_c_rm(rs, md, rb, ri, ms);
}

__jit_inline void
TESTBir(long im, jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _test_c_ir(im, rd);
}

__jit_inline void
TESTBim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _test_c_im(im, md, rb, ri, ms);
}

__jit_inline void
TESTWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _test_sil_rr(rs, rd);
}

__jit_inline void
TESTWrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _test_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
TESTWir(long im, jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _test_s_ir(im, rd);
}

__jit_inline void
TESTWim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _test_s_im(im, md, rb, ri, ms);
}

__jit_inline void
TESTLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _test_sil_rr(rs, rd);
}

__jit_inline void
TESTLrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _test_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
TESTLir(long im, jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _test_il_ir(im, rd);
}

__jit_inline void
TESTLim(long im, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _test_il_im(im, md, rb, ri, ms);
}

/* --- Exchange instructions ----------------------------------------------- */
__jit_inline void
CMPXCHGBrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _cmpxchg_c_rr(rs, rd);
}

__jit_inline void
CMPXCHGBrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _cmpxchg_c_rm(rs, md, rb, ri, ms);
}

__jit_inline void
CMPXCHGWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _cmpxchg_sil_rr(rs, rd);
}

__jit_inline void
CMPXCHGWrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _cmpxchg_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
CMPXCHGLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _cmpxchg_sil_rr(rs, rd);
}

__jit_inline void
CMPXCHGLrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _cmpxchg_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XADDBrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _xadd_c_rr(rs, rd);
}

__jit_inline void
XADDBrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _xadd_c_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XADDWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _xadd_sil_rr(rs, rd);
}

__jit_inline void
XADDWrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _xadd_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XADDLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _xadd_sil_rr(rs, rd);
}

__jit_inline void
XADDLrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _xadd_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XCHGBrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBrr(rs, rd);
    _xchg_c_rr(rs, rd);
}

__jit_inline void
XCHGBrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(rs, rb, ri);
    _xchg_c_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XCHGWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rs, rd);
    _xchg_sil_rr(rs, rd);
}

__jit_inline void
XCHGWrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(rs, rb, ri);
    _xchg_sil_rm(rs, md, rb, ri, ms);
}

__jit_inline void
XCHGLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rs, rd);
    _xchg_sil_rr(rs, rd);
}

__jit_inline void
XCHGLrm(jit_gpr_t rs, long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(rs, rb, ri);
    _xchg_sil_rm(rs, md, rb, ri, ms);
}

/* --- Increment/Decrement instructions ------------------------------------ */
__jit_inline void
DECBr(jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _dec_c_r(rd);
}

__jit_inline void
DECBm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _dec_c_m(md, rb, ri, ms);
}

__jit_inline void
DECWm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _dec_sil_m(md, rb, ri, ms);
}

__jit_inline void
DECLm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _dec_sil_m(md, rb, ri, ms);
}

__jit_inline void
DECWr(jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _dec_sil_r(rd);
}

__jit_inline void
DECLr(jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _dec_sil_r(rd);
}

__jit_inline void
INCWr(jit_gpr_t rd)
{
    _d16();
    _REXLrr(_NOREG, rd);
    _inc_sil_r(rd);
}

__jit_inline void
INCLr(jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _inc_sil_r(rd);
}

__jit_inline void
INCBr(jit_gpr_t rd)
{
    _REXBrr(_NOREG, rd);
    _inc_c_r(rd);
}

__jit_inline void
INCBm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXBrm(_NOREG, rb, ri);
    _inc_c_m(md, rb, ri, ms);
}

__jit_inline void
INCWm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _d16();
    _REXLrm(_NOREG, rb, ri);
    _inc_sil_m(md, rb, ri, ms);
}

__jit_inline void
INCLm(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms)
{
    _REXLrm(_NOREG, rb, ri);
    _inc_sil_m(md, rb, ri, ms);
}

/* --- Misc instructions --------------------------------------------------- */
__jit_inline void
BSFWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rd, rs);
    _bsf_sil_rr(rs, rd);
}

__jit_inline void
BSFWmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _bsf_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
BSRWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXLrr(rd, rs);
    _bsr_sil_rr(rs, rd);
}

__jit_inline void
BSRWmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _bsr_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
BSFLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rd, rs);
    _bsf_sil_rr(rs, rd);
}

__jit_inline void
BSFLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _bsf_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
BSRLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXLrr(rd, rs);
    _bsr_sil_rr(rs, rd);
}

__jit_inline void
BSRLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _bsr_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVSBWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXBLrr(rd, rs);
    _movsb_sil_rr(rs, rd);
}

__jit_inline void
MOVSBWmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _movsb_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVSBLrr(jit_gpr_t rs, jit_gpr_t rd)		
{
    _REXBLrr(rd, rs);
    _movsb_sil_rr(rs, rd);
}

__jit_inline void
MOVSBLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _movsb_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVSWLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBLrr(rd, rs);
    _movsw_il_rr(rs, rd);
}

__jit_inline void
MOVSWLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _movsw_il_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVZBWrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _d16();
    _REXBLrr(rd, rs);
    _movzb_sil_rr(rs, rd);
}

__jit_inline void
MOVZBWmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _d16();
    _REXLmr(rb, ri, rd);
    _movzb_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVZBLrr(jit_gpr_t rs, jit_gpr_t rd)		
{
    _REXBLrr(rd, rs);
    _movzb_sil_rr(rs, rd);
}

__jit_inline void
MOVZBLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _movzb_sil_mr(md, rb, ri, ms, rd);
}

__jit_inline void
MOVZWLrr(jit_gpr_t rs, jit_gpr_t rd)
{
    _REXBLrr(rd, rs);
    _movzw_il_rr(rs, rd);
}

__jit_inline void
MOVZWLmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _movzw_il_mr(md, rb, ri, ms, rd);
}

__jit_inline void
LEALmr(long md, jit_gpr_t rb, jit_gpr_t ri, jit_scl_t ms, jit_gpr_t rd)
{
    _REXLmr(rb, ri, rd);
    _lea_il_mr(md, rb, ri, ms, rd);
}

__jit_inline void
BSWAPLr(jit_gpr_t rd)
{
    _REXLrr(_NOREG, rd);
    _bswap_il_r(rd);
}

#define CLC_()				_O(0xf8)
#define STC_()				_O(0xf9)

#define CMC_()				_O(0xf5)
#define CLD_()				_O(0xfc)
#define STD_()				_O(0xfd)

#define CBTW_				CBW_
/* short ax = (char)al */
__jit_inline void
CBW_(void)
{
    _d16();
    _sign_extend_rax();
}

#define CWTL_				CWTL_
/* int eax = (short)ax */
__jit_inline void
CWDE_(void)
{
    _sign_extend_rax();
}

#define CWTD_()				CWD_()
/* short dx:ax = ax */
__jit_inline void
CWD_(void)
{
    _d16();
    _sign_extend_rdx_rax();
}

#define CLTD_()				CDQ_()
/* int edx:eax = eax */
__jit_inline void
CDQ_(void)
{
    _sign_extend_rdx_rax();
}

__jit_inline void
LAHF_(void)
{
    _O(0x9f);
}

__jit_inline void
SAHF_(void)
{
    _O(0x9e);
}

__jit_inline void
CPUID_(void)
{
    _O(0x0f);
    _O(0xa2);
}

__jit_inline void
RDTSC_(void)
{
    _O(0xff);
    _O(0x31);
}

__jit_inline void
ENTERii(long size, int nest)
{
    _O(0xc8);
    _jit_W(_su16(size));
    _jit_B(_su8(nest));
}

__jit_inline void
LEAVE_(void)
{
    _O(0xc9);
}

__jit_inline void
RET_(void)
{
    _O(0xc3);
}

__jit_inline void
RETi(long im)
{
    _O(0xc2);
    _jit_W(_su16(im));
}

__jit_inline void
NOP_(void)
{
    _O(0x90);
}

/* N byte NOPs */
__jit_inline void
NOPi(unsigned long nop)
{
    if (nop > 9)
	JITFAIL(".align argument too large");
    /* Recommended Multi-Byte Sequence of NOP Instruction */
    switch (nop) {
	case 0:
	    break;
	case 1:
	    /* NOP */
	    _O(0x90);
	    break;
	case 2:
	    /* 66 NOP */
	    _O(0x66);
	    _O(0x90);
	    break;
	case 3:
	    /* NOP DWORD ptr [EAX] */
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x00);
	    break;
	case 4:
	    /* NOP DWORD ptr [EAX + 00H] */
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x40);
	    _O(0x00);
	    break;
	case 5:
	    /* NOP DWORD ptr [EAX + EAX*1 + 00H] */
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x44);
	    _O(0x00);
	    _O(0x00);
	    break;
	case 6:
	    /* 66 NOP DWORD ptr [EAX + EAX*1 + 00H] */
	    _O(0x66);
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x44);
	    _O(0x00);
	    _O(0x00);
	    break;
	case 7:
	    /* NOP DWORD ptr [EAX + 00000000H] */
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x80);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    break;
	case 8:
	    /* NOP DWORD ptr [EAX + EAX*1 + 00000000H] */
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x84);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    break;
	default:
	    /* 66 NOP DWORD ptr [EAX + EAX*1 + 00000000H] */
	    _O(0x66);
	    _O(0x0f);
	    _O(0x1f);
	    _O(0x84);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    _O(0x00);
	    break;
    }
}


/* x87 instructions -- yay, we found a use for octal constants :-) */
enum {
    FPCW_MCW_PC		= 0x300,	/* Precision control mask */
    FPCW_64		= 0x300,	/* 64-bit precision */
    FPCW_53		= 0x200,	/* 53-bit precision */
    FPCW_24		= 0x000,	/* 24-bit precision */
    FPCW_MCW_RC		= 0xc00,	/* Rounding control mask */
    FPCW_CHOP		= 0xc00,	/* Truncate */
    FPCW_UP		= 0x800,	/* Round up */
    FPCW_DOWN		= 0x400,	/* Round down */
    FPCW_NEAR		= 0x000,	/* Round to nearest */
    FPCW_MCW_EM		= 0x03f,	/* Exception mask */
    FPCW_INVALID	= 0x001,	/* Allow invalid numbers */
    FPCW_DENORMAL	= 0x002,	/* Allow denormals */
    FPCW_ZERODIVIDE	= 0x004,	/* Allow divide by zero */
    FPCW_OVERFLOW	= 0x008,	/* Allow overflow */
    FPCW_UNDERFLOW	= 0x010,	/* Allow underflow */
    FPCW_INEXACT	= 0x020,	/* Allow inexact precision */
};

enum {
    FPSW_INVALID	= 0x0001,
    FPSW_DENORMAL	= 0x0002,
    FPSW_ZERODIVIDE	= 0x0004,
    FPSW_OVERFLOW	= 0x0008,
    FPSW_UNDERFLOW	= 0x0010,
    FPSW_INEXACT	= 0x0020,
    FPSW_STACKFAULT	= 0x0040,
    FPSW_EXCEPT		= 0x0080,
    FPSW_STACK		= 0x3800,	/* Top of stack pointer */
    FPSW_BUSY		= 0x8000,

    /* Condition mask (bits 14,10,9,8) */
    FPSW_COND		= 0x4700,
    FPSW_GT		= 0x0000,
    FPSW_LT		= 0x0100,
    FPSW_EQ		= 0x4000,
    FPSW_UN		= 0x4500,

    /* Classify mask (bits 14,10,9,8) bit 9 (C1) is sign */
    /* FIXME verify correctness */
    FPSW_CLASS		= 0x4700,	/* C3 C2 C1 C0 */
    FPSW_UNSUPPORTED	= 0x0000,	/*  0  0  0  0 */
    FPSW_NAN		= 0x0100,	/*  0  0  0  1 */
    FPSW_FINITE		= 0x0400,	/*  0  1  0  0 */
    FPSW_ZERO		= 0x4000,	/*  1  0  0  0 */
    FPSW_EMPTY		= 0x4100,	/*  1  0  0  1 */
    FPSW_DENORM		= 0x4400,	/*  1  1  0  0 */
};

#define ESCmi(D, B, I, S, OP)						\
    (_REXLrm(_NOREG, B, I),						\
     _O(0xd8 | (OP >> 3)),						\
     _i_X((OP & 7), D, B, I, S))

#define ESCri(RD, OP)							\
    _O(0xd8 | (OP >> 3)),						\
    _Mrm(_b11, (OP & 7), RD)

#define ESCrri(RS,RD,OP)						\
    ((RS) == _ST0							\
	? ESCri(RD,(OP | 040))						\
	: (RD) == _ST0							\
	    ? ESCri(RS, OP)						\
	    : JITFAIL ("coprocessor instruction without st0"))

#define    FLDSm(D,B,I,S)	ESCmi(D,B,I,S, 010)	/*    fld m32real */
#define    FSTSm(D,B,I,S)	ESCmi(D,B,I,S, 012)	/*    fst m32real */
#define   FSTPSm(D,B,I,S)	ESCmi(D,B,I,S, 013)	/*   fstp m32real */
#define   FLDCWm(D,B,I,S)	ESCmi(D,B,I,S, 015)	/*  fldcw m16int  */
#define   FSTCWm(D,B,I,S)	/* 0x9b prefix to fnstcw */		\
    (_REXLrm(_NOREG,B,I),						\
     _O(0x9b),								\
     _O(0xd9),								\
     _i_X(7, D, B, I, S))				/*  fstcw m16int  */
#define  FNSTCWm(D,B,I,S)	ESCmi(D,B,I,S, 017)	/* fnstcw m16int  */
#define   FILDLm(D,B,I,S)	ESCmi(D,B,I,S, 030)	/*   fild m32int  */

/* (p6 or newer) */
#define FISTTPLm(D,B,I,S)	ESCmi(D,B,I,S, 031)	/* fisttp m32int  */

#define   FISTLm(D,B,I,S)	ESCmi(D,B,I,S, 032)	/*   fist m32int  */
#define  FISTPLm(D,B,I,S)	ESCmi(D,B,I,S, 033)	/*  fistp m32int  */
#define    FLDTm(D,B,I,S)	ESCmi(D,B,I,S, 035)	/*    fld m80real */
#define   FSTPTm(D,B,I,S)	ESCmi(D,B,I,S, 037)	/*   fstp m80real */
#define    FLDLm(D,B,I,S)	ESCmi(D,B,I,S, 050)	/*    fld m64real */
#define    FSTLm(D,B,I,S)	ESCmi(D,B,I,S, 052)	/*    fst m64real */
#define   FSTPLm(D,B,I,S)	ESCmi(D,B,I,S, 053)	/*   fstp m64real */
#define   FILDWm(D,B,I,S)	ESCmi(D,B,I,S, 070)	/*   fild m16int  */

/* (p6 or newer) */
#define FISTTPQm(D,B,I,S)	ESCmi(D,B,I,S, 071)	/* fisttp m64int  */

#define   FISTWm(D,B,I,S)	ESCmi(D,B,I,S, 072)	/*   fist m16int  */
#define  FISTPWm(D,B,I,S)	ESCmi(D,B,I,S, 073)	/*  fistp m16int  */
#define   FILDQm(D,B,I,S)	ESCmi(D,B,I,S, 075)	/*   fild m64int  */
#define  FISTPQm(D,B,I,S)	ESCmi(D,B,I,S, 077)	/*  fistp m64int  */

#define  FADDrr(RS,RD)		ESCrri(RS,RD, 000)
#define  FMULrr(RS,RD)		ESCrri(RS,RD, 001)
#define  FSUBrr(RS,RD)		ESCrri(RS,RD, 004)
#define FSUBRrr(RS,RD)		ESCrri(RS,RD, 005)
#define  FDIVrr(RS,RD)		ESCrri(RS,RD, 006)
#define FDIVRrr(RS,RD)		ESCrri(RS,RD, 007)

/* st(0) = st(rs) if below (cf=1) (p6 or newer) */
#define   FCMOVB(RS)		ESCri(RS, 020)

/* st(0) = st(rs) if equal (zf=1) (p6 or newer) */
#define   FCMOVE(RS)		ESCri(RS, 021)

/* st(0) = st(rs) if below or equal (cf=1 or zf=1) (p6 or newer) */
#define  FCMOVBE(RS)		ESCri(RS, 022)

/* st(0) = st(rs) if unordered (pf=1) (p6 or newer) */
#define   FCMOVU(RS)		ESCri(RS, 023)

/* st(0) = st(rs) if not below (cf=0) (p6 or newer) */
#define  FCMOVNB(RS)		ESCri(RS, 030)

/* st(0) = st(rs) if not equal (zf=0) (p6 or newer) */
#define  FCMOVNE(RS)		ESCri(RS, 031)

/* st(0) = st(rs) if not below or equal (cf=0 and zf=0) (p6 or newer) */
#define FCMOVNBE(RS)		ESCri(RS, 032)

/* st(0) = st(rs) if not unordered (pf=0) (p6 or newer) */
#define  FCMOVNU(RS)		ESCri(RS, 033)

/* *push*, st(0) = st(rd+1) */
#define     FLDr(RD)		ESCri(RD, 010)
#define    FXCHr(RD)		ESCri(RD, 011)
#define   FFREEr(RD)		ESCri(RD, 050)
#define     FSTr(RD)		ESCri(RD, 052)
#define    FSTPr(RD)		ESCri(RD, 053)

/* Compare st(0) with st(rs) */
#define    FCOMr(RS)		ESCri(RS, 002)
#define   FCOMPr(RS)		ESCri(RS, 003)

/* Compare st(0) with st(rd) and set eflags (p6 or newer)
   (raise invalid operation if unordered argument) */
#define  FUCOMIr(RS)		ESCri(RS, 035)
#define FUCOMIPr(RS)		ESCri(RS, 075)

/* Compare st(0) with st(rs) and set eflags (p6 or newer)
   (does not raise invalid operation if unordered argument) */
#define   FCOMIr(RS)		ESCri(RS, 036)
#define  FCOMIPr(RS)		ESCri(RS, 076)

/* Compare st(0) with st(rs) */
#define   FUCOMr(RS)		ESCri(RS, 054)
#define  FUCOMPr(RS)		ESCri(RS, 055)

#define   FADDPr(RD)		ESCri(RD, 060)
#define   FMULPr(RD)		ESCri(RD, 061)
#define   FSUBPr(RD)		ESCri(RD, 064)
#define  FSUBRPr(RD)		ESCri(RD, 065)
#define   FDIVPr(RD)		ESCri(RD, 066)
#define  FDIVRPr(RD)		ESCri(RD, 067)

/* Clear exceptions */
#define FNCLEX_()		_OO(0xdbe2)

#define FCLEX_()		(_O(0x9b), FNCLEX_())

/* st(0) = -st(0) */
#define FCHS_()			_OO(0xd9e0)	/* ESCri(0, 014) */

/* st(0) = fabs(st(0)) */
#define FABS_()			_OO(0xd9e1)	/* ESCri(1, 014) */

/* Compare st(0) with 0.0 */
#define FTST_()			_OO(0xd9e4)	/* ESCri(4, 014) */

/* Classify ST(0) */
#define FXAM_()			_OO(0xd9e5)	/* ESCri(5, 014) */

/* Push +1.0 to the x87 stack */
#define FLD1_()			_OO(0xd9e8)	/* ESCri(0, 015) */

/* Push log2(10) */
#define FLDL2T_()		_OO(0xd9e9)	/* ESCri(1, 015) */

/* Push log2(e) */
#define FLDL2E_()		_OO(0xd9ea)	/* ESCri(2, 015) */

/* Push PI */
#define FLDPI_()		_OO(0xd9eb)	/* ESCri(3, 015) */

/* Push log10(2) */
#define FLDLG2_()		_OO(0xd9ec)	/* ESCri(4, 015) */

/* Push log(2) */
#define FLDLN2_()		_OO(0xd9ed)	/* ESCri(5, 015) */

/* Push +0.0 */
#define FLDZ_()			_OO(0xd9ee)	/* ESCri(6, 015) */

/* st(0) = pow(2, st(0)) - 1 */
#define F2XM1_()		_OO(0xd9f0)	/* ESCri(0, 016) */

/* temp = st(1) * log2(st(0)) => *pop*,  st(0) = temp */
#define FYL2X_()		_OO(0xd9f1)	/* ESCri(1, 016) */

/* temp = tangent(st(0)) => *push*, st(0) = 1.0, st(1) = temp */
#define FPTAN_()		_OO(0xd9f2)	/* ESCri(2, 016) */

/* temp = arctan(st(1)/st(0)) => *pop*, st(0) = temp */
#define FPATAN_()		_OO(0xd9f3)	/* ESCri(3, 016) */

/* temp = st(0)	 => st(0) = significand(temp), st(1) = exponent(temp) */
#define FXTRACT_()		_OO(0xd9f4)	/* ESCri(4, 016) */

/* ST(0) = rem(ST(0)/ST(1)) - IEEE spec */
#define FPREM1_()		_OO(0xd9f5)	/* ESCri(5, 016) */

/* pop/rotate x87 stack */
#define FDECSTP_()		_OO(0xd9f6)	/* ESCri(6, 016) */

/* pop/rotate x87 stack */
#define FINCSTP_()		_OO(0xd9f7)	/* ESCri(7, 016) */

/* st(0) = rem(st(0)/st(1)) - compat with 8087/80287 */
#define FPREM_()		_OO(0xd9f8)	/* ESCri(0, 017) */

/* temp = st(1) * log2(st(0) + 1) => *pop*,  st(0) = temp */
#define FYL2XP1_()		_OO(0xd9f9)	/* ESCri(1, 017) */

/* st(0) = sqrt(st(0)) */
#define FSQRT_()		_OO(0xd9fa)	/* ESCri(2, 017) */

/* (sin,cos) = sincos(st(0)) => *push*, st(0) = cos, st(1) = sin */
#define FSINCOS_()		_OO(0xd9fb)	/* ESCri(3, 017) */

/* Round st(0) to an integer accordingly to rounding mode */
#define FRNDINT_()		_OO(0xd9fc)	/* ESCri(4, 017) */

/* st(0) *= * pow(2, st(1)) */
#define FSCALE_()		_OO(0xd9fd)	/* ESCri(5, 017) */

/* st(0) = sin(st(0)) */
#define FSIN_()			_OO(0xd9fe)	/* ESCri(6, 017) */

/* st(0) = cos(st(0)) */
#define FCOS_()			_OO(0xd9ff)	/* ESCri(7, 017) */

#define FNSTSWr(RD)							\
    ((RD == _RAX) ? _OO(0xdfe0) : JITFAIL ("RAX expected"))


/* --- Media 128-bit instructions ------------------------------------------ */

enum {
  X86_SSE_MOV    = 0x10,
  X86_SSE_MOVLP  = 0x12,
  X86_SSE_MOVHP  = 0x16,
  X86_SSE_MOVA   = 0x28,
  X86_SSE_CVTIS  = 0x2a,
  X86_SSE_CVTTSI = 0x2c,
  X86_SSE_CVTSI  = 0x2d,
  X86_SSE_UCOMI  = 0x2e,
  X86_SSE_COMI   = 0x2f,
  X86_SSE_ROUND  = 0x3a,
  X86_SSE_SQRT   = 0x51,
  X86_SSE_RSQRT  = 0x52,
  X86_SSE_RCP    = 0x53,
  X86_SSE_AND    = 0x54,
  X86_SSE_ANDN   = 0x55,
  X86_SSE_OR     = 0x56,
  X86_SSE_XOR    = 0x57,
  X86_SSE_ADD    = 0x58,
  X86_SSE_MUL    = 0x59,
  X86_SSE_CVTSD  = 0x5a,
  X86_SSE_CVTDT  = 0x5b,
  X86_SSE_SUB    = 0x5c,
  X86_SSE_MIN    = 0x5d,
  X86_SSE_DIV    = 0x5e,
  X86_SSE_MAX    = 0x5f,
  X86_SSE_MOV2   = 0xd6
};

enum {
    MXCSR_INV_EXCPT	= 0x0001,
    MXCSR_DENORM_EXCPT	= 0x0002,
    MXCSR_ZERODIV_EXCPT	= 0x0004,
    MXCSR_OVER_EXCPT	= 0x0008,
    MXCSR_UNDER_EXCPT	= 0x0010,
    MXCSR_PREC_EXCPT	= 0x0020,
    MXCSR_DENORMISZERO	= 0x0040,
    MXCSR_INV_MASK	= 0x0080,
    MXCSR_DENORM_MASK	= 0x0100,
    MXCSR_ZERODIV_MASK	= 0x0200,
    MXCSR_OVER_MASK	= 0x0400,
    MXCSR_UNDER_MASK	= 0x0800,
    MXCSR_PREC_MASK	= 0x1000,

    MXCSR_RND_MASK	= 0x6000,	/* Round mode mask */
    MXCSR_RND_NEAR	= 0x0000,	/* Round to nearest */
    MXCSR_RND_DOWN	= 0x2000,	/* Round toward -oo */
    MXCSR_RND_UP	= 0x4000,	/* Round toward +oo */
    MXCSR_RND_CHOP	= 0x6000,	/* Truncate to zero */

    MXCSR_FLUSHTOZERO	= 0x8000
};

/*	_format	Opcd,	Mod,	r,	m,	mem=dsp+sib,	imm... */

#define __SSELrr(OP, RS, RD)						\
    (_REXFrr(RD, RS),							\
     _OO(0x0f00 | (OP)),						\
     _Mrm(_b11, _rX(RD), _rX(RS)))

#define __SSELFrr(OP, RS, RD)						\
    (_REXLFrr(RD, RS),							\
     _OO(0x0f00 | (OP)),						\
     _Mrm(_b11, _rA(RD), _rX(RS)))

#define __SSEFLrr(OP, RS, RD)						\
    (_REXFLrr(RD, RS),							\
     _OO(0x0f00 | (OP)),						\
     _Mrm(_b11, _rX(RD), _rA(RS)))

#define __SSELmr(OP, MD, MB, MI, MS, RD)				\
    (_REXFmr(MB, MI, RD),						\
     _OO(0x0f00 | (OP)),						\
     _f_X(RD, MD, MB, MI, MS))

#define __SSELrm(OP, RS, MD, MB, MI, MS)				\
    (_REXFrm(RS, MB, MI),						\
     _OO(0x0f00 | (OP)),						\
     _f_X(RS, MD, MB, MI, MS))

#define __SSEL1rm(OP, RS, MD, MB, MI, MS)				\
    (_REXFrm(RS, MB, MI),						\
     _OO(0x0f01 | (OP)),						\
     _f_X(RS, MD, MB, MI, MS))

#define _SSELrr(PX, OP, RS, RD)						\
    (_jit_B(PX), __SSELrr(OP, RS, RD))
#define _SSELFrr(PX, OP, RS, RD)					\
    (_jit_B(PX), __SSELFrr(OP, RS, RD))
#define _SSEFLrr(PX, OP, RS, RD)					\
    (_jit_B(PX), __SSEFLrr(OP, RS, RD))
#define _SSELmr(PX, OP, MD, MB, MI, MS, RD)				\
    (_jit_B(PX), __SSELmr(OP, MD, MB, MI, MS, RD))
#define _SSELrm(PX, OP, RS, MD, MB, MI, MS)				\
    (_jit_B(PX), __SSELrm(OP, RS, MD, MB, MI, MS))
#define _SSEL1rm(PX, OP, RS, MD, MB, MI, MS)				\
    (_jit_B(PX), __SSEL1rm(OP, RS, MD, MB, MI, MS))

#define _SSEPSrr(OP,RS,RD)		__SSELrr (      OP, RS, RD)
#define _SSEPSmr(OP,MD,MB,MI,MS,RD)	__SSELmr (      OP, MD, MB, MI, MS, RD)
#define _SSEPSrm(OP,RS,MD,MB,MI,MS)	__SSELrm (      OP, RS, MD, MB, MI, MS)
#define _SSEPS1rm(OP,RS,MD,MB,MI,MS)	__SSEL1rm(      OP, RS, MD, MB, MI, MS)

#define _SSEPDrr(OP,RS,RD)		 _SSELrr (0x66, OP, RS, RD)
#define _SSEPDmr(OP,MD,MB,MI,MS,RD)	 _SSELmr (0x66, OP, MD, MB, MI, MS, RD)
#define _SSEPDrm(OP,RS,MD,MB,MI,MS)	 _SSELrm (0x66, OP, RS, MD, MB, MI, MS)
#define _SSEPD1rm(OP,RS,MD,MB,MI,MS)	 _SSEL1rm(0x66, OP, RS, MD, MB, MI, MS)

#define _SSESSrr(OP,RS,RD)		 _SSELrr (0xf3, OP, RS, RD)
#define _SSESSmr(OP,MD,MB,MI,MS,RD)	 _SSELmr (0xf3, OP, MD, MB, MI, MS, RD)
#define _SSESSrm(OP,RS,MD,MB,MI,MS)	 _SSELrm (0xf3, OP, RS, MD, MB, MI, MS)
#define _SSESS1rm(OP,RS,MD,MB,MI,MS)	 _SSEL1rm(0xf3, OP, RS, MD, MB, MI, MS)

#define _SSESDrr(OP,RS,RD)		 _SSELrr (0xf2, OP, RS, RD)
#define _SSESDmr(OP,MD,MB,MI,MS,RD)	 _SSELmr (0xf2, OP, MD, MB, MI, MS, RD)
#define _SSESDrm(OP,RS,MD,MB,MI,MS)	 _SSELrm (0xf2, OP, RS, MD, MB, MI, MS)
#define _SSESD1rm(OP,RS,MD,MB,MI,MS)	 _SSEL1rm(0xf2, OP, RS, MD, MB, MI, MS)

#define LDMXCSRmr(MD, MB, MI, MS)					\
    (_REXLmr(MB, MI, _NOREG),						\
     _O(0x0f),								\
     _O(0xae),								\
     _i_X(2, MD, MB, MI, MS))
#define STMXCSRrm(MD, MB, MI, MS)					\
    (_REXLrm(_NOREG, MI, MB),						\
     _O(0x0f),								\
     _O(0xae),								\
     _i_X(3, MD, MB, MI, MS))

#define ADDPSrr(RS, RD)			_SSEPSrr(X86_SSE_ADD, RS, RD)
#define ADDPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_ADD, MD, MB, MI, MS, RD)
#define ADDPDrr(RS, RD)			_SSEPDrr(X86_SSE_ADD, RS, RD)
#define ADDPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_ADD, MD, MB, MI, MS, RD)

#define ADDSSrr(RS, RD)			_SSESSrr(X86_SSE_ADD, RS, RD)
#define ADDSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_ADD, MD, MB, MI, MS, RD)
#define ADDSDrr(RS, RD)			_SSESDrr(X86_SSE_ADD, RS, RD)
#define ADDSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_ADD, MD, MB, MI, MS, RD)

#define ANDNPSrr(RS, RD)		_SSEPSrr(X86_SSE_ANDN, RS, RD)
#define ANDNPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_ANDN, MD, MB, MI, MS, RD)
#define ANDNPDrr(RS, RD)		_SSEPDrr(X86_SSE_ANDN, RS, RD)
#define ANDNPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_ANDN, MD, MB, MI, MS, RD)

#define ANDNSSrr			ANDNPSrr
#define ANDNSSmr			ANDNPSrr
#define ANDNSDrr			ANDNPDrr
#define ANDNSDmr			ANDNPDrr

#define ANDPSrr(RS, RD)			_SSEPSrr(X86_SSE_AND, RS, RD)
#define ANDPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_AND, MD, MB, MI, MS, RD)
#define ANDPDrr(RS, RD)			_SSEPDrr(X86_SSE_AND, RS, RD)
#define ANDPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_AND, MD, MB, MI, MS, RD)

#define ANDSSrr				ANDPSrr
#define ANDSSmr				ANDPSrr
#define ANDSDrr				ANDPDrr
#define ANDSDmr				ANDPDrr

#define DIVPSrr(RS, RD)			_SSEPSrr(X86_SSE_DIV, RS, RD)
#define DIVPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_DIV, MD, MB, MI, MS, RD)
#define DIVPDrr(RS, RD)			_SSEPDrr(X86_SSE_DIV, RS, RD)
#define DIVPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_DIV, MD, MB, MI, MS, RD)

#define DIVSSrr(RS, RD)			_SSESSrr(X86_SSE_DIV, RS, RD)
#define DIVSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_DIV, MD, MB, MI, MS, RD)
#define DIVSDrr(RS, RD)			_SSESDrr(X86_SSE_DIV, RS, RD)
#define DIVSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_DIV, MD, MB, MI, MS, RD)

#define MAXPSrr(RS, RD)			_SSEPSrr(X86_SSE_MAX, RS, RD)
#define MAXPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_MAX, MD, MB, MI, MS, RD)
#define MAXPDrr(RS, RD)			_SSEPDrr(X86_SSE_MAX, RS, RD)
#define MAXPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_MAX, MD, MB, MI, MS, RD)

#define MAXSSrr(RS, RD)			_SSESSrr(X86_SSE_MAX, RS, RD)
#define MAXSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_MAX, MD, MB, MI, MS, RD)
#define MAXSDrr(RS, RD)			_SSESDrr(X86_SSE_MAX, RS, RD)
#define MAXSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_MAX, MD, MB, MI, MS, RD)

#define MINPSrr(RS, RD)			_SSEPSrr(X86_SSE_MIN, RS, RD)
#define MINPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_MIN, MD, MB, MI, MS, RD)
#define MINPDrr(RS, RD)			_SSEPDrr(X86_SSE_MIN, RS, RD)
#define MINPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_MIN, MD, MB, MI, MS, RD)

#define MINSSrr(RS, RD)			_SSESSrr(X86_SSE_MIN, RS, RD)
#define MINSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_MIN, MD, MB, MI, MS, RD)
#define MINSDrr(RS, RD)			_SSESDrr(X86_SSE_MIN, RS, RD)
#define MINSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_MIN, MD, MB, MI, MS, RD)

#define MULPSrr(RS, RD)			_SSEPSrr(X86_SSE_MUL, RS, RD)
#define MULPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_MUL, MD, MB, MI, MS, RD)
#define MULPDrr(RS, RD)			_SSEPDrr(X86_SSE_MUL, RS, RD)
#define MULPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_MUL, MD, MB, MI, MS, RD)

#define MULSSrr(RS, RD)			_SSESSrr(X86_SSE_MUL, RS, RD)
#define MULSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_MUL, MD, MB, MI, MS, RD)
#define MULSDrr(RS, RD)			_SSESDrr(X86_SSE_MUL, RS, RD)
#define MULSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_MUL, MD, MB, MI, MS, RD)

#define ORPSrr(RS, RD)			_SSEPSrr(X86_SSE_OR, RS, RD)
#define ORPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_OR, MD, MB, MI, MS, RD)
#define ORPDrr(RS, RD)			_SSEPDrr(X86_SSE_OR, RS, RD)
#define ORPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_OR, MD, MB, MI, MS, RD)

#define ORSSrr				ORPSrr
#define ORSSmr				ORPSrr
#define ORSDrr				ORPDrr
#define ORSDmr				ORPDrr

#define RCPPSrr(RS, RD)			_SSEPSrr(X86_SSE_RCP, RS, RD)
#define RCPPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_RCP, MD, MB, MI, MS, RD)
#define RCPSSrr(RS, RD)			_SSESSrr(X86_SSE_RCP, RS, RD)
#define RCPSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_RCP, MD, MB, MI, MS, RD)

#define RSQRTPSrr(RS, RD)		_SSEPSrr(X86_SSE_RSQRT, RS, RD)
#define RSQRTPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_RSQRT, MD, MB, MI, MS, RD)
#define RSQRTSSrr(RS, RD)		_SSESSrr(X86_SSE_RSQRT, RS, RD)
#define RSQRTSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_RSQRT, MD, MB, MI, MS, RD)

#define SQRTPSrr(RS, RD)		_SSEPSrr(X86_SSE_SQRT, RS, RD)
#define SQRTPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)
#define SQRTPDrr(RS, RD)		_SSEPDrr(X86_SSE_SQRT, RS, RD)
#define SQRTPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)

#define SQRTSSrr(RS, RD)		_SSESSrr(X86_SSE_SQRT, RS, RD)
#define SQRTSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)
#define SQRTSDrr(RS, RD)		_SSESDrr(X86_SSE_SQRT, RS, RD)
#define SQRTSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_SQRT, MD, MB, MI, MS, RD)

#define SUBPSrr(RS, RD)			_SSEPSrr(X86_SSE_SUB, RS, RD)
#define SUBPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_SUB, MD, MB, MI, MS, RD)
#define SUBPDrr(RS, RD)			_SSEPDrr(X86_SSE_SUB, RS, RD)
#define SUBPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_SUB, MD, MB, MI, MS, RD)

#define SUBSSrr(RS, RD)			_SSESSrr(X86_SSE_SUB, RS, RD)
#define SUBSSmr(MD, MB, MI, MS, RD)	_SSESSmr(X86_SSE_SUB, MD, MB, MI, MS, RD)
#define SUBSDrr(RS, RD)			_SSESDrr(X86_SSE_SUB, RS, RD)
#define SUBSDmr(MD, MB, MI, MS, RD)	_SSESDmr(X86_SSE_SUB, MD, MB, MI, MS, RD)

#define XORPSrr(RS, RD)			_SSEPSrr(X86_SSE_XOR, RS, RD)
#define XORPSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_XOR, MD, MB, MI, MS, RD)
#define XORPDrr(RS, RD)			_SSEPDrr(X86_SSE_XOR, RS, RD)
#define XORPDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_XOR, MD, MB, MI, MS, RD)

#define XORSSrr				XORPSrr
#define XORSSmr				XORPSrr
#define XORSDrr				XORPDrr
#define XORSDmr				XORPDrr

/* No prefixes here.  */
#define COMISSrr(RS, RD)		_SSEPSrr(X86_SSE_COMI, RS, RD)
#define COMISSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_COMI, MD, MB, MI, MS, RD)
#define COMISDrr(RS, RD)		_SSEPDrr(X86_SSE_COMI, RS, RD)
#define COMISDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_COMI, MD, MB, MI, MS, RD)

/* No prefixes here.  */
#define UCOMISSrr(RS, RD)		_SSEPSrr(X86_SSE_UCOMI, RS, RD)
#define UCOMISSmr(MD, MB, MI, MS, RD)	_SSEPSmr(X86_SSE_UCOMI, MD, MB, MI, MS, RD)
#define UCOMISDrr(RS, RD)		_SSEPDrr(X86_SSE_UCOMI, RS, RD)
#define UCOMISDmr(MD, MB, MI, MS, RD)	_SSEPDmr(X86_SSE_UCOMI, MD, MB, MI, MS, RD)

#define MOVSSrr(RS, RD)			_SSESSrr (X86_SSE_MOV, RS, RD)
#define MOVSSmr(MD, MB, MI, MS, RD)	_SSESSmr (X86_SSE_MOV, MD, MB, MI, MS, RD)
#define MOVSSrm(RS, MD, MB, MI, MS)	_SSESS1rm(X86_SSE_MOV, RS, MD, MB, MI, MS)

#define MOVSDrr(RS, RD)			_SSESDrr (X86_SSE_MOV, RS, RD)
#define MOVSDmr(MD, MB, MI, MS, RD)	_SSESDmr (X86_SSE_MOV, MD, MB, MI, MS, RD)
#define MOVSDrm(RS, MD, MB, MI, MS)	_SSESD1rm(X86_SSE_MOV, RS, MD, MB, MI, MS)

#define MOVAPSrr(RS, RD)		_SSEPSrr (X86_SSE_MOVA, RS, RD)
#define MOVAPSmr(MD, MB, MI, MS, RD)	_SSEPSmr (X86_SSE_MOVA, MD, MB, MI, MS, RD)
#define MOVAPSrm(RS, MD, MB, MI, MS)	_SSEPS1rm(X86_SSE_MOVA, RS, MD, MB, MI, MS)

#define MOVAPDrr(RS, RD)		_SSEPDrr (X86_SSE_MOVA, RS, RD)
#define MOVAPDmr(MD, MB, MI, MS, RD)	_SSEPDmr (X86_SSE_MOVA, MD, MB, MI, MS, RD)
#define MOVAPDrm(RS, MD, MB, MI, MS)	_SSEPD1rm(X86_SSE_MOVA, RS, MD, MB, MI, MS)

#define CVTPS2PIrr(RS, RD)		__SSELrr(      X86_SSE_CVTSI, RS, RD)
#define CVTPS2PImr(MD, MB, MI, MS, RD)	__SSELmr(      X86_SSE_CVTSI, MD, MB, MI, MS, RD)
#define CVTPD2PIrr(RS, RD)		 _SSELrr(0x66, X86_SSE_CVTSI, RS, RD)
#define CVTPD2PImr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, X86_SSE_CVTSI, MD, MB, MI, MS, RD)

#define CVTPI2PSrr(RS, RD)		__SSELrr(      X86_SSE_CVTIS, RS, RD)
#define CVTPI2PSmr(MD, MB, MI, MS, RD)	__SSELmr(      X86_SSE_CVTIS, MD, MB, MI, MS, RD)
#define CVTPI2PDrr(RS, RD)		 _SSELrr(0x66, X86_SSE_CVTIS, RS, RD)
#define CVTPI2PDmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, X86_SSE_CVTIS, MD, MB, MI, MS, RD)

#define CVTPS2PDrr(RS, RD)		__SSELrr(      X86_SSE_CVTSD, RS, RD)
#define CVTPS2PDmr(MD, MB, MI, MS, RD)	__SSELmr(      X86_SSE_CVTSD, MD, MB, MI, MS, RD)
#define CVTPD2PSrr(RS, RD)		 _SSELrr(0x66, X86_SSE_CVTSD, RS, RD)
#define CVTPD2PSmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, X86_SSE_CVTSD, MD, MB, MI, MS, RD)

#define CVTSS2SDrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_CVTSD, RS, RD)
#define CVTSS2SDmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, X86_SSE_CVTSD, MD, MB, MI, MS, RD)
#define CVTSD2SSrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_CVTSD, RS, RD)
#define CVTSD2SSmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf2, X86_SSE_CVTSD, MD, MB, MI, MS, RD)

#define CVTTSS2SILrr(RS, RD)		 _SSELFrr(0xf3, X86_SSE_CVTTSI, RS, RD)
#define CVTTSS2SILmr(MD, MB, MI, MS, RD) _SSELmr(0xf3, X86_SSE_CVTTSI, MD, MB, MI, MS, RD)
#define CVTTSD2SILrr(RS, RD)		 _SSELFrr(0xf2, X86_SSE_CVTTSI, RS, RD)
#define CVTTSD2SILmr(MD, MB, MI, MS, RD) _SSELmr(0xf2, X86_SSE_CVTTSI, MD, MB, MI, MS, RD)

#define CVTSS2SILrr(RS, RD)		 _SSELFrr(0xf3, X86_SSE_CVTSI, RS, RD)
#define CVTSS2SILmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, X86_SSE_CVTSI, MD, MB, MI, MS, RD)
#define CVTSD2SILrr(RS, RD)		 _SSELFrr(0xf2, X86_SSE_CVTSI, RS, RD)
#define CVTSD2SILmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf2, X86_SSE_CVTSI, MD, MB, MI, MS, RD)

#define CVTSI2SSLrr(RS, RD)		 _SSEFLrr(0xf3, X86_SSE_CVTIS, RS, RD)
#define CVTSI2SSLmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, X86_SSE_CVTIS, MD, MB, MI, MS, RD)
#define CVTSI2SDLrr(RS, RD)		 _SSEFLrr(0xf2, X86_SSE_CVTIS, RS, RD)
#define CVTSI2SDLmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf2, X86_SSE_CVTIS, MD, MB, MI, MS, RD)

#define MOVDLXrr(RS, RD)		 _SSEFLrr(0x66, 0x6e, RS, RD)
#define MOVDLXmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, 0x6e, MD, MB, MI, MS, RD)

#define MOVDXLrr(RS, RD)		 _SSELrr(0x66, 0x7e, RS, RD)
#define MOVDXLrm(RS, MD, MB, MI, MS)	 _SSELrm(0x66, 0x7e, RS, MD, MB, MI, MS)

#define MOVDLMrr(RS, RD)		__SSELrr(      0x6e, RS, RD)
#define MOVDLMmr(MD, MB, MI, MS, RD)	__SSELmr(      0x6e, MD, MB, MI, MS, RD)

#define MOVDMLrr(RS, RD)		__SSELrr(      0x7e, RS, RD)
#define MOVDMLrm(RS, MD, MB, MI, MS)	__SSELrm(      0x7e, RS, MD, MB, MI, MS)

#define MOVDQ2Qrr(RS, RD)		 _SSELrr(0xf2, X86_SSE_MOV2, RS, RD)
#define MOVQ2DQrr(RS, RD)		 _SSELrr(0xf3, X86_SSE_MOV2, RS, RD)
#define MOVHLPSrr(RS, RD)		__SSELrr(      X86_SSE_MOVLP, RS, RD)
#define MOVLHPSrr(RS, RD)		__SSELrr(      X86_SSE_MOVHP, RS, RD)

#define MOVDQArr(RS, RD)		 _SSELrr(0x66, 0x6f, RS, RD)
#define MOVDQAmr(MD, MB, MI, MS, RD)	 _SSELmr(0x66, 0x6f, MD, MB, MI, MS, RD)
#define MOVDQArm(RS, MD, MB, MI, MS)	 _SSELrm(0x66, 0x7f, RS, MD, MB, MI, MS)

#define MOVDQUrr(RS, RD)		 _SSELrr(0xf3, 0x6f, RS, RD)
#define MOVDQUmr(MD, MB, MI, MS, RD)	 _SSELmr(0xf3, 0x6f, MD, MB, MI, MS, RD)
#define MOVDQUrm(RS, MD, MB, MI, MS)	 _SSELrm(0xf3, 0x7f, RS, MD, MB, MI, MS)

#define MOVHPDmr(MD, MB, MI, MS, RD)	 _SSELmr (0x66, X86_SSE_MOVHP, MD, MB, MI, MS, RD)
#define MOVHPDrm(RS, MD, MB, MI, MS)	 _SSEL1rm(0x66, X86_SSE_MOVHP, RS, MD, MB, MI, MS)
#define MOVHPSmr(MD, MB, MI, MS, RD)	__SSELmr (      X86_SSE_MOVHP, MD, MB, MI, MS, RD)
#define MOVHPSrm(RS, MD, MB, MI, MS)	__SSEL1rm(      X86_SSE_MOVHP, RS, MD, MB, MI, MS)

#define MOVLPDmr(MD, MB, MI, MS, RD)	 _SSELmr (0x66, X86_SSE_MOVLP, MD, MB, MI, MS, RD)
#define MOVLPDrm(RS, MD, MB, MI, MS)	 _SSEL1rm(0x66, X86_SSE_MOVLP, RS, MD, MB, MI, MS)
#define MOVLPSmr(MD, MB, MI, MS, RD)	__SSELmr (      X86_SSE_MOVLP, MD, MB, MI, MS, RD)
#define MOVLPSrm(RS, MD, MB, MI, MS)	__SSEL1rm(      X86_SSE_MOVLP, RS, MD, MB, MI, MS)

#define PCMPEQBrr(RS, RD)						\
    _SSELrr(0x66, 0x74, RS, RD)
#define PCMPEQBrm(RS, MD, MB, MI, MS)					\
    _SSELmr(0x66, 0x74, MD, MB, MI, MS, RD)

#define PCMPEQWrr(RS, RD)						\
    _SSELrr(0x66, 0x75, RS, RD)
#define PCMPEQWrm(RS, MD, MB, MI, MS)					\
    _SSELmr(0x66, 0x75, MD, MB, MI, MS, RD)

#define PCMPEQLrr(RS, RD)						\
    _SSELrr(0x66, 0x76, RS, RD)
#define PCMPEQLrm(RS, MD, MB, MI, MS)					\
    _SSELmr(0x66, 0x76, MD, MB, MI, MS, RD)

#define PSRLWrr(RS, RD)							\
    _SSELrr(0x66, 0xd1, RS, RD)
#define PSRLWrm(RS, MD, MB, MI, MS)					\
    _SSELmr(0x66, 0xd1, MD, MB, MI, MS, RD)
#define PSRLWir(IM, RD)							\
    (_O(0x66),								\
     _REXLrr(_NOREG, RD),						\
     _O(0x0f),								\
     _O(0x71),								\
     _Mrm(_b11, 2, _rX(RD)),						\
     _O(IM))

#define PSRLLrr(RS, RD)							\
    _SSELrr(0x66, 0xd2, RS, RD)
#define PSRLLrm(RS, MD, MB, MI, MS)					\
    _SSELmr(0x66, 0xd2, MD, MB, MI, MS, RD)
#define PSRLLir(IM, RD)							\
    (_O(0x66),								\
     _REXLFrr(_NOREG, RD),						\
     _O(0x0f),								\
     _O(0x72),								\
     _Mrm(_b11, 2, _rX(RD)),						\
     _O(IM))

#define PSRLQrr(RS, RD)							\
    _SSELrr(0x66, 0xd3, RS, RD)
#define PSRLQrm(RS, MD, MB, MI, MS)					\
    _SSELmr(0x66, 0xd3, MD, MB, MI, MS, RD)
#define PSRLQir(IM, RD)							\
    (_O(0x66),								\
     _REXLFrr(_NOREG, RD),						\
     _O(0x0f),								\
     _O(0x73),								\
     _Mrm(_b11, 2, _rX(RD)),						\
     _O(IM))

/* SSE4.1 */
#define ROUNDSSrri(RS, RD, IM)						\
    (_O(0x66), _REXFrr(RD, RS), _OO(0xf00|X86_SSE_ROUND), _O(0x0b),	\
     _Mrm(_b11, _rX(RS), _rX(RD)), _O(IM))
#define ROUNDSDrri(RS, RD, IM)						\
    (_O(0x66), _REXFrr(RD, RS), _OO(0xf00|X86_SSE_ROUND), _O(0x0a),	\
     _Mrm(_b11, _rX(RS), _rX(RD)), _O(IM))
#define PCMPEQQrr(RS, RD)						\
    (_O(0x66), _REXFrr(RD, RS), _OO(0x0f38), _O(0x29),			\
     _Mrm(_b11, _rX(RS), _rX(RD)))

/*** References:										*/
/*												*/
/* [1] "Intel Architecture Software Developer's Manual Volume 1: Basic Architecture",		*/
/*     Intel Corporation 1997.									*/
/*												*/
/* [2] "Intel Architecture Software Developer's Manual Volume 2: Instruction Set Reference",	*/
/*     Intel Corporation 1997.									*/

#endif	/* LIGHTNING_DEBUG */
#endif	/* __lightning_asm_i386_h */
