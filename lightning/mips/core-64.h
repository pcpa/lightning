/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (mips version)
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
 * Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/

#ifndef __lightning_core_h
#define __lightning_core_h

#define jit_movr_l(r0, r1)		mips_movr_l(_jit, r0, r1)
__jit_inline void
mips_movr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1)
	mips_rrr_t(_jit, r1, JIT_RZERO, r0, MIPS_DADDU);
}

#define jit_movi_l(r0, i0)		mips_movi_l(_jit, r0, i0)
__jit_inline void
mips_movi_l(jit_state_t _jit, jit_gpr_t r0, long i0)
{
    unsigned long	ms;

    ms = i0 & 0xffffffff00000000L;
    /* LUI sign extends */
    if (ms == 0 || ms == 0xffffffff)
	jit_movi_i(r0, i0);
    else {
	jit_movi_i(r0, ms >> 32);
	if ((ms = i0 & 0xffff0000)) {
	    mips__rrit(_jit, r0, r0, 16, MIPS_SLL);
	    mipshrri(_jit, MIPS_ORI, r0, r0, ms >> 16);
	    mips__rrit(_jit, r0, r0, 16, MIPS_SLL);
	}
	else
	    mips__rrit(_jit, r0, r0, 32, MIPS_SLL);
	if ((ms = i0 & 0xffff))
	    mipshrri(_jit, MIPS_ORI, r0, r0, ms);
    }
}

#define jit_negr_l(r0, r1)		mips_negr_l(_jit, r0, r1)
__jit_inline void
mips_negr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mips_rrr_t(_jit, JIT_RZERO, r1, r0, MIPS_DSUBU);
}

#define jit_addr_l(r0, r1, r2)		mips_addr_l(_jit, r0, r1, r2)
__jit_inline void
mips_addr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, r0, MIPS_DADDU);
}

#define jit_addi_l(r0, r1, i0)		mips_addi_l(_jit, r0, r1, i0)
__jit_inline void
mips_addi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_DADDIU, r0, r1, i0);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_addr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_subr_l(r0, r1, r2)		mips_subr_l(_jit, r0, r1, r2)
__jit_inline void
mips_subr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, r0, MIPS_DSUBU);
}

#define jit_subi_l(r0, r1, i0)		mips_subi_l(_jit, r0, r1, i0)
__jit_inline void
mips_subi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_DADDIU, r0, r1, -i0);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_subr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_mulr_l(r0, r1, r2)		mips_mulr_l(_jit, r0, r1, r2)
__jit_inline void
mips_mulr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DMULTU);
    mips___r_t(_jit, r0, MIPS_MFLO);
}

#define jit_muli_l(r0, r1, i0)		mips_muli_l(_jit, r0, r1, i0)
__jit_inline void
mips_muli_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_mulr_l(r0, r1, JIT_RTEMP);
}

#define jit_hmulr_l(r0, r1, r2)		mips_hmulr_l(_jit, r0, r1, r2)
__jit_inline void
mips_hmulr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DMULT);
    mips___r_t(_jit, r0, MIPS_MFHI);
}

#define jit_hmuli_l(r0, r1, i0)		mips_hmuli_l(_jit, r0, r1, i0)
__jit_inline void
mips_hmuli_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_hmulr_l(r0, r1, JIT_RTEMP);
}

#define jit_hmulr_ul(r0, r1, r2)	mips_hmulr_ul(_jit, r0, r1, r2)
__jit_inline void
mips_hmulr_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DMULTU);
    mips___r_t(_jit, r0, MIPS_MFHI);
}

#define jit_hmuli_ul(r0, r1, i0)	mips_hmuli_ul(_jit, r0, r1, i0)
__jit_inline void
mips_hmuli_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_hmulr_ul(r0, r1, JIT_RTEMP);
}

#define jit_divr_l(r0, r1, r2)		mips_divr_l(_jit, r0, r1, r2)
__jit_inline void
mips_divr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DDIV);
    mips___r_t(_jit, r0, MIPS_MFLO);
}

#define jit_divi_l(r0, r1, i0)		mips_divi_l(_jit, r0, r1, i0)
__jit_inline void
mips_divi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_divr_l(r0, r1, JIT_RTEMP);
}

#define jit_divr_ul(r0, r1, r2)		mips_divr_ul(_jit, r0, r1, r2)
__jit_inline void
mips_divr_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DDIVU);
    mips___r_t(_jit, r0, MIPS_MFLO);
}

#define jit_divi_ul(r0, r1, i0)		mips_divi_ul(_jit, r0, r1, i0)
__jit_inline void
mips_divi_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_divr_ul(r0, r1, JIT_RTEMP);
}

#define jit_modr_l(r0, r1, r2)		mips_modr_l(_jit, r0, r1, r2)
__jit_inline void
mips_modr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DDIV);
    mips___r_t(_jit, r0, MIPS_MFHI);
}

#define jit_modi_l(r0, r1, i0)		mips_modi_l(_jit, r0, r1, i0)
__jit_inline void
mips_modi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_modr_l(r0, r1, JIT_RTEMP);
}

#define jit_modr_ul(r0, r1, r2)		mips_modr_ul(_jit, r0, r1, r2)
__jit_inline void
mips_modr_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r1, r2, JIT_RZERO, MIPS_DDIVU);
    mips___r_t(_jit, r0, MIPS_MFHI);
}

#define jit_modi_ul(r0, r1, i0)		mips_modi_ul(_jit, r0, r1, i0)
__jit_inline void
mips_modi_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_modr_ul(r0, r1, JIT_RTEMP);
}

#define jit_andr_l(r0, r1, r2)		jit_andr_i(r0, r1, r2)
#define jit_andi_l(r0, r1, i0)		mips_andi_l(_jit, r0, r1, i0)
__jit_inline void
mips_andi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_ANDI, r0, r1, i0);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_andr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_orr_l(r0, r1, r2)		jit_orr_i(r0, r1, r2)
#define jit_ori_l(r0, r1, i0)		mips_ori_l(_jit, r0, r1, i0)
__jit_inline void
mips_ori_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_ORI, r0, r1, i0);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_orr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_xorr_l(r0, r1, r2)		jit_xorr_i(r0, r1, r2)
#define jit_xori_l(r0, r1, i0)		mips_xori_l(_jit, r0, r1, i0)
__jit_inline void
mips_xori_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	mipshrri(_jit, MIPS_XORI, r0, r1, i0);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_xorr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_lshr_l(r0, r1, r2)		mips_lshr_l(_jit, r0, r1, r2)
__jit_inline void
mips_lshr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_DSLLV);
}

#define jit_lshi_l(r0, r1, i0)		mips_lshi_l(_jit, r0, r1, i0)
__jit_inline void
mips_lshi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 63);
    if (i0 < 32)
	mips__rrit(_jit, r1, r0, i0, MIPS_DSLL);
    else
	mips__rrit(_jit, r1, r0, i0 - 32, MIPS_DSLL32);
}

#define jit_rshr_l(r0, r1, r2)		mips_rshr_l(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_DSRAV);
}

#define jit_rshi_l(r0, r1, i0)		mips_rshi_l(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 63);
    if (i0 < 32)
	mips__rrit(_jit, r1, r0, i0, MIPS_DSRA);
    else
	mips__rrit(_jit, r1, r0, i0 - 32, MIPS_DSRA32);
}

#define jit_rshr_ul(r0, r1, r2)		mips_rshr_ul(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    mips_rrr_t(_jit, r2, r1, r0, MIPS_DSRLV);
}

#define jit_rshi_ul(r0, r1, i0)		mips_rshi_ul(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 63);
    if (i0 < 32)
	mips__rrit(_jit, r1, r0, i0, MIPS_DSRL);
    else
	mips__rrit(_jit, r1, r0, i0 - 32, MIPS_DSRL32);
}

#define jit_ldr_ui(r0, r1)		mips_ldr_x(_jit,  MIPS_LWU, r0, r1)
#define jit_ldr_l(r0, r1)		mips_ldr_x(_jit,  MIPS_LD,  r0, r1)
#define jit_ldi_ui(r0, i0)		mips_ldi_x(_jit,  MIPS_LWU, r0, i0)
#define jit_ldi_l(r0, i0)		mips_ldi_x(_jit,  MIPS_LD,  r0, i0)
#define jit_ldxr_ui(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LWU, r0, r1, r2)
#define jit_ldxr_l(r0, r1, r2)		mips_ldxr_x(_jit, MIPS_LD,  r0, r1, r2)
#define jit_ldxi_ui(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LWU, r0, r1, i0)
#define jit_ldxi_l(r0, r1, i0)		mips_ldxi_x(_jit, MIPS_LD,  r0, r1, i0)
#define jit_str_l(r0, r1)		mips_str_x(_jit,  MIPS_SD,  r0, r1)
#define jit_sti_l(i0, r0)		mips_sti_x(_jit,  MIPS_SD,  i0, r0)
#define jit_stxr_l(r0, r1, r2)		mips_stxr_x(_jit, MIPS_SD,  r0, r1, r2)
#define jit_stxi_l(i0, r0, r1)		mips_stxi_x(_jit, MIPS_SD,  i0, r0, r1)

#define jit_extr_c_l(r0, r1)		mips_extr_c_l(_jit, r0, r1)
__jit_inline void
mips_extr_c_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_c_i(r0, r1);
    mips__rrit(_jit, r0, r0, 0, MIPS_SLL);
}

#define jit_extr_c_ul(r0, r1)		mips_extr_c_ul(_jit, r0, r1)
__jit_inline void
mips_extr_c_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_c_ui(r0, r1);
    mips__rrit(_jit, r0, r0, 0, MIPS_SLL);
}

#define jit_extr_s_l(r0, r1)		mips_extr_s_l(_jit, r0, r1)
__jit_inline void
mips_extr_s_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_s_i(r0, r1);
    mips__rrit(_jit, r0, r0, 0, MIPS_SLL);
}

#define jit_extr_s_ul(r0, r1)		mips_extr_s_ul(_jit, r0, r1)
#define jit_extr_us_ul(r0, r1)		mips_extr_us_ul(_jit, r0, r1)
__jit_inline void
mips_extr_s_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_s_ui(r0, r1);
    mips__rrit(_jit, r0, r0, 0, MIPS_SLL);
}

#define jit_extr_i_l(r0, r1)		mips_extr_i_l(_jit, r0, r1)
__jit_inline void
mips_extr_i_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    mips__rrit(_jit, r1, r0, 0, MIPS_SLL);
}

#define jit_extr_i_ul(r0, r1)		mips_extr_i_ul(_jit, r0, r1)
#define jit_extr_ui_ul(r0, r1)		mips_extr_ui_ul(_jit, r0, r1)
__jit_inline void
mips_extr_i_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_movr_l(r0, r1);
    mipshrriit(_jit, r0, MIPS_HDINS, JIT_RZERO, 32, 32, MIPS_TDINS);
}

#define jit_pusharg_ul(r0)		mips_pusharg_l(_jit, r0)
#define jit_pusharg_l(r0)		mips_pusharg_l(_jit, r0)
__jit_inline void
mips_pusharg_l(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.nextarg_puti > 0);
    if (--_jitl.nextarg_puti >= JIT_A_NUM) {
	_jitl.stack_offset -= sizeof(long);
	assert(_jitl.stack_offset >= 0);
	jit_stxi_l(_jitl.stack_offset, JIT_SP, r0);
    }
    else
	jit_movr_l(jit_a_order[_jitl.nextarg_puti], r0);
}

#define jit_arg_ul()			mips_arg_l(_jit)
#define jit_arg_l()			mips_arg_l(_jit)
__jit_inline int
mips_arg_l(jit_state_t _jit)
{
    int		ofs;

    if (_jitl.nextarg_geti < JIT_A_NUM) {
	ofs = _jitl.nextarg_geti;
	++_jitl.nextarg_geti;
    }
    else {
	ofs = _jitl.framesize;
	assert((ofs & 7) == 0);
	_jitl.framesize += sizeof(long);
    }

    return (ofs);
}

#define jit_getarg_ul(r0, ofs)		mips_getarg_l(_jit, r0, ofs)
#define jit_getarg_l(r0, ofs)		mips_getarg_l(_jit, r0, ofs)
__jit_inline void
mips_getarg_l(jit_state_t _jit, jit_gpr_t r0, int ofs)
{
    if (ofs < JIT_A_NUM)
	jit_movr_l(r0, jit_a_order[ofs]);
    else
	jit_ldxi_l(r0, JIT_FP, ofs);
}

#endif /* __lightning_core_h */
