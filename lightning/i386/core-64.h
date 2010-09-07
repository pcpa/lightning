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

/* Used to implement ldc, stc, ... */
#define JIT_CAN_16 0
#define JIT_REXTMP		_R12

/* Number or integer argument registers */
#define JIT_ARG_MAX		6

/* Number of float argument registers */
#define JIT_FP_ARG_MAX		8

#define JIT_R_NUM		3
#define JIT_R(i)                ((i) == 0 ? _RAX : _R9 + (i))
#define JIT_V_NUM               3
#define JIT_V(i)                ((i) == 0 ? _RBX : _R12 + (i))

/* Whether a register in the "low" bank is used for the user-accessible
   registers.  */
#define jit_save(reg)		((reg) == _EAX || (reg) == _EBX)

/* Keep the stack 16-byte aligned, the SSE hardware prefers it this way.  */
#define jit_allocai_internal(amount, slack)                           \
  (((amount) < _jitl.alloca_slack                                     \
    ? 0                                                               \
    : (_jitl.alloca_slack += (amount) + (slack),                      \
      SUBQir((amount) + (slack), _ESP))),                             \
   _jitl.alloca_slack -= (amount),                                    \
   _jitl.alloca_offset -= (amount))

#define jit_allocai(n)                                                \
  jit_allocai_internal ((n), (_jitl.alloca_slack - (n)) & 15)

/* 3-parameter operation */
#define jit_bra_qr(s1, s2, op)		(CMPQrr(s2, s1), op, _jit.x.pc)
#define _jit_bra_l(rs, is, op)		(CMPQir(is, rs), op, _jit.x.pc)

#define jit_bra_l(rs, is, op) (_s32P((long)(is)) \
                               ? _jit_bra_l(rs, is, op) \
                               : (MOVQir(is, JIT_REXTMP), jit_bra_qr(rs, JIT_REXTMP, op)))

/* When CMP with 0 can be replaced with TEST */
#define jit_bra_l0(rs, is, op, op0)					\
	( (is) == 0 ? (TESTQrr(rs, rs), op0, _jit.x.pc) : jit_bra_l(rs, is, op))

#define jit_reduceQ(op, is, rs)							\
	(_u8P(is) ? jit_reduce_(op##Bir(is, jit_reg8(rs))) :	\
	jit_reduce_(op##Qir(is, rs)) )

#define jit_movi_p(d, is)	(MOVQir(((long)(is)), (d)), _jit.x.pc)
#define jit_movi_l(d, is)						\
    /* Value is not zero? */						\
    ((is)								\
	/* Value is unsigned and fits in unsigned 32 bits? */		\
	? (_u32P(is)							\
	    /* Use zero extending 32 bits opcode */			\
	    ? MOVLir(is, (d))						\
	    /* Use sign extending 64 bits opcode */			\
	    : MOVQir(is, (d)))						\
	/* Set register to zero. */					\
	: XORQrr((d), (d)))
#define jit_movr_l(d, rs)	((void)((rs) == (d) ? 0 : MOVQrr((rs), (d))))
#define jit_movi_ul(d, is)	jit_movi_l(d, is)

/* Alias some macros and add casts to ensure proper zero and sign extension */
#define jit_movi_i(d, is)	jit_movi_l(d, (long)(int)is)
#define jit_movi_ui(d, rs)	jit_movi_l((d), (_ul)(_ui)(rs))

#define jit_pushr_l(rs)		jit_pushr_i(rs)
#define jit_popr_l(rs)		jit_popr_i(rs)

/* Return address is 8 bytes, plus 5 registers = 40 bytes, total = 48 bytes. */
#define jit_prolog(n)							\
     /* Initialize counter of integer arguments */			\
    (_jitl.nextarg_puti = (n),						\
     /* Initialize counter of stack arguments */			\
     _jitl.argssize = (_jitl.nextarg_puti > JIT_ARG_MAX)		\
	? _jitl.nextarg_puti - JIT_ARG_MAX : 0,				\
     /* Initialize offset of stack arguments */				\
     _jitl.framesize = (_jitl.argssize & 1) ? 56 : 48,			\
     /* Initialize counter of float arguments */			\
     _jitl.nextarg_putfp = 0,						\
     /* Initialize offsets of arguments */				\
     _jitl.nextarg_getfp = _jitl.nextarg_geti = 0,			\
     /* Initialize alloca information */				\
     _jitl.alloca_offset = _jitl.alloca_slack = 0,			\
     /* Build stack frame */						\
     PUSHQr(_EBX), PUSHQr(_R12), PUSHQr(_R13), PUSHQr(_R14), 		\
     PUSHQr(_EBP), MOVQrr(_ESP, _EBP))

#define jit_ret()							\
    (LEAVE_(),								\
     POPQr(_R14), POPQr(_R13), POPQr(_R12), POPQr(_EBX),		\
     RET_())

#define jit_calli(sub)							\
    (MOVQir((long)(sub), JIT_REXTMP),					\
     _jitl.label = _jit.x.pc,						\
     CALLsr(JIT_REXTMP))
#define jit_callr(reg)		CALLsr((reg))
#define jit_patch_calli(pa, pv)	jit_patch_movi(pa, pv)

#define jit_prepare_i(ni)						\
    /* Initialize offset of right to left integer argument */		\
    (_jitl.nextarg_puti = (ni),						\
     /* Initialize float argument offset and register counter */	\
     _jitl.nextarg_putfp = _jitl.fprssize = 0,				\
     /* argssize is used to keep track of stack slots used */		\
     _jitl.argssize = _jitl.nextarg_puti > JIT_ARG_MAX			\
     ? _jitl.nextarg_puti - JIT_ARG_MAX : 0)

#define jit_pusharg_i(rs)						\
    /* Need to use stack for argument? */				\
    (--_jitl.nextarg_puti >= JIT_ARG_MAX				\
       /* Yes. Push it */						\
     ? PUSHQr(rs)							\
       /* No. Use a register */						\
     : MOVQrr(rs, jit_arg_reg_order[_jitl.nextarg_puti]))

#define jit_finish(label)		jit_finish(label)
__jit_inline jit_insn *
jit_finish(jit_insn *label)
{
    if (_jitl.fprssize) {
	MOVBir(_jitl.fprssize, _AL);
	_jitl.fprssize = 0;
    }
    else
	MOVBir(0, _AL);
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

#define jit_reg_is_arg(reg)						\
    (jit_reg64(reg) == _RCX || jit_reg64(reg) == _RDX)

#define jit_finishr(rs)			jit_finishr(rs)
__jit_inline void
jit_finishr(int rs)
{
    int		need_temp;

    if ((need_temp = jit_reg64(rs) == _RAX || jit_reg_is_arg(rs)))
	MOVQrr(rs, JIT_REXTMP);
    if (_jitl.fprssize) {
	MOVBir(_jitl.fprssize, _AL);
	_jitl.fprssize = 0;
    }
    else
	MOVBir(0, _AL);
    if (_jitl.argssize & 1) {
	PUSHQr(_RAX);
	++_jitl.argssize;
    }
    if (need_temp)
	jit_callr(JIT_REXTMP);
    else
	jit_callr(rs);
    if (_jitl.argssize) {
	ADDQir(sizeof(long) * _jitl.argssize, JIT_SP);
	_jitl.argssize = 0;
    }
}

#define jit_retval_l(rd)	((void)jit_movr_l ((rd), _RAX))
#define jit_arg_i()		(_jitl.nextarg_geti < JIT_ARG_MAX \
				 ? _jitl.nextarg_geti++ \
				 : ((_jitl.framesize += sizeof(long)) - sizeof(long)))
#define jit_arg_c()		jit_arg_i()
#define jit_arg_uc()		jit_arg_i()
#define jit_arg_s()		jit_arg_i()
#define jit_arg_us()		jit_arg_i()
#define jit_arg_ui()		jit_arg_i()
#define jit_arg_l()		jit_arg_i()
#define jit_arg_ul()		jit_arg_i()
#define jit_arg_p()		jit_arg_i()

#define jit_getarg_c(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_c_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_c((reg), JIT_FP, (ofs)))
#define jit_getarg_uc(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_uc_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_uc((reg), JIT_FP, (ofs)))
#define jit_getarg_s(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_s_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_s((reg), JIT_FP, (ofs)))
#define jit_getarg_us(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_us_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_us((reg), JIT_FP, (ofs)))
#define jit_getarg_i(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_i((reg), JIT_FP, (ofs)))
#define jit_getarg_ui(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_ui((reg), JIT_FP, (ofs)))
#define jit_getarg_l(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_l((reg), JIT_FP, (ofs)))
#define jit_getarg_ul(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_ul((reg), JIT_FP, ofs))
#define jit_getarg_p(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_p((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_p((reg), JIT_FP, (ofs)))

#define jit_patch_long_at(jump_pc,v)  (*_PSL((jump_pc) - sizeof(long)) = _jit_SL((jit_insn *)(v)))
#define jit_patch_short_at(jump_pc,v)  (*_PSI((jump_pc) - sizeof(int)) = _jit_SI((jit_insn *)(v) - (jump_pc)))
#define jit_patch_at(jump_pc,v) (_jitl.long_jumps ? jit_patch_long_at((jump_pc)-3, v) : jit_patch_short_at(jump_pc, v))

static int jit_arg_reg_order[] = { _EDI, _ESI, _EDX, _ECX, _R8D, _R9D };

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
 * FIXME there are shorter versions when register is %RAX
 */
/* Commutative */
#define jit_addci_ul(rd, r0, i0)	jit_addci_ul(rd, r0, i0)
__jit_inline void
jit_addci_ul(int rd, int r0, unsigned long i0)
{
    if (jit_can_sign_extend_unsigned_int_p(i0)) {
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
    if (jit_can_sign_extend_unsigned_int_p(i0)) {
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
    if (jit_can_sign_extend_unsigned_int_p(i0))
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
    if (rd != r0)
	MOVQrr(r0, rd);
    if (jit_can_sign_extend_unsigned_int_p(i0))
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
	jit_movr_l(rd, r0);
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
	jit_movr_l(rd, r0);
	if (jit_check8(rd) && jit_can_sign_extend_char_p(i0))
	    XORBir(i0, rd);
	else
	    XORQir(i0, rd);
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
    if (jit_reg64(r0) == _RAX) {
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
    if (jit_reg64(rd) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_l_(r0, i0);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(rd) == _RAX) {
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
    if (jit_reg64(r1) == _RAX)
	IMULQr(r0);
    else if (jit_reg64(r0) == _RAX)
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
    if (jit_reg64(rd) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_l_(r0, r1);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(rd) == _RAX) {
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
    if (jit_reg64(r0) == _RAX) {
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
    if (jit_reg64(rd) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_ul_(r0, i0);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(rd) == _RAX) {
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
    if (jit_reg64(r1) == _RAX)
	MULQr(r0);
    else if (jit_reg64(r0) == _RAX)
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
    if (jit_reg64(rd) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_ul_(r0, r1);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(rd) == _RAX) {
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
	    MOVBrr(jit_reg8(r1), _CL);
	_ROTSHIQrr(code, _CL, JIT_REXTMP);
	MOVQrr(JIT_REXTMP, _RCX);
    }
    else if (r1 != _RCX) {
	MOVQrr(_RCX, JIT_REXTMP);
	MOVBrr(jit_reg8(r1), _CL);
	jit_movr_l(rd, r0);
	_ROTSHIQrr(code, _CL, rd);
	MOVQrr(JIT_REXTMP, _RCX);
    }
    else {
	jit_movr_l(rd, r0);
	_ROTSHIQrr(code, _CL, rd);
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
    _jit_shift64(jit_reg64(rd), jit_reg64(r0), jit_reg64(r1), X86_SHL);
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
    _jit_shift64(jit_reg64(rd), jit_reg64(r0), jit_reg64(r1), X86_SAR);
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
    _jit_shift64(jit_reg64(rd), jit_reg64(r0), jit_reg64(r1), X86_SHR);
}

#define jit_bmsr_l(label, s1, s2)	(TESTQrr((s1), (s2)), JNZm(label), _jit.x.pc)
#define jit_bmcr_l(label, s1, s2)	(TESTQrr((s1), (s2)), JZm(label),  _jit.x.pc)
#define jit_boaddr_l(label, s1, s2)	(ADDQrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_bosubr_l(label, s1, s2)	(SUBQrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_boaddr_ul(label, s1, s2)	(ADDQrr((s2), (s1)), JCm(label), _jit.x.pc)
#define jit_bosubr_ul(label, s1, s2)	(SUBQrr((s2), (s1)), JCm(label), _jit.x.pc)

#define jit_boaddi_l(label, rs, is)	(ADDQir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_bosubi_l(label, rs, is)	(SUBQir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_boaddi_ul(label, rs, is)	(ADDQir((is), (rs)), JCm(label), _jit.x.pc)
#define jit_bosubi_ul(label, rs, is)	(SUBQir((is), (rs)), JCm(label), _jit.x.pc)

/* Memory */

/* Used to implement ldc, stc, ... We have SIL and friends which simplify it all.  */
#define jit_movbrm(rs, dd, db, di, ds)         MOVBrm(jit_reg8(rs), dd, db, di, ds)

#define jit_ldr_c(d, rs)                MOVSBQmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_c(d, s1, s2)           MOVSBQmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldr_s(d, rs)                MOVSWQmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_s(d, s1, s2)           MOVSWQmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldi_c(d, is)                (_u32P((long)(is)) ? MOVSBQmr((is), 0,    0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_c(d, JIT_REXTMP)))
#define jit_ldxi_c(d, rs, is)           (_u32P((long)(is)) ? MOVSBQmr((is), (rs), 0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_c(d, rs, JIT_REXTMP)))

#define jit_ldi_uc(d, is)               (_u32P((long)(is)) ? MOVZBLmr((is), 0,    0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_uc(d, JIT_REXTMP)))
#define jit_ldxi_uc(d, rs, is)          (_u32P((long)(is)) ? MOVZBLmr((is), (rs), 0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_uc(d, rs, JIT_REXTMP)))

#define jit_sti_c(id, rs)               (_u32P((long)(id)) ? MOVBrm(jit_reg8(rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_c(JIT_REXTMP, rs)))
#define jit_stxi_c(id, rd, rs)          (_u32P((long)(id)) ? MOVBrm(jit_reg8(rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_c(JIT_REXTMP, rd, rs)))

#define jit_ldi_s(d, is)                (_u32P((long)(is)) ? MOVSWQmr((is), 0,    0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_s(d, JIT_REXTMP)))
#define jit_ldxi_s(d, rs, is)           (_u32P((long)(is)) ? MOVSWQmr((is), (rs), 0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_s(d, rs, JIT_REXTMP)))

#define jit_ldi_us(d, is)               (_u32P((long)(is)) ? MOVZWLmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_us(d, JIT_REXTMP)))
#define jit_ldxi_us(d, rs, is)          (_u32P((long)(is)) ? MOVZWLmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_us(d, rs, JIT_REXTMP)))

#define jit_sti_s(id, rs)               (_u32P((long)(id)) ? MOVWrm(jit_reg16(rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_s(JIT_REXTMP, rs)))
#define jit_stxi_s(id, rd, rs)          (_u32P((long)(id)) ? MOVWrm(jit_reg16(rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_s(JIT_REXTMP, rd, rs)))

#define jit_ldi_ui(d, is)               (_u32P((long)(is)) ? MOVLmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_ui(d, JIT_REXTMP)))
#define jit_ldxi_ui(d, rs, is)          (_u32P((long)(is)) ? MOVLmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_ui(d, rs, JIT_REXTMP)))

#define jit_ldi_i(d, is)                (_u32P((long)(is)) ? MOVSLQmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_i(d, JIT_REXTMP)))
#define jit_ldxi_i(d, rs, is)           (_u32P((long)(is)) ? MOVSLQmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_i(d, rs, JIT_REXTMP)))

#define jit_sti_i(id, rs)               (_u32P((long)(id)) ? MOVLrm((rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_i(JIT_REXTMP, rs)))
#define jit_stxi_i(id, rd, rs)          (_u32P((long)(id)) ? MOVLrm((rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_i(JIT_REXTMP, rd, rs)))

#define jit_ldi_l(d, is)                (_u32P((long)(is)) ? MOVQmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_l(d, JIT_REXTMP)))
#define jit_ldxi_l(d, rs, is)           (_u32P((long)(is)) ? MOVQmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_l(d, rs, JIT_REXTMP)))

#define jit_sti_l(id, rs)               (_u32P((long)(id)) ? MOVQrm((rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_l(JIT_REXTMP, rs)))
#define jit_stxi_l(id, rd, rs)          (_u32P((long)(id)) ? MOVQrm((rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_l(JIT_REXTMP, rd, rs)))

#define jit_ldr_ui(d, rs)               MOVLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_ui(d, s1, s2)          MOVLmr(0,    (s1), (s2), 1,  (d))

#define jit_ldr_i(d, rs)                MOVSLQmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_i(d, s1, s2)           MOVSLQmr(0,    (s1), (s2), 1,  (d))

#define jit_ldr_l(d, rs)                MOVQmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_l(d, s1, s2)           MOVQmr(0,    (s1), (s2), 1,  (d))

#define jit_str_l(rd, rs)               MOVQrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_l(d1, d2, rs)          MOVQrm((rs), 0,    (d1), (d2), 1)

#define jit_blti_l(label, rs, is)	jit_bra_l0((rs), (is), JLm(label), JSm(label) )
#define jit_blei_l(label, rs, is)	jit_bra_l ((rs), (is), JLEm(label)		    )
#define jit_bgti_l(label, rs, is)	jit_bra_l ((rs), (is), JGm(label)		    )
#define jit_bgei_l(label, rs, is)	jit_bra_l0((rs), (is), JGEm(label), JNSm(label) )
#define jit_beqi_l(label, rs, is)	jit_bra_l0((rs), (is), JEm(label), JEm(label) )
#define jit_bnei_l(label, rs, is)	jit_bra_l0((rs), (is), JNEm(label), JNEm(label) )
#define jit_blti_ul(label, rs, is)	jit_bra_l ((rs), (is), JBm(label)		    )
#define jit_blei_ul(label, rs, is)	jit_bra_l0((rs), (is), JBEm(label), JEm(label) )
#define jit_bgti_ul(label, rs, is)	jit_bra_l0((rs), (is), JAm(label), JNEm(label) )
#define jit_bgei_ul(label, rs, is)	jit_bra_l ((rs), (is), JAEm(label)		    )
#define jit_bmsi_l(label, rs, is)	(jit_reduceQ(TEST, (is), (rs)), JNZm(label), _jit.x.pc)
#define jit_bmci_l(label, rs, is)	(jit_reduceQ(TEST, (is), (rs)), JZm(label),  _jit.x.pc)

#define jit_pusharg_l(rs) jit_pusharg_i(rs)
#define jit_bltr_l(label, s1, s2)	jit_bra_qr((s1), (s2), JLm(label) )
#define jit_bler_l(label, s1, s2)	jit_bra_qr((s1), (s2), JLEm(label) )
#define jit_bgtr_l(label, s1, s2)	jit_bra_qr((s1), (s2), JGm(label) )
#define jit_bger_l(label, s1, s2)	jit_bra_qr((s1), (s2), JGEm(label) )
#define jit_beqr_l(label, s1, s2)	jit_bra_qr((s1), (s2), JEm(label) )
#define jit_bner_l(label, s1, s2)	jit_bra_qr((s1), (s2), JNEm(label) )
#define jit_bltr_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JBm(label) )
#define jit_bler_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JBEm(label) )
#define jit_bgtr_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JAm(label) )
#define jit_bger_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JAEm(label) )

/* Bool operations.  */
#define jit_bool_qr(d, s1, s2, op)                                      \
        (jit_replace8(d, CMPQrr(s2, s1), op))

#define jit_bool_qi(d, rs, is, op)                                      \
        (jit_replace8(d, CMPQir(is, rs), op))

/* When CMP with 0 can be replaced with TEST */
#define jit_bool_qi0(d, rs, is, op, op0)                                \
        ((is) != 0                                                      \
          ? (jit_replace8(d, CMPQir(is, rs), op))                       \
          : (jit_replace8(d, TESTQrr(rs, rs), op0)))

#define jit_ltr_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETLr  )
#define jit_ler_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETLEr )
#define jit_gtr_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETGr  )
#define jit_ger_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETGEr )
#define jit_eqr_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETEr  )
#define jit_ner_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETNEr )
#define jit_ltr_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETBr  )
#define jit_ler_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETBEr )
#define jit_gtr_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETAr  )
#define jit_ger_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETAEr )

#define jit_lti_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETLr,  SETSr  )
#define jit_lei_l(d, rs, is)    jit_bool_qi ((d), (rs), (is), SETLEr         )
#define jit_gti_l(d, rs, is)    jit_bool_qi ((d), (rs), (is), SETGr          )
#define jit_gei_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETGEr, SETNSr )
#define jit_eqi_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETEr,  SETEr  )
#define jit_nei_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETNEr, SETNEr )
#define jit_lti_ul(d, rs, is)   jit_bool_qi ((d), (rs), (is), SETBr          )
#define jit_lei_ul(d, rs, is)   jit_bool_qi0((d), (rs), (is), SETBEr, SETEr  )
#define jit_gti_ul(d, rs, is)   jit_bool_qi0((d), (rs), (is), SETAr,  SETNEr )
#define jit_gei_ul(d, rs, is)   jit_bool_qi0((d), (rs), (is), SETAEr, INCLr  )

#endif /* __lightning_core_h */
