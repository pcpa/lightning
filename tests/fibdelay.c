/******************************** -*- C -*- ****************************
 *
 *	Sample example of branches
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/mman.h>
#include "lightning.h"

jit_insn *codeBuffer;

typedef int (*pifi)(int);	/* Pointer to Int Function of Int */

int main()
{
  pifi      nfibs;
  int	    in;				/* offset of the argument */
  jit_insn  *ref;			/* to patch the forward reference */
  jit_insn  *loop;			/* start of the loop */

  codeBuffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_ANON, -1, 0);
  if (codeBuffer == MAP_FAILED) {
    perror("mmap");
    exit(0);
  }

  nfibs = (pifi) (jit_set_ip(codeBuffer).iptr);
        jit_prolog   (1);
  in =  jit_arg_ui   ();
        jit_getarg_ui(JIT_R2, in);              /* V0 = n */
  jit_delay(
        jit_movi_ui  (JIT_R1, 1),
  ref = jit_blti_ui  (jit_forward(), JIT_R2, 2));
        jit_subi_ui  (JIT_R2, JIT_R2, 1);
        jit_movi_ui  (JIT_R0, 1);

  loop= jit_get_label();
        jit_subi_ui  (JIT_R2, JIT_R2, 1);       /* decr. counter */
        jit_addr_ui  (JIT_V0, JIT_R0, JIT_R1);  /* V0 = R0 + R1 */
        jit_movr_ui  (JIT_R0, JIT_R1);          /* R0 = R1 */
  jit_delay(
        jit_addi_ui  (JIT_R1, JIT_V0, 1),       /* R1 = V0 + 1 */
        jit_bnei_ui  (loop, JIT_R2, 0));        /* if (R2) goto loop; */

  jit_patch(ref);                               /* patch forward jump */
        jit_movr_ui  (JIT_RET, JIT_R1);         /* RET = R1 */
        jit_ret();

  jit_flush_code(codeBuffer, jit_get_ip().ptr);

#ifdef LIGHTNING_DISASSEMBLE
  disassemble(stderr, codeBuffer, jit_get_ip().ptr);
#endif
#ifndef LIGHTNING_CROSS
  /* call the generated code, passing 36 as an argument */
  printf("nfibs(%d) = %d\n", 36, nfibs(36));
#endif
  return 0;
}
