/******************************** -*- C -*- ****************************
 *
 *	lightning main include file
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000 Free Software Foundation, Inc.
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



#ifndef __lightning_h
#define __lightning_h

#include <sys/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef signed char		_sc;
typedef unsigned char		_uc, jit_insn;
typedef unsigned short		_us;
typedef unsigned int		_ui;
typedef long			_sl;
typedef unsigned long		_ul;
typedef struct jit_local_state	jit_local_state;

#if defined(__i386__) && !defined(__x86_64__)
struct jit_local_state {
    int		 framesize;
    int		 argssize;
    int		 alloca_offset;
    int		 alloca_slack;
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
    int		 argssize;
    int		 fprssize;
    int		 alloca_offset;
    int		 alloca_slack;
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
#else
#  error GNU lightning does not support the current target
#endif

typedef struct {
    union {
	jit_insn	*pc;
	_uc		*uc_pc;
	_us		*us_pc;
	_ui		*ui_pc;
	_ul		*ul_pc;
    } x;
    struct jit_fp	*fp;
    jit_local_state	 jitl;
} jit_state;

#ifdef jit_init
jit_state		_jit = jit_init();
#else
jit_state		_jit;
#endif

#define _jitl		_jit.jitl
#define __jit_inline	inline static

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
