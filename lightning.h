/******************************** -*- C -*- ****************************
 *
 *	lightning main include file
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000,2010 Free Software Foundation, Inc.
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
 *	Paolo Bonzini
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/



#ifndef __lightning_h
#define __lightning_h

#include <assert.h>
#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if __GNUC__
#  define __jit_constructor	__attribute__((constructor))
# define __jit_inline		inline static
#else
#  define __jit_constructor	/**/
# define __jit_inline		static
#endif

typedef signed char		_sc;
typedef unsigned char		_uc, jit_insn;
typedef unsigned short		_us;
typedef unsigned int		_ui;
typedef long			_sl;
typedef unsigned long		_ul;
typedef struct jit_local_state	jit_local_state;

#if defined(__i386__) || defined(__x86_64__)
struct {
    /* x87 present */
    _ui		fpu		: 1;
    /* cmpxchg8b instruction */
    _ui		cmpxchg8b	: 1;
    /* cmov and fcmov branchless conditional mov */
    _ui		cmov		: 1;
    /* mmx registers/instructions available */
    _ui		mmx		: 1;
    /* sse registers/instructions available */
    _ui		sse		: 1;
    /* sse2 registers/instructions available */
    _ui		sse2		: 1;
    /* sse3 instructions available */
    _ui		sse3		: 1;
    /* pcmulqdq instruction */
    _ui		pclmulqdq	: 1;
    /* ssse3 suplemental sse3 instructions available */
    _ui		ssse3		: 1;
    /* fused multiply/add using ymm state */
    _ui		fma		: 1;
    /* cmpxchg16b instruction */
    _ui		cmpxchg16b	: 1;
    /* sse4.1 instructions available */
    _ui		sse4_1		: 1;
    /* sse4.2 instructions available */
    _ui		sse4_2		: 1;
    /* movbe instruction available */
    _ui		movbe		: 1;
    /* popcnt instruction available */
    _ui		popcnt		: 1;
    /* aes instructions available */
    _ui		aes		: 1;
    /* avx instructions available */
    _ui		avx		: 1;
    /* lahf/sahf available in 64 bits mode */
    _ui		lahf		: 1;
} jit_cpu;

struct {
    /* round to nearest? */
    _ui		rnd_near	: 1;
    /* force push/pop for arguments and stack space float conversion?
     * this is useful in some very rare special cases, if generating
     * code that jumps from function to function, to make the stack
     * logic not dependent on patched value in jit_prolog */
    _ui		push_pop	: 1;
} jit_flags;
#elif !defined(__mips__) && !defined(__arm__)
#  define	jit_gpr_t	int
#  define	jit_fpr_t	int
#endif

#if defined(__i386__) && !defined(__x86_64__)
struct jit_local_state {
    int		 framesize;
    int		 float_offset;	/* %ebp offset for float conversion */
    int		 alloca_offset;	/* alloca offset from %ebp */
    int		 stack_length;	/* maximum number of arguments */
    int		 stack_offset;	/* argument offset */
    int		*stack;		/* patch address for immediate %esp adjust */
    jit_insn	*label;
};
#elif defined(__x86_64__)
struct jit_local_state {
    int		 long_jumps;
    int		 nextarg_getfp;
    int		 nextarg_putfp;
    int		 nextarg_geti;
    int		 nextarg_puti;
    int		 framesize;
    int		 fprssize;
    int		 float_offset;
    int		 alloca_offset;
    int		 stack_length;
    int		 stack_offset;
    int		*stack;
    jit_insn	*label;
};
#elif defined(__ppc__)
struct jit_local_state {
   int		 nextarg_puti;	/* number of integer args */
   int		 nextarg_putf;	/* number of float args   */
   int		 nextarg_putd;	/* number of double args  */
   int		 nextarg_geti;	/* Next r20-r25 reg. to be read */
   int		 nextarg_getd;	/* The FP args are picked up from FPR1 -> FPR10 */
   int		 nbArgs;	/* Number of arguments for the prolog */
   int		 frame_size, slack;
   jit_insn	*stwu;
};
#elif defined(__sparc__)
struct jit_local_state {
    int		 nextarg_put;	/* Next %o reg. to be written */
    int		 nextarg_get;	/* Next %i reg. to be read */
    jit_insn	*save;		/* Pointer to the `save' instruction */
    unsigned	 frame_size;	/* Current frame size as allocated by `save' */
    int		 alloca_offset; /* Current offset to the alloca'd memory
				 * (negative offset relative to %fp) */
    jit_insn	 delay;
};
#elif defined(__mips__)
struct jit_local_state {
    int		 framesize;
    int		 nextarg_int;
    int		 nextarg_put;
    int		 alloca_offset;
    int		 stack_length;
    int		 stack_offset;
    int		*stack;
#if !defined(__mips64__)
    int		*arguments[256];
    int		 types[8];
#endif
#ifdef JIT_NEED_PUSH_POP
    /* minor support for unsupported code but that exists in test cases... */
    int		 push[32];
    int		 pop;
#endif
};

struct {
    /* mips32 r2 instructions available? */
    _ui		mips2		: 1;

    /* assume memory doubles are 8 bytes aligned? */
    _ui		algndbl		: 1;

    _ui		movf		: 1;
    _ui		mul		: 1;
    _ui		mips64		: 1;
} jit_cpu;
#elif defined(__arm__)
struct jit_local_state {
    /* hack to only support one misalignment, so that it is not
     * required to patch arguments being constructed from right
     * to left */
    int		 reglist;
    int		 framesize;
    int		 nextarg_get;
    int		 nextarg_put;
    int		 alloca_offset;
    int		 stack_length;
    int		 stack_offset;
    void	*stack;
    int		*arguments[256];
    int		 types[8];
#ifdef JIT_NEED_PUSH_POP
    /* minor support for unsupported code but that exists in test cases... */
    int		 push[32];
    int		 pop;
#endif
};
struct {
    _ui		version		: 4;
    _ui		extend		: 1;
    _ui		thumb		: 2;
    _ui		softfp		: 1;
} jit_cpu;
#else
#  error GNU lightning does not support the current target
#endif

typedef struct jit_state {
    union {
	jit_insn	*pc;
	_uc		*uc_pc;
	_us		*us_pc;
	_ui		*ui_pc;
	_ul		*ul_pc;
    } x;
    struct jit_fp	*fp;
    jit_local_state	 jitl;
} jit_state_t[1];

#ifdef jit_init
jit_state_t		_jit = jit_init();
#else
jit_state_t		_jit;
#endif

#define _jitl		_jit->jitl

#include <lightning/asm-common.h>

#ifndef LIGHTNING_DEBUG
#include <lightning/asm.h>
#endif

#include <lightning/funcs.h>
#include <lightning/funcs-common.h>
#include <lightning/core.h>
#include <lightning/core-common.h>
#include <lightning/fp.h>
#include <lightning/fp-common.h>

#if defined(__cplusplus)
};
#endif

#endif /* __lightning_h */
