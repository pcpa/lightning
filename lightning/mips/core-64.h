/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (mips64 version)
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

#define jit_movr_l(r0, r1)		mips_movr_i(_jit, r0, r1)

#define jit_movi_l(r0, i0)		mips_movi_l(_jit, r0, i0)
__jit_inline void
mips_movi_l(jit_state_t _jit, jit_gpr_t r0, long i0)
{
    unsigned long	ms;

    ms = i0 & 0xffffffff00000000L;
    /* LUI sign extends */
    if ((ms == 0	  && !(i0 & 0x80000000))  ||
	(ms == 0xffffffff &&  (i0 & 0x80000000)))
	jit_movi_i(r0, i0);
    else {
	jit_movi_i(r0, ms >> 32);
	if ((ms = i0 & 0xffff0000)) {
	    _SLL(r0, r0, 16);
	    _ORI(r0, r0, ms >> 16);
	    _SLL(r0, r0, 16);
	}
	else
	    _SLL(r0, r0, 16);
	if ((ms = i0 & 0xffff))
	    _ORI(r0, r0, ms);
    }
}

#define jit_movi_p(r0, i0)		mips_movi_p(_jit, r0, i0)
__jit_inline jit_insn *
mips_movi_p(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    unsigned long	ms;

    ms = i0 & 0xffff000000000000L;
    _ORI(r0, r0, ms >> 48);
    _SLL(r0, r0, 16);
    ms = i0 & 0x0000ffff00000000L;
    _ORI(r0, r0, ms >> 32);
    _SLL(r0, r0, 16);
    ms = i0 & 0x00000000ffff0000L;
    _ORI(r0, r0, ms >> 16);
    _SLL(r0, r0, 16);
    ms = i0 & 0x000000000000ffffL;
    _ORI(r0, r0, ms);
    return (_jit->x.pc);
}

#define jit_negr_l(r0, r1)		mips_negr_l(_jit, r0, r1)
__jit_inline void
mips_negr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _DSUBU(r0, JIT_RZERO, r1);
}

#define jit_addr_l(r0, r1, r2)		mips_addr_l(_jit, r0, r1, r2)
__jit_inline void
mips_addr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DADDU(r0, r1, r2);
}

#define jit_addi_l(r0, r1, i0)		mips_addi_l(_jit, r0, r1, i0)
__jit_inline void
mips_addi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	_DADDIU(r0, r1, i0 & 0xffff);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_addr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_subr_l(r0, r1, r2)		mips_subr_l(_jit, r0, r1, r2)
__jit_inline void
mips_subr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DSUBU(r0, r1, r2);
}

#define jit_subi_l(r0, r1, i0)		mips_subi_l(_jit, r0, r1, i0)
__jit_inline void
mips_subi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0) && i0 != 0x8000)
	_DADDIU(r0, r1, -i0 & 0xffff);
    else {
	jit_movi_l(JIT_RTEMP, i0);
	jit_subr_l(r0, r1, JIT_RTEMP);
    }
}

#define jit_mulr_l(r0, r1, r2)		mips_mulr_l(_jit, r0, r1, r2)
__jit_inline void
mips_mulr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DMULTU(r1, r2);
    _MFLO(r0);
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
    _DMULT(r1, r2);
    _MFHI(r0);
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
    _DMULTU(r1, r2);
    _MFHI(r0);
}

#define jit_hmuli_ul(r0, r1, i0)	mips_hmuli_ul(_jit, r0, r1, i0)
__jit_inline void
mips_hmuli_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_hmulr_ul(r0, r1, JIT_RTEMP);
}

#define jit_divr_l(r0, r1, r2)		mips_divr_l(_jit, r0, r1, r2)
__jit_inline void
mips_divr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DDDIV(r1, r2);
    _MFLO(r0);
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
    _DDDIVU(r1, r2);
    _MFLO(r0);
}

#define jit_divi_ul(r0, r1, i0)		mips_divi_ul(_jit, r0, r1, i0)
__jit_inline void
mips_divi_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_divr_ul(r0, r1, JIT_RTEMP);
}

#define jit_modr_l(r0, r1, r2)		mips_modr_l(_jit, r0, r1, r2)
__jit_inline void
mips_modr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DDDIV(r1, r2);
    _MFHI(r0);
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
    _DDDIVU(r1, r2);
    _MFHI(r0);
}

#define jit_modi_ul(r0, r1, i0)		mips_modi_ul(_jit, r0, r1, i0)
__jit_inline void
mips_modi_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, unsigned long i0)
{
    jit_movi_l(JIT_RTEMP, i0);
    jit_modr_ul(r0, r1, JIT_RTEMP);
}

#define jit_andr_l(r0, r1, r2)		jit_andr_i(r0, r1, r2)
#define jit_andi_l(r0, r1, i0)		jit_andi_i(r0, r1, i0)
#define jit_orr_l(r0, r1, r2)		jit_orr_i(r0, r1, r2)
#define jit_ori_l(r0, r1, i0)		jit_ori_i(r0, r1, i0)
#define jit_xorr_l(r0, r1, r2)		jit_xorr_i(r0, r1, r2)
#define jit_xori_l(r0, r1, i0)		jit_xori_i(r0, r1, i0)

#define jit_lshr_l(r0, r1, r2)		mips_lshr_l(_jit, r0, r1, r2)
__jit_inline void
mips_lshr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DSLLV(r0, r1, r2);
}

#define jit_lshi_l(r0, r1, i0)		mips_lshi_l(_jit, r0, r1, i0)
__jit_inline void
mips_lshi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    assert(i0 <= 63);
    if (i0 < 32)
	_DSLL(r0, r1, i0);
    else
	_DSLL32(r0, r1, i0);
}

#define jit_rshr_l(r0, r1, r2)		mips_rshr_l(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DSRAV(r0, r1, r2);
}

#define jit_rshi_l(r0, r1, i0)		mips_rshi_l(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    assert(i0 <= 63);
    if (i0 < 32)
	_DSRA(r0, r1, i0);
    else
	_DSRA32(r0, r1, i0);
}

#define jit_rshr_ul(r0, r1, r2)		mips_rshr_ul(_jit, r0, r1, r2)
__jit_inline void
mips_rshr_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _DSRLV(r0, r1, r2);
}

#define jit_rshi_ul(r0, r1, i0)		mips_rshi_ul(_jit, r0, r1, i0)
__jit_inline void
mips_rshi_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, unsigned char i0)
{
    assert(i0 <= 63);
    if (i0 < 32)
	_DSRL(r0, r1, i0);
    else
	_DSRL32(r0, r1, i0);
}

#define jit_ldr_ui(r0, r1)		mips_ldr_ui(_jit, r0, r1)
__jit_inline void
mips_ldr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LWU(r0, 0, r1);
}

#define jit_ldr_l(r0, r1)		mips_ldr_l(_jit, r0, r1)
__jit_inline void
mips_ldr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LD(r0, 0, r1);
}

#define jit_ldi_ui(r0, i0)		mips_ldi_ui(_jit, r0, i0)
__jit_inline void
mips_ldi_ui(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_LWU(r0, ds & 0xffff, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_LWU(r0, 0, JIT_RTEMP);
    }
}

#define jit_ldi_l(r0, i0)		mips_ldi_l(_jit, r0, i0)
__jit_inline void
mips_ldi_l(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_LD(r0, ds & 0xffff, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_LD(r0, 0, JIT_RTEMP);
    }
}

#define jit_ldxr_ui(r0, r1, r2)		mips_ldxr_ui(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    _LWU(r0, 0, JIT_RTEMP);
}

#define jit_ldxr_l(r0, r1, r2)		mips_ldxr_l(_jit, r0, r1, r2)
__jit_inline void
mips_ldxr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r1, r2);
    _LD(r0, 0, JIT_RTEMP);
}

#define jit_ldxi_ui(r0, r1, i0)		mips_ldxi_ui(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	_LWU(r0, i0 & 0xffff, r1);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	_LWU(r0, 0, JIT_RTEMP);
    }
}

#define jit_ldxi_l(r0, r1, i0)		mips_ldxi_l(_jit, r0, r1, i0)
__jit_inline void
mips_ldxi_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, long i0)
{
    if (_s16P(i0))
	_LD(r0, i0 & 0xffff, r1);
    else {
	jit_addi_i(JIT_RTEMP, r1, i0);
	_LD(r0, 0, JIT_RTEMP);
    }
}

#define jit_str_l(r0, r1)		mips_str_l(_jit, r0, r1)
__jit_inline void
mips_str_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _SD(r1, 0, r0);
}

#define jit_sti_l(i0, r0)		mips_sti_l(_jit, i0, r0)
__jit_inline void
mips_sti_l(jit_state_t _jit, void *i0, jit_gpr_t r0)
{
    long	ds = (long)i0;

    if (_s16P(ds))
	_SD(r0, ds & 0xffff, JIT_RZERO);
    else {
	jit_movi_i(JIT_RTEMP, ds);
	_SD(r0, 0, JIT_RTEMP);
    }
}

#define jit_stxr_l(r0, r1, r2)		mips_stxr_l(_jit, r0, r1, r2)
__jit_inline void
mips_stxr_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    jit_addr_i(JIT_RTEMP, r0, r1);
    _SD(r2, 0, JIT_RTEMP);
}

#define jit_stxi_l(i0, r0, r1)		mips_stxi_l(_jit, i0, r0, r1)
__jit_inline void
mips_stxi_l(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    if (_s16P(i0))
	_SD(r1, i0 & 0xffff, r0);
    else {
	jit_addi_i(JIT_RTEMP, r0, i0);
	_SD(r1, 0, JIT_RTEMP);
    }
}

#define jit_extr_c_l(r0, r1)		mips_extr_c_l(_jit, r0, r1)
__jit_inline void
mips_extr_c_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_c_i(r0, r1);
    _SLL(r0, r0, 0);
}

#define jit_extr_c_ul(r0, r1)		mips_extr_c_ul(_jit, r0, r1)
__jit_inline void
mips_extr_c_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_c_ui(r0, r1);
    _SLL(r0, r0, 0);
}

#define jit_extr_s_l(r0, r1)		mips_extr_s_l(_jit, r0, r1)
__jit_inline void
mips_extr_s_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_s_i(r0, r1);
    _SLL(r0, r0, 0);
}

#define jit_extr_s_ul(r0, r1)		mips_extr_s_ul(_jit, r0, r1)
#define jit_extr_us_ul(r0, r1)		mips_extr_us_ul(_jit, r0, r1)
__jit_inline void
mips_extr_s_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_extr_s_ui(r0, r1);
    _SLL(r0, r0, 0);
}

#define jit_extr_i_l(r0, r1)		mips_extr_i_l(_jit, r0, r1)
__jit_inline void
mips_extr_i_l(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _SLL(r0, r0, 0);
}

#define jit_extr_i_ul(r0, r1)		mips_extr_i_ul(_jit, r0, r1)
#define jit_extr_ui_ul(r0, r1)		mips_extr_ui_ul(_jit, r0, r1)
__jit_inline void
mips_extr_i_ul(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_movr_l(r0, r1);
    _DINS(r0, JIT_RZERO, 32, 32);
}

#define jit_prolog(n)			mips_prolog(_jit, n)
__jit_inline void
mips_prolog(jit_state_t _jit, int n)
{
    _jitl.framesize = 80;
    _jitl.nextarg_get = 0;

    jit_subi_l(JIT_SP, JIT_SP, 80);
    jit_stxi_l(72, JIT_SP, _RA);
    jit_stxi_l(64, JIT_SP, _FP);
    jit_stxi_l(56, JIT_SP, _S7);
    jit_stxi_l(48, JIT_SP, _S6);
    jit_stxi_l(40, JIT_SP, _S5);
    jit_stxi_l(32, JIT_SP, _S4);
    jit_stxi_l(34, JIT_SP, _S3);
    jit_stxi_l(16, JIT_SP, _S2);
    jit_stxi_l( 8, JIT_SP, _S1);
    jit_stxi_l( 0, JIT_SP, _S0);
    jit_movr_l(JIT_FP, JIT_SP);

    /* patch alloca and stack adjustment */
    _jitl.stack = (int *)_jit->x.pc;
    jit_movi_p(JIT_RTEMP, 0);
    jit_subr_l(JIT_SP, JIT_SP, JIT_RTEMP);
    _jitl.alloca_offset = _jitl.stack_offset = _jitl.stack_length = 0;
}

#define jit_prepare_i(count)		mips_prepare_i(_jit, count)
__jit_inline void
mips_prepare_i(jit_state_t _jit, int count)
{
    assert(count		>= 0 &&
	   _jitl.stack_offset	== 0 &&
	   _jitl.nextarg_put	== 0);

    _jitl.nextarg_put = count;
    if (_jitl.nextarg_put > JIT_A_NUM) {
	_jitl.stack_offset = (_jitl.nextarg_put - JIT_A_NUM) << 3;
	if (_jitl.stack_length < _jitl.stack_offset) {
	    _jitl.stack_length = _jitl.stack_offset;
	    mips_set_stack(_jit, (_jitl.alloca_offset +
				  _jitl.stack_length + 7) & ~7);
	}
    }
}

#define jit_pusharg_i(r0)		mips_pusharg_l(_jit, r0)
#define jit_pusharg_l(r0)		mips_pusharg_l(_jit, r0)
__jit_inline void
mips_pusharg_l(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.nextarg_put > 0);
    if (--_jitl.nextarg_put >= JIT_A_NUM) {
	_jitl.stack_offset -= sizeof(long);
	assert(_jitl.stack_offset >= 0);
	jit_stxi_l(_jitl.stack_offset, JIT_SP, r0);
    }
    else
	jit_movr_l(jit_a_order[_jitl.nextarg_put], r0);
}

#define jit_arg_l()			mips_arg_l(_jit)
#define jit_arg_i()			mips_arg_l(_jit)
#define jit_arg_c()			mips_arg_l(_jit)
#define jit_arg_uc()			mips_arg_l(_jit)
#define jit_arg_s()			mips_arg_l(_jit)
#define jit_arg_us()			mips_arg_l(_jit)
#define jit_arg_ui()			mips_arg_l(_jit)
#define jit_arg_ul()			mips_arg_l(_jit)
__jit_inline int
mips_arg_l(jit_state_t _jit)
{
    int		ofs;

    if (_jitl.nextarg_get < JIT_A_NUM) {
	ofs = _jitl.nextarg_get;
	++_jitl.nextarg_get;
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

#define jit_finishr(rs)			mips_finishr(_jit, rs)
__jit_inline void
mips_finishr(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.stack_offset == 0 && _jitl.nextarg_put == 0);
    jit_callr(r0);
}

#define jit_finish(i0)			mips_finish(_jit, i0)
__jit_inline jit_insn *
mips_finish(jit_state_t _jit, void *i0)
{
    assert(_jitl.stack_offset == 0 && _jitl.nextarg_put == 0);
    return (jit_calli(i0));
}

#define jit_retval_i(r0)		mips_retval_l(_jit, r0)
__jit_inline void
mips_retval_l(jit_state_t _jit, jit_gpr_t r0)
{
    jit_movr_l(r0, JIT_RET);
}

#define jit_ret()			mips_ret(_jit)
__jit_inline void
mips_ret(jit_state_t jit)
{
    jit_movr_l(JIT_SP, JIT_FP);
    jit_ldxi_l(_S0, JIT_SP,  0);
    jit_ldxi_l(_S1, JIT_SP,  8);
    jit_ldxi_l(_S2, JIT_SP, 16);
    jit_ldxi_l(_S3, JIT_SP, 24);
    jit_ldxi_l(_S4, JIT_SP, 32);
    jit_ldxi_l(_S5, JIT_SP, 40);
    jit_ldxi_l(_S6, JIT_SP, 48);
    jit_ldxi_l(_S7, JIT_SP, 56);
    jit_ldxi_l(_FP, JIT_SP, 64);
    jit_ldxi_l(_RA, JIT_SP, 72);
    _JR(_RA);
    /* restore sp in delay slot */
    jit_addi_l(JIT_SP, JIT_SP, 80);
}

#endif /* __lightning_core_h */
