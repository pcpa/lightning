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

#define JIT_AP				_RBP

#define JIT_R_NUM			3
#define JIT_R(i)			(_RAX + (i))
#define JIT_V_NUM			3
#define JIT_V(i)			((i) == 0 ? _RBX : _RSI + (i) - 1)

#define jit_movi_i(r0, i0)		jit_movi_i(r0, i0)
__jit_inline void
jit_movi_i(jit_gpr_t r0, int i0)
{
    if (i0)
	MOVLir(i0, r0);
    else
	XORLrr(r0, r0);
}

#define jit_movi_p(r0, i0)		jit_movi_p(r0, i0)
__jit_inline jit_insn *
jit_movi_p(jit_gpr_t r0, void *i0)
{
    MOVLir((long)i0, r0);
    return (_jit->x.pc);
}

#define jit_patch_at(jump, label)	jit_patch_at(jump, label)
__jit_inline void
jit_patch_at(jit_insn *jump, jit_insn *label)
{
    jit_patch_rel_int_at(jump, label);
}

#define jit_prolog(n)			jit_prolog(n)
__jit_inline void
jit_prolog(int n)
{
    _jitl.framesize = 20;
    _jitl.alloca_offset = _jitl.alloca_slack = 0;
    PUSHLr(_RBX);
    PUSHLr(_RSI);
    PUSHLr(_RDI);
    PUSHLr(_RBP);
    MOVLrr(_RSP, _RBP);
    SUBLir(12, JIT_SP);
}

#define jit_ret				jit_ret
__jit_inline void
jit_ret(void)
{
    LEAVE_();
    POPLr(_RDI);
    POPLr(_RSI);
    POPLr(_RBX);
    RET_();
}

#define jit_allocai(n)			jit_allocai(n)
__jit_inline int
jit_allocai(int n)
{
    int		s = (_jitl.alloca_slack - n) & 15;
    if (n >= _jitl.alloca_slack) {
	_jitl.alloca_slack += n + s;
	if (n + s == sizeof(int))
	    PUSHLr(_RAX);
	else
	    SUBLir(n + s, _RSP);
    }
    _jitl.alloca_slack -= n;
    return (_jitl.alloca_offset -= n);
}

#define jit_prepare_i(ni)		jit_prepare_i(ni)
__jit_inline void
jit_prepare_i(int ni)
{
    _jitl.argssize = (ni + 3) & ~0x3;
    if (ni & 0x3)
	SUBLir(4 * (_jitl.argssize - ni), JIT_SP);
}

#define jit_calli(p0)			jit_calli(p0)
__jit_inline jit_insn *
jit_calli(void *p0)
{
    CALLm(p0);
    return (_jitl.label = _jit->x.pc);
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
    jit_patch_at(call, label);
}

#define jit_finish(p0)			jit_finish(p0)
__jit_inline jit_insn *
jit_finish(void *p0)
{
    jit_calli(p0);
    ADDLir(sizeof(long) * _jitl.argssize, JIT_SP);
    _jitl.argssize = 0;

    return (_jitl.label);
}

#define jit_finishr(r0)			jit_finishr(r0)
__jit_inline void
jit_finishr(jit_gpr_t r0)
{
    jit_callr(r0);
    ADDLir(sizeof(long) * _jitl.argssize, JIT_SP);
    _jitl.argssize = 0;
}

#define jit_pusharg_i(r0)		jit_pusharg_i(r0)
__jit_inline void
jit_pusharg_i(jit_gpr_t r0)
{
    PUSHLr(r0);
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

    ofs = _jitl.framesize;
    _jitl.framesize += sizeof(int);
    return (ofs);
}

/* Memory */
#define jit_ldr_c(r0, r1)		jit_ldr_c(r0, r1)
__jit_inline void
jit_ldr_c(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSBLmr(0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldxr_c(r0, r1, r2)		jit_ldxr_c(r0, r1, r2)
__jit_inline void
jit_ldxr_c(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSBLmr(0, r1, r2, _SCL1, r0);
}

#define jit_ldr_s(r0, r1)		jit_ldr_s(r0, r1)
__jit_inline void
jit_ldr_s(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSWLmr(0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldxr_s(r0, r1, r2)		jit_ldxr_s(r0, r1, r2)
__jit_inline void
jit_ldxr_s(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSWLmr(0, r1, r2, _SCL1, r0);
}

#define jit_ldi_c(r0, i0)		jit_ldi_c(r0, i0)
__jit_inline void
jit_ldi_c(jit_gpr_t r0, void *i0)
{
    MOVSBLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_c(r0, r1, i0)		jit_ldxi_c(r0, r1, i0)
__jit_inline void
jit_ldxi_c(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVSBLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldi_uc(r0, i0)		jit_ldi_uc(r0, i0)
__jit_inline void
jit_ldi_uc(jit_gpr_t r0, void *i0)
{
    MOVZBLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_uc(r0, r1, i0)		jit_ldxi_uc(r0, r1, i0)
__jit_inline void
jit_ldxi_uc(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVZBLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_sti_c(i0, r0)		jit_sti_c(i0, r0)
__jit_inline void
jit_sti_c(void *i0, jit_gpr_t r0)
{
    if (jit_reg8_p(r0))
	MOVBrm(r0, (long)i0, _NOREG, _NOREG, _SCL1);
    else {
	XCHGLrr(_RAX, r0);
	MOVBrm(_RAX, (long)i0, _NOREG, _NOREG, _SCL1);
	XCHGLrr(_RAX, r0);
    }
}

#define jit_stxi_c(i0, r0, r1)		jit_stxi_c(i0, r0, r1)
__jit_inline void
jit_stxi_c(long i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t	rep;

    if (jit_reg8_p(r1))
	MOVBrm(r1, i0, r0, _NOREG, _SCL1);
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
	MOVBrm(rep, i0, r0, _NOREG, _SCL1);
	if (r0 != r1)
	    XCHGLrr(rep, r1);
	else
	    jit_popr_i(rep);
    }
}

#define jit_str_c(r0, r1)		jit_str_c(r0, r1)
__jit_inline void
jit_str_c(jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t	rep;

    if (jit_reg8_p(r1))
	MOVBrm(r1, 0, r0, _NOREG, _SCL1);
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
	MOVBrm(rep, 0, r0, _NOREG, _SCL1);
	if (r0 != r1)
	    XCHGLrr(rep, r1);
	else
	    jit_popr_i(rep);
    }
}

#define jit_stxr_c(r0, r1, r2)		jit_stxr_c(r0, r1, r2)
__jit_inline void
jit_stxr_c(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_gpr_t	rep;

    if (jit_reg8_p(r2))
	MOVBrm(r2, 0, r0, r1, _SCL1);
    else {
	if (r0 == _RAX || r1 == _RAX) {
	    if (r0 == _RDX || r1 == _RDX)
		rep = _RCX;
	    else
		rep = _RDX;
	}
	else
	    rep = _RAX;
	if (r0 != r2 && r1 != r2)
	    XCHGLrr(rep, r2);
	else {
	    jit_pushr_i(rep);
	    MOVLrr(r2, rep);
	}
	MOVBrm(rep, 0, r0, r1, _SCL1);
	if (r0 != r2 && r1 != r2)
	    XCHGLrr(rep, r2);
	else
	    jit_popr_i(rep);
    }
}

#define jit_ldi_s(r0, i0)		jit_ldi_s(r0, i0)
__jit_inline void
jit_ldi_s(jit_gpr_t r0, void *i0)
{
    MOVSWLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_s(r0, r1, i0)		jit_ldxi_s(r0, r1, i0)
__jit_inline void
jit_ldxi_s(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVSWLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldi_us(r0, i0)		jit_ldi_us(r0, i0)
__jit_inline void
jit_ldi_us(jit_gpr_t r0, void *i0)
{
    MOVZWLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_us(r0, r1, i0)		jit_ldxi_us(r0, r1, i0)
__jit_inline void
jit_ldxi_us(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVZWLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_sti_s(i0, r0)		jit_sti_s(i0, r0)
__jit_inline void
jit_sti_s(void *i0, jit_gpr_t r0)
{
    MOVWrm(r0, (long)i0, _NOREG, _NOREG, _SCL1);
}

#define jit_stxi_s(i0, r0, r1)		jit_stxi_s(i0, r0, r1)
__jit_inline void
jit_stxi_s(long i0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVWrm(r1, i0, r0, _NOREG, _SCL1);
}

#define jit_ldi_i(r0, i0)		jit_ldi_i(r0, i0)
__jit_inline void
jit_ldi_i(jit_gpr_t r0, void *i0)
{
    MOVLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_i(r0, r1, i0)		jit_ldxi_i(r0, r1, i0)
__jit_inline void
jit_ldxi_i(jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldr_i(r0, r1)		jit_ldr_i(r0, r1)
__jit_inline void
jit_ldr_i(jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLmr(0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldxr_i(r0, r1, r2)		jit_ldxr_i(r0, r1, r2)
__jit_inline void
jit_ldxr_i(jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVLmr(0, r1, r2, _SCL1, r0);
}

#define jit_sti_i(i0, r0)		jit_sti_i(i0, r0)
__jit_inline void
jit_sti_i(void *i0, jit_gpr_t r0)
{
    MOVLrm(r0, (long)i0, _NOREG, _NOREG, _SCL1);
}

#define jit_stxi_i(i0, r0, r1)		jit_stxi_i(i0, r0, r1)
__jit_inline void
jit_stxi_i(long i0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLrm(r1, i0, r0, _NOREG, _SCL1);
}

#endif /* __lightning_core_h */
