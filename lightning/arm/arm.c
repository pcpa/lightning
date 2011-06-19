#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dis-asm.h>
#include <lightning.h>

#  if __WORDSIZE == 32
#    define address_buffer_length	16
#    define address_buffer_format	"%llx"
#  else
#    define address_buffer_length	32
#    define address_buffer_format	"%lx"
#  endif
static void
print_address(bfd_vma addr, struct disassemble_info *info)
{
    char		 buffer[address_buffer_length];

    sprintf(buffer, address_buffer_format, addr);
    (*info->fprintf_func)(info->stream, "0x%s", buffer);
}

static void
print_data(unsigned int *data, int length)
{
    int		offset;

    for (offset = 0; offset < length - 1; offset++)
	fprintf(stdout, "%02x ", data[offset]);
    if (offset < length)
	fprintf(stdout, "%02x\n", data[offset]);
}

static void disassemble(void *code, int length)
{
    static bfd			*bfd;
    static disassemble_info	 info;
    int				 bytes;
    static disassembler_ftype	 print_insn;
    bfd_vma			 pc = (unsigned long)code;
    bfd_vma			 end = (unsigned long)code + length;
    char			 buffer[address_buffer_length];

    if (bfd == NULL) {
	bfd_init();
	bfd = bfd_openr("a.out", NULL);
	assert(bfd != NULL);
	bfd_check_format(bfd, bfd_object);
	bfd_check_format(bfd, bfd_archive);
	print_insn = disassembler(bfd);
	assert(print_insn != NULL);
	INIT_DISASSEMBLE_INFO(info, stdout, fprintf);
#  if defined(__i386__) || defined(__x86_64__)
	info.arch = bfd_arch_i386;
#    if defined(__x86_64__)
	info.mach = bfd_mach_x86_64;
#    else
	info.mach = bfd_mach_i386_i386;
#    endif
#  endif
	info.print_address_func = print_address;
    }
    info.buffer = (bfd_byte *)code;
    info.buffer_vma = (unsigned long)code;
    info.buffer_length = length;
    while (pc < end) {
	bytes = sprintf(buffer, address_buffer_format, pc);
	(*info.fprintf_func)(stdout, "%*c0x%s\t", 16 - bytes, ' ', buffer);
	bytes = (*print_insn)(pc, &info);
#if 1
	fprintf(stdout, "\n\t\t\t");
	print_data((unsigned int *)(unsigned long)pc, bytes / sizeof(int));
#else
	putc('\n', stdout);
#endif
	pc += bytes;
    }
}

int
main(int argc, char *argv[])
{
    double		 a, b;
    int			 a0, a1, cond;
    char		 scond[16];
    unsigned char	*buffer = malloc(65536);

#define LT		0
#define LE		1
#define EQ		2
#define GE		3
#define GT		4
#define NE		5
#define UNLT		6
#define UNLE		7
#define UNEQ		8
#define UNGE		9
#define UNGT		10
#define LTGT		11
#define ORD		12
#define UNORD		13
    jit_get_cpu();

    for (;;) {
	printf("enter expr: ");	fflush(stdout);
	if (scanf("%lf %s %lf", &a, scond, &b) == 3) {
	    jit_set_ip(buffer);
	    jit_prolog(0);
	    jit_prolog_d(2);
	    a0 = jit_arg_d();
	    a1 = jit_arg_d();
	    jit_getarg_d(JIT_FPR0, a0);
	    jit_getarg_d(JIT_FPR1, a1);
	    if (scond[0] == '<') {
		if (scond[1] == '\0')		cond = LT;
		else if (scond[1] == '=') {
		    if (scond[2] == '\0')	cond = LE;
		    else			break;
		}
		else				break;
	    }
	    else if (scond[0] == '=') {
		if (scond[1] == '=') {
		    if (scond[2] == '\0')	cond = EQ;
		    else			break;
		}
		else				break;
	    }
	    else if (scond[0] == '>') {
		if (scond[1] == '\0')		cond = GT;
		else if (scond[1] == '=') {
		    if (scond[2] == '\0')	cond = GE;
		    else			break;
		}
		else				break;
	    }
	    else if (scond[0] == '@') {
		if (scond[1] == '\0')		cond = ORD;
		else				break;
	    }
	    else if (scond[0] == '!') {
		if (scond[1] == '=') {
		    if (scond[2] == '\0')	cond = NE;
		    else			break;
		}
		else if (scond[1] == '<') {
		    if (scond[2] == '\0')	cond = UNLT;
		    else if (scond[2] == '=') {
			if (scond[3] == '\0')	cond = UNLE;
			else			break;
		    }
		    else			break;
		}
		else if (scond[1] == '=') {
		    if (scond[2] == '=') {
			if (scond[3] == '\0')	cond = UNEQ;
			else			break;
		    }
		    else			break;
		}
		else if (scond[1] == '>') {
		    if (scond[2] == '\0')	cond = UNGT;
		    else if (scond[2] == '=') {
			if (scond[3] == '\0')	cond = UNGE;
			else			break;
		    }
		    else			break;
		}
		else if (scond[1] == '!') {
		    if (scond[2] == '=') {
			if (scond[3] == '\0')	cond = LTGT;
			else			break;
		    }
		    else			break;
		}
		else if (scond[1] == '@') {
		    if (scond[2] == '\0')	cond = UNORD;
		    else			break;
		}
	    }
	    else
		break;
	}
	else
	    break;
	switch (cond) {
	    case LT:
#if defined(jit_ltr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f < %f\n");
		jit_ltr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case LE:
#if defined(jit_ler_d)
		jit_movi_i(JIT_R1, (int)"%d: %f <= %f\n");
		jit_ler_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case EQ:
#if defined(jit_eqr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f == %f\n");
		jit_eqr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case GE:
#if defined(jit_ger_d)
		jit_movi_i(JIT_R1, (int)"%d: %f >= %f\n");
		jit_ger_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case GT:
#if defined(jit_gtr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f > %f\n");
		jit_gtr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case NE:
#if defined(jit_ner_d)
		jit_movi_i(JIT_R1, (int)"%d: %f != %f\n");
		jit_ner_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case UNLT:
#if defined(jit_unltr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !< %f\n");
		jit_unltr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case UNLE:
#if defined(jit_unler_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !<= %f\n");
		jit_unler_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case UNEQ:
#if defined(jit_uneqr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !== %f\n");
		jit_uneqr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case UNGE:
#if defined(jit_unger_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !>= %f\n");
		jit_unger_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case UNGT:
#if defined(jit_ungtr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !> %f\n");
		jit_ungtr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case LTGT:
#if defined(jit_ltgtr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !!= %f\n");
		jit_ltgtr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case ORD:
#if defined(jit_ordr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f @ %f\n");
		jit_ordr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    case UNORD:
#if defined(jit_unordr_d)
		jit_movi_i(JIT_R1, (int)"%d: %f !@ %f\n");
		jit_unordr_d(JIT_R0, JIT_FPR0, JIT_FPR1);
#else
		goto fail;
#endif
		break;
	    default:
		abort();
	}
	jit_prepare(2);
	jit_prepare_d(2);
	{
	    jit_pusharg_d(JIT_FPR1);
	    jit_pusharg_d(JIT_FPR0);
	    jit_pusharg_i(JIT_R0);
	    jit_pusharg_p(JIT_R1);
	}
	jit_finish(printf);
	jit_ret();

	jit_flush_code(buffer, jit_get_ip().ptr);
	((void (*)(double,double))buffer)(a, b);
	//disassemble(buffer, (long)jit_get_ip().ptr - (long)buffer);
	fflush(stdout);
    }
fail:
    return (0);
}
