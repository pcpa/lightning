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

#define jit_can_zero_extend_char_p(im)					\
    ((im) >= 0 && (im) <= 0x80)

#define jit_can_sign_extend_char_p(im)					\
    (((im) >= 0 && (im) <=  0x7f) ||					\
     ((im) <  0 && (im) >= -0x80))

#define jit_can_zero_extend_short_p(im)					\
    ((im) >= 0 && (im) <= 0x8000)

#define jit_can_sign_extend_short_p(im)					\
    (((im) >= 0 && (im) <=  0x7fff) ||					\
     ((im) <  0 && (im) >= -0x8000))

#define jit_can_zero_extend_int_p(im)					\
    ((im) >= 0 && (im) <= 0x80000000)

#define jit_can_sign_extend_int_p(im)					\
    (((im) >= 0 && (im) <=  0x7fffffff) ||				\
     ((im) <  0 && (im) >= -0x80000000))

#define jit_movr_i(rd, r0)		jit_movr_i(rd, r0)
#define jit_pushr_i(r0)			jit_pushr_i(r0)
#define jit_popr_i(r0)			jit_popr_i(r0)
#if __WORDSIZE == 32
__jit_inline void
jit_movr_i(int rd, int r0)
{
    if (rd != r0)
	MOVLrr(r0, rd);
}

__jit_inline void
jit_pushr_i(int r0)
{
    PUSHLr(r0);
}

__jit_inline void
jit_popr_i(int r0)
{
    POPLr(r0);
}
#else
#  define jit_movr_l(rd, r0)		jit_movr_i(rd, r0)
#  define jit_movr_ul(rd, r0)		jit_movr_i(rd, r0)
#  define jit_movr_p(rd, r0)		jit_movr_i(rd, r0)
__jit_inline void
jit_movr_i(int rd, int r0)
{
    if (rd != r0)
	MOVQrr(r0, rd);
}

#define jit_pushr_l(rs)			jit_pushr_i(rs)
__jit_inline void
jit_pushr_i(int r0)
{
    PUSHQr(r0);
}

#define jit_popr_l(rs)			jit_popr_i(rs)
__jit_inline void
jit_popr_i(int r0)
{
    POPQr(r0);
}
#endif	/* __WORDSIZE == 32 */

#define jit_patch_abs_long_at(jump, label)				\
	jit_patch_abs_long_at(jump, label)
__jit_inline void
jit_patch_abs_long_at(jit_insn *jump, jit_insn *label)
{
    *(long *)(jump - sizeof(long)) = (long)label;
}

#define jit_patch_rel_int_at(jump, label)				\
	jit_patch_rel_int_at(jump, label)
__jit_inline void
jit_patch_rel_int_at(jit_insn *jump, jit_insn *label)
{
    *(int *)(jump - sizeof(int)) = (int)(long)(label - jump);
}

#define jit_patch_rel_char_at(jump, label)				\
	jit_patch_rel_char_at(jump, label)
__jit_inline void
jit_patch_rel_char_at(jit_insn *jump, jit_insn *label)
{
    *(char *)(jump - 1) = (char)(long)(label - jump);
}

#define jit_patch_movi(address, label)	jit_patch_movi(address, label)
__jit_inline void
jit_patch_movi(jit_insn *address, jit_insn *label)
{
    jit_patch_abs_long_at(address, label);
}

#define jit_jmpi(label)			jit_jmpi(label)
__jit_inline jit_insn *
jit_jmpi(jit_insn *label)
{
    JMPm((_ul)label);
    return (_jit.x.pc);
}

#define jit_jmpr(rs)			jit_jmpr(rs)
__jit_inline void
jit_jmpr(int rs)
{
    JMPsr(rs);
}

/* Stack */
#define jit_retval_i(rd)		jit_retval_i(rd)
__jit_inline void
jit_retval_i(int rd)
{
    jit_movr_i(jit_reg32(rd), _EAX);
}

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

/* Boolean */
__jit_inline void
_jit_cmp_ri32(int rd, int r0, int i0, int code)
{
    int		same;

    if (jit_check8(rd)) {
	same = rd == r0;
	if (!same)
	    XORLrr(rd, rd);
	CMPLir(i0, r0);
	if (same)
	    MOVLir(0, rd);
	SETCCir(code, rd);
    }
    else {
	same = jit_reg32(r0) == _EAX;
	jit_pushr_i(_EAX);
	if (!same)
	    XORLrr(_EAX, _EAX);
	CMPLir(i0, r0);
	if (same)
	    MOVLir(0, _EAX);
	SETCCir(code, _AL);
	MOVLrr(_EAX, rd);
	jit_popr_i(_EAX);
    }
}

__jit_inline void
_jit_test_r32(int rd, int r0, int code)
{
    int		same;

    if (jit_check8(rd)) {
	same = rd == r0;
	if (!same)
	    XORLrr(rd, rd);
	TESTLrr(r0, r0);
	if (same)
	    MOVLir(0, rd);
	SETCCir(code, rd);
    }
    else {
	same = jit_reg32(r0) == _EAX;
	jit_pushr_i(_EAX);
	if (!same)
	    XORLrr(_EAX, _EAX);
	TESTLrr(r0, r0);
	if (same)
	    MOVLir(0, _EAX);
	SETCCir(code, _AL);
	MOVLrr(_EAX, rd);
	jit_popr_i(_EAX);
    }
}

__jit_inline void
_jit_cmp_rr32(int rd, int r0, int r1, int code)
{
    int		same;

    if (jit_check8(rd)) {
	same = rd == r0 || rd == r1;
	if (!same)
	    XORLrr(rd, rd);
	CMPLrr(r1, r0);
	if (same)
	    MOVLir(0, rd);
	SETCCir(code, rd);
    }
    else {
	same = jit_reg32(r0) == _EAX || jit_reg32(r1) == _EAX;
	jit_pushr_i(_EAX);
	if (!same)
	    XORLrr(_EAX, _EAX);
	CMPLrr(r1, r0);
	if (same)
	    MOVLir(0, _EAX);
	SETCCir(code, _AL);
	MOVLrr(_EAX, rd);
	jit_popr_i(_EAX);
    }
}

#define jit_lti_i(rd, r0, i0)		jit_lti_i(rd, r0, i0)
__jit_inline void
jit_lti_i(int rd, int r0, int i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_L);
    else
	_jit_test_r32(rd, r0,		X86_CC_S);
}

#define jit_ltr_i(rd, r0, r1)		jit_ltr_i(rd, r0, r1)
__jit_inline void
jit_ltr_i(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_L);
}

#define jit_lei_i(rd, r0, i0)		jit_lei_i(rd, r0, i0)
__jit_inline void
jit_lei_i(int rd, int r0, int i0)
{
    _jit_cmp_ri32(rd, r0, i0,		X86_CC_LE);
}

#define jit_ler_i(rd, r0, r1)		jit_ler_i(rd, r0, r1)
__jit_inline void
jit_ler_i(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_LE);
}

#define jit_eqi_i(rd, r0, i0)		jit_eqi_i(rd, r0, i0)
__jit_inline void
jit_eqi_i(int rd, int r0, int i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_E);
    else
	_jit_test_r32(rd, r0,		X86_CC_E);
}

#define jit_eqr_i(rd, r0, r1)		jit_eqr_i(rd, r0, r1)
__jit_inline void
jit_eqr_i(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_E);
}

#define jit_gei_i(rd, r0, i0)		jit_gei_i(rd, r0, i0)
__jit_inline void
jit_gei_i(int rd, int r0, int i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_GE);
    else
	_jit_test_r32(rd, r0,		X86_CC_NS);
}

#define jit_ger_i(rd, r0, r1)		jit_ger_i(rd, r0, r1)
__jit_inline void
jit_ger_i(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_GE);
}

#define jit_gti_i(rd, r0, i0)		jit_gti_i(rd, r0, i0)
__jit_inline void
jit_gti_i(int rd, int r0, int i0)
{
    _jit_cmp_ri32(rd, r0, i0,		X86_CC_G);
}

#define jit_gtr_i(rd, r0, r1)		jit_gtr_i(rd, r0, r1)
__jit_inline void
jit_gtr_i(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_G);
}

#define jit_nei_i(rd, r0, i0)		jit_nei_i(rd, r0, i0)
__jit_inline void
jit_nei_i(int rd, int r0, long i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_NE);
    else
	_jit_test_r32(rd, r0,		X86_CC_NE);
}

#define jit_ner_i(rd, r0, r1)		jit_ner_i(rd, r0, r1)
__jit_inline void
jit_ner_i(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_NE);
}

#define jit_lti_ui(rd, r0, i0)		jit_lti_ui(rd, r0, i0)
__jit_inline void
jit_lti_ui(int rd, int r0, unsigned int i0)
{
    _jit_cmp_ri32(rd, r0, i0,		X86_CC_B);
}

#define jit_ltr_ui(rd, r0, r1)		jit_ltr_ui(rd, r0, r1)
__jit_inline void
jit_ltr_ui(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_B);
}

#define jit_lei_ui(rd, r0, i0)		jit_lei_ui(rd, r0, i0)
__jit_inline void
jit_lei_ui(int rd, int r0, unsigned int i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_BE);
    else
	_jit_test_r32(rd, r0,		X86_CC_E);
}

#define jit_ler_ui(rd, r0, r1)		jit_ler_ui(rd, r0, r1)
__jit_inline void
jit_ler_ui(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_BE);
}

#define jit_gei_ui(rd, r0, i0)		jit_gei_ui(rd, r0, i0)
__jit_inline void
jit_gei_ui(int rd, int r0, unsigned int i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_AE);
    else
	_jit_test_r32(rd, r0,		X86_CC_NB);
}

#define jit_ger_ui(rd, r0, r1)		jit_ger_ui(rd, r0, r1)
__jit_inline void
jit_ger_ui(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_AE);
}

#define jit_gti_ui(rd, r0, i0)		jit_gti_ui(rd, r0, i0)
__jit_inline void
jit_gti_ui(int rd, int r0, unsigned int i0)
{
    if (i0)
	_jit_cmp_ri32(rd, r0, i0,	X86_CC_A);
    else
	_jit_test_r32(rd, r0,		X86_CC_NE);
}

#define jit_gtr_ui(rd, r0, r1)		jit_gtr_ui(rd, r0, r1)
__jit_inline void
jit_gtr_ui(int rd, int r0, int r1)
{
    _jit_cmp_rr32(rd, r0, r1,		X86_CC_A);
}

/* Jump */
__jit_inline void
_jit_bcmp_ri32(jit_insn *label, int r0, int i0, int code)
{
    CMPLir(i0, r0);
    JCCim(code, label);
}

__jit_inline void
_jit_btest_r32(jit_insn *label, int r0, int code)
{
    TESTLrr(r0, r0);
    JCCim(code, label);
}

__jit_inline void
_jit_bcmp_rr32(jit_insn *label, int r0, int r1, int code)
{
    CMPLrr(r1, r0);
    JCCim(code, label);
}

#define jit_blti_i(label, r0, i0)	jit_blti_i(label, r0, i0)
__jit_inline jit_insn *
jit_blti_i(jit_insn *label, int r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_L);
    else
	_jit_btest_r32(label, r0,	X86_CC_S);
    return (_jit.x.pc);
}

#define jit_bltr_i(label, r0, r1)	jit_bltr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_i(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_L);
    return (_jit.x.pc);
}

#define jit_blei_i(label, r0, i0)	jit_blei_i(label, r0, i0)
__jit_inline jit_insn *
jit_blei_i(jit_insn *label, int r0, int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_bler_i(label, r0, r1)	jit_bler_i(label, r0, r1)
__jit_inline jit_insn *
jit_bler_i(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_beqi_i(label, r0, i0)	jit_beqi_i(label, r0, i0)
__jit_inline jit_insn *
jit_beqi_i(jit_insn *label, int r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_E);
    else
	_jit_btest_r32(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_beqr_i(label, r0, r1)	jit_beqr_i(label, r0, r1)
__jit_inline jit_insn *
jit_beqr_i(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bgei_i(label, r0, i0)	jit_bgei_i(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_i(jit_insn *label, int r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_GE);
    else
	_jit_btest_r32(label, r0,	X86_CC_NS);
    return (_jit.x.pc);
}

#define jit_bger_i(label, r0, r1)	jit_bger_i(label, r0, r1)
__jit_inline jit_insn *
jit_bger_i(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_GE);
    return (_jit.x.pc);
}

#define jit_bgti_i(label, r0, i0)	jit_bgti_i(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_i(jit_insn *label, int r0, int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bgtr_i(label, r0, r1)	jit_bgtr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_i(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bnei_i(label, r0, i0)	jit_bnei_i(label, r0, i0)
__jit_inline jit_insn *
jit_bnei_i(jit_insn *label, int r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_NE);
    else
	_jit_btest_r32(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bner_i(label, r0, r1)	jit_bner_i(label, r0, r1)
__jit_inline jit_insn *
jit_bner_i(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_blti_ui(label, r0, i0)	jit_blti_ui(label, r0, i0)
__jit_inline jit_insn *
jit_blti_ui(jit_insn *label, int r0, unsigned int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_bltr_ui(label, r0, r1)	jit_bltr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_ui(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_blei_ui(label, r0, i0)	jit_blei_ui(label, r0, i0)
__jit_inline jit_insn *
jit_blei_ui(jit_insn *label, int r0, unsigned int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_BE);
    else
	_jit_btest_r32(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bler_ui(label, r0, r1)	jit_bler_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bler_ui(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_BE);
    return (_jit.x.pc);
}

#define jit_bgei_ui(label, r0, i0)	jit_bgei_ui(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_ui(jit_insn *label, int r0, unsigned int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bger_ui(label, r0, r1)	jit_bger_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bger_ui(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bgti_ui(label, r0, i0)	jit_bgti_ui(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_ui(jit_insn *label, int r0, unsigned int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_A);
    else
	_jit_btest_r32(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bgtr_ui(label, r0, r1)	jit_bgtr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_ui(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_A);
    return (_jit.x.pc);
}

#define jit_boaddi_i(label, r0, i0)	jit_boaddi_i(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_i(jit_insn *label, int r0, int i0)
{
    ADDLir(i0, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddr_i(label, r0, r1)	jit_boaddr_i(label, r0, r1)
__jit_inline jit_insn *
jit_boaddr_i(jit_insn *label, int r0, int r1)
{
    ADDLrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_i(label, r0, i0)	jit_bosubi_i(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_i(jit_insn *label, int r0, int i0)
{
    SUBLir(i0, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubr_i(label, r0, r1)	jit_bosubr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bosubr_i(jit_insn *label, int r0, int r1)
{
    SUBLrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddi_ui(label, r0, i0)	jit_boaddi_ui(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_ui(jit_insn *label, int r0, unsigned int i0)
{
    ADDLir(i0, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_boaddr_ui(label, r0, r1)	jit_boaddr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_boaddr_ui(jit_insn *label, int r0, int r1)
{
    ADDLrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_ui(label, r0, i0)	jit_bosubi_ui(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_ui(jit_insn *label, int r0, unsigned int i0)
{
    SUBLir(i0, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubr_ui(label, r0, r1)	jit_bosubr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bosubr_ui(jit_insn *label, int r0, int r1)
{
    SUBLrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bmsi_i(label, r0, i0)	jit_bmsi_i(label, r0, i0)
__jit_inline jit_insn *
jit_bmsi_i(jit_insn *label, int r0, int i0)
{
    if (jit_check8(r0) && jit_can_zero_extend_char_p(i0))
	TESTBir(i0, jit_reg8(r0));
    /* valid in 64 bits mode */
    else if (jit_can_zero_extend_short_p(i0))
	TESTWir(i0, jit_reg16(r0));
    else
	TESTLir(i0, jit_reg32(r0));
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmsr_i(label, r0, r1)	jit_bmsr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bmsr_i(jit_insn *label, int r0, int r1)
{
    TESTLrr(r1, r0);
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmci_i(label, r0, i0)	jit_bmci_i(label, r0, i0)
__jit_inline jit_insn *
jit_bmci_i(jit_insn *label, int r0, int i0)
{
    if (jit_check8(r0) && jit_can_zero_extend_char_p(i0))
	TESTBir(i0, jit_reg8(r0));
    /* valid in 64 bits mode */
    else if (jit_can_zero_extend_short_p(i0))
	TESTWir(i0, jit_reg16(r0));
    else
	TESTLir(i0, jit_reg32(r0));
    JZm(label);
    return (_jit.x.pc);
}

#define jit_bmcr_i(label, r0, r1)	jit_bmcr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bmcr_i(jit_insn *label, int r0, int r1)
{
    TESTLrr(r1, r0);
    JZm(label);
    return (_jit.x.pc);
}

/* Memory */
#define jit_ntoh_us(rd, r0)		jit_ntoh_us(rd, r0)
__jit_inline void
jit_ntoh_us(int rd, int r0)
{
    jit_movr_i(rd, r0);
    RORWir(8, rd);
}

#define jit_ntoh_ui(rd, r0)		jit_ntoh_ui(rd, r0)
__jit_inline void
jit_ntoh_ui(int rd, int r0)
{
    jit_movr_i(rd, r0);
    BSWAPLr(rd);
}

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

