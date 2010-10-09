/******************************** -*- C -*- ****************************
 *
 *	Floating-point support (mips)
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2010 Free Software Foundation, Inc.
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
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/

#ifndef __lightning_fp_mips_h
#define __lightning_fp_mips_h

#define JIT_FPR_NUM			6
static const jit_fpr_t
jit_f_order[JIT_FPR_NUM] = {
    _F0, _F2,  _F4,  _F6, _F8, _F10
};

#define JIT_FA_NUM			2
static const jit_fpr_t
jit_fa_order[JIT_FA_NUM] = {
    _F12, _F14
};

#define JIT_FPR(n)			jit_f_order[n]

#define JIT_FPRET			_F0
#define JIT_FPTMP			_F30

#define jit_addr_f(f0, f1, f2)		_ADD_S(f0, f1, f2)
#define jit_addr_d(f0, f1, f2)		_ADD_D(f0, f1, f2)
#define jit_subr_f(f0, f1, f2)		_SUB_S(f0, f1, f2)
#define jit_subr_d(f0, f1, f2)		_SUB_D(f0, f1, f2)
#define jit_mulr_f(f0, f1, f2)		_MUL_S(f0, f1, f2)
#define jit_mulr_d(f0, f1, f2)		_MUL_D(f0, f1, f2)

#define jit_divr_f(f0, f1, f2)						\
    mips_fp2(_jit, MIPS_fmt_S, f2, f1, f0, MIPS_DIV_fmt)
#define jit_divr_d(f0, f1, f2)						\
    mips_fp2(_jit, MIPS_fmt_D, f2, f1, f0, MIPS_DIV_fmt)

#define jit_sqrtr_f(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_S, f1, f0, MIPS_SQRT_fmt)
#define jit_sqrtr_d(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_D, f1, f0, MIPS_SQRT_fmt)

#define jit_absr_f(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_S, f1, f0, MIPS_ABS_fmt)
#define jit_absr_d(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_D, f1, f0, MIPS_ABS_fmt)

#define jit_movr_f(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_S, f1, f0, MIPS_MOV_fmt)
#define jit_movr_d(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_D, f1, f0, MIPS_MOV_fmt)

#define jit_negr_f(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_S, f1, f0, MIPS_NEG_fmt)
#define jit_negr_d(f0, f1)						\
    mips_fp1(_jit, MIPS_fmt_D, f1, f0, MIPS_NEG_fmt)

#define jit_ldr_f(f0, r0)						\
    mips_ldr_f(_jit, MIPS_LWC1, MIPS_fmt_S, f0, r0)
#define jit_ldr_d(f0, r0)						\
    mips_ldr_f(_jit, MIPS_LDC1, MIPS_fmt_D, f0, r0)
__jit_inline void
mips_ldr_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   jit_fpr_t f0, jit_gpr_t r0)
{
    mipshrfi(_jit, hc, fm, r0, f0, 0);
}

#define jit_ldi_f(f0, i0)						\
    mips_ldi_f(_jit, MIPS_LWC1, MIPS_fmt_S, f0, i0)
#define jit_ldi_d(f0, i0)						\
    mips_ldi_f(_jit, MIPS_LDC1, MIPS_fmt_D, f0, i0)
__jit_inline void
mips_ldi_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   jit_fpr_t f0, void *i0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	mipshrfi(_jit, hc, fm, JIT_RZERO, f0, ds);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	mipshrfi(_jit, hc, fm, JIT_RTEMP, f0, 0);
    }
}

#define jit_ldxr_f(f0, r0, r1)						\
    mips_ldxr_f(_jit, MIPS_LWC1, MIPS_fmt_S, f0, r0, r1, MIPS_LWXC1)
#define jit_ldxr_d(f0, r0, r1)						\
    mips_ldxr_f(_jit, MIPS_LDC1, MIPS_fmt_D, f0, r0, r1, MIPS_LDXC1)
__jit_inline void
mips_ldxr_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	    jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1,
	    mips_tc_t tc)
{
#if 0 /* FIXME not disassembled */
    mips_rr_f(_jit, r0, r1, f0, tc);
#else
    jit_addr_i(JIT_RTEMP, r0, r1);
    mipshrfi(_jit, hc, fm, JIT_RTEMP, f0, 0);
#endif
}

#define jit_ldxi_f(f0, r0, i0)						\
    mips_ldxi_f(_jit, MIPS_LWC1, MIPS_fmt_S, f0, r0, i0, MIPS_LWXC1)
#define jit_ldxi_d(f0, r0, i0)						\
    mips_ldxi_f(_jit, MIPS_LDC1, MIPS_fmt_D, f0, r0, i0, MIPS_LDXC1)
__jit_inline void
mips_ldxi_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	    jit_fpr_t f0, jit_gpr_t r0, long i0, mips_tc_t tc)
{
    if (_s16P(i0))
	mipshrfi(_jit, hc, fm, r0, f0, i0);
    else {
#if 0 /* FIXME not disassembled */
	jit_movi_i(JIT_RTEMP, i0);
	mips_rr_f(_jit, r0, JIT_RTEMP, f0, tc);
#else
    jit_addi_i(JIT_RTEMP, r0, i0);
    mipshrfi(_jit, hc, fm, JIT_RTEMP, f0, 0);
#endif
    }
}

#define jit_str_f(r0, f0)						\
    mips_str_f(_jit, MIPS_SWC1, MIPS_fmt_S, r0, f0)
#define jit_str_d(r0, f0)						\
    mips_str_f(_jit, MIPS_SDC1, MIPS_fmt_D, r0, f0)
__jit_inline void
mips_str_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   jit_gpr_t r0, jit_fpr_t f0)
{
    mipshrfi(_jit, hc, fm, r0, f0, 0);
}

#define jit_sti_f(i0, f0)						\
    mips_sti_f(_jit, MIPS_SWC1, MIPS_fmt_S, i0, f0)
#define jit_sti_d(i0, f0)						\
    mips_sti_f(_jit, MIPS_SDC1, MIPS_fmt_D, i0, f0)
__jit_inline void
mips_sti_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   void *i0, jit_fpr_t f0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	mipshrfi(_jit, hc, fm, JIT_RZERO, f0, ds);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	mipshrfi(_jit, hc, fm, JIT_RTEMP, f0, 0);
    }
}

#define jit_stxr_f(r0, r1, f0)						\
    mips_stxr_f(_jit, MIPS_SWC1, MIPS_fmt_S, r0, r1, f0, MIPS_SWXC1)
#define jit_stxr_d(r0, r1, f0)						\
    mips_stxr_f(_jit, MIPS_SDC1, MIPS_fmt_D, r0, r1, f0, MIPS_SDXC1)
__jit_inline void
mips_stxr_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	    jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0, mips_tc_t tc)
{
#if 0	/* FIXME not disassembled */
    mips_rrf_(_jit, r0, r1, f0, tc);
#else
    jit_addr_i(JIT_RTEMP, r0, r1);
    mipshrfi(_jit, hc, fm, JIT_RTEMP, f0, 0);
#endif
}

#define jit_stxi_f(i0, r0, f0)						\
    mips_stxi_f(_jit, MIPS_SWC1, MIPS_fmt_S, i0, r0, f0, MIPS_SWXC1)
#define jit_stxi_d(i0, r0, f0)						\
    mips_stxi_f(_jit, MIPS_SDC1, MIPS_fmt_D, i0, r0, f0, MIPS_SDXC1)
__jit_inline void
mips_stxi_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	    int i0, jit_gpr_t r0, jit_fpr_t f0, mips_tc_t tc)
{
    if (_s16P(i0))
	mipshrfi(_jit, hc, fm, r0, f0, i0);
    else {
#if 0	/* FIXME not disassembled */
	jit_movi_i(JIT_RTEMP, i0);
	mips_rrf_(_jit, r0, JIT_RTEMP, f0, tc);
#else
    jit_addi_i(JIT_RTEMP, r0, i0);
    mipshrfi(_jit, hc, fm, JIT_RTEMP, f0, 0);
#endif
    }
}

#define jit_extr_i_f(f0, r0)		mips_extr_i_f(_jit, f0, r0)
__jit_inline void
mips_extr_i_f(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    mips_xrf(_jit, MIPS_fmt_l_if, r0, JIT_FPTMP);
    mips_fp1(_jit, MIPS_fmt_S, f0, JIT_FPTMP, MIPS_CVT_fmt_W);
}

#define jit_extr_i_d(f0, r0)		mips_extr_i_d(_jit, f0, r0)
__jit_inline void
mips_extr_i_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    mips_xrf(_jit, MIPS_fmt_l_if, r0, JIT_FPTMP);
    mips_fp1(_jit, MIPS_fmt_D, f0, JIT_FPTMP, MIPS_CVT_fmt_W);
}

#define jit_extr_f_d(f0, r0)		mips_extr_f_d(_jit, f0, r0)
__jit_inline void
mips_extr_f_d(jit_state_t _jit, jit_fpr_t f0, jit_fpr_t f1)
{
    mips_fp1(_jit, MIPS_fmt_D, f0, f1, MIPS_CVT_fmt_S);
}

#define jit_extr_d_f(f0, r0)		mips_extr_d_f(_jit, f0, r0)
__jit_inline void
mips_extr_d_f(jit_state_t _jit, jit_fpr_t f0, jit_fpr_t f1)
{
    mips_fp1(_jit, MIPS_fmt_S, f0, f1, MIPS_CVT_fmt_D);
}

#define jit_roundr_f_i(r0, f0)		mips_roundr_f_i(_jit, r0, f0)
__jit_inline void
mips_roundr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_W, f0, JIT_FPTMP, MIPS_CVT_fmt_S);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_roundr_d_i(r0, f0)		mips_roundr_d_i(_jit, r0, f0)
__jit_inline void
mips_roundr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_W, f0, JIT_FPTMP, MIPS_CVT_fmt_D);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_truncr_f_i(r0, f0)		mips_truncr_f_i(_jit, r0, f0)
__jit_inline void
mips_truncr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_S, f0, JIT_FPTMP, MIPS_TRUNC_fmt_W);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_truncr_d_i(r0, f0)		mips_truncr_d_i(_jit, r0, f0)
__jit_inline void
mips_truncr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_D, f0, JIT_FPTMP, MIPS_TRUNC_fmt_W);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_ceilr_f_i(r0, f0)		mips_ceilr_f_i(_jit, r0, f0)
__jit_inline void
mips_ceilr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    CEIL_W_S(JIT_FPTMP, f0);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_ceilr_d_i(r0, f0)		mips_ceilr_d_i(_jit, r0, f0)
__jit_inline void
mips_ceilr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    CEIL_W_D(JIT_FPTMP, f0);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_floorr_f_i(r0, f0)		mips_floorr_f_i(_jit, r0, f0)
__jit_inline void
mips_floorr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_S, f0, JIT_FPTMP, MIPS_FLOOR_fmt_W);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#define jit_floorr_d_i(r0, f0)		mips_floorr_d_i(_jit, r0, f0)
__jit_inline void
mips_floorr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    mips_fp1(_jit, MIPS_fmt_D, f0, JIT_FPTMP, MIPS_FLOOR_fmt_W);
    mips_xrf(_jit, MIPS_fmt_l_fi, r0, JIT_FPTMP);
}

#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_MIPS64 \
	: defined (__mips64)
#  include "fp-64.h"
#endif

#define jit_prepare_d(count)		mips_prepare_f(_jit, count)
#define jit_prepare_f(count)		mips_prepare_f(_jit, count)
__jit_inline void
mips_prepare_f(jit_state_t _jit, int count)
{
    assert(count >= 0);
    _jitl.nextarg_putf += count;
    if (_jitl.nextarg_putf > JIT_FA_NUM) {
	if ((count = (_jitl.nextarg_puti - JIT_A_NUM)) < 0)
	    count = 0;
	count += _jitl.nextarg_putf - JIT_FA_NUM;
	_jitl.stack_offset = count << 3;
	if (_jitl.stack_length < _jitl.stack_offset) {
	    _jitl.stack_length = _jitl.stack_offset;
	    *_jitl.stack = (_jitl.alloca_offset +
			    _jitl.stack_length + 7) & ~7;
	}
    }
}

#define jit_arg_d()			mips_arg_f(_jit)
#define jit_arg_f()			mips_arg_f(_jit)
__jit_inline int
mips_arg_f(jit_state_t _jit)
{
    int		ofs;

    if (_jitl.nextarg_getf < JIT_FA_NUM)
	ofs = _jitl.nextarg_getf++;
    else {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(double);
    }

    return (ofs);
}

#define jit_getarg_f(f0, ofs)		mips_getarg_f(_jit, f0, ofs)
__jit_inline void
mips_getarg_f(jit_state_t _jit, jit_fpr_t f0, int ofs)
{
    if (ofs < JIT_FA_NUM)
	jit_movr_f(f0, jit_fa_order[ofs]);
    else
	jit_ldxi_f(f0, JIT_FP, ofs);
}

#define jit_getarg_d(f0, ofs)		mips_getarg_d(_jit, f0, ofs)
__jit_inline void
mips_getarg_d(jit_state_t _jit, jit_fpr_t f0, int ofs)
{
    if (ofs < JIT_FA_NUM)
	jit_movr_d(f0, jit_fa_order[ofs]);
    else
	jit_ldxi_d(f0, JIT_FP, ofs);
}

#define jit_pusharg_f(f0)		mips_pusharg_f(_jit, f0)
__jit_inline void
mips_pusharg_f(jit_state_t _jit, jit_fpr_t f0)
{
    assert(_jitl.nextarg_putf > 0);
    if (--_jitl.nextarg_putf >= JIT_FA_NUM) {
	_jitl.stack_offset -= sizeof(double);
	assert(_jitl.stack_offset >= 0);
	jit_stxi_f(_jitl.stack_offset, JIT_SP, f0);
    }
    else
	jit_movr_f(jit_fa_order[_jitl.nextarg_putf], f0);
}

#define jit_pusharg_d(f0)		mips_pusharg_d(_jit, f0)
__jit_inline void
mips_pusharg_d(jit_state_t _jit, jit_fpr_t f0)
{
    assert(_jitl.nextarg_putf > 0);
    if (--_jitl.nextarg_putf >= JIT_FA_NUM) {
	_jitl.stack_offset -= sizeof(double);
	assert(_jitl.stack_offset >= 0);
	jit_stxi_d(_jitl.stack_offset, JIT_SP, f0);
    }
    else
	jit_movr_d(jit_fa_order[_jitl.nextarg_putf], f0);
}

#endif /* __lightning_fp_mips_h */
