/******************************** -*- C -*- ****************************
 *
 *	Support macros for the i386 math coprocessor
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2004, 2008 Free Software Foundation, Inc.
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
#define JIT_FPRET			0
#define JIT_FPR(i)			(i)

/* FIXME should be a runtime flags */

/* FISTTP, FCOMI and FUCOMI are p6 or newer */
#define jit_i686_p()			1

/* assume always round to nearest */
/* not assuming round to nearest averages more than 3 times slower than
 * fastest version, and slightly less then 3 times slower than pre p6
 * version, but works regardless of rounding mode */
#define jit_round_to_nearest_p()	1

#include "fp-x87.h"

#define jit_absr_f(f0, f1)		jit_absr_d(f0, f1)
#define jit_absr_d(f0, f1)		jit_absr_d(f0, f1)
__jit_inline void
jit_absr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    x87_absr_d(f0, f1);
}

#define jit_negr_d(f0, f1)		jit_negr_d(f0, f1)
__jit_inline void
jit_negr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    x87_negr_d(f0, f1);
}

#define jit_sqrtr_f(f0, f1)		jit_sqrtr_d(f0, f1)
#define jit_sqrtr_d(f0, f1)		jit_sqrtr_d(f0, f1)
__jit_inline void
jit_sqrtr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    x87_sqrtr_d(f0, f1);
}

#define jit_addr_d(f0, f1, f2)		jit_addr_d(f0, f1, f2)
__jit_inline void
jit_addr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    x87_addr_d(f0, f1, f2);
}

#define jit_subr_d(f0, f1, f2)		jit_subr_d(f0, f1, f2)
__jit_inline void
jit_subr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    x87_subr_d(f0, f1, f2);
}

#define jit_mulr_d(f0, f1, f2)		jit_mulr_d(f0, f1, f2)
__jit_inline void
jit_mulr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    x87_mulr_d(f0, f1, f2);
}

#define jit_divr_d(f0, f1, f2)		jit_divr_d(f0, f1, f2)
__jit_inline void
jit_divr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    x87_divr_d(f0, f1, f2);
}

#define jit_ldi_f(f0, i0)		jit_ldi_f(f0, i0)
__jit_inline void
jit_ldi_f(jit_fpr_t f0, void *i0)
{
    x87_ldi_f(f0, i0);
}

#define jit_ldr_f(f0, r0)		jit_ldr_f(f0, r0)
__jit_inline void
jit_ldr_f(jit_fpr_t f0, jit_gpr_t r0)
{
    x87_ldr_f(f0, r0);
}

#define jit_ldxi_f(f0, r0, i0)		jit_ldxi_f(f0, r0, i0)
__jit_inline void
jit_ldxi_f(jit_fpr_t f0, jit_gpr_t r0, int i0)
{
    x87_ldxi_f(f0, r0, i0);
}

#define jit_ldxr_f(f0, r0, r1)		jit_ldxr_f(f0, r0, r1)
__jit_inline void
jit_ldxr_f(jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    x87_ldxr_f(f0, r0, r1);
}

#define jit_ldi_d(f0, i0)		jit_ldi_d(f0, i0)
__jit_inline void
jit_ldi_d(jit_fpr_t f0, void *i0)
{
    x87_ldi_d(f0, i0);
}

#define jit_ldr_d(f0, r0)		jit_ldr_d(f0, r0)
__jit_inline void
jit_ldr_d(jit_fpr_t f0, jit_gpr_t r0)
{
    x87_ldr_d(f0, r0);
}

#define jit_ldxi_d(f0, r0, i0)		jit_ldxi_d(f0, r0, i0)
__jit_inline void
jit_ldxi_d(jit_fpr_t f0, jit_gpr_t r0, int i0)
{
    x87_ldxi_d(f0, r0, i0);
}

#define jit_ldxr_d(f0, r0, r1)		jit_ldxr_d(f0, r0, r1)
__jit_inline void
jit_ldxr_d(jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    x87_ldxr_d(f0, r0, r1);
}

#define jit_sti_f(i0, f0)		jit_sti_f(i0, f0)
__jit_inline void
jit_sti_f(void *i0, jit_fpr_t f0)
{
    x87_sti_f(i0, f0);
}

#define jit_str_f(r0, f0)		jit_str_f(r0, f0)
__jit_inline void
jit_str_f(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_str_f(r0, f0);
}

#define jit_stxi_f(i0, r0, f0)		jit_stxi_f(i0, r0, f0)
__jit_inline void
jit_stxi_f(int i0, jit_gpr_t r0, jit_fpr_t f0)
{
    x87_stxi_f(i0, r0, f0);
}

#define jit_stxr_f(r0, r1, f0)		jit_stxr_f(r0, r1, f0)
__jit_inline void
jit_stxr_f(jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    x87_stxr_f(r0, r1, f0);
}

#define jit_sti_d(i0, f0)		jit_sti_d(i0, f0)
__jit_inline void
jit_sti_d(void *i0, jit_fpr_t f0)
{
    x87_sti_d(i0, f0);
}

#define jit_str_d(r0, f0)		jit_str_d(r0, f0)
__jit_inline void
jit_str_d(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_str_d(r0, f0);
}

#define jit_stxi_d(i0, r0, f0)		jit_stxi_d(i0, r0, f0)
__jit_inline void
jit_stxi_d(int i0, jit_gpr_t r0, jit_fpr_t f0)
{
    x87_stxi_d(i0, r0, f0);
}

#define jit_stxr_d(r0, r1, f0)		jit_stxr_d(r0, r1, f0)
__jit_inline void
jit_stxr_d(jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    x87_stxr_d(r0, r1, f0);
}

#define jit_movi_f(f0, i0)		jit_movi_f(f0, i0)
__jit_inline void
jit_movi_f(jit_fpr_t f0, float i0)
{
    x87_movi_f(f0, i0);
}

#define jit_movi_d(f0, i0)		jit_movi_d(f0, i0)
__jit_inline void
jit_movi_d(jit_fpr_t f0, double i0)
{
    x87_movi_d(f0, i0);
}

#define jit_movr_d(f0, f1)		jit_movr_d(f0, f1)
__jit_inline void
jit_movr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    x87_movr_d(f0, f1);
}

#define jit_extr_i_d(f0, r0)		jit_extr_i_d(f0, r0)
__jit_inline void
jit_extr_i_d(jit_fpr_t f0, jit_gpr_t r0)
{
    x87_extr_i_d(f0, r0);
}

#define jit_rintr_d_i(r0, f0)		jit_rintr_d_i(r0, f0)
__jit_inline void
jit_rintr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_rintr_d_i(r0, f0);
}

#define jit_roundr_d_i(r0, f0)		jit_roundr_d_i(r0, f0)
__jit_inline void
jit_roundr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_roundr_d_i(r0, f0);
}

#define jit_truncr_d_i(r0, f0)		jit_truncr_d_i(r0, f0)
__jit_inline void
jit_truncr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_truncr_d_i(r0, f0);
}

#define jit_floorr_d_i(r0, f0)		jit_floorr_d_i(r0, f0)
__jit_inline void
jit_floorr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_floorr_d_i(r0, f0);
}

#define jit_ceilr_d_i(r0, f0)		jit_ceilr_d_i(r0, f0)
__jit_inline void
jit_ceilr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    x87_ceilr_d_i(r0, f0);
}

#define jit_ltr_d(r0, f0, f1)		jit_ltr_d(r0, f0, f1)
__jit_inline void
jit_ltr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ltr_d(r0, f0, f1);
}

#define jit_ler_d(r0, f0, f1)		jit_ler_d(r0, f0, f1)
__jit_inline void
jit_ler_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ler_d(r0, f0, f1);
}

#define jit_eqr_d(r0, f0, f1)		jit_eqr_d(r0, f0, f1)
__jit_inline void
jit_eqr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_eqr_d(r0, f0, f1);
}

#define jit_ger_d(r0, f0, f1)		jit_ger_d(r0, f0, f1)
__jit_inline void
jit_ger_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ger_d(r0, f0, f1);
}

#define jit_gtr_d(r0, f0, f1)		jit_gtr_d(r0, f0, f1)
__jit_inline void
jit_gtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_gtr_d(r0, f0, f1);
}

#define jit_ner_d(r0, f0, f1)		jit_ner_d(r0, f0, f1)
__jit_inline void
jit_ner_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ner_d(r0, f0, f1);
}

#define jit_unltr_d(r0, f0, f1)		jit_unltr_d(r0, f0, f1)
__jit_inline void
jit_unltr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_unltr_d(r0, f0, f1);
}

#define jit_unler_d(r0, f0, f1)		jit_unler_d(r0, f0, f1)
__jit_inline void
jit_unler_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_unler_d(r0, f0, f1);
}

#define jit_ltgtr_d(r0, f0, f1)		jit_ltgtr_d(r0, f0, f1)
__jit_inline void
jit_ltgtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ltgtr_d(r0, f0, f1);
}

#define jit_uneqr_d(r0, f0, f1)		jit_uneqr_d(r0, f0, f1)
__jit_inline void
jit_uneqr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_uneqr_d(r0, f0, f1);
}

#define jit_unger_d(r0, f0, f1)		jit_unger_d(r0, f0, f1)
__jit_inline void
jit_unger_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_unger_d(r0, f0, f1);
}

#define jit_ungtr_d(r0, f0, f1)		jit_ungtr_d(r0, f0, f1)
__jit_inline void
jit_ungtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ungtr_d(r0, f0, f1);
}

#define jit_ordr_d(r0, f0, f1)		jit_ordr_d(r0, f0, f1)
__jit_inline void
jit_ordr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_ordr_d(r0, f0, f1);
}

#define jit_unordr_d(r0, f0, f1)	jit_unordr_d(r0, f0, f1)
__jit_inline void
jit_unordr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    x87_unordr_d(r0, f0, f1);
}

#define jit_bltr_d(label, f0, f1)	jit_bltr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bltr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bltr_d(label, f0, f1));
}

#define jit_bler_d(label, f0, f1)	jit_bler_d(label, f0, f1)
__jit_inline jit_insn *
jit_bler_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bler_d(label, f0, f1));
}

#define jit_beqr_d(label, f0, f1)	jit_beqr_d(label, f0, f1)
__jit_inline jit_insn *
jit_beqr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_beqr_d(label, f0, f1));
}

#define jit_bger_d(label, f0, f1)	jit_bger_d(label, f0, f1)
__jit_inline jit_insn *
jit_bger_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bger_d(label, f0, f1));
}

#define jit_bgtr_d(label, f0, f1)	jit_bgtr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bgtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bgtr_d(label, f0, f1));
}

#define jit_bner_d(label, f0, f1)	jit_bner_d(label, f0, f1)
__jit_inline jit_insn *
jit_bner_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bner_d(label, f0, f1));
}

#define jit_bunltr_d(label, f0, f1)	jit_bunltr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunltr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bunltr_d(label, f0, f1));
}

#define jit_bunler_d(label, f0, f1)	jit_bunler_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunler_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bunler_d(label, f0, f1));
}

#define jit_bltgtr_d(label, f0, f1)	jit_bltgtr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bltgtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bltgtr_d(label, f0, f1));
}

#define jit_buneqr_d(label, f0, f1)	jit_buneqr_d(label, f0, f1)
__jit_inline jit_insn *
jit_buneqr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_buneqr_d(label, f0, f1));
}

#define jit_bunger_d(label, f0, f1)	jit_bunger_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunger_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bunger_d(label, f0, f1));
}

#define jit_bungtr_d(label, f0, f1)	jit_bungtr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bungtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bungtr_d(label, f0, f1));
}

#define jit_bordr_d(label, f0, f1)	jit_bordr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bordr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bordr_d(label, f0, f1));
}

#define jit_bunordr_d(label, f0, f1)	jit_bunordr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunordr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    return (x87_bunordr_d(label, f0, f1));
}

/* ABI */
#define jit_retval_d(f0)		jit_retval_d(f0)
__jit_inline void
jit_retval_d(jit_fpr_t f0)
{
    FSTPr(f0 + 1);
}

#define jit_pusharg_f(f0)		jit_pusharg_f(f0)
__jit_inline void
jit_pusharg_f(jit_fpr_t f0)
{
    jit_subi_i(JIT_SP, JIT_SP, sizeof(float));
    jit_str_f(JIT_SP, f0);
}

#define jit_pusharg_d(f0)		jit_pusharg_d(f0)
__jit_inline void
jit_pusharg_d(jit_fpr_t f0)
{
    jit_subi_i(JIT_SP, JIT_SP, sizeof(double));
    jit_str_d(JIT_SP, f0);
}

#define jit_prepare_f(nf)		jit_prepare_f(nf)
__jit_inline void
jit_prepare_f(int nf)
{
    _jitl.argssize += nf;
}

#define jit_prepare_d(nd)		jit_prepare_d(nd)
__jit_inline void
jit_prepare_d(int nd)
{
    _jitl.argssize += nd << 1;
}

#define jit_arg_f			jit_arg_f
__jit_inline int
jit_arg_f(void)
{
    int		off;
    off = _jitl.framesize;
    _jitl.framesize += sizeof(float);
    return (off);
}

#define jit_arg_d			jit_arg_d
__jit_inline int
jit_arg_d(void)
{
    int		off;
    off = _jitl.framesize;
    _jitl.framesize += sizeof(double);
    return (off);
}

#endif /* __lightning_fp_h */
