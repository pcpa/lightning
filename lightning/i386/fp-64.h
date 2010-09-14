/******************************** -*- C -*- ****************************
 *
 *	Support macros for SSE floating-point math
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2006 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
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
 * Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 ***********************************************************************/


#ifndef __lightning_fp_h
#define __lightning_fp_h

#include <float.h>

#define JIT_FPR_NUM	7
#define JIT_FPRET	_XMM0
#define JIT_FPR(i)	(_XMM8 + (i))
#define JIT_FPTMP	_XMM15

#define jit_sse4_1()			0

#define jit_round_to_nearest()		1

/* Either use a temporary register that is finally AND/OR/XORed with RS = RD,
   or use RD as the temporary register and to the AND/OR/XOR with RS.  */
#define jit_unop_tmp(rd, rs, op)		\
	( (rs) == (rd)				\
	 ? op((rd), JIT_FPTMP, JIT_FPTMP))	\
	 : op((rd), (rd), (rs)))

#define jit_unop_f(rd, rs, op)						\
	((rs) == (rd) ? op((rd)) : (MOVSSrr ((rs), (rd)), op((rd))))

#define jit_unop_d(rd, rs, op)					\
	((rs) == (rd) ? op((rd)) : (MOVSDrr ((rs), (rd)), op((rd))))

#define jit_3opc_f(rd, s1, s2, op)				\
	( (s1) == (rd) ? op((s2), (rd))				\
	  : ((s2) == (rd) ? op((s1), (rd))			\
	     : (MOVSSrr ((s1), (rd)), op((s2), (rd)))))

#define jit_3opc_d(rd, s1, s2, op)				\
	( (s1) == (rd) ? op((s2), (rd))				\
	  : ((s2) == (rd) ? op((s1), (rd))			\
	     : (MOVSDrr ((s1), (rd)), op((s2), (rd)))))

#define jit_3op_f(rd, s1, s2, op)				\
	( (s1) == (rd) ? op((s2), (rd))				\
	  : ((s2) == (rd)					\
	     ? (MOVSSrr ((rd), JIT_FPTMP), MOVSSrr ((s1), (rd)), op(JIT_FPTMP, (rd)))	\
	     : (MOVSSrr ((s1), (rd)), op((s2), (rd)))))

#define jit_3op_d(rd, s1, s2, op)				\
	( (s1) == (rd) ? op((s2), (rd))				\
	  : ((s2) == (rd) 					\
	     ? (MOVSDrr ((rd), JIT_FPTMP), MOVSDrr ((s1), (rd)), op(JIT_FPTMP, (rd)))	\
	     : (MOVSDrr ((s1), (rd)), op((s2), (rd)))))

#define jit_addr_f(rd,s1,s2)	jit_3opc_f((rd), (s1), (s2), ADDSSrr)
#define jit_subr_f(rd,s1,s2)	jit_3op_f((rd), (s1), (s2), SUBSSrr)
#define jit_mulr_f(rd,s1,s2)	jit_3opc_f((rd), (s1), (s2), MULSSrr)
#define jit_divr_f(rd,s1,s2)	jit_3op_f((rd), (s1), (s2), DIVSSrr)

#define jit_addr_d(rd,s1,s2)	jit_3opc_d((rd), (s1), (s2), ADDSDrr)
#define jit_subr_d(rd,s1,s2)	jit_3op_d((rd), (s1), (s2), SUBSDrr)
#define jit_mulr_d(rd,s1,s2)	jit_3opc_d((rd), (s1), (s2), MULSDrr)
#define jit_divr_d(rd,s1,s2)	jit_3op_d((rd), (s1), (s2), DIVSDrr)

/* either pcmpeqd %xmm7, %xmm7 / psrld $1, %xmm7 / andps %xmm7, %RD (if RS = RD)
       or pcmpeqd %RD, %RD / psrld $1, %RD / andps %RS, %RD (if RS != RD) */
#define _jit_abs_f(rd,cnst,rs)						\
	(PCMPEQDrr((cnst), (cnst)), PSRLDir (1, (cnst)), ANDPSrr ((rs), (rd)))
#define jit_abs_f(rd,rs)	jit_unop_tmp ((rd), (rs), _jit_abs_f)

#define jit_sqrt_d(rd,rs)	SQRTSSrr((rs), (rd))
#define jit_sqrt_f(rd,rs)	SQRTSDrr((rs), (rd))

#define jit_ldr_f(f0, r0)		jit_ldr_f(f0, r0)
__jit_inline void
jit_ldr_f(int f0, jit_gpr_t r0)
{
    MOVSSmr(0, r0, 0, 0, f0);
}

#define jit_ldxr_f(f0, r0, r1)		jit_ldxr_f(f0, r0, r1)
__jit_inline void
jit_ldxr_f(int f0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSSmr(0, r0, r1, 1, f0);
}

#define jit_ldi_f(f0, i0)		jit_ldi_f(f0, i0)
__jit_inline void
jit_ldi_f(int f0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSSmr((long)i0, 0, 0, 0, f0);
    else {
	jit_movi_l((long)i0, JIT_REXTMP);
	jit_ldr_f(f0, JIT_REXTMP);
    }
}

#define jit_str_f(r0, f0)		jit_str_f(r0, f0)
__jit_inline void
jit_str_f(jit_gpr_t r0, int f0)
{
    MOVSSrm(f0, 0, r0, 0, 0);
}

#define jit_stxr_f(r0, r1, f0)		jit_stxr_f(r0, r1, f0)
__jit_inline void
jit_stxr_f(jit_gpr_t r0, jit_gpr_t r1, int f0)
{
    MOVSSrm(f0, 0, r0, r1, 1);
}

#define jit_ldxi_f(f0, r0, i0)		jit_ldxi_f(f0, r0, i0)
__jit_inline void
jit_ldxi_f(int f0, jit_gpr_t r0, long i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSSmr(i0, r0, 0, 0, f0);
    else {
	jit_movi_l(i0, JIT_REXTMP);
	jit_ldxr_f(f0, r0, JIT_REXTMP);
    }
}

#define jit_sti_f(i0, f0)		jit_sti_f(i0, f0)
__jit_inline void
jit_sti_f(void *i0, int f0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSSrm(f0, (long)i0, 0, 0, 0);
    else {
	jit_movi_l((long)i0, JIT_REXTMP);
	jit_str_f(JIT_REXTMP, f0);
    }
}

#define jit_stxi_f(i0, r0, f0)		jit_stxi_f(i0, r0, f0)
__jit_inline void
jit_stxi_f(long i0, jit_gpr_t r0, int f0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSSrm(f0, i0, r0, 0, 0);
    else {
	jit_movi_l(i0, JIT_REXTMP);
	jit_stxr_f(JIT_REXTMP, r0, f0);
    }
}

#define jit_ldr_d(f0, r0)		jit_ldr_d(f0, r0)
__jit_inline void
jit_ldr_d(int f0, jit_gpr_t r0)
{
    MOVSDmr(0, r0, 0, 0, f0);
}

#define jit_ldxr_d(f0, r0, r1)		jit_ldxr_d(f0, r0, r1)
__jit_inline void
jit_ldxr_d(int f0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSDmr(0, r0, r1, 1, f0);
}

#define jit_str_d(r0, f0)		jit_str_d(r0, f0)
__jit_inline void
jit_str_d(jit_gpr_t r0, int f0)
{
    MOVSDrm(f0, 0, r0, 0, 0);
}

#define jit_stxr_d(r0, r1, f0)		jit_stxr_d(r0, r1, f0)
__jit_inline void
jit_stxr_d(jit_gpr_t r0, jit_gpr_t r1, int f0)
{
    MOVSDrm(f0, 0, r0, r1, 1);
}

#define jit_ldi_d(f0, i0)		jit_ldi_d(f0, i0)
__jit_inline void
jit_ldi_d(int f0, void *i0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSDmr((long)i0, 0, 0, 0, f0);
    else {
	jit_movi_l((long)i0, JIT_REXTMP);
	jit_ldr_d(f0, JIT_REXTMP);
    }
}

#define jit_ldxi_d(f0, r0, i0)		jit_ldxi_d(f0, r0, i0)
__jit_inline void
jit_ldxi_d(int f0, jit_gpr_t r0, long i0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSDmr(i0, r0, 0, 0, f0);
    else {
	jit_movi_l(i0, JIT_REXTMP);
	jit_ldxr_d(f0, r0, JIT_REXTMP);
    }
}

#define jit_sti_d(i0, f0)		jit_sti_d(i0, f0)
__jit_inline void
jit_sti_d(void *i0, int f0)
{
    if (jit_can_sign_extend_int_p((long)i0))
	MOVSDrm(f0, (long)i0, 0, 0, 0);
    else {
	jit_movi_l((long)i0, JIT_REXTMP);
	jit_str_d(JIT_REXTMP, f0);
    }
}

#define jit_stxi_d(i0, r0, f0)		jit_stxi_d(i0, r0, f0)
__jit_inline void
jit_stxi_d(long i0, jit_gpr_t r0, int f0)
{
    if (jit_can_sign_extend_int_p(i0))
	MOVSDrm(f0, i0, r0, 0, 0);
    else {
	jit_movi_l(i0, JIT_REXTMP);
	jit_stxr_d(JIT_REXTMP, r0, f0);
    }
}

#define jit_movr_f(f0, f1)		jit_movr_f(f0, f1)
__jit_inline void
jit_movr_f(int f0, int f1)
{
    MOVSSrr(f1, f0);
}

#define jit_movr_d(f0, f1)		jit_movr_d(f0, f1)
__jit_inline void
jit_movr_d(int f0, int f1)
{
    MOVSDrr(f1, f0);
}

#define jit_movi_f(f0, i0)		jit_movi_f(f0, i0)
__jit_inline void
jit_movi_f(int f0, float i0)
{
    union {
	int	i;
	float	f;
    } data;
    data.f = i0;
    if (data.f == 0.0 && !(data.i & 0x80000000))
	XORPSrr(f0, f0);
    else {
	PUSHQi(data.i);
	jit_ldr_f(f0, _RSP);
	ADDQir(8, _RSP);
    }
}

#define jit_movi_d(f0, i0)		jit_movi_d(f0, i0)
__jit_inline void
jit_movi_d(int f0, double i0)
{
    union {
	long	l;
	double	d;
    } data;
    data.d = i0;
    if (data.d == 0.0 && !(data.l & 0x8000000000000000))
	XORPDrr(f0, f0);
    else {
	if (jit_can_sign_extend_int_p(data.l))
	    PUSHQi(data.l);
	else {
	    MOVQir(data.l, JIT_REXTMP);
	    PUSHQr(JIT_REXTMP);
	}
	jit_ldr_d(f0, _RSP);
	ADDQir(8, _RSP);
    }
}

#define jit_extr_i_f(f0, r0)		jit_extr_i_f(f0, r0)
__jit_inline void
jit_extr_i_f(int f0, jit_gpr_t r0)
{
    CVTSI2SSLrr(r0, f0);
}

#define jit_extr_i_d(f0, r0)		jit_extr_i_d(f0, r0)
__jit_inline void
jit_extr_i_d(int f0, jit_gpr_t r0)
{
    CVTSI2SDLrr(r0, f0);
}

#define jit_extr_l_f(f0, r0)		jit_extr_l_f(f0, r0)
__jit_inline void
jit_extr_l_f(int f0, jit_gpr_t r0)
{
    CVTSI2SSQrr(r0, f0);
}

#define jit_extr_l_d(f0, r0)		jit_extr_l_d(f0, r0)
__jit_inline void
jit_extr_l_d(int f0, jit_gpr_t r0)
{
    CVTSI2SDQrr(r0, f0);
}

#define jit_extr_f_d(f0, f1)		jit_extr_f_d(f0, f1)
__jit_inline void
jit_extr_f_d(int f0, int f1)
{
    CVTSS2SDrr(f1, f0);
}

#define jit_extr_d_f(f0, f1)		jit_extr_d_f(f0, f1)
__jit_inline void
jit_extr_d_f(int f0, int f1)
{
    CVTSD2SSrr(f1, f0);
}

#define jit_negr_f(f0, f1)		jit_negr_f(f0, f1)
__jit_inline void
jit_negr_f(int f0, int f1)
{
    PUSHQi(0x80000000);
    if (f0 == f1) {
	jit_ldr_f(JIT_FPTMP, _RSP);
	XORPSrr(JIT_FPTMP, f0);
    }
    else {
	jit_ldr_f(f0, _RSP);
	XORPSrr(f1, f0);
    }
    ADDQir(8, _RSP);
}

#define _jit_abs_d(rd,cnst,rs)						\
	(PCMPEQDrr((cnst), (cnst)), PSRLQir (1, (cnst)), ANDPDrr ((rs), (rd)))
#define jit_abs_d(rd,rs)	jit_unop_tmp ((rd), (rs), _jit_abs_d)

#define jit_negr_d(f0, f1)		jit_negr_d(f0, f1)
__jit_inline void
jit_negr_d(int f0, int f1)
{
    MOVQir(0x8000000000000000, JIT_REXTMP);
    PUSHQr(JIT_REXTMP);
    if (f0 == f1) {
	jit_ldr_d(JIT_FPTMP, _RSP);
	XORPDrr(JIT_FPTMP, f0);
    }
    else {
	jit_ldr_d(f0, _RSP);
	XORPDrr(f1, f0);
    }
    ADDQir(8, _RSP);
}

#define jit_rintr_f_i(r0, f0)		jit_rintr_f_i(r0, f0)
__jit_inline void
jit_rintr_f_i(jit_gpr_t r0, int f0)
{
    CVTSS2SILrr(f0, r0);
}

#define jit_rintr_f_l(r0, f0)		jit_rintr_f_l(r0, f0)
__jit_inline void
jit_rintr_f_l(jit_gpr_t r0, int f0)
{
    CVTSS2SIQrr(f0, r0);
}

#define jit_rintr_d_i(r0, f0)		jit_rintr_d_i(r0, f0)
__jit_inline void
jit_rintr_d_i(jit_gpr_t r0, int f0)
{
    CVTSD2SILrr(f0, r0);
}

#define jit_rintr_d_l(r0, f0)		jit_rintr_d_l(r0, f0)
__jit_inline void
jit_rintr_d_l(jit_gpr_t r0, int f0)
{
    CVTSD2SIQrr(f0, r0);
}

#define jit_roundr_d_i(rd, rs)	CVTSD2SILrr((rs), (rd))
#define jit_roundr_f_i(rd, rs)	CVTSS2SILrr((rs), (rd))
#define jit_roundr_d_l(rd, rs)	CVTSD2SIQrr((rs), (rd))
#define jit_roundr_f_l(rd, rs)	CVTSS2SIQrr((rs), (rd))

#define jit_truncr_f_i(r0, f0)		jit_truncr_f_i(r0, f0)
__jit_inline void
jit_truncr_f_i(jit_gpr_t r0, int f0)
{
    CVTTSS2SILrr(f0, r0);
}

#define jit_truncr_f_l(r0, f0)		jit_truncr_f_l(r0, f0)
__jit_inline void
jit_truncr_f_l(jit_gpr_t r0, int f0)
{
    CVTTSS2SIQrr(f0, r0);
}

#define jit_truncr_d_i(r0, f0)		jit_truncr_d_i(r0, f0)
__jit_inline void
jit_truncr_d_i(jit_gpr_t r0, int f0)
{
    CVTTSD2SILrr(f0, r0);
}

#define jit_truncr_d_l(r0, f0)		jit_truncr_d_l(r0, f0)
__jit_inline void
jit_truncr_d_l(jit_gpr_t r0, int f0)
{
    CVTTSD2SIQrr(f0, r0);
}

__jit_inline void
_jit_sse_rnd_enter(jit_gpr_t r0, int mode)
{
    SUBQir(8, _RSP);
    STMXCSRrm(0, _RSP, 0, 0);
    jit_ldr_i(r0, _RSP);
    jit_stxi_i(4, _RSP, r0);
    jit_andi_i(r0, r0, ~MXCSR_RND_MASK);
    if (mode)
	jit_ori_i(r0, r0, mode);
    jit_str_i(_RSP, r0);
    LDMXCSRmr(0, _RSP, 0, 0);
}

__jit_inline void
_jit_sse_rnd_leave(void)
{
    LDMXCSRmr(4, _RSP, 0, 0);
    ADDQir(8, _RSP);
}

#define jit_floorr_f_i(r0, f0)		jit_floorr_f_i(r0, f0)
__jit_inline void
jit_floorr_f_i(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSSrri(f0, JIT_FPTMP, MXCSR_RND_DOWN >> 13);
	CVTSS2SILrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_f(JIT_FPTMP, -0.5);
	ADDSSrr(f0, JIT_FPTMP);
	jit_rintr_f_i(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	jit_rintr_f_i(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_floorr_f_l(r0, f0)		jit_floorr_f_l(r0, f0)
__jit_inline void
jit_floorr_f_l(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSSrri(f0, JIT_FPTMP, MXCSR_RND_DOWN >> 13);
	CVTSS2SIQrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_f(JIT_FPTMP, -0.5);
	ADDSSrr(f0, JIT_FPTMP);
	jit_rintr_f_l(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	jit_rintr_f_l(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_floorr_d_i(r0, f0)		jit_floorr_d_i(r0, f0)
__jit_inline void
jit_floorr_d_i(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSDrri(f0, JIT_FPTMP, MXCSR_RND_DOWN >> 13);
	CVTSD2SILrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_d(JIT_FPTMP, -0.5);
	ADDSDrr(f0, JIT_FPTMP);
	jit_rintr_d_i(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	jit_rintr_d_i(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_floorr_d_l(r0, f0)		jit_floorr_d_l(r0, f0)
__jit_inline void
jit_floorr_d_l(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSDrri(f0, JIT_FPTMP, MXCSR_RND_DOWN >> 13);
	CVTSD2SIQrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_d(JIT_FPTMP, -0.5);
	ADDSDrr(f0, JIT_FPTMP);
	jit_rintr_d_l(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	jit_rintr_d_l(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_ceilr_f_i(r0, f0)		jit_ceilr_f_i(r0, f0)
__jit_inline void
jit_ceilr_f_i(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSSrri(f0, JIT_FPTMP, MXCSR_RND_UP >> 13);
	CVTSS2SILrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_f(JIT_FPTMP, 0.5);
	ADDSSrr(f0, JIT_FPTMP);
	jit_rintr_f_i(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_UP);
	jit_rintr_f_i(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_ceilr_f_l(r0, f0)		jit_ceilr_f_l(r0, f0)
__jit_inline void
jit_ceilr_f_l(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSSrri(f0, JIT_FPTMP, MXCSR_RND_UP >> 13);
	CVTSS2SIQrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_f(JIT_FPTMP, 0.5);
	ADDSSrr(f0, JIT_FPTMP);
	jit_rintr_f_l(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_UP);
	jit_rintr_f_l(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_ceilr_d_i(r0, f0)		jit_ceilr_d_i(r0, f0)
__jit_inline void
jit_ceilr_d_i(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSDrri(f0, JIT_FPTMP, MXCSR_RND_UP >> 13);
	CVTSD2SILrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_d(JIT_FPTMP, 0.5);
	ADDSDrr(f0, JIT_FPTMP);
	jit_rintr_d_i(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_UP);
	jit_rintr_d_i(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_ceilr_d_l(r0, f0)		jit_ceilr_d_l(r0, f0)
__jit_inline void
jit_ceilr_d_l(jit_gpr_t r0, int f0)
{
    if (jit_sse4_1()) {
	ROUNDSDrri(f0, JIT_FPTMP, MXCSR_RND_UP >> 13);
	CVTSD2SIQrr(JIT_FPTMP, r0);
    }
    else if (jit_round_to_nearest()) {
	jit_movi_d(JIT_FPTMP, 0.5);
	ADDSDrr(f0, JIT_FPTMP);
	jit_rintr_d_l(r0, JIT_FPTMP);
    }
    else {
	_jit_sse_rnd_enter(r0, MXCSR_RND_UP);
	jit_rintr_d_l(r0, f0);
	_jit_sse_rnd_leave();
    }
}

#define jit_bltr_f(d, s1, s2)            (UCOMISSrr ((s1), (s2)), JAm ((d)), _jit.x.pc)
#define jit_bler_f(d, s1, s2)            (UCOMISSrr ((s1), (s2)), JAEm ((d)), _jit.x.pc)
#define jit_beqr_f(d, s1, s2)            (UCOMISSrr ((s1), (s2)), _OO (0x7a06), JEm ((d)), _jit.x.pc)
#define jit_bner_f(label, f0, f1)	jit_bner_f(label, f0, f1)
__jit_inline jit_insn *
jit_bner_f(jit_insn *label, int f0, int f1)
{
    jit_insn	*jp_label;
    jit_insn	*jz_label;

    UCOMISSrr(f0, f1);
    /* jump to user jump if parity (unordered) */
    JPSm((long)(_jit.x.pc + 2));
    jp_label = _jit.x.pc;
    /* jump past user jump if zero (equal)  */
    JZSm((long)(_jit.x.pc + 5));
    jz_label = _jit.x.pc;
    jit_patch_rel_char_at(jp_label, _jit.x.pc);
    JMPm((long)label);
    jit_patch_rel_char_at(jz_label, _jit.x.pc);
    return (_jit.x.pc);
}
#define jit_bger_f(d, s1, s2)            (UCOMISSrr ((s2), (s1)), JAEm ((d)), _jit.x.pc)
#define jit_bgtr_f(d, s1, s2)            (UCOMISSrr ((s2), (s1)), JAm ((d)), _jit.x.pc)
#define jit_bunltr_f(label, f0, f1)	jit_bunltr_f(label, f0, f1)
__jit_inline jit_insn *
jit_bunltr_f(jit_insn *label, int f0, int f1)
{
    UCOMISSrr(f1, f0);
    JNAEm(label);
    return (_jit.x.pc);
}
#define jit_bunler_f(d, s1, s2)          (UCOMISSrr ((s2), (s1)), JNAm ((d)), _jit.x.pc)
#define jit_buneqr_f(d, s1, s2)          (UCOMISSrr ((s1), (s2)), JEm ((d)), _jit.x.pc)
#define jit_bltgtr_f(d, s1, s2)          (UCOMISSrr ((s1), (s2)), JNEm ((d)), _jit.x.pc)
#define jit_bunger_f(d, s1, s2)          (UCOMISSrr ((s1), (s2)), JNAm ((d)), _jit.x.pc)
#define jit_bungtr_f(d, s1, s2)          (UCOMISSrr ((s1), (s2)), JNAEm ((d)), _jit.x.pc)
#define jit_bordr_f(d, s1, s2)           (UCOMISSrr ((s1), (s2)), JNPm ((d)), _jit.x.pc)
#define jit_bunordr_f(d, s1, s2)         (UCOMISSrr ((s1), (s2)), JPm ((d)), _jit.x.pc)

#define jit_bltr_d(d, s1, s2)            (UCOMISDrr ((s1), (s2)), JAm ((d)), _jit.x.pc)
#define jit_bler_d(d, s1, s2)            (UCOMISDrr ((s1), (s2)), JAEm ((d)), _jit.x.pc)
#define jit_beqr_d(d, s1, s2)            (UCOMISDrr ((s1), (s2)), _OO (0x7a06), JEm ((d)), _jit.x.pc)
#define jit_bner_d(label, f0, f1)	jit_bner_d(label, f0, f1)
__jit_inline jit_insn *
jit_bner_d(jit_insn *label, int f0, int f1)
{
    jit_insn	*jp_label;
    jit_insn	*jz_label;

    UCOMISDrr(f0, f1);
    /* jump to user jump if parity (unordered) */
    JPSm((long)(_jit.x.pc + 2));
    jp_label = _jit.x.pc;
    /* jump past user jump if zero (equal)  */
    JZSm((long)(_jit.x.pc + 5));
    jz_label = _jit.x.pc;
    jit_patch_rel_char_at(jp_label, _jit.x.pc);
    JMPm((long)label);
    jit_patch_rel_char_at(jz_label, _jit.x.pc);
    return (_jit.x.pc);
}
#define jit_bger_d(d, s1, s2)            (UCOMISDrr ((s2), (s1)), JAEm ((d)), _jit.x.pc)
#define jit_bgtr_d(d, s1, s2)            (UCOMISDrr ((s2), (s1)), JAm ((d)), _jit.x.pc)
#define jit_bunltr_d(label, f0, f1)	jit_bunltr_d(label, f0, f1)
__jit_inline jit_insn *
jit_bunltr_d(jit_insn *label, int f0, int f1)
{
    UCOMISDrr(f1, f0);
    JNAEm(label);
    return (_jit.x.pc);
}
#define jit_bunler_d(d, s1, s2)          (UCOMISDrr ((s2), (s1)), JNAm ((d)), _jit.x.pc)
#define jit_buneqr_d(d, s1, s2)          (UCOMISDrr ((s1), (s2)), JEm ((d)), _jit.x.pc)
#define jit_bltgtr_d(d, s1, s2)          (UCOMISDrr ((s1), (s2)), JNEm ((d)), _jit.x.pc)
#define jit_bunger_d(d, s1, s2)          (UCOMISDrr ((s1), (s2)), JNAm ((d)), _jit.x.pc)
#define jit_bungtr_d(d, s1, s2)          (UCOMISDrr ((s1), (s2)), JNAEm ((d)), _jit.x.pc)
#define jit_bordr_d(d, s1, s2)           (UCOMISDrr ((s1), (s2)), JNPm ((d)), _jit.x.pc)
#define jit_bunordr_d(d, s1, s2)         (UCOMISDrr ((s1), (s2)), JPm ((d)), _jit.x.pc)

#define jit_ltr_f(d, s1, s2)            (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETAr ((d)))
#define jit_ler_f(d, s1, s2)            (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETAEr ((d)))
#define jit_eqr_f(r0, f0, f1)		jit_eqr_f(r0, f0, f1)
__jit_inline void
jit_eqr_f(int r0, int f0, int f1)
{
    /* set register to zero */
    XORLrr(r0, r0);
    /* compare operands */
    UCOMISSrr(f0, f1);
    /* jump if parity (unordered) */
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    /* set register if equal */
    SETEr(r0);
    /* was unordered */
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
}

#define jit_ner_f(r0, f0, f1)		jit_ner_f(r0, f0, f1)
__jit_inline void
jit_ner_f(int r0, int f0, int f1)
{
    /* set register to one */
    MOVLir(1, r0);
    /* compare operands */
    UCOMISSrr(f0, f1);
    /* jump if parity (unordered) */
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    /* set register if not equal */
    SETNEr(r0);
    /* was unordered */
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
}
#define jit_ger_f(d, s1, s2)            (XORLrr ((d), (d)), UCOMISSrr ((s2), (s1)), SETAEr ((d)))
#define jit_gtr_f(d, s1, s2)            (XORLrr ((d), (d)), UCOMISSrr ((s2), (s1)), SETAr ((d)))
#define jit_unltr_f(d, s1, s2)          (XORLrr ((d), (d)), UCOMISSrr ((s2), (s1)), SETNAEr ((d)))
#define jit_unler_f(d, s1, s2)          (XORLrr ((d), (d)), UCOMISSrr ((s2), (s1)), SETNAr ((d)))
#define jit_uneqr_f(d, s1, s2)          (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETEr ((d)))
#define jit_ltgtr_f(d, s1, s2)          (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETNEr ((d)))
#define jit_unger_f(d, s1, s2)          (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETNAr ((d)))
#define jit_ungtr_f(d, s1, s2)          (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETNAEr ((d)))
#define jit_ordr_f(d, s1, s2)           (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETNPr ((d)))
#define jit_unordr_f(d, s1, s2)         (XORLrr ((d), (d)), UCOMISSrr ((s1), (s2)), SETPr ((d)))

#define jit_ltr_d(d, s1, s2)            (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETAr ((d)))
#define jit_ler_d(d, s1, s2)            (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETAEr ((d)))
#define jit_eqr_d(r0, f0, f1)		jit_eqr_d(r0, f0, f1)
__jit_inline void
jit_eqr_d(int r0, int f0, int f1)
{
    /* set register to zero */
    XORLrr(r0, r0);
    /* compare operands */
    UCOMISDrr(f0, f1);
    /* jump if parity (unordered) */
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    /* set register if equal */
    SETEr(r0);
    /* was unordered */
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
}

#define jit_ner_d(r0, f0, f1)		jit_ner_d(r0, f0, f1)
__jit_inline void
jit_ner_d(int r0, int f0, int f1)
{
    /* set register to one */
    MOVLir(1, r0);
    /* compare operands */
    UCOMISDrr(f0, f1);
    /* jump if parity (unordered) */
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    /* set register if not equal */
    SETNEr(r0);
    /* was unordered */
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
}
#define jit_ger_d(d, s1, s2)            (XORLrr ((d), (d)), UCOMISDrr ((s2), (s1)), SETAEr ((d)))
#define jit_gtr_d(d, s1, s2)            (XORLrr ((d), (d)), UCOMISDrr ((s2), (s1)), SETAr ((d)))
#define jit_unltr_d(d, s1, s2)          (XORLrr ((d), (d)), UCOMISDrr ((s2), (s1)), SETNAEr ((d)))
#define jit_unler_d(d, s1, s2)          (XORLrr ((d), (d)), UCOMISDrr ((s2), (s1)), SETNAr ((d)))
#define jit_uneqr_d(d, s1, s2)          (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETEr ((d)))
#define jit_ltgtr_d(d, s1, s2)          (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETNEr ((d)))
#define jit_unger_d(d, s1, s2)          (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETNAr ((d)))
#define jit_ungtr_d(d, s1, s2)          (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETNAEr ((d)))
#define jit_ordr_d(d, s1, s2)           (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETNPr ((d)))
#define jit_unordr_d(d, s1, s2)         (XORLrr ((d), (d)), UCOMISDrr ((s1), (s2)), SETPr ((d)))

#define jit_prolog_f(num)						\
    /* Update counter. Have float arguments on stack? */		\
    (((_jitl.nextarg_putfp += (num)) > JIT_FP_ARG_MAX)			\
	/* Yes. Stack appears to be padded? */				\
	? ((_jitl.argssize & 1)						\
	    /* Yes. Update number of stack arguments */			\
	    ? (_jitl.argssize = ((_jitl.nextarg_puti > JIT_ARG_MAX)	\
				? _jitl.nextarg_puti - JIT_ARG_MAX	\
				: 0) + _jitl.nextarg_putfp		\
				     - JIT_FP_ARG_MAX,			\
		/* Stack still appears to be padded? */			\
		((_jitl.argssize & 1)					\
		    /* Yes. Do nothing */				\
		    ? 0							\
		    /* No. Update state for aligned stack */		\
		    : (_jitl.framesize -= sizeof(double))))		\
	    /* No. Update number of stack arguments */			\
	    : (_jitl.argssize = ((_jitl.nextarg_puti > JIT_ARG_MAX)	\
				? _jitl.nextarg_puti - JIT_ARG_MAX	\
				: 0) + _jitl.nextarg_putfp		\
				     - JIT_FP_ARG_MAX,			\
		/* Stack appears to be padded now? */			\
		((_jitl.argssize & 1)					\
		    /* Yes. Update state for padded stack */		\
		    ? (_jitl.framesize += sizeof(double))		\
		    /* No. Do nothing */				\
		    : 0)))						\
	/* No. Do nothing as there are no known floats on stack */	\
	: 0)
#define jit_prolog_d(num)		jit_prolog_f(num)

#define jit_prepare_f(num)						\
    /* Update counter. Need stack for float arguments? */		\
    (((_jitl.nextarg_putfp += num) > JIT_FP_ARG_MAX)			\
	/* Yes. Update counter of stack arguments */			\
	? (_jitl.argssize = ((_jitl.nextarg_puti > JIT_ARG_MAX)		\
			    ? _jitl.nextarg_puti - JIT_ARG_MAX		\
			    : 0) + _jitl.nextarg_putfp			\
				 - JIT_FP_ARG_MAX,			\
	   /* Update counter of float registers */			\
	   _jitl.fprssize = JIT_FP_ARG_MAX)				\
	: (_jitl.fprssize += (num)))
#define jit_prepare_d(num)		jit_prepare_f(num)

#define jit_arg_f()							\
     /* There are still free float registers? */			\
    (_jitl.nextarg_getfp < JIT_FP_ARG_MAX				\
	  /* Yes. Return the register offset */				\
	? _jitl.nextarg_getfp++						\
	  /* No. Return the stack offset */				\
	: ((_jitl.framesize += sizeof(double)) - sizeof(double)))
#define jit_arg_d()			jit_arg_f()

#define jit_getarg_f(reg, ofs)		((ofs) < JIT_FP_ARG_MAX \
					 ? jit_movr_f((reg), _XMM0 + (ofs)) \
					 : jit_ldxi_f((reg), JIT_FP, (ofs)))
#define jit_getarg_d(reg, ofs)		((ofs) < JIT_FP_ARG_MAX \
					 ? jit_movr_d((reg), _XMM0 + (ofs)) \
					 : jit_ldxi_d((reg), JIT_FP, (ofs)))

#define jit_pusharg_f(rs)		(--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX \
					 ? (SUBQir(sizeof(double), JIT_SP), jit_str_f(JIT_SP,(rs))) \
					 : jit_movr_f(_XMM0 + _jitl.nextarg_putfp, (rs)))
#define jit_pusharg_d(rs)		(--_jitl.nextarg_putfp >= JIT_FP_ARG_MAX \
					 ? (SUBQir(sizeof(double), JIT_SP), jit_str_d(JIT_SP,(rs))) \
					 : jit_movr_d(_XMM0 + _jitl.nextarg_putfp, (rs)))

#endif /* __lightning_fp_h */
