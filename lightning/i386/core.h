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



#ifndef __lightning_core_i386_h
#define __lightning_core_i386_h

#define JIT_FP			_EBP
#define JIT_SP			_ESP
#define JIT_RET			_EAX


/* 3-parameter operation */
#define jit_opr_(d, s1, s2, op1d, op2d)					\
	( (s2 == d) ? op1d :						\
	  (  ((s1 == d) ? (void)0 : (void)MOVLrr(s1, d)), op2d )	\
	)

/* 3-parameter operation, with immediate */
#define jit_op_(d, s1, op2d)				\
	((s1 == d) ? op2d : (MOVLrr(s1, d), op2d))

/* 3-parameter operation, optimizable */
#define jit_opo_(d, s1, s2, op1d, op2d, op12d)		\
	((s2 == d) ? op2d : 				\
	((s1 == d) ? op1d : op12d))

/* 3-parameter operation, optimizable, with immediate */
#define jit_opi_(d, rs, opdi, opdri)			\
	((rs == d) ? opdi : opdri)

/* For LT, LE, ... */
#define jit_replace8(d, cmp, op)				\
	(jit_check8(d)						\
	 ? ((cmp),						\
             MOVLir(0, (d)),					\
             op(_rR(d) | _AL))					\
	 : (jit_pushr_i(_EAX), (cmp),				\
            MOVLir(0, _EAX),					\
            op(_AL), MOVLrr(_EAX, (d)), jit_popr_i(_EAX)))

#define jit_bool_r(d, s1, s2, op)					\
	(jit_replace8(d, CMPLrr(s2, s1), op))

#define jit_bool_i(d, rs, is, op)					\
	(jit_replace8(d, CMPLir(is, rs), op))

/* When CMP with 0 can be replaced with TEST */
#define jit_bool_i0(d, rs, is, op, op0)					\
	((is) != 0							\
	  ? (jit_replace8(d, CMPLir(is, rs), op)) 			\
	  : (jit_replace8(d, TESTLrr(rs, rs), op0)))

/* For BLT, BLE, ... */
#define jit_bra_r(s1, s2, op)		(CMPLrr(s2, s1), op, _jit.x.pc)
#define jit_bra_i(rs, is, op)		(CMPLir(is, rs), op, _jit.x.pc)

/* When CMP with 0 can be replaced with TEST */
#define jit_bra_i0(rs, is, op, op0)					\
	( (is) == 0 ? (TESTLrr(rs, rs), op0, _jit.x.pc) : (CMPLir(is, rs), op, _jit.x.pc))

/* Reduce arguments of XOR/OR/TEST */
#define jit_reduce_(op)	op
#define jit_reduce(op, is, rs)							\
	(_u8P(is) && jit_check8(rs) ? jit_reduce_(op##Bir(is, jit_reg8(rs))) :	\
	(_u16P(is) && JIT_CAN_16 ? jit_reduce_(op##Wir(is, jit_reg16(rs))) :			\
	jit_reduce_(op##Lir(is, rs)) ))

/* Helper macros for MUL/DIV/IDIV */
#define jit_might(d, s1, op)					\
	((s1 == d) ? 0 : op)

#define jit_muli_ui_(is, rs)						\
	(MOVLir(is, (jit_reg32(rs) == _EAX) ? _EDX : _EAX),		\
	 MULLr(jit_reg32(rs) == _EAX ? _EDX : rs))

#define jit_divi_i_(result, d, rs, is)					\
     /* if (d != eax) *sp++ = eax */					\
    (jit_might(jit_reg32(d),  _EAX,		jit_pushr_i(_EAX)),	\
     /* if (d != ecx) *sp++ = ecx */					\
     jit_might(jit_reg32(d),  _ECX,		jit_pushr_i(_ECX)),	\
     /* if (d != edx) *sp++ = edx */					\
     jit_might(jit_reg32(d),  _EDX,		jit_pushr_i(_EDX)),	\
     /* if (rs != eax) eax = rs */					\
     jit_might(jit_reg32(rs), _EAX,		MOVLrr(rs, _EAX)),	\
     /* if (rs != edx) edx = rs */					\
     jit_might(jit_reg32(rs), _EDX,		MOVLrr(rs, _EDX)),	\
     /* ecx = is */							\
     MOVLir(is, _ECX),							\
     /* edx >>= 31 */							\
     SARLir(31, _EDX),							\
     /* edx:eax /= ecx	=> eax = quot, edx = rem */			\
     IDIVLr(_ECX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg32(d), jit_reg32(result),	MOVLrr(result, d)),	\
     /* if (d != edx) edx = --*sp  */					\
     jit_might(jit_reg32(d), _EDX,		jit_popr_i(_EDX)),	\
     /* if (d != ecx) ecx = --*sp  */					\
     jit_might(jit_reg32(d), _ECX,		jit_popr_i(_ECX)),	\
     /* if (d != eax) eax = --*sp  */					\
     jit_might(jit_reg32(d), _EAX,		jit_popr_i(_EAX)))

#define jit_divr_i_(result, d, s1, s2)					\
     /* if (d != eax) *sp++ = eax */					\
    (jit_might(jit_reg32(d),  _EAX,		jit_pushr_i(_EAX)),	\
     /* if (d != ecx) *sp++ = ecx */					\
     jit_might(jit_reg32(d),  _ECX,		jit_pushr_i(_ECX)),	\
     /* if (d != edx) *sp++ = edx */					\
     jit_might(jit_reg32(d),  _EDX,		jit_pushr_i(_EDX)),	\
     /* if (s1 == ecx) *sp++ = ecx */					\
     ((jit_reg32(s1) == _ECX) ?			jit_pushr_i(_ECX) : 0),	\
     /* if (s1 != ecx) ecx = s2 */					\
     jit_might(jit_reg32(s2), _ECX,		MOVLrr(s2, _ECX)),	\
     /* if (s1 == ecx) */						\
     ((jit_reg32(s1) == _ECX)						\
	/* edx = *--sp */						\
	? jit_popr_i(_EDX)						\
	/* else if (s1 == edx) edx = s1 */				\
	: jit_might(jit_reg32(s1), _EDX,	MOVLrr(s1, _EDX))),	\
     /* eax = edx */							\
     MOVLrr(_EDX, _EAX),						\
     /* edx >>= 31 */							\
     SARLir(31, _EDX),							\
     /* edx:eax /= ecx	=> eax = quot, edx = rem */			\
     IDIVLr(_ECX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg32(d), jit_reg32(result),	MOVLrr(result, d)),	\
     /* if (d != edx) edx = --*sp  */					\
     jit_might(jit_reg32(d), _EDX,		jit_popr_i(_EDX)),	\
     /* if (d != ecx) ecx = --*sp  */					\
     jit_might(jit_reg32(d), _ECX,		jit_popr_i(_ECX)),	\
     /* if (d != eax) eax = --*sp  */					\
     jit_might(jit_reg32(d), _EAX,		 jit_popr_i(_EAX)))

#define jit_divi_ui_(result, d, rs, is)					\
     /* if (d != eax) *sp++ = eax */					\
    (jit_might(jit_reg32(d),  _EAX,		jit_pushr_i(_EAX)),	\
     /* if (d != ecx) *sp++ = ecx */					\
     jit_might(jit_reg32(d),  _ECX,		jit_pushr_i(_ECX)),	\
     /* if (d != edx) *sp++ = edx */					\
     jit_might(jit_reg32(d),  _EDX,		jit_pushr_i(_EDX)),	\
     /* if (rs != eax) eax = rs */					\
     jit_might(jit_reg32(rs), _EAX,		MOVLrr(rs, _EAX)),	\
     /* ecx = is */							\
     MOVLir(is, _ECX),							\
     /* edx ^= edx */							\
     XORLrr(_EDX, _EDX),						\
     /* edx:eax /= ecx (unsigned) => eax = quot, edx = rem */		\
     DIVLr(_ECX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg32(d), jit_reg32(result),	MOVLrr(result, d)),	\
     /* if (d != edx) edx = --*sp  */					\
     jit_might(jit_reg32(d), _EDX,		jit_popr_i(_EDX)),	\
     /* if (d != ecx) ecx = --*sp  */					\
     jit_might(jit_reg32(d), _ECX,		jit_popr_i(_ECX)),	\
     /* if (d != eax) eax = --*sp  */					\
     jit_might(jit_reg32(d), _EAX,		jit_popr_i(_EAX)))

#define jit_divr_ui_(result, d, s1, s2)					\
     /* if (d != eax) *sp++ = eax */					\
    (jit_might(jit_reg32(d),  _EAX,		jit_pushr_i(_EAX)),	\
     /* if (d != ecx) *sp++ = ecx */					\
     jit_might(jit_reg32(d),  _ECX,		jit_pushr_i(_ECX)),	\
     /* if (d != edx) *sp++ = edx */					\
     jit_might(jit_reg32(d),  _EDX,		jit_pushr_i(_EDX)),	\
     /* if (s1 == ecx) *sp++ = ecx */					\
     ((jit_reg32(s1) == _ECX) ?			jit_pushr_i(_ECX) : 0),	\
     /* if (s1 != ecx) ecx = s2 */					\
     jit_might(jit_reg32(s2), _ECX,		MOVLrr(s2, _ECX)),	\
     /* if (s1 == ecx) */						\
     ((jit_reg32(s1) == _ECX)						\
	/* eax = *--sp */						\
	? jit_popr_i(_EAX)						\
	/* else if (s1 == eax) eax = s1 */				\
	: jit_might(jit_reg32(s1), _EAX,	MOVLrr(s1, _EAX))),	\
     /* edx ^= edx */							\
     XORLrr(_EDX, _EDX),						\
     /* edx:eax /= ecx (unsigned) => eax = quot, edx = rem */		\
     DIVLr(_ECX),							\
     /* if (d != result) d = result */					\
     jit_might(jit_reg32(d), jit_reg32(result), MOVLrr(result, d)),	\
     /* if (d != edx) edx = --*sp  */					\
     jit_might(jit_reg32(d), _EDX,		jit_popr_i(_EDX)),	\
     /* if (d != ecx) ecx = --*sp  */					\
     jit_might(jit_reg32(d), _ECX,		jit_popr_i(_ECX)),	\
     /* if (d != eax) eax = --*sp  */					\
     jit_might(jit_reg32(d), _EAX,		jit_popr_i(_EAX)))


/* ALU */
#define jit_addi_i(d, rs, is)	jit_opi_((d), (rs),       ADDLir((is), (d)), 			LEALmr((is), (rs), 0, 0, (d))  )
#define jit_addr_i(d, s1, s2)	jit_opo_((d), (s1), (s2), ADDLrr((s2), (d)), ADDLrr((s1), (d)), LEALmr(0, (s1), (s2), 1, (d))  )
#define jit_addci_i(d, rs, is)	jit_op_ ((d), (rs),       ADDLir((is), (d)) 		       )
#define jit_addcr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), ADDLrr((s1), (d)), ADDLrr((s2), (d)) )
#define jit_addxi_i(d, rs, is)	jit_op_ ((d), (rs),       ADCLir((is), (d)) 		       )
#define jit_addxr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), ADCLrr((s1), (d)), ADCLrr((s2), (d)) )
#define jit_andi_i(d, rs, is)	jit_op_ ((d), (rs),       ANDLir((is), (d)) 		       )
#define jit_andr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), ANDLrr((s1), (d)), ANDLrr((s2), (d)) )
#define jit_orr_i(d, s1, s2)	jit_opr_((d), (s1), (s2),  ORLrr((s1), (d)),  ORLrr((s2), (d)) )
#define jit_subr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), (SUBLrr((s1), (d)), NEGLr(d)),	SUBLrr((s2), (d))	       )
#define jit_subcr_i(d, s1, s2)	jit_subr_i((d), (s1), (s2))
#define jit_subxr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), SBBLrr((s1), (d)), SBBLrr((s2), (d)) )
#define jit_subxi_i(d, rs, is)	jit_op_ ((d), (rs),       SBBLir((is), (d)) 		       )
#define jit_xorr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), XORLrr((s1), (d)), XORLrr((s2), (d)) )


/* These can sometimes use byte or word versions! */
#define jit_ori_i(d, rs, is)	jit_op_ ((d), (rs),        jit_reduce(OR, (is), (d))	       )
#define jit_xori_i(d, rs, is)	jit_op_ ((d), (rs),        jit_reduce(XOR, (is), (d))	       )

#define jit_mulr_i(d, s1, s2)	jit_opr_((d), (s1), (s2), IMULLrr((s1), (d)), IMULLrr((s2), (d)) )

/* As far as low bits are concerned, signed and unsigned multiplies are
   exactly the same. */
#define jit_mulr_ui(d, s1, s2)	jit_opr_((d), (s1), (s2), IMULLrr((s1), (d)), IMULLrr((s2), (d)) )

/*  Instruction format is:
 *	imul reg32/mem32
 *  and the result is stored in %edx:%eax
 *  %eax = low 32 bits
 *  %edx = high 32 bits
 */
/*
jit_muli_i_(rs, is)
{
    if (jit_reg32(rs) == _EAX) {
	MOVLir(is, _EDX);
	IMULLr(_EDX);
    }
    else {
	MOVLir(is, _EAX);
	IMULLr(rs);
    }
}
 */
#define jit_muli_i_(rs, is)						\
    /* if (rs == eax) { */						\
    ((jit_reg32(rs) == _EAX)						\
    /*	edx = is; */							\
	? (MOVLir(is, _EDX),						\
    /* edx:eax = edx * eax;	<eax = low, edx = high> */		\
	   IMULLr(_EDX))						\
    /* else { */							\
    /*	eax = is; */							\
	: (MOVLir(is, _EAX),						\
    /* edx:eax = rs * eax;	<eax = low, edx = high> */		\
	   IMULLr(rs)))

/*
jit_hmuli_i(d, rs, is)	{
    if (jit_reg32(d) == _EDX) {
	jit_pushr_i(_EAX);
	jit_muli_i_(rs, is);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(d) == _EAX) {
	jit_pushr_i(_EDX);
	jit_muli_i_(rs, is);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_muli_i_(rs, is);
	MOVLrr(_EDX, d);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}
 */
#define jit_hmuli_i(d, rs, is)						\
    /* if (d == edx) { */						\
    ((jit_reg32(d) == _EDX)						\
    /* *sp++ = eax; */							\
	? (jit_pushr_i(_EAX),						\
    /* edx:eax = rs * is;	<eax = low, edx = high> */		\
	   jit_muli_i_(rs, is),						\
    /* eax = *--sp; */							\
	   jit_popr_i(_EAX))						\
   /* else if (d == eax) { */						\
	: ((jit_reg32(d) == _EAX)					\
    /* *sp++ = edx; */							\
	   ? (jit_pushr_i(_EDX),					\
    /* edx:eax = rs * is;	<eax = low, edx = high> */		\
	      jit_muli_i_(rs, is),					\
    /* eax = edx; */							\
	      MOVLrr(_EDX, _EAX),					\
    /* edx = *--sp; */							\
	      jit_popr_i(_EDX))						\
   /* else { */								\
    /* *sp++ = edx; */							\
	   : (jit_pushr_i(_EDX),					\
    /* *sp++ = eax; */							\
	      jit_pushr_i(_EAX),					\
    /* edx:eax = rs * is;	<eax = low, edx = high> */		\
	      jit_muli_i_(rs, is),					\
    /* d = edx; */							\
	      MOVLrr(_EDX, d),						\
    /* edx = *--sp; */							\
	      jit_popr_i(_EAX),						\
    /* eax = *--sp; */							\
	      jit_popr_i(_EDX))))

/*
jit_mulr_i_(s1, s2)
{
    if (jit_reg32(s2) == _EAX)
	IMULLr(s1);
    else if (jit_reg32(s1) == _EAX)
	IMULLr(s2);
    else {
	MOVLrr(s2, _EAX);
	IMULLr(s1);
    }
}
 */
#define jit_mulr_i_(s1, s2)						\
    /* if (s2 == eax) */						\
    ((jit_reg32(s2) == _EAX)						\
    /*	edx:eax = s1 * s2; */						\
	? IMULLr(s1)							\
    /* else if (s1 == eax) */						\
	: ((jit_reg32(s1) == _EAX)					\
    /*	edx:eax = s2 * s1; */						\
	    ? IMULLr(s2)						\
    /* else { */							\
    /*	eax = s2; */							\
	    : (MOVLrr(s2, _EAX),					\
    /*	edx:eax = s1 * s2; } */						\
	       IMULLr(s1))))

/*
jit_hmulr_i(d, s1, s2)	{
    if (jit_reg32(d) == _EDX) {
	jit_pushr_i(_EAX);
	jit_mulr_i_(s1, s2);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(d) == _EAX) {
	jit_pushr_i(_EDX);
	jit_mulr_i_(s1, s2);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_mulr_i_(s1, s2);
	MOVLrr(_EDX, d);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}
 */
#define jit_hmulr_i(d, s1, s2)						\
    /* if (d == edx) { */						\
    ((jit_reg32(d) == _EDX)						\
    /*	*sp++ = eax; */							\
	? (jit_pushr_i(_EAX),						\
    /*	edx:eax = s1 * s2;	<eax = low, edx = high> */		\
	   jit_mulr_i_(s1, s2),						\
    /*	eax = *--sp; */							\
	   jit_popr_i(_EAX))						\
    /* } else if (d == eax) { */					\
	: ((jit_reg32(d) == _EAX)					\
    /*	*sp++ = edx; */							\
	    ? (jit_pushr_i(_EDX),					\
    /*	edx:eax = s1 * s2;	<eax = low, edx = high> */		\
	       jit_mulr_i_(s1, s2),					\
    /*	eax = edx; */							\
	       MOVLrr(_EDX, _EAX),					\
    /*	edx = *--sp; */							\
	       jit_popr_i(_EDX))					\
    /* } else { */							\
    /*	*sp++ = edx; */							\
	    : (jit_pushr_i(_EDX),					\
    /*	*sp++ = eax; */							\
	       jit_pushr_i(_EAX),					\
    /*	edx:eax = s1 * s2;	<eax = low, edx = high> */		\
	       jit_mulr_i_(s1, s2),					\
    /*	d = eax; */							\
	       MOVLrr(_EDX, d),						\
    /*	eax = *--sp; */							\
	       jit_popr_i(_EAX),					\
    /*	edx = *--sp; */							\
	       jit_popr_i(_EDX))))

/*  Instruction format is:
 *	mul reg32/mem32
 *  and the result is stored in %edx:%eax
 *  %eax = low 32 bits
 *  %edx = high 32 bits
 */
/*
jit_muli_ui_(rs, is)
{
    if (jit_reg32(rs) == _EAX) {
	MOVLir(is, _EDX);
	MULLr(_EDX);
    }
    else {
	MOVLir(is, _EAX);
	MULLr(rs);
    }
}
 */
#define jit_muli_ui_(rs, is)						\
    /* if (rs == eax) { */						\
    ((jit_reg32(rs) == _EAX)						\
    /*	edx = is; */							\
	? (MOVLir(is, _EDX),						\
    /* edx:eax = edx * eax;	<eax = low, edx = high> */		\
	   MULLr(_EDX))							\
    /* else { */							\
    /*	eax = is; */							\
	: (MOVLir(is, _EAX),						\
    /* edx:eax = rs * eax;	<eax = low, edx = high> */		\
	   MULLr(rs)))

/*
jit_hmuli_ui(d, rs, is)	{
    if (jit_reg32(d) == _EDX) {
	jit_pushr_i(_EAX);
	jit_muli_ui_(rs, is);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(d) == _EAX) {
	jit_pushr_i(_EDX);
	jit_muli_ui_(rs, is);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_muli_ui_(rs, is);
	MOVLrr(_EDX, d);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}
 */
#define jit_hmuli_ui(d, rs, is)						\
    /* if (d == edx) { */						\
    ((jit_reg32(d) == _EDX)						\
    /* *sp++ = eax; */							\
	? (jit_pushr_i(_EAX),						\
    /* edx:eax = rs * is;	<eax = low, edx = high> */		\
	   jit_muli_ui_(rs, is),					\
    /* eax = *--sp; */							\
	   jit_popr_i(_EAX))						\
   /* else if (d == eax) { */						\
	: ((jit_reg32(d) == _EAX)					\
    /* *sp++ = edx; */							\
	   ? (jit_pushr_i(_EDX),					\
    /* edx:eax = rs * is;	<eax = low, edx = high> */		\
	      jit_muli_ui_(rs, is),					\
    /* eax = edx; */							\
	      MOVLrr(_EDX, _EAX),					\
    /* edx = *--sp; */							\
	      jit_popr_i(_EDX))						\
   /* else { */								\
    /* *sp++ = edx; */							\
	   : (jit_pushr_i(_EDX),					\
    /* *sp++ = eax; */							\
	      jit_pushr_i(_EAX),					\
    /* edx:eax = rs * is;	<eax = low, edx = high> */		\
	      jit_muli_ui_(rs, is),					\
    /* d = edx; */							\
	      MOVLrr(_EDX, d),						\
    /* edx = *--sp; */							\
	      jit_popr_i(_EAX),						\
    /* eax = *--sp; */							\
	      jit_popr_i(_EDX))))

/*
jit_mulr_ui_(s1, s2)
{
    if (jit_reg32(s2) == _EAX)
	MULLr(s1);
    else if (jit_reg32(s1) == _EAX)
	MULLr(s2);
    else {
	MOVLrr(s2, _EAX);
	MULLr(s1);
    }
}
 */
#define jit_mulr_ui_(s1, s2)						\
    /* if (s2 == eax) */						\
    ((jit_reg32(s2) == _EAX)						\
    /*	edx:eax = s1 * s2; */						\
	? MULLr(s1)							\
    /* else if (s1 == eax) */						\
	: ((jit_reg32(s1) == _EAX)					\
    /*	edx:eax = s2 * s1; */						\
	    ? MULLr(s2)							\
    /* else { */							\
    /*	eax = s2; */							\
	    : (MOVLrr(s2, _EAX),					\
    /*	edx:eax = s1 * s2; } */						\
	       MULLr(s1))))

/*
jit_hmulr_ui(d, s1, s2)	{
    if (jit_reg32(d) == _EDX) {
	jit_pushr_i(_EAX);
	jit_mulr_ui_(s1, s2);
	jit_popr_i(_EAX);
    }
    else if (jit_reg32(d) == _EAX) {
	jit_pushr_i(_EDX);
	jit_mulr_ui_(s1, s2);
	MOVLrr(_EDX, _EAX);
	jit_popr_i(_EDX);
    }
    else {
	jit_pushr_i(_EDX);
	jit_pushr_i(_EAX);
	jit_mulr_ui_(s1, s2);
	MOVLrr(_EDX, d);
	jit_popr_i(_EAX);
	jit_popr_i(_EDX);
    }
}
 */
#define jit_hmulr_ui(d, s1, s2)						\
    /* if (d == edx) { */						\
    ((jit_reg32(d) == _EDX)						\
    /*	*sp++ = eax; */							\
	? (jit_pushr_i(_EAX),						\
    /*	edx:eax = s1 * s2;	<eax = low, edx = high> */		\
	   jit_mulr_ui_(s1, s2),					\
    /*	eax = *--sp; */							\
	   jit_popr_i(_EAX))						\
    /* } else if (d == eax) { */					\
	: ((jit_reg32(d) == _EAX)					\
    /*	*sp++ = edx; */							\
	    ? (jit_pushr_i(_EDX),					\
    /*	edx:eax = s1 * s2;	<eax = low, edx = high> */		\
	       jit_mulr_ui_(s1, s2),					\
    /*	eax = edx; */							\
	       MOVLrr(_EDX, _EAX),					\
    /*	edx = *--sp; */							\
	       jit_popr_i(_EDX))					\
    /* } else { */							\
    /*	*sp++ = edx; */							\
	    : (jit_pushr_i(_EDX),					\
    /*	*sp++ = eax; */							\
	       jit_pushr_i(_EAX),					\
    /*	edx:eax = s1 * s2;	<eax = low, edx = high> */		\
	       jit_mulr_ui_(s1, s2),					\
    /*	d = eax; */							\
	       MOVLrr(_EDX, d),						\
    /*	eax = *--sp; */							\
	       jit_popr_i(_EAX),					\
    /*	edx = *--sp; */							\
	       jit_popr_i(_EDX))))

#define jit_divi_i(d, rs, is)	jit_divi_i_(_EAX, (d), (rs), (is))
#define jit_divi_ui(d, rs, is)	jit_divi_ui_(_EAX, (d), (rs), (is))
#define jit_modi_i(d, rs, is)	jit_divi_i_(_EDX, (d), (rs), (is))
#define jit_modi_ui(d, rs, is)	jit_divi_ui_(_EDX, (d), (rs), (is))
#define jit_divr_i(d, s1, s2)	jit_divr_i_(_EAX, (d), (s1), (s2))
#define jit_divr_ui(d, s1, s2)	jit_divr_ui_(_EAX, (d), (s1), (s2))
#define jit_modr_i(d, s1, s2)	jit_divr_i_(_EDX, (d), (s1), (s2))
#define jit_modr_ui(d, s1, s2)	jit_divr_ui_(_EDX, (d), (s1), (s2))


/* Shifts */
#define jit_shift(d, s1, s2, m)					\
     (jit_reg32(d) == _ECX || (d) == (s2)			\
      ? (jit_reg32(s2) == _EAX					\
	 ? jit_fixd(d, _EDX, jit_shift2(_EDX, s1, s2, m))	\
	 : jit_fixd(d, _EAX, jit_shift2(_EAX, s1, s2, m)))	\
      : jit_shift2(d, s1, s2, m))

/* Shift operation, assuming d != s2 or ECX */
#define jit_shift2(d, s1, s2, m)				\
     jit_op_(d, s1, jit_cfixs(s2, _ECX, m(_CL, d)))

/* Substitute x for destination register d */
#define jit_fixd(d, x, op)					\
     (jit_pushr_i(x), op, jit_movr_i(d, x), jit_popr_i(x))

/* Conditionally substitute y for source register s */
#define jit_cfixs(s, y, op)					\
     ((s) == (y) ? op :						\
      (jit_pushr_i(y), jit_movr_i(y, s), op, jit_popr_i(y)))

#define jit_lshi_i(d, rs, is)	((is) <= 3 ?   LEALmr(0, 0, (rs), 1 << (is), (d))   :   jit_op_ ((d), (rs), SHLLir((is), (d)) ))
#define jit_rshi_i(d, rs, is)								jit_op_ ((d), (rs), SARLir((is), (d))  )
#define jit_rshi_ui(d, rs, is)								jit_op_ ((d), (rs), SHRLir((is), (d))  )
#define jit_lshr_i(d, r1, r2)	jit_shift((d), (r1), (r2), SHLLrr)
#define jit_rshr_i(d, r1, r2)	jit_shift((d), (r1), (r2), SARLrr)
#define jit_rshr_ui(d, r1, r2)	jit_shift((d), (r1), (r2), SHRLrr)

/* Stack */
#define jit_retval_i(rd)	((void)jit_movr_i ((rd), _EAX))

#define jit_patch_movi(pa,pv)   (*_PSL((pa) - sizeof(long)) = _jit_SL((pv)))

#define jit_ntoh_ui(d, rs)	jit_op_((d), (rs), BSWAPLr(d))
#define jit_ntoh_us(d, rs)	jit_op_((d), (rs), RORWir(8, d))

/* Boolean */
#define jit_ltr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETLr  )
#define jit_ler_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETLEr )
#define jit_gtr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETGr  )
#define jit_ger_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETGEr )
#define jit_eqr_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETEr  )
#define jit_ner_i(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETNEr )
#define jit_ltr_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETBr  )
#define jit_ler_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETBEr )
#define jit_gtr_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETAr  )
#define jit_ger_ui(d, s1, s2)	jit_bool_r((d), (s1), (s2), SETAEr )

#define jit_lti_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETLr,  SETSr  )
#define jit_lei_i(d, rs, is)	jit_bool_i ((d), (rs), (is), SETLEr	    )
#define jit_gti_i(d, rs, is)	jit_bool_i ((d), (rs), (is), SETGr 	    )
#define jit_gei_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETGEr, SETNSr )
#define jit_eqi_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETEr,  SETEr  )
#define jit_nei_i(d, rs, is)	jit_bool_i0((d), (rs), (is), SETNEr, SETNEr )
#define jit_lti_ui(d, rs, is)	jit_bool_i ((d), (rs), (is), SETBr	    )
#define jit_lei_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETBEr, SETEr  )
#define jit_gti_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETAr,  SETNEr )
#define jit_gei_ui(d, rs, is)	jit_bool_i0((d), (rs), (is), SETAEr, INCLr  )

/* Jump */
#define jit_bltr_i(label, s1, s2)	jit_bra_r((s1), (s2), JLm(label) )
#define jit_bler_i(label, s1, s2)	jit_bra_r((s1), (s2), JLEm(label) )
#define jit_bgtr_i(label, s1, s2)	jit_bra_r((s1), (s2), JGm(label) )
#define jit_bger_i(label, s1, s2)	jit_bra_r((s1), (s2), JGEm(label) )
#define jit_beqr_i(label, s1, s2)	jit_bra_r((s1), (s2), JEm(label) )
#define jit_bner_i(label, s1, s2)	jit_bra_r((s1), (s2), JNEm(label) )
#define jit_bltr_ui(label, s1, s2)	jit_bra_r((s1), (s2), JBm(label) )
#define jit_bler_ui(label, s1, s2)	jit_bra_r((s1), (s2), JBEm(label) )
#define jit_bgtr_ui(label, s1, s2)	jit_bra_r((s1), (s2), JAm(label) )
#define jit_bger_ui(label, s1, s2)	jit_bra_r((s1), (s2), JAEm(label) )
#define jit_bmsr_i(label, s1, s2)	(TESTLrr((s1), (s2)), JNZm(label), _jit.x.pc)
#define jit_bmcr_i(label, s1, s2)	(TESTLrr((s1), (s2)), JZm(label),  _jit.x.pc)
#define jit_boaddr_i(label, s1, s2)	(ADDLrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_bosubr_i(label, s1, s2)	(SUBLrr((s2), (s1)), JOm(label), _jit.x.pc)
#define jit_boaddr_ui(label, s1, s2)	(ADDLrr((s2), (s1)), JCm(label), _jit.x.pc)
#define jit_bosubr_ui(label, s1, s2)	(SUBLrr((s2), (s1)), JCm(label), _jit.x.pc)

#define jit_blti_i(label, rs, is)	jit_bra_i0((rs), (is), JLm(label), JSm(label) )
#define jit_blei_i(label, rs, is)	jit_bra_i ((rs), (is), JLEm(label)		    )
#define jit_bgti_i(label, rs, is)	jit_bra_i ((rs), (is), JGm(label)		    )
#define jit_bgei_i(label, rs, is)	jit_bra_i0((rs), (is), JGEm(label), JNSm(label) )
#define jit_beqi_i(label, rs, is)	jit_bra_i0((rs), (is), JEm(label), JEm(label) )
#define jit_bnei_i(label, rs, is)	jit_bra_i0((rs), (is), JNEm(label), JNEm(label) )
#define jit_blti_ui(label, rs, is)	jit_bra_i ((rs), (is), JBm(label)		    )
#define jit_blei_ui(label, rs, is)	jit_bra_i0((rs), (is), JBEm(label), JEm(label) )
#define jit_bgti_ui(label, rs, is)	jit_bra_i0((rs), (is), JAm(label), JNEm(label) )
#define jit_bgei_ui(label, rs, is)	jit_bra_i ((rs), (is), JAEm(label)		    )
#define jit_boaddi_i(label, rs, is)	(ADDLir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_bosubi_i(label, rs, is)	(SUBLir((is), (rs)), JOm(label), _jit.x.pc)
#define jit_boaddi_ui(label, rs, is)	(ADDLir((is), (rs)), JCm(label), _jit.x.pc)
#define jit_bosubi_ui(label, rs, is)	(SUBLir((is), (rs)), JCm(label), _jit.x.pc)

#define jit_bmsi_i(label, rs, is)	(jit_reduce(TEST, (is), (rs)), JNZm(label), _jit.x.pc)
#define jit_bmci_i(label, rs, is)	(jit_reduce(TEST, (is), (rs)), JZm(label),  _jit.x.pc)

#define jit_jmpi(label)			(JMPm( ((unsigned long) (label))), _jit.x.pc)
#define jit_jmpr(reg)			JMPsr(reg)

/* Memory */
#define jit_ldr_uc(d, rs)               MOVZBLmr(0,    (rs), 0,    0, (d))
#define jit_ldxr_uc(d, s1, s2)          MOVZBLmr(0,    (s1), (s2), 1, (d))
							    
#define jit_str_c(rd, rs)               jit_movbrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_c(d1, d2, rs)          jit_movbrm((rs), 0,    (d1), (d2), 1)
							    
#define jit_ldr_us(d, rs)               MOVZWLmr(0,    (rs), 0,    0,  (d))
#define jit_ldxr_us(d, s1, s2)          MOVZWLmr(0,    (s1), (s2), 1,  (d))
							    
#define jit_str_s(rd, rs)               MOVWrm(jit_reg16(rs), 0,    (rd), 0,    0)
#define jit_stxr_s(d1, d2, rs)          MOVWrm(jit_reg16(rs), 0,    (d1), (d2), 1)
							    
#define jit_str_i(rd, rs)               MOVLrm((rs), 0,    (rd), 0,    0)
#define jit_stxr_i(d1, d2, rs)          MOVLrm((rs), 0,    (d1), (d2), 1)
							    
/* Extra */
#define jit_nop()			NOP_()

#define _jit_alignment(pc, n)		(((pc ^ _MASK(4)) + 1) & _MASK(n))
#define jit_align(n) 			NOPi(_jit_alignment(_jit_UL(_jit.x.pc), (n)))


#if LIGHTNING_CROSS \
	? LIGHTNING_TARGET == LIGHTNING_X86_64 \
	: defined (__x86_64__)
#include "core-64.h"
#else
#include "core-32.h"
#endif

#endif /* __lightning_core_i386_h */

