/******************************** -*- C -*- ****************************
 *
 *	Support macros for SSE floating-point math
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2006 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
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
 ***********************************************************************/


#ifndef __lightning_fp_h
#define __lightning_fp_h

#define JIT_FPR_NUM			6
#define JIT_FPRET			_XMM0
#define JIT_FPR(i)			((jit_fpr_t)(_XMM8 + (i)))
#define JIT_FPTMP0			_XMM14
#define JIT_FPTMP1			_XMM15

#include "fp-sse.h"

#define jit_addr_f(f0, f1, f2)		x86_addr_f(_jit, f0, f1, f2)
__jit_inline void
x86_addr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_addr_f(_jit, f0, f1, f2);
}

#define jit_subr_f(f0, f1, f2)		x86_subr_f(_jit, f0, f1, f2)
__jit_inline void
x86_subr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_subr_f(_jit, f0, f1, f2);
}

#define jit_mulr_f(f0, f1, f2)		x86_mulr_f(_jit, f0, f1, f2)
__jit_inline void
x86_mulr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_mulr_f(_jit, f0, f1, f2);
}

#define jit_divr_f(f0, f1, f2)		x86_divr_f(_jit, f0, f1, f2)
__jit_inline void
x86_divr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_divr_f(_jit, f0, f1, f2);
}

#define jit_addr_d(f0, f1, f2)		x86_addr_d(_jit, f0, f1, f2)
__jit_inline void
x86_addr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_addr_d(_jit, f0, f1, f2);
}

#define jit_subr_d(f0, f1, f2)		x86_subr_d(_jit, f0, f1, f2)
__jit_inline void
x86_subr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_subr_d(_jit, f0, f1, f2);
}

#define jit_mulr_d(f0, f1, f2)		x86_mulr_d(_jit, f0, f1, f2)
__jit_inline void
x86_mulr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_mulr_d(_jit, f0, f1, f2);
}

#define jit_divr_d(f0, f1, f2)		x86_divr_d(_jit, f0, f1, f2)
__jit_inline void
x86_divr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    sse_divr_d(_jit, f0, f1, f2);
}

#define jit_ldi_f(f0, i0)		x86_ldi_f(_jit, f0, i0)
__jit_inline void
x86_ldi_f(jit_state_t _jit,
	  jit_fpr_t f0, void *i0)
{
    sse_ldi_f(_jit, f0, i0);
}

#define jit_ldi_d(f0, i0)		x86_ldi_d(_jit, f0, i0)
__jit_inline void
x86_ldi_d(jit_state_t _jit,
	  jit_fpr_t f0, void *i0)
{
    sse_ldi_d(_jit, f0, i0);
}

#define jit_ldr_f(f0, r0)		x86_ldr_f(_jit, f0, r0)
__jit_inline void
x86_ldr_f(jit_state_t _jit,
	  jit_fpr_t f0, jit_gpr_t r0)
{
    sse_ldr_f(_jit, f0, r0);
}

#define jit_ldr_d(f0, r0)		x86_ldr_d(_jit, f0, r0)
__jit_inline void
x86_ldr_d(jit_state_t _jit,
	  jit_fpr_t f0, jit_gpr_t r0)
{
    sse_ldr_d(_jit, f0, r0);
}

#define jit_ldxi_f(f0, r0, i0)		x86_ldxi_f(_jit, f0, r0, i0)
__jit_inline void
x86_ldxi_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_gpr_t r0, long i0)
{
    sse_ldxi_f(_jit, f0, r0, i0);
}

#define jit_ldxi_d(f0, r0, i0)		x86_ldxi_d(_jit, f0, r0, i0)
__jit_inline void
x86_ldxi_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_gpr_t r0, long i0)
{
    sse_ldxi_d(_jit, f0, r0, i0);
}

#define jit_ldxr_f(f0, r0, r1)		x86_ldxr_f(_jit, f0, r0, r1)
__jit_inline void
x86_ldxr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    sse_ldxr_f(_jit, f0, r0, r1);
}

#define jit_ldxr_d(f0, r0, r1)		x86_ldxr_d(_jit, f0, r0, r1)
__jit_inline void
x86_ldxr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    sse_ldxr_d(_jit, f0, r0, r1);
}

#define jit_sti_f(i0, f0)		x86_sti_f(_jit, i0, f0)
__jit_inline void
x86_sti_f(jit_state_t _jit,
	  void *i0, jit_fpr_t f0)
{
    sse_sti_f(_jit, i0, f0);
}

#define jit_sti_d(i0, f0)		x86_sti_d(_jit, i0, f0)
__jit_inline void
x86_sti_d(jit_state_t _jit,
	  void *i0, jit_fpr_t f0)
{
    sse_sti_d(_jit, i0, f0);
}

#define jit_str_f(r0, f0)		x86_str_f(_jit, r0, f0)
__jit_inline void
x86_str_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0)
{
    sse_str_f(_jit, r0, f0);
}

#define jit_str_d(r0, f0)		x86_str_d(_jit, r0, f0)
__jit_inline void
x86_str_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0)
{
    sse_str_d(_jit, r0, f0);
}

#define jit_stxi_f(i0, r0, f0)		x86_stxi_f(_jit, i0, r0, f0)
__jit_inline void
x86_stxi_f(jit_state_t _jit,
	   long i0, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_stxi_f(_jit, i0, r0, f0);
}

#define jit_stxi_d(i0, r0, f0)		x86_stxi_d(_jit, i0, r0, f0)
__jit_inline void
x86_stxi_d(jit_state_t _jit,
	   long i0, jit_gpr_t r0, jit_fpr_t f0)
{
    sse_stxi_d(_jit, i0, r0, f0);
}

#define jit_stxr_f(r0, r1, f0)		x86_stxr_f(_jit, r0, r1, f0)
__jit_inline void
x86_stxr_f(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    sse_stxr_f(_jit, r0, r1, f0);
}

#define jit_stxr_d(r0, r1, f0)		x86_stxr_d(_jit, r0, r1, f0)
__jit_inline void
x86_stxr_d(jit_state_t _jit,
	   jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    sse_stxr_d(_jit, r0, r1, f0);
}

#define jit_movr_f(f0, f1)		x86_movr_f(_jit, f0, f1)
__jit_inline void
x86_movr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1)
{
    sse_movr_f(_jit, f0, f1);
}

#define jit_movr_d(f0, f1)		x86_movr_d(_jit, f0, f1)
__jit_inline void
x86_movr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1)
{
    sse_movr_d(_jit, f0, f1);
}

#define jit_movi_f(f0, i0)		x86_movi_f(_jit, f0, i0)
__jit_inline void
x86_movi_f(jit_state_t _jit,
	   jit_fpr_t f0, float i0)
{
    sse_movi_f(_jit, f0, i0);
}

#define jit_movi_d(f0, i0)		x86_movi_d(_jit, f0, i0)
__jit_inline void
x86_movi_d(jit_state_t _jit,
	   jit_fpr_t f0, double i0)
{
    sse_movi_d(_jit, f0, i0);
}

#define jit_extr_i_f(f0, r0)		x86_extr_i_f(_jit, f0, r0)
__jit_inline void
x86_extr_i_f(jit_state_t _jit,
	     jit_fpr_t f0, jit_gpr_t r0)
{
    sse_extr_i_f(_jit, f0, r0);
}

#define jit_extr_i_d(f0, r0)		x86_extr_i_d(_jit, f0, r0)
__jit_inline void
x86_extr_i_d(jit_state_t _jit,
	     jit_fpr_t f0, jit_gpr_t r0)
{
    sse_extr_i_d(_jit, f0, r0);
}

#define jit_extr_l_f(f0, r0)		x86_extr_l_f(_jit, f0, r0)
__jit_inline void
x86_extr_l_f(jit_state_t _jit,
	     jit_fpr_t f0, jit_gpr_t r0)
{
    sse_extr_l_f(_jit, f0, r0);
}

#define jit_extr_l_d(f0, r0)		x86_extr_l_d(_jit, f0, r0)
__jit_inline void
x86_extr_l_d(jit_state_t _jit,
	     jit_fpr_t f0, jit_gpr_t r0)
{
    sse_extr_l_d(_jit, f0, r0);
}

#define jit_extr_f_d(f0, f1)		x86_extr_f_d(_jit, f0, f1)
__jit_inline void
x86_extr_f_d(jit_state_t _jit,
	     jit_fpr_t f0, jit_fpr_t f1)
{
    sse_extr_f_d(_jit, f0, f1);
}

#define jit_extr_d_f(f0, f1)		x86_extr_d_f(_jit, f0, f1)
__jit_inline void
x86_extr_d_f(jit_state_t _jit,
	     jit_fpr_t f0, jit_fpr_t f1)
{
    sse_extr_d_f(_jit, f0, f1);
}

#define jit_absr_f(f0, f1)		x86_absr_f(_jit, f0, f1)
__jit_inline void
x86_absr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1)
{
    sse_absr_f(_jit, f0, f1);
}

#define jit_absr_d(f0, f1)		x86_absr_d(_jit, f0, f1)
__jit_inline void
x86_absr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1)
{
    sse_absr_d(_jit, f0, f1);
}

#define jit_sqrtr_f(f0, f1)		x86_sqrtr_f(_jit, f0, f1)
__jit_inline void
x86_sqrtr_f(jit_state_t _jit,
	    jit_fpr_t f0, jit_fpr_t f1)
{
    sse_sqrtr_f(_jit, f0, f1);
}

#define jit_sqrtr_d(f0, f1)		x86_sqrtr_d(_jit, f0, f1)
__jit_inline void
x86_sqrtr_d(jit_state_t _jit,
	    jit_fpr_t f0, jit_fpr_t f1)
{
    sse_sqrtr_d(_jit, f0, f1);
}

#define jit_negr_f(f0, f1)		x86_negr_f(_jit, f0, f1)
__jit_inline void
x86_negr_f(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1)
{
    sse_negr_f(_jit, f0, f1);
}

#define jit_negr_d(f0, f1)		x86_negr_d(_jit, f0, f1)
__jit_inline void
x86_negr_d(jit_state_t _jit,
	   jit_fpr_t f0, jit_fpr_t f1)
{
    sse_negr_d(_jit, f0, f1);
}

#define jit_rintr_f_i(r0, f0)		x86_rintr_f_i(_jit, r0, f0)
__jit_inline void
x86_rintr_f_i(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_rintr_f_i(_jit, r0, f0);
}

#define jit_rintr_f_l(r0, f0)		x86_rintr_f_l(_jit, r0, f0)
__jit_inline void
x86_rintr_f_l(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_rintr_f_l(_jit, r0, f0);
}

#define jit_rintr_d_i(r0, f0)		x86_rintr_d_i(_jit, r0, f0)
__jit_inline void
x86_rintr_d_i(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_rintr_d_i(_jit, r0, f0);
}

#define jit_rintr_d_l(r0, f0)		x86_rintr_d_l(_jit, r0, f0)
__jit_inline void
x86_rintr_d_l(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_rintr_d_l(_jit, r0, f0);
}

#define jit_roundr_f_i(r0, f0)		x86_roundr_f_i(_jit, r0, f0)
__jit_inline void
x86_roundr_f_i(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_roundr_f_i(_jit, r0, f0);
}

#define jit_roundr_f_l(r0, f0)		x86_roundr_f_l(_jit, r0, f0)
__jit_inline void
x86_roundr_f_l(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_roundr_f_l(_jit, r0, f0);
}

#define jit_roundr_d_i(r0, f0)		x86_roundr_d_i(_jit, r0, f0)
__jit_inline void
x86_roundr_d_i(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_roundr_d_i(_jit, r0, f0);
}

#define jit_roundr_d_l(r0, f0)		x86_roundr_d_l(_jit, r0, f0)
__jit_inline void
x86_roundr_d_l(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_roundr_d_l(_jit, r0, f0);
}

#define jit_truncr_f_i(r0, f0)		x86_truncr_f_i(_jit, r0, f0)
__jit_inline void
x86_truncr_f_i(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_truncr_f_i(_jit, r0, f0);
}

#define jit_truncr_f_l(r0, f0)		x86_truncr_f_l(_jit, r0, f0)
__jit_inline void
x86_truncr_f_l(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_truncr_f_l(_jit, r0, f0);
}

#define jit_truncr_d_i(r0, f0)		x86_truncr_d_i(_jit, r0, f0)
__jit_inline void
x86_truncr_d_i(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_truncr_d_i(_jit, r0, f0);
}

#define jit_truncr_d_l(r0, f0)		x86_truncr_d_l(_jit, r0, f0)
__jit_inline void
x86_truncr_d_l(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_truncr_d_l(_jit, r0, f0);
}

#define jit_floorr_f_i(r0, f0)		x86_floorr_f_i(_jit, r0, f0)
__jit_inline void
x86_floorr_f_i(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_floorr_f_i(_jit, r0, f0);
}

#define jit_floorr_f_l(r0, f0)		x86_floorr_f_l(_jit, r0, f0)
__jit_inline void
x86_floorr_f_l(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_floorr_f_l(_jit, r0, f0);
}

#define jit_floorr_d_i(r0, f0)		x86_floorr_d_i(_jit, r0, f0)
__jit_inline void
x86_floorr_d_i(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_floorr_d_i(_jit, r0, f0);
}

#define jit_floorr_d_l(r0, f0)		x86_floorr_d_l(_jit, r0, f0)
__jit_inline void
x86_floorr_d_l(jit_state_t _jit,
	       jit_gpr_t r0, jit_fpr_t f0)
{
    sse_floorr_d_l(_jit, r0, f0);
}

#define jit_ceilr_f_i(r0, f0)		x86_ceilr_f_i(_jit, r0, f0)
__jit_inline void
x86_ceilr_f_i(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_ceilr_f_i(_jit, r0, f0);
}

#define jit_ceilr_f_l(r0, f0)		x86_ceilr_f_l(_jit, r0, f0)
__jit_inline void
x86_ceilr_f_l(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_ceilr_f_l(_jit, r0, f0);
}

#define jit_ceilr_d_i(r0, f0)		x86_ceilr_d_i(_jit, r0, f0)
__jit_inline void
x86_ceilr_d_i(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_ceilr_d_i(_jit, r0, f0);
}

#define jit_ceilr_d_l(r0, f0)		x86_ceilr_d_l(_jit, r0, f0)
__jit_inline void
x86_ceilr_d_l(jit_state_t _jit,
	      jit_gpr_t r0, jit_fpr_t f0)
{
    sse_ceilr_d_l(_jit, r0, f0);
}

#define jit_ltr_f(r0, f0, f1)		x86_ltr_f(_jit, r0, f0, f1)
__jit_inline void
x86_ltr_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ltr_f(_jit, r0, f0, f1);
}

#define jit_ler_f(r0, f0, f1)		x86_ler_f(_jit, r0, f0, f1)
__jit_inline void
x86_ler_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ler_f(_jit, r0, f0, f1);
}

#define jit_eqr_f(r0, f0, f1)		x86_eqr_f(_jit, r0, f0, f1)
__jit_inline void
x86_eqr_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_eqr_f(_jit, r0, f0, f1);
}

#define jit_ger_f(r0, f0, f1)		x86_ger_f(_jit, r0, f0, f1)
__jit_inline void
x86_ger_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ger_f(_jit, r0, f0, f1);
}

#define jit_gtr_f(r0, f0, f1)		x86_gtr_f(_jit, r0, f0, f1)
__jit_inline void
x86_gtr_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_gtr_f(_jit, r0, f0, f1);
}

#define jit_ner_f(r0, f0, f1)		x86_ner_f(_jit, r0, f0, f1)
__jit_inline void
x86_ner_f(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ner_f(_jit, r0, f0, f1);
}

#define jit_unltr_f(r0, f0, f1)		x86_unltr_f(_jit, r0, f0, f1)
__jit_inline void
x86_unltr_f(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unltr_f(_jit, r0, f0, f1);
}

#define jit_unler_f(r0, f0, f1)		x86_unler_f(_jit, r0, f0, f1)
__jit_inline void
x86_unler_f(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unler_f(_jit, r0, f0, f1);
}

#define jit_uneqr_f(r0, f0, f1)		x86_uneqr_f(_jit, r0, f0, f1)
__jit_inline void
x86_uneqr_f(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_uneqr_f(_jit, r0, f0, f1);
}

#define jit_unger_f(r0, f0, f1)		x86_unger_f(_jit, r0, f0, f1)
__jit_inline void
x86_unger_f(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unger_f(_jit, r0, f0, f1);
}

#define jit_ungtr_f(r0, f0, f1)		x86_ungtr_f(_jit, r0, f0, f1)
__jit_inline void
x86_ungtr_f(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ungtr_f(_jit, r0, f0, f1);
}

#define jit_ltgtr_f(r0, f0, f1)		x86_ltgtr_f(_jit, r0, f0, f1)
__jit_inline void
x86_ltgtr_f(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ltgtr_f(_jit, r0, f0, f1);
}

#define jit_ordr_f(r0, f0, f1)		x86_ordr_f(_jit, r0, f0, f1)
__jit_inline void
x86_ordr_f(jit_state_t _jit,
	   jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ordr_f(_jit, r0, f0, f1);
}

#define jit_unordr_f(r0, f0, f1)	x86_unordr_f(_jit, r0, f0, f1)
__jit_inline void
x86_unordr_f(jit_state_t _jit,
	     jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unordr_f(_jit, r0, f0, f1);
}

#define jit_ltr_d(r0, f0, f1)		x86_ltr_d(_jit, r0, f0, f1)
__jit_inline void
x86_ltr_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ltr_d(_jit, r0, f0, f1);
}

#define jit_ler_d(r0, f0, f1)		x86_ler_d(_jit, r0, f0, f1)
__jit_inline void
x86_ler_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ler_d(_jit, r0, f0, f1);
}

#define jit_eqr_d(r0, f0, f1)		x86_eqr_d(_jit, r0, f0, f1)
__jit_inline void
x86_eqr_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_eqr_d(_jit, r0, f0, f1);
}

#define jit_ger_d(r0, f0, f1)		x86_ger_d(_jit, r0, f0, f1)
__jit_inline void
x86_ger_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ger_d(_jit, r0, f0, f1);
}

#define jit_gtr_d(r0, f0, f1)		x86_gtr_d(_jit, r0, f0, f1)
__jit_inline void
x86_gtr_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_gtr_d(_jit, r0, f0, f1);
}

#define jit_ner_d(r0, f0, f1)		x86_ner_d(_jit, r0, f0, f1)
__jit_inline void
x86_ner_d(jit_state_t _jit,
	  jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ner_d(_jit, r0, f0, f1);
}

#define jit_unltr_d(r0, f0, f1)		x86_unltr_d(_jit, r0, f0, f1)
__jit_inline void
x86_unltr_d(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unltr_d(_jit, r0, f0, f1);
}

#define jit_unler_d(r0, f0, f1)		x86_unler_d(_jit, r0, f0, f1)
__jit_inline void
x86_unler_d(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unler_d(_jit, r0, f0, f1);
}

#define jit_uneqr_d(r0, f0, f1)		x86_uneqr_d(_jit, r0, f0, f1)
__jit_inline void
x86_uneqr_d(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_uneqr_d(_jit, r0, f0, f1);
}

#define jit_unger_d(r0, f0, f1)		x86_unger_d(_jit, r0, f0, f1)
__jit_inline void
x86_unger_d(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unger_d(_jit, r0, f0, f1);
}

#define jit_ungtr_d(r0, f0, f1)		x86_ungtr_d(_jit, r0, f0, f1)
__jit_inline void
x86_ungtr_d(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ungtr_d(_jit, r0, f0, f1);
}

#define jit_ltgtr_d(r0, f0, f1)		x86_ltgtr_d(_jit, r0, f0, f1)
__jit_inline void
x86_ltgtr_d(jit_state_t _jit,
	    jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ltgtr_d(_jit, r0, f0, f1);
}

#define jit_ordr_d(r0, f0, f1)		x86_ordr_d(_jit, r0, f0, f1)
__jit_inline void
x86_ordr_d(jit_state_t _jit,
	   jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_ordr_d(_jit, r0, f0, f1);
}

#define jit_unordr_d(r0, f0, f1)	x86_unordr_d(_jit, r0, f0, f1)
__jit_inline void
x86_unordr_d(jit_state_t _jit,
	     jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    sse_unordr_d(_jit, r0, f0, f1);
}

#define jit_bltr_f(label, f0, f1)	x86_bltr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bltr_f(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bltr_f(_jit, label, f0, f1));
}

#define jit_bler_f(label, f0, f1)	x86_bler_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bler_f(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bler_f(_jit, label, f0, f1));
}

#define jit_beqr_f(label, f0, f1)	x86_beqr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_beqr_f(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_beqr_f(_jit, label, f0, f1));
}

#define jit_bger_f(label, f0, f1)	x86_bger_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bger_f(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bger_f(_jit, label, f0, f1));
}

#define jit_bgtr_f(label, f0, f1)	x86_bgtr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bgtr_f(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bgtr_f(_jit, label, f0, f1));
}

#define jit_bner_f(label, f0, f1)	x86_bner_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bner_f(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bner_f(_jit, label, f0, f1));
}

#define jit_bunltr_f(label, f0, f1)	x86_bunltr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunltr_f(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunltr_f(_jit, label, f0, f1));
}

#define jit_bunler_f(label, f0, f1)	x86_bunler_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunler_f(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunler_f(_jit, label, f0, f1));
}

#define jit_buneqr_f(label, f0, f1)	x86_buneqr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_buneqr_f(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_buneqr_f(_jit, label, f0, f1));
}

#define jit_bunger_f(label, f0, f1)	x86_bunger_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunger_f(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunger_f(_jit, label, f0, f1));
}

#define jit_bungtr_f(label, f0, f1)	x86_bungtr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bungtr_f(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bungtr_f(_jit, label, f0, f1));
}

#define jit_bltgtr_f(label, f0, f1)	x86_bltgtr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bltgtr_f(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bltgtr_f(_jit, label, f0, f1));
}

#define jit_bordr_f(label, f0, f1)	x86_bordr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bordr_f(jit_state_t _jit,
	    jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bordr_f(_jit, label, f0, f1));
}

#define jit_bunordr_f(label, f0, f1)	x86_bunordr_f(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunordr_f(jit_state_t _jit,
	      jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunordr_f(_jit, label, f0, f1));
}

#define jit_bltr_d(label, f0, f1)	x86_bltr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bltr_d(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bltr_d(_jit, label, f0, f1));
}

#define jit_bler_d(label, f0, f1)	x86_bler_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bler_d(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bler_d(_jit, label, f0, f1));
}

#define jit_beqr_d(label, f0, f1)	x86_beqr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_beqr_d(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_beqr_d(_jit, label, f0, f1));
}

#define jit_bger_d(label, f0, f1)	x86_bger_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bger_d(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bger_d(_jit, label, f0, f1));
}

#define jit_bgtr_d(label, f0, f1)	x86_bgtr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bgtr_d(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bgtr_d(_jit, label, f0, f1));
}

#define jit_bner_d(label, f0, f1)	x86_bner_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bner_d(jit_state_t _jit,
	   jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bner_d(_jit, label, f0, f1));
}

#define jit_bunltr_d(label, f0, f1)	x86_bunltr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunltr_d(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunltr_d(_jit, label, f0, f1));
}

#define jit_bunler_d(label, f0, f1)	x86_bunler_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunler_d(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunler_d(_jit, label, f0, f1));
}

#define jit_buneqr_d(label, f0, f1)	x86_buneqr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_buneqr_d(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_buneqr_d(_jit, label, f0, f1));
}

#define jit_bunger_d(label, f0, f1)	x86_bunger_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunger_d(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunger_d(_jit, label, f0, f1));
}

#define jit_bungtr_d(label, f0, f1)	x86_bungtr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bungtr_d(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bungtr_d(_jit, label, f0, f1));
}

#define jit_bltgtr_d(label, f0, f1)	x86_bltgtr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bltgtr_d(jit_state_t _jit,
	     jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bltgtr_d(_jit, label, f0, f1));
}

#define jit_bordr_d(label, f0, f1)	x86_bordr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bordr_d(jit_state_t _jit,
	    jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bordr_d(_jit, label, f0, f1));
}

#define jit_bunordr_d(label, f0, f1)	x86_bunordr_d(_jit, label, f0, f1)
__jit_inline jit_insn *
x86_bunordr_d(jit_state_t _jit,
	      jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (sse_bunordr_d(_jit, label, f0, f1));
}

#define jit_prolog_d(nf)		x86_prolog_f(_jit, nf)
#define jit_prolog_f(nf)		x86_prolog_f(_jit, nf)
__jit_inline void
x86_prolog_f(jit_state_t _jit,
	     int nf)
{
    /* update counter of float arguments */
    if ((_jitl.nextarg_putfp += nf) > JIT_FP_ARG_MAX) {
	/* need float arguments on stack */
	int	argssize;

	if ((argssize = _jitl.nextarg_puti - JIT_ARG_MAX) < 0)
	    argssize = 0;
	argssize += _jitl.nextarg_putfp - JIT_FP_ARG_MAX;
	if (_jitl.argssize & 1)	{
	    /* stack appears to be padded */
	    _jitl.argssize = argssize;
	    /* stack still appears to be padded? */
	    if (!(_jitl.argssize & 1))
		/* update state for aligned stack */
		_jitl.framesize -= sizeof(double);
	}
	else {
	    _jitl.argssize = argssize;
	    /* stack appears to be padded now? */
	    if (_jitl.argssize & 1)
		/* update state for padded stack */
		_jitl.framesize += sizeof(double);
	}
    }
}

#define jit_prepare_d(nf)		x86_prepare_f(_jit, nf)
#define jit_prepare_f(nf)		x86_prepare_f(_jit, nf)
__jit_inline void
x86_prepare_f(jit_state_t _jit,
	      int nf)
{
    if ((_jitl.nextarg_putfp += nf) > JIT_FP_ARG_MAX) {
	/* need floats on stack */
	if ((_jitl.argssize = _jitl.nextarg_puti - JIT_ARG_MAX) < 0)
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
x86_pusharg_f(jit_state_t _jit,
	      jit_fpr_t f0)
{
    if (--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX) {
	jit_subi_l(JIT_SP, JIT_SP, sizeof(double));
	jit_str_f(JIT_SP, f0);
    }
    else
	jit_movr_f((jit_fpr_t)(_XMM0 + _jitl.nextarg_putfp), f0);
}

#define jit_pusharg_d(f0)		x86_pusharg_d(_jit, f0)
__jit_inline void
x86_pusharg_d(jit_state_t _jit,
	      jit_fpr_t f0)
{
    if (--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX) {
	jit_subi_l(JIT_SP, JIT_SP, sizeof(double));
	jit_str_d(JIT_SP, f0);
    }
    else
	jit_movr_d((jit_fpr_t)(_XMM0 + _jitl.nextarg_putfp), f0);
}

#endif /* __lightning_fp_h */
