/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the arm
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2011 Free Software Foundation
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

#if defined(__linux__)
#  include <stdio.h>
#  include <string.h>
#endif

#include <unistd.h>
#include <sys/mman.h>

static void
jit_flush_code(void *start, void *end)
{
    mprotect(start, (char *)end - (char *)start,
	     PROT_READ | PROT_WRITE | PROT_EXEC);
}

#define jit_get_cpu			jit_get_cpu
__jit_constructor static void
jit_get_cpu(void)
{
#if defined(__linux__)
    FILE	*fp;
    char	*ptr;
    char	 buf[128];
    static int	 initialized;

    if (initialized)
	return;
    initialized = 1;
    if ((fp = fopen ("/proc/cpuinfo", "r")) == NULL)
	return;

    while (fgets(buf, sizeof (buf), fp)) {
	if (strncasecmp(buf, "CPU architecture:", 17) == 0) {
	    jit_cpu.armvn = strtol(buf + 17, &ptr, 10);
	    while (*ptr) {
		if (*ptr == 'T') {
		    ++ptr;
		    if (*ptr == '2') {
			jit_cpu.thumb = 2;
			++ptr;
		    }
		    else
			jit_cpu.thumb = 1;
		}
		else if (*ptr == 'E') {
		    jit_cpu.armve = 1;
		    ++ptr;
		}
		else
		    ++ptr;
	    }
	    break;
	}
    }
    fclose(fp);
#endif
}

#endif /* __lightning_funcs_h */
