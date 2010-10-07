/*
 * sample test/debug file, to be removed once test cases can be run
 */

#include <errno.h>
#include <stdio.h>
#include "lightning.h"
#include <sys/mman.h>

jit_insn	*code;
jit_insn	*end;

typedef long (*l_ll_t)(long, long);
typedef int (*i_ii_t)(int, int);
typedef int (*i_i_t)(int);
typedef void (*v_t)(void);

void
d(void)
{
    printf("done\n");
}

int
main(int argc, char *argv[])
{
    jit_insn	*label;
    v_t	 	 v;
    i_i_t	 i_i;
    i_ii_t	 i_ii;
    l_ll_t	 l_ll;

    code = mmap(NULL, 16384, PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (code == MAP_FAILED) {
	fprintf(stderr, "mmap failed (%s)", strerror(errno));
	exit(-1);
    }

    jit_set_ip(code);

#if 0
    label = jit_get_label();
    jit_movr_i(JIT_R0, _V0);
    jit_movr_i(JIT_R1, _V1);
    jit_movr_i(_V0, JIT_R0);
    jit_movr_i(_V1, JIT_R1);
    jit_movr_i(JIT_R1, JIT_R2);

    jit_movi_i(JIT_R0, 0);
    jit_movi_i(JIT_R0, 1);

    jit_negr_i(JIT_R0, JIT_R1);

    jit_addr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_addi_i(JIT_R0, JIT_R1, 16);

    jit_subr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_subi_i(JIT_R0, JIT_R1, 16);

    jit_mulr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_muli_i(JIT_R0, JIT_R1, 2);

    jit_hmulr_ui(JIT_R0, JIT_R1, JIT_R2);
    jit_hmuli_ui(JIT_R0, JIT_R1, 2);

    jit_divr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_divi_i(JIT_R0, JIT_R1, 2);

    jit_divr_ui(JIT_R0, JIT_R1, JIT_R2);
    jit_divi_ui(JIT_R0, JIT_R1, 2);

    jit_modr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_modi_i(JIT_R0, JIT_R1, 2);

    jit_modr_ui(JIT_R0, JIT_R1, JIT_R2);
    jit_modi_ui(JIT_R0, JIT_R1, 2);

    jit_andr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_andi_i(JIT_R0, JIT_R1, 16);

    jit_orr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_ori_i(JIT_R0, JIT_R1, 16);

    jit_xorr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_xori_i(JIT_R0, JIT_R1, 16);

    jit_lshr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_lshi_i(JIT_R0, JIT_R1, 16);

    jit_rshr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_rshi_i(JIT_R0, JIT_R1, 16);

    jit_rshr_ui(JIT_R0, JIT_R1, JIT_R2);
    jit_rshi_ui(JIT_R0, JIT_R1, 16);

    jit_jmpi(label);
    jit_jmpr(JIT_V0);
    jit_beqr_i(label, JIT_V0, JIT_V1);

    jit_ldxi_c(JIT_R0, JIT_R1, 10);
    jit_stxi_c(0, JIT_R0, JIT_R1);

    jit_ldxr_c(JIT_R0, JIT_R1, JIT_R2);
    jit_stxr_c(JIT_R2, JIT_R0, JIT_R1);

    jit_ldxi_uc(JIT_R0, JIT_R1, 10);
    jit_stxi_uc(0, JIT_R0, JIT_R1);

    jit_ldxr_uc(JIT_R0, JIT_R1, JIT_R2);
    jit_stxr_uc(JIT_R2, JIT_R0, JIT_R1);

    jit_ldxi_s(JIT_R0, JIT_R1, 10);
    jit_stxi_s(0, JIT_R0, JIT_R1);

    jit_ldxr_s(JIT_R0, JIT_R1, JIT_R2);
    jit_stxr_s(JIT_R2, JIT_R0, JIT_R1);

    jit_ldxi_us(JIT_R0, JIT_R1, 10);
    jit_stxi_us(0, JIT_R0, JIT_R1);

    jit_ldxr_us(JIT_R0, JIT_R1, JIT_R2);
    jit_stxr_us(JIT_R2, JIT_R0, JIT_R1);

    jit_ldxi_i(JIT_R0, JIT_R1, 10);
    jit_stxi_i(0, JIT_R0, JIT_R1);

    jit_ldxr_i(JIT_R0, JIT_R1, JIT_R2);
    jit_stxr_i(JIT_R2, JIT_R0, JIT_R1);

    jit_ldxi_ui(JIT_R0, JIT_R1, 10);
    jit_stxi_ui(0, JIT_R0, JIT_R1);

    jit_ldxr_ui(JIT_R0, JIT_R1, JIT_R2);
    jit_stxr_ui(JIT_R2, JIT_R0, JIT_R1);
    end = jit_get_label();
#endif

    label = jit_get_label();

#if 0
    /*	int f(int a)
     *  {
     *		return a + 1;
     *	}
     */
    jit_prolog(1);
    {
	int	a0;

	a0 = jit_arg_i();
	jit_getarg_i(JIT_R0, a0);
	jit_addi_i(JIT_R0, JIT_R0, 1);
	jit_movr_i(JIT_RET, JIT_R0);
    }
    jit_ret();

    jit_flush_code(code, jit_get_label());
    i_i = (i_i_t)code;
    printf("%d\n", (*i_i)(1));
#endif

#if 0
    /*	int f(int a, int b)
     *  {
     *		return a + b;
     *	}
     */
    jit_prolog(2);
    {
	int	a0, a1;

	a0 = jit_arg_i();
	a1 = jit_arg_i();
	jit_getarg_i(JIT_R0, a0);
	jit_getarg_i(JIT_R1, a1);
	jit_addr_i(JIT_R0, JIT_R0, JIT_R1);
	jit_movr_i(JIT_RET, JIT_R0);
    }
    jit_ret();

    jit_flush_code(code, jit_get_label());
    i_ii = (i_ii_t)code;
    printf("%d\n", (*i_ii)(-32, 2));
#endif

#if 0 /* FIXME figure out proper varargs abi... */
    jit_set_ip(code);
    jit_prolog(0);
    jit_prepare(1);
    jit_movi_p(JIT_R0, "Hello world!\n");
    jit_pusharg_p(JIT_R0);
    jit_finish(printf);
    jit_ret();
    jit_flush_code(code, jit_get_label());
    v = (v_t)code;
    (*v)();
#endif

#if defined(__mips64__)
#if 0
    /* long f(long a, long b)
     *  {
     *		return a + b;
     *	}
     */
    jit_prolog(2);
    {
	int	a0, a1;

	a0 = jit_arg_l();
	a1 = jit_arg_l();
	jit_getarg_l(JIT_R0, a0);
	jit_getarg_l(JIT_R1, a1);
	jit_addr_l(JIT_R0, JIT_R0, JIT_R1);
	jit_movr_l(JIT_RET, JIT_R0);
    }
    jit_ret();

    jit_flush_code(code, jit_get_label());
    l_ll = (l_ll_t)code;
    printf("%lx\n", (*l_ll)(0x3000000000000001L, 0x1000000000000003L));
#endif

    d();

    return 0;
}
