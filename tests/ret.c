/******************************** -*- C -*- ****************************
 *
 *	Test `JIT_RET'
 *
 ***********************************************************************/


/* Contributed by Ludovic Court�s.  */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "lightning.h"

typedef int (* int_return_int_t) (int);

jit_insn *buffer;

static int
identity (int arg)
{
  return arg;
}

static int_return_int_t
generate_function_proxy (int_return_int_t func)
{
  int_return_int_t result;
  int arg;

  buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_PRIVATE | MAP_ANON, -1, 0);
  if (buffer == MAP_FAILED) {
    perror("mmap");
    exit(0);
  }

  result = (int_return_int_t)(jit_set_ip (buffer).ptr);
  jit_prolog (1);
  arg = jit_arg_i ();
  jit_getarg_i (JIT_R1, arg);

  /* Reset `JIT_RET'.  */
  jit_movi_i (JIT_RET, -1);

  /* Invoke a FUNC.  */
  jit_prepare (1);
  jit_pusharg_i (JIT_R1);
  (void)jit_finish (func);

  /* Copy the result of FUNC from `JIT_RET' into our own result register.  */
  jit_retval_i (JIT_RET);

  jit_ret ();
  jit_flush_code (buffer, jit_get_ip ().ptr);

  return result;
}


int
main (int argc, char *argv[])
{
  int_return_int_t identity_proxy;

  identity_proxy = generate_function_proxy (identity);
  if (identity_proxy (7777) != 7777)
    {
      printf ("failed: got %i instead of %i\n",
	      identity_proxy (7777), 7777);
      return 1;
    }
  else
    printf ("succeeded\n");

  return 0;
}
