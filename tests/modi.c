/******************************** -*- C -*- ****************************
 *
 *	Test jit_modi_i
 *
 ***********************************************************************/


/* Contributed by Ludovic Courtes.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "lightning.h"

typedef int (* mod_t) (int);

jit_insn *buffer;

mod_t
generate_modi (int operand)
{
  mod_t result;
  int arg;

  buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_PRIVATE | MAP_ANON, -1, 0);
  if (buffer == MAP_FAILED) {
    perror("mmap");
    exit(0);
  }

  result = (mod_t)(jit_set_ip (buffer).iptr);
  jit_leaf (1);
  arg = jit_arg_i ();
  jit_getarg_i (JIT_R1, arg);

  jit_modi_i (JIT_R2, JIT_R1, operand);
  jit_movr_i (JIT_RET, JIT_R2);

  jit_ret ();
  jit_flush_code (buffer, jit_get_ip ().ptr);

  return result;
}

int
main (int argc, char *argv[])
{
  mod_t mod_eight = generate_modi (8);

  printf ("mod_eight (%i) = %i (vs. %i)\n",
	  20420, mod_eight (20420), (20420 % 8));
  printf ("mod_eight (%i) = %i (vs. %i)\n",
	  216096, mod_eight (216096), (216096 % 8));

  return 0;
}
