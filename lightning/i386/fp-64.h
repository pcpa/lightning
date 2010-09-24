/******************************** -*- C -*- ****************************
 *
 *	Support macros for SSE floating-point math
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2006,2010 Free Software Foundation, Inc.
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
 *	Paolo Bonzini
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/


#ifndef __lightning_fp_h
#define __lightning_fp_h

#define JIT_FPRET			_XMM0

#define JIT_FPR_NUM			6
static const jit_fpr_t
jit_sse_order[JIT_FPR_NUM] = {
    _XMM8, _XMM9, _XMM10, _XMM11, _XMM12, _XMM13
};
#define JIT_FPR(i)			jit_sse_order[i]

#define jit_extr_l_f(f0, r0)		x86_extr_l_f(_jit, f0, r0)
__jit_inline void
x86_extr_l_f(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    sse_extr_l_f(_jit, f0, r0);
}

#define jit_extr_l_d(f0, r0)		x86_extr_l_d(_jit, f0, r0)
__jit_inline void
x86_extr_l_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    sse_extr_l_d(_jit, f0, r0);
}

#define jit_rintr_f_l(r0, f0)		x86_rintr_f_l(_jit, r0, f0)
__jit_inline void
x86_rintr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_rintr_f_l(_jit, r0, f0);
}

#define jit_rintr_d_l(r0, f0)		x86_rintr_d_l(_jit, r0, f0)
__jit_inline void
x86_rintr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_rintr_d_l(_jit, r0, f0);
}

#define jit_roundr_f_l(r0, f0)		x86_roundr_f_l(_jit, r0, f0)
__jit_inline void
x86_roundr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_roundr_f_l(_jit, r0, f0);
}

#define jit_roundr_d_l(r0, f0)		x86_roundr_d_l(_jit, r0, f0)
__jit_inline void
x86_roundr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_roundr_d_l(_jit, r0, f0);
}

#define jit_truncr_f_l(r0, f0)		x86_truncr_f_l(_jit, r0, f0)
__jit_inline void
x86_truncr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_truncr_f_l(_jit, r0, f0);
}

#define jit_truncr_d_l(r0, f0)		x86_truncr_d_l(_jit, r0, f0)
__jit_inline void
x86_truncr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_truncr_d_l(_jit, r0, f0);
}

#define jit_floorr_f_l(r0, f0)		x86_floorr_f_l(_jit, r0, f0)
__jit_inline void
x86_floorr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_floorr_f_l(_jit, r0, f0);
}

#define jit_floorr_d_l(r0, f0)		x86_floorr_d_l(_jit, r0, f0)
__jit_inline void
x86_floorr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_floorr_d_l(_jit, r0, f0);
}

#define jit_ceilr_f_l(r0, f0)		x86_ceilr_f_l(_jit, r0, f0)
__jit_inline void
x86_ceilr_f_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_ceilr_f_l(_jit, r0, f0);
}

#define jit_ceilr_d_l(r0, f0)		x86_ceilr_d_l(_jit, r0, f0)
__jit_inline void
x86_ceilr_d_l(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_ceilr_d_l(_jit, r0, f0);
}

#define jit_prolog_d(nf)		x86_prolog_f(_jit, nf)
#define jit_prolog_f(nf)		x86_prolog_f(_jit, nf)
__jit_inline void
x86_prolog_f(jit_state_t _jit, int nf)
{
    /* update counter of float arguments */
    if ((_jitl.fp_args += nf) > JIT_FP_ARG_MAX) {
	/* have float arguments on stack */
	if ((_jitl.st_args = _jitl.gp_args - JIT_ARG_MAX) < 0)
	    _jitl.st_args = 0;
	_jitl.st_args += _jitl.fp_args - JIT_FP_ARG_MAX;
    }
}

#define jit_prepare_d(nf)		x86_prepare_f(_jit, nf)
#define jit_prepare_f(nf)		x86_prepare_f(_jit, nf)
__jit_inline void
x86_prepare_f(jit_state_t _jit, int nf)
{
    if ((_jitl.nextarg_putfp += nf) > JIT_FP_ARG_MAX) {
	/* need floats on stack */
	if ((_jitl.argssize = _jitl.gp_args - JIT_ARG_MAX) < 0)
	    _jitl.argssize = 0;
	_jitl.argssize += _jitl.nextarg_putfp - JIT_FP_ARG_MAX;
	_jitl.fprssize = JIT_FP_ARG_MAX;
    }
    else
	/* update counter of float argument registers */
	_jitl.fprssize += nf;
}

#define jit_arg_d()			x86_arg_f(_jit)
#define jit_arg_f()			x86_arg_f(_jit)
__jit_inline int
x86_arg_f(jit_state_t _jit)
{
    int		ofs;

    if (_jitl.st_args & 1) {
	/* true if first argument to a function with odd number
	 * of arguments that also requires arguments on stack */
	PUSHQr(_RAX);
	++_jitl.st_args;

	/* must call jit_allocai after aknowledging all arguments */
	assert(_jitl.alloca_offset == 0);

	_jitl.alloca_slack = sizeof(long);
    }
    if (_jitl.nextarg_getfp < JIT_FP_ARG_MAX)
	ofs = _jitl.nextarg_getfp++;
    else {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(double);
    }
    return (ofs);
}

#define jit_getarg_f(f0, ofs)		x86_getarg_f(_jit, f0, ofs)
__jit_inline void
x86_getarg_f(jit_state_t _jit,
	     jit_fpr_t f0, int ofs)
{
    if (ofs < JIT_FP_ARG_MAX)
	jit_movr_f(f0, (jit_fpr_t)(_XMM0 + ofs));
    else
	jit_ldxi_f(f0, JIT_FP, ofs);
}

#define jit_getarg_d(f0, ofs)		x86_getarg_d(_jit, f0, ofs)
__jit_inline void
x86_getarg_d(jit_state_t _jit,
	     jit_fpr_t f0, int ofs)
{
    if (ofs < JIT_FP_ARG_MAX)
	jit_movr_d(f0, (jit_fpr_t)(_XMM0 + ofs));
    else
	jit_ldxi_d(f0, JIT_FP, ofs);
}

#define jit_pusharg_f(f0)		x86_pusharg_f(_jit, f0)
__jit_inline void
x86_pusharg_f(jit_state_t _jit, jit_fpr_t f0)
{
    if (_jitl.argssize & 1) {
	/* true if first argument to a function with odd number
	 * of arguments that also requires arguments on stack */
	if (--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX) {
	    /* adjust and pass argument in stack */
	    jit_subi_l(JIT_SP, JIT_SP, sizeof(double) << 1);
	    jit_str_f(JIT_SP, f0);
	}
	else {
	    /* adjust stack and pass argument in register */
	    PUSHQr(_RAX);
	    jit_movr_f((jit_fpr_t)(_XMM0 + _jitl.nextarg_putfp), f0);
	}
	++_jitl.argssize;
    }
    else {
	if (--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX) {
	    /* pass argument in stack */
	    jit_subi_l(JIT_SP, JIT_SP, sizeof(double));
	    jit_str_f(JIT_SP, f0);
	}
	else
	    /* pass argument in register */
	    jit_movr_f((jit_fpr_t)(_XMM0 + _jitl.nextarg_putfp), f0);
    }

    /* only true if jit_prolog argument is less than jit_pusharg calls */
    assert(_jitl.nextarg_putfp >= 0);
}

#define jit_pusharg_d(f0)		x86_pusharg_d(_jit, f0)
__jit_inline void
x86_pusharg_d(jit_state_t _jit, jit_fpr_t f0)
{
    if (_jitl.argssize & 1) {
	/* true if first argument to a function with odd number
	 * of arguments that also requires arguments on stack */
	if (--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX) {
	    /* adjust and pass argument in stack */
	    jit_subi_l(JIT_SP, JIT_SP, sizeof(double) << 1);
	    jit_str_d(JIT_SP, f0);
	}
	else {
	    /* adjust stack and pass argument in register */
	    PUSHQr(_RAX);
	    jit_movr_d((jit_fpr_t)(_XMM0 + _jitl.nextarg_putfp), f0);
	}
	++_jitl.argssize;
    }
    else {
	if (--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX) {
	    /* pass argument in stack */
	    jit_subi_l(JIT_SP, JIT_SP, sizeof(double));
	    jit_str_d(JIT_SP, f0);
	}
	else
	    /* pass argument in register */
	    jit_movr_d((jit_fpr_t)(_XMM0 + _jitl.nextarg_putfp), f0);
    }

    /* only true if jit_prolog argument is less than jit_pusharg calls */
    assert(_jitl.nextarg_putfp >= 0);
}

#endif /* __lightning_fp_h */
