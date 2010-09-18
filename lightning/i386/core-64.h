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



#ifndef __lightning_core_h
#define __lightning_core_h

static int
jit_arg_reg_order[] = {
    _RDI, _RSI, _RDX, _RCX, _R8, _R9
};

#define JIT_REXTMP			_R12

/* Number or integer argument registers */
#define JIT_ARG_MAX			6

/* Number of float argument registers */
#define JIT_FP_ARG_MAX			8

#define JIT_R_NUM			3
#define JIT_R(i)			((i) == 0 ? _RAX : _R9 + (i))
#define JIT_V_NUM			3
#define JIT_V(i)			((i) == 0 ? _RBX : _R12 + (i))

#define jit_allocai(n)			jit_allocai(n)
__jit_inline int
jit_allocai(int n)
{
    int		s = (_jitl.alloca_slack - n) & 15;

    if (n >= _jitl.alloca_slack) {
	_jitl.alloca_slack += n + s;
	SUBQir(n + s, _RSP);
    }
    _jitl.alloca_slack -= n;
    return (_jitl.alloca_offset -= n);
}

#define jit_movi_p(r0, i0)		jit_movi_p(r0, i0)
__jit_inline jit_insn *
jit_movi_p(jit_gpr_t r0, void *i0)
{
    MOVQir((long)i0, r0);
    return (_jit.x.pc);
}

#define jit_movi_l(r0, i0)		jit_movi_l(r0, i0)
#define jit_movi_ul(r0, i0)		jit_movi_l(r0, i0)
/* ensure proper zero/sign extension */
#define jit_movi_i(r0, i0)		jit_movi_l(r0, (long)(int)i0)
#define jit_movi_ui(r0, i0)		jit_movi_l(r0, (_ul)(_ui)i0)
__jit_inline void
jit_movi_l(jit_gpr_t r0, long i0)
{
    if (i0) {
	if (jit_can_zero_extend_int_p(i0))
	    MOVLir(i0, r0);
	else
	    MOVQir(i0, r0);
    }
    else
	XORQrr(r0, r0);
}

/* Return address is 8 bytes, plus 5 registers = 40 bytes, total = 48 bytes. */
#define jit_prolog(n)			jit_prolog(n)
__jit_inline void
jit_prolog(int n)
{
     /* counter of integer arguments */
    _jitl.nextarg_puti = n;
    /* counter of stack arguments */
    _jitl.argssize = (_jitl.nextarg_puti > JIT_ARG_MAX)
	? _jitl.nextarg_puti - JIT_ARG_MAX : 0;
    /* offset of stack arguments */
    _jitl.framesize = (_jitl.argssize & 1) ? 56 : 48;
    /* counter of float arguments */
    _jitl.nextarg_putfp = 0;
    /*offsets of arguments */
    _jitl.nextarg_getfp = _jitl.nextarg_geti = 0;
    /* alloca information */
    _jitl.alloca_offset = _jitl.alloca_slack = 0;
    /* stack frame */
    PUSHQr(_RBX);
    PUSHQr(_R12);
    PUSHQr(_R13);
    PUSHQr(_R14);
    PUSHQr(_RBP);
    MOVQrr(_RSP, _RBP);
}

#define jit_ret				jit_ret
__jit_inline void
jit_ret(void)
{
    LEAVE_();
    POPQr(_R14);
    POPQr(_R13);
    POPQr(_R12);
    POPQr(_RBX);
    RET_();
}

#define jit_calli(p0)			jit_calli(p0)
__jit_inline jit_insn *
jit_calli(void *p0)
{
    MOVQir((long)p0, JIT_REXTMP);
    _jitl.label = _jit.x.pc;
    CALLsr(JIT_REXTMP);
    return (_jitl.label);
}

#define jit_callr(r0)			jit_callr(r0)
__jit_inline void
jit_callr(jit_gpr_t r0)
{
    CALLsr(r0);
}

#define jit_patch_calli(call, label)	jit_patch_calli(call, label)
__jit_inline void
jit_patch_calli(jit_insn *call, jit_insn *label)
{
    jit_patch_movi(call, label);
}

#define jit_prepare_i(ni)		jit_prepare_i(ni)
__jit_inline void
jit_prepare_i(int ni)
{
    /* offset of right to left integer argument */
    _jitl.nextarg_puti = ni;
    /* float argument offset and register counter */
    _jitl.nextarg_putfp = _jitl.fprssize = 0;
    /* argssize is used to keep track of stack slots used */
    _jitl.argssize = _jitl.nextarg_puti > JIT_ARG_MAX
	? _jitl.nextarg_puti - JIT_ARG_MAX : 0;
}

#define jit_pusharg_i(r0)		jit_pusharg_i(r0)
#define jit_pusharg_l(r0)		jit_pusharg_i(r0)
__jit_inline void
jit_pusharg_i(jit_gpr_t r0)
{
    if (--_jitl.nextarg_puti >= JIT_ARG_MAX)
	/* pass argument in stack */
	PUSHQr(r0);
    else
	/* pass argument in register */
	MOVQrr(r0, jit_arg_reg_order[_jitl.nextarg_puti]);
}

#define jit_finish(p0)			jit_finish(p0)
__jit_inline jit_insn *
jit_finish(void *p0)
{
    if (_jitl.fprssize) {
	MOVBir(_jitl.fprssize, _RAX);
	_jitl.fprssize = 0;
    }
    else
	MOVBir(0, _RAX);
    if (_jitl.argssize & 1) {
	PUSHQr(_RAX);
	++_jitl.argssize;
    }
    jit_calli(p0);
    if (_jitl.argssize) {
	ADDQir(sizeof(long) * _jitl.argssize, JIT_SP);
	_jitl.argssize = 0;
    }

    return (_jitl.label);
}

#define jit_finishr(rs)			jit_finishr(rs)
__jit_inline void
jit_finishr(jit_gpr_t r0)
{
    if (r0 == _RAX) {
	/* clobbered with # of fp registers (for varargs) */
	MOVQrr(_RAX, JIT_REXTMP);
	r0 = JIT_REXTMP;
    }
    if (_jitl.fprssize) {
	MOVBir(_jitl.fprssize, _RAX);
	_jitl.fprssize = 0;
    }
    else
	MOVBir(0, _RAX);
    if (_jitl.argssize & 1) {
	PUSHQr(_RAX);
	++_jitl.argssize;
    }
    jit_callr(r0);
    if (_jitl.argssize) {
	ADDQir(sizeof(long) * _jitl.argssize, JIT_SP);
	_jitl.argssize = 0;
    }
}

#define jit_patch_at(jump, label)	jit_patch_at(jump, label)
__jit_inline void
jit_patch_at(jit_insn *jump, jit_insn *label)
{
    if (_jitl.long_jumps)
	jit_patch_abs_long_at(jump - 3, label);
    else
	jit_patch_rel_int_at(jump, label);
}

/* ALU */
#define jit_negr_l(r0, r1)		jit_negr_l(r0, r1)
__jit_inline void
jit_negr_l(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 == r1)
	NEGQr(r0);
    else {
	XORQrr(r0, r0);
	SUBQrr(r1, r0);
    }
}

#define jit_addi_l(r0, r1, i0)		jit_addi_l(r0, r1, i0)
__jit_inline void
jit_addi_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0 == 0)
	jit_movr_l(r0, r1);
    else if (i0 == 1) {
	jit_movr_l(r0, r1);
	INCQr(r0);
    }
    else if (i0 == -1) {
	jit_movr_l(r0, r1);
	DECQr(r0);
    }
    else if (jit_can_sign_extend_int_p(i0)) {
	if (r0 == r1)
	    ADDQir(i0, r0);
	else
	    LEAQmr(i0, r1, 0, 0, r0);
    }
    else if (r0 != r1) {
	MOVQir(i0, r0);
	ADDQrr(r1, r0);
    }
    else {
	MOVQir(i0, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, r0);
    }
}

#define jit_addr_l(r0, r1, r2)		jit_addr_l(r0, r1, r2)
__jit_inline void
jit_addr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1)
	ADDQrr(r2, r0);
    else if (r0 == r2)
	ADDQrr(r1, r0);
    else
	LEAQmr(0, r1, r2, 1, r0);
}

#define jit_subr_l(r0, r1, r2)		jit_subr_l(r0, r1, r2)
__jit_inline void
jit_subr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2)
	XORQrr(r0, r0);
    else if (r0 == r2) {
	SUBQrr(r1, r0);
	NEGQr(r0);
    }
    else {
	jit_movr_l(r0, r1);
	SUBQrr(r2, r0);
    }
}

/* o Immediates are sign extended
 * o CF (C)arry (F)lag is set when interpreting it as unsigned addition
 * o OF (O)verflow (F)lag is set when interpreting it as signed addition
 */
/* Commutative */
#define jit_addci_ul(r0, r1, i0)	jit_addci_ul(r0, r1, i0)
__jit_inline void
jit_addci_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    if (jit_can_sign_extend_int_p(i0)) {
	jit_movr_l(r0, r1);
	ADDQir(i0, r0);
    }
    else if (r0 == r1) {
	MOVQir(i0, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, r0);
    }
    else {
	MOVQir(i0, r0);
	ADDQrr(r1, r0);
    }
}

#define jit_addcr_ul(r0, r1, r2)	jit_addcr_ul(r0, r1, r2)
__jit_inline void
jit_addcr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2) {
    if (r0 == r2)
	ADDQrr(r1, r0);
    else if (r0 == r1)
	ADDQrr(r2, r0);
    else {
	MOVQrr(r1, r0);
	ADDQrr(r2, r0);
    }
}

#define jit_addxi_ul(r0, r1, i0)	jit_addxi_ul(r0, r1, i0)
__jit_inline void
jit_addxi_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    if (jit_can_sign_extend_int_p(i0)) {
	jit_movr_l(r0, r1);
	ADCQir(i0, r0);
    }
    else if (r0 == r1) {
	MOVQir(i0, JIT_REXTMP);
	ADCQrr(JIT_REXTMP, r0);
    }
    else {
	MOVQir(i0, r0);
	ADCQrr(r1, r0);
    }
}

#define jit_addxr_ul(r0, r1, r2)	jit_addxr_ul(r0, r1, r2)
__jit_inline void
jit_addxr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2) {
    if (r0 == r2)
	ADCQrr(r1, r0);
    else if (r0 == r1)
	ADCQrr(r2, r0);
    else {
	MOVQrr(r1, r0);
	ADCQrr(r2, r0);
    }
}

/* Non commutative */
#define jit_subci_ul(r0, r1, i0)	jit_subci_ul(r0, r1, i0)
__jit_inline void
jit_subci_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    jit_movr_l(r0, r1);
    if (jit_can_sign_extend_int_p(i0))
	SUBQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	SUBQrr(JIT_REXTMP, r0);
    }
}

#define jit_subcr_ul(r0, r1, r2)	jit_subcr_ul(r0, r1, r2)
__jit_inline void
jit_subcr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r2) {
	MOVQrr(r0, JIT_REXTMP);
	MOVQrr(r1, r0);
	SUBQrr(JIT_REXTMP, r0);
    }
    else {
	jit_movr_l(r0, r1);
	SUBQrr(r2, r0);
    }
}

#define jit_subxi_ul(r0, r1, i0)	jit_subxi_ul(r0, r1, i0)
__jit_inline void
jit_subxi_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    jit_movr_l(r0, r1);
    if (jit_can_sign_extend_int_p(i0))
	SBBQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	SBBQrr(JIT_REXTMP, r0);
    }
}

#define jit_subxr_ul(r0, r1, r2)	jit_subxr_ul(r0, r1, r2)
__jit_inline void
jit_subxr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r2) {
	MOVQrr(r0, JIT_REXTMP);
	MOVQrr(r1, r0);
	SBBQrr(JIT_REXTMP, r0);
    }
    else {
	MOVQrr(r1, r0);
	SBBQrr(r2, r0);
    }
}

#define jit_andi_l(r0, r1, i0)		jit_andi_l(r0, r1, i0)
__jit_inline void
jit_andi_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0 == 0)
	XORQrr(r0, r0);
    else if (i0 == -1)
	jit_movr_l(r0, r1);
    else if (r0 == r1) {
	if (jit_can_sign_extend_int_p(i0))
	    ANDQir(i0, r0);
	else {
	    MOVQir(i0, JIT_REXTMP);
	    ANDQrr(JIT_REXTMP, r0);
	}
    }
    else {
	MOVQir(i0, r0);
	ANDQrr(r1, r0);
    }
}

#define jit_andr_l(r0, r1, r2)		jit_andr_l(r0, r1, r2)
__jit_inline void
jit_andr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2)
	jit_movr_l(r0, r1);
    else if (r0 == r1)
	ANDQrr(r2, r0);
    else if (r0 == r2)
	ANDQrr(r1, r0);
    else {
	MOVQrr(r1, r0);
	ANDQrr(r2, r0);
    }
}

#define jit_ori_l(r0, r1, i0)		jit_ori_l(r0, r1, i0)
__jit_inline void
jit_ori_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0 == 0)
	jit_movr_l(r0, r1);
    else if (i0 == -1)
	MOVQir(-1, r0);
    else if (r0 == r1) {
	if (jit_can_sign_extend_char_p(i0))
	    ORBir(i0, r0);
	else if (jit_can_sign_extend_int_p(i0))
	    ORQir(i0, r0);
	else {
	    MOVQir(i0, JIT_REXTMP);
	    ORQrr(JIT_REXTMP, r0);
	}
    }
    else {
	MOVQir(i0, r0);
	ORQrr(r1, r0);
    }
}

#define jit_orr_l(r0, r1, r2)		jit_orr_l(r0, r1, r2)
__jit_inline void
jit_orr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2)
	jit_movr_l(r0, r1);
    else if (r0 == r1)
	ORQrr(r2, r0);
    else if (r0 == r2)
	ORQrr(r1, r0);
    else {
	MOVQrr(r1, r0);
	ORQrr(r2, r0);
    }
}

#define jit_xori_l(r0, r1, i0)		jit_xori_l(r0, r1, i0)
__jit_inline void
jit_xori_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0 == 0)
	jit_movr_l(r0, r1);
    else if (i0 == -1) {
	jit_movr_l(r0, r1);
	NOTQr(r0);
    }
    else {
	if (jit_can_sign_extend_char_p(i0)) {
	    jit_movr_l(r0, r1);
	    XORBir(i0, r0);
	}
	else if (jit_can_sign_extend_int_p(i0)) {
	    jit_movr_l(r0, r1);
	    XORQir(i0, r0);
	}
	else {
	    if (r0 == r1) {
		MOVQir(i0, JIT_REXTMP);
		XORQrr(JIT_REXTMP, r0);
	    }
	    else {
		MOVQir(i0, r0);
		XORQrr(r1, r0);
	    }
	}
    }
}

#define jit_xorr_l(r0, r1, r2)		jit_xorr_l(r0, r1, r2)
__jit_inline void
jit_xorr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r1 == r2) {
	if (r0 != r1)
	    MOVQrr(r1, r0);
	else
	    XORQrr(r0, r0);
    }
    else if (r0 == r1)
	XORQrr(r2, r0);
    else if (r0 == r2)
	XORQrr(r1, r0);
    else {
	MOVQrr(r1, r0);
	XORQrr(r2, r0);
    }
}

#define jit_muli_l(r0, r1, i0)		jit_muli_l(r0, r1, i0)
#define jit_muli_ul(r0, r1, i0)		jit_muli_l(r0, r1, i0)
__jit_inline void
jit_muli_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0 == 0)
	XORQrr(r0, r0);
    else if (i0 == 1)
	jit_movr_l(r0, r1);
    else if (i0 == -1)
	jit_negr_l(r0, r1);
    else if (jit_can_sign_extend_int_p(i0))
	IMULQirr(i0, r1, r0);
    else if (r0 == r1) {
	MOVQir(i0, JIT_REXTMP);
	IMULQrr(JIT_REXTMP, r0);
    }
    else {
	MOVQir(i0, r0);
	IMULQrr(r1, r0);
    }
}

#define jit_mulr_l(r0, r1, r2)		jit_mulr_l(r0, r1, r2)
#define jit_mulr_ul(r0, r1, r2)		jit_mulr_l(r0, r1, r2)
__jit_inline void
jit_mulr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1)
	IMULQrr(r2, r0);
    else if (r0 == r2)
	IMULQrr(r1, r0);
    else {
	MOVQrr(r1, r0);
	IMULQrr(r2, r0);
    }
}

/*  Instruction format is:
 *	imul reg64/mem64
 *  and the result is stored in %rdx:%rax
 *  %rax = low 64 bits
 *  %rdx = high 64 bits
 */
__jit_inline void
jit_muli_l_(jit_gpr_t r0, long i0)
{
    if (r0 == _RAX) {
	jit_movi_l(_RDX, i0);
	IMULQr(_RDX);
    }
    else {
	jit_movi_l(_RAX, i0);
	IMULQr(r0);
    }
}

#define jit_hmuli_l(r0, r1, i0)		jit_hmuli_l(r0, r1, i0)
__jit_inline void
jit_hmuli_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (r0 == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_l_(r1, i0);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (r0 == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_muli_l_(r1, i0);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_muli_l_(r1, i0);
	MOVQrr(_RDX, r0);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

__jit_inline void
jit_mulr_l_(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r1 == _RAX)
	IMULQr(r0);
    else if (r0 == _RAX)
	IMULQr(r1);
    else {
	MOVQrr(r1, _RAX);
	IMULQr(r0);
    }
}

#define jit_hmulr_l(r0, r1, r2)		jit_hmulr_l(r0, r1, r2)
__jit_inline void
jit_hmulr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_l_(r1, r2);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (r0 == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_mulr_l_(r1, r2);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_mulr_l_(r1, r2);
	MOVQrr(_RDX, r0);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

/*  Instruction format is:
 *	mul reg64/mem64
 *  and the result is stored in %rdx:%rax
 *  %rax = low 64 bits
 *  %rdx = high 64 bits
 */
__jit_inline void
jit_muli_ul_(jit_gpr_t r0, unsigned long i0)
{
    if (r0 == _RAX) {
	jit_movi_ul(_RDX, i0);
	MULQr(_RDX);
    }
    else {
	jit_movi_ul(_RAX, i0);
	MULQr(r0);
    }
}

#define jit_hmuli_ul(r0, r1, i0)	jit_hmuli_ul(r0, r1, i0)
__jit_inline void
jit_hmuli_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    if (r0 == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_ul_(r1, i0);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (r0 == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_muli_ul_(r1, i0);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_muli_ul_(r1, i0);
	MOVQrr(_RDX, r0);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

__jit_inline void
jit_mulr_ul_(jit_gpr_t r0, jit_gpr_t r1)
{
    if (r1 == _RAX)
	MULQr(r0);
    else if (r0 == _RAX)
	MULQr(r1);
    else {
	MOVQrr(r1, _RAX);
	MULQr(r0);
    }
}

#define jit_hmulr_ul(r0, r1, r2)	jit_hmulr_ul(r0, r1, r2)
__jit_inline void
jit_hmulr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_ul_(r1, r2);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (r0 == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_mulr_ul_(r1, r2);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_mulr_ul_(r1, r2);
	MOVQrr(_RDX, r0);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

__jit_inline void
_jit_divi_l_(jit_gpr_t r0, jit_gpr_t r1, long i0, int is_signed, int is_divide)
{
    jit_gpr_t	div;

    if (r0 == _RAX) {
	jit_pushr_l(_RDX);
	div = JIT_REXTMP;
    }
    else if (r0 == _RDX) {
	jit_pushr_l(_RAX);
	div = JIT_REXTMP;
    }
    else if (r0 == r1) {
	jit_pushr_l(_RDX);
	jit_pushr_l(_RAX);
	div = JIT_REXTMP;
    }
    else {
	jit_pushr_l(_RDX);
	MOVQrr(_RAX, JIT_REXTMP);
	div = r0;
    }

    MOVQir(i0, div);
    jit_movr_l(_RAX, r1);

    if (is_signed) {
	CQO_();
	IDIVQr(div);
    }
    else {
	XORQrr(_RDX, _RDX);
	DIVQr(div);
    }

    if (r0 != _RAX) {
	if (is_divide)
	    MOVQrr(_RAX, r0);
	if (div == JIT_REXTMP)
	    jit_popr_l(_RAX);
	else
	    MOVQrr(JIT_REXTMP, _RAX);
    }
    if (r0 != _RDX) {
	if (!is_divide)
	    MOVQrr(_RDX, r0);
	jit_popr_l(_RDX);
    }
}

__jit_inline void
_jit_divr_l_(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2,
	     int is_signed, int is_divide)
{
    jit_gpr_t	div;

    div = (r2 == _RAX || r2 == _RDX) ? JIT_REXTMP : r2;
    if (r0 == _RAX)
	jit_pushr_l(_RDX);
    else if (r0 == _RDX)
	jit_pushr_l(_RAX);
    else if (div == JIT_REXTMP) {
	jit_pushr_l(_RAX);
	jit_pushr_l(_RDX);
    }
    else {
	jit_pushr_l(_RDX);
	MOVQrr(_RAX, JIT_REXTMP);
    }
    jit_movr_l(div, r2);
    jit_movr_l(_RAX, r1);

    if (is_signed) {
	CQO_();
	IDIVQr(div);
    }
    else {
	XORQrr(_RDX, _RDX);
	DIVQr(div);
    }

    if (r0 != _RAX) {
	if (is_divide)
	    MOVQrr(_RAX, r0);
	if (div == JIT_REXTMP)
	    jit_popr_l(_RAX);
	else
	    MOVQrr(JIT_REXTMP, _RAX);
    }
    if (r0 != _RDX) {
	if (!is_divide)
	    MOVQrr(_RDX, r0);
	jit_popr_l(_RDX);
    }
}

#define jit_divi_l(r0, r1, i0)		jit_divi_l(r0, r1, i0)
__jit_inline void
jit_divi_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    _jit_divi_l_(r0, r1, i0, 1, 1);
}

#define jit_divr_l(r0, r1, r2)		jit_divr_l(r0, r1, r2)
__jit_inline void
jit_divr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_divr_l_(r0, r1, r2, 1, 1);
}

#define jit_divi_ul(r0, r1, i0)		jit_divi_ul(r0, r1, i0)
__jit_inline void
jit_divi_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    _jit_divi_l_(r0, r1, i0, 0, 1);
}

#define jit_divr_ul(r0, r1, r2)		jit_divr_ul(r0, r1, r2)
__jit_inline void
jit_divr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_divr_l_(r0, r1, r2, 0, 1);
}

#define jit_modi_l(r0, r1, i0)		jit_modi_l(r0, r1, i0)
__jit_inline void
jit_modi_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    _jit_divi_l_(r0, r1, i0, 1, 0);
}

#define jit_modr_l(r0, r1, r2)		jit_modr_l(r0, r1, r2)
__jit_inline void
jit_modr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_divr_l_(r0, r1, r2, 1, 0);
}

#define jit_modi_ul(r0, r1, i0)		jit_modi_ul(r0, r1, i0)
__jit_inline void
jit_modi_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    _jit_divi_l_(r0, r1, i0, 0, 0);
}

#define jit_modr_ul(r0, r1, r2)		jit_modr_ul(r0, r1, r2)
__jit_inline void
jit_modr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_divr_l_(r0, r1, r2, 0, 0);
}

/*  Instruction format is:
 *  <shift> %r0 %r1
 *	%r0 <shift>= %r1
 *  only %cl can be used as %r1
 */
__jit_inline void
_jit_shift64(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, int code)
{
    if (r0 == _RCX) {
	MOVQrr(r1, JIT_REXTMP);
	if (r2 != _RCX)
	    MOVBrr(r2, _RCX);
	_ROTSHIQrr(code, _RCX, JIT_REXTMP);
	MOVQrr(JIT_REXTMP, _RCX);
    }
    else if (r2 != _RCX) {
	MOVQrr(_RCX, JIT_REXTMP);
	MOVBrr(r2, _RCX);
	jit_movr_l(r0, r1);
	_ROTSHIQrr(code, _RCX, r0);
	MOVQrr(JIT_REXTMP, _RCX);
    }
    else {
	jit_movr_l(r0, r1);
	_ROTSHIQrr(code, _RCX, r0);
    }
}

#define jit_lshi_l(r0, r1, i0)		jit_lshi_l(r0, r1, i0)
__jit_inline void
jit_lshi_l(jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    if (i0 == 0)
	jit_movr_l(r0, r1);
    else if (i0 <= 3)
	LEAQmr(0, 0, r1, 1 << i0, r0);
    else {
	jit_movr_l(r0, r1);
	SHLQir(i0, r0);
    }
}

#define jit_lshr_l(r0, r1, r2)		jit_lshr_l(r0, r1, r2)
__jit_inline void
jit_lshr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_shift64(r0, r1, r2, X86_SHL);
}

#define jit_rshi_l(r0, r1, i0)		jit_rshi_l(r0, r1, i0)
__jit_inline void
jit_rshi_l(jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    jit_movr_l(r0, r1);
    if (i0)
	SARQir(i0, r0);
}

#define jit_rshr_l(r0, r1, r2)		jit_rshr_l(r0, r1, r2)
__jit_inline void
jit_rshr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_shift64(r0, r1, r2, X86_SAR);
}

#define jit_rshi_ul(r0, r1, i0)		jit_rshi_ul(r0, r1, i0)
__jit_inline void
jit_rshi_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    jit_movr_l(r0, r1);
    if (i0)
	SHRQir(i0, r0);
}

#define jit_rshr_ul(r0, r1, r2)		jit_rshr_ul(r0, r1, r2)
__jit_inline void
jit_rshr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_shift64(r0, r1, r2, X86_SHR);
}

/* Boolean */
__jit_inline void
_jit_cmp_ri64(jit_gpr_t r0, jit_gpr_t r1, long i0, int code)
{
    int		same = r0 == r1;

    if (!same)
	/* XORLrr is cheaper */
	XORLrr(r0, r0);
    if (jit_can_sign_extend_int_p(i0))
	CMPQir(i0, r1);
    else {
	MOVQir(i0, JIT_REXTMP);
	CMPQrr(JIT_REXTMP, r1);
    }
    if (same)
	/* MOVLir is cheaper */
	MOVLir(0, r0);
    SETCCir(code, r0);
}

__jit_inline void
_jit_test_r64(jit_gpr_t r0, jit_gpr_t r1, int code)
{
    int		same = r0 == r1;

    if (!same)
	XORLrr(r0, r0);
    TESTQrr(r1, r1);
    if (same)
	MOVLir(0, r0);
    SETCCir(code, r0);
}

__jit_inline void
_jit_cmp_rr64(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, int code)
{
    int		same = r0 == r1 || r0 == r2;

    if (!same)
	XORLrr(r0, r0);
    CMPQrr(r2, r1);
    if (same)
	MOVLir(0, r0);
    SETCCir(code, r0);
}

#define jit_lti_l(r0, r1, i0)		jit_lti_l(r0, r1, i0)
__jit_inline void
jit_lti_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_L);
    else
	_jit_test_r64(r0, r1,		X86_CC_S);
}

#define jit_ltr_l(r0, r1, r2)		jit_ltr_l(r0, r1, r2)
__jit_inline void
jit_ltr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_L);
}

#define jit_lei_l(r0, r1, i0)		jit_lei_l(r0, r1, i0)
__jit_inline void
jit_lei_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    _jit_cmp_ri64(r0, r1, i0,		X86_CC_LE);
}

#define jit_ler_l(r0, r1, r2)		jit_ler_l(r0, r1, r2)
__jit_inline void
jit_ler_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_LE);
}

#define jit_eqi_l(r0, r1, i0)		jit_eqi_l(r0, r1, i0)
__jit_inline void
jit_eqi_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_E);
    else
	_jit_test_r64(r0, r1,		X86_CC_E);
}

#define jit_eqr_l(r0, r1, r2)		jit_eqr_l(r0, r1, r2)
__jit_inline void
jit_eqr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_E);
}

#define jit_gei_l(r0, r1, i0)		jit_gei_l(r0, r1, i0)
__jit_inline void
jit_gei_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_GE);
    else
	_jit_test_r64(r0, r1,		X86_CC_NS);
}

#define jit_ger_l(r0, r1, r2)		jit_ger_l(r0, r1, r2)
__jit_inline void
jit_ger_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_GE);
}

#define jit_gti_l(r0, r1, i0)		jit_gti_l(r0, r1, i0)
__jit_inline void
jit_gti_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    _jit_cmp_ri64(r0, r1, i0,		X86_CC_G);
}

#define jit_gtr_l(r0, r1, r2)		jit_gtr_l(r0, r1, r2)
__jit_inline void
jit_gtr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_G);
}

#define jit_nei_l(r0, r1, i0)		jit_nei_l(r0, r1, i0)
__jit_inline void
jit_nei_l(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_NE);
    else
	_jit_test_r64(r0, r1,		X86_CC_NE);
}

#define jit_ner_l(r0, r1, r2)		jit_ner_l(r0, r1, r2)
__jit_inline void
jit_ner_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_NE);
}

#define jit_lti_ul(r0, r1, i0)		jit_lti_ul(r0, r1, i0)
__jit_inline void
jit_lti_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    _jit_cmp_ri64(r0, r1, i0,		X86_CC_B);
}

#define jit_ltr_ul(r0, r1, r2)		jit_ltr_ul(r0, r1, r2)
__jit_inline void
jit_ltr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_B);
}

#define jit_lei_ul(r0, r1, i0)		jit_lei_ul(r0, r1, i0)
__jit_inline void
jit_lei_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_BE);
    else
	_jit_test_r64(r0, r1,		X86_CC_E);
}

#define jit_ler_ul(r0, r1, r2)		jit_ler_ul(r0, r1, r2)
__jit_inline void
jit_ler_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_BE);
}

#define jit_gei_ul(r0, r1, i0)		jit_gei_ul(r0, r1, i0)
__jit_inline void
jit_gei_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_AE);
    else
	_jit_test_r64(r0, r1,		X86_CC_NB);
}

#define jit_ger_ul(r0, r1, r2)		jit_ger_ul(r0, r1, r2)
__jit_inline void
jit_ger_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_AE);
}

#define jit_gti_ul(r0, r1, i0)		jit_gti_ul(r0, r1, i0)
__jit_inline void
jit_gti_ul(jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    if (i0)
	_jit_cmp_ri64(r0, r1, i0,	X86_CC_A);
    else
	_jit_test_r64(r0, r1,		X86_CC_NE);
}

#define jit_gtr_ul(r0, r1, r2)		jit_gtr_ul(r0, r1, r2)
__jit_inline void
jit_gtr_ul(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_cmp_rr64(r0, r1, r2,		X86_CC_A);
}

/* Jump */
__jit_inline void
_jit_bcmp_ri64(jit_insn *label, jit_gpr_t r0, long i0, int code)
{
    if (jit_can_sign_extend_int_p(i0))
	CMPQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	CMPQrr(JIT_REXTMP, r0);
    }
    JCCim(code, label);
}

__jit_inline void
_jit_btest_r64(jit_insn *label, jit_gpr_t r0, int code)
{
    TESTQrr(r0, r0);
    JCCim(code, label);
}

__jit_inline void
_jit_bcmp_rr64(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1, int code)
{
    CMPQrr(r1, r0);
    JCCim(code, label);
}

#define jit_blti_l(label, r0, i0)	jit_blti_l(label, r0, i0)
__jit_inline jit_insn *
jit_blti_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_L);
    else
	_jit_btest_r64(label, r0,	X86_CC_S);
    return (_jit.x.pc);
}

#define jit_bltr_l(label, r0, r1)	jit_bltr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_L);
    return (_jit.x.pc);
}

#define jit_blei_l(label, r0, i0)	jit_blei_l(label, r0, i0)
__jit_inline jit_insn *
jit_blei_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_bler_l(label, r0, r1)	jit_bler_l(label, r0, r1)
__jit_inline jit_insn *
jit_bler_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_beqi_l(label, r0, i0)	jit_beqi_l(label, r0, i0)
__jit_inline jit_insn *
jit_beqi_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_E);
    else
	_jit_btest_r64(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_beqr_l(label, r0, r1)	jit_beqr_l(label, r0, r1)
__jit_inline jit_insn *
jit_beqr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bgei_l(label, r0, i0)	jit_bgei_l(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_GE);
    else
	_jit_btest_r64(label, r0,	X86_CC_NS);
    return (_jit.x.pc);
}

#define jit_bger_l(label, r0, r1)	jit_bger_l(label, r0, r1)
__jit_inline jit_insn *
jit_bger_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_GE);
    return (_jit.x.pc);
}

#define jit_bgti_l(label, r0, i0)	jit_bgti_l(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bgtr_l(label, r0, r1)	jit_bgtr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bnei_l(label, r0, i0)	jit_bnei_l(label, r0, i0)
__jit_inline jit_insn *
jit_bnei_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_NE);
    else
	_jit_btest_r64(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bner_l(label, r0, r1)	jit_bner_l(label, r0, r1)
__jit_inline jit_insn *
jit_bner_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_blti_ul(label, r0, i0)	jit_blti_ul(label, r0, i0)
__jit_inline jit_insn *
jit_blti_ul(jit_insn *label, jit_gpr_t r0, unsigned long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_bltr_ul(label, r0, r1)	jit_bltr_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_ul(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_blei_ul(label, r0, i0)	jit_blei_ul(label, r0, i0)
__jit_inline jit_insn *
jit_blei_ul(jit_insn *label, jit_gpr_t r0, unsigned long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_BE);
    else
	_jit_btest_r64(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bler_ul(label, r0, r1)	jit_bler_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bler_ul(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_BE);
    return (_jit.x.pc);
}

#define jit_bgei_ul(label, r0, i0)	jit_bgei_ul(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_ul(jit_insn *label, jit_gpr_t r0, unsigned long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bger_ul(label, r0, r1)	jit_bger_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bger_ul(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bgti_ul(label, r0, i0)	jit_bgti_ul(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_ul(jit_insn *label, jit_gpr_t r0, unsigned long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_A);
    else
	_jit_btest_r64(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bgtr_ul(label, r0, r1)	jit_bgtr_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_ul(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_A);
    return (_jit.x.pc);
}

#define jit_boaddi_l(label, r0, i0)	jit_boaddi_l(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (jit_can_sign_extend_int_p(i0))
	ADDQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, r0);
    }
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddr_l(label, r0, r1)	jit_boaddr_l(label, r0, r1)
__jit_inline jit_insn *
jit_boaddr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    ADDQrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_l(label, r0, i0)	jit_bosubi_l(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (jit_can_sign_extend_int_p(i0))
	SUBQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	SUBQrr(JIT_REXTMP, r0);
    }
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubr_l(label, r0, r1)	jit_bosubr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bosubr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    SUBQrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddi_ul(label, r0, i0)	jit_boaddi_ul(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_ul(jit_insn *label, jit_gpr_t r0, unsigned long i0)
{
    if (jit_can_sign_extend_int_p(i0))
	ADDQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, r0);
    }
    JCm(label);
    return (_jit.x.pc);
}

#define jit_boaddr_ul(label, r0, r1)	jit_boaddr_ul(label, r0, r1)
__jit_inline jit_insn *
jit_boaddr_ul(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    ADDQrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_ul(label, r0, i0)	jit_bosubi_ul(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_ul(jit_insn *label, jit_gpr_t r0, unsigned long i0)
{
    if (jit_can_sign_extend_int_p(i0))
	SUBQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	SUBQrr(JIT_REXTMP, r0);
    }
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubr_ul(label, r0, r1)	jit_bosubr_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bosubr_ul(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    SUBQrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bmsi_l(label, r0, i0)	jit_bmsi_l(label, r0, i0)
__jit_inline jit_insn *
jit_bmsi_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (jit_can_zero_extend_char_p(i0))
	TESTBir(i0, r0);
    else if (jit_can_zero_extend_short_p(i0))
	TESTWir(i0, r0);
    else if (jit_can_sign_extend_int_p(i0))
	TESTLir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	TESTQrr(JIT_REXTMP, r0);
    }
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmsr_l(label, r0, r1)	jit_bmsr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bmsr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    TESTQrr(r1, r0);
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmci_l(label, r0, i0)	jit_bmci_l(label, r0, i0)
__jit_inline jit_insn *
jit_bmci_l(jit_insn *label, jit_gpr_t r0, long i0)
{
    if (jit_can_zero_extend_char_p(i0))
	TESTBir(i0, r0);
    else if (jit_can_zero_extend_short_p(i0))
	TESTWir(i0, r0);
    else if (jit_can_zero_extend_int_p(i0))
	TESTLir(i0, r0);
    else if (jit_can_sign_extend_int_p(i0))
	TESTQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	TESTQrr(JIT_REXTMP, r0);
    }
    JZm(label);
    return (_jit.x.pc);
}

#define jit_bmcr_l(label, r0, r1)	jit_bmcr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bmcr_l(jit_insn *label, jit_gpr_t r0, jit_gpr_t r1)
{
    TESTQrr(r1, r0);
    JZm(label);
    return (_jit.x.pc);
}

/* Memory */
#define jit_ntoh_ul(r0, r1)		jit_ntoh_ul(r0, r1)
__jit_inline void
jit_ntoh_ul(jit_gpr_t r0, jit_gpr_t r1)
{
    jit_movr_l(r0, r1);
    BSWAPQr(r0);
}

#define jit_ldr_c(r0, r1)		jit_ldr_c(r0, r1)
__jit_inline void
jit_ldr_c(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSBQmr(0, r1, 0,  0, r0);
}

#define jit_ldxr_c(r0, r1, r2)		jit_ldxr_c(r0, r1, r2)
__jit_inline void
jit_ldxr_c(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSBQmr(0, r1, r2, 1, r0);
}

#define jit_ldr_s(r0, r1)		jit_ldr_s(r0, r1)
__jit_inline void
jit_ldr_s(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSWQmr(0, r1, 0, 0, r0);
}

#define jit_ldxr_s(r0, r1, r2)		jit_ldxr_s(r0, r1, r2)
__jit_inline void
jit_ldxr_s(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSWQmr(0, r1, r2, 1, r0);
}

#define jit_ldi_c(r0, i0)		jit_ldi_c(r0, i0)
__jit_inline void
jit_ldi_c(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSBQmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_c(r0, JIT_REXTMP);
    }
}

#define jit_ldxi_c(r0, r1, i0)		jit_ldxi_c(r0, r1, i0)
__jit_inline void
jit_ldxi_c(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSBQmr(i0, r1, 0,  0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_c(r0, r1, JIT_REXTMP);
    }
}

#define jit_ldi_uc(r0, i0)		jit_ldi_uc(r0, i0)
__jit_inline void
jit_ldi_uc(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVZBLmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_uc(r0, JIT_REXTMP);
    }
}

#define jit_ldxi_uc(r0, r1, i0)		jit_ldxi_uc(r0, r1, i0)
__jit_inline void
jit_ldxi_uc(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVZBLmr(i0, r1, 0, 0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_uc(r0, r1, JIT_REXTMP);
    }
}

#define jit_str_c(r0, r1)		jit_str_c(r0, r1)
__jit_inline void
jit_str_c(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVBrm(r1, 0, r0, 0, 0);
}

#define jit_sti_c(i0, r0)		jit_sti_c(i0, r0)
__jit_inline void
jit_sti_c(void *i0, jit_gpr_t r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVBrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_str_c(JIT_REXTMP, r0);
    }
}

#define jit_stxr_c(r0, r1, r2)		jit_stxr_c(r0, r1, r2)
__jit_inline void
jit_stxr_c(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVBrm(r2, 0, r0, r1, 1);
}

#define jit_stxi_c(i0, r0, r1)		jit_stxi_c(i0, r0, r1)
__jit_inline void
jit_stxi_c(jit_idx_t i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVBrm(r1, i0, r0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_stxr_c(JIT_REXTMP, r0, r1);
    }
}

#define jit_ldi_s(r0, i0)		jit_ldi_s(r0, i0)
__jit_inline void
jit_ldi_s(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSWQmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_s(r0, JIT_REXTMP);
    }
}

#define jit_ldxi_s(r0, r1, i0)		jit_ldxi_s(r0, r1, i0)
__jit_inline void
jit_ldxi_s(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSWQmr(i0, r1, 0, 0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_s(r0, r1, JIT_REXTMP);
    }
}

#define jit_ldi_us(r0, i0)		jit_ldi_us(r0, i0)
__jit_inline void
jit_ldi_us(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVZWLmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_us(r0, JIT_REXTMP);
    }
}

#define jit_ldxi_us(r0, r1, i0)		jit_ldxi_us(r0, r1, i0)
__jit_inline void
jit_ldxi_us(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVZWLmr(i0, r1, 0, 0, r0);
    else  {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_us(r0, r1, JIT_REXTMP);
    }
}

#define jit_sti_s(i0, r0)		jit_sti_s(i0, r0)
__jit_inline void
jit_sti_s(void *i0, jit_gpr_t r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVWrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_str_s(JIT_REXTMP, r0);
    }
}

#define jit_stxi_s(i0, r0, r1)		jit_stxi_s(i0, r0, r1)
__jit_inline void
jit_stxi_s(jit_idx_t i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVWrm(r1, i0, r0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_stxr_s(JIT_REXTMP, r0, r1);
    }
}

#define jit_ldr_i(r0, r1)		jit_ldr_i(r0, r1)
__jit_inline void
jit_ldr_i(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSLQmr(0, r1, 0, 0, r0);
}

#define jit_ldi_i(r0, i0)		jit_ldi_i(r0, i0)
__jit_inline void
jit_ldi_i(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSLQmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_i(r0, JIT_REXTMP);
    }
}

#define jit_ldxr_i(r0, r1, r2)		jit_ldxr_i(r0, r1, r2)
__jit_inline void
jit_ldxr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSLQmr(0, r1, r2, 1, r0);
}

#define jit_ldxi_i(r0, r1, i0)		jit_ldxi_i(r0, r1, i0)
__jit_inline void
jit_ldxi_i(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSLQmr(i0, r1, 0, 0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_i(r0, r1, JIT_REXTMP);
    }
}

#define jit_ldr_ui(r0, r1)		jit_ldr_ui(r0, r1)
__jit_inline void
jit_ldr_ui(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLmr(0, r1, 0, 0, r0);
}

#define jit_ldi_ui(r0, i0)		jit_ldi_ui(r0, i0)
__jit_inline void
jit_ldi_ui(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVLmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_ui(r0, JIT_REXTMP);
    }
}

#define jit_ldxr_ui(r0, r1, r2)		jit_ldxr_ui(r0, r1, r2)
__jit_inline void
jit_ldxr_ui(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVLmr(0, r1, r2, 1, r0);
}

#define jit_ldxi_ui(r0, r1, i0)		jit_ldxi_ui(r0, r1, i0)
__jit_inline void
jit_ldxi_ui(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVLmr(i0, r1, 0, 0, r0);
    else  {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_ui(r0, r1, JIT_REXTMP);
    }
}

#define jit_sti_i(i0, r0)		jit_sti_i(i0, r0)
__jit_inline void
jit_sti_i(void *i0, jit_gpr_t r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVLrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_str_i(JIT_REXTMP, r0);
    }
}

#define jit_stxi_i(i0, r0, r1)		jit_stxi_i(i0, r0, r1)
__jit_inline void
jit_stxi_i(jit_idx_t i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVLrm(r1, i0, r0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_stxr_i(JIT_REXTMP, r0, r1);
    }
}

#define jit_ldr_l(r0, r1)		jit_ldr_l(r0, r1)
__jit_inline void
jit_ldr_l(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVQmr(0, r1, 0, 0, r0);
}

#define jit_ldi_l(r0, i0)		jit_ldi_l(r0, i0)
__jit_inline void
jit_ldi_l(jit_gpr_t r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVQmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_ldr_l(r0, JIT_REXTMP);
    }
}

#define jit_ldxr_l(r0, r1, r2)		jit_ldxr_l(r0, r1, r2)
__jit_inline void
jit_ldxr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVQmr(0, r1, r2, 1, r0);
}

#define jit_ldxi_l(r0, r1, i0)		jit_ldxi_l(r0, r1, i0)
#define jit_ldxi_ul(r0, r1, i0)		jit_ldxi_l(r0, r1, i0)
#define jit_ldxi_p(r0, r1, i0)		jit_ldxi_l(r0, r1, i0)
__jit_inline void
jit_ldxi_l(jit_gpr_t r0, jit_gpr_t r1, jit_idx_t i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVQmr(i0, r1, 0, 0, r0);
    else  {
	MOVQir(i0, JIT_REXTMP);
	jit_ldxr_l(r0, r1, JIT_REXTMP);
    }
}

#define jit_str_l(r0, r1)		jit_str_l(r0, r1)
__jit_inline void
jit_str_l(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVQrm(r1, 0, r0, 0, 0);
}

#define jit_sti_l(i0, r0)		jit_sti_l(i0, r0)
__jit_inline void
jit_sti_l(void *i0, jit_gpr_t r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVQrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir((long)i0, JIT_REXTMP);
	jit_str_l(JIT_REXTMP, r0);
    }
}

#define jit_stxr_l(r0, r1, r2)		jit_stxr_l(r0, r1, r2)
__jit_inline void
jit_stxr_l(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVQrm(r2, 0, r0, r1, 1);
}

#define jit_stxi_l(i0, r0, r1)		jit_stxi_l(i0, r0, r1)
__jit_inline void
jit_stxi_l(jit_idx_t i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVQrm(r1, i0, r0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_stxr_l(JIT_REXTMP, r0, r1);
    }
}

#define jit_extr_c_l(r0, r1)		jit_extr_c_l(r0, r1)
__jit_inline void
jit_extr_c_l(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSBQrr(r1, r0);
}

#define jit_extr_c_ul(r0, r1)		jit_extr_c_ul(r0, r1)
__jit_inline void
jit_extr_c_ul(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVZBQrr(r1, r0);
}

#define jit_extr_s_l(r0, r1)		jit_extr_s_l(r0, r1)
__jit_inline void
jit_extr_s_l(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSWQrr(r1, r0);
}

#define jit_extr_s_ul(r0, r1)		jit_extr_s_ul(r0, r1)
__jit_inline void
jit_extr_s_ul(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVZWQrr(r1, r0);
}

#define jit_extr_s_l(r0, r1)		jit_extr_s_l(r0, r1)
__jit_inline void
jit_extr_i_l(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSLQrr(r1, r0);
}

#define jit_extr_s_ul(r0, r1)		jit_extr_s_ul(r0, r1)
__jit_inline void
jit_extr_i_ul(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLrr(r1, r0);
}

#define jit_retval_l(r0)		jit_retval_l(r0)
__jit_inline void
jit_retval_l(jit_gpr_t r0)
{
    jit_movr_l(r0, _RAX);
}

#define jit_arg_i			jit_arg_i
#define jit_arg_c()			jit_arg_i()
#define jit_arg_uc()			jit_arg_i()
#define jit_arg_s()			jit_arg_i()
#define jit_arg_us()			jit_arg_i()
#define jit_arg_ui()			jit_arg_i()
#define jit_arg_l()			jit_arg_i()
#define jit_arg_ul()			jit_arg_i()
#define jit_arg_p()			jit_arg_i()
__jit_inline int
jit_arg_i(void)
{
    int		ofs;

    if (_jitl.nextarg_geti < JIT_ARG_MAX) {
	ofs = _jitl.nextarg_geti;
	++_jitl.nextarg_geti;
    }
    else {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(long);
    }

    return (ofs);
}

#define jit_getarg_c(r0, ofs)		jit_getarg_c(r0, ofs)
__jit_inline void
jit_getarg_c(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_c_l(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_c(r0, JIT_FP, ofs);
}

#define jit_getarg_uc(r0, ofs)		jit_getarg_uc(r0, ofs)
__jit_inline void
jit_getarg_uc(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_c_ul(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_uc(r0, JIT_FP, ofs);
}

#define jit_getarg_s(r0, ofs)		jit_getarg_s(r0, ofs)
__jit_inline void
jit_getarg_s(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_s_l(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_s(r0, JIT_FP, ofs);
}

#define jit_getarg_us(r0, ofs)		jit_getarg_us(r0, ofs)
__jit_inline void
jit_getarg_us(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_s_ul(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_us(r0, JIT_FP, ofs);
}

#define jit_getarg_i(r0, ofs)		jit_getarg_i(r0, ofs)
__jit_inline void
jit_getarg_i(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_l(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_i(r0, JIT_FP, ofs);
}

#define jit_getarg_ui(r0, ofs)		jit_getarg_ui(r0, ofs)
__jit_inline void
jit_getarg_ui(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_ul(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_ui(r0, JIT_FP, ofs);
}

#define jit_getarg_l(r0, ofs)		jit_getarg_l(r0, ofs)
__jit_inline void
jit_getarg_l(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_l(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_l(r0, JIT_FP, ofs);
}

#define jit_getarg_ul(r0, ofs)		jit_getarg_ul(r0, ofs)
__jit_inline void
jit_getarg_ul(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_ul(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_ul(r0, JIT_FP, ofs);
}

#define jit_getarg_p(r0, ofs)		jit_getarg_p(r0, ofs)
__jit_inline void
jit_getarg_p(jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_p(r0, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_p(r0, JIT_FP, ofs);
}

#endif /* __lightning_core_h */
