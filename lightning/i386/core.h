/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (i386 version)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2006 Free Software Foundation, Inc.
 * Written by Paolo Bonzini and Matthew Flatt.
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



#ifndef __lightning_core_i386_h
#define __lightning_core_i386_h

#define JIT_FP			_EBP
#define JIT_SP			_ESP
#define JIT_RET			_EAX

#define jit_can_sign_extend_unsigned_char_p(im)				\
    ((im) >= 0 && (im) <= 0x7f)

#define jit_can_sign_extend_char_p(im)					\
    (jit_can_sign_extend_unsigned_char_p(im) ||				\
     ((im) < 0 && (im) >= -0x80))

#define jit_can_sign_extend_unsigned_short_p(im)			\
    ((im) >= 0 && (im) <= 0x7fff)

#define jit_can_sign_extend_short_p(im)					\
    (jit_can_sign_extend_unsigned_short_p(im) ||			\
     ((im) < 0 && (im) >= -0x8000))

#define jit_can_sign_extend_unsigned_int_p(im)				\
    ((im) >= 0 && (im) <= 0x7fffffff)

#define jit_can_sign_extend_int_p(im)					\
    (jit_can_sign_extend_unsigned_int_p(im) ||				\
     ((im) < 0 && (im) >= -0x80000000))

#if __WORDSIZE == 32
#  define jit_movr_i(rd, r0)	((rd) == (r0) ? 0 : MOVLrr((r0), (rd)))
#  define jit_pushr_i(r0)	PUSHLr(r0)
#  define jit_popr_i(r0)	POPLr(r0)
#else
#  define jit_movr_i(rd, r0)	((rd) == (r0) ? 0 : MOVQrr((r0), (rd)))
#  define jit_pushr_i(r0)	PUSHQr(r0)
#  define jit_popr_i(r0)	POPQr(r0)
#endif

/* 3-parameter operation, with immediate */
#define jit_op_(d, s1, op2d)				\
	((s1 == d) ? op2d : (MOVLrr(s1, d), op2d))

/* 3-parameter operation, optimizable */
#define jit_opo_(d, s1, s2, op1d, op2d, op12d)		\
	((s2 == d) ? op2d : 				\
	((s1 == d) ? op1d : op12d))

/* For LT, LE, ... */
#define jit_replace8(d, cmp, op)				\
	(jit_check8(d)						\
	 ? ((cmp),						\
             MOVLir(0, (d)),					\
             op(_rR(d) | _AL))					\
	 : (jit_pushr_i(_EAX), (cmp),				\
            MOVLir(0, _EAX),					\
            op(_AL), MOVLrr(_EAX, (d)), jit_popr_i(_EAX)))

#define jit_bool_r(d, s1, s2, op)					\
	(jit_replace8(d, CMPLrr(s2, s1), op))

#define jit_bool_i(d, rs, is, op)					\
	(jit_replace8(d, CMPLir(is, rs), op))

/* When CMP with 0 can be replaced with TEST */
#define jit_bool_i0(d, rs, is, op, op0)					\
	((is) != 0							\
	  ? (jit_replace8(d, CMPLir(is, rs), op)) 			\
	  : (jit_replace8(d, TESTLrr(rs, rs), op0)))

/* For BLT, BLE, ... */
#define jit_bra_r(s1, s2, op)		(CMPLrr(s2, s1), op, _jit.x.pc)
#define jit_bra_i(rs, is, op)		(CMPLir(is, rs), op, _jit.x.pc)

/* When CMP with 0 can be replaced with TEST */
#define jit_bra_i0(rs, is, op, op0)					\
	( (is) == 0 ? (TESTLrr(rs, rs), op0, _jit.x.pc) : (CMPLir(is, rs), op, _jit.x.pc))

/* Reduce arguments of XOR/OR/TEST */
#define jit_reduce_(op)	op
#define jit_reduce(op, is, rs)							\
	(_u8P(is) && jit_check8(rs) ? jit_reduce_(op##Bir(is, jit_reg8(rs))) :	\
	(_u16P(is) && JIT_CAN_16 ? jit_reduce_(op##Wir(is, jit_reg16(rs))) :			\
	jit_reduce_(op##Lir(is, rs)) ))

/* ALU */
#define jit_negr_i(rd, r0)		jit_negr_i(rd, r0)
__jit_inline void
jit_negr_i(int rd, int r0)
{
    if (rd == r0)
	NEGLr(rd);
    else {
	XORLrr(rd, rd);
	SUBLrr(r0, rd);
    }
}

#define jit_addi_i(rd, r0, i0)		jit_addi_i(rd, r0, i0)
__jit_inline void
jit_addi_i(int rd, int r0, int i0)
{
    if (i0 == 0)
	jit_movr_i(rd, r0);
    else if (i0 == 1) {
	jit_movr_i(rd, r0);
	INCLr(rd);
    }
    else if (i0 == -1) {
	jit_movr_i(rd, r0);
	DECLr(rd);
    }
    else if (rd == r0)
	ADDLir(i0, rd);
    else
	LEALmr(i0, r0, 0, 0, rd);
}

#define jit_addr_i(rd, r0, r1)		jit_addr_i(rd, r0, r1)
__jit_inline void
jit_addr_i(int rd, int r0, int r1)
{
    if (rd == r0)
	ADDLrr(r1, rd);
    else if (rd == r1)
	ADDLrr(r0, rd);
    else
	LEALmr(0, r0, r1, 1, rd);
}

#define jit_subr_i(rd, r0, r1)		jit_subr_i(rd, r0, r1)
__jit_inline void
jit_subr_i(int rd, int r0, int r1)
{
    if (r0 == r1)
	XORLrr(rd, rd);
    else if (rd == r1) {
	SUBLrr(r0, rd);
	NEGLr(rd);
    }
    else {
	jit_movr_i(rd, r0);
	SUBLrr(r1, rd);
    }
}

#define jit_addci_ui(rd, r0, i0)	jit_addci_ui(rd, r0, i0)
__jit_inline void
jit_addci_ui(int rd, int r0, unsigned int i0)
{
    if (rd == r0)
	ADDLir(i0, rd);
    else {
	MOVLir(i0, rd);
	ADDLrr(r0, rd);
    }
}

#define jit_addcr_ui(rd, r0, r1)	jit_addcr_ui(rd, r0, r1)
__jit_inline void
jit_addcr_ui(int rd, int r0, int r1)
{
    if (rd == r1)
	ADDLrr(r0, rd);
    else if (rd == r0)
	ADDLrr(r1, rd);
    else {
	MOVLrr(r0, rd);
	ADDLrr(r1, rd);
    }
}

#define jit_addxi_ui(rd, r0, i0)	jit_addxi_ui(rd, r0, i0)
__jit_inline void
jit_addxi_ui(int rd, int r0, unsigned int i0)
{
    if (rd == r0)
	ADCLir(i0, rd);
    else {
	MOVLir(i0, rd);
	ADCLrr(r0, rd);
    }
}

#define jit_addxr_ui(rd, r0, r1)	jit_addxr_ui(rd, r0, r1)
__jit_inline void
jit_addxr_ui(int rd, int r0, int r1) {
    if (rd == r1)
	ADCLrr(r0, rd);
    else if (rd == r0)
	ADCLrr(r1, rd);
    else {
	MOVLrr(r0, rd);
	ADCLrr(r1, rd);
    }
}

#define jit_subci_ui(rd, r0, i0)	jit_subci_ui(rd, r0, i0)
__jit_inline void
jit_subci_ui(int rd, int r0, unsigned int i0)
{
    if (rd == r0)
	SUBLir(i0, rd);
    else {
	MOVLir(i0, rd);
	SUBLrr(r0, rd);
    }
}

#define jit_subcr_ui(rd, r0, r1)	jit_subcr_ui(rd, r0, r1)
__jit_inline void
jit_subcr_ui(int rd, int r0, int r1)
{
    if (rd == r0)
	SUBLrr(r1, rd);
    else {
	MOVLrr(r0, rd);
	SUBLir(r1, rd);
    }
}

#define jit_subxi_ui(rd, r0, i0)	jit_subxi_ui(rd, r0, i0)
__jit_inline void
jit_subxi_ui(int rd, int r0, unsigned int i0)
{
    if (rd == r0)
	SBBLir(i0, rd);
    else {
	MOVLir(i0, rd);
	SBBLrr(r0, rd);
    }
}

#define jit_subxr_ui(rd, r0, r1)	jit_subxr_ui(rd, r0, r1)
__jit_inline void
jit_subxr_ui(int rd, int r0, int r1)
{
    if (rd == r0)
	SBBLrr(r1, rd);
    else {
	MOVLrr(r0, rd);
	SBBLir(r1, rd);
    }
}

#define jit_andi_i(rd, r0, i0)		jit_andi_i(rd, r0, i0)
__jit_inline void
jit_andi_i(int rd, int r0, int i0)
{
    if (i0 == 0)
	XORLrr(rd, rd);
    else if (i0 == -1) {
	if (rd != r0)
	    MOVLrr(r0, rd);
    }
    else {
	if (rd != r0)
	    MOVLrr(r0, rd);
	ANDLir(i0, rd);
    }
}

#define jit_andr_i(rd, r0, r1)		jit_andr_i(rd, r0, r1)
__jit_inline void
jit_andr_i(int rd, int r0, int r1)
{
    if (r0 == r1)
	jit_movr_i(rd, r0);
    else if (rd == r0)
	ANDLrr(r1, rd);
    else if (rd == r1)
	ANDLrr(r0, rd);
    else {
	MOVLrr(r0, rd);
	ANDLrr(r1, rd);
    }
}

#define jit_ori_i(rd, r0, i0)		jit_ori_i(rd, r0, i0)
__jit_inline void
jit_ori_i(int rd, int r0, int i0)
{
    if (i0 == 0)
	jit_movr_i(rd, r0);
    else if (i0 == -1) {
	MOVLir(-1, rd);
    }
    else {
	jit_movr_i(rd, r0);
	if (jit_check8(rd) && jit_can_sign_extend_char_p(i0))
	    ORBir(i0, rd);
#if JIT_CAN_16
	else if (jit_can_sign_extend_short_p(i0))
	    ORWir(i0, rd);
#endif
	else
	    ORLir(i0, rd);
    }
}

#define jit_orr_i(rd, r0, r1)		jit_orr_i(rd, r0, r1)
__jit_inline void
jit_orr_i(int rd, int r0, int r1)
{
    if (r0 == r1)
	jit_movr_i(rd, r0);
    else if (rd == r0)
	ORLrr(r1, rd);
    else if (rd == r1)
	ORLrr(r0, rd);
    else {
	MOVLrr(r0, rd);
	ORLrr(r1, rd);
    }
}

#define jit_xori_i(rd, r0, i0)		jit_xori_i(rd, r0, i0)
__jit_inline void
jit_xori_i(int rd, int r0, int i0)
{
    if (i0 == 0)
	jit_movr_i(rd, r0);
    else if (i0 == -1) {
	jit_movr_i(rd, r0);
	NOTLr(rd);
    }
    else {
	jit_movr_i(rd, r0);
	if (jit_check8(rd) && jit_can_sign_extend_char_p(i0))
	    XORBir(i0, rd);
#if JIT_CAN_16
	else if (jit_can_sign_extend_short_p(i0))
	    XORWir(i0, rd);
#endif
	else
	    XORLir(i0, rd);
    }
}

#define jit_xorr_i(rd, r0, r1)		jit_xorr_i(rd, r0, r1)
__jit_inline void
jit_xorr_i(int rd, int r0, int r1)
{
    if (r0 == r1) {
	if (rd != r0)
	    MOVLrr(r0, rd);
	else
	    XORLrr(rd, rd);
    }
    else if (rd == r0)
	XORLrr(r1, rd);
    else if (rd == r1)
	XORLrr(r0, rd);
    else {
	MOVLrr(r0, rd);
	XORLrr(r1, rd);
    }
}

/*  Instruction format is:
 *	imul reg32/mem32
 *  and the result is stored in %edx:%eax
 *  %eax = low 32 bits
 *  %edx = high 32 bits
 */
__jit_inline void
jit_muli_i_(int r0, int i0)
{
    if (jit_reg32(r0) == _EAX) {
	MOVLir(i0, _EDX);
	IMULLr(_EDX);
    }
    else {
	MOVLir(i0, _EAX);
	IMULLr(r0);
    }
}

#define jit_hmuli_i(rd, r0, i1)		jit_hmuli_i(rd, r0, i1)
__jit_inline void
jit_hmuli_i(int rd, int r0, int i0)
{
    if (jit_reg32(rd) == _EDX) {
	jit_pushr_i(_EAX);
	jit_muli_i_(r0, i0);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(rd) == _EAX) {
	jit_pushr_i(_EDX);
	jit_muli_i_(r0, i0);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_muli_i_(r0, i0);
	MOVLrr(_EDX, rd);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}

__jit_inline void
jit_mulr_i_(int r0, int r1)
{
    if (jit_reg32(r1) == _EAX)
	IMULLr(r0);
    else if (jit_reg32(r0) == _EAX)
	IMULLr(r1);
    else {
	MOVLrr(r1, _EAX);
	IMULLr(r0);
    }
}

#define jit_hmulr_i(rd, r0, r1)		jit_hmulr_i(rd, r0, r1)
__jit_inline void
jit_hmulr_i(int rd, int r0, int r1)
{
    if (jit_reg32(rd) == _EDX) {
	jit_pushr_i(_EAX);
	jit_mulr_i_(r0, r1);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(rd) == _EAX) {
	jit_pushr_i(_EDX);
	jit_mulr_i_(r0, r1);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_mulr_i_(r0, r1);
	MOVLrr(_EDX, rd);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}

/*  Instruction format is:
 *	mul reg32/mem32
 *  and the result is stored in %edx:%eax
 *  %eax = low 32 bits
 *  %edx = high 32 bits
 */
__jit_inline void
jit_muli_ui_(int r0, unsigned int i0)
{
    if (jit_reg32(r0) == _EAX) {
	MOVLir(i0, _EDX);
	MULLr(_EDX);
    }
    else {
	MOVLir(i0, _EAX);
	MULLr(r0);
    }
}

#define jit_hmuli_ui(rd, r0, i1)	jit_hmuli_ui(rd, r0, i1)
__jit_inline void
jit_hmuli_ui(int rd, int r0, unsigned int i0)
{
    if (jit_reg32(rd) == _EDX) {
	jit_pushr_i(_EAX);
	jit_muli_ui_(r0, i0);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(rd) == _EAX) {
	jit_pushr_i(_EDX);
	jit_muli_ui_(r0, i0);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_muli_ui_(r0, i0);
	MOVLrr(_EDX, rd);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}

__jit_inline void
jit_mulr_ui_(int r0, int r1)
{
    if (jit_reg32(r1) == _EAX)
	MULLr(r0);
    else if (jit_reg32(r0) == _EAX)
	MULLr(r1);
    else {
	MOVLrr(r1, _EAX);
	MULLr(r0);
    }
}

#define jit_hmulr_ui(rd, r0, r1)	jit_hmulr_ui(rd, r0, r1)
__jit_inline void
jit_hmulr_ui(int rd, int r0, int r1)
{
    if (jit_reg32(rd) == _EDX) {
	jit_pushr_i(_EAX);
	jit_mulr_ui_(r0, r1);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(rd) == _EAX) {
	jit_pushr_i(_EDX);
	jit_mulr_ui_(r0, r1);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_mulr_ui_(r0, r1);
	MOVLrr(_EDX, rd);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}

#define jit_muli_i(rd, r0, i0)		jit_muli_i(rd, r0, i0)
#define jit_muli_ui(rd, r0, i0)		jit_muli_i(rd, r0, i0)
__jit_inline void
jit_muli_i(int rd, int r0, int i0)
{
    if (i0 == 0)
	XORLrr(rd, rd);
    else if (i0 == 1)
	jit_movr_i(rd, r0);
    else if (i0 == -1)
	jit_negr_i(rd, r0);
    else if (jit_can_sign_extend_char_p(i0))
	IMULBLLirr(i0, r0, rd);
    else
	IMULLLLirr(i0, r0, rd);
}

#define jit_mulr_i(rd, r0, r1)		jit_mulr_i(rd, r0, r1)
#define jit_mulr_ui(rd, r0, r1)		jit_mulr_i(rd, r0, r1)
__jit_inline void
jit_mulr_i(int rd, int r0, int r1)
{
    if (rd == r0)
	IMULLrr(r1, rd);
    else if (rd == r1)
	IMULLrr(r0, rd);
    else {
	MOVLrr(r0, rd);
	IMULLrr(r1, rd);
    }
}

__jit_inline void
jit_divi_i_(int rd, int r0, int i0, int is_signed, int is_divide)
{
    int		div;
    int		pop;

    if (rd != _EDX)
	jit_pushr_i(_EDX);
    if (rd != _EAX)
	jit_pushr_i(_EAX);

    if (rd == _EAX || rd == _EDX) {
	div = _ECX;
	pop = 1;
    }
    else {
	div = rd;
	pop = 0;
    }

    if (pop)
	jit_pushr_i(div);
    if (r0 != _EAX)
	MOVLrr(r0, _EAX);
    MOVLir(i0, div);

    if (is_signed) {
	CDQ_();
	IDIVLr(div);
    }
    else {
	XORLrr(_EDX, _EDX);
	DIVLr(div);
    }

    if (pop)
	jit_popr_i(div);

    if (rd != _EAX) {
	if (is_divide)
	    MOVLrr(_EAX, rd);
	jit_popr_i(_EAX);
    }
    if (rd != _EDX) {
	if (!is_divide)
	    MOVLrr(_EDX, rd);
	jit_popr_i(_EDX);
    }
}

__jit_inline void
jit_divr_i_(int rd, int r0, int r1, int is_signed, int is_divide)
{
    int		div;
    int		pop;

    if (rd != _EDX)
	jit_pushr_i(_EDX);
    if (rd != _EAX)
	jit_pushr_i(_EAX);

    if (r1 == _EAX) {
	if (rd == _EAX || rd == _EDX) {
	    div = r0 == _ECX ? _EBX : _ECX;
	    jit_pushr_i(div);
	    MOVLrr(_EAX, div);
	    if (r0 != _EAX)
		MOVLrr(r0, _EAX);
	    pop = 1;
	}
	else {
	    if (rd == r0)
		XCHGLrr(_EAX, rd);
	    else {
		if (rd != _EAX)
		    MOVLrr(_EAX, rd);
		if (r0 != _EAX)
		    MOVLrr(r0, _EAX);
	    }
	    div = rd;
	    pop = 0;
	}
    }
    else if (r1 == _EDX) {
	if (rd == _EAX || rd == _EDX) {
	    div = r0 == _ECX ? _EBX : _ECX;
	    jit_pushr_i(div);
	    MOVLrr(_EDX, div);
	    if (r0 != _EAX)
		MOVLrr(r0, _EAX);
	    pop = 1;
	}
	else {
	    if (r0 != _EAX)
		MOVLrr(r0, _EAX);
	    MOVLrr(_EDX, rd);
	    div = rd;
	    pop = 0;
	}
    }
    else {
	if (r0 != _EAX)
	    MOVLrr(r0, _EAX);
	div = r1;
	pop = 0;
    }

    if (is_signed) {
	CDQ_();
	IDIVLr(div);
    }
    else {
	XORLrr(_EDX, _EDX);
	DIVLr(div);
    }

    if (pop)
	jit_popr_i(div);
    if (rd != _EAX) {
	if (is_divide)
	    MOVLrr(_EAX, rd);
	jit_popr_i(_EAX);
    }
    if (rd != _EDX) {
	if (!is_divide)
	    MOVLrr(_EDX, rd);
	jit_popr_i(_EDX);
    }
}

#define jit_divi_i(rd, r0, i0)		jit_divi_i(rd, r0, i0)
__jit_inline void
jit_divi_i(int rd, int r0, int i0)
{
    jit_divi_i_(jit_reg32(rd), jit_reg32(r0), i0, 1, 1);
}

#define jit_divr_i(rd, r0, r1)		jit_divr_i(rd, r0, r1)
__jit_inline void
jit_divr_i(int rd, int r0, int r1)
{
    jit_divr_i_(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), 1, 1);
}

#define jit_divi_ui(rd, r0, i0)		jit_divi_ui(rd, r0, i0)
__jit_inline void
jit_divi_ui(int rd, int r0, unsigned int i0)
{
    jit_divi_i_(jit_reg32(rd), jit_reg32(r0), i0, 0, 1);
}

#define jit_divr_ui(rd, r0, r1)		jit_divr_ui(rd, r0, r1)
__jit_inline void
jit_divr_ui(int rd, int r0, int r1)
{
    jit_divr_i_(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), 0, 1);
}

#define jit_modi_i(rd, r0, i0)		jit_modi_i(rd, r0, i0)
__jit_inline void
jit_modi_i(int rd, int r0, int i0)
{
    jit_divi_i_(jit_reg32(rd), jit_reg32(r0), i0, 1, 0);
}

#define jit_modr_i(rd, r0, r1)		jit_modr_i(rd, r0, r1)
__jit_inline void
jit_modr_i(int rd, int r0, int r1)
{
    jit_divr_i_(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), 1, 0);
}

#define jit_modi_ui(rd, r0, i0)		jit_modi_ui(rd, r0, i0)
__jit_inline void
jit_modi_ui(int rd, int r0, unsigned int i0)
{
    jit_divi_i_(jit_reg32(rd), jit_reg32(r0), i0, 0, 0);
}

#define jit_modr_ui(rd, r0, r1)		jit_modr_ui(rd, r0, r1)
__jit_inline void
jit_modr_ui(int rd, int r0, int r1)
{
    jit_divr_i_(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), 0, 0);
}

/* Shifts */
__jit_inline void
_jit_shift32(int rd, int r0, int r1, int code)
{
    int		lsh;

    if (rd != _ECX && r1 != _ECX)
	jit_pushr_i(_ECX);

    if (r0 == _ECX) {
	if (rd != _ECX) {
	    if (rd == r1)
		XCHGLrr(_ECX, rd);
	    else {
		MOVLrr(_ECX, rd);
		MOVLrr(r1, _ECX);
	    }
	    lsh = rd;
	}
	/* rd == _ECX */
	else if (r1 == _ECX) {
	    jit_pushr_i(_EAX);
	    MOVLrr(_ECX, _EAX);
	    lsh = _EAX;
	}
	else {
	    jit_pushr_i(r1);
	    XCHGLrr(_ECX, r1);
	    lsh = r1;
	}
    }
    /* r0 != _ECX */
    else if (rd == _ECX) {
	jit_pushr_i(r0);
	if (r1 != _ECX)
	    MOVLrr(r1, _ECX);
	lsh = r0;
    }
    else {
	if (r1 != _ECX)
	    MOVLrr(r1, _ECX);
	if (rd != r0)
	    MOVLrr(r0, rd);
	lsh = rd;
    }

    _ROTSHILrr(code, _CL, lsh);

    if (lsh != rd) {
	MOVLrr(lsh, rd);
	jit_popr_i(lsh);
    }

    if (rd != _ECX && r1 != _ECX)
	jit_popr_i(_ECX);
}

#define jit_lshi_i(rd, r0, i0)		jit_lshi_i(rd, r0, i0)
__jit_inline void
jit_lshi_i(int rd, int r0, unsigned char i0)
{
    if (i0 == 0)
	jit_movr_i(rd, r0);
    else if (i0 <= 3)
	LEALmr(0, 0, r0, 1 << i0, rd);
    else {
	jit_movr_i(rd, r0);
	SHLLir(i0, rd);
    }
}

#define jit_lshr_i(rd, r0, r1)		jit_lshr_i(rd, r0, r1)
__jit_inline void
jit_lshr_i(int rd, int r0, int r1)
{
    _jit_shift32(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), X86_SHL);
}

#define jit_rshi_i(rd, r0, i0)		jit_rshi_i(rd, r0, i0)
__jit_inline void
jit_rshi_i(int rd, int r0, unsigned char i0)
{
    jit_movr_i(rd, r0);
    if (i0)
	SARLir(i0, rd);
}

#define jit_rshr_i(rd, r0, r1)		jit_rshr_i(rd, r0, r1)
__jit_inline void
jit_rshr_i(int rd, int r0, int r1)
{
    _jit_shift32(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), X86_SAR);
}

#define jit_rshi_ui(rd, r0, i0)		jit_rshi_ui(rd, r0, i0)
__jit_inline void
jit_rshi_ui(int rd, int r0, unsigned char i0)
{
    jit_movr_i(rd, r0);
    if (i0)
	SHRLir(i0, rd);
}

#define jit_rshr_ui(rd, r0, r1)		jit_rshr_ui(rd, r0, r1)
__jit_inline void
jit_rshr_ui(int rd, int r0, int r1)
{
    _jit_shift32(jit_reg32(rd), jit_reg32(r0), jit_reg32(r1), X86_SHR);
}

/* Stack */
#define jit_retval_i(rd)	((void)jit_movr_i ((rd), _EAX))

#define jit_patch_movi(pa,pv)   (*_PSL((pa) - sizeof(long)) = _jit_SL((pv)))

#define jit_ntoh_ui(d, rs)	jit_op_((d), (rs), BSWAPLr(d))
#define jit_ntoh_us(d, rs)	jit_op_((d), (rs), RORWir(8, d))

/* Boolean */
#define jit_ltr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETLr  )
#define jit_ler_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETLEr )
#define jit_gtr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETGr  )
#define jit_ger_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETGEr )
#define jit_eqr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETEr  )
#define jit_ner_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETNEr )
#define jit_ltr_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETBr  )
#define jit_ler_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETBEr )
#define jit_gtr_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETAr  )
#define jit_ger_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETAEr )

#define jit_lti_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETLr,  SETSr  )
#define jit_lei_i(d, rs, is)	jit_bool_i ((d), (rs), (is), SETLEr	    )
#define jit_gti_i(d, rs, is)	jit_bool_i ((d), (rs), (is), SETGr 	    )
#define jit_gei_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETGEr, SETNSr )
#define jit_eqi_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETEr,  SETEr  )
#define jit_nei_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETNEr, SETNEr )
#define jit_lti_ui(d, rs, is)	jit_bool_i ((d), (rs), (is), SETBr	    )
#define jit_lei_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETBEr, SETEr  )
#define jit_gti_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETAr,  SETNEr )
#define jit_gei_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETAEr, INCLr  )

/* Jump */
#define jit_bltr_i(label, s1, s2)	jit_bra_r((s1), (s2), JLm(label) )
#define jit_bler_i(label, s1, s2)	jit_bra_r((s1), (s2), JLEm(label) )
#define jit_bgtr_i(label, s1, s2)	jit_bra_r((s1), (s2), JGm(label) )
#define jit_bger_i(label, s1, s2)	jit_bra_r((s1), (s2), JGEm(label) )
#define jit_beqr_i(label, s1, s2)	jit_bra_r((s1), (s2), JEm(label) )
#define jit_bner_i(label, s1, s2)	jit_bra_r((s1), (s2), JNEm(label) )
#define jit_bltr_ui(label, s1, s2)	jit_bra_r((s1), (s2), JBm(label) )
#define jit_bler_ui(label, s1, s2)	jit_bra_r((s1), (s2), JBEm(label) )
#define jit_bgtr_ui(label, s1, s2)	jit_bra_r((s1), (s2), JAm(label) )
#define jit_bger_ui(label, s1, s2)	jit_bra_r((s1), (s2), JAEm(label) )
#define jit_bmsr_i(label, s1, s2)	(TESTLrr((s1), (s2)), JNZm(label), _jit.x.pc)
#define jit_bmcr_i(label, s1, s2)	(TESTLrr((s1), (s2)), JZm(label),  _jit.x.pc)
#define jit_boaddr_i(label, s1, s2)	(ADDLrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_bosubr_i(label, s1, s2)	(SUBLrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_boaddr_ui(label, s1, s2)	(ADDLrr((s2), (s1)), JCm(label), _jit.x.pc)
#define jit_bosubr_ui(label, s1, s2)	(SUBLrr((s2), (s1)), JCm(label), _jit.x.pc)

#define jit_blti_i(label, rs, is)	jit_bra_i0((rs), (is), JLm(label), JSm(label) )
#define jit_blei_i(label, rs, is)	jit_bra_i ((rs), (is), JLEm(label)		    )
#define jit_bgti_i(label, rs, is)	jit_bra_i ((rs), (is), JGm(label)		    )
#define jit_bgei_i(label, rs, is)	jit_bra_i0((rs), (is), JGEm(label), JNSm(label) )
#define jit_beqi_i(label, rs, is)	jit_bra_i0((rs), (is), JEm(label), JEm(label) )
#define jit_bnei_i(label, rs, is)	jit_bra_i0((rs), (is), JNEm(label), JNEm(label) )
#define jit_blti_ui(label, rs, is)	jit_bra_i ((rs), (is), JBm(label)		    )
#define jit_blei_ui(label, rs, is)	jit_bra_i0((rs), (is), JBEm(label), JEm(label) )
#define jit_bgti_ui(label, rs, is)	jit_bra_i0((rs), (is), JAm(label), JNEm(label) )
#define jit_bgei_ui(label, rs, is)	jit_bra_i ((rs), (is), JAEm(label)		    )
#define jit_boaddi_i(label, rs, is)	(ADDLir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_bosubi_i(label, rs, is)	(SUBLir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_boaddi_ui(label, rs, is)	(ADDLir((is), (rs)), JCm(label), _jit.x.pc)
#define jit_bosubi_ui(label, rs, is)	(SUBLir((is), (rs)), JCm(label), _jit.x.pc)

#define jit_bmsi_i(label, rs, is)	(jit_reduce(TEST, (is), (rs)), JNZm(label), _jit.x.pc)
#define jit_bmci_i(label, rs, is)	(jit_reduce(TEST, (is), (rs)), JZm(label),  _jit.x.pc)

#define jit_jmpi(label)			(JMPm( ((unsigned long) (label))), _jit.x.pc)
#define jit_jmpr(reg)			JMPsr(reg)

/* Memory */
#define jit_ldr_uc(d, rs)               MOVZBLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_uc(d, s1, s2)          MOVZBLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_str_c(rd, rs)               jit_movbrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_c(d1, d2, rs)          jit_movbrm((rs), 0,    (d1), (d2), 1)
							    
#define jit_ldr_us(d, rs)               MOVZWLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_us(d, s1, s2)          MOVZWLmr(0,    (s1), (s2), 1,  (d))
							    
#define jit_str_s(rd, rs)               MOVWrm(jit_reg16(rs), 0,    (rd), 0,    0)
#define jit_stxr_s(d1, d2, rs)          MOVWrm(jit_reg16(rs), 0,    (d1), (d2), 1)
							    
#define jit_str_i(rd, rs)               MOVLrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_i(d1, d2, rs)          MOVLrm((rs), 0,    (d1), (d2), 1)
							    
/* Extra */
#define jit_nop()			NOP_()

#define _jit_alignment(pc, n)		(((pc ^ _MASK(4)) + 1) & _MASK(n))
#define jit_align(n) 			NOPi(_jit_alignment(_jit_UL(_jit.x.pc), (n)))


#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_X86_64 \
	: defined (__x86_64__)
#include "core-64.h"
#else
#include "core-32.h"
#endif

#endif /* __lightning_core_i386_h */

