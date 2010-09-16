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


#ifndef __lightning_fp_sse_h
#define __lightning_fp_sse_h

__jit_inline void
sse_addr_f(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	ADDSSrr(f2, f0);
    else if (f0 == f2)
	ADDSSrr(f1, f0);
    else {
	MOVSSrr(f1, f0);
	ADDSSrr(f2, f0);
    }
}

__jit_inline void
sse_addr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	ADDSDrr(f2, f0);
    else if (f0 == f2)
	ADDSDrr(f1, f0);
    else {
	MOVSDrr(f1, f0);
	ADDSDrr(f2, f0);
    }
}

__jit_inline void
sse_subr_f(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	SUBSSrr(f2, f0);
    else if (f0 == f2) {
	MOVSSrr(f0, JIT_FPTMP0);
	MOVSSrr(f1, f0);
	SUBSSrr(JIT_FPTMP0, f0);
    }
    else {
	MOVSSrr(f1, f0);
	SUBSSrr(f2, f0);
    }
}

__jit_inline void
sse_subr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	SUBSDrr(f2, f0);
    else if (f0 == f2) {
	MOVSDrr(f0, JIT_FPTMP0);
	MOVSDrr(f1, f0);
	SUBSDrr(JIT_FPTMP0, f0);
    }
    else {
	MOVSDrr(f1, f0);
	SUBSDrr(f2, f0);
    }
}

__jit_inline void
sse_mulr_f(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	MULSSrr(f2, f0);
    else if (f0 == f2)
	MULSSrr(f1, f0);
    else {
	MOVSSrr(f1, f0);
	MULSSrr(f2, f0);
    }
}

__jit_inline void
sse_mulr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	MULSDrr(f2, f0);
    else if (f0 == f2)
	MULSDrr(f1, f0);
    else {
	MOVSDrr(f1, f0);
	MULSDrr(f2, f0);
    }
}

__jit_inline void
sse_divr_f(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	DIVSSrr(f2, f0);
    else if (f0 == f2) {
	MOVSSrr(f0, JIT_FPTMP0);
	MOVSSrr(f1, f0);
	DIVSSrr(JIT_FPTMP0, f0);
    }
    else {
	MOVSSrr(f1, f0);
	DIVSSrr(f2, f0);
    }
}

__jit_inline void
sse_divr_d(jit_fpr_t f0, jit_fpr_t f1, jit_fpr_t f2)
{
    if (f0 == f1)
	DIVSDrr(f2, f0);
    else if (f0 == f2) {
	MOVSDrr(f0, JIT_FPTMP0);
	MOVSDrr(f1, f0);
	DIVSDrr(JIT_FPTMP0, f0);
    }
    else {
	MOVSDrr(f1, f0);
	DIVSDrr(f2, f0);
    }
}

__jit_inline void
sse_ldr_f(jit_fpr_t f0, jit_gpr_t r0)
{
    MOVSSmr(0, r0, 0, 0, f0);
}

__jit_inline void
sse_ldr_d(jit_fpr_t f0, jit_gpr_t r0)
{
    MOVSDmr(0, r0, 0, 0, f0);
}

__jit_inline void
sse_ldi_f(jit_fpr_t f0, void *i0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p((long)i0)) {
	jit_movi_l((long)i0, JIT_REXTMP);
	sse_ldr_f(f0, JIT_REXTMP);
    }
    else
#endif
	MOVSSmr((long)i0, 0, 0, 0, f0);
}

__jit_inline void
sse_ldi_d(jit_fpr_t f0, void *i0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p((long)i0)) {
	jit_movi_l((long)i0, JIT_REXTMP);
	sse_ldr_d(f0, JIT_REXTMP);
    }
    else
#endif
	MOVSDmr((long)i0, 0, 0, 0, f0);
}

__jit_inline void
sse_ldxr_f(jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSSmr(0, r0, r1, 1, f0);
}

__jit_inline void
sse_ldxr_d(jit_fpr_t f0, jit_gpr_t r0, jit_gpr_t r1)
{
    MOVSDmr(0, r0, r1, 1, f0);
}

__jit_inline void
sse_ldxi_f(jit_fpr_t f0, jit_gpr_t r0, jit_idx_t i0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p(i0)) {
	jit_movi_l(i0, JIT_REXTMP);
	sse_ldxr_f(f0, r0, JIT_REXTMP);
    }
    else
#endif
	MOVSSmr(i0, r0, 0, 0, f0);
}

__jit_inline void
sse_ldxi_d(jit_fpr_t f0, jit_gpr_t r0, long i0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p(i0)) {
	jit_movi_l(i0, JIT_REXTMP);
	sse_ldxr_d(f0, r0, JIT_REXTMP);
    }
    else
#endif
	MOVSDmr(i0, r0, 0, 0, f0);
}

__jit_inline void
sse_str_f(jit_gpr_t r0, jit_fpr_t f0)
{
    MOVSSrm(f0, 0, r0, 0, 0);
}

__jit_inline void
sse_str_d(jit_gpr_t r0, jit_fpr_t f0)
{
    MOVSDrm(f0, 0, r0, 0, 0);
}

__jit_inline void
sse_sti_f(void *i0, jit_fpr_t f0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p((long)i0)) {
	jit_movi_l((long)i0, JIT_REXTMP);
	sse_str_f(JIT_REXTMP, f0);
    }
    else
#endif
	MOVSSrm(f0, (long)i0, 0, 0, 0);
}

__jit_inline void
sse_sti_d(void *i0, jit_fpr_t f0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p((long)i0)) {
	jit_movi_l((long)i0, JIT_REXTMP);
	sse_str_d(JIT_REXTMP, f0);
    }
    else
#endif
	MOVSDrm(f0, (long)i0, 0, 0, 0);
}

__jit_inline void
sse_stxr_f(jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    MOVSSrm(f0, 0, r0, r1, 1);
}

__jit_inline void
sse_stxr_d(jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t f0)
{
    MOVSDrm(f0, 0, r0, r1, 1);
}

__jit_inline void
sse_stxi_f(jit_idx_t i0, jit_gpr_t r0, jit_fpr_t f0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p(i0)) {
	jit_movi_l(i0, JIT_REXTMP);
	sse_stxr_f(JIT_REXTMP, r0, f0);
    }
    else
#endif
	MOVSSrm(f0, i0, r0, 0, 0);
}

__jit_inline void
sse_stxi_d(jit_idx_t i0, jit_gpr_t r0, jit_fpr_t f0)
{
#if __WORDSIZE == 64
    if (!jit_can_sign_extend_int_p(i0)) {
	jit_movi_l(i0, JIT_REXTMP);
	sse_stxr_d(JIT_REXTMP, r0, f0);
    }
    else
#endif
	MOVSDrm(f0, i0, r0, 0, 0);
}

__jit_inline void
sse_movi_f(jit_fpr_t f0, float i0)
{
    union {
	int	i;
	float	f;
    } data;
    data.f = i0;
    if (data.f == 0.0 && !(data.i & 0x80000000))
	XORPSrr(f0, f0);
    else {
#if __WORDSIZE == 32
	jit_pushi_i(data.i);
	sse_ldr_f(f0, _RSP);
	jit_addi_l(_RSP, _RSP, sizeof(long));
#else
	jit_movi_i(JIT_REXTMP, data.i);
	MOVDLXrr(JIT_REXTMP, f0);
#endif
    }
}

__jit_inline void
sse_movi_d(jit_fpr_t f0, double i0)
{
    union {
	int	i[2];
	long	l;
	double	d;
    } data;
    data.d = i0;
    if (data.d == 0.0 && !(data.i[1] & 0x80000000))
	XORPDrr(f0, f0);
    else {
#if __WORDSIZE == 32
	jit_pushi_i(data.i[1]);
	jit_pushi_i(data.i[0]);
	sse_ldr_d(f0, _RSP);
	jit_addi_l(_RSP, _RSP, sizeof(long) << 1);
#else
	jit_movi_l(JIT_REXTMP, data.l);
	MOVDQXrr(JIT_REXTMP, f0);
#endif
    }
}

__jit_inline void
sse_movr_f(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 != f1)
	MOVSSrr(f1, f0);
}

__jit_inline void
sse_movr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 != f1)
	MOVSDrr(f1, f0);
}

__jit_inline void
sse_extr_i_f(jit_fpr_t f0, jit_gpr_t r0)
{
    CVTSI2SSLrr(r0, f0);
}

__jit_inline void
sse_extr_i_d(jit_fpr_t f0, jit_gpr_t r0)
{
    CVTSI2SDLrr(r0, f0);
}

#if __WORDSIZE == 64
__jit_inline void
sse_extr_l_f(jit_fpr_t f0, jit_gpr_t r0)
{
    CVTSI2SSQrr(r0, f0);
}

__jit_inline void
sse_extr_l_d(jit_fpr_t f0, jit_gpr_t r0)
{
    CVTSI2SDQrr(r0, f0);
}
#endif

__jit_inline void
sse_extr_f_d(jit_fpr_t f0, jit_fpr_t f1)
{
    CVTSS2SDrr(f1, f0);
}

__jit_inline void
sse_extr_d_f(jit_fpr_t f0, jit_fpr_t f1)
{
    CVTSD2SSrr(f1, f0);
}

__jit_inline void
sse_absr_f(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 == f1) {
	PCMPEQLrr(JIT_FPTMP0, JIT_FPTMP0);
	PSRLLir(1, JIT_FPTMP0);
	ANDPSrr(JIT_FPTMP0, f0);
    }
    else {
	PCMPEQLrr(f0, f0);
	PSRLLir(1, f0);
	ANDPSrr(f1, f0);
    }
}

__jit_inline void
sse_absr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    if (f0 == f1) {
	PCMPEQLrr(JIT_FPTMP0, JIT_FPTMP0);
	PSRLQir(1, JIT_FPTMP0);
	ANDPDrr(JIT_FPTMP0, f0);
    }
    else {
	PCMPEQLrr(f0, f0);
	PSRLQir(1, f0);
	ANDPDrr(f1, f0);
    }
}

__jit_inline void
sse_sqrtr_f(jit_fpr_t f0, jit_fpr_t f1)
{
    SQRTSSrr(f1, f0);
}

__jit_inline void
sse_sqrtr_d(jit_fpr_t f0, jit_fpr_t f1)
{
    SQRTSDrr(f1, f0);
}

__jit_inline void
sse_negr_f(jit_fpr_t f0, jit_fpr_t f1)
{
#if __WORDSIZE == 32
    jit_pushi_i(0x80000000);
    if (f0 == f1) {
	sse_ldr_f(JIT_FPTMP0, _RSP);
	XORPSrr(JIT_FPTMP0, f0);
    }
    else {
	sse_ldr_f(f0, _RSP);
	XORPSrr(f1, f0);
    }
    jit_addi_l(_RSP, _RSP, sizeof(long));
#else
    jit_movi_i(JIT_REXTMP, 0x80000000);
    if (f0 == f1) {
	MOVDLXrr(JIT_REXTMP, JIT_FPTMP0);
	XORPSrr(JIT_FPTMP0, f0);
    }
    else {
	MOVDLXrr(JIT_REXTMP, f0);
	XORPSrr(f1, f0);
    }
#endif
}

__jit_inline void
sse_negr_d(jit_fpr_t f0, jit_fpr_t f1)
{
#if __WORDSIZE == 32
    jit_pushi_i(0x80000000);
    jit_pushi_i(0);
    if (f0 == f1) {
	sse_ldr_d(JIT_FPTMP0, _RSP);
	XORPSrr(JIT_FPTMP0, f0);
    }
    else {
	sse_ldr_d(f0, _RSP);
	XORPSrr(f1, f0);
    }
    jit_addi_l(_RSP, _RSP, sizeof(long) << 1);
#else
    jit_movi_l(JIT_REXTMP, 0x8000000000000000);
    if (f0 == f1) {
	MOVDQXrr(JIT_REXTMP, JIT_FPTMP0);
	XORPDrr(JIT_FPTMP0, f0);
    }
    else {
	MOVDQXrr(JIT_REXTMP, f0);
	XORPDrr(f1, f0);
    }
#endif
}

__jit_inline void
_sse_rnd_enter(jit_gpr_t r0, int mode)
{
    jit_subi_l(_RSP, _RSP, 8);
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
_sse_rnd_leave(int extra)
{
    LDMXCSRmr(4, _RSP, 0, 0);
    jit_addi_l(_RSP, _RSP, 8 + extra);
}

__jit_inline void
sse_rintr_f_i(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTSS2SILrr(f0, r0);
}

#if __WORDSIZE == 64
__jit_inline void
sse_rintr_f_l(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTSS2SIQrr(f0, r0);
}
#endif

__jit_inline void
sse_rintr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTSD2SILrr(f0, r0);
}

#if __WORDSIZE == 64
__jit_inline void
sse_rintr_d_l(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTSD2SIQrr(f0, r0);
}
#endif

__jit_inline void
sse_roundr_f_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;
    /* load stack with -0.5 if f0 >= 0, else load stack with 0.5 */
    MOVLir(0xbf000000, r0);
    XORPSrr(JIT_FPTMP0, JIT_FPTMP0);
    /* invert -0.5 sign if argument is negative */
    UCOMISSrr(JIT_FPTMP0, f0);
    JAESm((long)_jit.x.pc);
    label = _jit.x.pc;
    XORLir(0x80000000, r0);
    jit_patch_rel_char_at(label, _jit.x.pc);
    jit_pushr_i(r0);
    /* round */
    if (jit_round_to_nearest_p()) {
	sse_rintr_f_i(r0, f0);
	sse_extr_i_f(JIT_FPTMP0, r0);
	/* check difference of fractional part with value in stack */
	SUBSSrr(f0, JIT_FPTMP0);
	UCOMISSmr(0, _RSP, 0, 0, JIT_FPTMP0);
	/* if not zero round is already correct */
	JNESm((long)_jit.x.pc);
	label = _jit.x.pc;
	/* adjust and round again */
	sse_movr_f(JIT_FPTMP0, f0);
	SUBSSmr(0, _RSP, 0, 0, JIT_FPTMP0);
	sse_rintr_f_i(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
	jit_addi_l(_RSP, _RSP, sizeof(long));
    }
    else {
	sse_movr_f(JIT_FPTMP0, f0);
	SUBSSmr(0, _RSP, 0, 0, JIT_FPTMP0);
	_sse_rnd_enter(r0, MXCSR_RND_CHOP);
	sse_rintr_f_i(r0, JIT_FPTMP0);
	_sse_rnd_leave(sizeof(long));
    }
}

#if __WORDSIZE == 64
__jit_inline void
sse_roundr_f_l(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;
    /* load stack with -0.5 if f0 >= 0, else load stack with 0.5 */
    MOVLir(0xbf000000, r0);
    XORPSrr(JIT_FPTMP0, JIT_FPTMP0);
    /* invert -0.5 sign if argument is negative */
    UCOMISSrr(JIT_FPTMP0, f0);
    JAESm((long)_jit.x.pc);
    label = _jit.x.pc;
    XORLir(0x80000000, r0);
    jit_patch_rel_char_at(label, _jit.x.pc);
    jit_pushr_i(r0);
    /* round */
    if (jit_round_to_nearest_p()) {
	sse_rintr_f_l(r0, f0);
	sse_extr_l_f(JIT_FPTMP0, r0);
	/* check difference of fractional part with value in stack */
	SUBSSrr(f0, JIT_FPTMP0);
	UCOMISSmr(0, _RSP, 0, 0, JIT_FPTMP0);
	/* if not zero round is already correct */
	JNESm((long)_jit.x.pc);
	label = _jit.x.pc;
	/* adjust and round again */
	sse_movr_f(JIT_FPTMP0, f0);
	SUBSSmr(0, _RSP, 0, 0, JIT_FPTMP0);
	sse_rintr_f_l(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
	jit_addi_l(_RSP, _RSP, sizeof(long));
    }
    else {
	sse_movr_f(JIT_FPTMP0, f0);
	SUBSSmr(0, _RSP, 0, 0, JIT_FPTMP0);
	_sse_rnd_enter(r0, MXCSR_RND_CHOP);
	sse_rintr_f_l(r0, JIT_FPTMP0);
	_sse_rnd_leave(sizeof(long));
    }
}
#endif

__jit_inline void
sse_roundr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;
    /* load stack with -0.5 if f0 >= 0, else load stack with 0.5 */
    MOVLir(0xbfe00000, r0);
    XORPDrr(JIT_FPTMP0, JIT_FPTMP0);
    /* invert -0.5 sign if argument is negative */
    UCOMISDrr(JIT_FPTMP0, f0);
    JAESm((long)_jit.x.pc);
    label = _jit.x.pc;
    XORLir(0x80000000, r0);
    jit_patch_rel_char_at(label, _jit.x.pc);
#if __WORDSIZE == 32
    jit_pushr_i(r0);
    jit_pushi_i(0);
#else
    SHLQir(32, r0);
    jit_pushr_l(r0);
#endif
    /* round */
    if (jit_round_to_nearest_p()) {
	sse_rintr_d_i(r0, f0);
	sse_extr_i_d(JIT_FPTMP0, r0);
	/* check difference of fractional part with value in stack */
	SUBSDrr(f0, JIT_FPTMP0);
	UCOMISDmr(0, _RSP, 0, 0, JIT_FPTMP0);
	/* if not zero round is already correct */
	JNESm((long)_jit.x.pc);
	label = _jit.x.pc;
	/* adjust and round again */
	sse_movr_d(JIT_FPTMP0, f0);
	SUBSDmr(0, _RSP, 0, 0, JIT_FPTMP0);
	sse_rintr_d_i(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
	jit_addi_l(_RSP, _RSP, sizeof(double));
    }
    else {
	sse_movr_d(JIT_FPTMP0, f0);
	SUBSDmr(0, _RSP, 0, 0, JIT_FPTMP0);
	_sse_rnd_enter(r0, MXCSR_RND_CHOP);
	sse_rintr_d_i(r0, JIT_FPTMP0);
	_sse_rnd_leave(sizeof(double));
    }
}

#if __WORDSIZE == 64
__jit_inline void
sse_roundr_d_l(jit_gpr_t r0, jit_fpr_t f0)
{
    jit_insn	*label;
    /* load stack with -0.5 if f0 >= 0, else load stack with 0.5 */
    MOVLir(0xbfe00000, r0);
    XORPDrr(JIT_FPTMP0, JIT_FPTMP0);
    /* invert -0.5 sign if argument is negative */
    UCOMISDrr(JIT_FPTMP0, f0);
    JAESm((long)_jit.x.pc);
    label = _jit.x.pc;
    XORLir(0x80000000, r0);
    jit_patch_rel_char_at(label, _jit.x.pc);
    SHLQir(32, r0);
    jit_pushr_l(r0);
    /* round */
    if (jit_round_to_nearest_p()) {
	sse_rintr_d_l(r0, f0);
	sse_extr_l_d(JIT_FPTMP0, r0);
	/* check difference of fractional part with value in stack */
	SUBSDrr(f0, JIT_FPTMP0);
	UCOMISDmr(0, _RSP, 0, 0, JIT_FPTMP0);
	/* if not zero round is already correct */
	JNESm((long)_jit.x.pc);
	label = _jit.x.pc;
	/* adjust and round again */
	sse_movr_d(JIT_FPTMP0, f0);
	SUBSDmr(0, _RSP, 0, 0, JIT_FPTMP0);
	sse_rintr_d_l(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
	jit_addi_l(_RSP, _RSP, sizeof(double));
    }
    else {
	sse_movr_d(JIT_FPTMP0, f0);
	SUBSDmr(0, _RSP, 0, 0, JIT_FPTMP0);
	_sse_rnd_enter(r0, MXCSR_RND_CHOP);
	sse_rintr_d_l(r0, JIT_FPTMP0);
	_sse_rnd_leave(sizeof(double));
    }
}
#endif

__jit_inline void
sse_truncr_f_i(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTTSS2SILrr(f0, r0);
}

#if __WORDSIZE == 64
__jit_inline void
sse_truncr_f_l(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTTSS2SIQrr(f0, r0);
}
#endif

__jit_inline void
sse_truncr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTTSD2SILrr(f0, r0);
}

#if __WORDSIZE == 64
__jit_inline void
sse_truncr_d_l(jit_gpr_t r0, jit_fpr_t f0)
{
    CVTTSD2SIQrr(f0, r0);
}
#endif

__jit_inline void
sse_floorr_f_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSSrri(f0, JIT_FPTMP0, MXCSR_RND_DOWN >> 13);
	CVTSS2SILrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_f_i(r0, f0);
	sse_extr_i_f(JIT_FPTMP0, r0);
	UCOMISSrr(f0, JIT_FPTMP0);
	JBESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_f(JIT_FPTMP0, -0.5);
	ADDSSrr(f0, JIT_FPTMP0);
	sse_rintr_f_i(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	sse_rintr_f_i(r0, f0);
	_sse_rnd_leave(0);
    }
}

#if __WORDSIZE == 64
__jit_inline void
sse_floorr_f_l(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSSrri(f0, JIT_FPTMP0, MXCSR_RND_DOWN >> 13);
	CVTSS2SIQrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_f_l(r0, f0);
	sse_extr_l_f(JIT_FPTMP0, r0);
	UCOMISSrr(f0, JIT_FPTMP0);
	JBESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_f(JIT_FPTMP0, -0.5);
	ADDSSrr(f0, JIT_FPTMP0);
	sse_rintr_f_l(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	sse_rintr_f_l(r0, f0);
	_sse_rnd_leave(0);
    }
}
#endif

__jit_inline void
sse_floorr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSDrri(f0, JIT_FPTMP0, MXCSR_RND_DOWN >> 13);
	CVTSD2SILrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_d_i(r0, f0);
	sse_extr_i_d(JIT_FPTMP0, r0);
	UCOMISDrr(f0, JIT_FPTMP0);
	JBESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_d(JIT_FPTMP0, -0.5);
	ADDSDrr(f0, JIT_FPTMP0);
	sse_rintr_d_i(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	sse_rintr_d_i(r0, f0);
	_sse_rnd_leave(0);
    }
}

#if __WORDSIZE == 64
__jit_inline void
sse_floorr_d_l(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSDrri(f0, JIT_FPTMP0, MXCSR_RND_DOWN >> 13);
	CVTSD2SIQrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_d_l(r0, f0);
	sse_extr_l_d(JIT_FPTMP0, r0);
	UCOMISDrr(f0, JIT_FPTMP0);
	JBESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_d(JIT_FPTMP0, -0.5);
	ADDSDrr(f0, JIT_FPTMP0);
	sse_rintr_d_l(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_DOWN);
	sse_rintr_d_l(r0, f0);
	_sse_rnd_leave(0);
    }
}
#endif

__jit_inline void
sse_ceilr_f_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSSrri(f0, JIT_FPTMP0, MXCSR_RND_UP >> 13);
	CVTSS2SILrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_f_i(r0, f0);
	sse_extr_i_f(JIT_FPTMP0, r0);
	UCOMISSrr(f0, JIT_FPTMP0);
	JAESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_f(JIT_FPTMP0, 0.5);
	ADDSSrr(f0, JIT_FPTMP0);
	sse_rintr_f_i(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_UP);
	sse_rintr_f_i(r0, f0);
	_sse_rnd_leave(0);
    }
}

#if __WORDSIZE == 64
__jit_inline void
sse_ceilr_f_l(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSSrri(f0, JIT_FPTMP0, MXCSR_RND_UP >> 13);
	CVTSS2SIQrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_f_l(r0, f0);
	sse_extr_l_f(JIT_FPTMP0, r0);
	UCOMISSrr(f0, JIT_FPTMP0);
	JAESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_f(JIT_FPTMP0, 0.5);
	ADDSSrr(f0, JIT_FPTMP0);
	sse_rintr_f_l(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_UP);
	sse_rintr_f_l(r0, f0);
	_sse_rnd_leave(0);
    }
}
#endif

__jit_inline void
sse_ceilr_d_i(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSDrri(f0, JIT_FPTMP0, MXCSR_RND_UP >> 13);
	CVTSD2SILrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_d_i(r0, f0);
	sse_extr_i_d(JIT_FPTMP0, r0);
	UCOMISDrr(f0, JIT_FPTMP0);
	JAESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_d(JIT_FPTMP0, 0.5);
	ADDSDrr(f0, JIT_FPTMP0);
	sse_rintr_d_i(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_UP);
	sse_rintr_d_i(r0, f0);
	_sse_rnd_leave(0);
    }
}

#if __WORDSIZE == 64
__jit_inline void
sse_ceilr_d_l(jit_gpr_t r0, jit_fpr_t f0)
{
    if (jit_sse4_1_p()) {
	ROUNDSDrri(f0, JIT_FPTMP0, MXCSR_RND_UP >> 13);
	CVTSD2SIQrr(JIT_FPTMP0, r0);
    }
    else if (jit_round_to_nearest_p()) {
	jit_insn	*label;
	sse_rintr_d_l(r0, f0);
	sse_extr_l_d(JIT_FPTMP0, r0);
	UCOMISDrr(f0, JIT_FPTMP0);
	JAESm((long)_jit.x.pc);
	label = _jit.x.pc;
	sse_movi_d(JIT_FPTMP0, 0.5);
	ADDSDrr(f0, JIT_FPTMP0);
	sse_rintr_d_l(r0, JIT_FPTMP0);
	jit_patch_rel_char_at(label, _jit.x.pc);
    }
    else {
	_sse_rnd_enter(r0, MXCSR_RND_UP);
	sse_rintr_d_l(r0, f0);
	_sse_rnd_leave(0);
    }
}
#endif

__jit_inline void
_sse_fp_cmp_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1, int code)
{
    int		 rc;
    jit_gpr_t	 reg;
    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }
    XORLrr(reg, reg);
    UCOMISSrr(f0, f1);
    SETCCir(code, reg);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

__jit_inline void
_sse_fp_cmp_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1, int code)
{
    int		 rc;
    jit_gpr_t	 reg;
    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }
    XORLrr(reg, reg);
    UCOMISDrr(f0, f1);
    SETCCir(code, reg);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

__jit_inline void
sse_ltr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_A);
}

__jit_inline void
sse_ltr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_A);
}

__jit_inline void
sse_ler_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_AE);
}

__jit_inline void
sse_ler_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_AE);
}

__jit_inline void
sse_eqr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    int		 rc;
    jit_gpr_t	 reg;
    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }
    XORLrr(reg, reg);
    UCOMISSrr(f0, f1);
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    SETEr(reg);
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

__jit_inline void
sse_eqr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    int		 rc;
    jit_gpr_t	 reg;
    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }
    XORLrr(reg, reg);
    UCOMISDrr(f0, f1);
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    SETEr(reg);
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

__jit_inline void
sse_ger_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f1, f0, X86_CC_AE);
}

__jit_inline void
sse_ger_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f1, f0, X86_CC_AE);
}

__jit_inline void
sse_gtr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f1, f0, X86_CC_A);
}

__jit_inline void
sse_gtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f1, f0, X86_CC_A);
}

__jit_inline void
sse_ner_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    int		 rc;
    jit_gpr_t	 reg;
    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }
    MOVLir(1, reg);
    UCOMISSrr(f0, f1);
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    SETNEr(reg);
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

__jit_inline void
sse_ner_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    int		 rc;
    jit_gpr_t	 reg;
    if ((rc = jit_check8(r0)))
	reg = r0;
    else {
	MOVLrr(_RAX, r0);
	reg = _RAX;
    }
    MOVLir(1, reg);
    UCOMISDrr(f0, f1);
    JPESm((long)(_jit.x.pc + 3));
    _jitl.label = _jit.x.pc;
    SETNEr(reg);
    jit_patch_rel_char_at(_jitl.label, _jit.x.pc);
    if (!rc)
	XCHGLrr(_RAX, r0);
}

__jit_inline void
sse_unltr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f1, f0, X86_CC_NAE);
}

__jit_inline void
sse_unltr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f1, f0, X86_CC_NAE);
}

__jit_inline void
sse_unler_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f1, f0, X86_CC_NA);
}

__jit_inline void
sse_unler_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f1, f0, X86_CC_NA);
}

__jit_inline void
sse_uneqr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_E);
}

__jit_inline void
sse_uneqr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_E);
}

__jit_inline void
sse_unger_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_NA);
}

__jit_inline void
sse_unger_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_NA);
}

__jit_inline void
sse_ungtr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_NAE);
}

__jit_inline void
sse_ungtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_NAE);
}

__jit_inline void
sse_ltgtr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_NE);
}

__jit_inline void
sse_ltgtr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_NE);
}

__jit_inline void
sse_ordr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_NP);
}

__jit_inline void
sse_ordr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_NP);
}

__jit_inline void
sse_unordr_f(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_f(r0, f0, f1, X86_CC_P);
}

__jit_inline void
sse_unordr_d(jit_gpr_t r0, jit_fpr_t f0, jit_fpr_t f1)
{
    _sse_fp_cmp_d(r0, f0, f1, X86_CC_P);
}

__jit_inline jit_insn *
sse_bltr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bler_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_beqr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_insn	*jp_label;
    UCOMISSrr(f0, f1);
    /* jump after user jump if (unordered) */
    JPSm((long)(_jit.x.pc + 5));
    jp_label = _jit.x.pc;
    JEm(label);
    jit_patch_rel_char_at(jp_label, _jit.x.pc);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bger_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f1, f0);
    JAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bgtr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f1, f0);
    JAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bner_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
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

__jit_inline jit_insn *
sse_bunltr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f1, f0);
    JNAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bunler_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f1, f0);
    JNAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_buneqr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bunger_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JNAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bungtr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JNAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bltgtr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JNEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bordr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JNPm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bunordr_f(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISSrr(f0, f1);
    JPm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bltr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bler_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_beqr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    jit_insn	*jp_label;
    UCOMISDrr(f0, f1);
    /* jump after user jump if (unordered) */
    JPSm((long)(_jit.x.pc + 5));
    jp_label = _jit.x.pc;
    JEm(label);
    jit_patch_rel_char_at(jp_label, _jit.x.pc);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bger_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f1, f0);
    JAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bgtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f1, f0);
    JAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bner_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
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

__jit_inline jit_insn *
sse_bunltr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f1, f0);
    JNAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bunler_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f1, f0);
    JNAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_buneqr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bunger_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JNAm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bungtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JNAEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bltgtr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JNEm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bordr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JNPm(label);
    return (_jit.x.pc);
}

__jit_inline jit_insn *
sse_bunordr_d(jit_insn *label, jit_fpr_t f0, jit_fpr_t f1)
{
    UCOMISDrr(f0, f1);
    JPm(label);
    return (_jit.x.pc);
}

#endif /* __lightning_fp_sse_h */
