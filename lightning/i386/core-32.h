/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (i386 version)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
 *
 * This file is part of GNU lightning.
 *
 * GNU lightning is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1, or (at your option)
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

#define JIT_CAN_16 1
#define JIT_AP			_EBP

#include "core-i386.h"

struct jit_local_state {
  int	framesize;
  int	argssize;
  int	alloca_offset;
  int	alloca_slack;
};

#define jit_base_prolog() (PUSHLr(_EBP), MOVLrr(_ESP, _EBP), PUSHLr(_EBX), PUSHLr(_ESI), PUSHLr(_EDI))
#define jit_prolog(n) (_jitl.framesize = 8, _jitl.alloca_offset = -12, jit_base_prolog())

/* Used internally.  SLACK is used by the Darwin ABI which keeps the stack
   aligned to 16-bytes.  */

#define jit_allocai_internal(amount, slack)				  \
  (((amount) < _jitl.alloca_slack					  \
    ? 0									  \
    : (_jitl.alloca_slack += (amount) + (slack),			  \
       ((amount) + (slack) == sizeof (int)				  \
        ? PUSHLr(_EAX)							  \
        : SUBLir((amount) + (slack), _ESP)))),				  \
   _jitl.alloca_slack -= (amount),					  \
   _jitl.alloca_offset -= (amount))
   
/* Stack */
#define jit_pushr_i(rs)		PUSHLr(rs)
#define jit_popr_i(rs)		POPLr(rs)

/* The += in argssize allows for stack pollution */

#ifdef __APPLE__
/* Stack must stay 16-byte aligned: */
# define jit_prepare_i(ni)	(((ni & 0x3) \
                                  ? SUBLir(4 * ((((ni) + 3) & ~(0x3)) - (ni)), JIT_SP) \
                                  : (void)0), \
                                 _jitl.argssize += (((ni) + 3) & ~(0x3)))

#define jit_allocai(n)						\
  jit_allocai_internal ((n), (_jitl.alloca_slack - (n)) & 15)

#else
# define jit_prepare_i(ni)	(_jitl.argssize += (ni))

#define jit_allocai(n)						\
  jit_allocai_internal ((n), 0)
#endif

#define jit_pusharg_i(rs)	PUSHLr(rs)
#define jit_finish(sub)        ((void)jit_calli((sub)), ADDLir(sizeof(long) * _jitl.argssize, JIT_SP), _jitl.argssize = 0)
#define jit_finishr(reg)	(jit_callr((reg)), ADDLir(sizeof(long) * _jitl.argssize, JIT_SP), _jitl.argssize = 0)

#define	jit_arg_c()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_uc()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_s()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_us()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_i()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_ui()		((_jitl.framesize += sizeof(int)) - sizeof(int))
#define	jit_arg_l()		((_jitl.framesize += sizeof(long)) - sizeof(long))
#define	jit_arg_ul()		((_jitl.framesize += sizeof(long)) - sizeof(long))
#define	jit_arg_p()		((_jitl.framesize += sizeof(long)) - sizeof(long))

#define jit_patch_long_at(jump_pc,v)  (*_PSL((jump_pc) - sizeof(long)) = _jit_SL((jit_insn *)(v) - (jump_pc)))
#define jit_patch_at(jump_pc,v)  jit_patch_long_at(jump_pc, v)
#define jit_ret()		(POPLr(_EDI), POPLr(_ESI), POPLr(_EBX), (_jitl.alloca_offset < -12 ? LEAVE_() : POPLr(_EBP)), RET_())

#endif /* __lightning_core_h */

