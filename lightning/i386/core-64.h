/******************************** -*- C -*- ****************************
 *
 *	Platform-independent layer (i386 version)
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2000, 2001, 2002, 2003, 2006 Free Software Foundation, Inc.
 * Written by Paolo Bonzini and Matthew Flatt.
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
 * Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 ***********************************************************************/



#ifndef __lightning_core_h
#define __lightning_core_h

/* Used to implement ldc, stc, ... */
#define JIT_CAN_16 0
#define JIT_REXTMP		_R12

/* Number or integer argument registers */
#define JIT_ARG_MAX		6

/* Number of float argument registers */
#define JIT_FP_ARG_MAX		8

#define JIT_R_NUM		3
#define JIT_R(i)                ((i) == 0 ? _RAX : _R9 + (i))
#define JIT_V_NUM               3
#define JIT_V(i)                ((i) == 0 ? _RBX : _R12 + (i))

struct jit_local_state {
  int   long_jumps;
  int   nextarg_getfp;
  int   nextarg_putfp;
  int   nextarg_geti;
  int	nextarg_puti;
  int	framesize;
  int	argssize;
  int	fprssize;
  int   alloca_offset;
  int   alloca_slack;
  jit_insn	*label;
  union {
      int	 i;
      long	 l;
      float	 f;
      double	 d;
      void	*p;
  } data;
};

/* Whether a register in the "low" bank is used for the user-accessible
   registers.  */
#define jit_save(reg)		((reg) == _EAX || (reg) == _EBX)

/* Keep the stack 16-byte aligned, the SSE hardware prefers it this way.  */
#define jit_allocai_internal(amount, slack)                           \
  (((amount) < _jitl.alloca_slack                                     \
    ? 0                                                               \
    : (_jitl.alloca_slack += (amount) + (slack),                      \
      SUBQir((amount) + (slack), _ESP))),                             \
   _jitl.alloca_slack -= (amount),                                    \
   _jitl.alloca_offset -= (amount))

#define jit_allocai(n)                                                \
  jit_allocai_internal ((n), (_jitl.alloca_slack - (n)) & 15)

/* 3-parameter operation */
#define jit_qopr_(d, s1, s2, op1d, op2d)					\
	( ((s2) == (d)) ? op1d :						\
	  (  (((s1) == (d)) ? (void)0 : (void)MOVQrr((s1), (d))), op2d )	\
	)

/* 3-parameter operation, with immediate. TODO: fix the case where mmediate
   does not fit! */
#define jit_qop_small(d, s1, op2d)					\
	(((s1) == (d)) ? op2d : (MOVQrr((s1), (d)), op2d))
#define jit_qop_(d, s1, is, op2d, op2i)					\
	(_s32P((long)(is))						\
	 ? jit_qop_small ((d), (s1), (op2d))				\
	 : (MOVQir ((is), JIT_REXTMP), jit_qop_small ((d), (s1), (op2i))))

#define jit_bra_qr(s1, s2, op)		(CMPQrr(s2, s1), op, _jit.x.pc)
#define _jit_bra_l(rs, is, op)		(CMPQir(is, rs), op, _jit.x.pc)

#define jit_bra_l(rs, is, op) (_s32P((long)(is)) \
                               ? _jit_bra_l(rs, is, op) \
                               : (MOVQir(is, JIT_REXTMP), jit_bra_qr(rs, JIT_REXTMP, op)))

/* When CMP with 0 can be replaced with TEST */
#define jit_bra_l0(rs, is, op, op0)					\
	( (is) == 0 ? (TESTQrr(rs, rs), op0, _jit.x.pc) : jit_bra_l(rs, is, op))

#define jit_reduceQ(op, is, rs)							\
	(_u8P(is) ? jit_reduce_(op##Bir(is, jit_reg8(rs))) :	\
	jit_reduce_(op##Qir(is, rs)) )

#define jit_addi_l(d, rs, is)						\
    /* if (is != 0) */							\
    ((is)								\
	/* if (is != 1) */						\
	? (((is) != 1)							\
	    /* if (is != -1) */						\
	    ? (((is) != -1)						\
		/* if ((is < 0 && int32_p(is)) */			\
		? (((((is) < 0) && _s32P(is))				\
		    /* || (is > 0 && uint31_p(is))) */			\
		    || ((is) > 0 && _uiP(31, is)))			\
		    /* if (d == rs) */					\
		    ? (((d) == (rs))					\
			/* Use sign extending add opcode */		\
			? ADDQir((is), (d))				\
			/* Use sign extending 3 operand lea opcode */	\
			: LEAQmr((is), (rs), 0, 0, (d)))		\
		    /* <need register for immediate> if (d != rs) */	\
		    : (((d) != (rs))					\
			/* d = is, d += rs; */				\
			? (MOVQir((is), (d)), ADDQrr((rs), (d)))	\
			/* tmp = is, d += tmp; */			\
			: (MOVQir((is), JIT_REXTMP),			\
			   ADDQrr(JIT_REXTMP, (d)))))			\
		/* <is == -1> if (d != rs) d = rs; */			\
		: ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0),		\
		    /* --d; */						\
		    DECQr((d))))					\
	    /* <is == 1> if (d != rs) d = rs; ++d; */			\
	    : ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0), INCQr((d))))	\
	/* <is == 0> if (d != rs) d = rs; */				\
	: (((d) != (rs)) ? MOVQrr((rs), (d)) : 0))

#define jit_addr_l(d, s1, s2)						\
    /* if (d == s1) */							\
    (((d) == (s1))							\
	/* d += s2; */							\
	? ADDQrr((s2), (d))						\
	/* else if (d == s2) */						\
	: (((d) == (s2))						\
	    /* d += s1; */						\
	    ? ADDQrr((s1), (d))						\
	    /* <d != s2> use sign extending 3 operand lea opcode */	\
	    : LEAQmr(0, (s1), (s2), 1, (d))))

/* o Immediates are sign extended
 * o CF (C)arry (F)lag is set when interpreting it as unsigned addition
 * o OF (O)verflow (F)lag is set when interpreting it as signed addition
 * FIXME there are shorter versions when register is %RAX
 */
/* Commutative */
/*
jit_addci_ul(d, rs, is) {
    if ((is <= 0 && _s32P(is)) || (is > 0 && _uiP(31, is))) {
	if (d != rs)
	    MOVQrr(rs, d);
	ADDQir(is, d);
    }
    else if (d == rs) {
	MOVQir(is, JIT_REXTMP);
	ADDQrr(JIT_REXTMP, d);
    }
    else {
	MOVQir(is, d);
	ADDQrr(rs, d);
    }
}
 */
#define jit_addci_ul(d, rs, is)						\
    ((((is) <= 0 && _s32P(is)) || ((is) > 0 && _uiP(31, is)))		\
	? ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0),			\
	   ADDQir((is), (d)))						\
	: (((d) == (rs))						\
	    ? (MOVQir((is), JIT_REXTMP),				\
	       ADDQrr(JIT_REXTMP, (d)))					\
	    : (MOVQir((is), (d)),					\
	       ADDQrr((rs), (d)))))

/*
jit_addcr_ul(d, s1, s2) {
    if (d == s2)
	ADDQrr(s1, d);
    else if (d == s1)
	ADDQrr(s2, d);
    else {
	MOVQrr(s1, d);
	ADDQrr(s2, d);
    }
}
 */
#define jit_addcr_ul(d, s1, s2)						\
    (((d) == (s2))							\
	? ADDQrr((s1), (d))						\
	: (((d) == (s1))						\
	    ?  ADDQrr((s2), (d))					\
	    : (MOVQrr((s1), (d)),					\
	       ADDQrr((s2), (d)))))

/*
jit_addxi_ul(d, rs, is) {
    if ((is <= 0 && _s32P(is)) || (is > 0 && _uiP(31, is))) {
	if (d != rs)
	    MOVQrr(rs, d);
	ADCQir(is, d);
    }
    else if (d == rs) {
	MOVQir(is, JIT_REXTMP);
	ADCQrr(JIT_REXTMP, d);
    }
    else {
	MOVQir(is, d);
	ADCQrr(rs, d);
    }
}
 */
#define jit_addxi_ul(d, rs, is)						\
    ((((is) <= 0 && _s32P(is)) || ((is) > 0 && _uiP(31, is)))		\
	? ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0),			\
	   ADCQir((is), (d)))						\
	: (((d) == (rs))						\
	    ? (MOVQir((is), JIT_REXTMP),				\
	       ADCQrr(JIT_REXTMP, (d)))					\
	    : (MOVQir((is), (d)),					\
	       ADCQrr((rs), (d)))))

/*
jit_addxr_ul(d, s1, s2) {
    if (d == s2)
	ADCQrr(s1, d);
    else if (d == s1)
	ADCQrr(s2, d);
    else {
	MOVQrr(s1, d);
	ADCQrr(s2, d);
    }
}
 */
#define jit_addxr_ul(d, s1, s2)						\
    (((d) == (s2))							\
	? ADCQrr((s1), (d))						\
	: (((d) == (s1))						\
	    ? ADCQrr((s2), (d))						\
	    : (MOVQrr((s1), (d)),					\
	       ADCQrr((s2), (d)))))

/* Non commutative */
/*
jit_subci_ul(d, rs, is) {
    if (d != rs)
	MOVQrr(rs, d);
    if ((is <= 0 && _s32P(is)) || (is > 0 && _uiP(31, is)))
	SUBQir(is, d);
    else {
	MOVQir(is, JIT_REXTMP);
	SUBQrr(JIT_REXTMP, d);
    }
}
 */
#define jit_subci_ul(d, rs, is)						\
    ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0),				\
     ((((is) <= 0 && _s32P(is)) || ((is) > 0 && _uiP(31, is)))		\
	? SUBQir((is), (d))						\
	: (MOVQir((is), JIT_REXTMP),					\
	   SUBQrr(JIT_REXTMP, (d)))))

/*
jit_subcr_ul(d, s1, s2) {
    if (d == s2) {
	MOVQrr(d, JIT_REXTMP);
	MOVQrr(s1, d);
	SUBQrr(JIT_REXTMP, d);
    }
    else {
	if (d != s1)
	    MOVQrr(s1, d);
	SUBQrr(s2, d);
    }
}
 */
#define jit_subcr_ul(d, s1, s2)						\
    (((d) == (s2))							\
	? (MOVQrr(d, JIT_REXTMP),					\
	   MOVQrr(s1, d),						\
	   SUBQrr(JIT_REXTMP, d))					\
	: ((((d) != (s1)) ? MOVQrr((s1), (d)) : 0),			\
	   SUBQrr(s2, d)))

/*
jit_subxi_ul(d, rs, is) {
    if (d != rs)
	MOVQrr(rs, d);
    if ((is <= 0 && _s32P(is)) || (is > 0 && _uiP(31, is)))
	SBBQir(is, d);
    else {
	MOVQir(is, JIT_REXTMP);
	SBBQrr(JIT_REXTMP, d);
    }
}
 */
#define jit_subxi_ul(d, rs, is)						\
    ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0),				\
     ((((is) <= 0 && _s32P(is)) || ((is) > 0 && _uiP(31, is)))		\
	? SBBQir((is), (d))						\
	: (MOVQir((is), JIT_REXTMP),					\
	   SBBQrr(JIT_REXTMP, (d)))))

/*
jit_subxr_ul(d, s1, s2) {
    if (d == s2) {
	MOVQrr(d, JIT_REXTMP);
	MOVQrr(s1, d);
	SBBQrr(JIT_REXTMP, d);
    }
    else {
	if (d != s1)
	    MOVQrr(s1, d);
	SBBQrr(s2, d);
    }
}
 */
#define jit_subxr_ul(d, s1, s2)						\
    (((d) == (s2))							\
	? (MOVQrr(d, JIT_REXTMP),					\
	   MOVQrr(s1, d),						\
	   SBBQrr(JIT_REXTMP, d))					\
	: ((((d) != (s1)) ? MOVQrr((s1), (d)) : 0),			\
	   SBBQrr(s2, d)))

#define jit_andi_l(d, rs, is)	jit_qop_ ((d), (rs), (is), ANDQir((is), (d)), ANDQrr(JIT_REXTMP, (d)))
#define jit_andr_l(d, s1, s2)	jit_qopr_((d), (s1), (s2), ANDQrr((s1), (d)), ANDQrr((s2), (d)) )
#define jit_orr_l(d, s1, s2)	jit_qopr_((d), (s1), (s2),  ORQrr((s1), (d)),  ORQrr((s2), (d)) )
#define jit_subr_l(d, s1, s2)	jit_qopr_((d), (s1), (s2), (SUBQrr((s1), (d)), NEGQr(d)),	SUBQrr((s2), (d))	       )
#define jit_xorr_l(d, s1, s2)	jit_qopr_((d), (s1), (s2), XORQrr((s1), (d)), XORQrr((s2), (d)) )

/* These can sometimes use byte or word versions! */
#define jit_ori_l(d, rs, is)	jit_qop_ ((d), (rs), (is),   jit_reduceQ(OR, (is), (d)), ORQrr(JIT_REXTMP, (d))	       )
#define jit_xori_l(d, rs, is)	jit_qop_ ((d), (rs), (is),   jit_reduceQ(XOR, (is), (d)), XORQrr(JIT_REXTMP, (d))	       )

/*  Instruction format is:
 *  <shift> %r0 %r1
 *	%r0 <shift>= %r1
 *  only %cl can be used as %r1
 */
#define jit_qshiftr(d, s1, s2, shift)					\
	/* if (d == rcx) { */						\
    ((jit_reg64(d) == _RCX)						\
	    /* tmp = s2 */						\
	? (MOVQrr((s1), JIT_REXTMP),					\
	    /* if (r2 != rcx) r8(rcx) = r8(s2); */			\
	   ((jit_reg64(s2) != _RCX) ? MOVBrr(jit_reg8(s2), _CL) : 0),	\
	    /* tmp >>= cl */						\
	   shift(_CL, JIT_REXTMP),					\
	    /* <d == rcx> rcx = tmp; */					\
	   MOVQrr(JIT_REXTMP, _RCX))					\
	/* } else if (s2 != rcx) { */					\
	: (((jit_reg64(s2) != _RCX)					\
	    /* tmp = rcx; */						\
	    ? (MOVQrr(_RCX, JIT_REXTMP),				\
	    /* r8(ecx) = r8(s2); */					\
	       MOVBrr(jit_reg8(s2), _CL),				\
	    /* if (d != s1) d = s1; */					\
	       (((d) != (s1)) ? MOVQrr((s1), d) : 0),			\
	    /* d >>= cl; */						\
	       shift(_CL, (d)),						\
	    /* rcx = tmp; */						\
	       MOVQrr(JIT_REXTMP, _RCX))				\
	/* else { if (d != s1) d = s1; */				\
	    : ((((d) != (s1)) ? MOVQrr((s1), d) : 0),			\
	    /* d >>= cl; } */						\
	       shift(_CL, (d))))))

#define jit_qshifti(d, rs, is, shift)					\
     /* if (is != 0) */							\
    ((is)								\
	/* if (d != rs) d = rs, */					\
	? ((((d) != (rs)) ? jit_movr_l(d, rs) : 0),			\
	   shift((_uc)(is), (d)))					\
	: (((d) != (rs)) ? MOVQrr((rs), (d)) : 0))

#define jit_lshi_l(d, rs, is)						\
     /* if (is != 0) */							\
    ((is)								\
	/* if (is <= 3) */						\
	? (((_uc)(is) <= 3)						\
	    /* FIXME also better when d == rs? */			\
	    /* 3 register optimized opcode */				\
	    ? LEAQmr(0, 0, (rs), 1 << (_uc)(is), (d))			\
	    /* if (d != rs) d = rs, */					\
	    : (((d) != (rs) ? MOVQrr((rs), (d)) : 0),			\
		/* d >>= c; */						\
	       SHLQir((_uc)(is), (d))))					\
	: (((d) != (rs)) ? MOVQrr((rs), (d)) : 0))

#define jit_rshi_l(d, rs, is)	jit_qshifti((d), rs, is, SARQir)
#define jit_rshi_ul(d, rs, is)	jit_qshifti((d), rs, is, SHRQir)
#define jit_lshr_l(d, r1, r2)	jit_qshiftr((d), (r1), (r2), SHLQrr)
#define jit_rshr_l(d, r1, r2)	jit_qshiftr((d), (r1), (r2), SARQrr)
#define jit_rshr_ul(d, r1, r2)	jit_qshiftr((d), (r1), (r2), SHRQrr)


/* Stack */
#define jit_pushr_i(rs)		PUSHQr(rs)
#define jit_popr_i(rs)		POPQr(rs)

/* Return address is 8 bytes, plus 5 registers = 40 bytes, total = 48 bytes. */
#define jit_prolog(n)							\
     /* Initialize counter of integer arguments */			\
    (_jitl.nextarg_puti = (n),						\
     /* Initialize counter of stack arguments */			\
     _jitl.argssize = (_jitl.nextarg_puti > JIT_ARG_MAX)		\
	? _jitl.nextarg_puti - JIT_ARG_MAX : 0,				\
     /* Initialize offset of stack arguments */				\
     _jitl.framesize = (_jitl.argssize & 1) ? 56 : 48,			\
     /* Initialize counter of float arguments */			\
     _jitl.nextarg_putfp = 0,						\
     /* Initialize offsets of arguments */				\
     _jitl.nextarg_getfp = _jitl.nextarg_geti = 0,			\
     /* Initialize alloca information */				\
     _jitl.alloca_offset = _jitl.alloca_slack = 0,			\
     /* Build stack frame */						\
     PUSHQr(_EBX), PUSHQr(_R12), PUSHQr(_R13), PUSHQr(_R14), 		\
     PUSHQr(_EBP), MOVQrr(_ESP, _EBP))

#define jit_calli(sub)							\
    (MOVQir((long)(sub), JIT_REXTMP),					\
     _jitl.label = _jit.x.pc,						\
     CALLsr(JIT_REXTMP))
#define jit_callr(reg)		CALLsr((reg))
#define jit_patch_calli(pa, pv)	jit_patch_movi(pa, pv)

#define jit_prepare_i(ni)						\
    /* Initialize offset of right to left integer argument */		\
    (_jitl.nextarg_puti = (ni),						\
     /* Initialize float argument offset and register counter */	\
     _jitl.nextarg_putfp = _jitl.fprssize = 0,				\
     /* argssize is used to keep track of stack slots used */		\
     _jitl.argssize = _jitl.nextarg_puti > JIT_ARG_MAX			\
     ? _jitl.nextarg_puti - JIT_ARG_MAX : 0)

#define jit_pusharg_i(rs)						\
    /* Need to use stack for argument? */				\
    (--_jitl.nextarg_puti >= JIT_ARG_MAX				\
       /* Yes. Push it */						\
     ? PUSHQr(rs)							\
       /* No. Use a register */						\
     : MOVQrr(rs, jit_arg_reg_order[_jitl.nextarg_puti]))

/*
jit_finish(sub) {
    if (_jitl.fprssize) {
	MOVBir(_jitl.fprssize, _AL);
	_jitl.fprssize = 0;
    }
    else
	MOVBir(0, _AL);
    if (_jitl.argssize & 1) {
	PUSHQr(_RAX);
	++_jitl.argssize;
    }
    jit_callr(sub);
    if (_jitl.argssize) {
	ADDQir(sizeof(long) * _jitl.argssize, JIT_SP);
	_jitl.argssize = 0;
    }
    return _jitl.label;
}
 */
#define jit_finish(sub)							\
    ((_jitl.fprssize							\
	? (MOVBir(_jitl.fprssize, _AL), _jitl.fprssize = 0)		\
	:  MOVBir(0, _AL)),						\
    ((_jitl.argssize & 1)						\
	? (PUSHQr(_EAX), ++_jitl.argssize) : 0),			\
     jit_calli(sub),							\
    (_jitl.argssize							\
	? (ADDQir(sizeof(long) * _jitl.argssize, JIT_SP),		\
	   _jitl.argssize = 0)						\
	: 0),								\
     _jitl.label)

#define jit_reg_is_arg(reg)						\
    (jit_reg64(reg) == _RCX || jit_reg64(reg) == _RDX)

/*
jit_finishr(reg) {
    if (jit_reg64(reg) == _RAX || jit_reg_is_arg(reg))
	MOVQrr(reg, JIT_REXTMP);
    if (_jitl.fprssize) {
	MOVBir(_jitl.fprssize, _AL);
	_jitl.fprssize = 0;
    }
    else
	MOVBir(0, _AL);
    if (_jitl.argssize & 1) {
	PUSHQr(_RAX);
	++_jitl.argssize;
    }
    if (jit_reg64(reg) == _RAX || jit_reg_is_arg(reg))
	jit_callr(JIT_REXTMP);
    else
	jit_callr(reg);
    if (_jitl.argssize) {
	ADDQir(sizeof(long) * _jitl.argssize, JIT_SP);
	_jitl.argssize = 0;
    }
}
 */
#define jit_finishr(reg)						\
    (((jit_reg64(reg) == _RAX || jit_reg_is_arg(reg))			\
	? MOVQrr(reg, JIT_REXTMP) : 0),					\
     (_jitl.fprssize							\
	? (MOVBir(_jitl.fprssize, _AL), _jitl.fprssize = 0)		\
	: MOVBir(0, _AL)),						\
     ((_jitl.argssize & 1)						\
	? (PUSHQr(_RAX), ++_jitl.argssize) : 0),			\
     ((jit_reg64(reg) == _RAX || jit_reg_is_arg(reg))			\
	? jit_callr(JIT_REXTMP) : jit_callr(reg)),			\
     (_jitl.argssize							\
	? (ADDQir(sizeof(long) * _jitl.argssize, JIT_SP),		\
	   _jitl.argssize = 0)						\
	: 0))

#define jit_retval_l(rd)	((void)jit_movr_l ((rd), _RAX))
#define jit_arg_i()		(_jitl.nextarg_geti < JIT_ARG_MAX \
				 ? _jitl.nextarg_geti++ \
				 : ((_jitl.framesize += sizeof(long)) - sizeof(long)))
#define jit_arg_c()		jit_arg_i()
#define jit_arg_uc()		jit_arg_i()
#define jit_arg_s()		jit_arg_i()
#define jit_arg_us()		jit_arg_i()
#define jit_arg_ui()		jit_arg_i()
#define jit_arg_l()		jit_arg_i()
#define jit_arg_ul()		jit_arg_i()
#define jit_arg_p()		jit_arg_i()

#define jit_getarg_c(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_c_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_c((reg), JIT_FP, (ofs)))
#define jit_getarg_uc(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_uc_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_uc((reg), JIT_FP, (ofs)))
#define jit_getarg_s(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_s_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_s((reg), JIT_FP, (ofs)))
#define jit_getarg_us(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_extr_us_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_us((reg), JIT_FP, (ofs)))
#define jit_getarg_i(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_i((reg), JIT_FP, (ofs)))
#define jit_getarg_ui(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_ui((reg), JIT_FP, (ofs)))
#define jit_getarg_l(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_l((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_l((reg), JIT_FP, (ofs)))
#define jit_getarg_ul(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_ul((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_ul((reg), JIT_FP, ofs))
#define jit_getarg_p(reg, ofs)	((ofs) < JIT_ARG_MAX \
				 ? jit_movr_p((reg), jit_arg_reg_order[(ofs)]) \
				 : jit_ldxi_p((reg), JIT_FP, (ofs)))

static int jit_arg_reg_order[] = { _EDI, _ESI, _EDX, _ECX, _R8D, _R9D };

#define jit_negr_l(d, rs)	jit_opi_((d), (rs), NEGQr(d), (XORQrr((d), (d)), SUBQrr((rs), (d))) )
#define jit_movr_l(d, rs)	((void)((rs) == (d) ? 0 : MOVQrr((rs), (d))))
#define jit_movi_p(d, is)	(MOVQir(((long)(is)), (d)), _jit.x.pc)
#define jit_movi_l(d, is)						\
    /* Value is not zero? */						\
    ((is)								\
	/* Value is unsigned and fits in unsigned 32 bits? */		\
	? (_u32P(is)							\
	    /* Use zero extending 32 bits opcode */			\
	    ? MOVLir(is, (d))						\
	    /* Use sign extending 64 bits opcode */			\
	    : MOVQir(is, (d)))						\
	/* Set register to zero. */					\
	: XORQrr((d), (d)))

#define jit_bmsr_l(label, s1, s2)	(TESTQrr((s1), (s2)), JNZm(label), _jit.x.pc)
#define jit_bmcr_l(label, s1, s2)	(TESTQrr((s1), (s2)), JZm(label),  _jit.x.pc)
#define jit_boaddr_l(label, s1, s2)	(ADDQrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_bosubr_l(label, s1, s2)	(SUBQrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_boaddr_ul(label, s1, s2)	(ADDQrr((s2), (s1)), JCm(label), _jit.x.pc)
#define jit_bosubr_ul(label, s1, s2)	(SUBQrr((s2), (s1)), JCm(label), _jit.x.pc)

#define jit_boaddi_l(label, rs, is)	(ADDQir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_bosubi_l(label, rs, is)	(SUBQir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_boaddi_ul(label, rs, is)	(ADDQir((is), (rs)), JCm(label), _jit.x.pc)
#define jit_bosubi_ul(label, rs, is)	(SUBQir((is), (rs)), JCm(label), _jit.x.pc)

/* Alias some macros and add casts to ensure proper zero and sign extension */
#define jit_negr_i(d, rs)	jit_negr_l(d, rs)
#define jit_movr_i(d, rs)	jit_movr_l(d, rs)
#define jit_movi_i(d, is)	jit_movi_l(d, (long)(int)is)
#define jit_movi_ui(d, rs)	jit_movi_l((d), (_ul)(_ui)(rs))

#define jit_patch_long_at(jump_pc,v)  (*_PSL((jump_pc) - sizeof(long)) = _jit_SL((jit_insn *)(v)))
#define jit_patch_short_at(jump_pc,v)  (*_PSI((jump_pc) - sizeof(int)) = _jit_SI((jit_insn *)(v) - (jump_pc)))
#define jit_patch_at(jump_pc,v) (_jitl.long_jumps ? jit_patch_long_at((jump_pc)-3, v) : jit_patch_short_at(jump_pc, v))
#define jit_ret()			(LEAVE_(), POPQr(_R14), POPQr(_R13), POPQr(_R12), POPQr(_EBX), RET_())

/* Memory */

/* Used to implement ldc, stc, ... We have SIL and friends which simplify it all.  */
#define jit_movbrm(rs, dd, db, di, ds)         MOVBrm(jit_reg8(rs), dd, db, di, ds)

#define jit_ldr_c(d, rs)                MOVSBQmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_c(d, s1, s2)           MOVSBQmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldr_s(d, rs)                MOVSWQmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_s(d, s1, s2)           MOVSWQmr(0,    (s1), (s2), 1, (d))
							    
#define jit_ldi_c(d, is)                (_u32P((long)(is)) ? MOVSBQmr((is), 0,    0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_c(d, JIT_REXTMP)))
#define jit_ldxi_c(d, rs, is)           (_u32P((long)(is)) ? MOVSBQmr((is), (rs), 0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_c(d, rs, JIT_REXTMP)))

#define jit_ldi_uc(d, is)               (_u32P((long)(is)) ? MOVZBLmr((is), 0,    0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_uc(d, JIT_REXTMP)))
#define jit_ldxi_uc(d, rs, is)          (_u32P((long)(is)) ? MOVZBLmr((is), (rs), 0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_uc(d, rs, JIT_REXTMP)))

#define jit_sti_c(id, rs)               (_u32P((long)(id)) ? MOVBrm(jit_reg8(rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_c(JIT_REXTMP, rs)))
#define jit_stxi_c(id, rd, rs)          (_u32P((long)(id)) ? MOVBrm(jit_reg8(rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_c(JIT_REXTMP, rd, rs)))

#define jit_ldi_s(d, is)                (_u32P((long)(is)) ? MOVSWQmr((is), 0,    0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_s(d, JIT_REXTMP)))
#define jit_ldxi_s(d, rs, is)           (_u32P((long)(is)) ? MOVSWQmr((is), (rs), 0,    0, (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_s(d, rs, JIT_REXTMP)))

#define jit_ldi_us(d, is)               (_u32P((long)(is)) ? MOVZWLmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_us(d, JIT_REXTMP)))
#define jit_ldxi_us(d, rs, is)          (_u32P((long)(is)) ? MOVZWLmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_us(d, rs, JIT_REXTMP)))

#define jit_sti_s(id, rs)               (_u32P((long)(id)) ? MOVWrm(jit_reg16(rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_s(JIT_REXTMP, rs)))
#define jit_stxi_s(id, rd, rs)          (_u32P((long)(id)) ? MOVWrm(jit_reg16(rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_s(JIT_REXTMP, rd, rs)))

#define jit_ldi_ui(d, is)               (_u32P((long)(is)) ? MOVLmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_ui(d, JIT_REXTMP)))
#define jit_ldxi_ui(d, rs, is)          (_u32P((long)(is)) ? MOVLmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_ui(d, rs, JIT_REXTMP)))

#define jit_ldi_i(d, is)                (_u32P((long)(is)) ? MOVSLQmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_i(d, JIT_REXTMP)))
#define jit_ldxi_i(d, rs, is)           (_u32P((long)(is)) ? MOVSLQmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_i(d, rs, JIT_REXTMP)))

#define jit_sti_i(id, rs)               (_u32P((long)(id)) ? MOVLrm((rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_i(JIT_REXTMP, rs)))
#define jit_stxi_i(id, rd, rs)          (_u32P((long)(id)) ? MOVLrm((rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_i(JIT_REXTMP, rd, rs)))

#define jit_ldi_l(d, is)                (_u32P((long)(is)) ? MOVQmr((is), 0,    0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldr_l(d, JIT_REXTMP)))
#define jit_ldxi_l(d, rs, is)           (_u32P((long)(is)) ? MOVQmr((is), (rs), 0,    0,  (d)) :  (jit_movi_l(JIT_REXTMP, is), jit_ldxr_l(d, rs, JIT_REXTMP)))

#define jit_sti_l(id, rs)               (_u32P((long)(id)) ? MOVQrm((rs), (id), 0,    0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_str_l(JIT_REXTMP, rs)))
#define jit_stxi_l(id, rd, rs)          (_u32P((long)(id)) ? MOVQrm((rs), (id), (rd), 0,    0) : (jit_movi_l(JIT_REXTMP, id), jit_stxr_l(JIT_REXTMP, rd, rs)))

#define jit_ldr_ui(d, rs)               MOVLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_ui(d, s1, s2)          MOVLmr(0,    (s1), (s2), 1,  (d))

#define jit_ldr_i(d, rs)                MOVSLQmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_i(d, s1, s2)           MOVSLQmr(0,    (s1), (s2), 1,  (d))

#define jit_ldr_l(d, rs)                MOVQmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_l(d, s1, s2)           MOVQmr(0,    (s1), (s2), 1,  (d))

#define jit_str_l(rd, rs)               MOVQrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_l(d1, d2, rs)          MOVQrm((rs), 0,    (d1), (d2), 1)

#define jit_blti_l(label, rs, is)	jit_bra_l0((rs), (is), JLm(label), JSm(label) )
#define jit_blei_l(label, rs, is)	jit_bra_l ((rs), (is), JLEm(label)		    )
#define jit_bgti_l(label, rs, is)	jit_bra_l ((rs), (is), JGm(label)		    )
#define jit_bgei_l(label, rs, is)	jit_bra_l0((rs), (is), JGEm(label), JNSm(label) )
#define jit_beqi_l(label, rs, is)	jit_bra_l0((rs), (is), JEm(label), JEm(label) )
#define jit_bnei_l(label, rs, is)	jit_bra_l0((rs), (is), JNEm(label), JNEm(label) )
#define jit_blti_ul(label, rs, is)	jit_bra_l ((rs), (is), JBm(label)		    )
#define jit_blei_ul(label, rs, is)	jit_bra_l0((rs), (is), JBEm(label), JEm(label) )
#define jit_bgti_ul(label, rs, is)	jit_bra_l0((rs), (is), JAm(label), JNEm(label) )
#define jit_bgei_ul(label, rs, is)	jit_bra_l ((rs), (is), JAEm(label)		    )
#define jit_bmsi_l(label, rs, is)	(jit_reduceQ(TEST, (is), (rs)), JNZm(label), _jit.x.pc)
#define jit_bmci_l(label, rs, is)	(jit_reduceQ(TEST, (is), (rs)), JZm(label),  _jit.x.pc)

#define jit_pushr_l(rs) jit_pushr_i(rs)
#define jit_popr_l(rs)  jit_popr_i(rs)

#define jit_pusharg_l(rs) jit_pusharg_i(rs)
#define jit_bltr_l(label, s1, s2)	jit_bra_qr((s1), (s2), JLm(label) )
#define jit_bler_l(label, s1, s2)	jit_bra_qr((s1), (s2), JLEm(label) )
#define jit_bgtr_l(label, s1, s2)	jit_bra_qr((s1), (s2), JGm(label) )
#define jit_bger_l(label, s1, s2)	jit_bra_qr((s1), (s2), JGEm(label) )
#define jit_beqr_l(label, s1, s2)	jit_bra_qr((s1), (s2), JEm(label) )
#define jit_bner_l(label, s1, s2)	jit_bra_qr((s1), (s2), JNEm(label) )
#define jit_bltr_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JBm(label) )
#define jit_bler_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JBEm(label) )
#define jit_bgtr_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JAm(label) )
#define jit_bger_ul(label, s1, s2)	jit_bra_qr((s1), (s2), JAEm(label) )

/* Bool operations.  */
#define jit_bool_qr(d, s1, s2, op)                                      \
        (jit_replace8(d, CMPQrr(s2, s1), op))

#define jit_bool_qi(d, rs, is, op)                                      \
        (jit_replace8(d, CMPQir(is, rs), op))

/* When CMP with 0 can be replaced with TEST */
#define jit_bool_qi0(d, rs, is, op, op0)                                \
        ((is) != 0                                                      \
          ? (jit_replace8(d, CMPQir(is, rs), op))                       \
          : (jit_replace8(d, TESTQrr(rs, rs), op0)))

#define jit_ltr_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETLr  )
#define jit_ler_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETLEr )
#define jit_gtr_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETGr  )
#define jit_ger_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETGEr )
#define jit_eqr_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETEr  )
#define jit_ner_l(d, s1, s2)    jit_bool_qr((d), (s1), (s2), SETNEr )
#define jit_ltr_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETBr  )
#define jit_ler_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETBEr )
#define jit_gtr_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETAr  )
#define jit_ger_ul(d, s1, s2)   jit_bool_qr((d), (s1), (s2), SETAEr )

#define jit_lti_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETLr,  SETSr  )
#define jit_lei_l(d, rs, is)    jit_bool_qi ((d), (rs), (is), SETLEr         )
#define jit_gti_l(d, rs, is)    jit_bool_qi ((d), (rs), (is), SETGr          )
#define jit_gei_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETGEr, SETNSr )
#define jit_eqi_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETEr,  SETEr  )
#define jit_nei_l(d, rs, is)    jit_bool_qi0((d), (rs), (is), SETNEr, SETNEr )
#define jit_lti_ul(d, rs, is)   jit_bool_qi ((d), (rs), (is), SETBr          )
#define jit_lei_ul(d, rs, is)   jit_bool_qi0((d), (rs), (is), SETBEr, SETEr  )
#define jit_gti_ul(d, rs, is)   jit_bool_qi0((d), (rs), (is), SETAr,  SETNEr )
#define jit_gei_ul(d, rs, is)   jit_bool_qi0((d), (rs), (is), SETAEr, INCLr  )

/* Multiplication/division.  */
#define jit_divi_l_(result, d, rs, is)					\
     /* if (d != rax) *sp++ = rax */					\
    (jit_might(jit_reg64(d),  _RAX,		jit_pushr_l(_RAX)),	\
     /* if (d != rcx) *sp++ = rcx */					\
     jit_might(jit_reg64(d),  _RCX,		jit_pushr_l(_RCX)),	\
     /* if (d != rdx) *sp++ = rdx */					\
     jit_might(jit_reg64(d),  _RDX,		jit_pushr_l(_RDX)),	\
     /* if (rs != rax) rax = rs */					\
     jit_might(jit_reg64(rs), _RAX,		MOVQrr(rs, _RAX)),	\
     /* if (rs != rdx) rdx = rs */					\
     jit_might(jit_reg64(rs), _RDX,		MOVQrr(rs, _RDX)),	\
     /* rcx = is */							\
     MOVQir(is, _RCX),							\
     /* rdx >>= 63 */							\
     SARQir(63, _RDX),							\
     /* rdx:rax /= rcx	=> rax = quot, rdx = rem */			\
     IDIVQr(_RCX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg64(d), jit_reg64(result),	MOVQrr(result, d)),	\
     /* if (d != rdx) rdx = --*sp  */					\
     jit_might(jit_reg64(d), _RDX,		jit_popr_l(_RDX)),	\
     /* if (d != rcx) rcx = --*sp  */					\
     jit_might(jit_reg64(d), _RCX,		jit_popr_l(_RCX)),	\
     /* if (d != rax) rax = --*sp  */					\
     jit_might(jit_reg64(d), _RAX,		jit_popr_l(_RAX)))

#define jit_divr_l_(result, d, s1, s2)					\
     /* if (d != rax) *sp++ = rax */					\
    (jit_might(jit_reg64(d),  _RAX,		jit_pushr_l(_RAX)),	\
     /* if (d != rcx) *sp++ = rcx */					\
     jit_might(jit_reg64(d),  _RCX,		jit_pushr_l(_RCX)),	\
     /* if (d != rdx) *sp++ = rdx */					\
     jit_might(jit_reg64(d),  _RDX,		jit_pushr_l(_RDX)),	\
     /* if (s1 == rcx) *sp++ = rcx */					\
     ((jit_reg64(s1) == _RCX) ?			jit_pushr_l(_RCX) : 0),	\
     /* if (s1 != rcx) rcx = s2 */					\
     jit_might(jit_reg64(s2), _RCX,		MOVQrr(s2, _RCX)),	\
     /* if (s1 == rcx) */						\
     ((jit_reg64(s1) == _RCX)						\
	/* rdx = *--sp */						\
	? jit_popr_l(_RDX)						\
	/* else if (s1 == rdx) rdx = s1 */				\
	: jit_might(jit_reg64(s1), _RDX,	MOVQrr(s1, _RDX))),	\
     /* rax = rdx */							\
     MOVQrr(_RDX, _RAX),						\
     /* rdx >>= 63 */							\
     SARQir(63, _RDX),							\
     /* rdx:rax /= rcx	=> rax = quot, rdx = rem */			\
     IDIVQr(_RCX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg64(d), jit_reg64(result),	MOVQrr(result, d)),	\
     /* if (d != edx) edx = --*sp  */					\
     jit_might(jit_reg64(d), _RDX,		jit_popr_l(_RDX)),	\
     /* if (d != rcx) rcx = --*sp  */					\
     jit_might(jit_reg64(d), _RCX,		jit_popr_l(_RCX)),	\
     /* if (d != rax) rax = --*sp  */					\
     jit_might(jit_reg64(d), _RAX,		jit_popr_l(_RAX)))

#define jit_divi_ul_(result, d, rs, is)					\
     /* if (d != rax) *sp++ = rax */					\
    (jit_might(jit_reg64(d),  _RAX,		jit_pushr_l(_RAX)),	\
     /* if (d != rcx) *sp++ = rcx */					\
     jit_might(jit_reg64(d),  _RCX,		jit_pushr_l(_RCX)),	\
     /* if (d != rdx) *sp++ = rdx */					\
     jit_might(jit_reg64(d),  _RDX,		jit_pushr_l(_RDX)),	\
     /* if (rs != rax) rax = rs */					\
     jit_might(jit_reg64(rs), _RAX,		MOVQrr(rs, _RAX)),	\
     /* rcx = is */							\
     MOVQir(is, _RCX),							\
     /* rdx ^= rdx */							\
     XORQrr(_RDX, _RDX),						\
     /* rdx:rax /= rcx (unsigned) => rax = quot, rdx = rem */		\
     DIVQr(_RCX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg64(d), jit_reg64(result),	MOVQrr(result, d)),	\
     /* if (d != rdx) rdx = --*sp  */					\
     jit_might(jit_reg64(d), _RDX,		jit_popr_l(_RDX)),	\
     /* if (d != rcx) rcx = --*sp  */					\
     jit_might(jit_reg64(d), _RCX,		jit_popr_l(_RCX)),	\
     /* if (d != rax) rax = --*sp  */					\
     jit_might(jit_reg64(d), _RAX,		jit_popr_l(_RAX)))

#define jit_divr_ul_(result, d, s1, s2)					\
     /* if (d != rax) *sp++ = rax */					\
    (jit_might(jit_reg64(d),  _RAX,		jit_pushr_l(_RAX)),	\
     /* if (d != rcx) *sp++ = rcx */					\
     jit_might(jit_reg64(d),  _RCX,		jit_pushr_l(_RCX)),	\
     /* if (d != rdx) *sp++ = rdx */					\
     jit_might(jit_reg64(d),  _RDX,		jit_pushr_l(_RDX)),	\
     /* if (s1 == rcx) *sp++ = rcx */					\
     ((jit_reg64(s1) == _RCX) ?			jit_pushr_l(_RCX) : 0),	\
     /* if (s1 != rcx) rcx = s2 */					\
     jit_might(jit_reg64(s2), _RCX,		MOVQrr(s2, _RCX)),	\
     /* if (s1 == rcx) */						\
     ((jit_reg64(s1) == _RCX)						\
	/* rax = *--sp */						\
	? jit_popr_l(_RAX)						\
	/* else if (s1 == rax) rax = s1 */				\
	: jit_might(jit_reg64(s1), _RAX,	MOVQrr(s1, _RAX))),	\
     /* rdx ^= rdx */							\
     XORQrr(_RDX, _RDX),						\
     /* rdx:rax /= rcx (unsigned) => rax = quot, rdx = rem */		\
     DIVQr(_RCX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg64(d), jit_reg64(result), MOVQrr(result, d)),	\
     /* if (d != rdx) rdx = --*sp  */					\
     jit_might(jit_reg64(d), _RDX,		jit_popr_l(_RDX)),	\
     /* if (d != rcx) rcx = --*sp  */					\
     jit_might(jit_reg64(d), _RCX,		jit_popr_l(_RCX)),	\
     /* if (d != rax) rax = --*sp  */					\
     jit_might(jit_reg64(d), _RAX,		jit_popr_l(_RAX)))

#define jit_muli_i(d, rs, is)		jit_muli_l((d), (rs), (long)(int)(is))
#define jit_muli_ui(d, rs, is)		jit_muli_l((d), (rs), (_ul)(_ui)(is))

#define jit_muli_l(d, rs, is)						\
    /* if (is != 0) */							\
    ((is)								\
	/* if (is != 1) */						\
	? (((is) != 1)							\
	    /* if (is != -1) */						\
	    ? (((is) != -1)						\
		/* if (int32_p(is)) { */				\
		? ((_s32P((long)(is)))					\
		    /* if (d != rs) d = rs; <fits in imm int32> */	\
		    ? ((((d) != (rs)) ? MOVQrr((rs), (d)) : 0),		\
			/* d *= is; } */				\
			IMULQir((is), (d)))				\
		    /* if (d != rs) <does not fit in imm int32> */	\
		    : (((d) != (rs))					\
			/* d = is; d *= rs; */				\
			? (MOVQir((is), (d)), IMULQrr((rs), (d)))	\
			/* else tmp = is, d *= tmp;*/			\
			: (MOVQir((is), JIT_REXTMP),			\
			   IMULQrr(JIT_REXTMP, (d)))))			\
		/* is == -1; */						\
		: jit_negr_l((d), (rs)))				\
	    /* is == 1 */						\
	    : jit_movr_l((d), (rs)))					\
	/* is == 0 */							\
	: XORQrr((d), (d)))

#define jit_mulr_l(d, s1, s2)						\
    /* if (d == s1) */							\
    (((d) == (s1))							\
	/* d *= s2; */							\
	? IMULQrr((s2), (d))						\
	/* else { if (d != s2) d = s2; */				\
	: (((d) != (s2) ? MOVQrr((s2), (d)) : 0),			\
	/* d *= s1; } */						\
	   IMULQrr((s1), (d))))

/* As far as low bits are concerned, signed and unsigned multiplies are
   exactly the same. */
#define jit_muli_ul(d, rs, is)		jit_muli_l(d, rs, is)
#define jit_mulr_ul(d, s1, s2)		jit_mulr_l(d, s1, s2)

/*  Instruction format is:
 *	imul reg64/mem64
 *  and the result is stored in %rdx:%rax
 *  %rax = low 64 bits
 *  %rdx = high 64 bits
 */
/*
jit_muli_l_(rs, is)
{
    if (jit_reg64(rs) == _RAX) {
	MOVQir(is, _RDX);
	IMULQr(_RDX);
    }
    else {
	MOVQir(is, _RAX);
	IMULQr(rs);
    }
}
 */
#define jit_muli_l_(rs, is)						\
    ((jit_reg64(rs) == _RAX)						\
	? (MOVQir(is, _RDX),						\
	   IMULQr(_RDX))						\
	: (MOVQir(is, _RAX),						\
	   IMULQr(rs)))

/*
jit_hmuli_l(d, rs, is)	{
    if (jit_reg64(d) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_l_(rs, is);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(d) == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_muli_l_(rs, is);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_muli_l_(rs, is);
	MOVQrr(_RDX, d);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}
 */
#define jit_hmuli_l(d, rs, is)						\
    ((jit_reg64(d) == _RDX)						\
	? (MOVQrr(_RAX, JIT_REXTMP),					\
	   jit_muli_l_(rs, is),						\
	   MOVQrr(JIT_REXTMP, _RAX))					\
	: ((jit_reg64(d) == _RAX)					\
	   ? (MOVQrr(_RDX, JIT_REXTMP),					\
	      jit_muli_l_(rs, is),					\
	      MOVQrr(_RDX, _RAX),					\
	      MOVQrr(JIT_REXTMP, _RDX))					\
	   : (MOVQrr(_RDX, JIT_REXTMP),					\
	      jit_pushr_l(_RAX),					\
	      jit_muli_l_(rs, is),					\
	      MOVQrr(_RDX, d),						\
	      jit_popr_l(_RAX),						\
	      MOVQrr(JIT_REXTMP, _RDX))))

/*
jit_mulr_l_(s1, s2)
{
    if (jit_reg64(s2) == _RAX)
	IMULQr(s1);
    else if (jit_reg64(s1) == _RAX)
	IMULQr(s2);
    else {
	MOVQrr(s2, _RAX);
	IMULQr(s1);
    }
}
 */
#define jit_mulr_l_(s1, s2)						\
    ((jit_reg64(s2) == _RAX)						\
	? IMULQr(s1)							\
	: ((jit_reg64(s1) == _RAX)					\
	    ? IMULQr(s2)						\
	    : (MOVQrr(s2, _RAX),					\
	       IMULQr(s1))))

/*
jit_hmulr_l(d, s1, s2)	{
    if (jit_reg64(d) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_l_(s1, s2);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(d) == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_mulr_i_(s1, s2);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_mulr_l_(s1, s2);
	MOVQrr(_RDX, d);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}
 */
#define jit_hmulr_l(d, s1, s2)						\
    ((jit_reg64(d) == _RDX)						\
	? (MOVQrr(_RAX, JIT_REXTMP),					\
	   jit_mulr_l_(s1, s2),						\
	   MOVQrr(JIT_REXTMP, _RAX))					\
	: ((jit_reg64(d) == _RAX)					\
	    ? (MOVQrr(_RDX, JIT_REXTMP),				\
	       jit_mulr_l_(s1, s2),					\
	       MOVQrr(_RDX, _RAX),					\
	       MOVQrr(JIT_REXTMP, _RDX))				\
	    : (MOVQrr(_RDX, JIT_REXTMP),				\
	       jit_pushr_l(_RAX),					\
	       jit_mulr_l_(s1, s2),					\
	       MOVQrr(_RDX, d),						\
	       jit_popr_l(_RAX),					\
	       MOVQrr(JIT_REXTMP, _RDX))))

/*  Instruction format is:
 *	mul reg64/mem64
 *  and the result is stored in %rdx:%rax
 *  %rax = low 64 bits
 *  %rdx = high 64 bits
 */
/*
jit_muli_ul_(rs, is)
{
    if (jit_reg64(rs) == _RAX) {
	MOVQir(is, _RDX);
	MULQr(_RDX);
    }
    else {
	MOVQir(is, _RAX);
	MULQr(rs);
    }
}
 */
#define jit_muli_ul_(rs, is)						\
    ((jit_reg64(rs) == _RAX)						\
	? (MOVQir(is, _RDX),						\
	   MULQr(_RDX))							\
	: (MOVQir(is, _RAX),						\
	   MULQr(rs)))

/*
jit_hmuli_ul(d, rs, is)	{
    if (jit_reg64(d) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_muli_ul_(rs, is);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(d) == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_muli_ul_(rs, is);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_muli_ul_(rs, is);
	MOVQrr(_RDX, d);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}
 */
#define jit_hmuli_ul(d, rs, is)						\
    ((jit_reg64(d) == _RDX)						\
	? (MOVQrr(_RAX, JIT_REXTMP),					\
	   jit_muli_ul_(rs, is),					\
	   MOVQrr(JIT_REXTMP, _RAX))					\
	: ((jit_reg64(d) == _RAX)					\
	   ? (MOVQrr(_RDX, JIT_REXTMP),					\
	      jit_muli_ul_(rs, is),					\
	      MOVQrr(_RDX, _RAX),					\
	      MOVQrr(JIT_REXTMP, _RDX))					\
	   : (MOVQrr(_RDX, JIT_REXTMP),					\
	      jit_pushr_l(_RAX),					\
	      jit_muli_ul_(rs, is),					\
	      MOVQrr(_RDX, d),						\
	      jit_popr_l(_RAX),						\
	      MOVQrr(JIT_REXTMP, _RDX))))

/*
jit_mulr_ul_(s1, s2)
{
    if (jit_reg64(s2) == _RAX)
	MULQr(s1);
    else if (jit_reg64(s1) == _RAX)
	MULQr(s2);
    else {
	MOVQrr(s2, _RAX);
	MULQr(s1);
    }
}
 */
#define jit_mulr_ul_(s1, s2)						\
    ((jit_reg64(s2) == _RAX)						\
	? MULQr(s1)							\
	: ((jit_reg64(s1) == _RAX)					\
	    ? MULQr(s2)							\
	    : (MOVQrr(s2, _RAX),					\
	       MULQr(s1))))

/*
jit_hmulr_ul(d, s1, s2)	{
    if (jit_reg64(d) == _RDX) {
	MOVQrr(_RAX, JIT_REXTMP);
	jit_mulr_ul_(s1, s2);
	MOVQrr(JIT_REXTMP, _RAX);
    }
    else if (jit_reg64(d) == _RAX) {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_mulr_ul_(s1, s2);
	MOVQrr(_RDX, _RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
    else {
	MOVQrr(_RDX, JIT_REXTMP);
	jit_pushr_l(_RAX);
	jit_mulr_ul_(s1, s2);
	MOVQrr(_RDX, d);
	jit_popr_l(_RAX);
	MOVQrr(JIT_REXTMP, _RDX);
    }
}
 */
#define jit_hmulr_ul(d, s1, s2)						\
    ((jit_reg64(d) == _RDX)						\
	? (MOVQrr(_RAX, JIT_REXTMP),					\
	   jit_mulr_ul_(s1, s2),					\
	   MOVQrr(JIT_REXTMP, _RAX))					\
	: ((jit_reg64(d) == _RAX)					\
	    ? (MOVQrr(_RDX, JIT_REXTMP),				\
	       jit_mulr_ul_(s1, s2),					\
	       MOVQrr(_RDX, _RAX),					\
	       MOVQrr(JIT_REXTMP, _RDX))				\
	    : (MOVQrr(_RDX, JIT_REXTMP),				\
	       jit_pushr_l(_RAX),					\
	       jit_mulr_ul_(s1, s2),					\
	       MOVQrr(_RDX, d),						\
	       jit_popr_l(_RAX),					\
	       MOVQrr(JIT_REXTMP, _RDX))))

#define jit_divi_l(d, rs, is)	jit_divi_l_(_RAX, (d), (rs), (is))
#define jit_divi_ul(d, rs, is)	jit_divi_ul_(_RAX, (d), (rs), (is))
#define jit_modi_l(d, rs, is)	jit_divi_l_(_RDX, (d), (rs), (is))
#define jit_modi_ul(d, rs, is)	jit_divi_ul_(_RDX, (d), (rs), (is))
#define jit_divr_l(d, s1, s2)	jit_divr_l_(_RAX, (d), (s1), (s2))
#define jit_divr_ul(d, s1, s2)	jit_divr_ul_(_RAX, (d), (s1), (s2))
#define jit_modr_l(d, s1, s2)	jit_divr_l_(_RDX, (d), (s1), (s2))
#define jit_modr_ul(d, s1, s2)	jit_divr_ul_(_RDX, (d), (s1), (s2))

#endif /* __lightning_core_h */

