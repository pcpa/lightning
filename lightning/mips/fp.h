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
#define jit_divr_f(f0, f1, f2)		_DIV_S(f0, f1, f2)
#define jit_divr_d(f0, f1, f2)		_DIV_D(f0, f1, f2)
#define jit_sqrtr_f(f0, f1)		_SQRT_S(f0, f1)
#define jit_sqrtr_d(f0, f1)		_SQRT_D(f0, f1)
#define jit_absr_f(f0, f1)		_ABS_S(f0, f1)
#define jit_absr_d(f0, f1)		_ABS_D(f0, f1)

#define jit_movr_f(f0, f1)		mips_movr_f(_jit, f0, f1)
__jit_inline void
mips_movr_f(jit_state_t _jit, jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 != f1)
	_MOV_S(f0, f1);
}

#define jit_movr_d(f0, f1)		mips_movr_d(_jit, f0, f1)
__jit_inline void
mips_movr_d(jit_state_t _jit, jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 != f1)
	_MOV_D(f0, f1);
}

#define jit_negr_f(f0, f1)	_NEG_S(f0, f1)
#define jit_negr_d(f0, f1)	_NEG_D(f0, f1)

#define jit_ldr_f(f0, r0)	mips_ldr_f(_jit, f0, r0)
__jit_inline void
mips_ldr_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   jit_fpr_t f0, jit_gpr_t r0)
{
    _LWC1(f0, 0, r0);
}

#define jit_ldr_d(f0, r0)	mips_ldr_d(_jit, f0, r0)
__jit_inline void
mips_ldr_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    _LDC1(f0, 0, r0);
}

#define jit_ldi_f(f0, i0)	mips_ldi_f(_jit, f0, i0)
__jit_inline void
mips_ldi_f(jit_state_t _jit, jit_fpr_t f0, void *i0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_LWC1(f0, ds, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_LWC1(f0, 0, JIT_RTEMP);
    }
}

#define jit_ldi_d(f0, i0)		mips_ldi_d(_jit, f0, i0)
__jit_inline void
mips_ldi_d(jit_state_t _jit, jit_fpr_t f0, void *i0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_LDC1(f0, ds, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_LDC1(f0, 0, JIT_RTEMP);
    }
}

#define jit_ldxr_f(f0, r0, r1)		mips_ldxr_f(_jit, f0, r0, r1)
__jit_inline void
mips_ldxr_f(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
#if 0 /* FIXME not disassembled */
    _LWXC1(f0, r1, r0);
#else
    jit_addr_i(JIT_RTEMP, r0, r1);
    _LWC1(f0, 0, JIT_RTEMP);
#endif
}

#define jit_ldxr_d(f0, r0, r1)		mips_ldxr_d(_jit, f0, r0, r1)
__jit_inline void
mips_ldxr_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
#if 0 /* FIXME not disassembled */
    _LDXC1(f0, r1, r0);
#else
    jit_addr_i(JIT_RTEMP, r0, r1);
    _LDC1(f0, 0, JIT_RTEMP);
#endif
}

#define jit_ldxi_f(f0, r0, i0)		mips_ldxi_f(_jit, f0, r0, i0)
__jit_inline void
mips_ldxi_f(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0, long i0)
{
    if (_s16P(i0))
	_LWC1(f0, i0, r0);
    else {
#if 0 /* FIXME not disassembled */
	jit_movi_i(JIT_RTEMP, i0);
	_LWXC1(f0, JIT_RTEMP, r0);
#else
	jit_addi_i(JIT_RTEMP, r0, i0);
	_LWC1(f0, 0, JIT_RTEMP);
#endif
    }
}

#define jit_ldxi_d(f0, r0, i0)		mips_ldxi_d(_jit, f0, r0, i0)
__jit_inline void
mips_ldxi_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0, long i0)
{
    if (_s16P(i0))
	_LDC1(f0, i0, r0);
    else {
#if 0 /* FIXME not disassembled */
	jit_movi_i(JIT_RTEMP, i0);
	_LDXC1(f0, JIT_RTEMP, r0);
#else
	jit_addi_i(JIT_RTEMP, r0, i0);
	_LDC1(f0, 0, JIT_RTEMP);
#endif
    }
}

#define jit_str_f(r0, f0)		mips_str_f(_jit, r0, f0)
__jit_inline void
mips_str_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   jit_gpr_t r0, jit_fpr_t f0)
{
    _SWC1(f0, 0, r0);
}

#define jit_str_d(r0, f0)		mips_str_d(_jit, r0, f0)
__jit_inline void
mips_str_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _SDC1(f0, 0, r0);
}

#define jit_sti_f(i0, f0)		mips_sti_f(_jit, i0, f0)
__jit_inline void
mips_sti_f(jit_state_t _jit, mips_hc_t hc, mips_fmt_t fm,
	   void *i0, jit_fpr_t f0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_SWC1(f0, ds, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_SWC1(f0, 0, JIT_RTEMP);
    }
}

#define jit_sti_d(i0, f0)		mips_sti_d(_jit, i0, f0)
__jit_inline void
mips_sti_d(jit_state_t _jit, void *i0, jit_fpr_t f0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_SDC1(f0, ds, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_SDC1(f0, 0, JIT_RTEMP);
    }
}

#define jit_stxr_f(r0, r1, f0)		mips_stxr_f(_jit, r0, r1, f0)
__jit_inline void
mips_stxr_f(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
#if 0	/* FIXME not disassembled */
    _SWXC1(f0, r1, r0);
#else
    jit_addr_i(JIT_RTEMP, r0, r1);
    _SWC1(f0, 0, JIT_RTEMP);
#endif
}

#define jit_stxr_d(r0, r1, f0)		mips_stxr_d(_jit, r0, r1, f0)
__jit_inline void
mips_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
#if 0	/* FIXME not disassembled */
    _SDXC1(f0, r1, r0);
#else
    jit_addr_i(JIT_RTEMP, r0, r1);
    _SDC1(f0, 0, JIT_RTEMP);
#endif
}

#define jit_stxi_f(i0, r0, f0)		mips_stxi_f(_jit, i0, r0, f0)
__jit_inline void
mips_stxi_f(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t f0)
{
    if (_s16P(i0))
	_SWC1(f0, i0, r0);
    else {
#if 0	/* FIXME not disassembled */
	jit_movi_i(JIT_RTEMP, i0);
	_SWXC1(f0, JIT_RTEMP, r0);
#else
	jit_addi_i(JIT_RTEMP, r0, i0);
	_SWC1(f0, 0, JIT_RTEMP);
#endif
    }
}

#define jit_stxi_d(i0, r0, f0)		mips_stxi_d(_jit, i0, r0, f0)
__jit_inline void
mips_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t f0)
{
    if (_s16P(i0))
	_SDC1(f0, i0, r0);
    else {
#if 0	/* FIXME not disassembled */
	jit_movi_i(JIT_RTEMP, i0);
	_SDXC1(f0, JIT_RTEMP, r0);
#else
	jit_addi_i(JIT_RTEMP, r0, i0);
	_SDC1(f0, 0, JIT_RTEMP);
#endif
    }
}

#define jit_extr_i_f(f0, r0)		mips_extr_i_f(_jit, f0, r0)
__jit_inline void
mips_extr_i_f(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    _MTC1(r0, JIT_FPTMP);
    _CVT_S_W(f0, JIT_FPTMP);
}

#define jit_extr_i_d(f0, r0)		mips_extr_i_d(_jit, f0, r0)
__jit_inline void
mips_extr_i_d(jit_state_t _jit, jit_fpr_t f0, jit_gpr_t r0)
{
    _MTC1(r0, JIT_FPTMP);
    _CVT_D_W(f0, JIT_FPTMP);
}

#define jit_extr_f_d(f0, r0)		mips_extr_f_d(_jit, f0, r0)
__jit_inline void
mips_extr_f_d(jit_state_t _jit, jit_fpr_t f0, jit_fpr_t f1)
{
    _CVT_D_S(f0, f1);
}

#define jit_extr_d_f(f0, r0)		mips_extr_d_f(_jit, f0, r0)
__jit_inline void
mips_extr_d_f(jit_state_t _jit, jit_fpr_t f0, jit_fpr_t f1)
{
    _CVT_S_D(f0, f1);
}

#define jit_roundr_f_i(r0, f0)		mips_roundr_f_i(_jit, r0, f0)
__jit_inline void
mips_roundr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    /* FIXME round to nearest and even on ties... */
    _ROUND_W_S(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_roundr_d_i(r0, f0)		mips_roundr_d_i(_jit, r0, f0)
__jit_inline void
mips_roundr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    /* FIXME round to nearest and even on ties... */
    _ROUND_W_D(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_rintr_f_i(r0, f0)		mips_rintr_f_i(_jit, r0, f0)
__jit_inline void
mips_rintr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CVT_S_W(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_rintr_d_i(r0, f0)		mips_rintr_d_i(_jit, r0, f0)
__jit_inline void
mips_rintr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CVT_D_W(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_truncr_f_i(r0, f0)		mips_truncr_f_i(_jit, r0, f0)
__jit_inline void
mips_truncr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _TRUNC_W_S(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_truncr_d_i(r0, f0)		mips_truncr_d_i(_jit, r0, f0)
__jit_inline void
mips_truncr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _TRUNC_W_D(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_ceilr_f_i(r0, f0)		mips_ceilr_f_i(_jit, r0, f0)
__jit_inline void
mips_ceilr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CEIL_W_S(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_ceilr_d_i(r0, f0)		mips_ceilr_d_i(_jit, r0, f0)
__jit_inline void
mips_ceilr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _CEIL_W_D(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_floorr_f_i(r0, f0)		mips_floorr_f_i(_jit, r0, f0)
__jit_inline void
mips_floorr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _FLOOR_W_S(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_floorr_d_i(r0, f0)		mips_floorr_d_i(_jit, r0, f0)
__jit_inline void
mips_floorr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0)
{
    _FLOOR_W_D(JIT_FPTMP, f0);
    _MFC1(r0, JIT_FPTMP);
}

#define jit_ltr_f(r0, f0, f1)		mips_ltr_f(_jit, r0, f0, f1)
__jit_inline void
mips_ltr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLT_S(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ltr_d(r0, f0, f1)		mips_ltr_d(_jit, r0, f0, f1)
__jit_inline void
mips_ltr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLT_D(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ler_f(r0, f0, f1)		mips_ler_f(_jit, r0, f0, f1)
__jit_inline void
mips_ler_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLE_S(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ler_d(r0, f0, f1)		mips_ler_d(_jit, r0, f0, f1)
__jit_inline void
mips_ler_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLE_D(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_eqr_f(r0, f0, f1)		mips_eqr_f(_jit, r0, f0, f1)
__jit_inline void
mips_eqr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_EQ_S(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_eqr_d(r0, f0, f1)		mips_eqr_d(_jit, r0, f0, f1)
__jit_inline void
mips_eqr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_EQ_D(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ger_f(r0, f0, f1)		mips_ger_f(_jit, r0, f0, f1)
__jit_inline void
mips_ger_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLT_S(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ger_d(r0, f0, f1)		mips_ger_d(_jit, r0, f0, f1)
__jit_inline void
mips_ger_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLT_D(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_gtr_f(r0, f0, f1)		mips_gtr_f(_jit, r0, f0, f1)
__jit_inline void
mips_gtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLE_S(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_gtr_d(r0, f0, f1)		mips_gtr_d(_jit, r0, f0, f1)
__jit_inline void
mips_gtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_OLE_D(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ner_f(r0, f0, f1)		mips_ner_f(_jit, r0, f0, f1)
__jit_inline void
mips_ner_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_EQ_S(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_ner_d(r0, f0, f1)		mips_ner_d(_jit, r0, f0, f1)
__jit_inline void
mips_ner_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_EQ_D(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_unltr_f(r0, f0, f1)		mips_unltr_f(_jit, r0, f0, f1)
__jit_inline void
mips_unltr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULT_S(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_unltr_d(r0, f0, f1)		mips_unltr_d(_jit, r0, f0, f1)
__jit_inline void
mips_unltr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULT_D(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_unler_f(r0, f0, f1)		mips_unler_f(_jit, r0, f0, f1)
__jit_inline void
mips_unler_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULE_S(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_unler_d(r0, f0, f1)		mips_unler_d(_jit, r0, f0, f1)
__jit_inline void
mips_unler_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULE_D(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_uneqr_f(r0, f0, f1)		mips_uneqr_f(_jit, r0, f0, f1)
__jit_inline void
mips_uneqr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UEQ_S(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_uneqr_d(r0, f0, f1)		mips_uneqr_d(_jit, r0, f0, f1)
__jit_inline void
mips_uneqr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UEQ_D(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_unger_f(r0, f0, f1)		mips_unger_f(_jit, r0, f0, f1)
__jit_inline void
mips_unger_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULT_S(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_unger_d(r0, f0, f1)		mips_unger_d(_jit, r0, f0, f1)
__jit_inline void
mips_unger_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULT_D(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ungtr_f(r0, f0, f1)		mips_ungtr_f(_jit, r0, f0, f1)
__jit_inline void
mips_ungtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULE_S(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ungtr_d(r0, f0, f1)		mips_ungtr_d(_jit, r0, f0, f1)
__jit_inline void
mips_ungtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_ULE_D(f1, f0);
    _MOVF(r0, JIT_RZERO);
}

#define jit_ltgtr_f(r0, f0, f1)		mips_ltgtr_f(_jit, r0, f0, f1)
__jit_inline void
mips_ltgtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UEQ_S(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_ltgtr_d(r0, f0, f1)		mips_ltgtr_d(_jit, r0, f0, f1)
__jit_inline void
mips_ltgtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UEQ_D(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_ordr_f(r0, f0, f1)		mips_ordr_f(_jit, r0, f0, f1)
__jit_inline void
mips_ordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UN_S(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_ordr_d(r0, f0, f1)		mips_ordr_d(_jit, r0, f0, f1)
__jit_inline void
mips_ordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UN_D(f0, f1);
    _MOVT(r0, JIT_RZERO);
}

#define jit_unordr_f(r0, f0, f1)	mips_unordr_f(_jit, r0, f0, f1)
__jit_inline void
mips_unordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UN_S(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_unordr_d(r0, f0, f1)	mips_unordr_d(_jit, r0, f0, f1)
__jit_inline void
mips_unordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_movi_i(r0, 1);
    _C_UN_D(f0, f1);
    _MOVF(r0, JIT_RZERO);
}

#define jit_bltr_f(i0, f0, f1)		mips_bltr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bltr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLT_S(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bltr_d(i0, f0, f1)		mips_bltr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bltr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLT_D(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bler_f(i0, f0, f1)		mips_bler_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bler_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLE_S(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bler_d(i0, f0, f1)		mips_bler_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bler_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLE_D(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_beqr_f(i0, f0, f1)		mips_beqr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_beqr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_EQ_S(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_beqr_d(i0, f0, f1)		mips_beqr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_beqr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_EQ_D(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bger_f(i0, f0, f1)		mips_bger_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bger_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLT_S(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bger_d(i0, f0, f1)		mips_bger_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bger_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLT_D(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bgtr_f(i0, f0, f1)		mips_bgtr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bgtr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLE_S(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bgtr_d(i0, f0, f1)		mips_bgtr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bgtr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_OLE_D(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bner_f(i0, f0, f1)		mips_bner_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bner_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_EQ_S(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bner_d(i0, f0, f1)		mips_bner_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bner_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_EQ_D(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bunltr_f(i0, f0, f1)	mips_bunltr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunltr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULT_S(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bunltr_d(i0, f0, f1)	mips_bunltr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunltr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULT_D(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bunler_f(i0, f0, f1)	mips_bunler_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunler_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULE_S(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bunler_d(i0, f0, f1)	mips_bunler_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunler_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULE_D(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_buneqr_f(i0, f0, f1)	mips_buneqr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_buneqr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UEQ_S(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_buneqr_d(i0, f0, f1)	mips_buneqr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_buneqr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UEQ_D(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bunger_f(i0, f0, f1)	mips_bunger_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunger_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULT_S(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bunger_d(i0, f0, f1)	mips_bunger_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunger_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULT_D(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bungtr_f(i0, f0, f1)	mips_bungtr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bungtr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULE_S(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bungtr_d(i0, f0, f1)	mips_bungtr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bungtr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_ULE_D(f1, f0);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bltgtr_f(i0, f0, f1)	mips_bltgtr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bltgtr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UEQ_S(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bltgtr_d(i0, f0, f1)	mips_bltgtr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bltgtr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UEQ_D(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bordr_f(i0, f0, f1)		mips_bordr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bordr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UN_S(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bordr_d(i0, f0, f1)		mips_bordr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bordr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UN_D(f0, f1);
    _BC1F((long)i0);
    return (_jit->x.pc);
}

#define jit_bunordr_f(i0, f0, f1)	mips_bunordr_f(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunordr_f(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UN_S(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
}

#define jit_bunordr_d(i0, f0, f1)	mips_bunordr_d(_jit, i0, f0, f1)
__jit_inline jit_insn *
mips_bunordr_d(jit_state_t _jit, jit_insn * i0, jit_fpr_t f0, jit_fpr_t f1)
{
    _C_UN_D(f0, f1);
    _BC1T((long)i0);
    return (_jit->x.pc);
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
