/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (i386 version)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2006 Free Software Foundation, Inc.
 * Written by Paolo Bonzini and Matthew Flatt.
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
 ***********************************************************************/



#ifndef __lightning_core_h
#define __lightning_core_h

#define JIT_CAN_16			1
#define JIT_AP				_EBP

#define JIT_R_NUM			3
#define JIT_R(i)			(_EAX + (i))
#define JIT_V_NUM			3
#define JIT_V(i)			((i) == 0 ? _EBX : _ESI + (i) - 1)

#define jit_movi_i(rd, i0)		jit_movi_i(rd, i0)
__jit_inline void
jit_movi_i(int rd, int i0)
{
    if (i0)
	MOVLir(i0, rd);
    else
	XORLrr(rd, rd);
}

#define jit_movi_p(rd, i0)		jit_movi_p(rd, i0)
__jit_inline jit_insn *
jit_movi_p(int rd, void *i0)
{
    MOVLir((long)i0, rd);
    return (_jit.x.pc);
}

#define jit_patch_at(jump, label)	jit_patch_at(jump, label)
__jit_inline void
jit_patch_at(jit_insn *jump, jit_insn *label)
{
    jit_patch_rel_int_at(jump, label);
}

#define jit_prolog(n)			jit_prolog(n)
__jit_inline void
jit_prolog(int n)
{
    _jitl.framesize = 20;
    _jitl.alloca_offset = _jitl.alloca_slack = 0;
    PUSHLr(_EBX);
    PUSHLr(_ESI);
    PUSHLr(_EDI);
    PUSHLr(_EBP);
    MOVLrr(_ESP, _EBP);
    SUBLir(12, JIT_SP);
}

#define jit_ret				jit_ret
__jit_inline void
jit_ret(void)
{
    LEAVE_();
    POPLr(_EDI);
    POPLr(_ESI);
    POPLr(_EBX);
    RET_();
}

#define jit_allocai(n)			jit_allocai(n)
__jit_inline int
jit_allocai(int n)
{
    int		s = (_jitl.alloca_slack - n) & 15;
    if (n >= _jitl.alloca_slack) {
	_jitl.alloca_slack += n + s;
	if (n + s == sizeof(int))
	    PUSHLr(_EAX);
	else
	    SUBLir(n + s, _ESP);
    }
    _jitl.alloca_slack -= n;
    return (_jitl.alloca_offset -= n);
}

#define jit_prepare_i(ni)		jit_prepare_i(ni)
__jit_inline void
jit_prepare_i(int ni)
{
    _jitl.argssize = (ni + 3) & ~0x3;
    if (ni & 0x3)
	SUBLir(4 * (_jitl.argssize - ni), JIT_SP);
}

#define jit_calli(address)		jit_calli(address)
__jit_inline jit_insn *
jit_calli(void *address)
{
    CALLm((_ul)address);
    return (_jitl.label = _jit.x.pc);
}

#define jit_callr(rs)			jit_callr(rs)
__jit_inline void
jit_callr(int rs)
{
    CALLsr(rs);
}

#define jit_patch_calli(call, label)	jit_patch_calli(call, label)
__jit_inline void
jit_patch_calli(jit_insn *call, jit_insn *label)
{
    jit_patch_at(call, label);
}

#define jit_finish(label)		jit_finish(label)
__jit_inline jit_insn *
jit_finish(jit_insn *label)
{
    jit_calli(label);
    ADDLir(sizeof(long) * _jitl.argssize, JIT_SP);
    _jitl.argssize = 0;

    return (_jitl.label);
}

#define jit_finishr(rs)			jit_finishr(rs)
__jit_inline void
jit_finishr(int rs)
{
    jit_callr(rs);
    ADDLir(sizeof(long) * _jitl.argssize, JIT_SP);
    _jitl.argssize = 0;
}

#define jit_pusharg_i(rs)		jit_pusharg_i(rs)
__jit_inline void
jit_pusharg_i(int rs)
{
    PUSHLr(rs);
}

#define jit_arg_i			jit_arg_i
#define jit_arg_c()			jit_arg_i()
#define jit_arg_uc()			jit_arg_i()
#define jit_arg_s()			jit_arg_i()
#define jit_arg_us()			jit_arg_i()
#define jit_arg_ui()			jit_arg_i()
#define jit_arg_l()			jit_arg_i()
#define jit_arg_ul()			jit_arg_i()
#define jit_arg_p()			jit_arg_i()
__jit_inline int
jit_arg_i(void)
{
    int		ofs;

    ofs = _jitl.framesize;
    _jitl.framesize += sizeof(int);
    return (ofs);
}

/* Memory */
#define jit_replace(s, rep, op)                         \
        (jit_pushr_i(rep),                              \
         MOVLrr((s), (rep)),                            \
         op, jit_popr_i(rep))

#define jit_movbrm(rs, dd, db, di, ds)                                          \
        (jit_check8(rs)                                                         \
                ? MOVBrm(jit_reg8(rs), dd, db, di, ds)                          \
                : jit_replace(rs,                                               \
                              ((dd != _EAX && db != _EAX && di != _EAX) ? _EAX :              \
                              ((dd != _ECX && db != _ECX && di != _ECX) ? _ECX : _EDX)),      \
                              MOVBrm(((dd != _EAX && db != _EAX && di != _EAX) ? _AL :        \
                                     ((dd != _ECX && db != _ECX && di != _ECX) ? _CL : _DL)), \
                                     dd, db, di, ds)))

#define jit_ldr_c(d, rs)                MOVSBLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_c(d, s1, s2)           MOVSBLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldr_s(d, rs)                MOVSWLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_s(d, s1, s2)           MOVSWLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldi_c(d, is)                MOVSBLmr((is), 0,    0,    0, (d))
#define jit_ldxi_c(d, rs, is)           MOVSBLmr((is), (rs), 0,    0, (d))

#define jit_ldi_uc(d, is)               MOVZBLmr((is), 0,    0,    0, (d))
#define jit_ldxi_uc(d, rs, is)          MOVZBLmr((is), (rs), 0,    0, (d))

#define jit_sti_c(id, rs)               jit_movbrm((rs), (id), 0,    0,    0)
#define jit_stxi_c(id, rd, rs)          jit_movbrm((rs), (id), (rd), 0,    0)

#define jit_ldi_s(d, is)                MOVSWLmr((is), 0,    0,    0, (d))
#define jit_ldxi_s(d, rs, is)           MOVSWLmr((is), (rs), 0,    0, (d))

#define jit_ldi_us(d, is)               MOVZWLmr((is), 0,    0,    0,  (d))
#define jit_ldxi_us(d, rs, is)          MOVZWLmr((is), (rs), 0,    0,  (d))

#define jit_sti_s(id, rs)               MOVWrm(jit_reg16(rs), (id), 0,    0,    0)
#define jit_stxi_s(id, rd, rs)          MOVWrm(jit_reg16(rs), (id), (rd), 0,    0)

#define jit_ldi_i(d, is)                MOVLmr((is), 0,    0,    0,  (d))
#define jit_ldxi_i(d, rs, is)           MOVLmr((is), (rs), 0,    0,  (d))

#define jit_ldr_i(d, rs)                MOVLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_i(d, s1, s2)           MOVLmr(0,    (s1), (s2), 1,  (d))
							    
#define jit_sti_i(id, rs)               MOVLrm((rs), (id), 0,    0,    0)
#define jit_stxi_i(id, rd, rs)          MOVLrm((rs), (id), (rd), 0,    0)

#endif /* __lightning_core_h */
