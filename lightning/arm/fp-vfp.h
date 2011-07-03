/******************************** -*- C -*- ****************************
 *
 *	Support macros for arm VFP floating-point math
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2011 Free Software Foundation, Inc.
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
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 ***********************************************************************/


#ifndef __lightning_fp_vfp_h
#define __lightning_fp_vfp_h

__jit_inline void
vfp_movr_f(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	if (r0 == JIT_FPRET)
	    /* jit_ret() must follow! */
	    _VMOV_A_S(_R0, r1);
	else
	    _VMOV_F32(r0, r1);
    }
}

__jit_inline void
vfp_movr_d(jit_state_t _jit, jit_fpr_t r0, jit_fpr_t r1)
{
    if (r0 != r1) {
	if (r0 == JIT_FPRET)
	    /* jit_ret() must follow! */
	    _VMOV_AA_D(_R0, _R1, r1);
	else
	    _VMOV_F64(r0, r1);
    }
}

__jit_inline void
vfp_movi_f(jit_state_t _jit, jit_fpr_t r0, float i0)
{
    union {
	int	i;
	float	f;
    } u;
    int		code;
    u.f = i0;
    if (r0 == JIT_FPRET)
	/* jit_ret() must follow! */
	jit_movi_i(_R0, u.i);
    else {
	if ((code = encode_vfp_immediate(ARM_VMOVI, u.i, u.i)) != -1)
	    _VMOVI(code, r0);
	else if ((code = encode_vfp_immediate(ARM_VMVNI, ~u.i, ~u.i)) != -1)
	    _VMVNI(code, r0);
	else {
	    jit_movi_i(JIT_FTMP, u.i);
	    _VMOV_S_A(r0, JIT_FTMP);
	}
    }
}

__jit_inline void
vfp_movi_d(jit_state_t _jit, jit_fpr_t r0, double i0)
{
    union {
	int	i[2];
	double	d;
    } u;
    int		code;
    u.d = i0;
    if (r0 == JIT_FPRET) {
	/* jit_ret() must follow! */
	jit_movi_i(_R0, u.i[0]);
	jit_movi_i(_R1, u.i[1]);
    }
    else {
	if ((code = encode_vfp_immediate(ARM_VMOVI,
					 u.i[0], u.i[1])) != -1)
	    _VMOVI(code, r0);
	else if ((code = encode_vfp_immediate(ARM_VMVNI,
					      ~u.i[0], ~u.i[1])) != -1)
	    _VMVNI(code, r0);
	else {
	    jit_movi_i(JIT_TMP, u.i[0]);
	    jit_movi_i(JIT_FTMP, u.i[1]);
	    _VMOV_D_AA(r0, JIT_TMP, JIT_FTMP);
	}
    }
}

__jit_inline void
vfp_extr_i_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    _VMOV_V_I32(r0, r1);
    _VCVT_F32_S32(r0, r0);
}

__jit_inline void
vfp_extr_i_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1)
{
    _VMOV_V_I32(r0, r1);
    _VCVT_F64_S32(r0, r0);
}

#define vfp_extr_d_f(_jit, r0, r1)	_VCVT_F64_F32(r0, r1)
#define vfp_extr_f_d(_jit, r0, r1)	_VCVT_F32_F64(r0, r1)

/* FIXME keeping for now, in case there exists, and/or to support
 * only 4 double precision registers */
#define vfp_get_tmp(r, n)						\
    jit_fpr_t		 tmp;						\
    if (0) {								\
	tmp = r == _F0 ? _F1 : _F0;					\
	_VPUSH_F##n(tmp, 2);						\
    }									\
    else								\
	tmp = _F7
#define vfp_unget_tmp(n)						\
    if (0)								\
	_VPOP_F##n(tmp, 2)

__jit_inline void
vfp_rintr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 32);
    _VCVT_S32_F32(tmp, r1);
    _VMOV_A_S32(r0, tmp);
    vfp_unget_tmp(32);
}

__jit_inline void
vfp_rintr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 64);
    _VCVT_S32_F64(tmp, r1);
    _VMOV_A_S32(r0, tmp);
    vfp_unget_tmp(64);
}

__jit_inline void
vfp_roundr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 32);
    _VMOV_A_S(JIT_FTMP, r1);
    if (jit_thumb_p())
	_TSTI(JIT_FTMP, encode_arm_immediate(0x80000000));
    else
	_ANDSI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(0x80000000));
    /* add -0.5 if negative */
    _CC_MOVI(ARM_CC_NE, JIT_FTMP, encode_arm_immediate(0xbf000000));
    /* add 0.5 if positive */
    _CC_MOVI(ARM_CC_EQ, JIT_FTMP, encode_arm_immediate(0x3f000000));
    _VMOV_S_A(tmp, JIT_FTMP);
    _VADD_F32(tmp, r1, tmp);
    /* truncate to zero */
    _VCVT_S32_F32(tmp, tmp);
    _VMOV_A_S32(r0, tmp);
    vfp_unget_tmp(32);
}

__jit_inline void
vfp_roundr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 64);
    _VMOV_AA_D(JIT_TMP, JIT_FTMP, r1);
    if (jit_thumb_p())
	_TSTI(JIT_FTMP, encode_arm_immediate(0x80000000));
    else
	_ANDSI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(0x80000000));
    _MOVI(JIT_FTMP, encode_arm_immediate(0x0fe00000));
    /* add -0.5 if negative */
    _CC_ORI(ARM_CC_NE, JIT_FTMP, JIT_FTMP, encode_arm_immediate(0xb0000000));
    /* add 0.5 if positive */
    _CC_ORI(ARM_CC_EQ, JIT_FTMP, JIT_FTMP, encode_arm_immediate(0x30000000));
    _MOVI(JIT_TMP, 0);
    _VMOV_D_AA(tmp, JIT_TMP, JIT_FTMP);
    _VADD_F64(tmp, r1, tmp);
    /* truncate to zero */
    _VCVT_S32_F64(tmp, tmp);
    _VMOV_A_S32(r0, tmp);
    vfp_unget_tmp(64);
}

#define vfp_truncr_f_i(_jit, r0, r1)	vfp_rintr_f_i(_jit, r0, r1)
#define vfp_truncr_d_i(_jit, r0, r1)	vfp_rintr_d_i(_jit, r0, r1)

__jit_inline void
vfp_ceilr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 32);
    _VMRS(JIT_TMP);
    _BICI(JIT_FTMP, JIT_TMP, encode_arm_immediate(FPSCR_RMASK));
    _ORI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(FPSCR_RP));
    _VMSR(JIT_FTMP);
    _VCVTR_S32_F32(tmp, r1);
    _VMOV_A_S32(r0, tmp);
    _VMSR(JIT_TMP);
    vfp_unget_tmp(32);
}

__jit_inline void
vfp_ceilr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 64);
    _VMRS(JIT_TMP);
    _BICI(JIT_FTMP, JIT_TMP, encode_arm_immediate(FPSCR_RMASK));
    _ORI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(FPSCR_RP));
    _VMSR(JIT_FTMP);
    _VCVTR_S32_F64(tmp, r1);
    _VMOV_A_S32(r0, tmp);
    _VMSR(JIT_TMP);
    vfp_unget_tmp(64);
}

__jit_inline void
vfp_floorr_f_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 32);
    _VMRS(JIT_TMP);
    _BICI(JIT_FTMP, JIT_TMP, encode_arm_immediate(FPSCR_RMASK));
    _ORI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(FPSCR_RM));
    _VMSR(JIT_FTMP);
    _VCVTR_S32_F32(tmp, r1);
    _VMOV_A_S32(r0, tmp);
    _VMSR(JIT_TMP);
    vfp_unget_tmp(32);
}

__jit_inline void
vfp_floorr_d_i(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1)
{
    vfp_get_tmp(r1, 64);
    _VMRS(JIT_TMP);
    _BICI(JIT_FTMP, JIT_TMP, encode_arm_immediate(FPSCR_RMASK));
    _ORI(JIT_FTMP, JIT_FTMP, encode_arm_immediate(FPSCR_RM));
    _VMSR(JIT_FTMP);
    _VCVTR_S32_F64(tmp, r1);
    _VMOV_A_S32(r0, tmp);
    _VMSR(JIT_TMP);
    vfp_unget_tmp(64);
}

#define vfp_absr_f(_jit, r0, r1)	_VABS_F32(r0, r1)
#define vfp_absr_d(_jit, r0, r1)	_VABS_F64(r0, r1)
#define vfp_negr_f(_jit, r0, r1)	_VNEG_F32(r0, r1)
#define vfp_negr_d(_jit, r0, r1)	_VNEG_F64(r0, r1)
#define vfp_sqrtr_f(_jit, r0, r1)	_VSQRT_F32(r0, r1)
#define vfp_sqrtr_d(_jit, r0, r1)	_VSQRT_F64(r0, r1)
#define vfp_addr_f(_jit, r0, r1, r2)	_VADD_F32(r0, r1, r2)
#define vfp_addr_d(_jit, r0, r1, r2)	_VADD_F64(r0, r1, r2)
#define vfp_subr_f(_jit, r0, r1, r2)	_VSUB_F32(r0, r1, r2)
#define vfp_subr_d(_jit, r0, r1, r2)	_VSUB_F64(r0, r1, r2)
#define vfp_mulr_f(_jit, r0, r1, r2)	_VMUL_F32(r0, r1, r2)
#define vfp_mulr_d(_jit, r0, r1, r2)	_VMUL_F64(r0, r1, r2)
#define vfp_divr_f(_jit, r0, r1, r2)	_VDIV_F32(r0, r1, r2)
#define vfp_divr_d(_jit, r0, r1, r2)	_VDIV_F64(r0, r1, r2)

__jit_inline void
vfp_ltr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_PL, r0, 0);
    _CC_MOVI(ARM_CC_MI, r0, 1);
}

__jit_inline void
vfp_ltr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_PL, r0, 0);
    _CC_MOVI(ARM_CC_MI, r0, 1);
}

__jit_inline void
vfp_ler_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_HI, r0, 0);
    _CC_MOVI(ARM_CC_LS, r0, 1);
}

__jit_inline void
vfp_ler_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_HI, r0, 0);
    _CC_MOVI(ARM_CC_LS, r0, 1);
}

__jit_inline void
vfp_eqr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_NE, r0, 0);
    _CC_MOVI(ARM_CC_EQ, r0, 1);
}

__jit_inline void
vfp_eqr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_NE, r0, 0);
    _CC_MOVI(ARM_CC_EQ, r0, 1);
}

__jit_inline void
vfp_ger_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_LT, r0, 0);
    _CC_MOVI(ARM_CC_GE, r0, 1);
}

__jit_inline void
vfp_ger_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_LT, r0, 0);
    _CC_MOVI(ARM_CC_GE, r0, 1);
}

__jit_inline void
vfp_gtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_LE, r0, 0);
    _CC_MOVI(ARM_CC_GT, r0, 1);
}

__jit_inline void
vfp_gtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_LE, r0, 0);
    _CC_MOVI(ARM_CC_GT, r0, 1);
}

__jit_inline void
vfp_ner_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_EQ, r0, 0);
    _CC_MOVI(ARM_CC_NE, r0, 1);
}

__jit_inline void
vfp_ner_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_EQ, r0, 0);
    _CC_MOVI(ARM_CC_NE, r0, 1);
}

__jit_inline void
vfp_unltr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 1);
    _CC_MOVI(ARM_CC_GE, r0, 0);
}

__jit_inline void
vfp_unltr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 1);
    _CC_MOVI(ARM_CC_GE, r0, 0);
}

__jit_inline void
vfp_unler_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 1);
    _CC_MOVI(ARM_CC_GT, r0, 0);
}

__jit_inline void
vfp_unler_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 1);
    _CC_MOVI(ARM_CC_GT, r0, 0);
}

__jit_inline void
vfp_uneqr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_NE, r0, 0);
    _CC_MOVI(ARM_CC_EQ, r0, 1);
    _CC_MOVI(ARM_CC_VS, r0, 1);
}

__jit_inline void
vfp_uneqr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_NE, r0, 0);
    _CC_MOVI(ARM_CC_EQ, r0, 1);
    _CC_MOVI(ARM_CC_VS, r0, 1);
}

__jit_inline void
vfp_unger_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_CS, r0, 1);
}

__jit_inline void
vfp_unger_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_CS, r0, 1);
}

__jit_inline void
vfp_ungtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_HI, r0, 1);
}

__jit_inline void
vfp_ungtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_HI, r0, 1);
}

__jit_inline void
vfp_ltgtr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_NE, r0, 1);
    _CC_MOVI(ARM_CC_EQ, r0, 0);
    _CC_MOVI(ARM_CC_VS, r0, 0);
}

__jit_inline void
vfp_ltgtr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _CC_MOVI(ARM_CC_NE, r0, 1);
    _CC_MOVI(ARM_CC_EQ, r0, 0);
    _CC_MOVI(ARM_CC_VS, r0, 0);
}

__jit_inline void
vfp_ordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 1);
    _CC_MOVI(ARM_CC_VS, r0, 0);
}

__jit_inline void
vfp_ordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 1);
    _CC_MOVI(ARM_CC_VS, r0, 0);
}

__jit_inline void
vfp_unordr_f(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F32(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_VS, r0, 1);
}

__jit_inline void
vfp_unordr_d(jit_state_t _jit, jit_gpr_t r0, jit_fpr_t r1, jit_fpr_t r2)
{
    _VCMP_F64(r1, r2);
    _VMRS(_R15);
    _MOVI(r0, 0);
    _CC_MOVI(ARM_CC_VS, r0, 1);
}

__jit_inline jit_insn *
vfp_bltr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_MI, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bltr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_MI, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bler_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_LS, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bler_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_LS, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_beqr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_EQ, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_beqr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_EQ, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bger_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_GE, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bger_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_GE, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bgtr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_GT, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bgtr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_GT, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bner_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_NE, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bner_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_NE, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bunltr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_GE, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(i);
    return (l);
}

__jit_inline jit_insn *
vfp_bunltr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_GE, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(i);
    return (l);
}

__jit_inline jit_insn *
vfp_bunler_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_GT, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(i);
    return (l);
}

__jit_inline jit_insn *
vfp_bunler_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_GT, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(i);
    return (l);
}

__jit_inline jit_insn *
vfp_buneqr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*j;
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_VS, 0);
    j = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    jit_patch(i);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(j);
    return (l);
}

__jit_inline jit_insn *
vfp_buneqr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*j;
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_VS, 0);
    j = _jit->x.pc;
    _CC_B(ARM_CC_NE, 0);
    jit_patch(i);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(j);
    return (l);
}

__jit_inline jit_insn *
vfp_bunger_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_MI, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_HS, d & 0x00ffffff);
    jit_patch(i);
    return (l);
}

__jit_inline jit_insn *
vfp_bunger_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_MI, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_HS, d & 0x00ffffff);
    jit_patch(i);
    return (l);
}

__jit_inline jit_insn *
vfp_bungtr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_HI, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bungtr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_HI, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bltgtr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*j;
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_VS, 0);
    j = _jit->x.pc;
    _CC_B(ARM_CC_EQ, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(i);
    jit_patch(j);
    return (l);
}

__jit_inline jit_insn *
vfp_bltgtr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*i;
    jit_insn	*j;
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    i = _jit->x.pc;
    _CC_B(ARM_CC_VS, 0);
    j = _jit->x.pc;
    _CC_B(ARM_CC_EQ, 0);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _B(d & 0x00ffffff);
    jit_patch(i);
    jit_patch(j);
    return (l);
}

__jit_inline jit_insn *
vfp_bordr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_VC, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bordr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_VC, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bunordr_f(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F32(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_VS, d & 0x00ffffff);
    return (l);
}

__jit_inline jit_insn *
vfp_bunordr_d(jit_state_t _jit, jit_insn *i0, jit_fpr_t r0, jit_fpr_t r1)
{
    jit_insn	*l;
    long	 d;
    _VCMP_F64(r0, r1);
    _VMRS(_R15);
    l = _jit->x.pc;
    d = (((long)i0 - (long)l) >> 2) - 2;
    assert(_s24P(d));
    _CC_B(ARM_CC_VS, d & 0x00ffffff);
    return (l);
}

#define vfp_ldr_f(_jit, r0, r1)		_VLDR_F32(r0, r1, 0)
#define vfp_ldr_d(_jit, r0, r1)		_VLDR_F64(r0, r1, 0)

__jit_inline void
vfp_ldi_f(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    jit_movi_i(JIT_FTMP, (int)i0);
    _VLDR_F32(r0, JIT_FTMP, 0);
}

__jit_inline void
vfp_ldi_d(jit_state_t _jit, jit_fpr_t r0, void *i0)
{
    jit_movi_i(JIT_FTMP, (int)i0);
    _VLDR_F64(r0, JIT_FTMP, 0);
}

__jit_inline void
vfp_ldxr_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADD(JIT_FTMP, r1, r2);
    _VLDR_F32(r0, JIT_FTMP, 0);
}

__jit_inline void
vfp_ldxr_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, jit_gpr_t r2)
{
    _ADD(JIT_FTMP, r1, r2);
    _VLDR_F64(r0, JIT_FTMP, 0);
}

__jit_inline void
vfp_ldxi_f(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 >= 0) {
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VLDR_F32(r0, r1, i0);
	else {
	    jit_addi_i(JIT_FTMP, r1, i0);
	    _VLDR_F32(r0, JIT_FTMP, 0);
	}
    }
    else {
	i0 = -i0;
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VLDRN_F32(r0, r1, i0);
	else {
	    jit_subi_i(JIT_FTMP, r1, i0);
	    _VLDRN_F32(r0, JIT_FTMP, 0);
	}
    }
}

__jit_inline void
vfp_ldxi_d(jit_state_t _jit, jit_fpr_t r0, jit_gpr_t r1, int i0)
{
    if (i0 >= 0) {
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VLDR_F64(r0, r1, i0);
	else {
	    jit_addi_i(JIT_FTMP, r1, i0);
	    _VLDR_F64(r0, JIT_FTMP, 0);
	}
    }
    else {
	i0 = -i0;
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VLDRN_F64(r0, r1, i0);
	else {
	    jit_subi_i(JIT_FTMP, r1, i0);
	    _VLDRN_F64(r0, JIT_FTMP, 0);
	}
    }
}

#define vfp_str_f(_jit, r0, r1)		_VSTR_F32(r1, r0, 0)
#define vfp_str_d(_jit, r0, r1)		_VSTR_F64(r1, r0, 0)

__jit_inline void
vfp_sti_f(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    jit_movi_i(JIT_FTMP, (int)i0);
    _VSTR_F32(r0, JIT_FTMP, 0);
}

__jit_inline void
vfp_sti_d(jit_state_t _jit, void *i0, jit_fpr_t r0)
{
    jit_movi_i(JIT_FTMP, (int)i0);
    _VSTR_F64(r0, JIT_FTMP, 0);
}

__jit_inline void
vfp_stxr_f(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    _ADD(JIT_FTMP, r0, r1);
    _VSTR_F32(r2, JIT_FTMP, 0);
}

__jit_inline void
vfp_stxr_d(jit_state_t _jit, jit_gpr_t r0, jit_gpr_t r1, jit_fpr_t r2)
{
    _ADD(JIT_FTMP, r0, r1);
    _VSTR_F64(r2, JIT_FTMP, 0);
}

__jit_inline void
vfp_stxi_f(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    if (i0 >= 0) {
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VSTR_F32(r1, r0, i0);
	else {
	    jit_addi_i(JIT_FTMP, r0, i0);
	    _VSTR_F32(r1, JIT_FTMP, 0);
	}
    }
    else {
	i0 = -i0;
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VSTRN_F32(r1, r0, i0);
	else {
	    jit_addi_i(JIT_FTMP, r1, i0);
	    _VSTRN_F32(r0, JIT_FTMP, 0);
	}
    }
}

__jit_inline void
vfp_stxi_d(jit_state_t _jit, int i0, jit_gpr_t r0, jit_fpr_t r1)
{
    if (i0 >= 0) {
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VSTR_F64(r1, r0, i0);
	else {
	    jit_addi_i(JIT_FTMP, r0, i0);
	    _VSTR_F64(r1, JIT_FTMP, 0);
	}
    }
    else {
	i0 = -i0;
	assert(!(i0 & 3));
	i0 >>= 2;
	if (i0 < 256)
	    _VSTRN_F64(r1, r0, i0);
	else {
	    jit_subi_i(JIT_FTMP, r1, i0);
	    _VSTRN_F64(r0, JIT_FTMP, 0);
	}
    }
}

__jit_inline void
vfp_getarg_f(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (i0 < 4)
#if 0
	_VMOV_S_A(r0, i0);
#else
	vfp_ldxi_f(_jit, r0, JIT_FP, i0 << 2);
#endif
    else
	vfp_ldxi_f(_jit, r0, JIT_FP, i0);
}

__jit_inline void
vfp_getarg_d(jit_state_t _jit, jit_fpr_t r0, int i0)
{
    if (i0 < 4)
#if 0
	_VMOV_D_AA(r0, i0, i0 + 1);
#else
	vfp_ldxi_d(_jit, r0, JIT_FP, i0 << 2);
#endif
    else
	vfp_ldxi_d(_jit, r0, JIT_FP, i0);
}

__jit_inline void
vfp_pusharg_f(jit_state_t _jit, jit_fpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;
    assert(ofs < 256);
    _jitl.stack_offset -= sizeof(float);
    _VMOV_A_S(JIT_FTMP, r0);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] &= ~(1 << (ofs & 31));
    jit_stxi_i(0, JIT_SP, JIT_FTMP);
}

__jit_inline void
vfp_pusharg_d(jit_state_t _jit, jit_fpr_t r0)
{
    int		ofs = _jitl.nextarg_put++;
    assert(ofs < 256);
    _jitl.stack_offset -= sizeof(double);
    _VMOV_AA_D(JIT_TMP, JIT_FTMP, r0);
    _jitl.arguments[ofs] = (int *)_jit->x.pc;
    _jitl.types[ofs >> 5] |= 1 << (ofs & 31);
    jit_stxi_i(0, JIT_SP, JIT_TMP);
    jit_stxi_i(0, JIT_SP, JIT_FTMP);
}

#define vfp_retval_f(_jit, r0)		_VMOV_S_A(r0, _R0)
#define vfp_retval_d(_jit, r0)		_VMOV_D_AA(r0, _R0, _R1)

#undef vfp_unget_tmp
#undef vfp_get_tmp

#endif /* __lightning_fp_vfp_h */
