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

#define JIT_FP			_RBP
#define JIT_SP			_RSP
#define JIT_RET			_RAX

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

#define jit_movr_i(r0, r1)		jit_movr_i(r0, r1)
#define jit_pushr_i(r0)			jit_pushr_i(r0)
#define jit_pushi_i(i0)			jit_pushi_i(i0)
#define jit_popr_i(r0)			jit_popr_i(r0)
#if __WORDSIZE == 32
__jit_inline void
jit_movr_i(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1)
	MOVLrr(r1, r0);
}

__jit_inline void
jit_pushr_i(jit_gpr_t r0)
{
    PUSHLr(r0);
}

__jit_inline void
jit_pushi_i(int i0)
{
    PUSHLi(i0);
}

__jit_inline void
jit_popr_i(jit_gpr_t r0)
{
    POPLr(r0);
}
#else
#  define jit_movr_l(r0, r1)		jit_movr_i(r0, r1)
#  define jit_movr_ul(r0, r1)		jit_movr_i(r0, r1)
#  define jit_movr_p(r0, r1)		jit_movr_i(r0, r1)
__jit_inline void
jit_movr_i(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1)
	MOVQrr(r1, r0);
}

#define jit_pushr_l(r0)			jit_pushr_i(r0)
__jit_inline void
jit_pushr_i(jit_gpr_t r0)
{
    PUSHQr(r0);
}

__jit_inline void
jit_pushi_i(long i0)
{
    PUSHQi(i0);
}

#define jit_popr_l(r0)			jit_popr_i(r0)
__jit_inline void
jit_popr_i(jit_gpr_t r0)
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

#define jit_jmpr(r0)			jit_jmpr(r0)
__jit_inline void
jit_jmpr(jit_gpr_t r0)
{
    JMPsr(r0);
}

/* Stack */
#define jit_retval_i(r0)		jit_retval_i(r0)
__jit_inline void
jit_retval_i(jit_gpr_t r0)
{
    jit_movr_i(r0, _RAX);
}

/* ALU */
#define jit_negr_i(r0, r1)		jit_negr_i(r0, r1)
__jit_inline void
jit_negr_i(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 == r1)
	NEGLr(r0);
    else {
	XORLrr(r0, r0);
	SUBLrr(r1, r0);
    }
}

#define jit_addi_i(r0, r1, i0)		jit_addi_i(r0, r1, i0)
__jit_inline void
jit_addi_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 == 0)
	jit_movr_i(r0, r1);
    else if (i0 == 1) {
	jit_movr_i(r0, r1);
	INCLr(r0);
    }
    else if (i0 == -1) {
	jit_movr_i(r0, r1);
	DECLr(r0);
    }
    else if (r0 == r1)
	ADDLir(i0, r0);
    else
	LEALmr(i0, r1, 0, 0, r0);
}

#define jit_addr_i(r0, r1, r2)		jit_addr_i(r0, r1, r2)
__jit_inline void
jit_addr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1)
	ADDLrr(r2, r0);
    else if (r0 == r2)
	ADDLrr(r1, r0);
    else
	LEALmr(0, r1, r2, 1, r0);
}

#define jit_subr_i(r0, r1, r2)		jit_subr_i(r0, r1, r2)
__jit_inline void
jit_subr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2)
	XORLrr(r0, r0);
    else if (r0 == r2) {
	SUBLrr(r1, r0);
	NEGLr(r0);
    }
    else {
	jit_movr_i(r0, r1);
	SUBLrr(r2, r0);
    }
}

#define jit_addci_ui(r0, r1, i0)	jit_addci_ui(r0, r1, i0)
__jit_inline void
jit_addci_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (r0 == r1)
	ADDLir((int)i0, r0);
    else {
	MOVLir(i0, r0);
	ADDLrr(r1, r0);
    }
}

#define jit_addcr_ui(r0, r1, r2)	jit_addcr_ui(r0, r1, r2)
__jit_inline void
jit_addcr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r2)
	ADDLrr(r1, r0);
    else if (r0 == r1)
	ADDLrr(r2, r0);
    else {
	MOVLrr(r1, r0);
	ADDLrr(r2, r0);
    }
}

#define jit_addxi_ui(r0, r1, i0)	jit_addxi_ui(r0, r1, i0)
__jit_inline void
jit_addxi_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (r0 == r1)
	ADCLir((int)i0, r0);
    else {
	MOVLir(i0, r0);
	ADCLrr(r1, r0);
    }
}

#define jit_addxr_ui(r0, r1, r2)	jit_addxr_ui(r0, r1, r2)
__jit_inline void
jit_addxr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2) {
    if (r0 == r2)
	ADCLrr(r1, r0);
    else if (r0 == r1)
	ADCLrr(r2, r0);
    else {
	MOVLrr(r1, r0);
	ADCLrr(r2, r0);
    }
}

#define jit_subci_ui(r0, r1, i0)	jit_subci_ui(r0, r1, i0)
__jit_inline void
jit_subci_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (r0 == r1)
	SUBLir((int)i0, r0);
    else {
	MOVLir(i0, r0);
	SUBLrr(r1, r0);
    }
}

#define jit_subcr_ui(r0, r1, r2)	jit_subcr_ui(r0, r1, r2)
__jit_inline void
jit_subcr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1)
	SUBLrr(r2, r0);
    else {
	MOVLrr(r1, r0);
	SUBLir(r2, r0);
    }
}

#define jit_subxi_ui(r0, r1, i0)	jit_subxi_ui(r0, r1, i0)
__jit_inline void
jit_subxi_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (r0 == r1)
	SBBLir((int)i0, r0);
    else {
	MOVLir(i0, r0);
	SBBLrr(r1, r0);
    }
}

#define jit_subxr_ui(r0, r1, r2)	jit_subxr_ui(r0, r1, r2)
__jit_inline void
jit_subxr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1)
	SBBLrr(r2, r0);
    else {
	MOVLrr(r1, r0);
	SBBLir(r2, r0);
    }
}

#define jit_andi_i(r0, r1, i0)		jit_andi_i(r0, r1, i0)
__jit_inline void
jit_andi_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 == 0)
	XORLrr(r0, r0);
    else {
	jit_movr_i(r0, r1);
	if (i0 != -1)
	    ANDLir(i0, r0);
    }
}

#define jit_andr_i(r0, r1, r2)		jit_andr_i(r0, r1, r2)
__jit_inline void
jit_andr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2)
	jit_movr_i(r0, r1);
    else if (r0 == r1)
	ANDLrr(r2, r0);
    else if (r0 == r2)
	ANDLrr(r1, r0);
    else {
	MOVLrr(r1, r0);
	ANDLrr(r2, r0);
    }
}

#define jit_ori_i(r0, r1, i0)		jit_ori_i(r0, r1, i0)
__jit_inline void
jit_ori_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 == 0)
	jit_movr_i(r0, r1);
    else if (i0 == -1)
	MOVLir(0xffffffff, r0);
    else {
	jit_movr_i(r0, r1);
	if (jit_check8(r0) && jit_can_sign_extend_char_p(i0))
	    ORBir(i0, r0);
#if __WORDSIZE == 32
	else if (jit_can_sign_extend_short_p(i0))
	    ORWir(i0, r0);
#endif
	else
	    ORLir(i0, r0);
    }
}

#define jit_orr_i(r0, r1, r2)		jit_orr_i(r0, r1, r2)
__jit_inline void
jit_orr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2)
	jit_movr_i(r0, r1);
    else if (r0 == r1)
	ORLrr(r2, r0);
    else if (r0 == r2)
	ORLrr(r1, r0);
    else {
	MOVLrr(r1, r0);
	ORLrr(r2, r0);
    }
}

#define jit_xori_i(r0, r1, i0)		jit_xori_i(r0, r1, i0)
__jit_inline void
jit_xori_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 == 0)
	jit_movr_i(r0, r1);
    else if (i0 == -1) {
	jit_movr_i(r0, r1);
	NOTLr(r0);
    }
    else {
	jit_movr_i(r0, r1);
	if (jit_check8(r0) && jit_can_sign_extend_char_p(i0))
	    XORBir(i0, r0);
#if __WORDSIZE == 32
	else if (jit_can_sign_extend_short_p(i0))
	    XORWir(i0, r0);
#endif
	else
	    XORLir(i0, r0);
    }
}

#define jit_xorr_i(r0, r1, r2)		jit_xorr_i(r0, r1, r2)
__jit_inline void
jit_xorr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2) {
	if (r0 != r1)
	    MOVLrr(r1, r0);
	else
	    XORLrr(r0, r0);
    }
    else if (r0 == r1)
	XORLrr(r2, r0);
    else if (r0 == r2)
	XORLrr(r1, r0);
    else {
	MOVLrr(r1, r0);
	XORLrr(r2, r0);
    }
}

/*  Instruction format is:
 *	imul reg32/mem32
 *  and the result is stored in %edx:%eax
 *  %eax = low 32 bits
 *  %edx = high 32 bits
 */
__jit_inline void
jit_muli_i_(jit_gpr_t r0, int i0)
{
    if (r0 == _RAX) {
	MOVLir((unsigned)i0, _RDX);
	IMULLr(_RDX);
    }
    else {
	MOVLir((unsigned)i0, _RAX);
	IMULLr(r0);
    }
}

#define jit_hmuli_i(r0, r1, i0)		jit_hmuli_i(r0, r1, i0)
__jit_inline void
jit_hmuli_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (r0 == _RDX) {
	jit_pushr_i(_RAX);
	jit_muli_i_(r1, i0);
	jit_popr_i(_RAX);
    }
    else if (r0 == _RAX) {
	jit_pushr_i(_RDX);
	jit_muli_i_(r1, i0);
	MOVLrr(_RDX, _RAX);
	jit_popr_i(_RDX);
    }
    else {
	jit_pushr_i(_RDX);
	jit_pushr_i(_RAX);
	jit_muli_i_(r1, i0);
	MOVLrr(_RDX, r0);
	jit_popr_i(_RAX);
	jit_popr_i(_RDX);
    }
}

__jit_inline void
jit_mulr_i_(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r1 == _RAX)
	IMULLr(r0);
    else if (r0 == _RAX)
	IMULLr(r1);
    else {
	MOVLrr(r1, _RAX);
	IMULLr(r0);
    }
}

#define jit_hmulr_i(r0, r1, r2)		jit_hmulr_i(r0, r1, r2)
__jit_inline void
jit_hmulr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == _RDX) {
	jit_pushr_i(_RAX);
	jit_mulr_i_(r1, r2);
	jit_popr_i(_RAX);
    }
    else if (r0 == _RAX) {
	jit_pushr_i(_RDX);
	jit_mulr_i_(r1, r2);
	MOVLrr(_RDX, _RAX);
	jit_popr_i(_RDX);
    }
    else {
	jit_pushr_i(_RDX);
	jit_pushr_i(_RAX);
	jit_mulr_i_(r1, r2);
	MOVLrr(_RDX, r0);
	jit_popr_i(_RAX);
	jit_popr_i(_RDX);
    }
}

/*  Instruction format is:
 *	mul reg32/mem32
 *  and the result is stored in %edx:%eax
 *  %eax = low 32 bits
 *  %edx = high 32 bits
 */
__jit_inline void
jit_muli_ui_(jit_gpr_t r0, unsigned int i0)
{
    if (r0 == _RAX) {
	MOVLir(i0, _RDX);
	MULLr(_RDX);
    }
    else {
	MOVLir(i0, _RAX);
	MULLr(r0);
    }
}

#define jit_hmuli_ui(r0, r1, i0)	jit_hmuli_ui(r0, r1, i0)
__jit_inline void
jit_hmuli_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (r0 == _RDX) {
	jit_pushr_i(_RAX);
	jit_muli_ui_(r1, i0);
	jit_popr_i(_RAX);
    }
    else if (r0 == _RAX) {
	jit_pushr_i(_RDX);
	jit_muli_ui_(r1, i0);
	MOVLrr(_RDX, _RAX);
	jit_popr_i(_RDX);
    }
    else {
	jit_pushr_i(_RDX);
	jit_pushr_i(_RAX);
	jit_muli_ui_(r1, i0);
	MOVLrr(_RDX, r0);
	jit_popr_i(_RAX);
	jit_popr_i(_RDX);
    }
}

__jit_inline void
jit_mulr_ui_(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r1 == _RAX)
	MULLr(r0);
    else if (r0 == _RAX)
	MULLr(r1);
    else {
	MOVLrr(r1, _RAX);
	MULLr(r0);
    }
}

#define jit_hmulr_ui(r0, r1, r2)	jit_hmulr_ui(r0, r1, r2)
__jit_inline void
jit_hmulr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == _RDX) {
	jit_pushr_i(_RAX);
	jit_mulr_ui_(r1, r2);
	jit_popr_i(_RAX);
    }
    else if (r0 == _RAX) {
	jit_pushr_i(_RDX);
	jit_mulr_ui_(r1, r2);
	MOVLrr(_RDX, _RAX);
	jit_popr_i(_RDX);
    }
    else {
	jit_pushr_i(_RDX);
	jit_pushr_i(_RAX);
	jit_mulr_ui_(r1, r2);
	MOVLrr(_RDX, r0);
	jit_popr_i(_RAX);
	jit_popr_i(_RDX);
    }
}

#define jit_muli_i(r0, r1, i0)		jit_muli_i(r0, r1, i0)
#define jit_muli_ui(r0, r1, i0)		jit_muli_i(r0, r1, i0)
__jit_inline void
jit_muli_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 == 0)
	XORLrr(r0, r0);
    else if (i0 == 1)
	jit_movr_i(r0, r1);
    else if (i0 == -1)
	jit_negr_i(r0, r1);
    else if (jit_can_sign_extend_char_p(i0))
	IMULBLLirr(i0, r1, r0);
    else
	IMULLLLirr(i0, r1, r0);
}

#define jit_mulr_i(r0, r1, r2)		jit_mulr_i(r0, r1, r2)
#define jit_mulr_ui(r0, r1, r2)		jit_mulr_i(r0, r1, r2)
__jit_inline void
jit_mulr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1)
	IMULLrr(r2, r0);
    else if (r0 == r2)
	IMULLrr(r1, r0);
    else {
	MOVLrr(r1, r0);
	IMULLrr(r2, r0);
    }
}

__jit_inline void
jit_divi_i_(jit_gpr_t r0, jit_gpr_t r1, int i0, int is_signed, int is_divide)
{
    jit_gpr_t	div;
    int		pop;

    if (r0 != _RDX)
	jit_pushr_i(_RDX);
    if (r0 != _RAX)
	jit_pushr_i(_RAX);

    if (r0 == _RAX || r0 == _RDX) {
	div = _RCX;
	pop = 1;
    }
    else {
	div = r0;
	pop = 0;
    }

    if (pop)
	jit_pushr_i(div);
    if (r1 != _RAX)
	MOVLrr(r1, _RAX);
    MOVLir((unsigned)i0, div);

    if (is_signed) {
	CDQ_();
	IDIVLr(div);
    }
    else {
	XORLrr(_RDX, _RDX);
	DIVLr(div);
    }

    if (pop)
	jit_popr_i(div);

    if (r0 != _RAX) {
	if (is_divide)
	    MOVLrr(_RAX, r0);
	jit_popr_i(_RAX);
    }
    if (r0 != _RDX) {
	if (!is_divide)
	    MOVLrr(_RDX, r0);
	jit_popr_i(_RDX);
    }
}

__jit_inline void
jit_divr_i_(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2,
	    int is_signed, int is_divide)
{
    jit_gpr_t	div;
    jit_gpr_t		pop;

    if (r0 != _RDX)
	jit_pushr_i(_RDX);
    if (r0 != _RAX)
	jit_pushr_i(_RAX);

    if (r2 == _RAX) {
	if (r0 == _RAX || r0 == _RDX) {
	    div = r1 == _RCX ? _RBX : _RCX;
	    jit_pushr_i(div);
	    MOVLrr(_RAX, div);
	    if (r1 != _RAX)
		MOVLrr(r1, _RAX);
	    pop = 1;
	}
	else {
	    if (r0 == r1)
		XCHGLrr(_RAX, r0);
	    else {
		if (r0 != _RAX)
		    MOVLrr(_RAX, r0);
		if (r1 != _RAX)
		    MOVLrr(r1, _RAX);
	    }
	    div = r0;
	    pop = 0;
	}
    }
    else if (r2 == _RDX) {
	if (r0 == _RAX || r0 == _RDX) {
	    div = r1 == _RCX ? _RBX : _RCX;
	    jit_pushr_i(div);
	    MOVLrr(_RDX, div);
	    if (r1 != _RAX)
		MOVLrr(r1, _RAX);
	    pop = 1;
	}
	else {
	    if (r1 != _RAX)
		MOVLrr(r1, _RAX);
	    MOVLrr(_RDX, r0);
	    div = r0;
	    pop = 0;
	}
    }
    else {
	if (r1 != _RAX)
	    MOVLrr(r1, _RAX);
	div = r2;
	pop = 0;
    }

    if (is_signed) {
	CDQ_();
	IDIVLr(div);
    }
    else {
	XORLrr(_RDX, _RDX);
	DIVLr(div);
    }

    if (pop)
	jit_popr_i(div);
    if (r0 != _RAX) {
	if (is_divide)
	    MOVLrr(_RAX, r0);
	jit_popr_i(_RAX);
    }
    if (r0 != _RDX) {
	if (!is_divide)
	    MOVLrr(_RDX, r0);
	jit_popr_i(_RDX);
    }
}

#define jit_divi_i(r0, r1, i0)		jit_divi_i(r0, r1, i0)
__jit_inline void
jit_divi_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_divi_i_(r0, r1, i0, 1, 1);
}

#define jit_divr_i(r0, r1, r2)		jit_divr_i(r0, r1, r2)
__jit_inline void
jit_divr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_divr_i_(r0, r1, r2, 1, 1);
}

#define jit_divi_ui(r0, r1, i0)		jit_divi_ui(r0, r1, i0)
__jit_inline void
jit_divi_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    jit_divi_i_(r0, r1, i0, 0, 1);
}

#define jit_divr_ui(r0, r1, r2)		jit_divr_ui(r0, r1, r2)
__jit_inline void
jit_divr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_divr_i_(r0, r1, r2, 0, 1);
}

#define jit_modi_i(r0, r1, i0)		jit_modi_i(r0, r1, i0)
__jit_inline void
jit_modi_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_divi_i_(r0, r1, i0, 1, 0);
}

#define jit_modr_i(r0, r1, r2)		jit_modr_i(r0, r1, r2)
__jit_inline void
jit_modr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_divr_i_(r0, r1, r2, 1, 0);
}

#define jit_modi_ui(r0, r1, i0)		jit_modi_ui(r0, r1, i0)
__jit_inline void
jit_modi_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    jit_divi_i_(r0, r1, i0, 0, 0);
}

#define jit_modr_ui(r0, r1, r2)		jit_modr_ui(r0, r1, r2)
__jit_inline void
jit_modr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_divr_i_(r0, r1, r2, 0, 0);
}

/* Shifts */
__jit_inline void
_jit_shift32(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, int code)
{
    jit_gpr_t	lsh;

    if (r0 != _RCX && r2 != _RCX)
	jit_pushr_i(_RCX);

    if (r1 == _RCX) {
	if (r0 != _RCX) {
	    if (r0 == r2)
		XCHGLrr(_RCX, r0);
	    else {
		MOVLrr(_RCX, r0);
		MOVLrr(r2, _RCX);
	    }
	    lsh = r0;
	}
	/* r0 == _RCX */
	else if (r2 == _RCX) {
	    jit_pushr_i(_RAX);
	    MOVLrr(_RCX, _RAX);
	    lsh = _RAX;
	}
	else {
	    jit_pushr_i(r2);
	    XCHGLrr(_RCX, r2);
	    lsh = r2;
	}
    }
    /* r1 != _RCX */
    else if (r0 == _RCX) {
	jit_pushr_i(r1);
	if (r2 != _RCX)
	    MOVLrr(r2, _RCX);
	lsh = r1;
    }
    else {
	if (r2 != _RCX)
	    MOVLrr(r2, _RCX);
	if (r0 != r1)
	    MOVLrr(r1, r0);
	lsh = r0;
    }

    _ROTSHILrr(code, _RCX, lsh);

    if (lsh != r0) {
	MOVLrr(lsh, r0);
	jit_popr_i(lsh);
    }

    if (r0 != _RCX && r2 != _RCX)
	jit_popr_i(_RCX);
}

#define jit_lshi_i(r0, r1, i0)		jit_lshi_i(r0, r1, i0)
__jit_inline void
jit_lshi_i(jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    if (i0 == 0)
	jit_movr_i(r0, r1);
    else if (i0 <= 3)
	LEALmr(0, 0, r1, 1 << i0, r0);
    else {
	jit_movr_i(r0, r1);
	SHLLir(i0, r0);
    }
}

#define jit_lshr_i(r0, r1, r2)		jit_lshr_i(r0, r1, r2)
__jit_inline void
jit_lshr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_shift32(r0, r1, r2, X86_SHL);
}

#define jit_rshi_i(r0, r1, i0)		jit_rshi_i(r0, r1, i0)
__jit_inline void
jit_rshi_i(jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    jit_movr_i(r0, r1);
    if (i0)
	SARLir(i0, r0);
}

#define jit_rshr_i(r0, r1, r2)		jit_rshr_i(r0, r1, r2)
__jit_inline void
jit_rshr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_shift32(r0, r1, r2, X86_SAR);
}

#define jit_rshi_ui(r0, r1, i0)		jit_rshi_ui(r0, r1, i0)
__jit_inline void
jit_rshi_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    jit_movr_i(r0, r1);
    if (i0)
	SHRLir(i0, r0);
}

#define jit_rshr_ui(r0, r1, r2)		jit_rshr_ui(r0, r1, r2)
__jit_inline void
jit_rshr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_shift32(r0, r1, r2, X86_SHR);
}

/* Boolean */
__jit_inline void
_jit_cmp_ri32(jit_gpr_t r0, jit_gpr_t r1, int i0, int code)
{
    int		op;
    jit_gpr_t	reg;

    if (jit_check8(r0)) {
	if (!(op = r0 == r1))
	    XORLrr(r0, r0);
	CMPLir(i0, r1);
	if (op)
	    MOVLir(0, r0);
	SETCCir(code, r0);
    }
    else {
	reg = r1 == _RAX ? _RDX : _RAX;
	MOVLrr(reg, r0);
	XORLrr(reg, reg);
	CMPLir(i0, r1);
	SETCCir(code, reg);
	XCHGLrr(reg, r0);
    }
}

__jit_inline void
_jit_test_r32(jit_gpr_t r0, jit_gpr_t r1, int code)
{
    int		op;
    jit_gpr_t	reg;

    if (jit_check8(r0)) {
	if (!(op = r0 == r1))
	    XORLrr(r0, r0);
	TESTLrr(r1, r1);
	if (op)
	    MOVLir(0, r0);
	SETCCir(code, r0);
    }
    else {
	reg = r1 == _RAX ? _RDX : _RAX;
	MOVLrr(reg, r0);
	XORLrr(reg, reg);
	TESTLrr(r1, r1);
	SETCCir(code, reg);
	XCHGLrr(reg, r0);
    }
}

__jit_inline void
_jit_cmp_rr32(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, int code)
{
    int		op;
    jit_gpr_t	reg;

    if (jit_check8(r0)) {
	if (!(op = r0 == r1 || r0 == r2))
	    XORLrr(r0, r0);
	CMPLrr(r2, r1);
	if (op)
	    MOVLir(0, r0);
	SETCCir(code, r0);
    }
    else {
	if (r1 == _RAX || r2 == _RAX) {
	    if (r1 == _RDX || r2 == _RDX)
		reg = _RCX;
	    else
		reg = _RDX;
	}
	else
	    reg = _RAX;
	MOVLrr(reg, r0);
	XORLrr(reg, reg);
	CMPLrr(r2, r1);
	SETCCir(code, reg);
	XCHGLrr(reg, r0);
    }
}

#define jit_lti_i(r0, r1, i0)		jit_lti_i(r0, r1, i0)
__jit_inline void
jit_lti_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_L);
    else
	_jit_test_r32(r0, r1,		X86_CC_S);
}

#define jit_ltr_i(r0, r1, r2)		jit_ltr_i(r0, r1, r2)
__jit_inline void
jit_ltr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_L);
}

#define jit_lei_i(r0, r1, i0)		jit_lei_i(r0, r1, i0)
__jit_inline void
jit_lei_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    _jit_cmp_ri32(r0, r1, i0,		X86_CC_LE);
}

#define jit_ler_i(r0, r1, r2)		jit_ler_i(r0, r1, r2)
__jit_inline void
jit_ler_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_LE);
}

#define jit_eqi_i(r0, r1, i0)		jit_eqi_i(r0, r1, i0)
__jit_inline void
jit_eqi_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_E);
    else
	_jit_test_r32(r0, r1,		X86_CC_E);
}

#define jit_eqr_i(r0, r1, r2)		jit_eqr_i(r0, r1, r2)
__jit_inline void
jit_eqr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_E);
}

#define jit_gei_i(r0, r1, i0)		jit_gei_i(r0, r1, i0)
__jit_inline void
jit_gei_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_GE);
    else
	_jit_test_r32(r0, r1,		X86_CC_NS);
}

#define jit_ger_i(r0, r1, r2)		jit_ger_i(r0, r1, r2)
__jit_inline void
jit_ger_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_GE);
}

#define jit_gti_i(r0, r1, i0)		jit_gti_i(r0, r1, i0)
__jit_inline void
jit_gti_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    _jit_cmp_ri32(r0, r1, i0,		X86_CC_G);
}

#define jit_gtr_i(r0, r1, r2)		jit_gtr_i(r0, r1, r2)
__jit_inline void
jit_gtr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_G);
}

#define jit_nei_i(r0, r1, i0)		jit_nei_i(r0, r1, i0)
__jit_inline void
jit_nei_i(jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_NE);
    else
	_jit_test_r32(r0, r1,		X86_CC_NE);
}

#define jit_ner_i(r0, r1, r2)		jit_ner_i(r0, r1, r2)
__jit_inline void
jit_ner_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_NE);
}

#define jit_lti_ui(r0, r1, i0)		jit_lti_ui(r0, r1, i0)
__jit_inline void
jit_lti_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    _jit_cmp_ri32(r0, r1, i0,		X86_CC_B);
}

#define jit_ltr_ui(r0, r1, r2)		jit_ltr_ui(r0, r1, r2)
__jit_inline void
jit_ltr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_B);
}

#define jit_lei_ui(r0, r1, i0)		jit_lei_ui(r0, r1, i0)
__jit_inline void
jit_lei_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_BE);
    else
	_jit_test_r32(r0, r1,		X86_CC_E);
}

#define jit_ler_ui(r0, r1, r2)		jit_ler_ui(r0, r1, r2)
__jit_inline void
jit_ler_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_BE);
}

#define jit_gei_ui(r0, r1, i0)		jit_gei_ui(r0, r1, i0)
__jit_inline void
jit_gei_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_AE);
    else
	_jit_test_r32(r0, r1,		X86_CC_NB);
}

#define jit_ger_ui(r0, r1, r2)		jit_ger_ui(r0, r1, r2)
__jit_inline void
jit_ger_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_AE);
}

#define jit_gti_ui(r0, r1, i0)		jit_gti_ui(r0, r1, i0)
__jit_inline void
jit_gti_ui(jit_gpr_t r0, jit_gpr_t r1, unsigned int i0)
{
    if (i0)
	_jit_cmp_ri32(r0, r1, i0,	X86_CC_A);
    else
	_jit_test_r32(r0, r1,		X86_CC_NE);
}

#define jit_gtr_ui(r0, r1, r2)		jit_gtr_ui(r0, r1, r2)
__jit_inline void
jit_gtr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr32(r0, r1, r2,		X86_CC_A);
}

/* Jump */
__jit_inline void
_jit_bcmp_ri32(jit_insn *label, jit_gpr_t r0, int i0, int code)
{
    CMPLir(i0, r0);
    JCCim(code, label);
}

__jit_inline void
_jit_btest_r32(jit_insn *label, jit_gpr_t r0, int code)
{
    TESTLrr(r0, r0);
    JCCim(code, label);
}

__jit_inline void
_jit_bcmp_rr32(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1, int code)
{
    CMPLrr(r1, r0);
    JCCim(code, label);
}

#define jit_blti_i(label, r0, i0)	jit_blti_i(label, r0, i0)
__jit_inline jit_insn *
jit_blti_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_L);
    else
	_jit_btest_r32(label, r0,	X86_CC_S);
    return (_jit.x.pc);
}

#define jit_bltr_i(label, r0, r1)	jit_bltr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_L);
    return (_jit.x.pc);
}

#define jit_blei_i(label, r0, i0)	jit_blei_i(label, r0, i0)
__jit_inline jit_insn *
jit_blei_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_bler_i(label, r0, r1)	jit_bler_i(label, r0, r1)
__jit_inline jit_insn *
jit_bler_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_beqi_i(label, r0, i0)	jit_beqi_i(label, r0, i0)
__jit_inline jit_insn *
jit_beqi_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_E);
    else
	_jit_btest_r32(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_beqr_i(label, r0, r1)	jit_beqr_i(label, r0, r1)
__jit_inline jit_insn *
jit_beqr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bgei_i(label, r0, i0)	jit_bgei_i(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_GE);
    else
	_jit_btest_r32(label, r0,	X86_CC_NS);
    return (_jit.x.pc);
}

#define jit_bger_i(label, r0, r1)	jit_bger_i(label, r0, r1)
__jit_inline jit_insn *
jit_bger_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_GE);
    return (_jit.x.pc);
}

#define jit_bgti_i(label, r0, i0)	jit_bgti_i(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bgtr_i(label, r0, r1)	jit_bgtr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bnei_i(label, r0, i0)	jit_bnei_i(label, r0, i0)
__jit_inline jit_insn *
jit_bnei_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_NE);
    else
	_jit_btest_r32(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bner_i(label, r0, r1)	jit_bner_i(label, r0, r1)
__jit_inline jit_insn *
jit_bner_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_blti_ui(label, r0, i0)	jit_blti_ui(label, r0, i0)
__jit_inline jit_insn *
jit_blti_ui(jit_insn *label, jit_gpr_t r0, unsigned int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_bltr_ui(label, r0, r1)	jit_bltr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_ui(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_blei_ui(label, r0, i0)	jit_blei_ui(label, r0, i0)
__jit_inline jit_insn *
jit_blei_ui(jit_insn *label, jit_gpr_t r0, unsigned int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_BE);
    else
	_jit_btest_r32(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bler_ui(label, r0, r1)	jit_bler_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bler_ui(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_BE);
    return (_jit.x.pc);
}

#define jit_bgei_ui(label, r0, i0)	jit_bgei_ui(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_ui(jit_insn *label, jit_gpr_t r0, unsigned int i0)
{
    _jit_bcmp_ri32(label, r0, i0,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bger_ui(label, r0, r1)	jit_bger_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bger_ui(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bgti_ui(label, r0, i0)	jit_bgti_ui(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_ui(jit_insn *label, jit_gpr_t r0, unsigned int i0)
{
    if (i0)
	_jit_bcmp_ri32(label, r0, i0,	X86_CC_A);
    else
	_jit_btest_r32(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bgtr_ui(label, r0, r1)	jit_bgtr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_ui(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr32(label, r0, r1,	X86_CC_A);
    return (_jit.x.pc);
}

#define jit_boaddi_i(label, r0, i0)	jit_boaddi_i(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    ADDLir(i0, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddr_i(label, r0, r1)	jit_boaddr_i(label, r0, r1)
__jit_inline jit_insn *
jit_boaddr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    ADDLrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_i(label, r0, i0)	jit_bosubi_i(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    SUBLir(i0, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubr_i(label, r0, r1)	jit_bosubr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bosubr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    SUBLrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddi_ui(label, r0, i0)	jit_boaddi_ui(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_ui(jit_insn *label, jit_gpr_t r0, unsigned int i0)
{
    ADDLir((int)i0, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_boaddr_ui(label, r0, r1)	jit_boaddr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_boaddr_ui(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    ADDLrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_ui(label, r0, i0)	jit_bosubi_ui(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_ui(jit_insn *label, jit_gpr_t r0, unsigned int i0)
{
    SUBLir((int)i0, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubr_ui(label, r0, r1)	jit_bosubr_ui(label, r0, r1)
__jit_inline jit_insn *
jit_bosubr_ui(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    SUBLrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bmsi_i(label, r0, i0)	jit_bmsi_i(label, r0, i0)
__jit_inline jit_insn *
jit_bmsi_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    if (jit_check8(r0) && jit_can_zero_extend_char_p(i0))
	TESTBir(i0, r0);
    /* valid in 64 bits mode */
    else if (jit_can_zero_extend_short_p(i0))
	TESTWir(i0, r0);
    else
	TESTLir(i0, r0);
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmsr_i(label, r0, r1)	jit_bmsr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bmsr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    TESTLrr(r1, r0);
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmci_i(label, r0, i0)	jit_bmci_i(label, r0, i0)
__jit_inline jit_insn *
jit_bmci_i(jit_insn *label, jit_gpr_t r0, int i0)
{
    if (jit_check8(r0) && jit_can_zero_extend_char_p(i0))
	TESTBir(i0, r0);
    /* valid in 64 bits mode */
    else if (jit_can_zero_extend_short_p(i0))
	TESTWir(i0, r0);
    else
	TESTLir(i0, r0);
    JZm(label);
    return (_jit.x.pc);
}

#define jit_bmcr_i(label, r0, r1)	jit_bmcr_i(label, r0, r1)
__jit_inline jit_insn *
jit_bmcr_i(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    TESTLrr(r1, r0);
    JZm(label);
    return (_jit.x.pc);
}

/* Memory */
#define jit_ntoh_us(r0, r1)		jit_ntoh_us(r0, r1)
__jit_inline void
jit_ntoh_us(jit_gpr_t r0, jit_gpr_t r1)
{
    jit_movr_i(r0, r1);
    RORWir(8, r0);
}

#define jit_ntoh_ui(r0, r1)		jit_ntoh_ui(r0, r1)
__jit_inline void
jit_ntoh_ui(jit_gpr_t r0, jit_gpr_t r1)
{
    jit_movr_i(r0, r1);
    BSWAPLr(r0);
}

#define jit_extr_c_i(r0, r1)		jit_extr_c_i(r0, r1)
__jit_inline void
jit_extr_c_i(jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t	rep;

    if (jit_check8(r1))
	MOVSBLrr(r1, r0);
    else {
	if (r0 == _RAX)
	    rep = _RDX;
	else
	    rep = _RAX;
	if (r0 != r1)
	    XCHGLrr(rep, r1);
	else {
	    jit_pushr_i(rep);
	    MOVLrr(r1, rep);
	}
	MOVSBLrr(rep, r0);
	if (r0 != r1)
	    XCHGLrr(rep, r1);
	else
	    jit_popr_i(rep);
    }
}

#define jit_extr_c_ui(r0, r1)		jit_extr_c_ui(r0, r1)
__jit_inline void
jit_extr_c_ui(jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t	rep;

    if (jit_check8(r1))
	MOVZBLrr(r1, r0);
    else {
	if (r0 == _RAX)
	    rep = _RDX;
	else
	    rep = _RAX;
	if (r0 != r1)
	    XCHGLrr(rep, r1);
	else {
	    jit_pushr_i(rep);
	    MOVLrr(r1, rep);
	}
	MOVZBLrr(rep, r0);
	if (r0 != r1)
	    XCHGLrr(rep, r1);
	else
	    jit_popr_i(rep);
    }
}

#define jit_extr_s_i(r0, r1)		jit_extr_s_i(r0, r1)
__jit_inline void
jit_extr_s_i(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSWLrr(r1, r0);
}

#define jit_extr_s_ui(r0, r1)		jit_extr_s_ui(r0, r1)
__jit_inline void
jit_extr_s_ui(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVZWLrr(r1, r0);
}

#define jit_ldr_uc(r0, r1)		jit_ldr_uc(r0, r1)
__jit_inline void
jit_ldr_uc(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVZBLmr(0, r1, 0, 0, r0);
}

#define jit_ldxr_uc(r0, r1, r2)		jit_ldxr_uc(r0, r1, r2)
__jit_inline void
jit_ldxr_uc(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVZBLmr(0, r1, r2, 1, r0);
}

#define jit_ldr_us(r0, r1)		jit_ldr_us(r0, r1)
__jit_inline void
jit_ldr_us(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVZWLmr(0, r1, 0, 0, r0);
}

#define jit_ldxr_us(r0, r1, r2)		jit_ldxr_us(r0, r1, r2)
__jit_inline void
jit_ldxr_us(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVZWLmr(0, r1, r2, 1, r0);
}

#define jit_str_s(r0, r1)		jit_str_s(r0, r1)
__jit_inline void
jit_str_s(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVWrm(r1, 0, r0, 0, 0);
}

#define jit_stxr_s(r0, r1, r2)		jit_stxr_s(r0, r1, r2)
__jit_inline void
jit_stxr_s(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVWrm(r2, 0, r0, r1, 1);
}

#define jit_str_i(r0, r1)		jit_str_i(r0, r1)
__jit_inline void
jit_str_i(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLrm(r1, 0, r0, 0, 0);
}

#define jit_stxr_i(r0, r1, r2)		jit_stxr_i(r0, r1, r2)
__jit_inline void
jit_stxr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVLrm(r2, 0, r0, r1, 1);
}

/* Extra */
#define jit_nop				jit_nop
__jit_inline void
jit_nop(void)
{
    NOP_();
}

#define jit_align(n) 			jit_align(n)
__jit_inline void
jit_align(int n)
{
    int		align = ((((_ul)_jit.x.pc) ^ _MASK(4)) + 1) & _MASK(n);

    NOPi(align);
}

#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_X86_64 \
	: defined (__x86_64__)
#include "core-64.h"
#else
#include "core-32.h"
#endif

#endif /* __lightning_core_i386_h */
