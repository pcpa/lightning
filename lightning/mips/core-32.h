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

#define jit_movi_p(r0, i0)		mips_movi_p(_jit, r0, i0)
__jit_inline jit_insn *
mips_movi_p(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_insn	*l;
    unsigned	im = (unsigned)i0;

    l = _jit->x.pc;
    _LUI(r0, im >> 16);
    _ORI(r0, r0, im & 0xffff);
    return (l);
}

#define jit_patch_movi(i0, i1)		mips_patch_movi(_jit, i0, i1)
__jit_inline void
mips_patch_movi(jit_state_t _jit, jit_insn *i0, void *i1)
{
    mips_code_t		c;
    union {
	char		*c;
	int		*i;
	short		*s;
	void		*v;
    } u;
    unsigned	 im = (unsigned)i1;

    u.v = i0;

    c.op = u.i[0];
    assert(c.hc.b == MIPS_LUI);
    c.is.b = im >> 16;
    u.i[0] = c.op;

    c.op = u.i[1];
    assert(c.hc.b == MIPS_ORI);
    c.is.b = im & 0xffff;
    u.i[1] = c.op;
}

#define jit_patch_at(jump, label)	mips_patch_at(_jit, jump, label)
__jit_inline void
mips_patch_at(jit_state_t _jit, jit_insn *jump, jit_insn *label)
{
    mips_code_t		c;
    long		d;
    union {
	char		*c;
	int		*i;
	short		*s;
	void		*v;
    } u;

    u.v = jump;
    c.op = u.i[0];
    switch (c.hc.b) {
	/* 16 bit immediate opcodes */
	case MIPS_REGIMM:
	    switch (c.rt.b) {
		case MIPS_BLTZ:		case MIPS_BLTZL:
		case MIPS_BLTZAL:	case MIPS_BLTZALL:
		case MIPS_BGEZ:		case MIPS_BGEZAL:
		case MIPS_BGEZALL:	case MIPS_BGEZL:
		case MIPS_TEQI:		case MIPS_TGEI:
		case MIPS_TGEIU:	case MIPS_TLTI:
		case MIPS_TLTIU:	case MIPS_TNEI:
		    d = (((long)label - (long)jump) >> 2) - 1;
		    c.is.b = _s16(d);
		    u.i[0] = c.op;
		    break;
		default:
		    assert(!"unhandled branch opcode");
		    break;
	    }
	    break;

	case MIPS_COP1:			case MIPS_COP2:
	    assert(c.rs.b == MIPS_BC);
	    switch (c.rt.b) {
		case MIPS_BCF:		case MIPS_BCFL:
		case MIPS_BCT:		case MIPS_BCTL:
		    d = (((long)label - (long)jump) >> 2) - 1;
		    c.is.b = _s16(d);
		    u.i[0] = c.op;
		    break;
		default:
		    assert(!"unhandled branch opcode");
		    break;
	    }
	    break;

	case MIPS_BLEZ:			case MIPS_BLEZL:
	case MIPS_BEQ:			case MIPS_BEQL:
	case MIPS_BGTZ:			case MIPS_BGTZL:
	case MIPS_BNE:			case MIPS_BNEL:
	    d = (((long)label - (long)jump) >> 2) - 1;
	    c.is.b = _s16(d);
	    u.i[0] = c.op;
	    break;

	case MIPS_LUI:
	    /* move and jump to register or wrong, but works,
	     * call to jit_patch instead of jit_patch_movi */
	    mips_patch_movi(_jit, jump, label);
	    break;

	case MIPS_J:			case MIPS_JAL:
	case MIPS_JALX:
	    d = (long)label;
	    assert((((long)jump + sizeof(int)) & 0xf0000000) ==
		   (d & 0xf0000000));
	    c.ii.b = (d & ~0xf0000000) >> 2;
	    u.i[0] = c.op;
	    break;

	default:
	    assert(!"unhandled branch opcode");
	    break;
    }
}

#define jit_jmpi(i0)			mips_jmpi(_jit, i0)
__jit_inline jit_insn *
mips_jmpi(jit_state_t _jit, void *i0)
{
    jit_insn	*l;
    long	 pc = (long)_jit->x.pc + sizeof(int);
    long	 lb = (long)i0;
    l = _jit->x.pc;
    if ((pc & 0xf0000000) == (lb & 0xf0000000)) {
	_J((lb & ~0xf0000000) >> 2);
	jit_nop(1);
    }
    else {
	jit_movi_p(JIT_RTEMP, i0);
	jit_jmpr(JIT_RTEMP);
    }
    return (l);
}

#define jit_prolog(n)			mips_prolog(_jit, n)
__jit_inline void
mips_prolog(jit_state_t _jit, int n)
{
    _jitl.framesize = 40;
    _jitl.nextarg_get = 0;

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
    _jitl.stack = (int *)_jit->x.pc;
    jit_movi_p(JIT_RTEMP, 0);
    jit_subr_i(JIT_SP, JIT_SP, JIT_RTEMP);
    _jitl.alloca_offset = _jitl.stack_offset = _jitl.stack_length = 0;
}

#define jit_prepare_i(count)		mips_prepare_i(_jit, count)
__jit_inline void
mips_prepare_i(jit_state_t _jit, int count)
{
    assert(count		>= 0 &&
	   _jitl.stack_offset	== 0 &&
	   _jitl.nextarg_put	== 0);
    _jitl.stack_offset = count << 2;
    if (_jitl.stack_length < _jitl.stack_offset) {
	_jitl.stack_length = _jitl.stack_offset;
	mips_set_stack(_jit, (_jitl.alloca_offset +
			      _jitl.stack_length + 7) & ~7);
    }
}

#define jit_pusharg_i(r0)		mips_pusharg_i(_jit, r0)
__jit_inline void
mips_pusharg_i(jit_state_t _jit, jit_gpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;

    assert(ofs < 256);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] &= ~(1 << (ofs & 31));
    _jitl.stack_offset -= sizeof(int);
    ofs = _jitl.stack_offset >> 2;
    if (ofs >= JIT_A_NUM)
	jit_stxi_i(_jitl.stack_offset, JIT_SP, r0);
    else
	jit_movr_i(jit_a_order[ofs], r0);
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

    if (_jitl.nextarg_get < JIT_A_NUM)
	ofs = _jitl.nextarg_get;
    else
	ofs = _jitl.framesize;
    _jitl.nextarg_get += 1;
    _jitl.framesize += sizeof(int);

    return (ofs);
}

#define jit_calli(i0)			mips_calli(_jit, i0)
__jit_inline jit_insn *
mips_calli(jit_state_t _jit, void *i0)
{
    jit_insn	*l;
#if 0
    /* FIXME still usable to call jit functions that are not really
     * position independent code */
    long	pc = (long)_jit->x.pc;
    long	lb = (long)i0;

    l = _jit->x.pc;
    /* FIXME return an address that can be patched so, should always
     * call register to not be limited to same 256Mb segment */
    if ((pc & 0xf0000000) == (lb & 0xf0000000))
	_JAL(((long)i0 & ~0xf0000000) >> 2);
    else {
	jit_movi_p(JIT_RTEMP, lb);
	_JALR(JIT_RTEMP);
    }
    jit_nop(1);
#else
    /* if calling a pic function, _T9 *must* hold the function pointer */

    l = _jit->x.pc;
    jit_movi_p(_T9, i0);
    _JALR(_T9);
    jit_nop(1);
#endif

    return (l);
}

#define jit_patch_calli(call, label)	mips_patch_calli(_jit, call, label)
__jit_inline void
mips_patch_calli(jit_state_t _jit, jit_insn *call, jit_insn *label)
{
    jit_patch_at(call, label);
}

__jit_inline void
mips_patch_arguments(jit_state_t _jit)
{
    mips_code_t	 c;
    int		 index;
    int		 offset;

    for (index = 0, offset = _jitl.nextarg_put >> 5; offset >= 0; offset--)
	if (_jitl.types[offset])
	    index++;
    /* no double arguments that may require patching */
    if (index == 0)
	return;

    for (index = _jitl.nextarg_put - 1, offset = 0; index >= 0; index--) {
	if (offset >= 16) {
	    c.op = *_jitl.arguments[index];
	    switch (c.hc.b) {
		case MIPS_SW:		/* sp[off] = int32 */
		case MIPS_SWC1:		/* sp[off] = float32 */
		case MIPS_SDC1:		/* sp[off] = float64 */
		    break;
		case MIPS_SPECIAL:
		    /* move a3,t(n) that now needs to go on stack
		     * due to register "alignment" */
		    assert(c.tc.b == MIPS_OR);
		    c.hc.b = MIPS_SW;
		    c.rt.b = c.rs.b;
		    c.rs.b = JIT_SP;
		    break;
		case MIPS_COP1:
		    /* swc1 a3,f(n) that now needs to go on stack
		     * due to register "alignment" */
		    assert(c.rs.b == MIPS_fmt_S);
		    c.hc.b = MIPS_SWC1;
		    c.rs.b = JIT_SP;
		    c.rt.b = c.fd.b;
		    /* convert to noop the extra store to %f15;
		     * that is generated to work correctly if
		     * calling a prototyped function */
		    _jitl.arguments[index][1] = 0;
		    break;
		default:
		    assert(!"unhandled argument opcode");
		    break;
	    }
	    c.is.b = offset;
	    *_jitl.arguments[index] = c.op;
	}
	if (_jitl.types[index >> 5]) {
	    if (offset & 7)
		offset += sizeof(int);
	    offset += sizeof(double);
	}
	else
	    offset += sizeof(int);
    }
    if (_jitl.stack_length < offset) {
	_jitl.stack_length = offset;
	mips_set_stack(_jit, (_jitl.alloca_offset +
			      _jitl.stack_length + 7) & ~7);
    }
}

#define jit_finishr(rs)			mips_finishr(_jit, rs)
__jit_inline void
mips_finishr(jit_state_t _jit, jit_gpr_t r0)
{
    assert(_jitl.stack_offset == 0);
    mips_patch_arguments(_jit);
    _jitl.nextarg_put = 0;
    jit_callr(r0);
}

#define jit_finish(i0)			mips_finish(_jit, i0)
__jit_inline jit_insn *
mips_finish(jit_state_t _jit, void *i0)
{
    assert(_jitl.stack_offset == 0);
    mips_patch_arguments(_jit);
    _jitl.nextarg_put = 0;
    return (jit_calli(i0));
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
    _JR(_RA);
    /* restore sp in delay slot */
    jit_addi_i(JIT_SP, JIT_SP, 40);
}

#endif /* __lightning_core_h */
