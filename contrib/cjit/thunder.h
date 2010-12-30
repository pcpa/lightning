/*
 * Copyright (C) 2010  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#ifndef _thunder_h
#define _thunder_h

#define ejit_i				ejit_i
#define ejit_ir				ejit_i
#define ejit_fr				ejit_i
#define ejit_n				ejit_p
#define ejit_p_ir			ejit_p_i
#define ejit_p_fr			ejit_p_i
#define ejit_ir_i			ejit_i_i
#define ejit_ir_ui			ejit_i_i
#define ejit_ir_l			ejit_i_l
#define ejit_ir_ul			ejit_i_l
#define ejit_ir_p			ejit_i_p
#define ejit_ir_f			ejit_i_f
#define ejit_ir_d			ejit_i_d
#define ejit_ir_ir			ejit_i_i
#define ejit_ir_fr			ejit_i_i
#define ejit_ir_n			ejit_i_p
#define ejit_fr_p			ejit_i_p
#define ejit_fr_ir			ejit_i_i
#define ejit_fr_fr			ejit_i_i
#define ejit_fr_n			ejit_i_p
#define ejit_n_ir			ejit_p_i
#define ejit_l_ir_ir			ejit_l_i_i
#define ejit_l_ir_fr			ejit_l_i_i
#define ejit_ir_ir_i			ejit_i_i_i
#define ejit_ir_ir_ui			ejit_i_i_i
#define ejit_ir_ir_l			ejit_i_i_l
#define ejit_ir_ir_ul			ejit_i_i_l
#define ejit_ir_ir_p			ejit_i_i_p
#define ejit_ir_ir_ir			ejit_i_i_i
#define ejit_ir_ir_fr			ejit_i_i_i
#define ejit_ir_fr_fr			ejit_i_i_i
#define ejit_fr_ir_l			ejit_i_i_l
#define ejit_fr_ir_ir			ejit_i_i_i
#define ejit_fr_fr_fr			ejit_i_i_i
#define ejit_n_ir_i			ejit_n_i_i
#define ejit_n_ir_ui			ejit_n_i_i
#define ejit_n_ir_l			ejit_n_i_l
#define ejit_n_ir_ul			ejit_n_i_l
#define ejit_n_ir_p			ejit_n_i_p
#define ejit_n_ir_ir			ejit_n_i_i
#define ejit_n_fr_fr			ejit_n_i_i

#define ejit_gpr(value)			(value)
#define ejit_fpr(value)			(value)

#define ejit_note(st)							\
    ejit(st, code_note)
#define ejit_label(st)							\
    ejit(st, code_label)
#define ejit_addr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addr_i, u, v, w)
#define ejit_addr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addr_ui, u, v, w)
#define ejit_addr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addr_l, u, v, w)
#define ejit_addr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addr_ul, u, v, w)
#define ejit_addr_p(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addr_p, u, v, w)
#define ejit_addr_f(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_addr_f, u, v, w)
#define ejit_addr_d(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_addr_d, u, v, w)
#define ejit_addi_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_addi_i, u, v, w)
#define ejit_addi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_addi_ui, u, v, w)
#define ejit_addi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_addi_l, u, v, w)
#define ejit_addi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_addi_ul, u, v, w)
#define ejit_addi_p(st, u, v, w)					\
    ejit_ir_ir_p(st, code_addi_p, u, v, w)
#define ejit_addxr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addxr_ui, u, v, w)
#define ejit_addxr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addxr_ui, u, v, w)
#define ejit_addxr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addxr_ul, u, v, w)
#define ejit_addxr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addxr_ul, u, v, w)
#define ejit_addxi_i(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_addxi_ui, u, v, w)
#define ejit_addxi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_addxi_ui, u, v, w)
#define ejit_addxi_l(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_addxi_ul, u, v, w)
#define ejit_addxi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_addxi_ul, u, v, w)
#define ejit_addcr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addcr_ui, u, v, w)
#define ejit_addcr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addcr_ui, u, v, w)
#define ejit_addcr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addcr_ul, u, v, w)
#define ejit_addcr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_addcr_ul, u, v, w)
#define ejit_addci_i(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_addci_ui, u, v, w)
#define ejit_addci_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_addci_ui, u, v, w)
#define ejit_addci_l(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_addci_ul, u, v, w)
#define ejit_addci_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_addci_ul, u, v, w)
#define ejit_subr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subr_i, u, v, w)
#define ejit_subr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subr_ui, u, v, w)
#define ejit_subr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subr_l, u, v, w)
#define ejit_subr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subr_ul, u, v, w)
#define ejit_subr_p(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subr_p, u, v, w)
#define ejit_subr_f(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_subr_f, u, v, w)
#define ejit_subr_d(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_subr_d, u, v, w)
#define ejit_subi_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_subi_i, u, v, w)
#define ejit_subi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_subi_ui, u, v, w)
#define ejit_subi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_subi_l, u, v, w)
#define ejit_subi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_subi_ul, u, v, w)
#define ejit_subi_p(st, u, v, w)					\
    ejit_ir_ir_p(st, code_subi_p, u, v, w)
#define ejit_subxr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subxr_ui, u, v, w)
#define ejit_subxr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subxr_ui, u, v, w)
#define ejit_subxr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subxr_ul, u, v, w)
#define ejit_subxr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subxr_ul, u, v, w)
#define ejit_subxi_i(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_subxi_ui, u, v, w)
#define ejit_subxi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_subxi_ui, u, v, w)
#define ejit_subxi_l(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_subxi_ul, u, v, w)
#define ejit_subxi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_subxi_ul, u, v, w)
#define ejit_subcr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subcr_ui, u, v, w)
#define ejit_subcr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subcr_ui, u, v, w)
#define ejit_subcr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subcr_ul, u, v, w)
#define ejit_subcr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_subcr_ul, u, v, w)
#define ejit_subci_i(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_subci_ui, u, v, w)
#define ejit_subci_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_subci_ui, u, v, w)
#define ejit_subci_l(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_subci_ul, u, v, w)
#define ejit_subci_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_subci_ul, u, v, w)
#define ejit_rsbr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rsbr_i, u, v, w)
#define ejit_rsbr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rsbr_ui, u, v, w)
#define ejit_rsbr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rsbr_l, u, v, w)
#define ejit_rsbr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rsbr_ul, u, v, w)
#define ejit_rsbr_p(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rsbr_p, u, v, w)
#define ejit_rsbr_f(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_rsbr_f, u, v, w)
#define ejit_rsbr_d(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_rsbr_d, u, v, w)
#define ejit_rsbi_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_rsbi_i, u, v, w)
#define ejit_rsbi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_rsbi_ui, u, v, w)
#define ejit_rsbi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_rsbi_l, u, v, w)
#define ejit_rsbi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_rsbi_ul, u, v, w)
#define ejit_rsbi_p(st, u, v, w)					\
    ejit_ir_ir_p(st, code_rsbi_p, u, v, w)
#define ejit_mulr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_mulr_i, u, v, w)
#define ejit_mulr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_mulr_ui, u, v, w)
#define ejit_mulr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_mulr_l, u, v, w)
#define ejit_mulr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_mulr_ul, u, v, w)
#define ejit_mulr_f(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_mulr_f, u, v, w)
#define ejit_mulr_d(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_mulr_d, u, v, w)
#define ejit_muli_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_muli_i, u, v, w)
#define ejit_muli_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_muli_ui, u, v, w)
#define ejit_muli_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_muli_l, u, v, w)
#define ejit_muli_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_muli_ul, u, v, w)
#define ejit_hmulr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_hmulr_i, u, v, w)
#define ejit_hmulr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_hmulr_ui, u, v, w)
#define ejit_hmulr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_hmulr_l, u, v, w)
#define ejit_hmulr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_hmulr_ul, u, v, w)
#define ejit_hmuli_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_hmuli_i, u, v, w)
#define ejit_hmuli_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_hmuli_ui, u, v, w)
#define ejit_hmuli_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_hmuli_l, u, v, w)
#define ejit_hmuli_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_hmuli_ul, u, v, w)
#define ejit_divr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_divr_i, u, v, w)
#define ejit_divr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_divr_ui, u, v, w)
#define ejit_divr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_divr_l, u, v, w)
#define ejit_divr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_divr_ul, u, v, w)
#define ejit_divr_f(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_divr_f, u, v, w)
#define ejit_divr_d(st, u, v, w)					\
    ejit_fr_fr_fr(st, code_divr_d, u, v, w)
#define ejit_divi_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_divi_i, u, v, w)
#define ejit_divi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_divi_ui, u, v, w)
#define ejit_divi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_divi_l, u, v, w)
#define ejit_divi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_divi_ul, u, v, w)
#define ejit_modr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_modr_i, u, v, w)
#define ejit_modr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_modr_ui, u, v, w)
#define ejit_modr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_modr_l, u, v, w)
#define ejit_modr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_modr_ul, u, v, w)
#define ejit_modi_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_modi_i, u, v, w)
#define ejit_modi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_modi_ui, u, v, w)
#define ejit_modi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_modi_l, u, v, w)
#define ejit_modi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_modi_ul, u, v, w)
#define ejit_andr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_andr_i, u, v, w)
#define ejit_andr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_andr_ui, u, v, w)
#define ejit_andr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_andr_l, u, v, w)
#define ejit_andr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_andr_ul, u, v, w)
#define ejit_andi_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_andi_i, u, v, w)
#define ejit_andi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_andi_ui, u, v, w)
#define ejit_andi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_andi_l, u, v, w)
#define ejit_andi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_andi_ul, u, v, w)
#define ejit_orr_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_orr_i, u, v, w)
#define ejit_orr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_orr_ui, u, v, w)
#define ejit_orr_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_orr_l, u, v, w)
#define ejit_orr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_orr_ul, u, v, w)
#define ejit_ori_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_ori_i, u, v, w)
#define ejit_ori_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_ori_ui, u, v, w)
#define ejit_ori_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_ori_l, u, v, w)
#define ejit_ori_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_ori_ul, u, v, w)
#define ejit_xorr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_xorr_i, u, v, w)
#define ejit_xorr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_xorr_ui, u, v, w)
#define ejit_xorr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_xorr_l, u, v, w)
#define ejit_xorr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_xorr_ul, u, v, w)
#define ejit_xori_i(st, u, v, w)					\
    ejit_ir_ir_i(st, code_xori_i, u, v, w)
#define ejit_xori_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_xori_ui, u, v, w)
#define ejit_xori_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_xori_l, u, v, w)
#define ejit_xori_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_xori_ul, u, v, w)
#define ejit_lshr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_lshr_i, u, v, w)
#define ejit_lshr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_lshr_ui, u, v, w)
#define ejit_lshr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_lshr_l, u, v, w)
#define ejit_lshr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_lshr_ul, u, v, w)
#define ejit_lshi_i(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_lshi_i, u, v, w)
#define ejit_lshi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_lshi_ui, u, v, w)
#define ejit_lshi_l(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_lshi_l, u, v, w)
#define ejit_lshi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_lshi_ul, u, v, w)
#define ejit_rshr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rshr_i, u, v, w)
#define ejit_rshr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rshr_ui, u, v, w)
#define ejit_rshr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rshr_l, u, v, w)
#define ejit_rshr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_rshr_ul, u, v, w)
#define ejit_rshi_i(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_rshi_i, u, v, w)
#define ejit_rshi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_rshi_ui, u, v, w)
#define ejit_rshi_l(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_rshi_l, u, v, w)
#define ejit_rshi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_rshi_ul, u, v, w)
#define ejit_absr_f(st, u, v)						\
    ejit_fr_fr(st, code_absr_f, u, v)
#define ejit_absr_d(st, u, v)						\
    ejit_fr_fr(st, code_absr_d, u, v)
#define ejit_negr_i(st, u, v)						\
    ejit_ir_ir(st, code_negr_i, u, v)
#define ejit_negr_l(st, u, v)						\
    ejit_ir_ir(st, code_negr_l, u, v)
#define ejit_negr_f(st, u, v)						\
    ejit_fr_fr(st, code_negr_f, u, v)
#define ejit_negr_d(st, u, v)						\
    ejit_fr_fr(st, code_negr_d, u, v)
#define ejit_notr_i(st, u, v)						\
    ejit_ir_ir(st, code_notr_i, u, v)
#define ejit_notr_l(st, u, v)						\
    ejit_ir_ir(st, code_notr_l, u, v)
#define ejit_ltr_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ltr_i, u, v, w)
#define ejit_ltr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ltr_ui, u, v, w)
#define ejit_ltr_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ltr_l, u, v, w)
#define ejit_ltr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ltr_ul, u, v, w)
#define ejit_ltr_p(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ltr_p, u, v, w)
#define ejit_ltr_f(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ltr_f, u, v, w)
#define ejit_ltr_d(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ltr_d, u, v, w)
#define ejit_lti_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_lti_i, u, v, w)
#define ejit_lti_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_lti_ui, u, v, w)
#define ejit_lti_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_lti_l, u, v, w)
#define ejit_lti_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_lti_ul, u, v, w)
#define ejit_lti_p(st, u, v, w)						\
    ejit_ir_ir_p(st, code_lti_p, u, v, w)
#define ejit_ler_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ler_i, u, v, w)
#define ejit_ler_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ler_ui, u, v, w)
#define ejit_ler_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ler_l, u, v, w)
#define ejit_ler_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ler_ul, u, v, w)
#define ejit_ler_p(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ler_p, u, v, w)
#define ejit_ler_f(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ler_f, u, v, w)
#define ejit_ler_d(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ler_d, u, v, w)
#define ejit_lei_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_lei_i, u, v, w)
#define ejit_lei_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_lei_ui, u, v, w)
#define ejit_lei_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_lei_l, u, v, w)
#define ejit_lei_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_lei_ul, u, v, w)
#define ejit_lei_p(st, u, v, w)						\
    ejit_ir_ir_p(st, code_lei_p, u, v, w)
#define ejit_gtr_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_gtr_i, u, v, w)
#define ejit_gtr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_gtr_ui, u, v, w)
#define ejit_gtr_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_gtr_l, u, v, w)
#define ejit_gtr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_gtr_ul, u, v, w)
#define ejit_gtr_p(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_gtr_p, u, v, w)
#define ejit_gtr_f(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_gtr_f, u, v, w)
#define ejit_gtr_d(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_gtr_d, u, v, w)
#define ejit_gti_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_gti_i, u, v, w)
#define ejit_gti_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_gti_ui, u, v, w)
#define ejit_gti_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_gti_l, u, v, w)
#define ejit_gti_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_gti_ul, u, v, w)
#define ejit_gti_p(st, u, v, w)						\
    ejit_ir_ir_p(st, code_gti_p, u, v, w)
#define ejit_ger_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ger_i, u, v, w)
#define ejit_ger_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ger_ui, u, v, w)
#define ejit_ger_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ger_l, u, v, w)
#define ejit_ger_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ger_ul, u, v, w)
#define ejit_ger_p(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ger_p, u, v, w)
#define ejit_ger_f(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ger_f, u, v, w)
#define ejit_ger_d(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ger_d, u, v, w)
#define ejit_gei_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_gei_i, u, v, w)
#define ejit_gei_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_gei_ui, u, v, w)
#define ejit_gei_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_gei_l, u, v, w)
#define ejit_gei_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_gei_ul, u, v, w)
#define ejit_gei_p(st, u, v, w)						\
    ejit_ir_ir_p(st, code_gei_p, u, v, w)
#define ejit_eqr_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_eqr_i, u, v, w)
#define ejit_eqr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_eqr_ui, u, v, w)
#define ejit_eqr_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_eqr_l, u, v, w)
#define ejit_eqr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_eqr_ul, u, v, w)
#define ejit_eqr_p(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_eqr_p, u, v, w)
#define ejit_eqr_f(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_eqr_f, u, v, w)
#define ejit_eqr_d(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_eqr_d, u, v, w)
#define ejit_eqi_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_eqi_i, u, v, w)
#define ejit_eqi_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_eqi_ui, u, v, w)
#define ejit_eqi_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_eqi_l, u, v, w)
#define ejit_eqi_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_eqi_ul, u, v, w)
#define ejit_eqi_p(st, u, v, w)						\
    ejit_ir_ir_p(st, code_eqi_p, u, v, w)
#define ejit_ner_i(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ner_i, u, v, w)
#define ejit_ner_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ner_ui, u, v, w)
#define ejit_ner_l(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ner_l, u, v, w)
#define ejit_ner_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ner_ul, u, v, w)
#define ejit_ner_p(st, u, v, w)						\
    ejit_ir_ir_ir(st, code_ner_p, u, v, w)
#define ejit_ner_f(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ner_f, u, v, w)
#define ejit_ner_d(st, u, v, w)						\
    ejit_ir_fr_fr(st, code_ner_d, u, v, w)
#define ejit_nei_i(st, u, v, w)						\
    ejit_ir_ir_i(st, code_nei_i, u, v, w)
#define ejit_nei_ui(st, u, v, w)					\
    ejit_ir_ir_ui(st, code_nei_ui, u, v, w)
#define ejit_nei_l(st, u, v, w)						\
    ejit_ir_ir_l(st, code_nei_l, u, v, w)
#define ejit_nei_ul(st, u, v, w)					\
    ejit_ir_ir_ul(st, code_nei_ul, u, v, w)
#define ejit_nei_p(st, u, v, w)						\
    ejit_ir_ir_p(st, code_nei_p, u, v, w)
#define ejit_unltr_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unltr_f, u, v, w)
#define ejit_unltr_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unltr_d, u, v, w)
#define ejit_unler_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unler_f, u, v, w)
#define ejit_unler_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unler_d, u, v, w)
#define ejit_ungtr_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_ungtr_f, u, v, w)
#define ejit_ungtr_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_ungtr_d, u, v, w)
#define ejit_unger_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unger_f, u, v, w)
#define ejit_unger_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unger_d, u, v, w)
#define ejit_uneqr_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_uneqr_f, u, v, w)
#define ejit_uneqr_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_uneqr_d, u, v, w)
#define ejit_ltgtr_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_ltgtr_f, u, v, w)
#define ejit_ltgtr_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_ltgtr_d, u, v, w)
#define ejit_ordr_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_ordr_f, u, v, w)
#define ejit_ordr_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_ordr_d, u, v, w)
#define ejit_unordr_f(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unordr_f, u, v, w)
#define ejit_unordr_d(st, u, v, w)					\
    ejit_ir_fr_fr(st, code_unordr_d, u, v, w)
#define ejit_movr_i(st, u, v)						\
    ejit_ir_ir(st, code_movr_i, u, v)
#define ejit_movr_ui(st, u, v)						\
    ejit_ir_ir(st, code_movr_ui, u, v)
#define ejit_movr_l(st, u, v)						\
    ejit_ir_ir(st, code_movr_l, u, v)
#define ejit_movr_ul(st, u, v)						\
    ejit_ir_ir(st, code_movr_ul, u, v)
#define ejit_movr_p(st, u, v)						\
    ejit_ir_ir(st, code_movr_p, u, v)
#define ejit_movr_f(st, u, v)						\
    ejit_fr_fr(st, code_movr_f, u, v)
#define ejit_movr_d(st, u, v)						\
    ejit_fr_fr(st, code_movr_d, u, v)
#define ejit_movi_i(st, u, v)						\
    ejit_ir_i(st, code_movi_i, u, v)
#define ejit_movi_ui(st, u, v)						\
    ejit_ir_ui(st, code_movi_ui, u, v)
#define ejit_movi_l(st, u, v)						\
    ejit_ir_l(st, code_movi_l, u, v)
#define ejit_movi_ul(st, u, v)						\
    ejit_ir_ul(st, code_movi_ul, u, v)
#define ejit_movi_p(st, u, v)						\
    ejit_ir_p(st, code_movi_p, u, v)
#define ejit_movi_f(st, u, v)						\
    ejit_ir_f(st, code_movi_f, u, v)
#define ejit_movi_d(st, u, v)						\
    ejit_ir_d(st, code_movi_d, u, v)
#define ejit_extr_c_i(st, u, v)						\
    ejit_ir_ir(st, code_extr_c_i, u, v)
#define ejit_extr_uc_ui(st, u, v)					\
    ejit_ir_ir(st, code_extr_uc_ui, u, v)
#define ejit_extr_s_i(st, u, v)						\
    ejit_ir_ir(st, code_extr_s_i, u, v)
#define ejit_extr_us_ui(st, u, v)					\
    ejit_ir_ir(st, code_extr_us_ui, u, v)
#define ejit_extr_c_l(st, u, v)						\
    ejit_ir_ir(st, code_extr_c_l, u, v)
#define ejit_extr_uc_ul(st, u, v)					\
    ejit_ir_ir(st, code_extr_uc_ul, u, v)
#define ejit_extr_s_l(st, u, v)						\
    ejit_ir_ir(st, code_extr_s_l, u, v)
#define ejit_extr_us_ul(st, u, v)					\
    ejit_ir_ir(st, code_extr_us_ul, u, v)
#define ejit_extr_i_l(st, u, v)						\
    ejit_ir_ir(st, code_extr_i_l, u, v)
#define ejit_extr_ui_ul(st, u, v)					\
    ejit_ir_ir(st, code_extr_ui_ul, u, v)
#define ejit_extr_i_f(st, u, v)						\
    ejit_fr_ir(st, code_extr_i_f, u, v)
#define ejit_extr_i_d(st, u, v)						\
    ejit_fr_ir(st, code_extr_i_d, u, v)
#define ejit_extr_l_f(st, u, v)						\
    ejit_fr_ir(st, code_extr_l_f, u, v)
#define ejit_extr_l_d(st, u, v)						\
    ejit_fr_ir(st, code_extr_l_d, u, v)
#define ejit_extr_f_d(st, u, v)						\
    ejit_fr_fr(st, code_extr_f_d, u, v)
#define ejit_extr_d_f(st, u, v)						\
    ejit_fr_fr(st, code_extr_d_f, u, v)

#define ejit_movr_i_f(st, u, v)						\
    ejit_fr_ir(st, code_movr_i_f, u, v)
#define ejit_movr_l_d(st, u, v)						\
    ejit_fr_ir(st, code_movr_l_d, u, v)
#define ejit_movr_f_i(st, u, v)						\
    ejit_ir_fr(st, code_movr_f_i, u, v)
#define ejit_movr_d_l(st, u, v)						\
    ejit_ir_fr(st, code_movr_d_l, u, v)

#define ejit_roundr_f_i(st, u, v)					\
    ejit_ir_fr(st, code_roundr_f_i, u, v)
#define ejit_roundr_f_l(st, u, v)					\
    ejit_ir_fr(st, code_roundr_f_l, u, v)
#define ejit_roundr_d_i(st, u, v)					\
    ejit_ir_fr(st, code_roundr_d_i, u, v)
#define ejit_roundr_d_l(st, u, v)					\
    ejit_ir_fr(st, code_roundr_d_l, u, v)
#define ejit_truncr_f_i(st, u, v)					\
    ejit_ir_fr(st, code_truncr_f_i, u, v)
#define ejit_truncr_f_l(st, u, v)					\
    ejit_ir_fr(st, code_truncr_f_l, u, v)
#define ejit_truncr_d_i(st, u, v)					\
    ejit_ir_fr(st, code_truncr_d_i, u, v)
#define ejit_truncr_d_l(st, u, v)					\
    ejit_ir_fr(st, code_truncr_d_l, u, v)
#define ejit_floorr_f_i(st, u, v)					\
    ejit_ir_fr(st, code_floorr_f_i, u, v)
#define ejit_floorr_f_l(st, u, v)					\
    ejit_ir_fr(st, code_floorr_f_l, u, v)
#define ejit_floorr_d_i(st, u, v)					\
    ejit_ir_fr(st, code_floorr_d_i, u, v)
#define ejit_floorr_d_l(st, u, v)					\
    ejit_ir_fr(st, code_floorr_d_l, u, v)
#define ejit_ceilr_f_i(st, u, v)					\
    ejit_ir_fr(st, code_ceilr_f_i, u, v)
#define ejit_ceilr_f_l(st, u, v)					\
    ejit_ir_fr(st, code_ceilr_f_l, u, v)
#define ejit_ceilr_d_i(st, u, v)					\
    ejit_ir_fr(st, code_ceilr_d_i, u, v)
#define ejit_ceilr_d_l(st, u, v)					\
    ejit_ir_fr(st, code_ceilr_d_l, u, v)
#define ejit_hton_us_ui(st, u, v)					\
    ejit_ir_ir(st, code_hton_ui_ui, u, v)
#define ejit_ntoh_us_ui(st, u, v)					\
    ejit_ir_ir(st, code_ntoh_ui_ui, u, v)
#define ejit_ldr_c(st, u, v)						\
    ejit_ir_ir(st, code_ldr_c, u, v)
#define ejit_ldr_uc(st, u, v)						\
    ejit_ir_ir(st, code_ldr_uc, u, v)
#define ejit_ldr_s(st, u, v)						\
    ejit_ir_ir(st, code_ldr_s, u, v)
#define ejit_ldr_us(st, u, v)						\
    ejit_ir_ir(st, code_ldr_us, u, v)
#define ejit_ldr_i(st, u, v)						\
    ejit_ir_ir(st, code_ldr_i, u, v)
#define ejit_ldr_ui(st, u, v)						\
    ejit_ir_ir(st, code_ldr_ui, u, v)
#define ejit_ldr_l(st, u, v)						\
    ejit_ir_ir(st, code_ldr_l, u, v)
#define ejit_ldr_ul(st, u, v)						\
    ejit_ir_ir(st, code_ldr_ul, u, v)
#define ejit_ldr_p(st, u, v)						\
    ejit_ir_ir(st, code_ldr_p, u, v)
#define ejit_ldr_f(st, u, v)						\
    ejit_fr_ir(st, code_ldr_f, u, v)
#define ejit_ldr_d(st, u, v)						\
    ejit_fr_ir(st, code_ldr_d, u, v)
#define ejit_ldi_c(st, u, v)						\
    ejit_ir_p(st, code_ldi_c, u, v)
#define ejit_ldi_uc(st, u, v)						\
    ejit_ir_p(st, code_ldi_uc, u, v)
#define ejit_ldi_s(st, u, v)						\
    ejit_ir_p(st, code_ldi_s, u, v)
#define ejit_ldi_us(st, u, v)						\
    ejit_ir_p(st, code_ldi_us, u, v)
#define ejit_ldi_i(st, u, v)						\
    ejit_ir_p(st, code_ldi_i, u, v)
#define ejit_ldi_ui(st, u, v)						\
    ejit_ir_p(st, code_ldi_ui, u, v)
#define ejit_ldi_l(st, u, v)						\
    ejit_ir_p(st, code_ldi_l, u, v)
#define ejit_ldi_ul(st, u, v)						\
    ejit_ir_p(st, code_ldi_ul, u, v)
#define ejit_ldi_p(st, u, v)						\
    ejit_ir_p(st, code_ldi_p, u, v)
#define ejit_ldi_f(st, u, v)						\
    ejit_fr_p(st, code_ldi_f, u, v)
#define ejit_ldi_d(st, u, v)						\
    ejit_fr_p(st, code_ldi_d, u, v)
#define ejit_ldxr_c(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_c, u, v, w)
#define ejit_ldxr_uc(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_uc, u, v, w)
#define ejit_ldxr_s(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_s, u, v, w)
#define ejit_ldxr_us(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_us, u, v, w)
#define ejit_ldxr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_i, u, v, w)
#define ejit_ldxr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_ui, u, v, w)
#define ejit_ldxr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_l, u, v, w)
#define ejit_ldxr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_ul, u, v, w)
#define ejit_ldxr_p(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_ldxr_p, u, v, w)
#define ejit_ldxr_f(st, u, v, w)					\
    ejit_fr_ir_ir(st, code_ldxr_f, u, v, w)
#define ejit_ldxr_d(st, u, v, w)					\
    ejit_fr_ir_ir(st, code_ldxr_d, u, v, w)
#define ejit_ldxi_c(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_c, u, v, w)
#define ejit_ldxi_uc(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_uc, u, v, w)
#define ejit_ldxi_s(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_s, u, v, w)
#define ejit_ldxi_us(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_us, u, v, w)
#define ejit_ldxi_i(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_i, u, v, w)
#define ejit_ldxi_ui(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_ui, u, v, w)
#define ejit_ldxi_l(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_l, u, v, w)
#define ejit_ldxi_ul(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_ul, u, v, w)
#define ejit_ldxi_p(st, u, v, w)					\
    ejit_ir_ir_l(st, code_ldxi_p, u, v, w)
#define ejit_ldxi_f(st, u, v, w)					\
    ejit_fr_ir_l(st, code_ldxi_f, u, v, w)
#define ejit_ldxi_d(st, u, v, w)					\
    ejit_fr_ir_l(st, code_ldxi_d, u, v, w)
#define ejit_str_c(st, u, v)						\
    ejit_ir_ir(st, code_str_c, u, v)
#define ejit_str_uc(st, u, v)						\
    ejit_ir_ir(st, code_str_uc, u, v)
#define ejit_str_s(st, u, v)						\
    ejit_ir_ir(st, code_str_s, u, v)
#define ejit_str_us(st, u, v)						\
    ejit_ir_ir(st, code_str_us, u, v)
#define ejit_str_i(st, u, v)						\
    ejit_ir_ir(st, code_str_i, u, v)
#define ejit_str_ui(st, u, v)						\
    ejit_ir_ir(st, code_str_ui, u, v)
#define ejit_str_l(st, u, v)						\
    ejit_ir_ir(st, code_str_l, u, v)
#define ejit_str_ul(st, u, v)						\
    ejit_ir_ir(st, code_str_ul, u, v)
#define ejit_str_p(st, u, v)						\
    ejit_ir_ir(st, code_str_p, u, v)
#define ejit_str_f(st, u, v)						\
    ejit_ir_fr(st, code_str_f, u, v)
#define ejit_str_d(st, u, v)						\
    ejit_ir_fr(st, code_str_d, u, v)
#define ejit_sti_c(st, u, v)						\
    ejit_p_ir(st, code_sti_c, u, v)
#define ejit_sti_uc(st, u, v)						\
    ejit_p_ir(st, code_sti_uc, u, v)
#define ejit_sti_s(st, u, v)						\
    ejit_p_ir(st, code_sti_s, u, v)
#define ejit_sti_us(st, u, v)						\
    ejit_p_ir(st, code_sti_us, u, v)
#define ejit_sti_i(st, u, v)						\
    ejit_p_ir(st, code_sti_i, u, v)
#define ejit_sti_ui(st, u, v)						\
    ejit_p_ir(st, code_sti_ui, u, v)
#define ejit_sti_l(st, u, v)						\
    ejit_p_ir(st, code_sti_l, u, v)
#define ejit_sti_ul(st, u, v)						\
    ejit_p_ir(st, code_sti_ul, u, v)
#define ejit_sti_p(st, u, v)						\
    ejit_p_ir(st, code_sti_p, u, v)
#define ejit_sti_f(st, u, v)						\
    ejit_p_fr(st, code_sti_f, u, v)
#define ejit_sti_d(st, u, v)						\
    ejit_p_fr(st, code_sti_d, u, v)
#define ejit_stxr_c(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_c, u, v, w)
#define ejit_stxr_uc(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_uc, u, v, w)
#define ejit_stxr_s(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_s, u, v, w)
#define ejit_stxr_us(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_us, u, v, w)
#define ejit_stxr_i(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_i, u, v, w)
#define ejit_stxr_ui(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_ui, u, v, w)
#define ejit_stxr_l(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_l, u, v, w)
#define ejit_stxr_ul(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_ul, u, v, w)
#define ejit_stxr_p(st, u, v, w)					\
    ejit_ir_ir_ir(st, code_stxr_p, u, v, w)
#define ejit_stxr_f(st, u, v, w)					\
    ejit_ir_ir_fr(st, code_stxr_f, u, v, w)
#define ejit_stxr_d(st, u, v, w)					\
    ejit_ir_ir_fr(st, code_stxr_d, u, v, w)
#define ejit_stxi_c(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_c, u, v, w)
#define ejit_stxi_uc(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_uc, u, v, w)
#define ejit_stxi_s(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_s, u, v, w)
#define ejit_stxi_us(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_us, u, v, w)
#define ejit_stxi_i(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_i, u, v, w)
#define ejit_stxi_ui(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_ui, u, v, w)
#define ejit_stxi_l(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_l, u, v, w)
#define ejit_stxi_ul(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_ul, u, v, w)
#define ejit_stxi_p(st, u, v, w)					\
    ejit_l_ir_ir(st, code_stxi_p, u, v, w)
#define ejit_stxi_f(st, u, v, w)					\
    ejit_l_ir_fr(st, code_stxi_f, u, v, w)
#define ejit_stxi_d(st, u, v, w)					\
    ejit_l_ir_fr(st, code_stxi_d, u, v, w)
#define ejit_prepare_i(st, u)						\
    ejit_i(st, code_prepare_i, u)
#define ejit_prepare_f(st, u)						\
    ejit_i(st, code_prepare_f, u)
#define ejit_prepare_d(st, u)						\
    ejit_i(st, code_prepare_d, u)
#define ejit_pusharg_c(st, u)						\
    ejit_ir(st, code_pusharg_c, u)
#define ejit_pusharg_uc(st, u)						\
    ejit_ir(st, code_pusharg_uc, u)
#define ejit_pusharg_s(st, u)						\
    ejit_ir(st, code_pusharg_s, u)
#define ejit_pusharg_us(st, u)						\
    ejit_ir(st, code_pusharg_us, u)
#define ejit_pusharg_i(st, u)						\
    ejit_ir(st, code_pusharg_i, u)
#define ejit_pusharg_ui(st, u)						\
    ejit_ir(st, code_pusharg_ui, u)
#define ejit_pusharg_l(st, u)						\
    ejit_ir(st, code_pusharg_l, u)
#define ejit_pusharg_ul(st, u)						\
    ejit_ir(st, code_pusharg_ul, u)
#define ejit_pusharg_p(st, u)						\
    ejit_ir(st, code_pusharg_p, u)
#define ejit_pusharg_f(st, u)						\
    ejit_fr(st, code_pusharg_f, u)
#define ejit_pusharg_d(st, u)						\
    ejit_fr(st, code_pusharg_d, u)
#define ejit_getarg_c(st, u, v)						\
    ejit_ir_n(st, code_getarg_c, u, v)
#define ejit_getarg_uc(st, u, v)					\
    ejit_ir_n(st, code_getarg_uc, u, v)
#define ejit_getarg_s(st, u, v)						\
    ejit_ir_n(st, code_getarg_s, u, v)
#define ejit_getarg_us(st, u, v)					\
    ejit_ir_n(st, code_getarg_us, u, v)
#define ejit_getarg_i(st, u, v)						\
    ejit_ir_n(st, code_getarg_i, u, v)
#define ejit_getarg_ui(st, u, v)					\
    ejit_ir_n(st, code_getarg_ui, u, v)
#define ejit_getarg_l(st, u, v)						\
    ejit_ir_n(st, code_getarg_l, u, v)
#define ejit_getarg_ul(st, u, v)					\
    ejit_ir_n(st, code_getarg_ul, u, v)
#define ejit_getarg_p(st, u, v)						\
    ejit_ir_n(st, code_getarg_p, u, v)
#define ejit_getarg_f(st, u, v)						\
    ejit_fr_n(st, code_getarg_f, u, v)
#define ejit_getarg_d(st, u, v)						\
    ejit_fr_n(st, code_getarg_d, u, v)
#define ejit_arg_c(st)							\
    ejit(st, code_arg_c)
#define ejit_arg_uc(st)							\
    ejit(st, code_arg_uc)
#define ejit_arg_s(st)							\
    ejit(st, code_arg_s)
#define ejit_arg_us(st)							\
    ejit(st, code_arg_us)
#define ejit_arg_i(st)							\
    ejit(st, code_arg_i)
#define ejit_arg_ui(st)							\
    ejit(st, code_arg_ui)
#define ejit_arg_l(st)							\
    ejit(st, code_arg_l)
#define ejit_arg_ul(st)							\
    ejit(st, code_arg_ul)
#define ejit_arg_p(st)							\
    ejit(st, code_arg_p)
#define ejit_arg_f(st)							\
    ejit(st, code_arg_f)
#define ejit_arg_d(st)							\
    ejit(st, code_arg_d)
#define ejit_retval_c(st, u)						\
    ejit_ir(st, code_retval_c, u)
#define ejit_retval_uc(st, u)						\
    ejit_ir(st, code_retval_uc, u)
#define ejit_retval_s(st, u)						\
    ejit_ir(st, code_retval_s, u)
#define ejit_retval_us(st, u)						\
    ejit_ir(st, code_retval_us, u)
#define ejit_retval_i(st, u)						\
    ejit_ir(st, code_retval_i, u)
#define ejit_retval_ui(st, u)						\
    ejit_ir(st, code_retval_ui, u)
#define ejit_retval_l(st, u)						\
    ejit_ir(st, code_retval_l, u)
#define ejit_retval_ul(st, u)						\
    ejit_ir(st, code_retval_ul, u)
#define ejit_retval_p(st, u)						\
    ejit_ir(st, code_retval_p, u)
#define ejit_retval_f(st, u)						\
    ejit_fr(st, code_retval_f, u)
#define ejit_retval_d(st, u)						\
    ejit_fr(st, code_retval_d, u)
#define ejit_bltr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bltr_i, u, v, w)
#define ejit_bltr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bltr_ui, u, v, w)
#define ejit_bltr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bltr_l, u, v, w)
#define ejit_bltr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bltr_ul, u, v, w)
#define ejit_bltr_p(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bltr_p, u, v, w)
#define ejit_bltr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bltr_f, u, v, w)
#define ejit_bltr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bltr_d, u, v, w)
#define ejit_blti_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_blti_i, u, v, w)
#define ejit_blti_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_blti_ui, u, v, w)
#define ejit_blti_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_blti_l, u, v, w)
#define ejit_blti_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_blti_ul, u, v, w)
#define ejit_blti_p(st, u, v, w)					\
    ejit_n_ir_p(st, code_blti_p, u, v, w)
#define ejit_bler_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bler_i, u, v, w)
#define ejit_bler_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bler_ui, u, v, w)
#define ejit_bler_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bler_l, u, v, w)
#define ejit_bler_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bler_ul, u, v, w)
#define ejit_bler_p(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bler_p, u, v, w)
#define ejit_bler_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bler_f, u, v, w)
#define ejit_bler_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bler_d, u, v, w)
#define ejit_blei_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_blei_i, u, v, w)
#define ejit_blei_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_blei_ui, u, v, w)
#define ejit_blei_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_blei_l, u, v, w)
#define ejit_blei_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_blei_ul, u, v, w)
#define ejit_blei_p(st, u, v, w)					\
    ejit_n_ir_p(st, code_blei_p, u, v, w)
#define ejit_bgtr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bgtr_i, u, v, w)
#define ejit_bgtr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bgtr_ui, u, v, w)
#define ejit_bgtr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bgtr_l, u, v, w)
#define ejit_bgtr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bgtr_ul, u, v, w)
#define ejit_bgtr_p(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bgtr_p, u, v, w)
#define ejit_bgtr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bgtr_f, u, v, w)
#define ejit_bgtr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bgtr_d, u, v, w)
#define ejit_bgti_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_bgti_i, u, v, w)
#define ejit_bgti_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_bgti_ui, u, v, w)
#define ejit_bgti_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_bgti_l, u, v, w)
#define ejit_bgti_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_bgti_ul, u, v, w)
#define ejit_bgti_p(st, u, v, w)					\
    ejit_n_ir_p(st, code_bgti_p, u, v, w)
#define ejit_bger_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bger_i, u, v, w)
#define ejit_bger_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bger_ui, u, v, w)
#define ejit_bger_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bger_l, u, v, w)
#define ejit_bger_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bger_ul, u, v, w)
#define ejit_bger_p(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bger_p, u, v, w)
#define ejit_bger_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bger_f, u, v, w)
#define ejit_bger_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bger_d, u, v, w)
#define ejit_bgei_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_bgei_i, u, v, w)
#define ejit_bgei_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_bgei_ui, u, v, w)
#define ejit_bgei_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_bgei_l, u, v, w)
#define ejit_bgei_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_bgei_ul, u, v, w)
#define ejit_bgei_p(st, u, v, w)					\
    ejit_n_ir_p(st, code_bgei_p, u, v, w)
#define ejit_beqr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_beqr_i, u, v, w)
#define ejit_beqr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_beqr_ui, u, v, w)
#define ejit_beqr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_beqr_l, u, v, w)
#define ejit_beqr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_beqr_ul, u, v, w)
#define ejit_beqr_p(st, u, v, w)					\
    ejit_n_ir_ir(st, code_beqr_p, u, v, w)
#define ejit_beqr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_beqr_f, u, v, w)
#define ejit_beqr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_beqr_d, u, v, w)
#define ejit_beqi_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_beqi_i, u, v, w)
#define ejit_beqi_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_beqi_ui, u, v, w)
#define ejit_beqi_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_beqi_l, u, v, w)
#define ejit_beqi_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_beqi_ul, u, v, w)
#define ejit_beqi_p(st, u, v, w)					\
    ejit_n_ir_p(st, code_beqi_p, u, v, w)
#define ejit_bner_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bner_i, u, v, w)
#define ejit_bner_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bner_ui, u, v, w)
#define ejit_bner_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bner_l, u, v, w)
#define ejit_bner_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bner_ul, u, v, w)
#define ejit_bner_p(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bner_p, u, v, w)
#define ejit_bner_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bner_f, u, v, w)
#define ejit_bner_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bner_d, u, v, w)
#define ejit_bnei_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_bnei_i, u, v, w)
#define ejit_bnei_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_bnei_ui, u, v, w)
#define ejit_bnei_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_bnei_l, u, v, w)
#define ejit_bnei_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_bnei_ul, u, v, w)
#define ejit_bnei_p(st, u, v, w)					\
    ejit_n_ir_p(st, code_bnei_p, u, v, w)
#define ejit_bunltr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunltr_f, u, v, w)
#define ejit_bunltr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunltr_d, u, v, w)
#define ejit_bunler_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunler_f, u, v, w)
#define ejit_bunler_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunler_d, u, v, w)
#define ejit_bungtr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bungtr_f, u, v, w)
#define ejit_bungtr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bungtr_d, u, v, w)
#define ejit_bunger_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunger_f, u, v, w)
#define ejit_bunger_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunger_d, u, v, w)
#define ejit_buneqr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_buneqr_f, u, v, w)
#define ejit_buneqr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_buneqr_d, u, v, w)
#define ejit_bltgtr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bltgtr_f, u, v, w)
#define ejit_bltgtr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bltgtr_d, u, v, w)
#define ejit_bordr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bordr_f, u, v, w)
#define ejit_bordr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bordr_d, u, v, w)
#define ejit_bunordr_f(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunordr_f, u, v, w)
#define ejit_bunordr_d(st, u, v, w)					\
    ejit_n_fr_fr(st, code_bunordr_d, u, v, w)
#define ejit_bmcr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmcr_i, u, v, w)
#define ejit_bmcr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmcr_ui, u, v, w)
#define ejit_bmcr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmcr_l, u, v, w)
#define ejit_bmcr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmcr_ul, u, v, w)
#define ejit_bmci_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmci_i, u, v, w)
#define ejit_bmci_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_bmci_ui, u, v, w)
#define ejit_bmci_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_bmci_l, u, v, w)
#define ejit_bmci_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_bmci_ul, u, v, w)
#define ejit_bmsr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmsr_i, u, v, w)
#define ejit_bmsr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmsr_ui, u, v, w)
#define ejit_bmsr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmsr_l, u, v, w)
#define ejit_bmsr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bmsr_ul, u, v, w)
#define ejit_bmsi_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_bmsi_i, u, v, w)
#define ejit_bmsi_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_bmsi_ui, u, v, w)
#define ejit_bmsi_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_bmsi_l, u, v, w)
#define ejit_bmsi_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_bmsi_ul, u, v, w)
#define ejit_boaddr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_boaddr_i, u, v, w)
#define ejit_boaddr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_boaddr_ui, u, v, w)
#define ejit_boaddr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_boaddr_l, u, v, w)
#define ejit_boaddr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_boaddr_ul, u, v, w)
#define ejit_boaddi_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_boaddi_i, u, v, w)
#define ejit_boaddi_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_boaddi_ui, u, v, w)
#define ejit_boaddi_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_boaddi_l, u, v, w)
#define ejit_boaddi_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_boaddi_ul, u, v, w)
#define ejit_bosubr_i(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bosubr_i, u, v, w)
#define ejit_bosubr_ui(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bosubr_ui, u, v, w)
#define ejit_bosubr_l(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bosubr_l, u, v, w)
#define ejit_bosubr_ul(st, u, v, w)					\
    ejit_n_ir_ir(st, code_bosubr_ul, u, v, w)
#define ejit_bosubi_i(st, u, v, w)					\
    ejit_n_ir_i(st, code_bosubi_i, u, v, w)
#define ejit_bosubi_ui(st, u, v, w)					\
    ejit_n_ir_ui(st, code_bosubi_ui, u, v, w)
#define ejit_bosubi_l(st, u, v, w)					\
    ejit_n_ir_l(st, code_bosubi_l, u, v, w)
#define ejit_bosubi_ul(st, u, v, w)					\
    ejit_n_ir_ul(st, code_bosubi_ul, u, v, w)
#define ejit_calli(st, u)						\
    ejit_p(st, code_calli, u)
#define ejit_callr(st, u)						\
    ejit_ir(st, code_callr, u)
#define ejit_finish(st, u)						\
    ejit_p(st, code_finish, u)
#define ejit_finishr(st, u)						\
    ejit_ir(st, code_finishr, u)
#define ejit_jmpi(st, u)						\
    ejit_p(st, code_jmpi, u)
#define ejit_jmpr(st, u)						\
    ejit_ir(st, code_jmpr, u)
#define ejit_ret(st)							\
    ejit(st, code_ret)
#define ejit_leave(st)							\
    ejit(st, code_leave)
#define ejit_prolog(st, u)						\
    ejit_i(st, code_prolog, u)
#define ejit_prolog_f(st, u)						\
    ejit_i(st, code_prolog_f, u)
#define ejit_prolog_d(st, u)						\
    ejit_i(st, code_prolog_d, u)
#define ejit_leaf(st, u)						\
    ejit_i(st, code_leaf, u)
#define ejit_allocai(st, u)						\
    ejit_i(st, code_allocai, u)

/*
 * Types
 */
typedef struct ejit_node	ejit_node_t;
typedef union ejit_data		ejit_data_t;
typedef struct ejit_state	ejit_state_t;

typedef enum {
    code_note,
    code_label,
    code_addr_i,
    code_addr_ui,
    code_addr_l,
    code_addr_ul,
    code_addr_p,
    code_addr_f,
    code_addr_d,
    code_addi_i,
    code_addi_ui,
    code_addi_l,
    code_addi_ul,
    code_addi_p,
    code_addxr_ui,
    code_addxr_ul,
    code_addxi_ui,
    code_addxi_ul,
    code_addcr_ui,
    code_addcr_ul,
    code_addci_ui,
    code_addci_ul,
    code_subr_i,
    code_subr_ui,
    code_subr_l,
    code_subr_ul,
    code_subr_p,
    code_subr_f,
    code_subr_d,
    code_subi_i,
    code_subi_ui,
    code_subi_l,
    code_subi_ul,
    code_subi_p,
    code_subxr_ui,
    code_subxr_ul,
    code_subxi_ui,
    code_subxi_ul,
    code_subcr_ui,
    code_subcr_ul,
    code_subci_ui,
    code_subci_ul,
    code_rsbr_i,
    code_rsbr_ui,
    code_rsbr_l,
    code_rsbr_ul,
    code_rsbr_p,
    code_rsbr_f,
    code_rsbr_d,
    code_rsbi_i,
    code_rsbi_ui,
    code_rsbi_l,
    code_rsbi_ul,
    code_rsbi_p,
    code_mulr_i,
    code_mulr_ui,
    code_mulr_l,
    code_mulr_ul,
    code_mulr_f,
    code_mulr_d,
    code_muli_i,
    code_muli_ui,
    code_muli_l,
    code_muli_ul,
    code_hmulr_i,
    code_hmulr_ui,
    code_hmulr_l,
    code_hmulr_ul,
    code_hmuli_i,
    code_hmuli_ui,
    code_hmuli_l,
    code_hmuli_ul,
    code_divr_i,
    code_divr_ui,
    code_divr_l,
    code_divr_ul,
    code_divr_f,
    code_divr_d,
    code_divi_i,
    code_divi_ui,
    code_divi_l,
    code_divi_ul,
    code_modr_i,
    code_modr_ui,
    code_modr_l,
    code_modr_ul,
    code_modi_i,
    code_modi_ui,
    code_modi_l,
    code_modi_ul,
    code_andr_i,
    code_andr_ui,
    code_andr_l,
    code_andr_ul,
    code_andi_i,
    code_andi_ui,
    code_andi_l,
    code_andi_ul,
    code_orr_i,
    code_orr_ui,
    code_orr_l,
    code_orr_ul,
    code_ori_i,
    code_ori_ui,
    code_ori_l,
    code_ori_ul,
    code_xorr_i,
    code_xorr_ui,
    code_xorr_l,
    code_xorr_ul,
    code_xori_i,
    code_xori_ui,
    code_xori_l,
    code_xori_ul,
    code_lshr_i,
    code_lshr_ui,
    code_lshr_l,
    code_lshr_ul,
    code_lshi_i,
    code_lshi_ui,
    code_lshi_l,
    code_lshi_ul,
    code_rshr_i,
    code_rshr_ui,
    code_rshr_l,
    code_rshr_ul,
    code_rshi_i,
    code_rshi_ui,
    code_rshi_l,
    code_rshi_ul,
    code_absr_f,
    code_absr_d,
    code_negr_i,
    code_negr_l,
    code_negr_f,
    code_negr_d,
    code_notr_i,
    code_notr_l,
    code_ltr_i,
    code_ltr_ui,
    code_ltr_l,
    code_ltr_ul,
    code_ltr_p,
    code_ltr_f,
    code_ltr_d,
    code_lti_i,
    code_lti_ui,
    code_lti_l,
    code_lti_ul,
    code_lti_p,
    code_ler_i,
    code_ler_ui,
    code_ler_l,
    code_ler_ul,
    code_ler_p,
    code_ler_f,
    code_ler_d,
    code_lei_i,
    code_lei_ui,
    code_lei_l,
    code_lei_ul,
    code_lei_p,
    code_eqr_i,
    code_eqr_ui,
    code_eqr_l,
    code_eqr_ul,
    code_eqr_p,
    code_eqr_f,
    code_eqr_d,
    code_eqi_i,
    code_eqi_ui,
    code_eqi_l,
    code_eqi_ul,
    code_eqi_p,
    code_ger_i,
    code_ger_ui,
    code_ger_l,
    code_ger_ul,
    code_ger_p,
    code_ger_f,
    code_ger_d,
    code_gei_i,
    code_gei_ui,
    code_gei_l,
    code_gei_ul,
    code_gei_p,
    code_gtr_i,
    code_gtr_ui,
    code_gtr_l,
    code_gtr_ul,
    code_gtr_p,
    code_gtr_f,
    code_gtr_d,
    code_gti_i,
    code_gti_ui,
    code_gti_l,
    code_gti_ul,
    code_gti_p,
    code_ner_i,
    code_ner_ui,
    code_ner_l,
    code_ner_ul,
    code_ner_p,
    code_ner_f,
    code_ner_d,
    code_nei_i,
    code_nei_ui,
    code_nei_l,
    code_nei_ul,
    code_nei_p,
    code_unltr_f,
    code_unltr_d,
    code_unler_f,
    code_unler_d,
    code_uneqr_f,
    code_uneqr_d,
    code_unger_f,
    code_unger_d,
    code_ungtr_f,
    code_ungtr_d,
    code_ltgtr_f,
    code_ltgtr_d,
    code_ordr_f,
    code_ordr_d,
    code_unordr_f,
    code_unordr_d,
    code_movr_i,
    code_movr_ui,
    code_movr_l,
    code_movr_ul,
    code_movr_p,
    code_movr_f,
    code_movr_d,
    code_movi_i,
    code_movi_ui,
    code_movi_l,
    code_movi_ul,
    code_movi_p,
    code_movi_f,
    code_movi_d,
    code_extr_c_i,
    code_extr_uc_ui,
    code_extr_s_i,
    code_extr_us_ui,
    code_extr_c_l,
    code_extr_uc_ul,
    code_extr_s_l,
    code_extr_us_ul,
    code_extr_i_l,
    code_extr_ui_ul,
    code_extr_i_f,
    code_extr_i_d,
    code_extr_l_f,
    code_extr_l_d,
    code_extr_f_d,
    code_extr_d_f,

    code_movr_i_f,
    code_movr_l_d,
    code_movr_f_i,
    code_movr_d_l,

    code_roundr_f_i,
    code_roundr_f_l,
    code_roundr_d_i,
    code_roundr_d_l,
    code_truncr_f_i,
    code_truncr_f_l,
    code_truncr_d_i,
    code_truncr_d_l,
    code_floorr_f_i,
    code_floorr_f_l,
    code_floorr_d_i,
    code_floorr_d_l,
    code_ceilr_f_i,
    code_ceilr_f_l,
    code_ceilr_d_i,
    code_ceilr_d_l,
    code_hton_us_ui,
    code_ntoh_us_ui,
    code_ldr_c,
    code_ldr_uc,
    code_ldr_s,
    code_ldr_us,
    code_ldr_i,
    code_ldr_ui,
    code_ldr_l,
    code_ldr_ul,
    code_ldr_p,
    code_ldr_f,
    code_ldr_d,
    code_ldi_c,
    code_ldi_uc,
    code_ldi_s,
    code_ldi_us,
    code_ldi_i,
    code_ldi_ui,
    code_ldi_l,
    code_ldi_ul,
    code_ldi_p,
    code_ldi_f,
    code_ldi_d,
    code_ldxr_c,
    code_ldxr_uc,
    code_ldxr_s,
    code_ldxr_us,
    code_ldxr_i,
    code_ldxr_ui,
    code_ldxr_l,
    code_ldxr_ul,
    code_ldxr_p,
    code_ldxr_f,
    code_ldxr_d,
    code_ldxi_c,
    code_ldxi_uc,
    code_ldxi_s,
    code_ldxi_us,
    code_ldxi_i,
    code_ldxi_ui,
    code_ldxi_l,
    code_ldxi_ul,
    code_ldxi_p,
    code_ldxi_f,
    code_ldxi_d,
    code_str_c,
    code_str_uc,
    code_str_s,
    code_str_us,
    code_str_i,
    code_str_ui,
    code_str_l,
    code_str_ul,
    code_str_p,
    code_str_f,
    code_str_d,
    code_sti_c,
    code_sti_uc,
    code_sti_s,
    code_sti_us,
    code_sti_i,
    code_sti_ui,
    code_sti_l,
    code_sti_ul,
    code_sti_p,
    code_sti_f,
    code_sti_d,
    code_stxr_c,
    code_stxr_uc,
    code_stxr_s,
    code_stxr_us,
    code_stxr_i,
    code_stxr_ui,
    code_stxr_l,
    code_stxr_ul,
    code_stxr_p,
    code_stxr_f,
    code_stxr_d,
    code_stxi_c,
    code_stxi_uc,
    code_stxi_s,
    code_stxi_us,
    code_stxi_i,
    code_stxi_ui,
    code_stxi_l,
    code_stxi_ul,
    code_stxi_p,
    code_stxi_f,
    code_stxi_d,
    code_prepare_i,
    code_prepare_f,
    code_prepare_d,
    code_pusharg_c,
    code_pusharg_uc,
    code_pusharg_s,
    code_pusharg_us,
    code_pusharg_i,
    code_pusharg_ui,
    code_pusharg_l,
    code_pusharg_ul,
    code_pusharg_p,
    code_pusharg_f,
    code_pusharg_d,
    code_getarg_c,
    code_getarg_uc,
    code_getarg_s,
    code_getarg_us,
    code_getarg_i,
    code_getarg_ui,
    code_getarg_l,
    code_getarg_ul,
    code_getarg_p,
    code_getarg_f,
    code_getarg_d,
    code_arg_c,
    code_arg_uc,
    code_arg_s,
    code_arg_us,
    code_arg_i,
    code_arg_ui,
    code_arg_l,
    code_arg_ul,
    code_arg_p,
    code_arg_f,
    code_arg_d,
    code_retval_c,
    code_retval_uc,
    code_retval_s,
    code_retval_us,
    code_retval_i,
    code_retval_ui,
    code_retval_l,
    code_retval_ul,
    code_retval_p,
    code_retval_f,
    code_retval_d,
    code_bltr_i,
    code_bltr_ui,
    code_bltr_l,
    code_bltr_ul,
    code_bltr_p,
    code_bltr_f,
    code_bltr_d,
    code_blti_i,
    code_blti_ui,
    code_blti_l,
    code_blti_ul,
    code_blti_p,
    code_bler_i,
    code_bler_ui,
    code_bler_l,
    code_bler_ul,
    code_bler_p,
    code_bler_f,
    code_bler_d,
    code_blei_i,
    code_blei_ui,
    code_blei_l,
    code_blei_ul,
    code_blei_p,
    code_beqr_i,
    code_beqr_ui,
    code_beqr_l,
    code_beqr_ul,
    code_beqr_p,
    code_beqr_f,
    code_beqr_d,
    code_beqi_i,
    code_beqi_ui,
    code_beqi_l,
    code_beqi_ul,
    code_beqi_p,
    code_bger_i,
    code_bger_ui,
    code_bger_l,
    code_bger_ul,
    code_bger_p,
    code_bger_f,
    code_bger_d,
    code_bgei_i,
    code_bgei_ui,
    code_bgei_l,
    code_bgei_ul,
    code_bgei_p,
    code_bgtr_i,
    code_bgtr_ui,
    code_bgtr_l,
    code_bgtr_ul,
    code_bgtr_p,
    code_bgtr_f,
    code_bgtr_d,
    code_bgti_i,
    code_bgti_ui,
    code_bgti_l,
    code_bgti_ul,
    code_bgti_p,
    code_bner_i,
    code_bner_ui,
    code_bner_l,
    code_bner_ul,
    code_bner_p,
    code_bner_f,
    code_bner_d,
    code_bnei_i,
    code_bnei_ui,
    code_bnei_l,
    code_bnei_ul,
    code_bnei_p,
    code_bunltr_f,
    code_bunltr_d,
    code_bunler_f,
    code_bunler_d,
    code_buneqr_f,
    code_buneqr_d,
    code_bunger_f,
    code_bunger_d,
    code_bungtr_f,
    code_bungtr_d,
    code_bltgtr_f,
    code_bltgtr_d,
    code_bordr_f,
    code_bordr_d,
    code_bunordr_f,
    code_bunordr_d,
    code_bmsr_i,
    code_bmsr_ui,
    code_bmsr_l,
    code_bmsr_ul,
    code_bmsi_i,
    code_bmsi_ui,
    code_bmsi_l,
    code_bmsi_ul,
    code_bmcr_i,
    code_bmcr_ui,
    code_bmcr_l,
    code_bmcr_ul,
    code_bmci_i,
    code_bmci_ui,
    code_bmci_l,
    code_bmci_ul,
    code_boaddr_i,
    code_boaddr_ui,
    code_boaddr_l,
    code_boaddr_ul,
    code_boaddi_i,
    code_boaddi_ui,
    code_boaddi_l,
    code_boaddi_ul,
    code_bosubr_i,
    code_bosubr_ui,
    code_bosubr_l,
    code_bosubr_ul,
    code_bosubi_i,
    code_bosubi_ui,
    code_bosubi_l,
    code_bosubi_ul,
    code_finish,
    code_finishr,
    code_calli,
    code_callr,
    code_jmpi,
    code_jmpr,
    code_ret,
    code_leave,
    code_prolog,
    code_prolog_f,
    code_prolog_d,
    code_leaf,
    code_allocai,
} ejit_code_t;

union ejit_data {
    int			 i;
    unsigned int	ui;
    long		 l;
    unsigned long	ul;
    float		 f;
    double		 d;
    void		*p;
    ejit_node_t		*n;
};

/* argument is an ejit_node_t and not a fixed address pointer */
#define EJIT_NODE_ARG	1

struct ejit_node {
    ejit_node_t		*next;
    ejit_code_t		 code;
    int			 hint;
    ejit_data_t		 u;
    ejit_data_t		 v;
    ejit_data_t		 w;
    ejit_node_t		*link;
};

struct ejit_state {
    ejit_node_t		*head;
    ejit_node_t		*tail;
};

/*
 * Prototypes
 */
extern ejit_state_t *
ejit_create_state(void);

extern void
ejit_patch(ejit_state_t *s, ejit_node_t *label, ejit_node_t *instr);

extern ejit_node_t *
ejit(ejit_state_t *s, ejit_code_t c);

extern ejit_node_t *
ejit_i(ejit_state_t *s, ejit_code_t c, int u);

extern ejit_node_t *
ejit_p(ejit_state_t *s, ejit_code_t c, void *u);

extern ejit_node_t *
ejit_i_i(ejit_state_t *s, ejit_code_t c, int u, int v);

extern ejit_node_t *
ejit_i_l(ejit_state_t *s, ejit_code_t c, int u, long v);

extern ejit_node_t *
ejit_i_p(ejit_state_t *s, ejit_code_t c, int u, void *v);

extern ejit_node_t *
ejit_i_f(ejit_state_t *s, ejit_code_t c, int u, float v);

extern ejit_node_t *
ejit_i_d(ejit_state_t *s, ejit_code_t c, int u, double v);

extern ejit_node_t *
ejit_i_i_i(ejit_state_t *s, ejit_code_t c, int u, int v, int w);

extern ejit_node_t *
ejit_i_i_l(ejit_state_t *s, ejit_code_t c, int u, int v, long w);

extern ejit_node_t *
ejit_i_i_p(ejit_state_t *s, ejit_code_t c, int u, int v, void *w);

extern ejit_node_t *
ejit_l_i_i(ejit_state_t *s, ejit_code_t c, long u, int v, int w);

extern ejit_node_t *
ejit_p_i(ejit_state_t *s, ejit_code_t c, void *u, int v);

extern ejit_node_t *
ejit_n_i_i(ejit_state_t *s, ejit_code_t c, ejit_node_t *u, int v, int w);

extern ejit_node_t *
ejit_n_i_l(ejit_state_t *s, ejit_code_t c, ejit_node_t *u, int v, long w);

extern ejit_node_t *
ejit_n_i_p(ejit_state_t *s, ejit_code_t c, ejit_node_t *u, int v, void *w);

extern int
ejit_optimize(ejit_state_t *s);

extern void
ejit_print(ejit_state_t *s);

#endif /* _thunder_h */
