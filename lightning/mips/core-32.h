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

#define jit_ldi_ui(r0, i0)		jit_ldi_i(r0, i0)
#define jit_ldi_l(r0, i0)		jit_ldi_i(r0, i0)
#define jit_ldr_ui(r0, r1)		jit_ldr_i(r0, r1)
#define jit_ldr_l(r0, r1)		jit_ldr_i(r0, r1)
#define jit_sti_l(r0, i0)		jit_sti_i(r0, i0)
#define jit_str_l(r0, r1)		jit_str_i(r0, r1)
#define jit_arg_ui()			mips_arg_i(_jit)
#define jit_arg_l()			mips_arg_i(_jit)
#define jit_arg_ul()			mips_arg_i(_jit)
#define jit_arg_p()			mips_arg_i(_jit)
#define jit_getarg_ui(r0, ofs)		jit_getarg_i(r0, ofs)
#define jit_getarg_l(r0, ofs)		jit_getarg_i(r0, ofs)
#define jit_getarg_ul(r0, ofs)		jit_getarg_i(r0, ofs)
#define jit_getarg_p(r0, ofs)		jit_getarg_i(r0, ofs)

#define jit_jmpi(i0)			mips_jmpi(_jit, i0)
__jit_inline jit_insn *
mips_jmpi(jit_state_t _jit, void *i0)
{
    long	pc = (long)_jit->x.pc;
    long	lb = (long)i0;

    /* FIXME how to patch efficiently? */

    /* FIXME return an address that can be patched so, should always
     * jump to register, to not be limited to same 256Mb segment */

    if ((pc & 0xf0000000) == (lb & 0xf0000000))
	_J(i0);
    else {
	jit_movi_i(JIT_RTEMP, lb);
	mips_r_it(_jit, JIT_RTEMP, 0, MIPS_JR);
    }
    jit_nop(1);
    return (_jit->x.pc);
}

#define jit_prolog(n)			mips_prolog(_jit, n)
__jit_inline void
mips_prolog(jit_state_t _jit, int n)
{
    _jitl.framesize = 40;
    _jitl.nextarg_geti = 0;

    jit_subi_i(JIT_SP, JIT_SP, 40);
    jit_stxi_i(36, JIT_SP, _RA);
    jit_stxi_i(32, JIT_SP, _FP);
    jit_stxi_i(28, JIT_SP, _S7);
    jit_stxi_i(24, JIT_SP, _S6);
    jit_stxi_i(20, JIT_SP, _S5);
    jit_stxi_i(16, JIT_SP, _S4);
    jit_stxi_i(12, JIT_SP, _S3);
    jit_stxi_i( 8, JIT_SP, _S2);
    jit_stxi_i( 4, JIT_SP, _S1);
    jit_stxi_i( 0, JIT_SP, _S0);
    jit_movr_i(JIT_FP, JIT_SP);

    /* patch alloca and stack adjustment */
    jit_subi_i(JIT_SP, JIT_SP, 0);
    /* FIXME should not limit to 15 bits */
    _jitl.stack = ((short *)_jit->x.pc) - 2;
    _jitl.alloca_offset = _jitl.stack_offset = _jitl.stack_length = 0;
}

#define jit_prepare_i(count)		mips_prepare_i(_jit, count)
__jit_inline void
mips_prepare_i(jit_state_t _jit, int count)
{
    assert(count		>= 0 &&
	   _jitl.stack_offset	== 0 &&
	   _jitl.nextarg_puti	== 0);

    _jitl.nextarg_puti = count;
    if (_jitl.nextarg_puti > JIT_A_NUM) {
	_jitl.stack_offset = (_jitl.nextarg_puti - JIT_A_NUM) << 2;
	if (_jitl.stack_length < _jitl.stack_offset) {
	    _jitl.stack_length = _jitl.stack_offset;
	    *_jitl.stack = (_jitl.alloca_offset +
			    _jitl.stack_length + 7) & ~7;
	}
    }
}

#define jit_pusharg_i(r0)		mips_pusharg_i(_jit, r0)
__jit_inline void
mips_pusharg_i(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.nextarg_puti > 0);
    if (--_jitl.nextarg_puti >= JIT_A_NUM) {
	_jitl.stack_offset -= sizeof(int);
	assert(_jitl.stack_offset >= 0);
	jit_stxi_i(_jitl.stack_offset, JIT_SP, r0);
    }
    else
	jit_movr_i(jit_a_order[_jitl.nextarg_puti], r0);
}

#define jit_arg_i()			mips_arg_i(_jit)
#define jit_arg_c()			mips_arg_i(_jit)
#define jit_arg_uc()			mips_arg_i(_jit)
#define jit_arg_s()			mips_arg_i(_jit)
#define jit_arg_us()			mips_arg_i(_jit)
#define jit_arg_ui()			mips_arg_i(_jit)
__jit_inline int
mips_arg_i(jit_state_t _jit)
{
    int		ofs;

    if (_jitl.nextarg_geti < JIT_A_NUM) {
	ofs = _jitl.nextarg_geti;
	++_jitl.nextarg_geti;
    }
    else {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(int);
    }

    return (ofs);
}

#define jit_calli(i0)			mips_calli(_jit, i0)
__jit_inline jit_insn *
mips_calli(jit_state_t _jit, void *i0)
{
#if 0
    /* FIXME still usable to call jit functions that are not really
     * position independent code */

    long	pc = (long)_jit->x.pc;
    long	lb = (long)i0;

    /* FIXME return an address that can be patched so, should always
     * call register to not be limited to same 256Mb segment */
    if ((pc & 0xf0000000) == (lb & 0xf0000000))
	_JAL(i0);
    else {
	jit_movi_i(JIT_RTEMP, lb);
	mips_r_rit(_jit, JIT_RTEMP, _RA, 0, MIPS_JALR);
    }
    jit_nop(1);
#else
    /* if calling a pic function, _T9 *must* hold the function pointer */

    jit_movi_i(_T9, (long)i0);
    jit_callr(_T9);
#endif
    return (_jit->x.pc);
}

#define jit_finish(i0)			mips_finish(_jit, i0)
__jit_inline jit_insn *
mips_finish(jit_state_t _jit, void *i0)
{
    assert(_jitl.stack_offset	== 0 &&
	   _jitl.nextarg_puti	== 0);
    jit_calli(i0);
    /* FIXME return patchable address */
    return (_jit->x.pc);
}

#define jit_retval_i(r0)		mips_retval_i(_jit, r0)
__jit_inline void
mips_retval_i(jit_state_t _jit, jit_gpr_t r0)
{
    jit_movr_i(r0, JIT_RET);
}

#define jit_ret()			mips_ret(_jit)
__jit_inline void
mips_ret(jit_state_t jit)
{
    jit_movr_i(JIT_SP, JIT_FP);
    jit_ldxi_i(_S0, JIT_SP,  0);
    jit_ldxi_i(_S1, JIT_SP,  4);
    jit_ldxi_i(_S2, JIT_SP,  8);
    jit_ldxi_i(_S3, JIT_SP, 12);
    jit_ldxi_i(_S4, JIT_SP, 16);
    jit_ldxi_i(_S5, JIT_SP, 20);
    jit_ldxi_i(_S6, JIT_SP, 24);
    jit_ldxi_i(_S7, JIT_SP, 28);
    jit_ldxi_i(_FP, JIT_SP, 32);
    jit_ldxi_i(_RA, JIT_SP, 36);
    jit_jmpr(_RA);
    /* restore sp in delay slot */
    jit_addi_i(JIT_SP, JIT_SP, 40);
}

#define jit_allocai(n)			mips_allocai(_jit, n)
__jit_inline int
mips_allocai(jit_state_t _jit, int length)
{
    assert(length >= 0);
    _jitl.alloca_offset += length;
    if (_jitl.alloca_offset + _jitl.stack_length > *_jitl.stack)
	*_jitl.stack += (length + 8) & ~7;
    return (-_jitl.alloca_offset);
}

#endif /* __lightning_core_h */
