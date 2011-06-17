/*
  Temporary file for easier testing, should just include the proper
  headers... but self contained for now.
  
  Procedure for initial tests is a qemu image built using a recent
  (buildroot-2011.02 and/or buildroot-2011.05) http://buildroot.net/ 
  enabling development tools.
  
  Most of the opcodes binary values are being "decoded" from a comand like:
  # vi t.s; as t.s -o t; objdump -d t

  and this file tested as:
  # gcc arm.c -lopcodes -lbfd -liberty -lintl; ./a.out
  
  if small changes, just change arm.c in qemu, otherwise, run something like:
  
  $ sudo mount -o loop /home/pcpa/buildroot-2011.02/output/images/rootfs.ext2 /mnt; sudo cp lightning/arm/arm.c /mnt/root; sudo umount /mnt

  just ensure qemu is not running in the image, and before closing qemu:
  
  # halt

  qemu is run as:
  
  qemu-system-arm -M versatilepb -kernel /home/pcpa/buildroot-2011.02/output/images/zImage -drive file=/home/pcpa/buildroot-2011.02/output/images/rootfs.ext2 --append root=/dev/sda

  --
  documentation downloaded from:
  http://infocenter.arm.com/
  files being consulted:
  arm_inst.pdf  DUI0489C_arm_assembler_reference.pdf  QRC0001_UAL.pdf

  --
  For now, only implementing the instructions supported by all arm boards
  and ignoring thumb instructions.
  
  Idea is to detect model based on /proc/cpuinfo and then figure out if
  will implement division in jit or call external C function (e.g. require
  being linked to libgcc) for boards that do not provide it (armv5 or older)
  and same for float registers (probably should map to stack offsets)
 */

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dis-asm.h>

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

/**********************************************************************/

typedef union jit_code {	
  char		 *ptr;
  void		 (*vptr)(void);
  char		 (*cptr)(void);
  unsigned char	 (*ucptr)(void);
  short		 (*sptr)(void);
  unsigned short (*usptr)(void);
  int		 (*iptr)(void);
  unsigned int	 (*uiptr)(void);
  long		 (*lptr)(void);
  unsigned long	 (*ulptr)(void);
  void *	 (*pptr)(void);
  float		 (*fptr)(void);
  double	 (*dptr)(void);
} jit_code;

typedef unsigned char	jit_insn;
typedef struct jit_local_state {
    int		 framesize;
    int		 nextarg_get;
    int		 nextarg_put;
    int		 alloca_offset;
    int		 stack_length;
    int		 stack_offset;
    void	*stack;
} jit_local_state;
struct {
    unsigned	armvn		: 4;
    unsigned	armve		: 1;
    unsigned	thumb		: 2;
} jit_cpu;

typedef struct jit_state {
    union {
	jit_insn	*pc;
	unsigned char	*uc_pc;
	unsigned short	*us_pc;
	unsigned int	*ui_pc;
	unsigned long	*ul_pc;
    } x;
    struct jit_fp	*fp;
    jit_local_state	 jitl;
} jit_state_t[1];

jit_state_t		_jit;

#define _jitl			_jit->jitl
#define __jit_inline		inline
#define _jit_I(ii)		(*_jit->x.ui_pc++)= ii
#define _u4(n)			((n) & 0xf)
#define _u4P(n)			!((n) & ~0xf)
#define _u8(n)			((n) & 0xff)
#define _u8P(n)			!((n) & ~0xff)
#define _u12(n)			((n) & 0xfff)
#define _u12P(n)		!((n) & ~0xfff)
#define _u16(n)			((n) & 0xffff)
#define _u24(n)			((n) & 0xffffff)
#define _s24P(n)		((n) <= 0x7fffff && n >= -0x800000L)

#define	jit_get_ip()		(*(jit_code *) &_jit->x.pc)
#define	jit_set_ip(ptr)		(_jit->x.pc = (ptr), jit_get_ip ())
#define	jit_get_label()		(_jit->x.pc)
#define	jit_forward()		(_jit->x.pc)
#define jit_patch(pv)		jit_patch_at((pv), (_jit->x.pc))

/**********************************************************************/
static void
jit_flush_code(void *start, void *end)
{
    mprotect(start, (char *)end - (char *)start,
	     PROT_READ | PROT_WRITE | PROT_EXEC);
}

#define jit_get_cpu			jit_get_cpu
static void
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

/**********************************************************************/
typedef enum {
    _R0,	/* argument / result */
    _R1,	/* argument */
    _R2,	/* argument */
    _R3,	/* argument */
    _R4,	/* variable */
    _R5,	/* variable */
    _R6,	/* variable */
    _R7,	/* variable */
    _R8,	/* variable */
    _R9,	/* variable (real frame pointer) */
    _R10,	/* sl - stack limit */
    _R11,	/* fp - frame pointer */
    _R12,	/* ip - temporary */
    _R13,	/* sp - stack pointer */
    _R14,	/* lr - link register */
    _R15,	/* pc - program counter */
} jit_gpr_t;

typedef enum {
    _F0,	/* result */
    _F1,	/* scratch */
    _F2,	/* scratch */
    _F3,	/* scratch */
    _F4,	/* variable */
    _F5,	/* variable */
    _F6,	/* variable */
    _F7,	/* variable */
} jit_fpr_t;

#define JIT_PC		_R15
#define JIT_LR		_R14
#define JIT_SP		_R13
#define JIT_FP		_R11
#define JIT_TMP		_R8

#define ARM_CC_EQ	0x00000000	/* Z=1 */
#define ARM_CC_NE	0x10000000	/* Z=0 */
#define ARM_CC_HS	0x20000000	/* C=1 */
#define ARM_CC_LO	0x30000000	/* C=0 */
#define ARM_CC_MI	0x40000000	/* N=1 */
#define ARM_CC_PL	0x50000000	/* N=0 */
#define ARM_CC_VS	0x60000000	/* V=1 */
#define ARM_CC_VC	0x70000000	/* V=0 */
#define ARM_CC_HI	0x80000000	/* C=1 && Z=0 */
#define ARM_CC_LS	0x90000000	/* C=0 || Z=1 */
#define ARM_CC_GE	0xa0000000	/* (N=1 && V=1) || (N=0 && V=0) */
#define ARM_CC_LT	0xb0000000	/* (N=1 && V=0) || (N=0 && V=1) */
#define ARM_CC_GT	0xc0000000	/* Z=0 && ((N=1 && V=1) || (N=0 && V=1)) XXX */
#define ARM_CC_LE	0xd0000000	/* Z=1 || (N=1 && V=0) || (N=1 && V=0) */
#define ARM_CC_AL	0xe0000000	/* always */
#define ARM_CC_NV	0xf0000000	/* reserved */

#define ARM_MOV		0x01a00000
#define ARM_MVN		0x01e00000

#define ARM_I		0x02000000	/* immediate */
#define ARM_S		0x00100000	/* set flags */
#define ARM_ADD		0x00800000
#define ARM_ADC		0x00a00000
#define ARM_SUB		0x00400000
#define ARM_SBC		0x00c00000
#define ARM_RSB		0x00600000
#define ARM_RSC		0x00e00000	/* ARMV7M */
#define ARM_MUL		0x00000090
#define ARM_MLA		0x00200090
#define ARM_UMULL	0x00800090
#define ARM_UMLAL	0x00a00090
#define ARM_SMULL	0x00c00090
#define ARM_SMLAL	0x00e00090
#define ARM_AND		0x00000000
#define ARM_BIC		0x01c00000
#define ARM_ORR		0x01800000
#define ARM_EOR		0x00200000
#define ARM_ASR		0x01a00050
#define ARM_LSR		0x01a00030
#define ARM_LSL		0x01a00010
#define ARM_ROR		0x01a00070
#define ARM_ASRI	0x01a00040
#define ARM_LSRI	0x01a00020
#define ARM_LSLI	0x01a00000
#define ARM_RORI	0x01a00060

#define ARM_CMP		0x01500000
#define ARM_CMN		0x01700000
#define ARM_TST		0x01100000	/* THUMB */
#define ARM_TEQ		0x01300000	/* ARMV6T2 */

/* branch */
#define ARM_BX		0x012fff10
#define ARM_BLX		0x012fff30
#define ARM_B		0x0a000000
#define ARM_BL		0x0b000000

/* ldr/str */
#define ARM_P		0x00800000	/* positive offset */
#define ARM_LDRSB	0x011000d0
#define ARM_LDRSBI	0x015000d0
#define ARM_LDRB	0x07500000
#define ARM_LDRBI	0x05500000
#define ARM_LDRSH	0x011000f0
#define ARM_LDRSHI	0x015000f0
#define ARM_LDRH	0x011000b0
#define ARM_LDRHI	0x015000b0
#define ARM_LDR		0x07100000
#define ARM_LDRI	0x05100000
#define ARM_STRB	0x07400000
#define ARM_STRBI	0x05400000
#define ARM_STRH	0x010000b0
#define ARM_STRHI	0x014000b0
#define ARM_STR		0x07000000
#define ARM_STRI	0x05000000

/* ldm/stm */
#define ARM_M		0x08000000
#define ARM_M_L		0x00100000	/* load; store if not set */
#define ARM_M_I		0x00800000	/* inc; dec if not set */
#define ARM_M_B		0x01000000	/* before; after if not set */
#define ARM_M_U		0x00200000	/* update Rn */

/* from binutils */
#define rotate_left(v, n)	(v << n | v >> (32 - n))
static int
encode_arm_immediate(unsigned int v)
{
    unsigned int	a, i;

    for (i = 0; i < 32; i += 2)
	if ((a = rotate_left(v, i)) <= 0xff)
	    return (a | (i << 7));

    return (-1);
}

__jit_inline void
arm_cc_orrr(jit_state_t _jit, int cc, int o,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fff0f));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u4(r2));
}

__jit_inline void
arm_cc_orri(jit_state_t _jit, int cc, int o,
	    jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fffff));
    assert(!(i0 & 0xfffff000));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u12(i0));
}

__jit_inline void
arm_cc_orri8(jit_state_t _jit, int cc, int o,
	     jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fff0f));
    assert(!(i0 & 0xffffff00));
    _jit_I(cc|o|(_u4(r1)<<16)|(_u4(r0)<<12)|((i0&0xf0)<<4)|(i0&0x0f));
}

__jit_inline void
arm_cc_orrrr(jit_state_t _jit, int cc, int o,
	     jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, jit_gpr_t r3)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x000fff0f));
    _jit_I(cc|o|(_u4(r0)<<16)|(_u4(r1)<<12)|(_u4(r2)<<8)|_u4(r3));
}

__jit_inline void
arm_cc_srrr(jit_state_t _jit, int cc, int o,
	    jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0000ff0f));
    _jit_I(cc|o|(_u4(r0)<<12)|(_u4(r2)<<8)|_u4(r1));
}

__jit_inline void
arm_cc_srri(jit_state_t _jit, int cc,
	    int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0000ff0f));
    assert(i0 >= 1 && i0 <= 31);
    _jit_I(cc|o|(_u4(r0)<<12)|(i0<<7)|(_u4(r1)));
}

__jit_inline void
arm_cc_b(jit_state_t _jit, int cc, int o, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x00ffffff));
    _jit_I(cc|o|_u24(i0));
}

__jit_inline void
arm_cc_bx(jit_state_t _jit, int cc, int o, int r0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0000000f));
    _jit_I(cc|o|_u4(r0));
}

__jit_inline void
arm_cc_orl(jit_state_t _jit, int cc, int o, jit_gpr_t r0, int i0)
{
    assert(!(cc & 0x0fffffff));
    assert(!(o  & 0x0000ffff));
    _jit_I(cc|o|(_u4(r0)<<16)|_u16(i0));
}

#define _CC_MOV(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_MOV,r0,0,r1)
#define _MOV(r0,r1)		_CC_MOV(ARM_CC_AL,r0,r1)
#define _CC_MOVI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_MOV|ARM_I,r0,0,i0)
#define _MOVI(r0,i0)		_CC_MOVI(ARM_CC_AL,r0,i0)
#define _CC_MVN(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_MVN,r0,0,r1)
#define _MVN(r0, r1)		_CC_MVN(ARM_CC_AL,r0,r1)
#define _CC_MVNI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_MVN|ARM_I,r0,0,i0)
#define _MVNI(r0,i0)		_CC_MVNI(ARM_CC_AL,r0,i0)
#define _CC_NOT(cc,r0,r1)	_CC_MVN(cc,r0,r1)
#define _NOT(r0, r1)		_MVN(r0, r1)
#define _NOP()			_MOV(_R0, _R0)

#define _CC_ADD(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADD,r0,r1,r2)
#define _ADD(r0,r1,r2)		_CC_ADD(ARM_CC_AL,r0,r1,r2)
#define _CC_ADDI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADD|ARM_I,r0,r1,i0)
#define _ADDI(r0,r1,i0)		_CC_ADDI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADDS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADD|ARM_S,r0,r1,r2)
#define _ADDS(r0,r1,r2)		_CC_ADDS(ARM_CC_AL,r0,r1,r2)
#define _CC_ADDSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADD|ARM_S|ARM_I,r0,r1,i0)
#define _ADDSI(r0,r1,i0)	_CC_ADDSI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADC,r0,r1,r2)
#define _ADC(r0,r1,r2)		_CC_ADC(ARM_CC_AL,r0,r1,r2)
#define _CC_ADCI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADC|ARM_I,r0,r1,i0)
#define _ADCI(r0,r1,i0)		_CC_ADCI(ARM_CC_AL,r0,r1,i0)
#define _CC_ADCS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ADC|ARM_S,r0,r1,r2)
#define _ADCS(r0,r1,r2)		_CC_ADCS(ARM_CC_AL,r0,r1,r2)
#define _CC_ADCSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ADC|ARM_S|ARM_I,r0,r1,i0)
#define _ADCSI(r0,r1,i0)	_CC_ADCSI(ARM_CC_AL,r0,r1,i0)
#define _CC_SUB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SUB,r0,r1,r2)
#define _SUB(r0,r1,r2)		_CC_SUB(ARM_CC_AL,r0,r1,r2)
#define _CC_SUBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SUB|ARM_I,r0,r1,i0)
#define _SUBI(r0,r1,i0)		_CC_SUBI(ARM_CC_AL,r0,r1,i0)
#define _CC_SUBS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SUB|ARM_S,r0,r1,r2)
#define _SUBS(r0,r1,r2)		_CC_SUBS(ARM_CC_AL,r0,r1,r2)
#define _CC_SUBSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SUB|ARM_S|ARM_I,r0,r1,i0)
#define _SUBSI(r0,r1,i0)	_CC_SUBSI(ARM_CC_AL,r0,r1,i0)
#define _CC_SBC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SBC,r0,r1,r2)
#define _SBC(r0,r1,r2)		_CC_SBC(ARM_CC_AL,r0,r1,r2)
#define _CC_SBCI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SBC|ARM_I,r0,r1,i0)
#define _SBCI(r0,r1,i0)		_CC_SBCI(ARM_CC_AL,r0,r1,i0)
#define _CC_SBCS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_SBC|ARM_S,r0,r1,r2)
#define _SBCS(r0,r1,r2)		_CC_SBCS(ARM_CC_AL,r0,r1,r2)
#define _CC_SBCSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_SBC|ARM_S|ARM_I,r0,r1,i0)
#define _SBCSI(r0,r1,i0)	_CC_SBCSI(ARM_CC_AL,r0,r1,i0)
#define _CC_RSB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_RSB,r0,r1,r2)
#define _RSB(r0,r1,r2)		_CC_RSB(ARM_CC_AL,r0,r1,r2)
#define _CC_RSBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_RSB|ARM_I,r0,r1,i0)
#define _RSBI(r0,r1,i0)		_CC_RSBI(ARM_CC_AL,r0,r1,i0)

/* >> ARMV7M */
#define _CC_RSC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_RSC,r0,r1,r2)
#define _RSC(r0,r1,r2)		_CC_RSC(ARM_CC_AL,r0,r1,r2)
#define _CC_RSCI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_RSC|ARM_I,r0,r1,i0)
#define _RSCI(r0,r1,i0)		_CC_RSCI(ARM_CC_AL,r0,r1,i0)
/* << ARMV7M */

#define _CC_MUL(cc,r0,r1,r2)	  arm_cc_orrrr(_jit,cc,ARM_MUL,r0,0,r2,r1)
#define _MUL(r0,r1,r2)		  _CC_MUL(ARM_CC_AL,r0,r1,r2)
#define _CC_MLA(cc,r0,r1,r2,r3)	  arm_cc_orrrr(_jit,cc,ARM_MLA,r0,r3,r2,r1)
#define _MLA(r0,r1,r2,r3)	  _CC_MLA(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_UMULL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_UMULL,r1,r0,r3,r2)
#define _UMULL(r0,r1,r2,r3)	  _CC_UMULL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_UMLAL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_UMLAL,r1,r0,r3,r2)
#define _UMLAL(r0,r1,r2,r3)	  _CC_UMLAL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_SMULL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_SMULL,r1,r0,r3,r2)
#define _SMULL(r0,r1,r2,r3)	  _CC_SMULL(ARM_CC_AL,r0,r1,r2,r3)
#define _CC_SMLAL(cc,r0,r1,r2,r3) arm_cc_orrrr(_jit,cc,ARM_SMLAL,r1,r0,r3,r2)
#define _SMLAL(r0,r1,r2,r3)	  _CC_SMLAL(ARM_CC_AL,r0,r1,r2,r3)

#define _CC_AND(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_AND,r0,r1,r2)
#define _AND(r0,r1,r2)		_CC_AND(ARM_CC_AL,r0,r1,r2)
#define _CC_ANDI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_AND|ARM_I,r0,r1,i0)
#define _ANDI(r0,r1,i0)		_CC_ANDI(ARM_CC_AL,r0,r1,i0)
#define _CC_ANDS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_AND|ARM_S,r0,r1,r2)
#define _ANDS(r0,r1,r2)		_CC_ANDS(ARM_CC_AL,r0,r1,r2)
#define _CC_ANDSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_AND|ARM_S|ARM_I,r0,r1,i0)
#define _ANDSI(r0,r1,i0)	_CC_ANDSI(ARM_CC_AL,r0,r1,i0)
#define _CC_BIC(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_BIC,r0,r1,r2)
#define _BIC(r0,r1,r2)		_CC_BIC(ARM_CC_AL,r0,r1,r2)
#define _CC_BICI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_BIC|ARM_I,r0,r1,i0)
#define _BICI(r0,r1,i0)		_CC_BICI(ARM_CC_AL,r0,r1,i0)
#define _CC_BICS(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_BIC|ARM_S,r0,r1,r2)
#define _BICS(r0,r1,r2)		_CC_BICS(ARM_CC_AL,r0,r1,r2)
#define _CC_BICSI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_BIC|ARM_S|ARM_I,r0,r1,i0)
#define _BICSI(r0,r1,i0)	_CC_BICSI(ARM_CC_AL,r0,r1,i0)
#define _CC_OR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_ORR,r0,r1,r2)
#define _OR(r0,r1,r2)		_CC_OR(ARM_CC_AL,r0,r1,r2)
#define _CC_ORI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_ORR|ARM_I,r0,r1,i0)
#define _ORI(r0,r1,i0)		_CC_ORI(ARM_CC_AL,r0,r1,i0)
#define _CC_XOR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_EOR,r0,r1,r2)
#define _XOR(r0,r1,r2)		_CC_XOR(ARM_CC_AL,r0,r1,r2)
#define _CC_XORI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_EOR|ARM_I,r0,r1,i0)
#define _XORI(r0,r1,i0)		_CC_XORI(ARM_CC_AL,r0,r1,i0)
#define _CC_ASR(cc,r0,r1,r2)	arm_cc_srrr(_jit,cc,ARM_ASR,r0,r1,r2)
#define _ASR(r0,r1,r2)		_CC_ASR(ARM_CC_AL,r0,r1,r2)
#define _CC_ASRI(cc,r0,r1,i0)	arm_cc_srri(_jit,cc,ARM_ASRI,r0,r1,i0)
#define _ASRI(r0,r1,i0)		_CC_ASRI(ARM_CC_AL,r0,r1,i0)
#define _CC_LSR(cc,r0,r1,r2)	arm_cc_srrr(_jit,cc,ARM_LSR,r0,r1,r2)
#define _LSR(r0,r1,r2)		_CC_LSR(ARM_CC_AL,r0,r1,r2)
#define _CC_LSRI(cc,r0,r1,i0)	arm_cc_srri(_jit,cc,ARM_LSRI,r0,r1,i0)
#define _LSRI(r0,r1,i0)		_CC_LSRI(ARM_CC_AL,r0,r1,i0)
#define _CC_LSL(cc,r0,r1,r2)	arm_cc_srrr(_jit,cc,ARM_LSL,r0,r1,r2)
#define _LSL(r0,r1,r2)		_CC_LSL(ARM_CC_AL,r0,r1,r2)
#define _CC_LSLI(cc,r0,r1,i0)	arm_cc_srri(_jit,cc,ARM_LSLI,r0,r1,i0)
#define _LSLI(r0,r1,i0)		_CC_LSLI(ARM_CC_AL,r0,r1,i0)
#define _CC_ROR(cc,r0,r1,r2)	arm_cc_srrr(_jit,cc,ARM_ROR,r0,r1,r2)
#define _ROR(r0,r1,r2)		_CC_ROR(ARM_CC_AL,r0,r1,r2)
#define _CC_RORI(cc,r0,r1,i0)	arm_cc_srri(_jit,cc,ARM_RORI,r0,r1,i0)
#define _RORI(r0,r1,i0)		_CC_RORI(ARM_CC_AL,r0,r1,i0)

#define _CC_CMP(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_CMP,0,r0,r1)
#define _CMP(r0,r1)		_CC_CMP(ARM_CC_AL,r0,r1)
#define _CC_CMPI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_CMP|ARM_I,0,r0,i0)
#define _CMPI(r0,i0)		_CC_CMPI(ARM_CC_AL,r0,i0)
#define _CC_CMN(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_CMN,0,r0,r1)
#define _CMN(r0,r1)		_CC_CMN(ARM_CC_AL,r0,r1)
#define _CC_CMNI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_CMN|ARM_I,0,r0,i0)
#define _CMNI(r0,i0)		_CC_CMNI(ARM_CC_AL,r0,i0)

/* >> THUMB */
#define _CC_TST(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_TST,0,r0,r1)
#define _TST(r0,r1)		_CC_TST(ARM_CC_AL,r0,r1)
#define _CC_TSTI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_TST|ARM_I,0,r0,i0)
#define _TSTI(r0,i0)		_CC_TSTI(ARM_CC_AL,r0,i0)
/* << THUMB */

/* >> ARVM6T2 */
#define _CC_TEQ(cc,r0,r1)	arm_cc_orrr(_jit,cc,ARM_TEQ,0,r0,r1)
#define _TEQ(r0,r1)		_CC_TEQ(ARM_CC_AL,r0,r1)
#define _CC_TEQI(cc,r0,i0)	arm_cc_orri(_jit,cc,ARM_TEQ|ARM_I,0,r0,i0)
#define _TEQI(r0,i0)		_CC_TEQI(ARM_CC_AL,r0,i0)
/* << ARVM6T2 */

#define _CC_BX(cc,r0)		arm_cc_bx(_jit,cc,ARM_BX,r0)
#define _BX(i0)			_CC_BX(ARM_CC_AL,r0)
#define _CC_BLX(cc,r0)		arm_cc_bx(_jit,cc,ARM_BLX,r0)
#define _BLX(r0)		_CC_BLX(ARM_CC_AL,r0)
#define _CC_B(cc,i0)		arm_cc_b(_jit,cc,ARM_B,i0)
#define _B(i0)			_CC_B(ARM_CC_AL,i0)
#define _CC_BL(cc,i0)		arm_cc_b(_jit,cc,ARM_BL,i0)
#define _BL(i0)			_CC_BL(ARM_CC_AL,i0)

#define _CC_LDRSB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSB|ARM_P,r0,r1,r2)
#define _LDRSB(r0,r1,r2)	_CC_LDRSB(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSBN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSB,r0,r1,r2)
#define _LDRSBN(r0,r1,r2)	_CC_LDRSBN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSBI(cc,r0,r1,i0)	 arm_cc_orri8(_jit,cc,ARM_LDRSBI|ARM_P,r0,r1,i0)
#define _LDRSBI(r0,r1,i0)	 _CC_LDRSBI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSBIN(cc,r0,r1,i0) arm_cc_orri8(_jit,cc,ARM_LDRSBI,r0,r1,i0)
#define _LDRSBIN(r0,r1,i0)	 _CC_LDRSBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRB|ARM_P,r0,r1,r2)
#define _LDRB(r0,r1,r2)		_CC_LDRB(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRBN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRB,r0,r1,r2)
#define _LDRBN(r0,r1,r2)	_CC_LDRBN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRBI|ARM_P,r0,r1,i0)
#define _LDRBI(r0,r1,i0)	_CC_LDRBI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRBIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRBI,r0,r1,i0)
#define _LDRBIN(r0,r1,i0)	_CC_LDRBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSH(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSH|ARM_P,r0,r1,r2)
#define _LDRSH(r0,r1,r2)	_CC_LDRSH(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSHN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRSH,r0,r1,r2)
#define _LDRSHN(r0,r1,r2)	_CC_LDRSHN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRSHI(cc,r0,r1,i0)	 arm_cc_orri8(_jit,cc,ARM_LDRSHI|ARM_P,r0,r1,i0)
#define _LDRSHI(r0,r1,i0)	 _CC_LDRSHI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRSHIN(cc,r0,r1,i0) arm_cc_orri8(_jit,cc,ARM_LDRSHI,r0,r1,i0)
#define _LDRSHIN(r0,r1,i0)	 _CC_LDRSHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRH(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRH|ARM_P,r0,r1,r2)
#define _LDRH(r0,r1,r2)		_CC_LDRH(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRHN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDRH,r0,r1,r2)
#define _LDRHN(r0,r1,r2)	_CC_LDRHN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRHI(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_LDRHI|ARM_P,r0,r1,i0)
#define _LDRHI(r0,r1,i0)	_CC_LDRHI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRHIN(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_LDRHI,r0,r1,i0)
#define _LDRHIN(r0,r1,i0)	_CC_LDRHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_LDR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDR|ARM_P,r0,r1,r2)
#define _LDR(r0,r1,r2)		_CC_LDR(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_LDR,r0,r1,r2)
#define _LDRN(r0,r1,r2)		_CC_LDRN(ARM_CC_AL,r0,r1,r2)
#define _CC_LDRI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRI|ARM_P,r0,r1,i0)
#define _LDRI(r0,r1,i0)		_CC_LDRI(ARM_CC_AL,r0,r1,i0)
#define _CC_LDRIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_LDRI,r0,r1,i0)
#define _LDRIN(r0,r1,i0)	_CC_LDRIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRB(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRB|ARM_P,r2,r1,r0)
#define _STRB(r0,r1,r2)		_CC_STRB(ARM_CC_AL,r0,r1,r2)
#define _CC_STRBN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRB,r2,r1,r0)
#define _STRBN(r0,r1,r2)	_CC_STRBN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRBI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRBI|ARM_P,r1,r0,i0)
#define _STRBI(r0,r1,i0)	_CC_STRBI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRBIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRBI,r1,r0,i0)
#define _STRBIN(r0,r1,i0)	_CC_STRBIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STRH(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRH|ARM_P,r2,r1,r0)
#define _STRH(r0,r1,r2)		_CC_STRH(ARM_CC_AL,r0,r1,r2)
#define _CC_STRHN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STRH,r2,r1,r0)
#define _STRHN(r0,r1,r2)	_CC_STRHN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRHI(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_STRHI|ARM_P,r1,r0,i0)
#define _STRHI(r0,r1,i0)	_CC_STRHI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRHIN(cc,r0,r1,i0)	arm_cc_orri8(_jit,cc,ARM_STRHI,r1,r0,i0)
#define _STRHIN(r0,r1,i0)	_CC_STRHIN(ARM_CC_AL,r0,r1,i0)
#define _CC_STR(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STR|ARM_P,r2,r1,r0)
#define _STR(r0,r1,r2)		_CC_STR(ARM_CC_AL,r0,r1,r2)
#define _CC_STRN(cc,r0,r1,r2)	arm_cc_orrr(_jit,cc,ARM_STR,r2,r1,r0)
#define _STRN(r0,r1,r2)		_CC_STRN(ARM_CC_AL,r0,r1,r2)
#define _CC_STRI(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRI|ARM_P,r1,r0,i0)
#define _STRI(r0,r1,i0)		_CC_STRI(ARM_CC_AL,r0,r1,i0)
#define _CC_STRIN(cc,r0,r1,i0)	arm_cc_orri(_jit,cc,ARM_STRI,r1,r0,i0)
#define _STRIN(r0,r1,i0)	_CC_STRIN(ARM_CC_AL,r0,r1,i0)

#define _CC_LDMIA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I,r0,i0)
#define _LDMIA(r0,i0)		_CC_LDMIA(ARM_CC_AL,r0,i0)
#define _LDM(r0,i0)		_LDMIA(r0,i0)
#define _CC_LDMIA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_U,r0,i0)
#define _LDMIA_U(r0,i0)		_CC_LDMIA_U(ARM_CC_AL,r0,i0)
#define _LDM_U(r0,i0)		_LDMIA_U(r0,i0)
#define _CC_LDMIB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_B,r0,i0)
#define _LDMIB(r0,i0)		_CC_LDMIB(ARM_CC_AL,r0,i0)
#define _CC_LDMIB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_I|ARM_M_B|ARM_M_U,r0,i0)
#define _LDMIB_U(r0,i0)		_CC_LDMIB_U(ARM_CC_AL,r0,i0)
#define _CC_LDMDA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L,r0,i0)
#define _LDMDA(r0,i0)		_CC_LDMDA(ARM_CC_AL,r0,i0)
#define _CC_LDMDA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_U,r0,i0)
#define _LDMDA_U(r0,i0)		_CC_LDMDA_U(ARM_CC_AL,r0,i0)
#define _CC_LDMDB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_B,r0,i0)
#define _LDMDB(r0,i0)		_CC_LDMDB(ARM_CC_AL,r0,i0)
#define _CC_LDMDB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_L|ARM_M_B|ARM_M_U,r0,i0)
#define _LDMDB_U(r0,i0)		_CC_LDMDB_U(ARM_CC_AL,r0,i0)
#define _CC_STMIA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I,r0,i0)
#define _STMIA(r0,i0)		_CC_STMIA(ARM_CC_AL,r0,i0)
#define _STM(r0,i0)		_STMIA(r0,i0)
#define _CC_STMIA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I|ARM_M_U,r0,i0)
#define _STMIA_U(r0,i0)		_CC_STMIA_U(ARM_CC_AL,r0,i0)
#define _STM_U(r0,i0)		_STMIA_U(r0,i0)
#define _CC_STMIB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I|ARM_M_B,r0,i0)
#define _STMIB(r0,i0)		_CC_STMIB(ARM_CC_AL,r0,i0)
#define _CC_STMIB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_I|ARM_M_B|ARM_M_U,r0,i0)
#define _STMIB_U(r0,i0)		_CC_STMIB_U(ARM_CC_AL,r0,i0)
#define _CC_STMDA(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M,r0,i0)
#define _STMDA(r0,i0)		_CC_STMDA(ARM_CC_AL,r0,i0)
#define _CC_STMDA_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_U,r0,i0)
#define _STMDA_U(r0,i0)		_CC_STMDA_U(ARM_CC_AL,r0,i0)
#define _CC_STMDB(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_B,r0,i0)
#define _STMDB(r0,i0)		_CC_STMDB(ARM_CC_AL,r0,i0)
#define _CC_STMDB_U(cc,r0,i0)	arm_cc_orl(_jit,cc,ARM_M|ARM_M_B|ARM_M_U,r0,i0)
#define _STMDB_U(r0,i0)		_CC_STMDB_U(ARM_CC_AL,r0,i0)
#define _CC_PUSH(cc,i0)		_CC_STMDB_U(cc,JIT_SP,i0)
#define _PUSH(i0)		_CC_PUSH(ARM_CC_AL,i0)
#define _CC_POP(cc,i0)		_CC_LDMIA_U(cc,JIT_SP,i0)
#define _POP(i0)		_CC_POP(ARM_CC_AL,i0)

/**********************************************************************/
#define JIT_R_NUM			4
static const jit_gpr_t
jit_r_order[JIT_R_NUM] = {
    _R0, _R1, _R2, _R3
};
#define JIT_R(i)			jit_r_order[i]

#define JIT_V_NUM			4
static const jit_gpr_t
jit_v_order[JIT_V_NUM] = {
    _R4, _R5, _R6, _R7
};
#define JIT_V(i)			jit_v_order[i]

#define JIT_R0				JIT_R(0)
#define JIT_R1				JIT_R(1)
#define JIT_R2				JIT_R(2)
#define JIT_V0				JIT_V(0)
#define JIT_V1				JIT_V(1)
#define JIT_V2				JIT_V(2)

#define jit_nop(n)			arm_nop(_jit, n)
__jit_inline void
arm_nop(jit_state_t _jit, int n)
{
    assert(n >= 0);
    while (n--)
	_NOP();
}

#define jit_movr_i(r0, r1)		arm_movr_i(_jit, r0, r1)
__jit_inline void
arm_movr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    if (r0 != r1)
	_MOV(r0, r1);
}

#define jit_movi_i(r0, i0)		arm_movi_i(_jit, r0, i0)
__jit_inline void
arm_movi_i(jit_state_t _jit, jit_gpr_t r0, int i0)
{
    int		i;
    if ((i = encode_arm_immediate(i0)) != -1)
	_MOVI(r0, i);
    else if ((i = encode_arm_immediate(~i0)) != -1)
	_MVNI(r0, i);
    else {
	int	p0, p1, p2, p3, q0, q1, q2, q3;
	p0 = i0 & 0x000000ff;	p1 = i0 & 0x0000ff00;
	p2 = i0 & 0x00ff0000;	p3 = i0 & 0xff000000;
	i0 = ~i0;
	q0 = i0 & 0x000000ff;	q1 = i0 & 0x0000ff00;
	q2 = i0 & 0x00ff0000;	q3 = i0 & 0xff000000;
	if (!!p0 + !!p1 + !!p2 + !!p3 <= !!q0 + !!q1 + !!q2 + !!q3) {
	    /* prefer no inversion on tie */
	    if (p3) {
		_MOVI(r0, encode_arm_immediate(p3));
		if (p2)	_ORI(r0, r0, encode_arm_immediate(p2));
		if (p1)	_ORI(r0, r0, encode_arm_immediate(p1));
		if (p0)	_ORI(r0, r0, p0);
	    }
	    else if (p2) {
		_MOVI(r0, encode_arm_immediate(p2));
		if (p1)	_ORI(r0, r0, encode_arm_immediate(p1));
		if (p0)	_ORI(r0, r0, p0);
	    }
	    else {
		_MOVI(r0, encode_arm_immediate(p1));
		_ORI(r0, r0, p0);
	    }
	}
	else {
	    if (q3) {
		_MVNI(r0, encode_arm_immediate(q3));
		if (q2)	_XORI(r0, r0, encode_arm_immediate(q2));
		if (q1)	_XORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_XORI(r0, r0, q0);
	    }
	    else if (q2) {
		_MVNI(r0, encode_arm_immediate(q2));
		if (q1)	_XORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_XORI(r0, r0, q0);
	    }
	    else {
		_MVNI(r0, encode_arm_immediate(q1));
		_XORI(r0, r0, q0);
	    }
	}
    }
}

#define jit_movi_p(r0, i0)		arm_movi_p(_jit, r0, i0)
__jit_inline jit_insn *
arm_movi_p(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_insn	*l;
    int		 im, q0, q1, q2, q3;
    im = (int)i0; 		l = _jit->x.pc;
    q0 = im & 0x000000ff;	q1 = im & 0x0000ff00;
    q2 = im & 0x00ff0000;	q3 = im & 0xff000000;
    _MOVI(r0, encode_arm_immediate(q3));
    _ORI(r0, r0, encode_arm_immediate(q2));
    _ORI(r0, r0, encode_arm_immediate(q1));
    _ORI(r0, r0, q0);
    return (l);
}

#define jit_patch_movi(i0, i1)		arm_patch_movi(_jit, i0, i1)
__jit_inline void
arm_patch_movi(jit_state_t _jit, jit_insn *i0, void *i1)
{
    union {
	int		*i;
	void		*v;
    } u;
    int			 im, q0, q1, q2, q3;
    im = (int)i1;		u.v = i0;
    q0 = im & 0x000000ff;	q1 = im & 0x0000ff00;
    q2 = im & 0x00ff0000;	q3 = im & 0xff000000;
    assert(  (u.i[0] & 0x0ff00000) == (ARM_MOV|ARM_I));
    u.i[0] = (u.i[0] & 0xfffff000) | encode_arm_immediate(q3);
    assert(  (u.i[1] & 0x0ff00000) == (ARM_ORR|ARM_I));
    u.i[1] = (u.i[1] & 0xfffff000) | encode_arm_immediate(q2);
    assert(  (u.i[2] & 0x0ff00000) == (ARM_ORR|ARM_I));
    u.i[2] = (u.i[2] & 0xfffff000) | encode_arm_immediate(q1);
    assert(  (u.i[3] & 0x0ff00000) == (ARM_ORR|ARM_I));
    u.i[3] = (u.i[3] & 0xfffff000) | encode_arm_immediate(q0);
}

#define jit_patch_calli(i0, i1)		arm_patch_at(_jit, i0, i1)
#define jit_patch_at(jump, label)	arm_patch_at(_jit, jump, label)
__jit_inline void
arm_patch_at(jit_state_t _jit, jit_insn *jump, jit_insn *label)
{
    long		 d;
    union {
	int		*i;
	void		*v;
    } u;
    u.v = jump;
    /* 0x0e000000 because 0x01000000 is (branch&) link modifier */
    if ((u.i[0] & 0x0e000000) == ARM_B) {
	d = (((long)label - (long)jump) >> 2) - 2;
	assert(_s24P(d));
	u.i[0] = (u.i[0] & 0xff000000) | (d & 0x00ffffff);
    }
    else if ((u.i[0] & 0x0ff00000) == ARM_MOV|ARM_I)
	jit_patch_movi(jump, label);
    else
	assert(!"unhandled branch opcode");
}

#define jit_notr_i(r0, r1)		arm_notr_i(_jit, r0, r1)
__jit_inline void
arm_notr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _NOT(r0, r1);
}

#define jit_negr_i(r0, r1)		arm_negr_i(_jit, r0, r1)
__jit_inline void
arm_negr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _RSBI(r0, r1, 0);
}

#define jit_addr_i(r0, r1, r2)		arm_addr_i(_jit, r0, r1, r2)
__jit_inline void
arm_addr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADD(r0, r1, r2);
}

#define jit_addi_i(r0, r1, i0)		arm_addi_i(_jit, r0, r1, i0)
__jit_inline void
arm_addi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ADDI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_SUBI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_ADD(r0, r1, reg);
    }
}

#define jit_addcr_ui(r0, r1, r2)	arm_addcr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_addcr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADDS(r0, r1, r2);
}

#define jit_addci_ui(r0, r1, i0)	arm_addci_ui(_jit, r0, r1, i0)
__jit_inline void
arm_addci_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ADDSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_SUBSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_ADDS(r0, r1, reg);
    }
}

#define jit_addxr_ui(r0, r1, r2)	arm_addxr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_addxr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* keep setting carry because don't know last ADC */
    _ADCS(r0, r1, r2);
}

#define jit_addxi_ui(r0, r1, i0)	arm_addxi_ui(_jit, r0, r1, i0)
__jit_inline void
arm_addxi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ADCSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_SBCSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_ADCS(r0, r1, reg);
    }
}

#define jit_subr_i(r0, r1, r2)		arm_subr_i(_jit, r0, r1, r2)
__jit_inline void
arm_subr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SUB(r0, r1, r2);
}

#define jit_subi_i(r0, r1, i0)		arm_subi_i(_jit, r0, r1, i0)
__jit_inline void
arm_subi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SUBI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADDI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SUB(r0, r1, reg);
    }
}

#define jit_subcr_ui(r0, r1, r2)	arm_subcr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_subcr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SUBS(r0, r1, r2);
}

#define jit_subci_ui(r0, r1, i0)	arm_subci_ui(_jit, r0, r1, i0)
__jit_inline void
arm_subci_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SUBSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADDSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SUBS(r0, r1, reg);
    }
}

#define jit_subxr_ui(r0, r1, r2)	arm_subxr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_subxr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    /* keep setting carry because don't know last SBC */
    _SBCS(r0, r1, r2);
}

#define jit_subxi_ui(r0, r1, i0)	arm_subxi_ui(_jit, r0, r1, i0)
__jit_inline void
arm_subxi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SBCSI(r0, r1, i);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADCSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SBCS(r0, r1, reg);
    }
}

#define jit_rsbr_i(r0, r1, r2)		arm_rsbr_i(_jit, r0, r1, r2)
__jit_inline void
arm_rsbr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _RSB(r0, r1, r2);
}

#define jit_rsbi_i(r0, r1, i0)		arm_rsbi_i(_jit, r0, r1, i0)
__jit_inline void
arm_rsbi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_RSBI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_RSB(r0, r1, reg);
    }
}

#define jit_mulr_i(r0, r1, r2)		arm_mulr_i(_jit, r0, r1, r2)
__jit_inline void
arm_mulr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1 && jit_cpu.armvn < 6) {
	_MOV(JIT_TMP, r1);
	_MUL(r0, JIT_TMP, r2);
    }
    else
	_MUL(r0, r1, r2);
}

#define jit_muli_i(r0, r1, i0)		arm_muli_i(_jit, r0, r1, i0)
__jit_inline void
arm_muli_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    reg = r0 != r1 ? r0 : JIT_TMP;
    jit_movi_i(reg, i0);
    if (r0 == r1 && jit_cpu.armvn < 6)
	_MUL(r0, reg, r1);
    else
	_MUL(r0, r1, reg);
}

#define jit_hmulr_i(r0, r1, r2)		arm_hmulr_i(_jit, r0, r1, r2)
__jit_inline void
arm_hmulr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1 && jit_cpu.armvn < 6) {
	assert(r2 != JIT_TMP);
	_SMULL(JIT_TMP, r0, r2, r1);
    }
    else
	_SMULL(JIT_TMP, r0, r1, r2);
}

#define jit_hmuli_i(r0, r1, i0)		arm_hmuli_i(_jit, r0, r1, i0)
__jit_inline void
arm_hmuli_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t	reg;
    if (r0 != r1 || jit_cpu.armvn >= 6) {
	jit_movi_i(JIT_TMP, i0);
	_SMULL(JIT_TMP, r0, r1, JIT_TMP);
    }
    else {
	if (r0 != _R0)		reg = _R0;
	else if (r0 != _R1)	reg = _R1;
	else if (r0 != _R2)	reg = _R2;
	else			reg = _R3;
	_PUSH(1<<reg);
	jit_movi_i(reg, i0);
	_SMULL(JIT_TMP, r0, r1, reg);
	_POP(1<<reg);
    }
}

#define jit_hmulr_ui(r0, r1, r2)	arm_hmulr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_hmulr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    if (r0 == r1 && jit_cpu.armvn < 6) {
	assert(r2 != JIT_TMP);
	_UMULL(JIT_TMP, r0, r2, r1);
    }
    else
	_UMULL(JIT_TMP, r0, r1, r2);
}

#define jit_hmuli_ui(r0, r1, i0)	arm_hmuli_ui(_jit, r0, r1, i0)
__jit_inline void
arm_hmuli_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t	reg;
    if (r0 != r1 || jit_cpu.armvn >= 6) {
	jit_movi_i(JIT_TMP, i0);
	_UMULL(JIT_TMP, r0, r1, JIT_TMP);
    }
    else {
	if (r0 != _R0)		reg = _R0;
	else if (r0 != _R1)	reg = _R1;
	else if (r0 != _R2)	reg = _R2;
	else			reg = _R3;
	_PUSH(1<<reg);
	jit_movi_i(reg, i0);
	_UMULL(JIT_TMP, r0, r1, reg);
	_POP(1<<reg);
    }
}

#define jit_andr_i(r0, r1, r2)		arm_andr_i(_jit, r0, r1, r2)
__jit_inline void
arm_andr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _AND(r0, r1, r2);
}

#define jit_andi_i(r0, r1, i0)		arm_andi_i(_jit, r0, r1, i0)
__jit_inline void
arm_andi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ANDI(r0, r1, i0);
    else if ((i = encode_arm_immediate(~i0)) != -1)
	_BICI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_AND(r0, r1, reg);
    }
}

#define jit_orr_i(r0, r1, r2)		arm_orr_i(_jit, r0, r1, r2)
__jit_inline void
arm_orr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _OR(r0, r1, r2);
}

#define jit_ori_i(r0, r1, i0)		arm_ori_i(_jit, r0, r1, i0)
__jit_inline void
arm_ori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_ORI(r0, r1, i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_OR(r0, r1, reg);
    }
}

#define jit_xorr_i(r0, r1, r2)		arm_xorr_i(_jit, r0, r1, r2)
__jit_inline void
arm_xorr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _XOR(r0, r1, r2);
}

#define jit_xori_i(r0, r1, i0)		arm_xori_i(_jit, r0, r1, i0)
__jit_inline void
arm_xori_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_XORI(r0, r1, i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_XOR(r0, r1, reg);
    }
}

#define jit_lshr_i(r0, r1, r2)		arm_lshr_i(_jit, r0, r1, r2)
__jit_inline void
arm_lshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LSL(r0, r1, r2);
}

#define jit_lshi_i(r0, r1, i0)		arm_lshi_i(_jit, r0, r1, i0)
__jit_inline void
arm_lshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 0 && i0 <= 31);
    _LSLI(r0, r1, i0);
}

#define jit_rshr_i(r0, r1, r2)		arm_rshr_i(_jit, r0, r1, r2)
__jit_inline void
arm_rshr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ASR(r0, r1, r2);
}

#define jit_rshi_i(r0, r1, i0)		arm_rshi_i(_jit, r0, r1, i0)
__jit_inline void
arm_rshi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 1 && i0 <= 32);
    _ASRI(r0, r1, i0);
}

#define jit_rshr_ui(r0, r1, r2)		arm_rshr_ui(_jit, r0, r1, r2)
__jit_inline void
arm_rshr_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LSR(r0, r1, r2);
}

#define jit_rshi_ui(r0, r1, i0)		arm_rshi_ui(_jit, r0, r1, i0)
__jit_inline void
arm_rshi_ui(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 1 && i0 <= 32);
    _LSRI(r0, r1, i0);
}

__jit_inline void
arm_ccr(jit_state_t _jit, int cct, int ccf,
	jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _CMP(r1, r2);
    _CC_MOVI(cct, r0, 1);
    _CC_MOVI(ccf, r0, 0);
}
__jit_inline void
arm_cci(jit_state_t _jit, int cct, int ccf,
	jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_CMPI(r1, i0);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_CMNI(r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_CMP(r1, reg);
    }
    _CC_MOVI(cct, r0, 1);
    _CC_MOVI(ccf, r0, 0);
}
#define jit_ltr_i(r0, r1, r2)	arm_ccr(_jit,ARM_CC_LT,ARM_CC_GE,r0,r1,r2)
#define jit_lti_i(r0, r1, i0)	arm_cci(_jit,ARM_CC_LT,ARM_CC_GE,r0,r1,i0)
#define jit_ltr_ui(r0, r1, r2)	arm_ccr(_jit,ARM_CC_LO,ARM_CC_HS,r0,r1,r2)
#define jit_lti_ui(r0, r1, i0)	arm_cci(_jit,ARM_CC_LO,ARM_CC_HS,r0,r1,i0)
#define jit_ler_i(r0, r1, r2)	arm_ccr(_jit,ARM_CC_LE,ARM_CC_GT,r0,r1,r2)
#define jit_lei_i(r0, r1, i0)	arm_cci(_jit,ARM_CC_LE,ARM_CC_GT,r0,r1,i0)
#define jit_ler_ui(r0, r1, r2)	arm_ccr(_jit,ARM_CC_LS,ARM_CC_HI,r0,r1,r2)
#define jit_lei_ui(r0, r1, i0)	arm_cci(_jit,ARM_CC_LS,ARM_CC_HI,r0,r1,i0)
#define jit_eqr_i(r0, r1, r2)	arm_ccr(_jit,ARM_CC_EQ,ARM_CC_NE,r0,r1,r2)
#define jit_eqi_i(r0, r1, i0)	arm_cci(_jit,ARM_CC_EQ,ARM_CC_NE,r0,r1,i0)
#define jit_ger_i(r0, r1, r2)	arm_ccr(_jit,ARM_CC_GE,ARM_CC_LT,r0,r1,r2)
#define jit_gei_i(r0, r1, i0)	arm_cci(_jit,ARM_CC_GE,ARM_CC_LT,r0,r1,i0)
#define jit_ger_ui(r0, r1, r2)	arm_ccr(_jit,ARM_CC_HS,ARM_CC_LO,r0,r1,r2)
#define jit_gei_ui(r0, r1, i0)	arm_cci(_jit,ARM_CC_HS,ARM_CC_LO,r0,r1,i0)
#define jit_gtr_i(r0, r1, r2)	arm_ccr(_jit,ARM_CC_GT,ARM_CC_LE,r0,r1,r2)
#define jit_gti_i(r0, r1, i0)	arm_cci(_jit,ARM_CC_GT,ARM_CC_LE,r0,r1,i0)
#define jit_gtr_ui(r0, r1, r2)	arm_ccr(_jit,ARM_CC_HI,ARM_CC_LS,r0,r1,r2)
#define jit_gti_ui(r0, r1, i0)	arm_cci(_jit,ARM_CC_HI,ARM_CC_LS,r0,r1,i0)

#define jit_ner_i(r0, r1, r2)		arm_ner_i(_jit, r0, r1, r2)
__jit_inline void
arm_ner_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _SUBS(r0, r1, r2);
    _CC_MOVI(ARM_CC_NE, r0, 1);
}

#define jit_nei_i(r0, r1, i0)		arm_nei_i(_jit, r0, r1, i0)
__jit_inline void
arm_nei_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    int		i;
    jit_gpr_t	reg;
    if ((i = encode_arm_immediate(i0)) != -1)
	_SUBSI(r0, r1, i0);
    else if ((i = encode_arm_immediate(-i0)) != -1)
	_ADDSI(r0, r1, i);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_SUBS(r0, r1, reg);
    }
    _CC_MOVI(ARM_CC_NE, r0, 1);
}

#define jit_jmpr(r0)			arm_jmpr(_jit, r0)
__jit_inline void
arm_jmpr(jit_state_t _jit, jit_gpr_t r0)
{
    _MOV(_R15, r0);
}

#define jit_jmpi(i0)			arm_jmpi(_jit, i0)
__jit_inline jit_insn *
arm_jmpi(jit_state_t _jit, void *i0)
{
    jit_insn	*l;
    l = _jit->x.pc;
    jit_movi_p(JIT_TMP, i0);
    jit_jmpr(JIT_TMP);
    return (l);
}

__jit_inline jit_insn *
arm_bccr(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _CMP(r0, r1);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
__jit_inline jit_insn *
arm_bcci(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_insn	*l;
    long	 d;
    int		 i;
    if ((i = encode_arm_immediate(i1)) != -1)
	_CMPI(r0, i);
    else if ((i = encode_arm_immediate(-i1)) != -1)
	_CMNI(r0, i);
    else {
	jit_movi_i(JIT_TMP, i1);
	_CMP(r0, JIT_TMP);
    }
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
#define jit_bltr_i(i0, r0, r1)		arm_bccr(_jit, ARM_CC_LT, i0, r0, r1)
#define jit_blti_i(i0, r0, i1)		arm_bcci(_jit, ARM_CC_LT, i0, r0, i1)
#define jit_bltr_ui(i0, r0, r1)		arm_bccr(_jit, ARM_CC_LO, i0, r0, r1)
#define jit_blti_ui(i0, r0, i1)		arm_bcci(_jit, ARM_CC_LO, i0, r0, i1)
#define jit_bler_i(i0, r0, r1)		arm_bccr(_jit, ARM_CC_LE, i0, r0, r1)
#define jit_blei_i(i0, r0, i1)		arm_bcci(_jit, ARM_CC_LE, i0, r0, i1)
#define jit_bler_ui(i0, r0, r1)		arm_bccr(_jit, ARM_CC_LS, i0, r0, r1)
#define jit_blei_ui(i0, r0, i1)		arm_bcci(_jit, ARM_CC_LS, i0, r0, i1)
#define jit_beqr_i(i0, r0, r1)		arm_bccr(_jit, ARM_CC_EQ, i0, r0, r1)
#define jit_beqi_i(i0, r0, i1)		arm_bcci(_jit, ARM_CC_EQ, i0, r0, i1)
#define jit_bger_i(i0, r0, r1)		arm_bccr(_jit, ARM_CC_GE, i0, r0, r1)
#define jit_bgei_i(i0, r0, i1)		arm_bcci(_jit, ARM_CC_GE, i0, r0, i1)
#define jit_bger_ui(i0, r0, r1)		arm_bccr(_jit, ARM_CC_HS, i0, r0, r1)
#define jit_bgei_ui(i0, r0, i1)		arm_bcci(_jit, ARM_CC_HS, i0, r0, i1)
#define jit_bgtr_i(i0, r0, r1)		arm_bccr(_jit, ARM_CC_GT, i0, r0, r1)
#define jit_bgti_i(i0, r0, i1)		arm_bcci(_jit, ARM_CC_GT, i0, r0, i1)
#define jit_bgtr_ui(i0, r0, r1)		arm_bccr(_jit, ARM_CC_HI, i0, r0, r1)
#define jit_bgti_ui(i0, r0, i1)		arm_bcci(_jit, ARM_CC_HI, i0, r0, i1)
#define jit_bner_i(i0, r0, r1)		arm_bccr(_jit, ARM_CC_NE, i0, r0, r1)
#define jit_bnei_i(i0, r0, i1)		arm_bcci(_jit, ARM_CC_NE, i0, r0, i1)

__jit_inline jit_insn *
arm_baddr(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _ADDS(r0, r0, r1);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
__jit_inline jit_insn *
arm_baddi(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_insn	*l;
    long	 d;
    int		 i;
    if ((i = encode_arm_immediate(i1)) != -1)
	_ADDSI(r0, r0, i);
    else if ((i = encode_arm_immediate(-i1)) != -1)
	_SUBSI(r0, r0, i);
    else {
	jit_movi_i(JIT_TMP, i1);
	_ADDS(r0, r0, JIT_TMP);
    }
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
#define jit_boaddr_i(i0, r0, r1)	arm_baddr(_jit, ARM_CC_VS, i0, r0, r1)
#define jit_boaddi_i(i0, r0, i1)	arm_baddi(_jit, ARM_CC_VS, i0, r0, i1)
#define jit_boaddr_ui(i0, r0, r1)	arm_baddr(_jit, ARM_CC_HS, i0, r0, r1)
#define jit_boaddi_ui(i0, r0, i1)	arm_baddi(_jit, ARM_CC_HS, i0, r0, i1)

__jit_inline jit_insn *
arm_bsubr(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _SUBS(r0, r0, r1);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
__jit_inline jit_insn *
arm_bsubi(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_insn	*l;
    long	 d;
    int		 i;
    if ((i = encode_arm_immediate(i1)) != -1)
	_SUBSI(r0, r0, i);
    else if ((i = encode_arm_immediate(-i1)) != -1)
	_ADDSI(r0, r0, i);
    else {
	jit_movi_i(JIT_TMP, i1);
	_SUBS(r0, r0, JIT_TMP);
    }
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
#define jit_bosubr_i(i0, r0, r1)	arm_bsubr(_jit, ARM_CC_VS, i0, r0, r1)
#define jit_bosubi_i(i0, r0, i1)	arm_bsubi(_jit, ARM_CC_VS, i0, r0, i1)
#define jit_bosubr_ui(i0, r0, r1)	arm_bsubr(_jit, ARM_CC_HS, i0, r0, r1)
#define jit_bosubi_ui(i0, r0, i1)	arm_bsubi(_jit, ARM_CC_HS, i0, r0, i1)

__jit_inline jit_insn *
arm_bmxr(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_insn	*l;
    long	 d;
    if (jit_cpu.thumb)
	_TST(r0, r1);
    else
	_ANDS(JIT_TMP, r0, r1);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
__jit_inline jit_insn *
arm_bmxi(jit_state_t _jit, int cc, jit_insn *i0, jit_gpr_t r0, int i1)
{
    jit_insn	*l;
    long	 d;
    int		 i;
    if (jit_cpu.thumb) {
	if ((i = encode_arm_immediate(i1)) != -1)
	    _TSTI(r0, i);
	else if (jit_cpu.armvn >= 6 && (i = encode_arm_immediate(~i1)) != -1)
	    _TEQI(r0, i);
	else {
	    jit_movi_i(JIT_TMP, i1);
	    _TST(r0, JIT_TMP);
	}
    }
    else {
	if ((i = encode_arm_immediate(i1)) != -1)
	    _ANDSI(JIT_TMP, r0, i);
	else if ((i = encode_arm_immediate(~i1)) != -1)
	    _BICSI(JIT_TMP, r0, i);
	else {
	    jit_movi_i(JIT_TMP, i1);
	    _ANDS(JIT_TMP, r0, JIT_TMP);
	}
    }
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(cc, d & 0x00ffffff);
    return (l);
}
#define jit_bmsr_i(i0, r0, r1)		arm_bmxr(_jit, ARM_CC_EQ, i0, r0, r1)
#define jit_bmsi_i(i0, r0, i1)		arm_bmxi(_jit, ARM_CC_EQ, i0, r0, i1)
#define jit_bmcr_i(i0, r0, r1)		arm_bmxr(_jit, ARM_CC_NE, i0, r0, r1)
#define jit_bmci_i(i0, r0, i1)		arm_bmxi(_jit, ARM_CC_NE, i0, r0, i1)

#define jit_ldr_c(r0, r1)		arm_ldr_c(_jit, r0, r1)
__jit_inline void
arm_ldr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LDRSBI(r0, r1, 0);
}

#define jit_ldi_c(r0, i0)		arm_ldi_c(_jit, r0, i0)
__jit_inline void
arm_ldi_c(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRSBI(r0, JIT_TMP, 0);
}

#define jit_ldxr_c(r0, r1, r2)		arm_ldxr_c(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRSB(r0, r1, r2);
}

#define jit_ldxi_c(r0, r1, i0)		arm_ldxi_c(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRSBI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRSBIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRSB(r0, r1, reg);
    }
}

#define jit_ldr_uc(r0, r1)		arm_ldr_uc(_jit, r0, r1)
__jit_inline void
arm_ldr_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LDRBI(r0, r1, 0);
}

#define jit_ldi_uc(r0, i0)		arm_ldi_uc(_jit, r0, i0)
__jit_inline void
arm_ldi_uc(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRBI(r0, JIT_TMP, 0);
}

#define jit_ldxr_uc(r0, r1, r2)		arm_ldxr_uc(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRB(r0, r1, r2);
}

#define jit_ldxi_uc(r0, r1, i0)		arm_ldxi_uc(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_uc(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_LDRBI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_LDRBIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRB(r0, r1, reg);
    }
}

#define jit_ldr_s(r0, r1)		arm_ldr_s(_jit, r0, r1)
__jit_inline void
arm_ldr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LDRSHI(r0, r1, 0);
}

#define jit_ldi_s(r0, i0)		arm_ldi_s(_jit, r0, i0)
__jit_inline void
arm_ldi_s(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRSHI(r0, JIT_TMP, 0);
}

#define jit_ldxr_s(r0, r1, r2)		arm_ldxr_s(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRSH(r0, r1, r2);
}

#define jit_ldxi_s(r0, r1, i0)		arm_ldxi_s(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRSHI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRSHIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRSH(r0, r1, reg);
    }
}

#define jit_ldr_us(r0, r1)		arm_ldr_us(_jit, r0, r1)
__jit_inline void
arm_ldr_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LDRHI(r0, r1, 0);
}

#define jit_ldi_us(r0, i0)		arm_ldi_us(_jit, r0, i0)
__jit_inline void
arm_ldi_us(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRHI(r0, JIT_TMP, 0);
}

#define jit_ldxr_us(r0, r1, r2)		arm_ldxr_us(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDRH(r0, r1, r2);
}

#define jit_ldxi_us(r0, r1, i0)		arm_ldxi_us(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_us(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_LDRHI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_LDRHIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDRH(r0, r1, reg);
    }
}

#define jit_ldr_i(r0, r1)		arm_ldr_i(_jit, r0, r1)
__jit_inline void
arm_ldr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _LDRI(r0, r1, 0);
}

#define jit_ldi_i(r0, i0)		arm_ldi_i(_jit, r0, i0)
__jit_inline void
arm_ldi_i(jit_state_t _jit, jit_gpr_t r0, void *i0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _LDRI(r0, JIT_TMP, 0);
}

#define jit_ldxr_i(r0, r1, r2)		arm_ldxr_i(_jit, r0, r1, r2)
__jit_inline void
arm_ldxr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _LDR(r0, r1, r2);
}

#define jit_ldxi_i(r0, r1, i0)		arm_ldxi_i(_jit, r0, r1, i0)
__jit_inline void
arm_ldxi_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_LDRI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_LDRIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_LDR(r0, r1, reg);
    }
}

#define jit_str_c(r0, r1)		arm_str_c(_jit, r0, r1)
__jit_inline void
arm_str_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _STRBI(r0, r1, 0);
}

#define jit_sti_c(r0, i0)		arm_sti_c(_jit, r0, i0)
__jit_inline void
arm_sti_c(jit_state_t _jit, void *i0, jit_gpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _STRBI(JIT_TMP, r0, 0);
}

#define jit_stxr_c(r0, r1, r2)		arm_stxr_c(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_c(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STRB(r0, r1, r2);
}

#define jit_stxi_c(r0, r1, i0)		arm_stxi_c(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_c(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_STRBI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_STRBIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRB(r0, r1, reg);
    }
}

#define jit_str_s(r0, r1)		arm_str_s(_jit, r0, r1)
__jit_inline void
arm_str_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _STRHI(r0, r1, 0);
}

#define jit_sti_s(r0, i0)		arm_sti_s(_jit, r0, i0)
__jit_inline void
arm_sti_s(jit_state_t _jit, void *i0, jit_gpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _STRHI(JIT_TMP, r0, 0);
}

#define jit_stxr_s(r0, r1, r2)		arm_stxr_s(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_s(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STRH(r0, r1, r2);
}

#define jit_stxi_s(r0, r1, i0)		arm_stxi_s(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_s(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 255)
	_STRHI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -255)
	_STRHIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STRH(r0, r1, reg);
    }
}

#define jit_str_i(r0, r1)		arm_str_i(_jit, r0, r1)
__jit_inline void
arm_str_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _STRI(r0, r1, 0);
}

#define jit_sti_i(r0, i0)		arm_sti_i(_jit, r0, i0)
__jit_inline void
arm_sti_i(jit_state_t _jit, void *i0, jit_gpr_t r0)
{
    jit_movi_i(JIT_TMP, (int)i0);
    _STRI(JIT_TMP, r0, 0);
}

#define jit_stxr_i(r0, r1, r2)		arm_stxr_i(_jit, r0, r1, r2)
__jit_inline void
arm_stxr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _STR(r0, r1, r2);
}

#define jit_stxi_i(r0, r1, i0)		arm_stxi_i(_jit, r0, r1, i0)
__jit_inline void
arm_stxi_i(jit_state_t _jit, int i0, jit_gpr_t r0, jit_gpr_t r1)
{
    jit_gpr_t		reg;
    if (i0 >= 0 && i0 <= 4095)
	_STRI(r0, r1, i0);
    else if (i0 < 0 && i0 >= -4095)
	_STRIN(r0, r1, -i0);
    else {
	reg = r0 != r1 ? r0 : JIT_TMP;
	jit_movi_i(reg, i0);
	_STR(r0, r1, reg);
    }
}

#define jit_allocai(i0)			arm_allocai(_jit, i0)
__jit_inline int
arm_allocai(jit_state_t _jit, int i0)
{
    assert(i0 >= 0);
    _jitl.alloca_offset += i0;
    jit_patch_movi(_jitl.stack, (void *)
		   ((_jitl.alloca_offset +
		     _jitl.stack_length + 7) & -8));
    return (-_jitl.alloca_offset);
}

#define jit_prolog(n)			arm_prolog(_jit, n)
__jit_inline void
arm_prolog(jit_state_t _jit, int i0)
{
    _PUSH(/* arguments (should keep state and only save "i0" registers) */
	  (1<<_R0)|(1<<_R1)|(1<<_R2)|(1<<_R3)|
	  /* callee save (FIXME _R9 also added to align at 8 bytes but
	   * need to check alloca implementation and/or alignment if
	   * running out of register arguments when calling functions) */
	  (1<<_R4)|(1<<_R5)|(1<<_R6)|(1<<_R7)|(1<<_R8)|(1<<_R9)|
	  /* previous fp and return address */
	  (1<<JIT_FP)|(1<<JIT_LR));
    _MOV(JIT_FP, JIT_SP);

    _jitl.nextarg_get = 0;
    _jitl.framesize = 48;

    /* patch alloca and stack adjustment */
    _jitl.stack = (int *)_jit->x.pc;
    jit_movi_p(JIT_TMP, 0);
    _SUB(JIT_SP, JIT_SP, JIT_TMP);
    _jitl.alloca_offset = _jitl.stack_offset = _jitl.stack_length = 0;
}

#define jit_callr(r0)			_BLX(r0)
#define jit_calli(i0)			arm_calli(_jit, i0)
__jit_inline jit_insn *
arm_calli(jit_state_t _jit, void *i0)
{
    /* FIXME if not patching (99% of the time), check range and use bl label */
    jit_insn	*l;
    l = _jit->x.pc;
    jit_movi_p(JIT_TMP, i0);
    _BLX(JIT_TMP);
    return (l);
}

#define jit_prepare(i0)			arm_prepare_i(_jit, i0)
__jit_inline void
arm_prepare_i(jit_state_t _jit, int i0)
{
    assert(i0 >= 0 && !_jitl.stack_offset && !_jitl.nextarg_put);
    if (i0 > 4) {
	_jitl.stack_offset = i0 << 2;
	if (_jitl.stack_length < _jitl.stack_offset) {
	    _jitl.stack_length = _jitl.stack_offset;
	    jit_patch_movi(_jitl.stack, (void *)
			   ((_jitl.alloca_offset +
			     _jitl.stack_length + 7) & -8));
	}
    }
}

#define jit_arg_i()			arm_arg_i(_jit)
__jit_inline int
arm_arg_i(jit_state_t _jit)
{
    int		ofs = _jitl.nextarg_get++;
    if (ofs > 3) {
	ofs = _jitl.framesize;
	_jitl.framesize += sizeof(int);
    }
    return (ofs);
}

#define jit_getarg_i(r0, i0)		arm_getarg_i(_jit, r0, i0)
__jit_inline void
arm_getarg_i(jit_state_t _jit, jit_gpr_t r0, int i0)
{
    /* arguments are saved in prolog */
    if (i0 < 4)
	jit_ldxi_i(r0, JIT_FP, (i0 << 2));
    else
	jit_ldxi_i(r0, JIT_FP, i0);
}

#define jit_pusharg_i(r0)		arm_pusharg_i(_jit, r0)
__jit_inline void
arm_pusharg_i(jit_state_t _jit, jit_gpr_t r0)
{
    if (_jitl.nextarg_put < 4)
	jit_stxi_i(16 - (_jitl.nextarg_put << 2) + 8, JIT_FP, r0);
    else {
	_jitl.stack_offset -= sizeof(int);
	jit_stxi_i(_jitl.stack_offset, JIT_SP, r0);
    }
    _jitl.nextarg_put++;
}

#define jit_finishr(rs)			arm_finishr(_jit, rs)
__jit_inline void
arm_finishr(jit_state_t _jit, jit_gpr_t r0)
{
    int		list;
    assert(_jitl.stack_offset == 0);
    if (_jitl.nextarg_put) {
	list = (1 << _jitl.nextarg_put) - 1;
	_ADDI(JIT_TMP, JIT_FP, 12);
	_LDMIA(JIT_TMP, list);
	_jitl.nextarg_put = 0;
    }
    jit_callr(r0);
}

#define jit_finish(i0)			arm_finishi(_jit, i0)
__jit_inline jit_insn *
arm_finishi(jit_state_t _jit, void *i0)
{
    int		list;
    assert(_jitl.stack_offset == 0);
    if (_jitl.nextarg_put) {
	list = (1 << _jitl.nextarg_put) - 1;
	_ADDI(JIT_TMP, JIT_FP, 12);
	_LDMIA(JIT_TMP, list);
	_jitl.nextarg_put = 0;
    }
    return (jit_calli(i0));
}

#define jit_ret()			arm_ret(_jit)
__jit_inline void
arm_ret(jit_state_t jit)
{
    /* do not restore arguments */
    _ADDI(JIT_SP, JIT_FP, 16);
    _POP(/* callee save */
	 (1<<_R4)|(1<<_R5)|(1<<_R6)|(1<<_R7)|(1<<_R8)|(1<<_R9)|
	 /* previous fp and return address */
	 (1<<JIT_FP)|(1<<JIT_PC));
}

/**********************************************************************/
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

#if 1
    /*
     * void f(int a, int b) { printf("%d + %d + %d = %d\n", a, b, a + b); }
     */
    {
	int	a0, a1;
	jit_prolog(2);		// push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,fp,lr}; mov fp, sp; mov r8, r8, #8, orr, r8, r8, #0...; sub sp, sp, r8
	a0 = jit_arg_i();
	a1 = jit_arg_i();
	jit_getarg_i(JIT_V0, a0);	// ldr r4, [fp]
	jit_getarg_i(JIT_V1, a1);	// ldr r5, [fp, #4]
	jit_addr_i(JIT_R1, JIT_V0, JIT_V1);	// add r1, r4, r5
	jit_movi_p(JIT_R0, "%d + %d = %d\n");	// mov r0, r0, #<q0>, orr, r0, r0, #<q1>...
	jit_prepare(4);
	{
	    jit_pusharg_i(JIT_R1);	// str r1, [fp, #24]
	    jit_pusharg_i(JIT_V1);	// str r5, [fp, #20]
	    jit_pusharg_i(JIT_V0);	// str r4, [fp, #16]
	    jit_pusharg_i(JIT_R0);	// str r0, [fp, #12]
	}
	jit_finish(printf);		// add r8, fp, #12, ldm r8, {r0, r1, r2, r3}, mov r0, #<q3>; orr r8, r8, #<q2>...; blx r8
	jit_ret();			// sub sp, fp, #16; pop {r4, r5, r6, r7, r8, r9, fp, pc}
    }
    jit_flush_code(buffer, jit_get_ip().ptr);
    ((void (*)(int,int))buffer)(1, 2);
#endif
    disassemble(buffer, (long)jit_get_ip().ptr - (long)buffer);

    return (0);
}
