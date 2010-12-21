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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "thunder.h"

/*
 * Prototypes
 */
static inline ejit_node_t *
node_new(ejit_code_t c);

static inline ejit_node_t *
node_link(ejit_state_t *s, ejit_node_t *n);

/*
 * Implementation
 */
static inline ejit_node_t *
node_new(ejit_code_t c)
{
    ejit_node_t	*n = calloc(1, sizeof(ejit_node_t));
    n->code = c;
    return (n);
}

static inline ejit_node_t *
node_link(ejit_state_t *s, ejit_node_t *n)
{
    if (s->tail)
	s->tail->next = n;
    else
	s->head = n;
    return (s->tail = n);
}

ejit_state_t *
ejit_create_state(void)
{
    return (calloc(1, sizeof(ejit_state_t)));
}

void
ejit_patch(ejit_state_t *s, ejit_node_t *label, ejit_node_t *instr)
{
    assert(label->code == code_label);
#if !defined(NDEBUG)
    switch (instr->code) {
	case code_movi_p:
	case code_bltr_i:	case code_bltr_ui:
	case code_bltr_l:	case code_bltr_ul:
	case code_bltr_p:
	case code_bltr_f:	case code_bltr_d:
	case code_blti_i:	case code_blti_ui:
	case code_bler_i:	case code_bler_ui:
	case code_bler_l:	case code_bler_ul:
	case code_bler_p:
	case code_bler_f:	case code_bler_d:
	case code_blei_i:	case code_blei_ui:
	case code_beqr_i:	case code_beqr_ui:
	case code_beqr_l:	case code_beqr_ul:
	case code_beqr_p:
	case code_beqr_f:	case code_beqr_d:
	case code_beqi_i:	case code_beqi_ui:
	case code_bger_i:	case code_bger_ui:
	case code_bger_l:	case code_bger_ul:
	case code_bger_p:
	case code_bger_f:	case code_bger_d:
	case code_bgei_i:	case code_bgei_ui:
	case code_bgtr_i:	case code_bgtr_ui:
	case code_bgtr_l:	case code_bgtr_ul:
	case code_bgtr_p:
	case code_bgtr_f:	case code_bgtr_d:
	case code_bgti_i:	case code_bgti_ui:
	case code_bner_i:	case code_bner_ui:
	case code_bner_l:	case code_bner_ul:
	case code_bner_p:
	case code_bner_f:	case code_bner_d:
	case code_bnei_i:	case code_bnei_ui:
	case code_bunltr_f:	case code_bunltr_d:
	case code_bunler_f:	case code_bunler_d:
	case code_buneqr_f:	case code_buneqr_d:
	case code_bunger_f:	case code_bunger_d:
	case code_bungtr_f:	case code_bungtr_d:
	case code_bltgtr_f:	case code_bltgtr_d:
	case code_bordr_f:	case code_bordr_d:
	case code_bunordr_f:	case code_bunordr_d:
	case code_bmsr_i:	case code_bmsr_ui:
	case code_bmsr_l:	case code_bmsr_ul:
	case code_bmsi_i:	case code_bmsi_ui:
	case code_bmsi_l:	case code_bmsi_ul:
	case code_bmcr_i:	case code_bmcr_ui:
	case code_bmcr_l:	case code_bmcr_ul:
	case code_bmci_i:	case code_bmci_ui:
	case code_bmci_l:	case code_bmci_ul:
	case code_boaddr_i:	case code_boaddr_ui:
	case code_boaddr_l:	case code_boaddr_ul:
	case code_boaddi_i:	case code_boaddi_ui:
	case code_boaddi_l:	case code_boaddi_ul:
	case code_bosubr_i:	case code_bosubr_ui:
	case code_bosubr_l:	case code_bosubr_ul:
	case code_bosubi_i:	case code_bosubi_ui:
	case code_bosubi_l:	case code_bosubi_ul:
	case code_finish:
	case code_calli:
	case code_jmpi:
	    break;
	default:
	    fprintf(stderr, "unsupported patch\n");
	    abort();
    }
#endif
    instr->link = label->link;
    label->link = instr;
}

ejit_node_t *
ejit(ejit_state_t *s, ejit_code_t c)
{
    ejit_node_t	*n = node_new(c);
    return (node_link(s, n));
}

ejit_node_t *
ejit_i(ejit_state_t *s, ejit_code_t c, int u)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    return (node_link(s, n));
}

ejit_node_t *
ejit_p(ejit_state_t *s, ejit_code_t c, void *u)
{
    ejit_node_t	*n = node_new(c);
    n->u.p = u;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_i(ejit_state_t *s, ejit_code_t c, int u, int v)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.i = v;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_l(ejit_state_t *s, ejit_code_t c, int u, long v)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.l = v;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_p(ejit_state_t *s, ejit_code_t c, int u, void *v)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.p = v;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_f(ejit_state_t *s, ejit_code_t c, int u, float v)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.f = v;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_d(ejit_state_t *s, ejit_code_t c, int u, double v)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.d = v;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_i_i(ejit_state_t *s, ejit_code_t c, int u, int v, int w)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.i = v;
    n->w.i = w;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_i_l(ejit_state_t *s, ejit_code_t c, int u, int v, long w)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.i = v;
    n->w.l = w;
    return (node_link(s, n));
}

ejit_node_t *
ejit_i_i_p(ejit_state_t *s, ejit_code_t c, int u, int v, void *w)
{
    ejit_node_t	*n = node_new(c);
    n->u.i = u;
    n->v.i = v;
    n->w.p = w;
    return (node_link(s, n));
}

ejit_node_t *
ejit_l_i_i(ejit_state_t *s, ejit_code_t c, long u, int v, int w)
{
    ejit_node_t	*n = node_new(c);
    n->u.l = u;
    n->v.i = v;
    n->w.i = w;
    return (node_link(s, n));
}

ejit_node_t *
ejit_p_i(ejit_state_t *s, ejit_code_t c, void *u, int v)
{
    ejit_node_t	*n = node_new(c);
    n->u.p = u;
    n->v.i = v;
    return (node_link(s, n));
}

ejit_node_t *
ejit_n_i_i(ejit_state_t *s, ejit_code_t c, ejit_node_t *u, int v, int w)
{
    ejit_node_t	*n = node_new(c);
    n->u.n = u;
    n->v.i = v;
    n->w.i = w;
    return (node_link(s, n));
}

ejit_node_t *
ejit_n_i_l(ejit_state_t *s, ejit_code_t c, ejit_node_t *u, int v, long w)
{
    ejit_node_t	*n = node_new(c);
    n->u.n = u;
    n->v.i = v;
    n->w.l = w;
    return (node_link(s, n));
}

ejit_node_t *
ejit_n_i_p(ejit_state_t *s, ejit_code_t c, ejit_node_t *u, int v, void *w)
{
    ejit_node_t	*n = node_new(c);
    n->u.n = u;
    n->v.i = v;
    n->w.p = w;
    return (node_link(s, n));
}

void
ejit_print(ejit_state_t *s)
{
    ejit_node_t	*n = s->head;
    for (; n; n = n->next) {
	printf("\n\t");
	switch (n->code) {
	i:
	    printf(" %d", n->u.i);				continue;
	p:
	    printf(" %p", n->u.p);				continue;
	n:
	    printf(" %p", n);					continue;
	i_i:
	    printf(" %d %d", n->u.i, n->v.i);			continue;
	i_ui:
	    printf(" %d %u", n->u.i, n->v.ui);			continue;
	i_l:
	    printf(" %d %ld", n->u.i, n->v.l);			continue;
	i_ul:
	    printf(" %d %lu", n->u.i, n->v.ul);			continue;
	i_p:
	    printf(" %d %p", n->u.i, n->v.p);			continue;
	i_f:
	    printf(" %d %f", n->u.i, (double)n->v.f);		continue;
	i_d:
	    printf(" %d %f", n->u.i, n->v.d);			continue;
	p_i:
	    printf(" %p %d", n->u.p, n->v.i);			continue;
	i_i_i:
	    printf(" %d %d %d", n->u.i, n->v.i, n->w.i);	continue;
	i_i_ui:
	    printf(" %d %d %u", n->u.i, n->v.i, n->w.ui);	continue;
	i_i_l:
	    printf(" %d %d %ld", n->u.i, n->v.i, n->w.l);	continue;
	i_i_ul:
	    printf(" %d %d %lu", n->u.i, n->v.i, n->w.ul);	continue;
	i_i_p:
	    printf(" %d %d %p", n->u.i, n->v.i, n->w.p);	continue;
	l_i_i:
	    printf(" %ld %d %d", n->u.l, n->v.i, n->w.i);	continue;
	p_i_i:
	    printf(" %p %d %d", n->u.p, n->v.i, n->w.i);	continue;
	p_i_ui:
	    printf(" %p %d %u", n->u.p, n->v.i, n->w.ui);	continue;
	p_i_l:
	    printf(" %p %d %ld", n->u.p, n->v.i, n->w.l);	continue;
	p_i_ul:
	    printf(" %p %d %lu", n->u.p, n->v.i, n->w.ul);	continue;
	p_i_p:
	    printf(" %p %d %p", n->u.p, n->v.i, n->w.p);	continue;
	    case code_note:		printf("note");		break;
	    case code_label:		printf("label");	goto n;
	    case code_addr_i:		printf("addr_i");	goto i_i_i;
	    case code_addr_ui:		printf("addr_ui");	goto i_i_i;
	    case code_addr_l:		printf("addr_l");	goto i_i_i;
	    case code_addr_ul:		printf("addr_ul");	goto i_i_i;
	    case code_addr_p:		printf("addr_p");	goto i_i_i;
	    case code_addr_f:		printf("addr_f");	goto i_i_i;
	    case code_addr_d:		printf("addr_d");	goto i_i_i;
	    case code_addi_i:		printf("addi_i");	goto i_i_i;
	    case code_addi_ui:		printf("addi_ui");	goto i_i_ui;
	    case code_addi_l:		printf("addi_l");	goto i_i_l;
	    case code_addi_ul:		printf("addi_ul");	goto i_i_ul;
	    case code_addi_p:		printf("addi_p");	goto i_i_p;
	    case code_addxr_ui:		printf("addxr_ui");	goto i_i_i;
	    case code_addxr_ul:		printf("addxr_ul");	goto i_i_i;
	    case code_addxi_ui:		printf("addxi_ui");	goto i_i_ui;
	    case code_addxi_ul:		printf("addxi_ul");	goto i_i_ul;
	    case code_addcr_ui:		printf("addcr_ui");	goto i_i_i;
	    case code_addcr_ul:		printf("addcr_ul");	goto i_i_i;
	    case code_addci_ui:		printf("addci_ui");	goto i_i_ui;
	    case code_addci_ul:		printf("addci_ul");	goto i_i_ul;
	    case code_subr_i:		printf("subr_i");	goto i_i_i;
	    case code_subr_ui:		printf("subr_ui");	goto i_i_i;
	    case code_subr_l:		printf("subr_l");	goto i_i_i;
	    case code_subr_ul:		printf("subr_ul");	goto i_i_i;
	    case code_subr_p:		printf("subr_p");	goto i_i_i;
	    case code_subr_f:		printf("subr_f");	goto i_i_i;
	    case code_subr_d:		printf("subr_d");	goto i_i_i;
	    case code_subi_i:		printf("subi_i");	goto i_i_i;
	    case code_subi_ui:		printf("subi_ui");	goto i_i_ui;
	    case code_subi_l:		printf("subi_l");	goto i_i_l;
	    case code_subi_ul:		printf("subi_ul");	goto i_i_ul;
	    case code_subi_p:		printf("subi_p");	goto i_i_p;
	    case code_subxr_ui:		printf("subxr_ui");	goto i_i_i;
	    case code_subxr_ul:		printf("subxr_ul");	goto i_i_i;
	    case code_subxi_ui:		printf("subxi_ui");	goto i_i_ui;
	    case code_subxi_ul:		printf("subxi_ul");	goto i_i_ul;
	    case code_subcr_ui:		printf("subcr_ui");	goto i_i_i;
	    case code_subcr_ul:		printf("subcr_ul");	goto i_i_i;
	    case code_subci_ui:		printf("subci_ui");	goto i_i_ui;
	    case code_subci_ul:		printf("subci_ul");	goto i_i_ul;
	    case code_rsbr_i:		printf("rsbr_i");	goto i_i_i;
	    case code_rsbr_ui:		printf("rsbr_ui");	goto i_i_i;
	    case code_rsbr_l:		printf("rsbr_l");	goto i_i_i;
	    case code_rsbr_ul:		printf("rsbr_ul");	goto i_i_i;
	    case code_rsbr_p:		printf("rsbr_p");	goto i_i_i;
	    case code_rsbr_f:		printf("rsbr_f");	goto i_i_i;
	    case code_rsbr_d:		printf("rsbr_d");	goto i_i_i;
	    case code_rsbi_i:		printf("rsbi_i");	goto i_i_i;
	    case code_rsbi_ui:		printf("rsbi_ui");	goto i_i_ui;
	    case code_rsbi_l:		printf("rsbi_l");	goto i_i_l;
	    case code_rsbi_ul:		printf("rsbi_ul");	goto i_i_ul;
	    case code_rsbi_p:		printf("rsbi_p");	goto i_i_p;
	    case code_mulr_i:		printf("mulr_i");	goto i_i_i;
	    case code_mulr_ui:		printf("mulr_ui");	goto i_i_i;
	    case code_mulr_l:		printf("mulr_l");	goto i_i_i;
	    case code_mulr_ul:		printf("mulr_ul");	goto i_i_i;
	    case code_mulr_f:		printf("mulr_f");	goto i_i_i;
	    case code_mulr_d:		printf("mulr_d");	goto i_i_i;
	    case code_muli_i:		printf("muli_i");	goto i_i_i;
	    case code_muli_ui:		printf("muli_ui");	goto i_i_ui;
	    case code_muli_l:		printf("muli_l");	goto i_i_l;
	    case code_muli_ul:		printf("muli_ul");	goto i_i_ul;
	    case code_hmulr_i:		printf("hmulr_i");	goto i_i_i;
	    case code_hmulr_ui:		printf("hmulr_ui");	goto i_i_i;
	    case code_hmulr_l:		printf("hmulr_l");	goto i_i_i;
	    case code_hmulr_ul:		printf("hmulr_ul");	goto i_i_i;
	    case code_hmuli_i:		printf("hmuli_i");	goto i_i_i;
	    case code_hmuli_ui:		printf("hmuli_ui");	goto i_i_ui;
	    case code_hmuli_l:		printf("hmuli_l");	goto i_i_l;
	    case code_hmuli_ul:		printf("hmuli_ul");	goto i_i_ul;
	    case code_divr_i:		printf("divr_i");	goto i_i_i;
	    case code_divr_ui:		printf("divr_ui");	goto i_i_i;
	    case code_divr_l:		printf("divr_l");	goto i_i_i;
	    case code_divr_ul:		printf("divr_ul");	goto i_i_i;
	    case code_divr_f:		printf("divr_f");	goto i_i_i;
	    case code_divr_d:		printf("divr_d");	goto i_i_i;
	    case code_divi_i:		printf("divi_i");	goto i_i_i;
	    case code_divi_ui:		printf("divi_ui");	goto i_i_ui;
	    case code_divi_l:		printf("divi_l");	goto i_i_l;
	    case code_divi_ul:		printf("divi_ul");	goto i_i_ul;
	    case code_modr_i:		printf("modr_i");	goto i_i_i;
	    case code_modr_ui:		printf("modr_ui");	goto i_i_i;
	    case code_modr_l:		printf("modr_l");	goto i_i_i;
	    case code_modr_ul:		printf("modr_ul");	goto i_i_i;
	    case code_modi_i:		printf("modi_i");	goto i_i_i;
	    case code_modi_ui:		printf("modi_ui");	goto i_i_ui;
	    case code_modi_l:		printf("modi_l");	goto i_i_l;
	    case code_modi_ul:		printf("modi_ul");	goto i_i_ul;
	    case code_andr_i:		printf("andr_i");	goto i_i_i;
	    case code_andr_ui:		printf("andr_ui");	goto i_i_i;
	    case code_andr_l:		printf("andr_l");	goto i_i_i;
	    case code_andr_ul:		printf("andr_ul");	goto i_i_i;
	    case code_andi_i:		printf("andi_i");	goto i_i_i;
	    case code_andi_ui:		printf("andi_ui");	goto i_i_ui;
	    case code_andi_l:		printf("andi_l");	goto i_i_l;
	    case code_andi_ul:		printf("andi_ul");	goto i_i_ul;
	    case code_orr_i:		printf("orr_i");	goto i_i_i;
	    case code_orr_ui:		printf("orr_ui");	goto i_i_i;
	    case code_orr_l:		printf("orr_l");	goto i_i_i;
	    case code_orr_ul:		printf("orr_ul");	goto i_i_i;
	    case code_ori_i:		printf("ori_i");	goto i_i_i;
	    case code_ori_ui:		printf("ori_ui");	goto i_i_ui;
	    case code_ori_l:		printf("ori_l");	goto i_i_l;
	    case code_ori_ul:		printf("ori_ul");	goto i_i_ul;
	    case code_xorr_i:		printf("xorr_i");	goto i_i_i;
	    case code_xorr_ui:		printf("xorr_ui");	goto i_i_i;
	    case code_xorr_l:		printf("xorr_l");	goto i_i_i;
	    case code_xorr_ul:		printf("xorr_ul");	goto i_i_i;
	    case code_xori_i:		printf("xori_i");	goto i_i_i;
	    case code_xori_ui:		printf("xori_ui");	goto i_i_ui;
	    case code_xori_l:		printf("xori_l");	goto i_i_l;
	    case code_xori_ul:		printf("xori_ul");	goto i_i_ul;
	    case code_lshr_i:		printf("lshr_i");	goto i_i_i;
	    case code_lshr_ui:		printf("lshr_ui");	goto i_i_i;
	    case code_lshr_l:		printf("lshr_l");	goto i_i_i;
	    case code_lshr_ul:		printf("lshr_ul");	goto i_i_i;
	    case code_lshi_i:		printf("lshi_i");	goto i_i_i;
	    case code_lshi_ui:		printf("lshi_ui");	goto i_i_ui;
	    case code_lshi_l:		printf("lshi_l");	goto i_i_l;
	    case code_lshi_ul:		printf("lshi_ul");	goto i_i_ul;
	    case code_rshr_i:		printf("rshr_i");	goto i_i_i;
	    case code_rshr_ui:		printf("rshr_ui");	goto i_i_i;
	    case code_rshr_l:		printf("rshr_l");	goto i_i_i;
	    case code_rshr_ul:		printf("rshr_ul");	goto i_i_i;
	    case code_rshi_i:		printf("rshi_i");	goto i_i_i;
	    case code_rshi_ui:		printf("rshi_ui");	goto i_i_ui;
	    case code_rshi_l:		printf("rshi_l");	goto i_i_l;
	    case code_rshi_ul:		printf("rshi_ul");	goto i_i_ul;
	    case code_absr_f:		printf("absr_f");	goto i_i;
	    case code_absr_d:		printf("absr_d");	goto i_i;
	    case code_negr_i:		printf("negr_i");	goto i_i;
	    case code_negr_l:		printf("negr_l");	goto i_i;
	    case code_negr_f:		printf("negr_f");	goto i_i;
	    case code_negr_d:		printf("negr_d");	goto i_i;
	    case code_notr_i:		printf("notr_i");	goto i_i;
	    case code_notr_l:		printf("notr_l");	goto i_i;
	    case code_ltr_i:		printf("ltr_i");	goto i_i_i;
	    case code_ltr_ui:		printf("ltr_ui");	goto i_i_i;
	    case code_ltr_l:		printf("ltr_l");	goto i_i_i;
	    case code_ltr_ul:		printf("ltr_ul");	goto i_i_i;
	    case code_ltr_p:		printf("ltr_p");	goto i_i_i;
	    case code_ltr_f:		printf("ltr_f");	goto i_i_i;
	    case code_ltr_d:		printf("ltr_d");	goto i_i_i;
	    case code_lti_i:		printf("lti_i");	goto i_i_i;
	    case code_lti_ui:		printf("lti_ui");	goto i_i_ui;
	    case code_lti_l:		printf("lti_l");	goto i_i_l;
	    case code_lti_ul:		printf("lti_ul");	goto i_i_ul;
	    case code_lti_p:		printf("lti_p");	goto i_i_p;
	    case code_ler_i:		printf("ler_i");	goto i_i_i;
	    case code_ler_ui:		printf("ler_ui");	goto i_i_i;
	    case code_ler_l:		printf("ler_l");	goto i_i_i;
	    case code_ler_ul:		printf("ler_ul");	goto i_i_i;
	    case code_ler_p:		printf("ler_p");	goto i_i_i;
	    case code_ler_f:		printf("ler_f");	goto i_i_i;
	    case code_ler_d:		printf("ler_d");	goto i_i_i;
	    case code_lei_i:		printf("lei_i");	goto i_i_i;
	    case code_lei_ui:		printf("lei_ui");	goto i_i_ui;
	    case code_lei_l:		printf("lei_l");	goto i_i_l;
	    case code_lei_ul:		printf("lei_ul");	goto i_i_ul;
	    case code_lei_p:		printf("lei_p");	goto i_i_p;
	    case code_eqr_i:		printf("eqr_i");	goto i_i_i;
	    case code_eqr_ui:		printf("eqr_ui");	goto i_i_i;
	    case code_eqr_l:		printf("eqr_l");	goto i_i_i;
	    case code_eqr_ul:		printf("eqr_ul");	goto i_i_i;
	    case code_eqr_p:		printf("eqr_p");	goto i_i_i;
	    case code_eqr_f:		printf("eqr_f");	goto i_i_i;
	    case code_eqr_d:		printf("eqr_d");	goto i_i_i;
	    case code_eqi_i:		printf("eqi_i");	goto i_i_i;
	    case code_eqi_ui:		printf("eqi_ui");	goto i_i_ui;
	    case code_eqi_l:		printf("eqi_l");	goto i_i_l;
	    case code_eqi_ul:		printf("eqi_ul");	goto i_i_ul;
	    case code_eqi_p:		printf("eqi_p");	goto i_i_p;
	    case code_ger_i:		printf("ger_i");	goto i_i_i;
	    case code_ger_ui:		printf("ger_ui");	goto i_i_i;
	    case code_ger_l:		printf("ger_l");	goto i_i_i;
	    case code_ger_ul:		printf("ger_ul");	goto i_i_i;
	    case code_ger_p:		printf("ger_p");	goto i_i_i;
	    case code_ger_f:		printf("ger_f");	goto i_i_i;
	    case code_ger_d:		printf("ger_d");	goto i_i_i;
	    case code_gei_i:		printf("gei_i");	goto i_i_i;
	    case code_gei_ui:		printf("gei_ui");	goto i_i_ui;
	    case code_gei_l:		printf("gei_l");	goto i_i_l;
	    case code_gei_ul:		printf("gei_ul");	goto i_i_ul;
	    case code_gei_p:		printf("gei_p");	goto i_i_p;
	    case code_gtr_i:		printf("gtr_i");	goto i_i_i;
	    case code_gtr_ui:		printf("gtr_ui");	goto i_i_i;
	    case code_gtr_l:		printf("gtr_l");	goto i_i_i;
	    case code_gtr_ul:		printf("gtr_ul");	goto i_i_i;
	    case code_gtr_p:		printf("gtr_p");	goto i_i_i;
	    case code_gtr_f:		printf("gtr_f");	goto i_i_i;
	    case code_gtr_d:		printf("gtr_d");	goto i_i_i;
	    case code_gti_i:		printf("gti_i");	goto i_i_i;
	    case code_gti_ui:		printf("gti_ui");	goto i_i_ui;
	    case code_gti_l:		printf("gti_l");	goto i_i_l;
	    case code_gti_ul:		printf("gti_ul");	goto i_i_ul;
	    case code_gti_p:		printf("gti_p");	goto i_i_p;
	    case code_ner_i:		printf("ner_i");	goto i_i_i;
	    case code_ner_ui:		printf("ner_ui");	goto i_i_i;
	    case code_ner_l:		printf("ner_l");	goto i_i_i;
	    case code_ner_ul:		printf("ner_ul");	goto i_i_i;
	    case code_ner_p:		printf("ner_p");	goto i_i_i;
	    case code_ner_f:		printf("ner_f");	goto i_i_i;
	    case code_ner_d:		printf("ner_d");	goto i_i_i;
	    case code_nei_i:		printf("nei_i");	goto i_i_i;
	    case code_nei_ui:		printf("nei_ui");	goto i_i_ui;
	    case code_nei_l:		printf("nei_l");	goto i_i_l;
	    case code_nei_ul:		printf("nei_ul");	goto i_i_ul;
	    case code_nei_p:		printf("nei_p");	goto i_i_p;
	    case code_unltr_f:		printf("unltr_f");	goto i_i_i;
	    case code_unltr_d:		printf("unltr_d");	goto i_i_i;
	    case code_unler_f:		printf("unler_f");	goto i_i_i;
	    case code_unler_d:		printf("unler_d");	goto i_i_i;
	    case code_uneqr_f:		printf("uneqr_f");	goto i_i_i;
	    case code_uneqr_d:		printf("uneqr_d");	goto i_i_i;
	    case code_unger_f:		printf("unger_f");	goto i_i_i;
	    case code_unger_d:		printf("unger_d");	goto i_i_i;
	    case code_ungtr_f:		printf("ungtr_f");	goto i_i_i;
	    case code_ungtr_d:		printf("ungtr_d");	goto i_i_i;
	    case code_ltgtr_f:		printf("ltgtr_f");	goto i_i_i;
	    case code_ltgtr_d:		printf("ltgtr_d");	goto i_i_i;
	    case code_ordr_f:		printf("ordr_f");	goto i_i_i;
	    case code_ordr_d:		printf("ordr_d");	goto i_i_i;
	    case code_unordr_f:		printf("unordr_f");	goto i_i_i;
	    case code_unordr_d:		printf("unordr_d");	goto i_i_i;
	    case code_movr_i:		printf("movr_i");	goto i_i;
	    case code_movr_ui:		printf("movr_ui");	goto i_i;
	    case code_movr_l:		printf("movr_l");	goto i_i;
	    case code_movr_ul:		printf("movr_ul");	goto i_i;
	    case code_movr_p:		printf("movr_p");	goto i_i;
	    case code_movr_f:		printf("movr_f");	goto i_i;
	    case code_movr_d:		printf("movr_d");	goto i_i;
	    case code_movi_i:		printf("movi_i");	goto i_i;
	    case code_movi_ui:		printf("movi_ui");	goto i_ui;
	    case code_movi_l:		printf("movi_l");	goto i_l;
	    case code_movi_ul:		printf("movi_ul");	goto i_ul;
	    case code_movi_p:		printf("movi_p");	goto i_p;
	    case code_movi_f:		printf("movi_f");	goto i_f;
	    case code_movi_d:		printf("movi_d");	goto i_d;
	    case code_extr_c_i:		printf("extr_c_i");	goto i_i;
	    case code_extr_uc_ui:	printf("extr_uc_ui");	goto i_i;
	    case code_extr_s_i:		printf("extr_s_i");	goto i_i;
	    case code_extr_us_ui:	printf("extr_us_ui");	goto i_i;
	    case code_extr_c_l:		printf("extr_c_l");	goto i_i;
	    case code_extr_uc_ul:	printf("extr_uc_ul");	goto i_i;
	    case code_extr_s_l:		printf("extr_s_l");	goto i_i;
	    case code_extr_us_ul:	printf("extr_us_ul");	goto i_i;
	    case code_extr_i_f:		printf("extr_i_f");	goto i_i;
	    case code_extr_i_d:		printf("extr_i_d");	goto i_i;
	    case code_extr_l_f:		printf("extr_l_f");	goto i_i;
	    case code_extr_l_d:		printf("extr_l_d");	goto i_i;
	    case code_roundr_i_f:	printf("roundr_i_f");	goto i_i;
	    case code_roundr_i_d:	printf("roundr_i_d");	goto i_i;
	    case code_roundr_l_f:	printf("roundr_l_f");	goto i_i;
	    case code_roundr_l_d:	printf("roundr_l_d");	goto i_i;
	    case code_truncr_i_f:	printf("truncr_i_f");	goto i_i;
	    case code_truncr_i_d:	printf("truncr_i_d");	goto i_i;
	    case code_truncr_l_f:	printf("truncr_l_f");	goto i_i;
	    case code_truncr_l_d:	printf("truncr_l_d");	goto i_i;
	    case code_floorr_i_f:	printf("floorr_i_f");	goto i_i;
	    case code_floorr_i_d:	printf("floorr_i_d");	goto i_i;
	    case code_floorr_l_f:	printf("floorr_l_f");	goto i_i;
	    case code_floorr_l_d:	printf("floorr_l_d");	goto i_i;
	    case code_ceilr_i_f:	printf("ceilr_i_f");	goto i_i;
	    case code_ceilr_i_d:	printf("ceilr_i_d");	goto i_i;
	    case code_ceilr_l_f:	printf("ceilr_l_f");	goto i_i;
	    case code_ceilr_l_d:	printf("ceilr_l_d");	goto i_i;
	    case code_hton_us_ui:	printf("hton_us_ui");	goto i_i;
	    case code_ntoh_us_ui:	printf("ntoh_us_ui");	goto i_i;
	    case code_ldr_c:		printf("ldr_c");	goto i_i;
	    case code_ldr_uc:		printf("ldr_uc");	goto i_i;
	    case code_ldr_s:		printf("ldr_s");	goto i_i;
	    case code_ldr_us:		printf("ldr_us");	goto i_i;
	    case code_ldr_i:		printf("ldr_i");	goto i_i;
	    case code_ldr_ui:		printf("ldr_ui");	goto i_i;
	    case code_ldr_l:		printf("ldr_l");	goto i_i;
	    case code_ldr_ul:		printf("ldr_ul");	goto i_i;
	    case code_ldr_p:		printf("ldr_p");	goto i_i;
	    case code_ldr_f:		printf("ldr_f");	goto i_i;
	    case code_ldr_d:		printf("ldr_d");	goto i_i;
	    case code_ldi_c:		printf("ldi_c");	goto i_p;
	    case code_ldi_uc:		printf("ldi_uc");	goto i_p;
	    case code_ldi_s:		printf("ldi_s");	goto i_p;
	    case code_ldi_us:		printf("ldi_us");	goto i_p;
	    case code_ldi_i:		printf("ldi_i");	goto i_p;
	    case code_ldi_ui:		printf("ldi_ui");	goto i_p;
	    case code_ldi_l:		printf("ldi_l");	goto i_p;
	    case code_ldi_ul:		printf("ldi_ul");	goto i_p;
	    case code_ldi_p:		printf("ldi_p");	goto i_p;
	    case code_ldi_f:		printf("ldi_f");	goto i_p;
	    case code_ldi_d:		printf("ldi_d");	goto i_p;
	    case code_ldxr_c:		printf("ldxr_c");	goto i_i_i;
	    case code_ldxr_uc:		printf("ldxr_uc");	goto i_i_i;
	    case code_ldxr_s:		printf("ldxr_s");	goto i_i_i;
	    case code_ldxr_us:		printf("ldxr_us");	goto i_i_i;
	    case code_ldxr_i:		printf("ldxr_i");	goto i_i_i;
	    case code_ldxr_ui:		printf("ldxr_ui");	goto i_i_i;
	    case code_ldxr_l:		printf("ldxr_l");	goto i_i_i;
	    case code_ldxr_ul:		printf("ldxr_ul");	goto i_i_i;
	    case code_ldxr_p:		printf("ldxr_p");	goto i_i_i;
	    case code_ldxr_f:		printf("ldxr_f");	goto i_i_i;
	    case code_ldxr_d:		printf("ldxr_d");	goto i_i_i;
	    case code_ldxi_c:		printf("ldxi_c");	goto i_i_l;
	    case code_ldxi_uc:		printf("ldxi_uc");	goto i_i_l;
	    case code_ldxi_s:		printf("ldxi_s");	goto i_i_l;
	    case code_ldxi_us:		printf("ldxi_us");	goto i_i_l;
	    case code_ldxi_i:		printf("ldxi_i");	goto i_i_l;
	    case code_ldxi_ui:		printf("ldxi_ui");	goto i_i_l;
	    case code_ldxi_l:		printf("ldxi_l");	goto i_i_l;
	    case code_ldxi_ul:		printf("ldxi_ul");	goto i_i_l;
	    case code_ldxi_p:		printf("ldxi_p");	goto i_i_l;
	    case code_ldxi_f:		printf("ldxi_f");	goto i_i_l;
	    case code_ldxi_d:		printf("ldxi_d");	goto i_i_l;
	    case code_str_c:		printf("str_c");	goto i_i;
	    case code_str_uc:		printf("str_uc");	goto i_i;
	    case code_str_s:		printf("str_s");	goto i_i;
	    case code_str_us:		printf("str_us");	goto i_i;
	    case code_str_i:		printf("str_i");	goto i_i;
	    case code_str_ui:		printf("str_ui");	goto i_i;
	    case code_str_l:		printf("str_l");	goto i_i;
	    case code_str_ul:		printf("str_ul");	goto i_i;
	    case code_str_p:		printf("str_p");	goto i_i;
	    case code_str_f:		printf("str_f");	goto i_i;
	    case code_str_d:		printf("str_d");	goto i_i;
	    case code_sti_c:		printf("sti_c");	goto p_i;
	    case code_sti_uc:		printf("sti_uc");	goto p_i;
	    case code_sti_s:		printf("sti_s");	goto p_i;
	    case code_sti_us:		printf("sti_us");	goto p_i;
	    case code_sti_i:		printf("sti_i");	goto p_i;
	    case code_sti_ui:		printf("sti_ui");	goto p_i;
	    case code_sti_l:		printf("sti_l");	goto p_i;
	    case code_sti_ul:		printf("sti_ul");	goto p_i;
	    case code_sti_p:		printf("sti_p");	goto p_i;
	    case code_sti_f:		printf("sti_f");	goto p_i;
	    case code_sti_d:		printf("sti_d");	goto p_i;
	    case code_stxr_c:		printf("stxr_c");	goto i_i_i;
	    case code_stxr_uc:		printf("stxr_uc");	goto i_i_i;
	    case code_stxr_s:		printf("stxr_s");	goto i_i_i;
	    case code_stxr_us:		printf("stxr_us");	goto i_i_i;
	    case code_stxr_i:		printf("stxr_i");	goto i_i_i;
	    case code_stxr_ui:		printf("stxr_ui");	goto i_i_i;
	    case code_stxr_l:		printf("stxr_l");	goto i_i_i;
	    case code_stxr_ul:		printf("stxr_ul");	goto i_i_i;
	    case code_stxr_p:		printf("stxr_p");	goto i_i_i;
	    case code_stxr_f:		printf("stxr_f");	goto i_i_i;
	    case code_stxr_d:		printf("stxr_d");	goto i_i_i;
	    case code_stxi_c:		printf("stxi_c");	goto l_i_i;
	    case code_stxi_uc:		printf("stxi_uc");	goto l_i_i;
	    case code_stxi_s:		printf("stxi_s");	goto l_i_i;
	    case code_stxi_us:		printf("stxi_us");	goto l_i_i;
	    case code_stxi_i:		printf("stxi_i");	goto l_i_i;
	    case code_stxi_ui:		printf("stxi_ui");	goto l_i_i;
	    case code_stxi_l:		printf("stxi_l");	goto l_i_i;
	    case code_stxi_ul:		printf("stxi_ul");	goto l_i_i;
	    case code_stxi_p:		printf("stxi_p");	goto l_i_i;
	    case code_stxi_f:		printf("stxi_f");	goto l_i_i;
	    case code_stxi_d:		printf("stxi_d");	goto l_i_i;
	    case code_prepare_i:	printf("prepare_i");	goto i;
	    case code_prepare_f:	printf("prepare_f");	goto i;
	    case code_prepare_d:	printf("prepare_d");	goto i;
	    case code_pusharg_c:	printf("pusharg_c");	goto i;
	    case code_pusharg_uc:	printf("pusharg_uc");	goto i;
	    case code_pusharg_s:	printf("pusharg_s");	goto i;
	    case code_pusharg_us:	printf("pusharg_us");	goto i;
	    case code_pusharg_i:	printf("pusharg_i");	goto i;
	    case code_pusharg_ui:	printf("pusharg_ui");	goto i;
	    case code_pusharg_l:	printf("pusharg_l");	goto i;
	    case code_pusharg_ul:	printf("pusharg_ul");	goto i;
	    case code_pusharg_p:	printf("pusharg_p");	goto i;
	    case code_pusharg_f:	printf("pusharg_f");	goto i;
	    case code_pusharg_d:	printf("pusharg_d");	goto i;
	    case code_getarg_c:		printf("getarg_c");	goto i_p;
	    case code_getarg_uc:	printf("getarg_uc");	goto i_p;
	    case code_getarg_s:		printf("getarg_s");	goto i_p;
	    case code_getarg_us:	printf("getarg_us");	goto i_p;
	    case code_getarg_i:		printf("getarg_i");	goto i_p;
	    case code_getarg_ui:	printf("getarg_ui");	goto i_p;
	    case code_getarg_l:		printf("getarg_l");	goto i_p;
	    case code_getarg_ul:	printf("getarg_ul");	goto i_p;
	    case code_getarg_p:		printf("getarg_p");	goto i_p;
	    case code_getarg_f:		printf("getarg_f");	goto i_p;
	    case code_getarg_d:		printf("getarg_d");	goto i_p;
	    case code_putarg_c:		printf("putarg_c");	goto p_i;
	    case code_putarg_uc:	printf("putarg_uc");	goto p_i;
	    case code_putarg_s:		printf("putarg_s");	goto p_i;
	    case code_putarg_us:	printf("putarg_us");	goto p_i;
	    case code_putarg_i:		printf("putarg_i");	goto p_i;
	    case code_putarg_ui:	printf("putarg_ui");	goto p_i;
	    case code_putarg_l:		printf("putarg_l");	goto p_i;
	    case code_putarg_ul:	printf("putarg_ul");	goto p_i;
	    case code_putarg_p:		printf("putarg_p");	goto p_i;
	    case code_putarg_f:		printf("putarg_f");	goto p_i;
	    case code_putarg_d:		printf("putarg_d");	goto p_i;
	    case code_arg_c:		printf("arg_c");	goto n;
	    case code_arg_uc:		printf("arg_uc");	goto n;
	    case code_arg_s:		printf("arg_s");	goto n;
	    case code_arg_us:		printf("arg_us");	goto n;
	    case code_arg_i:		printf("arg_i");	goto n;
	    case code_arg_ui:		printf("arg_ui");	goto n;
	    case code_arg_l:		printf("arg_l");	goto n;
	    case code_arg_ul:		printf("arg_ul");	goto n;
	    case code_arg_p:		printf("arg_p");	goto n;
	    case code_arg_f:		printf("arg_f");	goto n;
	    case code_arg_d:		printf("arg_d");	goto n;
	    case code_retval_c:		printf("retval_c");	goto i;
	    case code_retval_uc:	printf("retval_uc");	goto i;
	    case code_retval_s:		printf("retval_s");	goto i;
	    case code_retval_us:	printf("retval_us");	goto i;
	    case code_retval_i:		printf("retval_i");	goto i;
	    case code_retval_ui:	printf("retval_ui");	goto i;
	    case code_retval_l:		printf("retval_l");	goto i;
	    case code_retval_ul:	printf("retval_ul");	goto i;
	    case code_retval_p:		printf("retval_p");	goto i;
	    case code_retval_f:		printf("retval_f");	goto i;
	    case code_retval_d:		printf("retval_d");	goto i;
	    case code_bltr_i:		printf("bltr_i");	goto p_i_i;
	    case code_bltr_ui:		printf("bltr_ui");	goto p_i_i;
	    case code_bltr_l:		printf("bltr_l");	goto p_i_i;
	    case code_bltr_ul:		printf("bltr_ul");	goto p_i_i;
	    case code_bltr_p:		printf("bltr_p");	goto p_i_i;
	    case code_bltr_f:		printf("bltr_f");	goto p_i_i;
	    case code_bltr_d:		printf("bltr_d");	goto p_i_i;
	    case code_blti_i:		printf("blti_i");	goto p_i_i;
	    case code_blti_ui:		printf("blti_ui");	goto p_i_ui;
	    case code_blti_l:		printf("blti_l");	goto p_i_l;
	    case code_blti_ul:		printf("blti_ul");	goto p_i_ul;
	    case code_blti_p:		printf("blti_p");	goto p_i_p;
	    case code_bler_i:		printf("bler_i");	goto p_i_i;
	    case code_bler_ui:		printf("bler_ui");	goto p_i_i;
	    case code_bler_l:		printf("bler_l");	goto p_i_i;
	    case code_bler_ul:		printf("bler_ul");	goto p_i_i;
	    case code_bler_p:		printf("bler_p");	goto p_i_i;
	    case code_bler_f:		printf("bler_f");	goto p_i_i;
	    case code_bler_d:		printf("bler_d");	goto p_i_i;
	    case code_blei_i:		printf("blei_i");	goto p_i_i;
	    case code_blei_ui:		printf("blei_ui");	goto p_i_ui;
	    case code_blei_l:		printf("blei_l");	goto p_i_l;
	    case code_blei_ul:		printf("blei_ul");	goto p_i_ul;
	    case code_blei_p:		printf("blei_p");	goto p_i_p;
	    case code_beqr_i:		printf("beqr_i");	goto p_i_i;
	    case code_beqr_ui:		printf("beqr_ui");	goto p_i_i;
	    case code_beqr_l:		printf("beqr_l");	goto p_i_i;
	    case code_beqr_ul:		printf("beqr_ul");	goto p_i_i;
	    case code_beqr_p:		printf("beqr_p");	goto p_i_i;
	    case code_beqr_f:		printf("beqr_f");	goto p_i_i;
	    case code_beqr_d:		printf("beqr_d");	goto p_i_i;
	    case code_beqi_i:		printf("beqi_i");	goto p_i_i;
	    case code_beqi_ui:		printf("beqi_ui");	goto p_i_ui;
	    case code_beqi_l:		printf("beqi_l");	goto p_i_l;
	    case code_beqi_ul:		printf("beqi_ul");	goto p_i_ul;
	    case code_beqi_p:		printf("beqi_p");	goto p_i_p;
	    case code_bger_i:		printf("bger_i");	goto p_i_i;
	    case code_bger_ui:		printf("bger_ui");	goto p_i_i;
	    case code_bger_l:		printf("bger_l");	goto p_i_i;
	    case code_bger_ul:		printf("bger_ul");	goto p_i_i;
	    case code_bger_p:		printf("bger_p");	goto p_i_i;
	    case code_bger_f:		printf("bger_f");	goto p_i_i;
	    case code_bger_d:		printf("bger_d");	goto p_i_i;
	    case code_bgei_i:		printf("bgei_i");	goto p_i_i;
	    case code_bgei_ui:		printf("bgei_ui");	goto p_i_ui;
	    case code_bgei_l:		printf("bgei_l");	goto p_i_l;
	    case code_bgei_ul:		printf("bgei_ul");	goto p_i_ul;
	    case code_bgei_p:		printf("bgei_p");	goto p_i_p;
	    case code_bgtr_i:		printf("bgtr_i");	goto p_i_i;
	    case code_bgtr_ui:		printf("bgtr_ui");	goto p_i_i;
	    case code_bgtr_l:		printf("bgtr_l");	goto p_i_i;
	    case code_bgtr_ul:		printf("bgtr_ul");	goto p_i_i;
	    case code_bgtr_p:		printf("bgtr_p");	goto p_i_i;
	    case code_bgtr_f:		printf("bgtr_f");	goto p_i_i;
	    case code_bgtr_d:		printf("bgtr_d");	goto p_i_i;
	    case code_bgti_i:		printf("bgti_i");	goto p_i_i;
	    case code_bgti_ui:		printf("bgti_ui");	goto p_i_ui;
	    case code_bgti_l:		printf("bgti_l");	goto p_i_l;
	    case code_bgti_ul:		printf("bgti_ul");	goto p_i_ul;
	    case code_bgti_p:		printf("bgti_p");	goto p_i_p;
	    case code_bner_i:		printf("bner_i");	goto p_i_i;
	    case code_bner_ui:		printf("bner_ui");	goto p_i_i;
	    case code_bner_l:		printf("bner_l");	goto p_i_i;
	    case code_bner_ul:		printf("bner_ul");	goto p_i_i;
	    case code_bner_p:		printf("bner_p");	goto p_i_i;
	    case code_bner_f:		printf("bner_f");	goto p_i_i;
	    case code_bner_d:		printf("bner_d");	goto p_i_i;
	    case code_bnei_i:		printf("bnei_i");	goto p_i_i;
	    case code_bnei_ui:		printf("bnei_ui");	goto p_i_ui;
	    case code_bnei_l:		printf("bnei_l");	goto p_i_l;
	    case code_bnei_ul:		printf("bnei_ul");	goto p_i_ul;
	    case code_bnei_p:		printf("bnei_p");	goto p_i_p;
	    case code_bunltr_f:		printf("bunltr_f");	goto p_i_i;
	    case code_bunltr_d:		printf("bunltr_d");	goto p_i_i;
	    case code_bunler_f:		printf("bunler_f");	goto p_i_i;
	    case code_bunler_d:		printf("bunler_d");	goto p_i_i;
	    case code_buneqr_f:		printf("buneqr_f");	goto p_i_i;
	    case code_buneqr_d:		printf("buneqr_d");	goto p_i_i;
	    case code_bunger_f:		printf("bunger_f");	goto p_i_i;
	    case code_bunger_d:		printf("bunger_d");	goto p_i_i;
	    case code_bungtr_f:		printf("bungtr_f");	goto p_i_i;
	    case code_bungtr_d:		printf("bungtr_d");	goto p_i_i;
	    case code_bltgtr_f:		printf("bltgtr_f");	goto p_i_i;
	    case code_bltgtr_d:		printf("bltgtr_d");	goto p_i_i;
	    case code_bordr_f:		printf("bordr_f");	goto p_i_i;
	    case code_bordr_d:		printf("bordr_d");	goto p_i_i;
	    case code_bunordr_f:	printf("bunordr_f");	goto p_i_i;
	    case code_bunordr_d:	printf("bunordr_d");	goto p_i_i;
	    case code_bmsr_i:		printf("bmsr_i");	goto p_i_i;
	    case code_bmsr_ui:		printf("bmsr_ui");	goto p_i_i;
	    case code_bmsr_l:		printf("bmsr_l");	goto p_i_i;
	    case code_bmsr_ul:		printf("bmsr_ul");	goto p_i_i;
	    case code_bmsi_i:		printf("bmsi_i");	goto p_i_i;
	    case code_bmsi_ui:		printf("bmsi_ui");	goto p_i_ui;
	    case code_bmsi_l:		printf("bmsi_l");	goto p_i_l;
	    case code_bmsi_ul:		printf("bmsi_ul");	goto p_i_ul;
	    case code_bmcr_i:		printf("bmcr_i");	goto p_i_i;
	    case code_bmcr_ui:		printf("bmcr_ui");	goto p_i_i;
	    case code_bmcr_l:		printf("bmcr_l");	goto p_i_i;
	    case code_bmcr_ul:		printf("bmcr_ul");	goto p_i_i;
	    case code_bmci_i:		printf("bmci_i");	goto p_i_i;
	    case code_bmci_ui:		printf("bmci_ui");	goto p_i_ui;
	    case code_bmci_l:		printf("bmci_l");	goto p_i_l;
	    case code_bmci_ul:		printf("bmci_ul");	goto p_i_ul;
	    case code_boaddr_i:		printf("boaddr_i");	goto p_i_i;
	    case code_boaddr_ui:	printf("boaddr_ui");	goto p_i_i;
	    case code_boaddr_l:		printf("boaddr_l");	goto p_i_i;
	    case code_boaddr_ul:	printf("boaddr_ul");	goto p_i_i;
	    case code_boaddi_i:		printf("boaddi_i");	goto p_i_i;
	    case code_boaddi_ui:	printf("boaddi_ui");	goto p_i_ui;
	    case code_boaddi_l:		printf("boaddi_l");	goto p_i_l;
	    case code_boaddi_ul:	printf("boaddi_ul");	goto p_i_ul;
	    case code_bosubr_i:		printf("bosubr_i");	goto p_i_i;
	    case code_bosubr_ui:	printf("bosubr_ui");	goto p_i_i;
	    case code_bosubr_l:		printf("bosubr_l");	goto p_i_i;
	    case code_bosubr_ul:	printf("bosubr_ul");	goto p_i_i;
	    case code_bosubi_i:		printf("bosubi_i");	goto p_i_i;
	    case code_bosubi_ui:	printf("bosubi_ui");	goto p_i_ui;
	    case code_bosubi_l:		printf("bosubi_l");	goto p_i_l;
	    case code_bosubi_ul:	printf("bosubi_ul");	goto p_i_ul;
	    case code_finish:		printf("finish");	goto p;
	    case code_finishr:		printf("finishr");	goto i;
	    case code_calli:		printf("calli");	goto p;
	    case code_callr:		printf("callr");	goto i;
	    case code_jmpi:		printf("jmpi");		goto p;
	    case code_jmpr:		printf("jmpr");		goto i;
	    case code_ret:		printf("ret");		break;;
	    case code_prolog:		printf("prolog");	goto i;
	    case code_prolog_f:		printf("prolog_f");	goto i;
	    case code_prolog_d:		printf("prolog_d");	goto i;
	    case code_leaf:		printf("leaf");		goto i;
	    case code_allocai:		printf("allocai");	goto i;
	    default:						abort();
	}
    }
}
