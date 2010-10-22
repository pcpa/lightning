/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the mips
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2010 Free Software Foundation
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

#ifndef __lightning_funcs_h
#define __lightning_funcs_h

#include <unistd.h>
#include <sys/mman.h>

#if defined(__linux__)
#  include <sys/cachectl.h>
#endif

static void
jit_flush_code(void *start, void *end)
{
#if 0	/* FIXME not compiled ...
	 * and probably should use the cache instruction */
    __asm__ __volatile__ ("lw $t0,%0\n\tsynci 0($t0)"
			  : : "g" (start) : "t0");
#endif
    mprotect(start, (char*)end - (char*)start,
	     PROT_READ | PROT_WRITE | PROT_EXEC);
#if defined(__linux__)
    _flush_cache(start, (long)end - (long)start, ICACHE);
#endif
}

#define jit_get_cpu			jit_get_cpu
__jit_constructor static void
jit_get_cpu(void)
{
}

#endif /* __lightning_funcs_h */
