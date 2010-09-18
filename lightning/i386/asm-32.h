/******************************** -*- C -*- ****************************
 *
 *	Run-time assembler for the i386
 *
 ***********************************************************************/


/***********************************************************************
 *
 * Copyright 2003 Gwenole Beauchesne
 * Copyright 2006 Free Software Foundation, Inc.
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




#ifndef __lightning_asm_h
#define __lightning_asm_h
#ifndef LIGHTNING_DEBUG

/*	OPCODE	+ i		= immediate operand
 *		+ r		= register operand
 *		+ m		= memory operand (disp,base,index,scale)
 *		+ sr/sm		= a star preceding a register or memory
 */

#define CALLsr(R)			CALLLsr(R)
#define JMPsr(R)			JMPLsr(R)

/* --- Increment/Decrement instructions ------------------------------------ */
__jit_inline void
_dec_sil_r(jit_gpr_t rd)
{
    _Or(0x48, _rA(rd));
}

__jit_inline void
_inc_sil_r(jit_gpr_t rd)
{
    _Or(0x40, _rA(rd));
}

/* --- REX prefixes -------------------------------------------------------- */
__jit_inline void
_REXBrr(jit_gpr_t rr, jit_gpr_t mr)
{
}

__jit_inline void
_REXBmr(jit_gpr_t mb, jit_gpr_t mi, jit_gpr_t rd)
{
}

__jit_inline void
_REXBrm(jit_gpr_t rs, jit_gpr_t mb, jit_gpr_t mi)
{
}

__jit_inline void
_REXLr(jit_gpr_t rr)
{
}

__jit_inline void
_REXLm(jit_gpr_t mb, jit_gpr_t mi)
{
}

__jit_inline void
_REXBLrr(jit_gpr_t rr, jit_gpr_t mr)
{
}

__jit_inline void
_REXLrr(jit_gpr_t rr, jit_gpr_t mr)
{
}

__jit_inline void
_REXLmr(jit_gpr_t mb, jit_gpr_t mi, jit_gpr_t rd)
{
}

__jit_inline void
_REXLrm(jit_gpr_t rs, jit_gpr_t mb, jit_gpr_t mi)
{
}

/* --- Push/Pop instructions ----------------------------------------------- */
__jit_inline void
POPWr(jit_gpr_t rd)
{
    _d16();
    _pop_sil_r(rd);
}

__jit_inline void
POPWm(int md, jit_gpr_t mb, jit_gpr_t mi, int ms)
{
    _d16();
    _pop_sil_m(md, mb, mi, ms);
}

__jit_inline void
POPLr(jit_gpr_t rd)
{
    _pop_sil_r(rd);
}

__jit_inline void
POPLm(int md, jit_gpr_t mb, jit_gpr_t mi, int ms)
{
    _pop_sil_m(md, mb, mi, ms);
}

__jit_inline void
PUSHWr(jit_gpr_t rs)
{
    _d16();
    _push_sil_r(rs);
}

__jit_inline void
PUSHWm(int md, jit_gpr_t mb, jit_gpr_t mi, int ms)
{
    _d16();
    _push_sil_m(md, mb, mi, ms);
}

__jit_inline void
PUSHWi(long im)
{
    if (_s8P(im))
	_push_c_i(im);
    else {
	_d16();
	_O(0x68);
	_jit_W(_s16(im));
    }
}

__jit_inline void
PUSHLr(jit_gpr_t rs)
{
    _push_sil_r(rs);
}

__jit_inline void
PUSHLm(int md, jit_gpr_t mb, jit_gpr_t mi, int ms)
{
    _push_sil_m(md, mb, mi, ms);
}

__jit_inline void
PUSHLi(long im)
{
    _push_il_i(im);
}

#endif	/* LIGHTNING_DEBUG */
#endif	/* __lightning_asm_h */
