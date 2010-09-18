/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the i386
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

#if !_ASM_SAFETY
#  define _r1(R)		_rN(R)
#  define _r2(R)		_rN(R)
#  define _r4(R)		_rN(R)
#  define _r8(R)		_rN(R)
#  define _rM(R)		_rN(R)
#  define _rX(R)		_rN(R)
#else
#  define _r1(R)							\
    (((R) >= _RAX && (R) <= _RBX)					\
	? _rN(R)							\
	: JITFAIL("bad 8-bit register " #R))
#  define _r2(R)							\
    (((R) >= _RAX && (R) <= _RDI)					\
	? _rN(R)							\
	: JITFAIL("bad 16-bit register " #R))
#  define _r4(R)							\
    (((R) >= _RAX && (R) <= _RDI)					\
	? _rN(R)							\
	: JITFAIL("bad 32-bit register " #R))
#  define _r8(R)							\
	JITFAIL("bad 64-bit register " #R)
#  define _rM(R)							\
    (((R) >= _XMM0 && (R) <= _XMM7)					\
	? _rN(R)							\
	: JITFAIL("bad MMX register " #R))
#  define _rX(R)							\
    (((R) >= _XMM0 && (R) <= _XMM7)					\
	? _rN(R)							\
	: JITFAIL("bad SSE register " #R))
#endif

#define _rA(R)			_r4(R)

#define jit_reg8_p(rs)		((rs) >= _RAX && (rs) <= _RBX)

/* Use RIP-addressing in 64-bit mode, if possible */
#define _r_X(   R, D,B,I,S,O)	(_r0P(I) ? (_r0P(B)    ? _r_D   (R,D                ) : \
				           (_rsp12P(B) ? _r_DBIS(R,D,_RSP,_RSP,1)   : \
						         _r_DB  (R,D,     B       )))  : \
				 (_r0P(B)	       ? _r_4IS (R,D,	         I,S)   : \
				 (!_rspP(I)            ? _r_DBIS(R,D,     B,     I,S)   : \
						         JITFAIL("illegal index register: %esp"))))
#define _m32only(X)		(X)
#define _m64only(X)		JITFAIL("invalid instruction in 32-bit mode")
#define _m64(X)			((void)0)

#define CALLsr(R)			CALLLsr(R)
#define JMPsr(R)			JMPLsr(R)

#endif
#endif /* __lightning_asm_h */
