/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the i386
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
    _BP		= 0x1e,		/* (callee saved) frame pointer */
    _RA		= 0x1f,		/* return address */
} jit_gpr_t;

/* FIXME these are most likely reversed... */
typedef union {
    struct {
	_ui	h : 16;
	_ui	d : 5;
	_ui	c : 11;
    } b;
    int		o;
} mips_coder_t;

typedef union {
    struct {
	_ui	h : 6;
	_ui	s : 5;
	_ui	t : 5;
	_ui	d : 5;
	_ui	c : 11;
    } b;
    int		o;
} mips_alur_t;

typedef union {
    struct {
	_ui	h : 6;
	_ui	s : 5;
	_ui	t : 5;
	_ui	i : 16;
    } b;
    int		o;
} mips_alui_t;

typedef union {
    struct {
	_ui	h : 6;
	_ui	s : 5;
	_ui	t : 5;
	_ui	d : 5;
	_ui	v : 5;
	_ui	c : 6;
    } b;
    int		o;
} mips_shift_t;

typedef enum {
    MIPS_SLLV	= 0x04,
    MIPS_SRLV	= 0x06,
    MIPS_MFHI	= 0x10,
    MIPS_MFLO	= 0x10,
    MIPS_ADD	= 0x20,
    MIPS_ADDU	= 0x21,
    MIPS_AND	= 0x24,
    MIPS_MUL	= 0x18,
    MIPS_MULU	= 0x19,
    MIPS_DIV	= 0x1a,
    MIPS_DIVU	= 0x1b,
    MIPS_SUB	= 0x22,
    MIPS_SUBU	= 0x23,
    MIPS_OR	= 0x25,
    MIPS_XOR	= 0x26,
    MIPS_SLT	= 0x2a,
    MIPS_SLTU	= 0x2b,
} mips_rcode_t;

typedef enum {
    MIPS_SLL	= 0x0,
    MIPS_SRL	= 0x2,
    MIPS_SRA	= 0x3,
    MIPS_ADDI	= 0x8,
    MIPS_ADDIU	= 0x9,
    MIPS_ANDI	= 0xc,
    MIPS_ORI	= 0xd,
    MIPS_XORI	= 0xe,
    MIPS_LUI	= 0xf,
} mips_icode_t;

__jit_inline void
mips_coder(jit_state_t _jit, mips_rcode_t code, jit_gpr_t r0)
{
    mips_coder_t	c;

    c.b.h = 0;
    c.b.d = r0;
    c.b.c = code;

    _jit_I(c.o);
}

__jit_inline void
mips_coderi(jit_state_t _jit, mips_icode_t code, jit_gpr_t r0, int i0)
{
    mips_alui_t		c;

    c.b.h = code;
    c.b.s = 0;		/* ignored */
    c.b.t = r0;
    c.b.i = _s16(i0);

    _jit_I(c.o);
}

__jit_inline void
mips_alurrr(jit_state_t _jit, mips_rcode_t code,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_alur_t		c;

    c.b.h = 0;
    c.b.s = r1;
    c.b.t = r2;
    c.b.d = r0;
    c.b.c = code;

    _jit_I(c.o);
}

__jit_inline void
mips_alurri(jit_state_t _jit, mips_icode_t code,
	    jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    mips_alui_t		c;

    c.b.h = code;
    c.b.s = r1;
    c.b.t = r0;
    c.b.i = _s16(i0);

    _jit_I(c.o);
}

__jit_inline void
mips_shiftrri(jit_state_t _jit, mips_rcode_t code,
	      jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    mips_shift_t	c;

    c.b.h = 0;
    c.b.s = 0;		/* ignored? */
    c.b.t = r1;
    c.b.d = r0;
    c.b.v = _u5(i0);
    c.b.c = code;

    _jit_I(c.o);
}

__jit_inline void
mips_shiftrrr(jit_state_t _jit, mips_rcode_t code,
	      jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_shift_t	c;

    c.b.h = 0;
    c.b.s = r2;
    c.b.t = r1;
    c.b.d = r0;
    c.b.v = 0;		/* ignored */
    c.b.c = code;

    _jit_I(c.o);
}

/* Reference:
 *	http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
 */

#endif /* __lightning_asm_h */
