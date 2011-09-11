/******************************** -*- C -*- ****************************
 *
 *	Test `jit_pushr_i' and `jit_popr_i'
 *
 ***********************************************************************/


/* Contributed by Ludovic Courtès.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define JIT_NEED_PUSH_POP

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "lightning.h"

jit_insn *buffer;

typedef int (* stakumilo_t) (int);

static void
display_message (const char *fmt, int i)
{
  printf (fmt, i);
}

static stakumilo_t
generate_push_pop (void)
{
  static const char msg[] = "we got %i\n";
  stakumilo_t result;
  int arg;
  int retval;

  buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_PRIVATE | MAP_ANON, -1, 0);
  if (buffer == MAP_FAILED) {
    perror("mmap");
    exit(0);
  }

  result = (stakumilo_t)(jit_set_ip (buffer).ptr);
  jit_prolog (1);
  arg = jit_arg_i ();
  jit_getarg_i (JIT_R1, arg);

  /* Save R1 on the stack.  */
  jit_pushr_i (JIT_R1);

  /* Save two other registers just for the sake of using the stack.  */
  jit_movi_i (JIT_R0, -1);
  jit_movi_i (JIT_R2, -1);
  jit_pushr_i (JIT_R0);
  jit_pushr_i (JIT_R2);
  /* most likely need stack aligned at 16 bytes, dummy push to force align */
  jit_pushr_i (JIT_R2);

  jit_movr_i (JIT_R0, JIT_R1);
  jit_movi_p (JIT_R1, msg);

  /* Invoke a function that may modify R1.  */
  jit_prepare (2);
  jit_pusharg_i (JIT_R0);
  jit_pusharg_p (JIT_R1);
  (void)jit_finish (display_message);

  /* dummy pop for the sake of calling function with 16 byte aligned stack */
  jit_popr_i (JIT_R2);
  /* Restore the dummy registers.  */
  jit_popr_i (JIT_R2);
  jit_popr_i (JIT_R0);

  /* Restore R1.  */
  jit_popr_i (JIT_R1);

  jit_movr_i (JIT_RET, JIT_R1);

  jit_ret ();
  jit_flush_code (buffer, jit_get_ip ().ptr);

  return result;
}


int
main (int argc, char *argv[])
{
  stakumilo_t stakumilo;

  stakumilo = generate_push_pop ();
  if (stakumilo (7777) != 7777)
    {
      printf ("failed: got %i instead of %i\n",
	      stakumilo (7777), 7777);
      return 1;
    }
  else
    printf ("succeeded\n");

  return 0;
}
