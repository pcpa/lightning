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
    jit_insn		*back;
    jit_insn		*next;
    unsigned char	*buffer = malloc(65536);

    jit_get_cpu();
    printf("jit for armv%d%s%s%s\n",
	   jit_cpu.armvn,
	   jit_cpu.thumb ? "t" : "", jit_cpu.thumb > 1 ? "2" : "",
	   jit_cpu.armve ? "e" : "");
    jit_set_ip(buffer);

    /* <T> is JIT_TMP (may still change) */

#if 0
    back = jit_get_label();
    jit_nop(1);				// mov r0, r0
    jit_movr_i(_R0, _R1);		// mov r0, r1
    jit_movi_i(_R0, 1);			// mov r0, #1
    jit_movi_i(_R0, -1);		// mvn r0, #0
    jit_notr_i(_R0, _R1);		// mvn r0, r1
    jit_negr_i(_R0, _R1);		// rsb r0, r1, #0
    jit_addr_i(_R0, _R1, _R2);		// add r0, r1, r2
    jit_addi_i(_R0, _R1, 2);		// add r0, r1, #2
    jit_addi_i(_R0, _R1, -2);		// sub r0, r1, #2
    jit_addcr_ui(_R0, _R1, _R2);	// adds r0, r1, r2
    jit_addci_ui(_R0, _R1, 2);		// adds r0, r1, #2
    jit_addci_ui(_R0, _R1, -2);		// subs r0, r1, #2
    jit_addxr_ui(_R0, _R1, _R2);	// adcs r0, r1, r2
    jit_addxi_ui(_R0, _R1, 2);		// adcs r0, r1, #2
    jit_addxi_ui(_R0, _R1, -2);		// sbcs r0, r1, #2
    jit_subr_i(_R0, _R1, _R2);		// sub r0, r1, r2
    jit_subi_i(_R0, _R1, 2);		// sub r0, r1, #2
    jit_subi_i(_R0, _R1, -2);		// add r0, r1, #2
    jit_subcr_ui(_R0, _R1, _R2);	// subs r0, r1, r2
    jit_subci_ui(_R0, _R1, 2);		// subs r0, r1, #2
    jit_subci_ui(_R0, _R1, -2);		// adds r0, r1, #2
    jit_subxr_ui(_R0, _R1, _R2);	// sbcs r0, r1, r2
    jit_subxi_ui(_R0, _R1, 2);		// sbcs r0, r1, #2
    jit_subxi_ui(_R0, _R1, -2);		// adcs r0, r1, #2
    jit_rsbr_i(_R0, _R1, _R2);		// rsb r0, r1, r2
    jit_rsbi_i(_R0, _R1, 2);		// rsb r0, r1, #2
    jit_rsbi_i(_R0, _R1, -2);		// mvn r0, #1; rsb r0, r1, r0
    jit_mulr_i(_R0, _R1, _R2);		// mul r0, r1, r2
    jit_muli_i(_R0, _R1, 2);		// mov r0, #2; mul r0, r1, r0
    jit_muli_i(_R0, _R1, -2);		// mvn r0, #1; mul r0, r1, r0
    jit_mulr_i(_R0, _R1, _R2);		// mul r0, r1, r2
    jit_muli_i(_R0, _R1, 2);		// mov r0, #2; mul r0, r1, r0
    jit_hmulr_i(_R0, _R1, _R2);		// smull <T>, r0, r1, r2
    jit_hmuli_i(_R0, _R1, 2);		// mov <T>, #2; smull <T>, r0, r1, <T>
    jit_hmulr_ui(_R0, _R1, _R2);	// umull <T>, r0, r1, r2
    jit_hmuli_ui(_R0, _R1, 2);		// mov <T>, #2; umull <T>, r0, r1, <T>
    jit_divr_i(_R0, _R1, _R2);		// push {r1,r2,r3}; mov r0, r1; mov r1, r2; bl #<__eabi_idivmod>; pop {r1, r2, r3}
    jit_divr_ui(_R0, _R1, _R2);		// push {r1,r2,r3}; mov r0, r1; mov r1, r2; bl #<__eabi_uidivmod>; pop {r1, r2, r3}
    jit_divi_i(_R0, _R1, 2);		// mov r8, #2; push {r1,r2,r3}; mov r0, r1; mov r1, r8; bl #<__eabi_idivmod>; pop {r1, r2, r3}
    jit_modr_i(_R0, _R1, _R2);		// push {r1,r2,r3}; mov r0, r1; mov r1, r2; bl #<__eabi_idivmod>; mov r0, r1; pop {r1, r2, r3}
    jit_modr_ui(_R0, _R1, _R2);		// push {r1,r2,r3}; mov r0, r1; mov r1, r2; bl #<__eabi_uidivmod>; mov r0, r1; pop {r1, r2, r3}
    jit_modi_i(_R0, _R1, 2);		// mov r8, #2; push {r1,r2,r3}; mov r0, r1; mov r1, r8; bl #<__eabi_idivmod>; mov r0, r1; pop {r1, r2, r3}
    jit_andr_i(_R0, _R1, _R2);		// and r0, r1, r2
    jit_andi_i(_R0, _R1, 2);		// and r0, r1, #2
    jit_andi_i(_R0, _R1, -2);		// bic r0, r1, #1
    jit_orr_i(_R0, _R1, _R2);		// orr r0, r1, r2
    jit_ori_i(_R0, _R1, 2);		// orr r0, r1, #2
    jit_ori_i(_R0, _R1, -2);		// mvn r0, #1; orr r0, r1, r0
    jit_xorr_i(_R0, _R1, _R2);		// eor r0, r1, r2
    jit_xori_i(_R0, _R1, 2);		// eor r0, r1, #2
    jit_xori_i(_R0, _R1, -2);		// mvn r0, #1; eor r0, r1, r0
    jit_lshr_i(_R0, _R1, _R2);		// lsl r0, r1, r2
    jit_lshi_i(_R0, _R1, 2);		// lsl r0, r1, #2
    jit_rshr_i(_R0, _R1, _R2);		// asr r0, r1, r2
    jit_rshi_i(_R0, _R1, 2);		// asr r0, r1, #2
    jit_rshr_ui(_R0, _R1, _R2);		// lsr r0, r1, r2
    jit_rshi_ui(_R0, _R1, 2);		// lsr r0, r1, #2
    jit_ltr_i(_R0, _R1, _R2);		// cmp r1, r2; movlt r0, #1; movge r0, #0
    jit_lti_i(_R0, _R1, 2);		// cmp r1, #2; movlt r0, #1; movge r0, #0
    jit_lti_i(_R0, _R1, -2);		// cmn r1, #2; movlt r0, #1; movge r0, #0
    jit_ltr_ui(_R0, _R1, _R2);		// cmp r1, r2; movcc r0, #1; movcs r0, #0
    jit_lti_ui(_R0, _R1, 2);		// cmp r1, #2; movcc r0, #1; movcs r0, #0
    jit_lti_ui(_R0, _R1, -2);		// cmn r1, #2; movcc r0, #1; movcs r0, #0
    jit_ler_i(_R0, _R1, _R2);		// cmp r1, r2; movle r0, #1; movgt r0, #0
    jit_lei_i(_R0, _R1, 2);		// cmp r1, #2; movle r0, #1; movgt r0, #0
    jit_lei_i(_R0, _R1, -2);		// cmn r1, #2; movle r0, #1; movgt r0, #0
    jit_ler_ui(_R0, _R1, _R2);		// cmp r1, r2; movls r0, #1; movhi r0, #0
    jit_lei_ui(_R0, _R1, 2);		// cmp r1, #2; movls r0, #1; movhi r0, #0
    jit_lei_ui(_R0, _R1, -2);		// cmn r1, #2; movls r0, #1; movhi r0, #0
    jit_eqr_i(_R0, _R1, _R2);		// cmp r1, r2; moveq r0, #1; movne r0, #0
    jit_eqi_i(_R0, _R1, 2);		// cmp r1, #2; moveq r0, #1; movne r0, #0
    jit_eqi_i(_R0, _R1, -2);		// cmn r1, #2; moveq r0, #1; movne r0, #0
    jit_ger_i(_R0, _R1, _R2);		// cmp r1, r2; movge r0, #1; movlt r0, #0
    jit_gei_i(_R0, _R1, 2);		// cmp r1, #2; movge r0, #1; movlt r0, #0
    jit_gei_i(_R0, _R1, -2);		// cmn r1, #2; movge r0, #1; movlt r0, #0
    jit_ger_ui(_R0, _R1, _R2);		// cmp r1, r2; movcs r0, #1; movcc r0, #0
    jit_gei_ui(_R0, _R1, 2);		// cmp r1, #2; movcs r0, #1; movcc r0, #0
    jit_gei_ui(_R0, _R1, -2);		// cmn r1, #2; movcs r0, #1; movcc r0, #0
    jit_gtr_i(_R0, _R1, _R2);		// cmp r1, r2; movgt r0, #1; movle r0, #0
    jit_gti_i(_R0, _R1, 2);		// cmp r1, #2; movgt r0, #1; movle r0, #0
    jit_gti_i(_R0, _R1, -2);		// cmn r1, #2; movgt r0, #1; movle r0, #0
    jit_gtr_ui(_R0, _R1, _R2);		// cmp r1, r2; movhi r0, #1; movls r0, #0
    jit_gti_ui(_R0, _R1, 2);		// cmp r1, #2; movhi r0, #1; movls r0, #0
    jit_gti_ui(_R0, _R1, -2);		// cmn r1, #2; movhi r0, #1; movls r0, #0
    jit_ner_i(_R0, _R1, _R2);		// subs r0, r1, r2; movne r0, #1
    jit_nei_i(_R0, _R1, 2);		// subs r0, r1, #2; movne r0, #1
    jit_nei_i(_R0, _R1, -2);		// adds r0, r1, #2; movne r0, #1
    jit_jmpr(_R0);			// mov pc, r0
    jit_jmpi(back);			// mov <T>, #<q3>; orr <T>, <T>, #<q2>; orr <T>, <T>, #<q1>; orr <T>, <T> #<q0>; mov pc, <T>
    next = jit_movi_p(_R0, 0);		// mov r0, #-570425344; orr r0, r0, #11337728; orr r0, r0, #48640; orr r0, r0, #239
    jit_patch_movi(next, (void *)0xdeadbeef);	// 0xde000000		    0xad0000		   0xbe00	       0xef
    jit_bltr_i(back, _R0, _R1);		// cmp r0, r1; blt #<back>
    next = jit_bltr_i(NULL, _R0, _R1);	// cmp r0, r1; blt #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_blti_i(back, _R0, 1);		// cmp r0, #1; blt #<back>
    next = jit_blti_i(NULL, _R0, 1);	// cmp r0, #1; blt #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bltr_ui(back, _R0, _R1);	// cmp r0, r1; bcc #<back>
    next = jit_bltr_ui(NULL, _R0, _R1);	// cmp r0, r1; bcc #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_blti_ui(back, _R0, 1);		// cmp r0, #1; bcc #<back>
    next = jit_blti_ui(NULL, _R0, 1);	// cmp r0, #1; bcc #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bler_i(back, _R0, _R1);		// cmp r0, r1; ble #<back>
    next = jit_bler_i(NULL, _R0, _R1);	// cmp r0, r1; ble #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_blei_i(back, _R0, 1);		// cmp r0, #1; ble #<back>
    next = jit_blei_i(NULL, _R0, 1);	// cmp r0, #1; ble #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bler_ui(back, _R0, _R1);	// cmp r0, r1; bls #<back>
    next = jit_bler_ui(NULL, _R0, _R1);	// cmp r0, r1; bls #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_blei_ui(back, _R0, 1);		// cmp r0, #1; bls #<back>
    next = jit_blei_ui(NULL, _R0, 1);	// cmp r0, #1; bls #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_beqr_i(back, _R0, _R1);		// cmp r0, r1; beq #<back>
    next = jit_beqr_i(NULL, _R0, _R1);	// cmp r0, r1; beq #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_beqi_i(back, _R0, 1);		// cmp r0, #1; beq #<back>
    next = jit_beqi_i(NULL, _R0, 1);	// cmp r0, #1; beq #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bger_i(back, _R0, _R1);		// cmp r0, r1; bge #<back>
    next = jit_bger_i(NULL, _R0, _R1);	// cmp r0, r1; bge #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bgei_i(back, _R0, 1);		// cmp r0, #1; bge #<back>
    next = jit_bgei_i(NULL, _R0, 1);	// cmp r0, #1; bge #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bger_ui(back, _R0, _R1);	// cmp r0, r1; bcs #<back>
    next = jit_bger_ui(NULL, _R0, _R1);	// cmp r0, r1; bcs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bgei_ui(back, _R0, 1);		// cmp r0, #1; bcs #<back>
    next = jit_bgei_ui(NULL, _R0, 1);	// cmp r0, #1; bcs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bgtr_i(back, _R0, _R1);		// cmp r0, r1; bgt #<back>
    next = jit_bgtr_i(NULL, _R0, _R1);	// cmp r0, r1; bgt #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bgti_i(back, _R0, 1);		// cmp r0, #1; bgt #<back>
    next = jit_bgti_i(NULL, _R0, 1);	// cmp r0, #1; bgt #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bgtr_ui(back, _R0, _R1);	// cmp r0, r1; bhi #<back>
    next = jit_bgtr_ui(NULL, _R0, _R1);	// cmp r0, r1; bhi #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bgti_ui(back, _R0, 1);		// cmp r0, #1; bhi #<back>
    next = jit_bgti_ui(NULL, _R0, 1);	// cmp r0, #1; bhi #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bner_i(back, _R0, _R1);		// cmp r0, r1; bne #<back>
    next = jit_bner_i(NULL, _R0, _R1);	// cmp r0, r1; bne #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bnei_i(back, _R0, 1);		// cmp r0, #1; bne #<back>
    next = jit_bnei_i(NULL, _R0, 1);	// cmp r0, #1; bne #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_boaddr_i(back, _R0, _R1);	// adds r0, r0, r1; bvs #<back>
    next = jit_boaddr_i(NULL, _R0, _R1);// adds r0, r0, r1; bvs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_boaddi_i(back, _R0, 1);		// adds r0, r0, #1; bvs #<back>
    next = jit_boaddi_i(NULL, _R0, 1);	// adds r0, r0, #1; bvs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_boaddr_ui(back, _R0, _R1);	// adds r0, r0, r1; bcs #<back>
    next = jit_boaddr_ui(NULL, _R0, _R1);// adds r0, r0, r1; bcs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_boaddi_ui(back, _R0, 1);	// adds r0, r0, #1; bcs #<back>
    next = jit_boaddi_ui(NULL, _R0, 1);	// adds r0, r0, #1; bcs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bosubr_i(back, _R0, _R1);	// subs r0, r0, r1; bvs #<back>
    next = jit_bosubr_i(NULL, _R0, _R1);// subs r0, r0, r1; bvs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bosubi_i(back, _R0, 1);		// subs r0, r0, #1; bvs #<back>
    next = jit_bosubi_i(NULL, _R0, 1);	// subs r0, r0, #1; bvs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bosubr_ui(back, _R0, _R1);	// subs r0, r0, r1; bcs #<back>
    next = jit_bosubr_ui(NULL, _R0, _R1);// subs r0, r0, r1; bcs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bosubi_ui(back, _R0, 1);	// subs r0, r0, #1; bcs #<back>
    next = jit_bosubi_ui(NULL, _R0, 1);	// subs r0, r0, #1; bcs #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bmcr_i(back, _R0, _R1);		// tst r0, r1; bne #<back>
    next = jit_bmcr_i(NULL, _R0, _R1);	// tst r0, r1; bne #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bmci_i(back, _R0, 1);		// tst r0, #1; bne #<back>
    jit_bmci_i(back, _R0, -1);		// (mvn <T>, #0; tst r0, <T>) or (teq r0, #0); bne #<back>
    next = jit_bmci_i(NULL, _R0, 1);	// tst r0, #1; bne #<next>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bmsr_i(back, _R0, _R1);		// tst r0, r1; beq #<back>
    next = jit_bmsr_i(NULL, _R0, _R1);	// tst r0, r1; beq #<back>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    jit_bmsi_i(back, _R0, 1);		// tst r0, #1; beq #<back>
    jit_bmsi_i(back, _R0, -1);		// (mvn <T>, #0; tst r0, <T>) or (teq r0, #0); beq #<back>
    next = jit_bmsi_i(NULL, _R0, 1);	// tst r0, #1; beq #<back>
    jit_nop(1);	jit_patch(next);	// mov r0, r0; #<next>
    next = jit_get_label();
    jit_ldr_c(_R0, _R1);		// ldrsb r0, [r1]
    jit_ldi_c(_R0, next);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... ldrsb r0, [<T>]
    jit_ldxr_c(_R0, _R1, _R2);		// ldrsb r0, [r1, r2]
    jit_ldxi_c(_R0, _R1, 2);		// ldrsb r0, [r1, #2]
    jit_ldxi_c(_R0, _R1, -2);		// ldrsb r0, [r1, #-2]
    jit_ldr_uc(_R0, _R1);		// ldrb r0, [r1]
    jit_ldi_uc(_R0, next);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... ldrb r0, [<T>]
    jit_ldxr_uc(_R0, _R1, _R2);		// ldrb r0, [r1, r2]
    jit_ldxi_uc(_R0, _R1, 2);		// ldrb r0, [r1, #2]
    jit_ldxi_uc(_R0, _R1, -2);		// ldrb r0, [r1, #-2]
    jit_ldr_s(_R0, _R1);		// ldrsh r0, [r1]
    jit_ldi_s(_R0, next);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... ldrsh r0, [<T>]
    jit_ldxr_s(_R0, _R1, _R2);		// ldrsh r0, [r1, r2]
    jit_ldxi_s(_R0, _R1, 2);		// ldrsh r0, [r1, #2]
    jit_ldxi_s(_R0, _R1, -2);		// ldrsh r0, [r1, #-2]
    jit_ldr_us(_R0, _R1);		// ldrh r0, [r1]
    jit_ldi_us(_R0, next);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... ldrh r0, [<T>]
    jit_ldxr_us(_R0, _R1, _R2);		// ldrh r0, [r1, r2]
    jit_ldxi_us(_R0, _R1, 2);		// ldrh r0, [r1, #2]
    jit_ldxi_us(_R0, _R1, -2);		// ldrh r0, [r1, #-2]
    jit_ldr_i(_R0, _R1);		// ldr r0, [r1]
    jit_ldi_i(_R0, next);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... ldr r0, [<T>]
    jit_ldxr_i(_R0, _R1, _R2);		// ldr r0, [r1, r2]
    jit_ldxi_i(_R0, _R1, 2);		// ldr r0, [r1, #2]
    jit_ldxi_i(_R0, _R1, -2);		// ldr r0, [r1, #-2]
    jit_str_c(_R1, _R0);		// strb r0, [r1]
    jit_sti_c(next, _R0);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... strb r0, [<T>]
    jit_stxr_c(_R2, _R1, _R0);		// strb r0, [r1, r2]
    jit_stxi_c(2, _R1, _R0);		// strb r0, [r1, #2]
    jit_stxi_c(-2, _R1, _R0);		// strb r0, [r1, #-2]
    jit_str_s(_R1, _R0);		// strh r0, [r1]
    jit_sti_s(next, _R0);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... strh r0, [<T>]
    jit_stxr_s(_R2, _R1, _R0);		// strh r0, [r1, r2]
    jit_stxi_s(2, _R1, _R0);		// strh r0, [r1, #2]
    jit_stxi_s(-2, _R1, _R0);		// strh r0, [r1, #-2]
    jit_str_i(_R1, _R0);		// str r0, [r1]
    jit_sti_i(next, _R0);		// mov <T>, <T>, #<q3>; orr <T>, <T> #<q2> ... str r0, [<T>]
    jit_stxr_i(_R2, _R1, _R0);		// str r0, [r1, r2]
    jit_stxi_i(2, _R1, _R0);		// str r0, [r1, #2]
    jit_stxi_i(-2, _R1, _R0);		// str r0, [r1, #-2]
    jit_ldr_d(_F0, _R2);		// ldrd r9, [r2]; str r9, [fp, #-8]; ldrd r9, [r2, #4]; str r9, [fp, #-4]
    jit_ldi_d(_F0, (void*)0x80000000);	// mov <T>, #-2147483648; ldrd r9, [r8]; str r9, [fp, #-8]; ldrd r9, [r8, #4]; str r9, [fp, #-4]
    jit_ldxr_d(_F0, _R2, _R3);		// ldr r9, [r2, r3]; str r9, [fp, #-8]; add r8, r3, #4; ldrd r9, [r2, r8]; str r9, [fp, #-4]
    jit_ldxi_d(_F0, _R2, 3);		// ldr r9, [r2, #3]; str r9, [fp, #-8]; ldrd r9, [r2, #7]; str r9, [fp, #-4]
    jit_ldxi_d(_F0, _R2, -3);		// ldr r9, [r2, #-3]; str r9, [fp, #-8]; ldrd r9, [r2, #1]; str r9, [fp, #-4]
    jit_str_d(_R2, _F0);		// ldr r9, [fp, #-8]; str r9, [r2]; ldr r9, [fp, #-4]; str r9, [r2, #4]
    jit_sti_d((void*)0x80000000, _R0);	// ldr r9, [fp, #-8]; mov <T>, #-2147483648; strd r0, [<T>]; str r9, [f8]; ldr r9, [fp, #-4]; str r9, [r8, #4]
    jit_stxr_d(_R3, _R2, _F0);		// ldr r9, [fp, #-8]; str r9, [r2, r3]; ldr r9, [fp, #-4]; add r8, r3, #4; str r9, [r2, r8]
    jit_stxi_d(3, _R2, _F0);		// ldr r9, [fp, #-8]; str r9, [r2, #3]; ldr r9, [fp, #-4]; str r9, [r2, #7]
    jit_stxi_d(-3, _R2, _F0);		// ldr r9, [fp, #-8]; str r9, [r2, #-3]; ldr r9, [fp, #-4]; str r9, [r2, #1]
    _LDRD(_F0, _R2, _R3);		// ldrd r0, [r2, r3]
    _LDRDN(_F0, _R2, _R3);		// ldrd r0, [r2, -r3]
    _LDRDI(_F0, _R2, 3);		// ldrd r0, [r2, #3]
    _LDRDIN(_F0, _R2, 3);		// ldrd r0, [r2, #-3]
    _STRD(_F0, _R2, _R3);		// strd r0, [r2, r3]
    _STRDN(_F0, _R2, _R3);		// strd r0, [r2, -r3]
    _STRDI(_F0, _R2, 3);		// strd r0, [r2, #3]
    _STRDIN(_F0, _R2, 3);		// strd r0, [r2, #-3]
    _LDMIA(_R0, 0xffff);	// ldm r0, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
    _LDMIA_U(_R1, 0x7ffe);	// ldm r1!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr}
    _LDMIB(_R2, 0x3ffc);	// ldmib r2, {r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp}
    _LDMIB_U(_R3, 0x1ff8);	// ldmib r3!, {r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
    _LDMDA(_R4, 0x0ff0);	// ldmda r4, {r4, r5, r6, r7, r8, r9, sl, fp}
    _LDMDA_U(_R5, 0x07e0);	// ldmda r5!, {r5, r6, r7, r8, r9, sl}
    _LDMDB(_R6, 0x03c0);	// ldmdb r6, {r6, r7, r8, r9}
    _LDMDB_U(_R7, 0x0180);	// ldmdb r7!, {r7, r8}
    _STMIA(_R8, 0x0240);	// stm r8, {r6, r9}
    _STMIA_U(_R9, 0x0660);	// stmia r9!, {r5, r6, r9, sl}
    _STMIB(_R10, 0x0e70);	// stmib sl, {r4, r5, r6, r9, sl, fp}
    _STMIB_U(_R11, 0x1e78);	// stmib fp!, {r3, r4, r5, r6, r9, sl, fp, ip}
    _STMDA(_R12, 0x3e7c);	// stmda ip, {r2, r3, r4, r5, r6, r9, sl, fp, ip, sp}
    _STMDA_U(_R13, 0x7e7e);	// stmda sp!, {r1, r2, r3, r4, r5, r6, r9, sl, fp, ip, sp, lr}
    _STMDB(_R14, 0xfe7f);	// stmdb lr, {r0, r1, r2, r3, r4, r5, r6, r9, sl, fp, ip, sp, lr, pc}
    _STMDB_U(JIT_FP, 0xffff);	// stmdb fp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, pc}
    _PUSH(0xf);			// push {r0, r1, r2, r3}
    _POP(0xf);			// pop {r0, r1, r2, r3}
    jit_movi_p(JIT_R0, main);	// mov r0, #<q3>, orrr r0, r0, #<q2> ...
    jit_callr(JIT_R0);		// blx r0
    jit_calli(printf);		// mov <T>, #<q3>, orrr r0, r8, #<q2> ...; blx <T>
#endif

#if 0
    /*
     * void f(int a, int b) { printf("%d + %d = %d\n", a, b, a + b); }
     */
    {
	int	a0, a1;
	jit_prolog(2);		// push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,fp,lr}; mov fp, sp; mov r8, r8, #8, orr, r8, r8, #0...; sub sp, sp, r8
	a0 = jit_arg_i();
	a1 = jit_arg_i();
	jit_getarg_i(JIT_V0, a0);	// ldr r4, [fp]
	jit_getarg_i(JIT_V1, a1);	// ldr r5, [fp, #4]
	jit_addr_i(JIT_R1, JIT_V0, JIT_V1);	// add r1, r4, r5
	jit_movi_p(JIT_R0, "%d + %d = %d\n");	// mov r0, #<q0>, orr, r0, r0, #<q1>...
	jit_prepare(4);
	{
	    jit_pusharg_i(JIT_R1);	// str r1, [fp, #12]
	    jit_pusharg_i(JIT_V1);	// str r5, [fp, #8]
	    jit_pusharg_i(JIT_V0);	// str r4, [fp, #4]
	    jit_pusharg_i(JIT_R0);	// str r0, [fp]
	}
	jit_finish(printf);		// ldm fp, {r0, r1, r2, r3}, mov r0, #<q3>; orr r8, r8, #<q2>...; blx r8
	jit_ret();			// add sp, fp, #16; pop {r4, r5, r6, r7, r8, r9, fp, pc}
    }
    jit_flush_code(buffer, jit_get_ip().ptr);
    ((void (*)(int,int))buffer)(1, 2);
#endif

#if 1
    /*
     *	void f(double a, double b) {
     *		printf("%f + %f = %f\n", a, b, a + b);
     *		printf("%f - %f = %f\n", a, b, a - b);
     *		printf("%f * %f = %f\n", a, b, a * b);
     *		printf("%f / %f = %f\n", a, b, a / b);
     *	}
     */
    {
	int	a0, a1;
	jit_prolog(0);		// push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,fp,lr}; mov fp, sp; mov r8, r8, #8, orr, r8, r8, #0...; sub sp, sp, r8
	jit_prolog_d(2);
	a0 = jit_arg_d();
	a1 = jit_arg_d();
	jit_getarg_d(JIT_FPR0, a0);	// ldrd r8, [fp]; strd r8, [fp, #-8]
	jit_getarg_d(JIT_FPR1, a1);	// ldrd r8, [fp, #8]; strd r8, [fp, #-16]
	jit_addr_d(JIT_FPR2, JIT_FPR0, JIT_FPR1);	// push {r0, r1, r2, r3}; ldrd r0, [fp, #-8]; ldrd r0, [fp, #-16]; bl #<__adddf3>; strd r0, [fp, #-24]; pop {r0, r1, r2, r3}
	jit_movi_p(JIT_R0, "%f + %f = %f\n");	// mov r0, #<q0>, orr, r0, r0, #<q1>...
	jit_prepare(1);
	jit_prepare_d(3);
	{
	    jit_pusharg_d(JIT_FPR2);	// ldrd r8, [fp, #-24]; strd r8, [sp, #8]
	    jit_pusharg_d(JIT_FPR1);	// ldrd r8, [fp, #-16]; strd r8, [sp]
	    jit_pusharg_d(JIT_FPR0);	// ldrd r8, [fp, #-8]; strd r8, [fp, #8]
	    jit_pusharg_i(JIT_R0);	// str r0, [fp]
	}
	jit_finish(printf);		// ldm fp, {r0, r1, r2, r3}, mov r0, #<q3>; orr r8, r8, #<q2>...; blx r8

	jit_subr_d(JIT_FPR2, JIT_FPR0, JIT_FPR1);	// push {r0, r1, r2, r3}; ldrd r0, [fp, #-8]; ldrd r0, [fp, #-16]; bl #<__aeabi_dsub>; strd r0, [fp, #-24]; pop {r0, r1, r2, r3}
	jit_movi_p(JIT_R0, "%f - %f = %f\n");	// mov r0, #<q0>, orr, r0, r0, #<q1>...
	jit_prepare(1);
	jit_prepare_d(3);
	{
	    jit_pusharg_d(JIT_FPR2);	// ldrd r8, [fp, #-24]; strd r8, [sp, #8]
	    jit_pusharg_d(JIT_FPR1);	// ldrd r8, [fp, #-16]; strd r8, [sp]
	    jit_pusharg_d(JIT_FPR0);	// ldrd r8, [fp, #-8]; strd r8, [fp, #8]
	    jit_pusharg_i(JIT_R0);	// str r0, [fp]
	}
	jit_finish(printf);		// ldm fp, {r0, r1, r2, r3}, mov r0, #<q3>; orr r8, r8, #<q2>...; blx r8

	jit_mulr_d(JIT_FPR2, JIT_FPR0, JIT_FPR1);	// push {r0, r1, r2, r3}; ldrd r0, [fp, #-8]; ldrd r0, [fp, #-16]; bl #<__aeabi_dmul>; strd r0, [fp, #-24]; pop {r0, r1, r2, r3}
	jit_movi_p(JIT_R0, "%f * %f = %f\n");	// mov r0, #<q0>, orr, r0, r0, #<q1>...
	jit_prepare(1);
	jit_prepare_d(3);
	{
	    jit_pusharg_d(JIT_FPR2);	// ldrd r8, [fp, #-24]; strd r8, [sp, #8]
	    jit_pusharg_d(JIT_FPR1);	// ldrd r8, [fp, #-16]; strd r8, [sp]
	    jit_pusharg_d(JIT_FPR0);	// ldrd r8, [fp, #-8]; strd r8, [fp, #8]
	    jit_pusharg_i(JIT_R0);	// str r0, [fp]
	}
	jit_finish(printf);		// ldm fp, {r0, r1, r2, r3}, mov r0, #<q3>; orr r8, r8, #<q2>...; blx r8

	jit_divr_d(JIT_FPR2, JIT_FPR0, JIT_FPR1);	// push {r0, r1, r2, r3}; ldrd r0, [fp, #-8]; ldrd r0, [fp, #-16]; bl #<__aeabi_ddiv>; strd r0, [fp, #-24]; pop {r0, r1, r2, r3}
	jit_movi_p(JIT_R0, "%f / %f = %f\n");	// mov r0, #<q0>, orr, r0, r0, #<q1>...
	jit_prepare(1);
	jit_prepare_d(3);
	{
	    jit_pusharg_d(JIT_FPR2);	// ldrd r8, [fp, #-24]; strd r8, [sp, #8]
	    jit_pusharg_d(JIT_FPR1);	// ldrd r8, [fp, #-16]; strd r8, [sp]
	    jit_pusharg_d(JIT_FPR0);	// ldrd r8, [fp, #-8]; strd r8, [fp, #8]
	    jit_pusharg_i(JIT_R0);	// str r0, [fp]
	}
	jit_finish(printf);		// ldm fp, {r0, r1, r2, r3}, mov r0, #<q3>; orr r8, r8, #<q2>...; blx r8

	jit_ret();			// add sp, fp, #16; pop {r4, r5, r6, r7, r8, r9, fp, pc}
    }
    jit_flush_code(buffer, jit_get_ip().ptr);
    ((void (*)(double,double))buffer)(1.5, 2.5);
#endif
    //disassemble(buffer, (long)jit_get_ip().ptr - (long)buffer);

    return (0);
}
