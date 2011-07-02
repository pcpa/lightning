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

    jit_get_cpu();

    jit_set_ip(buffer);
    jit_prolog(0);

/* simple tests calling directly arm/asm.h macros to check generated
 * code; after these are validated, proper lightning interfaces should
 * be implemented */

_VADD_F32(_F0,_F1,_F2);
_VADD_F64(_F0,_F1,_F2);
_VSUB_F32(_F0,_F1,_F2);
_VSUB_F64(_F0,_F1,_F2);
_VMUL_F32(_F0,_F1,_F2);
_VMUL_F64(_F0,_F1,_F2);
_VDIV_F32(_F0,_F1,_F2);
_VDIV_F64(_F0,_F1,_F2);
_VABS_F32(_F0,_F1);
_VABS_F64(_F0,_F1);
_VNEG_F32(_F0,_F1);
_VNEG_F64(_F0,_F1);
_VMOV_F32(_F0,_F1);
_VMOV_F64(_F0,_F1);
_VMOV_AA_D(_R1,_R2,_F0);
_VMOV_D_AA(_F0,_R1,_R2);
_VMOV_A_S(_R1,_F0);
_VMOV_S_A(_F0,_R1);
_VCMP_F32(_F0,_F1);
_VCMP_F64(_F0,_F1);
_VCMPE_F32(_F0,_F1);
_VCMPE_F64(_F0,_F1);
_VMRS(_R0);
_VMRS(_R15);	// APSR_nzcv encoded as _R15 (pc)
_VMSR(_R0);
_VCVT_S32_F32(_F0,_F1);
_VCVT_U32_F32(_F0,_F1);
_VCVT_S32_F64(_F0,_F1);
_VCVT_U32_F64(_F0,_F1);
_VCVT_F32_S32(_F0,_F1);
_VCVT_F32_U32(_F0,_F1);
_VCVT_F64_S32(_F0,_F1);
_VCVT_F64_U32(_F0,_F1);
_VCVT_F32_F64(_F0,_F1);
_VCVT_F64_F32(_F0,_F1);

/* set rounding mode in fpscr */
_VCVTR_S32_F32(_F0,_F1);
_VCVTR_U32_F32(_F0,_F1);
_VCVTR_S32_F64(_F0,_F1);
_VCVTR_U32_F64(_F0,_F1);

_VLDMIA_F32(_R0,_F2,6);
_VLDMIA_F64(_R0,_F2,6);
_VSTMIA_F32(_R0,_F2,6);
_VSTMIA_F64(_R0,_F2,6);
_VLDMIA_U_F32(_R0,_F2,6);
_VLDMIA_U_F64(_R0,_F2,6);
_VSTMIA_U_F32(_R0,_F2,6);
_VSTMIA_U_F64(_R0,_F2,6);
_VLDMDB_U_F32(_R0,_F2,6);
_VLDMDB_U_F64(_R0,_F2,6);
_VSTMDB_U_F32(_R0,_F2,6);
_VSTMDB_U_F64(_R0,_F2,6);
_VPUSH_F32(_F0,6);
_VPUSH_F64(_F0,6);
_VPOP_F32(_F0,6);
_VPOP_F64(_F0,6);

_VMOV_A_D_S8(_R0,_F1);
_VMOV_A_D_U8(_R0,_F1);
_VMOV_A_D_S16(_R0,_F1);
_VMOV_A_D_U16(_R0,_F1);
_VMOV_D_A_I8(_F1,_R0);
_VMOV_D_A_I16(_F1,_R0);

_VADD_I8(_F0,_F1,_F2);
_VADDQ_I8(_F0,_F2,_F4);
_VADD_I16(_F0,_F1,_F2);
_VADDQ_I16(_F0,_F2,_F4);
_VADD_I32(_F0,_F1,_F2);
_VADDQ_I32(_F0,_F2,_F4);
_VADD_I64(_F0,_F1,_F2);
_VADDQ_I64(_F0,_F2,_F4);
_VQADD_S8(_F0,_F1,_F2);
_VQADD_U8(_F0,_F1,_F2);
_VQADDQ_S8(_F0,_F2,_F4);
_VQADDQ_U8(_F0,_F2,_F4);
_VQADD_S16(_F0,_F1,_F2);
_VQADD_U16(_F0,_F1,_F2);
_VQADDQ_S16(_F0,_F2,_F4);
_VQADDQ_U16(_F0,_F2,_F4);
_VQADD_S32(_F0,_F1,_F2);
_VQADD_U32(_F0,_F1,_F2);
_VQADDQ_S32(_F0,_F2,_F4);
_VQADDQ_U32(_F0,_F2,_F4);
_VQADD_S64(_F0,_F1,_F2);
_VQADD_U64(_F0,_F1,_F2);
_VQADDQ_S64(_F0,_F2,_F4);
_VQADDQ_U64(_F0,_F2,_F4);
_VADDL_S8(_F0,_F1,_F2);
_VADDL_U8(_F0,_F1,_F2);
_VADDL_S16(_F0,_F1,_F2);
_VADDL_U16(_F0,_F1,_F2);
_VADDL_S32(_F0,_F1,_F2);
_VADDL_U32(_F0,_F1,_F2);
_VADDW_S8(_F0,_F2,_F3);
_VADDW_U8(_F0,_F2,_F3);
_VADDW_S16(_F0,_F2,_F3);
_VADDW_U16(_F0,_F2,_F3);
_VADDW_S32(_F0,_F2,_F3);
_VADDW_U32(_F0,_F2,_F3);

_VSUB_I8(_F0,_F1,_F2);
_VSUBQ_I8(_F0,_F2,_F4);
_VSUB_I16(_F0,_F1,_F2);
_VSUBQ_I16(_F0,_F2,_F4);
_VSUB_I32(_F0,_F1,_F2);
_VSUBQ_I32(_F0,_F2,_F4);
_VSUB_I64(_F0,_F1,_F2);
_VSUBQ_I64(_F0,_F2,_F4);
_VQSUB_S8(_F0,_F1,_F2);
_VQSUB_U8(_F0,_F1,_F2);
_VQSUBQ_S8(_F0,_F2,_F4);
_VQSUBQ_U8(_F0,_F2,_F4);
_VQSUB_S16(_F0,_F1,_F2);
_VQSUB_U16(_F0,_F1,_F2);
_VQSUBQ_S16(_F0,_F2,_F4);
_VQSUBQ_U16(_F0,_F2,_F4);
_VQSUB_S32(_F0,_F1,_F2);
_VQSUB_U32(_F0,_F1,_F2);
_VQSUBQ_S32(_F0,_F2,_F4);
_VQSUBQ_U32(_F0,_F2,_F4);
_VQSUB_S64(_F0,_F1,_F2);
_VQSUB_U64(_F0,_F1,_F2);
_VQSUBQ_S64(_F0,_F2,_F4);
_VQSUBQ_U64(_F0,_F2,_F4);
_VSUBL_S8(_F0,_F1,_F2);
_VSUBL_U8(_F0,_F1,_F2);
_VSUBL_S16(_F0,_F1,_F2);
_VSUBL_U16(_F0,_F1,_F2);
_VSUBL_S32(_F0,_F1,_F2);
_VSUBL_U32(_F0,_F1,_F2);
_VSUBW_S8(_F0,_F2,_F3);
_VSUBW_U8(_F0,_F2,_F3);
_VSUBW_S16(_F0,_F2,_F3);
_VSUBW_U16(_F0,_F2,_F3);
_VSUBW_S32(_F0,_F2,_F3);
_VSUBW_U32(_F0,_F2,_F3);
_VMUL_I8(_F0,_F1,_F2);
_VMULQ_I8(_F0,_F2,_F4);
_VMUL_I16(_F0,_F1,_F2);
_VMULQ_I16(_F0,_F2,_F4);
_VMUL_I32(_F0,_F1,_F2);
_VMULQ_I32(_F0,_F2,_F4);
_VMULL_S8(_F0,_F1,_F2);
_VMULL_U8(_F0,_F1,_F2);
_VMULL_S16(_F0,_F1,_F2);
_VMULL_U16(_F0,_F1,_F2);
_VMULL_S32(_F0,_F1,_F2);
_VMULL_U32(_F0,_F1,_F2);
_VABS_S8(_F0,_F1);
_VABSQ_S8(_F0,_F2);
_VQABS_S8(_F0,_F1);
_VQABSQ_S8(_F0,_F2);
_VABS_S16(_F0,_F1);
_VABSQ_S16(_F0,_F2);
_VQABS_S16(_F0,_F1);
_VQABSQ_S16(_F0,_F2);
_VABS_S32(_F0,_F1);
_VABSQ_S32(_F0,_F2);
_VQABS_S32(_F0,_F1);
_VQABSQ_S32(_F0,_F2);
_VNEG_S8(_F0,_F1);
_VNEGQ_S8(_F0,_F2);
_VQNEG_S8(_F0,_F1);
_VQNEGQ_S8(_F0,_F2);
_VNEG_S16(_F0,_F1);
_VNEGQ_S16(_F0,_F2);
_VQNEG_S16(_F0,_F1);
_VQNEGQ_S16(_F0,_F2);
_VNEG_S32(_F0,_F1);
_VNEGQ_S32(_F0,_F2);
_VQNEG_S32(_F0,_F1);
_VQNEGQ_S32(_F0,_F2);
_VAND(_F0,_F1,_F2);
_VANDQ(_F0,_F2,_F4);
_VBIC(_F0,_F1,_F2);
_VBICQ(_F0,_F2,_F4);
_VORR(_F0,_F1,_F2);
_VORRQ(_F0,_F2,_F4);
_VORN(_F0,_F1,_F2);
_VORNQ(_F0,_F2,_F4);
_VEOR(_F0,_F1,_F2);
_VEORQ(_F0,_F2,_F4);
_VMOV(_F0,_F1);
_VMOVQ(_F0,_F2);
_VMOVL_S8(_F0,_F2);
_VMOVL_U8(_F0,_F2);
_VMOVL_S16(_F0,_F2);
_VMOVL_U16(_F0,_F2);
_VMOVL_S32(_F0,_F2);
_VMOVL_U32(_F0,_F2);

{
    int code;
    if ((code = encode_vfp_immediate(ARM_VMOVI, 0xff0000ff, 0x00ffff00)) != -1)
	_VMOVI(code, _F0);
    if ((code = encode_vfp_immediate(ARM_VMOVI, 0x100, 0x100)) != -1)
	_VMOVI(code, _F0);
    if ((code = encode_vfp_immediate(ARM_VMOVI, 0x10001000, 0x10001000)) != -1)
	_VMOVI(code, _F0);
    if ((code = encode_vfp_immediate(ARM_VMVNI, 0x7fff, 0x7fff)) != -1)
	_VMVNI(code, _F0);
    if ((code = encode_vfp_immediate(ARM_VMOVI, 0xbe000000, 0xbe000000)) != -1)
	_VMOVI(code, _F0);
    if ((code = encode_vfp_immediate(ARM_VMOVI, 0xc1f80000, 0xc1f80000)) != -1)
	_VMOVI(code, _F0);
}

_VLDR_F32(_F0,_R0,2);
_VLDRN_F32(_F0,_R0,2);
_VLDR_F64(_F0,_R0,2);
_VLDRN_F64(_F0,_R0,2);
_VSTR_F32(_F0,_R0,2);
_VSTRN_F32(_F0,_R0,2);
_VSTR_F64(_F0,_R0,2);
_VSTRN_F64(_F0,_R0,2);

    jit_ret();
    jit_flush_code(buffer, jit_get_ip().ptr);
    //((void (*)(double,double))buffer)(a, b);
    disassemble(buffer, (long)jit_get_ip().ptr - (long)buffer);

    return (0);
}
