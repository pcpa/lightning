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

#ifndef __lightning_fp_h
#define __lightning_fp_h

#define jit_movi_d(f0, i0)		mips_movi_d(_jit, f0, i0)
__jit_inline void
mips_movi_d(jit_state_t _jit, jit_fpr_t f0, double i0)
{
    union {
	int	i[2];
	long	l;
	double	d;
    } data;

    data.d = i0;
    jit_movi_i(JIT_RTEMP, data.i[0]);
    _MTC1(JIT_RTEMP, f0);
    jit_movi_i(JIT_RTEMP, data.i[1]);
#  if 0 /* FIXME illegal instruction */
    _MTHC1(JIT_RTEMP, f0);
#  else
    _MTC1(JIT_RTEMP, (jit_fpr_t)(f0 + 1));
#  endif
}

#define jit_prepare_d(count)		mips_prepare_d(_jit, count)
__jit_inline void
mips_prepare_d(jit_state_t _jit, int count)
{
    assert(count >= 0);
    _jitl.stack_offset += count << 3;
    if (_jitl.stack_length < _jitl.stack_offset) {
	_jitl.stack_length = _jitl.stack_offset;
	mips_set_stack(_jit, (_jitl.alloca_offset +
			      _jitl.stack_length + 7) & ~7);
    }
}

#define jit_prepare_f(count)		mips_prepare_f(_jit, count)
__jit_inline void
mips_prepare_f(jit_state_t _jit, int count)
{
    assert(count >= 0);
    _jitl.stack_offset += count << 2;
    if (_jitl.stack_length < _jitl.stack_offset) {
	_jitl.stack_length = _jitl.stack_offset;
	mips_set_stack(_jit, (_jitl.alloca_offset +
			      _jitl.stack_length + 7) & ~7);
    }
}

#define jit_arg_d()			mips_arg_d(_jit)
__jit_inline int
mips_arg_d(jit_state_t _jit)
{
    int		ofs;
    int		reg;

    if (_jitl.framesize & 7)
	_jitl.framesize += 4;
    reg = (_jitl.framesize - JIT_FRAMESIZE) >> 3;
    if (reg < JIT_DA_NUM)
	ofs = reg;
    else
	ofs = _jitl.framesize;
    _jitl.framesize += sizeof(double);

    return (ofs);
}

#define jit_arg_f()			mips_arg_f(_jit)
__jit_inline int
mips_arg_f(jit_state_t _jit)
{
    int		ofs;
    int		regf;
    int		regd;

    /* first two floats use double registers */
    regf = (_jitl.framesize - JIT_FRAMESIZE) >> 2;
    regd = (_jitl.framesize - JIT_FRAMESIZE) >> 3;
    if (regd < JIT_DA_NUM)
	ofs = regd;
    else if (regf < JIT_FA_NUM)
	ofs = regf;
    else
	ofs = _jitl.framesize;
    _jitl.framesize += sizeof(float);

    return (ofs);
}

#define jit_getarg_d(f0, ofs)		mips_getarg_d(_jit, f0, ofs)
__jit_inline void
mips_getarg_d(jit_state_t _jit, jit_fpr_t f0, int ofs)
{
    if (ofs < JIT_DA_NUM) {
	_MTC1((jit_gpr_t)jit_da_order[ofs], f0);
	_MTC1((jit_gpr_t)(jit_da_order[ofs] + 1), (jit_fpr_t)(f0 + 1));
    }
    else {
	jit_ldxi_f(f0, JIT_FP, ofs);
	jit_ldxi_f((jit_fpr_t)(f0 + 1), JIT_FP, ofs + 4);
    }
}

#define jit_getarg_f(f0, ofs)		mips_getarg_f(_jit, f0, ofs)
__jit_inline void
mips_getarg_f(jit_state_t _jit, jit_fpr_t f0, int ofs)
{
    if (ofs < JIT_DA_NUM)
	_MTC1((jit_gpr_t)jit_fa_order[ofs], f0);
    else if (ofs < JIT_FA_NUM)
	jit_movr_f(f0, (jit_fpr_t)jit_fa_order[ofs]);
    else
	jit_ldxi_f(f0, JIT_FP, ofs);
}

#define jit_pusharg_d(f0)		mips_pusharg_d(_jit, f0)
__jit_inline void
mips_pusharg_d(jit_state_t _jit, jit_fpr_t f0)
{
    int		ofs = _jitl.nextarg_put++;

    assert(ofs < 256);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] |= 1 << (ofs & 31);
    _jitl.stack_offset -= sizeof(double);
    jit_stxi_f(_jitl.stack_offset, JIT_SP, f0);
    jit_stxi_f(_jitl.stack_offset + 4, JIT_SP, (jit_fpr_t)(f0 + 1));
}

#define jit_pusharg_f(f0)		mips_pusharg_f(_jit, f0)
__jit_inline void
mips_pusharg_f(jit_state_t _jit, jit_fpr_t f0)
{
    int		ofs = _jitl.nextarg_put++;

    assert(ofs < 256);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] &= ~(1 << (ofs & 31));
    _jitl.stack_offset -= sizeof(float);
    jit_stxi_f(_jitl.stack_offset, JIT_SP, f0);
}

#endif /* __lightning_fp_h */
