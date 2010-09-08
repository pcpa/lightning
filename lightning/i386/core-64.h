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

#define jit_movi_p(rd, i0)		jit_movi_p(rd, i0)
__jit_inline jit_insn *
jit_movi_p(int rd, void *i0)
{
    MOVQir((long)i0, rd);
    return (_jit.x.pc);
}

#define jit_movi_l(rd, i0)		jit_movi_l(rd, i0)
#define jit_movi_ul(rd, i0)		jit_movi_l(rd, i0)
/* ensure proper zero/sign extension */
#define jit_movi_i(d, is)		jit_movi_l(d, (long)(int)is)
#define jit_movi_ui(d, rs)		jit_movi_l((d), (_ul)(_ui)(rs))
__jit_inline void
jit_movi_l(int rd, long i0)
{
    if (i0) {
	if (jit_can_zero_extend_int_p(i0))
	    MOVLir(i0, rd);
	else
	    MOVQir(i0, rd);
    }
    else
	XORQrr(rd, rd);
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

#define jit_calli(address)		jit_calli(address)
__jit_inline jit_insn *
jit_calli(void *address)
{
    MOVQir((long)address, JIT_REXTMP);
    _jitl.label = _jit.x.pc;
    CALLsr(JIT_REXTMP);
    return (_jitl.label);
}

#define jit_callr(rs)			jit_callr(rs)
__jit_inline void
jit_callr(int rs)
{
    CALLsr(rs);
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

#define jit_pusharg_i(rs)		jit_pusharg_i(rs)
#define jit_pusharg_l(rs)		jit_pusharg_i(rs)
__jit_inline void
jit_pusharg_i(int rs)
{
    if (--_jitl.nextarg_puti >= JIT_ARG_MAX)
	/* pass argument in stack */
	PUSHQr(rs);
    else
	/* pass argument in register */
	MOVQrr(rs, jit_arg_reg_order[_jitl.nextarg_puti]);
}

#define jit_finish(label)		jit_finish(label)
__jit_inline jit_insn *
jit_finish(jit_insn *label)
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
    jit_calli(label);
    if (_jitl.argssize) {
	ADDQir(sizeof(long) * _jitl.argssize, JIT_SP);
	_jitl.argssize = 0;
    }

    return (_jitl.label);
}

#define jit_finishr(rs)			jit_finishr(rs)
__jit_inline void
jit_finishr(int rs)
{
    if (rs == _RAX) {
	/* clobbered with # of fp registers (for varargs) */
	MOVQrr(_RAX, JIT_REXTMP);
	rs = JIT_REXTMP;
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
    jit_callr(rs);
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
#define jit_negr_l(rd, r0)		jit_negr_l(rd, r0)
__jit_inline void
jit_negr_l(int rd, int r0)
{
    if (rd == r0)
	NEGQr(rd);
    else {
	XORQrr(rd, rd);
	SUBQrr(r0, rd);
    }
}

#define jit_addi_l(rd, r0, i0)		jit_addi_l(rd, r0, i0)
__jit_inline void
jit_addi_l(int rd, int r0, long i0)
{
    if (i0 == 0)
	jit_movr_l(rd, r0);
    else if (i0 == 1) {
	jit_movr_l(rd, r0);
	INCQr(rd);
    }
    else if (i0 == -1) {
	jit_movr_l(rd, r0);
	DECQr(rd);
    }
    else if (jit_can_sign_extend_int_p(i0)) {
	if (rd == r0)
	    ADDQir(i0, rd);
	else
	    LEAQmr(i0, r0, 0, 0, rd);
    }
    else if (rd != r0) {
	MOVQir(i0, rd);
	ADDQrr(r0, rd);
    }
    else {
	MOVQir(i0, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, rd);
    }
}

#define jit_addr_l(rd, r0, r1)		jit_addr_l(rd, r0, r1)
__jit_inline void
jit_addr_l(int rd, int r0, int r1)
{
    if (rd == r0)
	ADDQrr(r1, rd);
    else if (rd == r1)
	ADDQrr(r0, rd);
    else
	LEAQmr(0, r0, r1, 1, rd);
}

#define jit_subr_l(rd, r0, r1)		jit_subr_l(rd, r0, r1)
__jit_inline void
jit_subr_l(int rd, int r0, int r1)
{
    if (r0 == r1)
	XORQrr(rd, rd);
    else if (rd == r1) {
	SUBQrr(r0, rd);
	NEGQr(rd);
    }
    else {
	jit_movr_l(rd, r0);
	SUBQrr(r1, rd);
    }
}

/* o Immediates are sign extended
 * o CF (C)arry (F)lag is set when interpreting it as unsigned addition
 * o OF (O)verflow (F)lag is set when interpreting it as signed addition
 */
/* Commutative */
#define jit_addci_ul(rd, r0, i0)	jit_addci_ul(rd, r0, i0)
__jit_inline void
jit_addci_ul(int rd, int r0, unsigned long i0)
{
    if (jit_can_sign_extend_int_p(i0)) {
	jit_movr_l(rd, r0);
	ADDQir(i0, rd);
    }
    else if (rd == r0) {
	MOVQir(i0, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, rd);
    }
    else {
	MOVQir(i0, rd);
	ADDQrr(r0, rd);
    }
}

#define jit_addcr_ul(rd, r0, r1)	jit_addcr_ul(rd, r0, r1)
__jit_inline void
jit_addcr_ul(int rd, int r0, int r1) {
    if (rd == r1)
	ADDQrr(r0, rd);
    else if (rd == r0)
	ADDQrr(r1, rd);
    else {
	MOVQrr(r0, rd);
	ADDQrr(r1, rd);
    }
}

#define jit_addxi_ul(rd, r0, i0)	jit_addxi_ul(rd, r0, i0)
__jit_inline void
jit_addxi_ul(int rd, int r0, unsigned long i0)
{
    if (jit_can_sign_extend_int_p(i0)) {
	jit_movr_l(rd, r0);
	ADCQir(i0, rd);
    }
    else if (rd == r0) {
	MOVQir(i0, JIT_REXTMP);
	ADCQrr(JIT_REXTMP, rd);
    }
    else {
	MOVQir(i0, rd);
	ADCQrr(r0, rd);
    }
}

#define jit_addxr_ul(rd, r0, r1)	jit_addxr_ul(rd, r0, r1)
__jit_inline void
jit_addxr_ul(int rd, int r0, int r1) {
    if (rd == r1)
	ADCQrr(r0, rd);
    else if (rd == r0)
	ADCQrr(r1, rd);
    else {
	MOVQrr(r0, rd);
	ADCQrr(r1, rd);
    }
}

/* Non commutative */
#define jit_subci_ul(rd, r0, i0)	jit_subci_ul(rd, r0, i0)
__jit_inline void
jit_subci_ul(int rd, int r0, unsigned long i0)
{
    jit_movr_l(rd, r0);
    if (jit_can_sign_extend_int_p(i0))
	SUBQir(i0, rd);
    else {
	MOVQir(i0, JIT_REXTMP);
	SUBQrr(JIT_REXTMP, rd);
    }
}

#define jit_subcr_ul(rd, r0, r1)	jit_subcr_ul(rd, r0, r1)
__jit_inline void
jit_subcr_ul(int rd, int r0, int r1)
{
    if (rd == r1) {
	MOVQrr(rd, JIT_REXTMP);
	MOVQrr(r0, rd);
	SUBQrr(JIT_REXTMP, rd);
    }
    else {
	jit_movr_l(rd, r0);
	SUBQrr(r1, rd);
    }
}

#define jit_subxi_ul(rd, r0, i0)	jit_subxi_ul(rd, r0, i0)
__jit_inline void
jit_subxi_ul(int rd, int r0, unsigned long i0)
{
    jit_movr_l(rd, r0);
    if (jit_can_sign_extend_int_p(i0))
	SBBQir(i0, rd);
    else {
	MOVQir(i0, JIT_REXTMP);
	SBBQrr(JIT_REXTMP, rd);
    }
}

#define jit_subxr_ul(rd, r0, r1)	jit_subxr_ul(rd, r0, r1)
__jit_inline void
jit_subxr_ul(int rd, int r0, int r1)
{
    if (rd == r1) {
	MOVQrr(rd, JIT_REXTMP);
	MOVQrr(r0, rd);
	SBBQrr(JIT_REXTMP, rd);
    }
    else {
	MOVQrr(r0, rd);
	SBBQrr(r1, rd);
    }
}

#define jit_andi_l(rd, r0, i0)		jit_andi_l(rd, r0, i0)
__jit_inline void
jit_andi_l(int rd, int r0, long i0)
{
    if (i0 == 0)
	XORQrr(rd, rd);
    else if (i0 == -1)
	jit_movr_l(rd, r0);
    else if (rd == r0) {
	if (jit_can_sign_extend_int_p(i0))
	    ANDQir(i0, rd);
	else {
	    MOVQir(i0, JIT_REXTMP);
	    ANDQrr(JIT_REXTMP, rd);
	}
    }
    else {
	MOVQir(i0, rd);
	ANDQrr(r0, rd);
    }
}

#define jit_andr_l(rd, r0, r1)		jit_andr_l(rd, r0, r1)
__jit_inline void
jit_andr_l(int rd, int r0, int r1)
{
    if (r0 == r1)
	jit_movr_l(rd, r0);
    else if (rd == r0)
	ANDQrr(r1, rd);
    else if (rd == r1)
	ANDQrr(r0, rd);
    else {
	MOVQrr(r0, rd);
	ANDQrr(r1, rd);
    }
}

#define jit_ori_l(rd, r0, i0)		jit_ori_l(rd, r0, i0)
__jit_inline void
jit_ori_l(int rd, int r0, long i0)
{
    if (i0 == 0)
	jit_movr_l(rd, r0);
    else if (i0 == -1)
	MOVQir(-1, rd);
    else if (rd == r0) {
	if (jit_can_sign_extend_char_p(i0))
	    ORBir(i0, rd);
	else if (jit_can_sign_extend_int_p(i0))
	    ORQir(i0, rd);
	else {
	    MOVQir(i0, JIT_REXTMP);
	    ORQrr(JIT_REXTMP, rd);
	}
    }
    else {
	MOVQir(i0, rd);
	ORQrr(r0, rd);
    }
}

#define jit_orr_l(rd, r0, r1)		jit_orr_l(rd, r0, r1)
__jit_inline void
jit_orr_l(int rd, int r0, int r1)
{
    if (r0 == r1)
	jit_movr_l(rd, r0);
    else if (rd == r0)
	ORQrr(r1, rd);
    else if (rd == r1)
	ORQrr(r0, rd);
    else {
	MOVQrr(r0, rd);
	ORQrr(r1, rd);
    }
}

#define jit_xori_l(rd, r0, i0)		jit_xori_l(rd, r0, i0)
__jit_inline void
jit_xori_l(int rd, int r0, long i0)
{
    if (i0 == 0)
	jit_movr_l(rd, r0);
    else if (i0 == -1) {
	jit_movr_l(rd, r0);
	NOTQr(rd);
    }
    else {
	if (jit_can_sign_extend_char_p(i0)) {
	    jit_movr_l(rd, r0);
	    XORBir(i0, rd);
	}
	else if (jit_can_sign_extend_int_p(i0)) {
	    jit_movr_l(rd, r0);
	    XORQir(i0, rd);
	}
	else {
	    if (rd == r0) {
		MOVQir(i0, JIT_REXTMP);
		XORQrr(JIT_REXTMP, rd);
	    }
	    else {
		MOVQir(i0, rd);
		XORQrr(r0, rd);
	    }
	}
    }
}

#define jit_xorr_l(rd, r0, r1)		jit_xorr_l(rd, r0, r1)
__jit_inline void
jit_xorr_l(int rd, int r0, int r1)
{
    if (r0 == r1) {
	if (rd != r0)
	    MOVQrr(r0, rd);
	else
	    XORQrr(rd, rd);
    }
    else if (rd == r0)
	XORQrr(r1, rd);
    else if (rd == r1)
	XORQrr(r0, rd);
    else {
	MOVQrr(r0, rd);
	XORQrr(r1, rd);
    }
}

#define jit_muli_l(rd, r0, i0)		jit_muli_l(rd, r0, i0)
#define jit_muli_ul(rd, r0, i0)		jit_muli_l(rd, r0, i0)
__jit_inline void
jit_muli_l(int rd, int r0, long i0)
{
    if (i0 == 0)
	XORQrr(rd, rd);
    else if (i0 == 1)
	jit_movr_l(rd, r0);
    else if (i0 == -1)
	jit_negr_l(rd, r0);
    else if (jit_can_sign_extend_char_p(i0))
	IMULBQQirr(i0, r0, rd);
    else if (jit_can_sign_extend_int_p(i0))
	IMULLQQirr(i0, r0, rd);
    else if (rd == r0) {
	MOVQir(i0, JIT_REXTMP);
	IMULQrr(JIT_REXTMP, rd);
    }
    else {
	MOVQir(i0, rd);
	IMULQrr(r0, rd);
    }
}

#define jit_mulr_l(rd, r0, r1)		jit_mulr_l(rd, r0, r1)
#define jit_mulr_ul(rd, r0, r1)		jit_mulr_l(rd, r0, r1)
__jit_inline void
jit_mulr_l(int rd, int r0, int r1)
{
    if (rd == r0)
	IMULQrr(r1, rd);
    else if (rd == r1)
	IMULQrr(r0, rd);
    else {
	MOVQrr(r0, rd);
	IMULQrr(r1, rd);
    }
}

/*  Instruction format is:
 *	imul reg64/mem64
 *  and the result is stored in %rdx:%rax
 *  %rax = low 64 bits
 *  %rdx = high 64 bits
 */
__jit_inline void
jit_muli_l_(int r0, long i0)
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

#define jit_hmuli_l(rd, r0, i0)		jit_hmuli_l(rd, r0, i0)
__jit_inline void
jit_hmuli_l(int rd, int r0, long i0)
{
    if (rd == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_l_(r0, i0);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (rd == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_muli_l_(r0, i0);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_muli_l_(r0, i0);
	MOVQrr(_RDX, rd);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

__jit_inline void
jit_mulr_l_(int r0, int r1)
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

#define jit_hmulr_l(rd, r0, r1)		jit_hmulr_l(rd, r0, r1)
__jit_inline void
jit_hmulr_l(int rd, int r0, int r1)
{
    if (rd == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_l_(r0, r1);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (rd == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_mulr_l_(r0, r1);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_mulr_l_(r0, r1);
	MOVQrr(_RDX, rd);
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
jit_muli_ul_(int r0, unsigned long i0)
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

#define jit_hmuli_ul(rd, r0, i0)	jit_hmuli_ul(rd, r0, i0)
__jit_inline void
jit_hmuli_ul(int rd, int r0, unsigned long i0)
{
    if (rd == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_ul_(r0, i0);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (rd == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_muli_ul_(r0, i0);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_muli_ul_(r0, i0);
	MOVQrr(_RDX, rd);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

__jit_inline void
jit_mulr_ul_(int r0, int r1)
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

#define jit_hmulr_ul(rd, r0, r1)	jit_hmulr_ul(rd, r0, r1)
__jit_inline void
jit_hmulr_ul(int rd, int r0, int r1)
{
    if (rd == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_ul_(r0, r1);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (rd == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_mulr_ul_(r0, r1);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_mulr_ul_(r0, r1);
	MOVQrr(_RDX, rd);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}

__jit_inline void
_jit_divi_l_(int rd, int r0, long i0, int is_signed, int is_divide)
{
    int		div;

    if (rd == _RAX) {
	jit_pushr_l(_RDX);
	div = JIT_REXTMP;
    }
    else if (rd == _RDX) {
	jit_pushr_l(_RAX);
	div = JIT_REXTMP;
    }
    else if (rd == r0) {
	jit_pushr_l(_RDX);
	jit_pushr_l(_RAX);
	div = JIT_REXTMP;
    }
    else {
	jit_pushr_l(_RDX);
	MOVQrr(_RAX, JIT_REXTMP);
	div = rd;
    }

    MOVQir(i0, div);
    jit_movr_l(_RAX, r0);

    if (is_signed) {
	CQO_();
	IDIVQr(div);
    }
    else {
	XORQrr(_RDX, _RDX);
	DIVQr(div);
    }

    if (rd != _RAX) {
	if (is_divide)
	    MOVQrr(_RAX, rd);
	if (div == JIT_REXTMP)
	    jit_popr_l(_RAX);
	else
	    MOVQrr(JIT_REXTMP, _RAX);
    }
    if (rd != _RDX) {
	if (!is_divide)
	    MOVQrr(_RDX, rd);
	jit_popr_l(_RDX);
    }
}

__jit_inline void
_jit_divr_l_(int rd, int r0, int r1, int is_signed, int is_divide)
{
    int		div;

    div = (r1 == _RAX || r1 == _RDX) ? JIT_REXTMP : r1;
    if (rd == _RAX)
	jit_pushr_l(_RDX);
    else if (rd == _RDX)
	jit_pushr_l(_RAX);
    else if (div == JIT_REXTMP) {
	jit_pushr_l(_RAX);
	jit_pushr_l(_RDX);
    }
    else {
	jit_pushr_l(_RDX);
	MOVQrr(_RAX, JIT_REXTMP);
    }
    jit_movr_l(div, r1);
    jit_movr_l(_RAX, r0);

    if (is_signed) {
	CQO_();
	IDIVQr(div);
    }
    else {
	XORQrr(_RDX, _RDX);
	DIVQr(div);
    }

    if (rd != _RAX) {
	if (is_divide)
	    MOVQrr(_RAX, rd);
	if (div == JIT_REXTMP)
	    jit_popr_l(_RAX);
	else
	    MOVQrr(JIT_REXTMP, _RAX);
    }
    if (rd != _RDX) {
	if (!is_divide)
	    MOVQrr(_RDX, rd);
	jit_popr_l(_RDX);
    }
}

#define jit_divi_l(rd, r0, i0)		jit_divi_l(rd, r0, i0)
__jit_inline void
jit_divi_l(int rd, int r0, long i0)
{
    _jit_divi_l_(rd, r0, i0, 1, 1);
}

#define jit_divr_l(rd, r0, r1)		jit_divr_l(rd, r0, r1)
__jit_inline void
jit_divr_l(int rd, int r0, int r1)
{
    _jit_divr_l_(rd, r0, r1, 1, 1);
}

#define jit_divi_ul(rd, r0, i0)		jit_divi_ul(rd, r0, i0)
__jit_inline void
jit_divi_ul(int rd, int r0, unsigned long i0)
{
    _jit_divi_l_(rd, r0, i0, 0, 1);
}

#define jit_divr_ul(rd, r0, r1)		jit_divr_ul(rd, r0, r1)
__jit_inline void
jit_divr_ul(int rd, int r0, int r1)
{
    _jit_divr_l_(rd, r0, r1, 0, 1);
}

#define jit_modi_l(rd, r0, i0)		jit_modi_l(rd, r0, i0)
__jit_inline void
jit_modi_l(int rd, int r0, long i0)
{
    _jit_divi_l_(rd, r0, i0, 1, 0);
}

#define jit_modr_l(rd, r0, r1)		jit_modr_l(rd, r0, r1)
__jit_inline void
jit_modr_l(int rd, int r0, int r1)
{
    _jit_divr_l_(rd, r0, r1, 1, 0);
}

#define jit_modi_ul(rd, r0, i0)		jit_modi_ul(rd, r0, i0)
__jit_inline void
jit_modi_ul(int rd, int r0, unsigned long i0)
{
    _jit_divi_l_(rd, r0, i0, 0, 0);
}

#define jit_modr_ul(rd, r0, r1)		jit_modr_ul(rd, r0, r1)
__jit_inline void
jit_modr_ul(int rd, int r0, int r1)
{
    _jit_divr_l_(rd, r0, r1, 0, 0);
}

/*  Instruction format is:
 *  <shift> %r0 %r1
 *	%r0 <shift>= %r1
 *  only %cl can be used as %r1
 */
__jit_inline void
_jit_shift64(int rd, int r0, int r1, int code)
{
    if (rd == _RCX) {
	MOVQrr(r0, JIT_REXTMP);
	if (r1 != _RCX)
	    MOVBrr(r1, _RCX);
	_ROTSHIQrr(code, _RCX, JIT_REXTMP);
	MOVQrr(JIT_REXTMP, _RCX);
    }
    else if (r1 != _RCX) {
	MOVQrr(_RCX, JIT_REXTMP);
	MOVBrr(r1, _RCX);
	jit_movr_l(rd, r0);
	_ROTSHIQrr(code, _RCX, rd);
	MOVQrr(JIT_REXTMP, _RCX);
    }
    else {
	jit_movr_l(rd, r0);
	_ROTSHIQrr(code, _RCX, rd);
    }
}

#define jit_lshi_l(rd, r0, i0)		jit_lshi_l(rd, r0, i0)
__jit_inline void
jit_lshi_l(int rd, int r0, unsigned char i0)
{
    if (i0 == 0)
	jit_movr_l(rd, r0);
    else if (i0 <= 3)
	LEAQmr(0, 0, r0, 1 << i0, rd);
    else {
	jit_movr_l(rd, r0);
	SHLQir(i0, rd);
    }
}

#define jit_lshr_l(rd, r1, r2)		jit_lshr_l(rd, r1, r2)
__jit_inline void
jit_lshr_l(int rd, int r0, int r1)
{
    _jit_shift64(rd, r0, r1, X86_SHL);
}

#define jit_rshi_l(rd, r0, i0)		jit_rshi_l(rd, r0, i0)
__jit_inline void
jit_rshi_l(int rd, int r0, unsigned char i0)
{
    jit_movr_l(rd, r0);
    if (i0)
	SARQir(i0, rd);
}

#define jit_rshr_l(rd, r1, r2)		jit_rshr_l(rd, r1, r2)
__jit_inline void
jit_rshr_l(int rd, int r0, int r1)
{
    _jit_shift64(rd, r0, r1, X86_SAR);
}

#define jit_rshi_ul(rd, r0, i0)		jit_rshi_ul(rd, r0, i0)
__jit_inline void
jit_rshi_ul(int rd, int r0, unsigned char i0)
{
    jit_movr_l(rd, r0);
    if (i0)
	SHRQir(i0, rd);
}

#define jit_rshr_ul(rd, r1, r2)		jit_rshr_ul(rd, r1, r2)
__jit_inline void
jit_rshr_ul(int rd, int r0, int r1)
{
    _jit_shift64(rd, r0, r1, X86_SHR);
}

/* Boolean */
__jit_inline void
_jit_cmp_ri64(int rd, int r0, long i0, int code)
{
    int		same = rd == r0;

    if (!same)
	/* XORLrr is cheaper */
	XORLrr(rd, rd);
    if (jit_can_sign_extend_int_p(i0))
	CMPQir(i0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	CMPQrr(JIT_REXTMP, r0);
    }
    if (same)
	/* MOVLir is cheaper */
	MOVLir(0, rd);
    SETCCir(code, rd);
}

__jit_inline void
_jit_test_r64(int rd, int r0, int code)
{
    int		same = rd == r0;

    if (!same)
	XORLrr(rd, rd);
    TESTQrr(r0, r0);
    if (same)
	MOVLir(0, rd);
    SETCCir(code, rd);
}

__jit_inline void
_jit_cmp_rr64(int rd, int r0, int r1, int code)
{
    int		same = rd == r0 || rd == r1;

    if (!same)
	XORLrr(rd, rd);
    CMPQrr(r1, r0);
    if (same)
	MOVLir(0, rd);
    SETCCir(code, rd);
}

#define jit_lti_l(rd, r0, i0)		jit_lti_l(rd, r0, i0)
__jit_inline void
jit_lti_l(int rd, int r0, long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_L);
    else
	_jit_test_r64(rd, r0,		X86_CC_S);
}

#define jit_ltr_l(rd, r0, r1)		jit_ltr_l(rd, r0, r1)
__jit_inline void
jit_ltr_l(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_L);
}

#define jit_lei_l(rd, r0, i0)		jit_lei_l(rd, r0, i0)
__jit_inline void
jit_lei_l(int rd, int r0, long i0)
{
    _jit_cmp_ri64(rd, r0, i0,		X86_CC_LE);
}

#define jit_ler_l(rd, r0, r1)		jit_ler_l(rd, r0, r1)
__jit_inline void
jit_ler_l(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_LE);
}

#define jit_eqi_l(rd, r0, i0)		jit_eqi_l(rd, r0, i0)
__jit_inline void
jit_eqi_l(int rd, int r0, long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_E);
    else
	_jit_test_r64(rd, r0,		X86_CC_E);
}

#define jit_eqr_l(rd, r0, r1)		jit_eqr_l(rd, r0, r1)
__jit_inline void
jit_eqr_l(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_E);
}

#define jit_gei_l(rd, r0, i0)		jit_gei_l(rd, r0, i0)
__jit_inline void
jit_gei_l(int rd, int r0, long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_GE);
    else
	_jit_test_r64(rd, r0,		X86_CC_NS);
}

#define jit_ger_l(rd, r0, r1)		jit_ger_l(rd, r0, r1)
__jit_inline void
jit_ger_l(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_GE);
}

#define jit_gti_l(rd, r0, i0)		jit_gti_l(rd, r0, i0)
__jit_inline void
jit_gti_l(int rd, int r0, long i0)
{
    _jit_cmp_ri64(rd, r0, i0,		X86_CC_G);
}

#define jit_gtr_l(rd, r0, r1)		jit_gtr_l(rd, r0, r1)
__jit_inline void
jit_gtr_l(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_G);
}

#define jit_nei_l(rd, r0, i0)		jit_nei_l(rd, r0, i0)
__jit_inline void
jit_nei_l(int rd, int r0, long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_NE);
    else
	_jit_test_r64(rd, r0,		X86_CC_NE);
}

#define jit_ner_l(rd, r0, r1)		jit_ner_l(rd, r0, r1)
__jit_inline void
jit_ner_l(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_NE);
}

#define jit_lti_ul(rd, r0, i0)		jit_lti_ul(rd, r0, i0)
__jit_inline void
jit_lti_ul(int rd, int r0, unsigned long i0)
{
    _jit_cmp_ri64(rd, r0, i0,		X86_CC_B);
}

#define jit_ltr_ul(rd, r0, r1)		jit_ltr_ul(rd, r0, r1)
__jit_inline void
jit_ltr_ul(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_B);
}

#define jit_lei_ul(rd, r0, i0)		jit_lei_ul(rd, r0, i0)
__jit_inline void
jit_lei_ul(int rd, int r0, unsigned long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_BE);
    else
	_jit_test_r64(rd, r0,		X86_CC_E);
}

#define jit_ler_ul(rd, r0, r1)		jit_ler_ul(rd, r0, r1)
__jit_inline void
jit_ler_ul(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_BE);
}

#define jit_gei_ul(rd, r0, i0)		jit_gei_ul(rd, r0, i0)
__jit_inline void
jit_gei_ul(int rd, int r0, unsigned long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_AE);
    else
	_jit_test_r64(rd, r0,		X86_CC_NB);
}

#define jit_ger_ul(rd, r0, r1)		jit_ger_ul(rd, r0, r1)
__jit_inline void
jit_ger_ul(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_AE);
}

#define jit_gti_ul(rd, r0, i0)		jit_gti_ul(rd, r0, i0)
__jit_inline void
jit_gti_ul(int rd, int r0, unsigned long i0)
{
    if (i0)
	_jit_cmp_ri64(rd, r0, i0,	X86_CC_A);
    else
	_jit_test_r64(rd, r0,		X86_CC_NE);
}

#define jit_gtr_ul(rd, r0, r1)		jit_gtr_ul(rd, r0, r1)
__jit_inline void
jit_gtr_ul(int rd, int r0, int r1)
{
    _jit_cmp_rr64(rd, r0, r1,		X86_CC_A);
}

/* Jump */
__jit_inline void
_jit_bcmp_ri64(jit_insn *label, int r0, long i0, int code)
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
_jit_btest_r64(jit_insn *label, int r0, int code)
{
    TESTQrr(r0, r0);
    JCCim(code, label);
}

__jit_inline void
_jit_bcmp_rr64(jit_insn *label, int r0, int r1, int code)
{
    CMPQrr(r1, r0);
    JCCim(code, label);
}

#define jit_blti_l(label, r0, i0)	jit_blti_l(label, r0, i0)
__jit_inline jit_insn *
jit_blti_l(jit_insn *label, int r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_L);
    else
	_jit_btest_r64(label, r0,	X86_CC_S);
    return (_jit.x.pc);
}

#define jit_bltr_l(label, r0, r1)	jit_bltr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_l(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_L);
    return (_jit.x.pc);
}

#define jit_blei_l(label, r0, i0)	jit_blei_l(label, r0, i0)
__jit_inline jit_insn *
jit_blei_l(jit_insn *label, int r0, long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_bler_l(label, r0, r1)	jit_bler_l(label, r0, r1)
__jit_inline jit_insn *
jit_bler_l(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_LE);
    return (_jit.x.pc);
}

#define jit_beqi_l(label, r0, i0)	jit_beqi_l(label, r0, i0)
__jit_inline jit_insn *
jit_beqi_l(jit_insn *label, int r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_E);
    else
	_jit_btest_r64(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_beqr_l(label, r0, r1)	jit_beqr_l(label, r0, r1)
__jit_inline jit_insn *
jit_beqr_l(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bgei_l(label, r0, i0)	jit_bgei_l(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_l(jit_insn *label, int r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_GE);
    else
	_jit_btest_r64(label, r0,	X86_CC_NS);
    return (_jit.x.pc);
}

#define jit_bger_l(label, r0, r1)	jit_bger_l(label, r0, r1)
__jit_inline jit_insn *
jit_bger_l(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_GE);
    return (_jit.x.pc);
}

#define jit_bgti_l(label, r0, i0)	jit_bgti_l(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_l(jit_insn *label, int r0, long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bgtr_l(label, r0, r1)	jit_bgtr_l(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_l(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_G);
    return (_jit.x.pc);
}

#define jit_bnei_l(label, r0, i0)	jit_bnei_l(label, r0, i0)
__jit_inline jit_insn *
jit_bnei_l(jit_insn *label, int r0, long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_NE);
    else
	_jit_btest_r64(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bner_l(label, r0, r1)	jit_bner_l(label, r0, r1)
__jit_inline jit_insn *
jit_bner_l(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_blti_ul(label, r0, i0)	jit_blti_ul(label, r0, i0)
__jit_inline jit_insn *
jit_blti_ul(jit_insn *label, int r0, unsigned long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_bltr_ul(label, r0, r1)	jit_bltr_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bltr_ul(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_B);
    return (_jit.x.pc);
}

#define jit_blei_ul(label, r0, i0)	jit_blei_ul(label, r0, i0)
__jit_inline jit_insn *
jit_blei_ul(jit_insn *label, int r0, unsigned long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_BE);
    else
	_jit_btest_r64(label, r0,	X86_CC_E);
    return (_jit.x.pc);
}

#define jit_bler_ul(label, r0, r1)	jit_bler_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bler_ul(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_BE);
    return (_jit.x.pc);
}

#define jit_bgei_ul(label, r0, i0)	jit_bgei_ul(label, r0, i0)
__jit_inline jit_insn *
jit_bgei_ul(jit_insn *label, int r0, unsigned long i0)
{
    _jit_bcmp_ri64(label, r0, i0,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bger_ul(label, r0, r1)	jit_bger_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bger_ul(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_AE);
    return (_jit.x.pc);
}

#define jit_bgti_ul(label, r0, i0)	jit_bgti_ul(label, r0, i0)
__jit_inline jit_insn *
jit_bgti_ul(jit_insn *label, int r0, unsigned long i0)
{
    if (i0)
	_jit_bcmp_ri64(label, r0, i0,	X86_CC_A);
    else
	_jit_btest_r64(label, r0,	X86_CC_NE);
    return (_jit.x.pc);
}

#define jit_bgtr_ul(label, r0, r1)	jit_bgtr_ul(label, r0, r1)
__jit_inline jit_insn *
jit_bgtr_ul(jit_insn *label, int r0, int r1)
{
    _jit_bcmp_rr64(label, r0, r1,	X86_CC_A);
    return (_jit.x.pc);
}

#define jit_boaddi_l(label, r0, i0)	jit_boaddi_l(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_l(jit_insn *label, int r0, long i0)
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
jit_boaddr_l(jit_insn *label, int r0, int r1)
{
    ADDQrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_l(label, r0, i0)	jit_bosubi_l(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_l(jit_insn *label, int r0, long i0)
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
jit_bosubr_l(jit_insn *label, int r0, int r1)
{
    SUBQrr(r1, r0);
    JOm(label);
    return (_jit.x.pc);
}

#define jit_boaddi_ul(label, r0, i0)	jit_boaddi_ul(label, r0, i0)
__jit_inline jit_insn *
jit_boaddi_ul(jit_insn *label, int r0, unsigned long i0)
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
jit_boaddr_ul(jit_insn *label, int r0, int r1)
{
    ADDQrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bosubi_ul(label, r0, i0)	jit_bosubi_ul(label, r0, i0)
__jit_inline jit_insn *
jit_bosubi_ul(jit_insn *label, int r0, unsigned long i0)
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
jit_bosubr_ul(jit_insn *label, int r0, int r1)
{
    SUBQrr(r1, r0);
    JCm(label);
    return (_jit.x.pc);
}

#define jit_bmsi_l(label, r0, i0)	jit_bmsi_l(label, r0, i0)
__jit_inline jit_insn *
jit_bmsi_l(jit_insn *label, int r0, long i0)
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
jit_bmsr_l(jit_insn *label, int r0, int r1)
{
    TESTQrr(r1, r0);
    JNZm(label);
    return (_jit.x.pc);
}

#define jit_bmci_l(label, r0, i0)	jit_bmci_l(label, r0, i0)
__jit_inline jit_insn *
jit_bmci_l(jit_insn *label, int r0, long i0)
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
jit_bmcr_l(jit_insn *label, int r0, int r1)
{
    TESTQrr(r1, r0);
    JZm(label);
    return (_jit.x.pc);
}

/* Memory */
#define jit_ntoh_ul(d, rs)		jit_ntoh_ul(d, rs)
__jit_inline void
jit_ntoh_ul(int rd, int r0)
{
    jit_movr_l(rd, r0);
    BSWAPQr(rd);
}

#define jit_ldr_c(r0, r1)		jit_ldr_c(r0, r1)
__jit_inline void
jit_ldr_c(int r0, int r1)
{
    MOVSBQmr(0, r1, 0,  0, r0);
}

#define jit_ldxr_c(r0, r1, r2)		jit_ldxr_c(r0, r1, r2)
__jit_inline void
jit_ldxr_c(int r0, int r1, int r2)
{
    MOVSBQmr(0, r1, r2, 1, r0);
}

#define jit_ldr_s(r0, r1)		jit_ldr_s(r0, r1)
__jit_inline void
jit_ldr_s(int r0, int r1)
{
    MOVSWQmr(0, r1, 0, 0, r0);
}

#define jit_ldxr_s(r0, r1, r2)		jit_ldxr_s(r0, r1, r2)
__jit_inline void
jit_ldxr_s(int r0, int r1, int r2)
{
    MOVSWQmr(0, r1, r2, 1, r0);
}

#define jit_ldi_c(r0, i0)		jit_ldi_c(r0, i0)
__jit_inline void
jit_ldi_c(int r0, void *i0)
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
jit_ldxi_c(int r0, int r1, int i0)
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
jit_ldi_uc(int r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVZBLmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldr_uc(r0, JIT_REXTMP);
    }
}

#define jit_ldxi_uc(r0, r1, i0)		jit_ldxi_uc(r0, r1, i0)
__jit_inline void
jit_ldxi_uc(int r0, int r1, int i0)
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
jit_str_c(int r0, int r1)
{
    MOVBrm(r1, 0, r0, 0, 0);
}

#define jit_sti_c(i0, r0)		jit_sti_c(i0, r0)
__jit_inline void
jit_sti_c(void *i0, int r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVBrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_str_c(JIT_REXTMP, r0);
    }
}

#define jit_stxr_c(r0, r1, r2)		jit_stxr_c(r0, r1, r2)
__jit_inline void
jit_stxr_c(int r0, int r1, int r2)
{
    MOVBrm(r2, 0, r0, r1, 1);
}

#define jit_stxi_c(i0, r0, r1)		jit_stxi_c(i0, r0, r1)
__jit_inline void
jit_stxi_c(int i0, int r0, int r1)
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
jit_ldi_s(int r0, void *i0)
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
jit_ldxi_s(int r0, int r1, int i0)
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
jit_ldi_us(int r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVZWLmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldr_us(r0, JIT_REXTMP);
    }
}

#define jit_ldxi_us(r0, r1, i0)		jit_ldxi_us(r0, r1, i0)
__jit_inline void
jit_ldxi_us(int r0, int r1, int i0)
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
jit_sti_s(void *i0, int r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVWrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_str_s(JIT_REXTMP, r0);
    }
}

#define jit_stxi_s(i0, r0, r1)		jit_stxi_s(i0, r0, r1)
__jit_inline void
jit_stxi_s(int i0, int r0, int r1)
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
jit_ldr_i(int r0, int r1)
{
    MOVSLQmr(0, r1, 0, 0, r0);
}

#define jit_ldi_i(r0, i0)		jit_ldi_i(r0, i0)
__jit_inline void
jit_ldi_i(int r0, void *i0)
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
jit_ldxr_i(int r0, int r1, int r2)
{
    MOVSLQmr(0, r1, r2, 1, r0);
}

#define jit_ldxi_i(r0, r1, i0)		jit_ldxi_i(r0, r1, i0)
__jit_inline void
jit_ldxi_i(int r0, int r1, int i0)
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
jit_ldr_ui(int r0, int r1)
{
    MOVLmr(0, r1, 0, 0, r0);
}

#define jit_ldi_ui(r0, i0)		jit_ldi_ui(r0, i0)
__jit_inline void
jit_ldi_ui(int r0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVLmr((long)i0, 0, 0, 0, r0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_ldr_ui(r0, JIT_REXTMP);
    }
}

#define jit_ldxr_ui(r0, r1, r2)		jit_ldxr_ui(r0, r1, r2)
__jit_inline void
jit_ldxr_ui(int r0, int r1, int r2)
{
    MOVLmr(0, r1, r2, 1, r0);
}

#define jit_ldxi_ui(r0, r1, i0)		jit_ldxi_ui(r0, r1, i0)
__jit_inline void
jit_ldxi_ui(int r0, int r1, int i0)
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
jit_sti_i(void *i0, int r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVLrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_str_i(JIT_REXTMP, r0);
    }
}

#define jit_stxi_i(i0, r0, r1)		jit_stxi_i(i0, r0, r1)
__jit_inline void
jit_stxi_i(int i0, int r0, int r1)
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
jit_ldr_l(int r0, int r1)
{
    MOVQmr(0, r1, 0, 0, r0);
}

#define jit_ldi_l(r0, i0)		jit_ldi_l(r0, i0)
__jit_inline void
jit_ldi_l(int r0, void *i0)
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
jit_ldxr_l(int r0, int r1, int r2)
{
    MOVQmr(0, r1, r2, 1, r0);
}

#define jit_ldxi_l(r0, r1, i0)		jit_ldxi_l(r0, r1, i0)
#define jit_ldxi_ul(r0, r1, i0)		jit_ldxi_l(r0, r1, i0)
#define jit_ldxi_p(r0, r1, i0)		jit_ldxi_l(r0, r1, i0)
__jit_inline void
jit_ldxi_l(int r0, int r1, int i0)
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
jit_str_l(int r0, int r1)
{
    MOVQrm(r1, 0, r0, 0, 0);
}

#define jit_sti_l(i0, r0)		jit_sti_l(i0, r0)
__jit_inline void
jit_sti_l(void *i0, int r0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVQrm(r0, (long)i0, 0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_str_l(JIT_REXTMP, r0);
    }
}

#define jit_stxr_l(r0, r1, r2)		jit_stxr_l(r0, r1, r2)
__jit_inline void
jit_stxr_l(int r0, int r1, int r2)
{
    MOVQrm(r2, 0, r0, r1, 1);
}

#define jit_stxi_l(i0, r0, r1)		jit_stxi_l(i0, r0, r1)
__jit_inline void
jit_stxi_l(int i0, int r0, int r1)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVQrm(r1, i0, r0, 0, 0);
    else {
	MOVQir(i0, JIT_REXTMP);
	jit_stxr_l(JIT_REXTMP, r0, r1);
    }
}

#define jit_extr_c_l(rd, r0)		jit_extr_c_l(rd, r0)
__jit_inline void
jit_extr_c_l(int rd, int r0)
{
    MOVSBQrr(r0, rd);
}

#define jit_extr_c_ul(rd, r0)		jit_extr_c_ul(rd, r0)
__jit_inline void
jit_extr_c_ul(int rd, int r0)
{
    MOVZBQrr(r0, rd);
}

#define jit_extr_s_l(rd, r0)		jit_extr_s_l(rd, r0)
__jit_inline void
jit_extr_s_l(int rd, int r0)
{
    MOVSWQrr(r0, rd);
}

#define jit_extr_s_ul(rd, r0)		jit_extr_s_ul(rd, r0)
__jit_inline void
jit_extr_s_ul(int rd, int r0)
{
    MOVZWQrr(r0, rd);
}

#define jit_extr_s_l(rd, r0)		jit_extr_s_l(rd, r0)
__jit_inline void
jit_extr_i_l(int rd, int r0)
{
    MOVSLQrr(r0, rd);
}

#define jit_extr_s_ul(rd, r0)		jit_extr_s_ul(rd, r0)
__jit_inline void
jit_extr_i_ul(int rd, int r0)
{
    MOVLrr(r0, rd);
}

#define jit_retval_l(rd)		jit_retval_l(rd)
__jit_inline void
jit_retval_l(int rd)
{
    jit_movr_l(rd, _RAX);
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

#define jit_getarg_c(rd, ofs)		jit_getarg_c(rd, ofs)
__jit_inline void
jit_getarg_c(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_c_l(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_c(rd, JIT_FP, ofs);
}

#define jit_getarg_uc(rd, ofs)		jit_getarg_uc(rd, ofs)
__jit_inline void
jit_getarg_uc(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_c_ul(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_uc(rd, JIT_FP, ofs);
}

#define jit_getarg_s(rd, ofs)		jit_getarg_s(rd, ofs)
__jit_inline void
jit_getarg_s(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_s_l(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_s(rd, JIT_FP, ofs);
}

#define jit_getarg_us(rd, ofs)		jit_getarg_us(rd, ofs)
__jit_inline void
jit_getarg_us(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_extr_s_ul(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_us(rd, JIT_FP, ofs);
}

#define jit_getarg_i(rd, ofs)		jit_getarg_i(rd, ofs)
__jit_inline void
jit_getarg_i(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_l(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_i(rd, JIT_FP, ofs);
}

#define jit_getarg_ui(rd, ofs)		jit_getarg_ui(rd, ofs)
__jit_inline void
jit_getarg_ui(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_ul(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_ui(rd, JIT_FP, ofs);
}

#define jit_getarg_l(rd, ofs)		jit_getarg_l(rd, ofs)
__jit_inline void
jit_getarg_l(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_l(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_l(rd, JIT_FP, ofs);
}

#define jit_getarg_ul(rd, ofs)		jit_getarg_ul(rd, ofs)
__jit_inline void
jit_getarg_ul(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_ul(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_ul(rd, JIT_FP, ofs);
}

#define jit_getarg_p(rd, ofs)		jit_getarg_p(rd, ofs)
__jit_inline void
jit_getarg_p(int rd, int ofs)
{
    if (ofs < JIT_ARG_MAX)
	jit_movr_p(rd, jit_arg_reg_order[ofs]);
    else
	jit_ldxi_p(rd, JIT_FP, ofs);
}

#endif /* __lightning_core_h */
