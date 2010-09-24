/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (i386 version)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000,2001,2002,2003,2006 Free Software Foundation, Inc.
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
 * Authors:
 *	Matthew Flatt
 *	Paolo Bonzini
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/



#ifndef __lightning_core_h
#define __lightning_core_h

#define JIT_AP				_RBP
#define JIT_R_NUM			3
static const jit_gpr_t
jit_r_order[JIT_R_NUM] = {
    _RAX, _RCX, _RDX
};
#define JIT_R(i)			jit_r_order[i]

#define JIT_V_NUM			3
static const jit_gpr_t
jit_v_order[JIT_V_NUM] = {
    _RBX, _RSI, _RDI
};
#define JIT_V(i)			jit_v_order[i]

#define jit_movi_i(r0, i0)		x86_movi_i(_jit, r0, i0)
__jit_inline void
x86_movi_i(jit_state_t _jit,
	   jit_gpr_t r0, int i0)
{
    if (i0)
	MOVLir(i0, r0);
    else
	XORLrr(r0, r0);
}

#define jit_movi_p(r0, i0)		x86_movi_p(_jit, r0, i0)
__jit_inline jit_insn *
x86_movi_p(jit_state_t _jit,
	   jit_gpr_t r0, void *i0)
{
    MOVLir((long)i0, r0);
    return (_jit->x.pc);
}

#define jit_patch_at(jump, label)	x86_patch_at(_jit, jump, label)
__jit_inline void
x86_patch_at(jit_state_t _jit,
	     jit_insn *jump, jit_insn *label)
{
    jit_patch_rel_int_at(jump, label);
}

#define jit_prolog(n)			x86_prolog(_jit, n)
__jit_inline void
x86_prolog(jit_state_t _jit,
	   int n)
{
    _jitl.framesize = 20;
    _jitl.alloca_offset = 0;
    _jitl.alloca_slack = 12;
    PUSHLr(_RBX);
    PUSHLr(_RSI);
    PUSHLr(_RDI);
    PUSHLr(_RBP);
    MOVLrr(_RSP, _RBP);
    SUBLir(12, JIT_SP);
}

#define jit_ret()			x86_ret(_jit)
__jit_inline void
x86_ret(jit_state_t _jit)
{
    LEAVE_();
    POPLr(_RDI);
    POPLr(_RSI);
    POPLr(_RBX);
    RET_();
}

#define jit_allocai(n)			x86_allocai(_jit, n)
__jit_inline int
x86_allocai(jit_state_t _jit,
	    int n)
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

#define jit_prepare_i(ni)		x86_prepare_i(_jit, ni)
__jit_inline void
x86_prepare_i(jit_state_t _jit,
	      int ni)
{
    _jitl.argssize = ni;
}

#define jit_calli(p0)			x86_calli(_jit, p0)
__jit_inline jit_insn *
x86_calli(jit_state_t _jit,
	  void *p0)
{
    CALLm(p0);
    return (_jitl.label = _jit->x.pc);
}

#define jit_callr(r0)			x86_callr(_jit, r0)
__jit_inline void
x86_callr(jit_state_t _jit,
	  jit_gpr_t r0)
{
    CALLsr(r0);
}

#define jit_patch_calli(call, label)	x86_patch_calli(_jit, call, label)
__jit_inline void
x86_patch_calli(jit_state_t _jit,
		jit_insn *call, jit_insn *label)
{
    jit_patch_at(call, label);
}

#define jit_finish(p0)			x86_finish(_jit, p0)
__jit_inline jit_insn *
x86_finish(jit_state_t _jit,
	   void *p0)
{
    jit_calli(p0);
    jit_addi_i(JIT_SP, JIT_SP, _jitl.argssize << 2);
    _jitl.argssize = 0;

    return (_jitl.label);
}

#define jit_finishr(r0)			x86_finishr(_jit, r0)
__jit_inline void
x86_finishr(jit_state_t _jit,
	    jit_gpr_t r0)
{
    jit_callr(r0);
    jit_addi_i(JIT_SP, JIT_SP, _jitl.argssize << 2);
    _jitl.argssize = 0;
}

#define jit_pusharg_i(r0)		x86_pusharg_i(_jit, r0)
__jit_inline void
x86_pusharg_i(jit_state_t _jit, jit_gpr_t r0)
{
    if (_jitl.argssize & ~3) {
	/* only true if first argument to a function with
	 * stack arguments not aligned at 16 bytes */
	int	argssize = (_jitl.argssize + 3) & ~3;
	jit_subi_i(JIT_SP, JIT_SP,
		   ((argssize - _jitl.argssize) << 2) + sizeof(int));
	_jitl.argssize = argssize;
	jit_str_i(JIT_SP, r0);
    }
    else
	PUSHLr(r0);
}

#define jit_arg_i()			x86_arg_i(_jit)
#define jit_arg_c()			x86_arg_i(_jit)
#define jit_arg_uc()			x86_arg_i(_jit)
#define jit_arg_s()			x86_arg_i(_jit)
#define jit_arg_us()			x86_arg_i(_jit)
#define jit_arg_ui()			x86_arg_i(_jit)
#define jit_arg_l()			x86_arg_i(_jit)
#define jit_arg_ul()			x86_arg_i(_jit)
#define jit_arg_p()			x86_arg_i(_jit)
__jit_inline int
x86_arg_i(jit_state_t _jit)
{
    int		ofs;

    ofs = _jitl.framesize;
    _jitl.framesize += sizeof(int);
    return (ofs);
}

/* Memory */
#define jit_ldr_c(r0, r1)		x86_ldr_c(_jit, r0, r1)
__jit_inline void
x86_ldr_c(jit_state_t _jit,
	  jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSBLmr(0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldxr_c(r0, r1, r2)		x86_ldxr_c(_jit, r0, r1, r2)
__jit_inline void
x86_ldxr_c(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSBLmr(0, r1, r2, _SCL1, r0);
}

#define jit_ldr_s(r0, r1)		x86_ldr_s(_jit, r0, r1)
__jit_inline void
x86_ldr_s(jit_state_t _jit,
	  jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSWLmr(0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldxr_s(r0, r1, r2)		x86_ldxr_s(_jit, r0, r1, r2)
__jit_inline void
x86_ldxr_s(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVSWLmr(0, r1, r2, _SCL1, r0);
}

#define jit_ldi_c(r0, i0)		x86_ldi_c(_jit, r0, i0)
__jit_inline void
x86_ldi_c(jit_state_t _jit,
	  jit_gpr_t r0, void *i0)
{
    MOVSBLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_c(r0, r1, i0)		x86_ldxi_c(_jit, r0, r1, i0)
__jit_inline void
x86_ldxi_c(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVSBLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldi_uc(r0, i0)		x86_ldi_uc(_jit, r0, i0)
__jit_inline void
x86_ldi_uc(jit_state_t _jit,
	   jit_gpr_t r0, void *i0)
{
    MOVZBLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_uc(r0, r1, i0)		x86_ldxi_uc(_jit, r0, r1, i0)
__jit_inline void
x86_ldxi_uc(jit_state_t _jit,
	    jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVZBLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_sti_c(i0, r0)		x86_sti_c(_jit, i0, r0)
__jit_inline void
x86_sti_c(jit_state_t _jit,
	  void *i0, jit_gpr_t r0)
{
    if (jit_reg8_p(r0))
	MOVBrm(r0, (long)i0, _NOREG, _NOREG, _SCL1);
    else {
	XCHGLrr(_RAX, r0);
	MOVBrm(_RAX, (long)i0, _NOREG, _NOREG, _SCL1);
	XCHGLrr(_RAX, r0);
    }
}

#define jit_stxi_c(i0, r0, r1)		x86_stxi_c(_jit, i0, r0, r1)
__jit_inline void
x86_stxi_c(jit_state_t _jit,
	   long i0, jit_gpr_t r0, jit_gpr_t r1)
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

#define jit_str_c(r0, r1)		x86_str_c(_jit, r0, r1)
__jit_inline void
x86_str_c(jit_state_t _jit,
	  jit_gpr_t r0, jit_gpr_t r1)
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

#define jit_stxr_c(r0, r1, r2)		x86_stxr_c(_jit, r0, r1, r2)
__jit_inline void
x86_stxr_c(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
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

#define jit_ldi_s(r0, i0)		x86_ldi_s(_jit, r0, i0)
__jit_inline void
x86_ldi_s(jit_state_t _jit,
	  jit_gpr_t r0, void *i0)
{
    MOVSWLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_s(r0, r1, i0)		x86_ldxi_s(_jit, r0, r1, i0)
__jit_inline void
x86_ldxi_s(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVSWLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldi_us(r0, i0)		x86_ldi_us(_jit, r0, i0)
__jit_inline void
x86_ldi_us(jit_state_t _jit,
	   jit_gpr_t r0, void *i0)
{
    MOVZWLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_us(r0, r1, i0)		x86_ldxi_us(_jit, r0, r1, i0)
__jit_inline void
x86_ldxi_us(jit_state_t _jit,
	    jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVZWLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_sti_s(i0, r0)		x86_sti_s(_jit, i0, r0)
__jit_inline void
x86_sti_s(jit_state_t _jit,
	  void *i0, jit_gpr_t r0)
{
    MOVWrm(r0, (long)i0, _NOREG, _NOREG, _SCL1);
}

#define jit_stxi_s(i0, r0, r1)		x86_stxi_s(_jit, i0, r0, r1)
__jit_inline void
x86_stxi_s(jit_state_t _jit,
	   long i0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVWrm(r1, i0, r0, _NOREG, _SCL1);
}

#define jit_ldi_i(r0, i0)		x86_ldi_i(_jit, r0, i0)
__jit_inline void
x86_ldi_i(jit_state_t _jit,
	  jit_gpr_t r0, void *i0)
{
    MOVLmr((long)i0, _NOREG, _NOREG, _SCL1, r0);
}

#define jit_ldxi_i(r0, r1, i0)		x86_ldxi_i(_jit, r0, r1, i0)
__jit_inline void
x86_ldxi_i(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    MOVLmr(i0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldr_i(r0, r1)		x86_ldr_i(_jit, r0, r1)
__jit_inline void
x86_ldr_i(jit_state_t _jit,
	  jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLmr(0, r1, _NOREG, _SCL1, r0);
}

#define jit_ldxr_i(r0, r1, r2)		x86_ldxr_i(_jit, r0, r1, r2)
__jit_inline void
x86_ldxr_i(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    MOVLmr(0, r1, r2, _SCL1, r0);
}

#define jit_sti_i(i0, r0)		x86_sti_i(_jit, i0, r0)
__jit_inline void
x86_sti_i(jit_state_t _jit,
	  void *i0, jit_gpr_t r0)
{
    MOVLrm(r0, (long)i0, _NOREG, _NOREG, _SCL1);
}

#define jit_stxi_i(i0, r0, r1)		x86_stxi_i(_jit, i0, r0, r1)
__jit_inline void
x86_stxi_i(jit_state_t _jit,
	   long i0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVLrm(r1, i0, r0, _NOREG, _SCL1);
}

#endif /* __lightning_core_h */
