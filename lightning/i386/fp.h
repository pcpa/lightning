/******************************** -*- C -*- ****************************
 *
 *	Floating-point support (i386)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2008 Free Software Foundation, Inc.
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



#ifndef __lightning_fp_i386_h
#define __lightning_fp_i386_h

#define jit_sse_p()			jit_cpu.sse
#define jit_sse4_1_p()			jit_cpu.sse4_1
#define jit_i686_p()			jit_cpu.cmov
#define jit_round_to_nearest_p()	jit_flags.rnd_near
#define jit_x87_reg_p(reg)		((reg) >= _ST0 && (reg) <= _ST7)

#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_X86_64 \
	: defined (__x86_64__)
#  define jit_sse_reg_p(reg)		((reg) >= _XMM0 && (reg) <= _XMM15)
#  include "fp-64.h"
#else
#  define jit_sse_reg_p(reg)		((reg) >= _XMM0 && (reg) <= _XMM7)
#  include "fp-32.h"
#endif

#endif /* __lightning_fp_i386_h */
