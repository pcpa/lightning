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
#include <stdlib.h>
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
} jit_local_state;
struct {
    unsigned	armv6		: 1;
    unsigned	thumb		: 1;
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

#define __jit_inline		inline
#define _jit_I(ii)		(*_jit->x.ui_pc++)= ii
#define _u4(n)			((n) & 0xf)
#define _u4P(n)			!((n) & ~0xf)
#define _u8(n)			((n) & 0xff)
#define _u8P(n)			!((n) & ~0xff)
#define _u12(n)			((n) & 0xfff)
#define _u12P(n)		!((n) & ~0xfff)

#define	jit_get_ip()		(*(jit_code *) &_jit->x.pc)
#define	jit_set_ip(ptr)		(_jit->x.pc = (ptr), jit_get_ip ())
#define	jit_get_label()		(_jit->x.pc)
#define	jit_forward()		(_jit->x.pc)


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

#define ARM_I		0x02000000	/* immediate */
#define ARM_S		0x00100000	/* set flags */
#define ARM_ADD		0x00800000
#define ARM_ADC		0x00a00000
#define ARM_SUB		0x00400000
#define ARM_SBC		0x00c00000
#define ARM_RSB		0x00600000
#define ARM_RSC		0x00e00000
#define ARM_MUL		0x00000090
#define ARM_MLA		0x00200090

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

#define ARM_MOV		0x01a00000
#define ARM_MVN		0x01e00000

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
_orr(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r0)<<12)|_u4(r1));
}

__jit_inline void
_ori(jit_state_t _jit, int o, jit_gpr_t r0, int i0)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r0)<<12)|_u12(i0));
}

__jit_inline void
_orrr(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u4(r2));
}

__jit_inline void
_orri(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u12(i0));
}

__jit_inline void
_orri8(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r1)<<16)|(_u4(r0)<<12)|_u8(i0));
}

__jit_inline void
_orrrr(jit_state_t _jit, int o,
       jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2, jit_gpr_t r3)
{
    assert(!(o & 0xfff0f));
    _jit_I(o|(_u4(r0)<<16)|(_u4(r1)<<12)|(_u4(r2)<<8)|_u4(r3));
}

__jit_inline void
_srrr(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _jit_I(o|(_u4(r0)<<12)|(_u4(r2)<<8)|_u4(r1));
}

__jit_inline void
_srri(jit_state_t _jit, int o, jit_gpr_t r0, jit_gpr_t r1, int i0)
{
    assert(i0 >= 1 && i0 <= 31);
    _jit_I(o|(_u4(r0)<<12)|(i0<<7)|(_u4(r1)));
}

#define _NOP()			_MOV(_R0, _R0)
#define _ADD(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADD, r0, r1, r2)
#define _ADDI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADD|ARM_I, r0, r1, i0)
#define _ADDS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADD|ARM_S, r0, r1, r2)
#define _ADDSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADD|ARM_S|ARM_I, r0, r1, i0)
#define _ADC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADC, r0, r1, r2)
#define _ADCI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADC|ARM_I, r0, r1, i0)
#define _ADCS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_ADC|ARM_S, r0, r1, r2)
#define _ADCSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ADC|ARM_S|ARM_I, r0, r1, i0)
#define _SUB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SUB, r0, r1, r2)
#define _SUBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SUB|ARM_I, r0, r1, i0)
#define _SUBS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SUB|ARM_S, r0, r1, r2)
#define _SUBSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SUB|ARM_S|ARM_I, r0, r1, i0)
#define _SBC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SBC, r0, r1, r2)
#define _SBCI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SBC|ARM_I, r0, r1, i0)
#define _SBCS(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_SBC|ARM_S, r0, r1, r2)
#define _SBCSI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_SBC|ARM_S|ARM_I, r0, r1, i0)
#define _RSB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_RSB, r0, r1, r2)
#define _RSBI(r0, r1, i0)	_orrr(_jit,ARM_CC_AL|ARM_RSB|ARM_I, r0, r1, i0)
#define _RSC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_RSC, r0, r1, r2)
#define _RSCI(r0, r1, i0)	_orrr(_jit,ARM_CC_AL|ARM_RSC|ARM_I, r0, r1, i0)
#define _MUL(r0, r1, r2)	_orrrr(_jit, ARM_CC_AL|ARM_MUL, r0, 0, r2, r1)
#define _MLA(r0, r1, r2, r3)	_orrrr(_jit, ARM_CC_AL|ARM_MLA, r0, r3, r2, r1)

#define _AND(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_AND, r0, r1, r2)
#define _ANDI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_AND|ARM_I, r0, r1, i0)
#define _BIC(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_BIC, r0, r1, r2)
#define _BICI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_BIC|ARM_I, r0, r1, i0)
#define _OR(r0, r1, r2)		_orrr(_jit,ARM_CC_AL|ARM_ORR, r0, r1, r2)
#define _ORI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_ORR|ARM_I, r0, r1, i0)
#define _XOR(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_EOR, r0, r1, r2)
#define _XORI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_EOR|ARM_I, r0, r1, i0)
#define _MOV(r0, r1)		_orr(_jit,ARM_CC_AL|ARM_MOV, r0, r1)
#define _MOVI(r0, i0)		_ori(_jit,ARM_CC_AL|ARM_MOV|ARM_I, r0, i0)
#define _MVN(r0, r1)		_orr(_jit,ARM_CC_AL|ARM_MVN, r0, r1)
#define _MVNI(r0, i0)		_ori(_jit,ARM_CC_AL|ARM_MVN|ARM_I, r0, i0)

#define _NOT(r0, r1)		_orr(_jit,ARM_CC_AL|ARM_MVN, r0, r1)

#define _ASR(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_ASR, r0, r1, r2)
#define _LSR(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_LSL, r0, r1, r2)
#define _LSL(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_LSL, r0, r1, r2)
#define _ROR(r0, r1, r2)	_srrr(_jit,ARM_CC_AL|ARM_ROR, r0, r1, r2)
#define _ASRI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_ASRI, r0, r1, i0)
#define _LSRI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_LSRI, r0, r1, i0)
#define _LSLI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_LSLI, r0, r1, i0)
#define _RORI(r0, r1, i0)	_srri(_jit,ARM_CC_AL|ARM_RORI, r0, r1, i0)

#define _LDRSB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSB|ARM_P, r0, r1, r2)
#define _LDRSBN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSB, r0, r1, r2)
#define _LDRSBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSBI|ARM_P, r0, r1, i0)
#define _LDRSBIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSBI, r0, r1, i0)
#define _LDRB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRB|ARM_P, r0, r1, r2)
#define _LDRBN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRB, r0, r1, r2)
#define _LDRBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRBI|ARM_P, r0, r1, i0)
#define _LDRBIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRBI, r0, r1, i0)
#define _LDRSH(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSH|ARM_P, r0, r1, r2)
#define _LDRSHN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRSH, r0, r1, r2)
#define _LDRSHI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSHI|ARM_P, r0, r1, i0)
#define _LDRSHIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRSHI, r0, r1, i0)
#define _LDRH(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRH|ARM_P, r0, r1, r2)
#define _LDRHN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDRH, r0, r1, r2)
#define _LDRHI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRHI|ARM_P, r0, r1, i0)
#define _LDRHIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRHI, r0, r1, i0)
#define _LDR(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDR|ARM_P, r0, r1, r2)
#define _LDRN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_LDR, r0, r1, r2)
#define _LDRI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRI|ARM_P, r0, r1, i0)
#define _LDRIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_LDRI, r0, r1, i0)
#define _STRB(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRB|ARM_P, r2, r1, r0)
#define _STRBN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRB, r2, r1, r0)
#define _STRBI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRBI|ARM_P, r1, r0, i0)
#define _STRBIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRBI, r1, r0, i0)
#define _STRH(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRH|ARM_P, r2, r1, r0)
#define _STRHN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STRH, r2, r1, r0)
#define _STRHI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRHI|ARM_P, r1, r0, i0)
#define _STRHIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRHI, r1, r0, i0)
#define _STR(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STR|ARM_P, r2, r1, r0)
#define _STRN(r0, r1, r2)	_orrr(_jit,ARM_CC_AL|ARM_STR, r2, r1, r0)
#define _STRI(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRI|ARM_P, r1, r0, i0)
#define _STRIN(r0, r1, i0)	_orri(_jit,ARM_CC_AL|ARM_STRI, r1, r0, i0)

/**********************************************************************/
#define JIT_PC			_R15
#define JIT_SP			_R13
#define JIT_TMP			_R12

#define jit_movr_i(r0, r1)		arm_movr_i(_jit, r0, r1)
__jit_inline void
arm_movr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
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
	int	q0, q1, q2, q3;
	q0 = i0 & 0x000000ff;		q1 = i0 & 0x0000ff00;
	q2 = i0 & 0x00ff0000;		q3 = i0 & 0xff000000;
	if (!q0 + !q1 + !q2 + !q3 >= 2) {
	    /* prefer no inversion on tie */
	    if (q3) {
		_MOVI(r0, encode_arm_immediate(q3));
		if (q2)	_ORI(r0, r0, encode_arm_immediate(q2));
		if (q1)	_ORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_ORI(r0, r0, q0);
	    }
	    else if (q2) {
		_MOVI(r0, encode_arm_immediate(q2));
		if (q1)	_ORI(r0, r0, encode_arm_immediate(q1));
		if (q0)	_ORI(r0, r0, q0);
	    }
	    else if (q1) {
		_MOVI(r0, encode_arm_immediate(q1));
		_ORI(r0, r0, q0);
	    }
	}
	else {
	    i0 = ~i0;
	    q0 = i0 & 0x000000ff;	q1 = i0 & 0x0000ff00;
	    q2 = i0 & 0x00ff0000;	q3 = i0 & 0xff000000;
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
	    else if (q1) {
		_MVNI(r0, encode_arm_immediate(q1));
		_XORI(r0, r0, q0);
	    }
	}
    }
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

#define jit_addxr_ui(r0, r1, r2)	arm_addxr_i(_jit, r0, r1, r2)
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
    if (r0 == r1 && !jit_cpu.armv6) {
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
    if (r0 == r1 && !jit_cpu.armv6)
	_MUL(r0, reg, r1);
    else
	_MUL(r0, r1, reg);
}


#define jit_notr_i(r0, r1)		arm_notr_i(_jit, r0, r1)
__jit_inline void
arm_notr_i(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1)
{
    _NOT(r0, r1);
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

/**********************************************************************/
int
main(int argc, char *argv[])
{
    unsigned char	*buffer = malloc(4096);

    jit_set_ip(buffer);
    /*
    jit_ldxr_c(_R0, _R1, _R2);
    jit_ldxi_c(_R0, _R1, 2);
    jit_ldxi_c(_R0, _R1, -2);
    jit_ldxr_uc(_R0, _R1, _R2);
    jit_ldxi_uc(_R0, _R1, 2);
    jit_ldxi_uc(_R0, _R1, -2);
    jit_ldxr_s(_R0, _R1, _R2);
    jit_ldxi_s(_R0, _R1, 2);
    jit_ldxi_s(_R0, _R1, -2);
    jit_ldxr_us(_R0, _R1, _R2);
    jit_ldxi_us(_R0, _R1, 2);
    jit_ldxi_us(_R0, _R1, -2);
    jit_ldxr_i(_R0, _R1, _R2);
    jit_ldxi_i(_R0, _R1, 2);
    jit_ldxi_i(_R0, _R1, -2);
    */
    jit_stxr_c(_R2, _R1, _R0);
    jit_stxi_c(2, _R1, _R0);
    jit_stxi_c(-2, _R1, _R0);
    jit_stxr_s(_R2, _R1, _R0);
    jit_stxi_s(2, _R1, _R0);
    jit_stxi_s(-2, _R1, _R0);
    jit_stxr_i(_R2, _R1, _R0);
    jit_stxi_i(2, _R1, _R0);
    jit_stxi_i(-2, _R1, _R0);

    disassemble(buffer, (long)jit_get_ip().ptr - (long)buffer);
    fflush(stdout);

    return (0);
}
