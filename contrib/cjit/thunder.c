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
#include "lightning.h"

/*
 * Prototypes
 */
static inline ejit_node_t *
node_new(ejit_code_t c);

static inline ejit_frame_t *
frame_new(void);

static inline void
frame_del(ejit_frame_t *f);

static inline void
node_del(ejit_state_t *s, ejit_node_t *p, ejit_node_t *n);

static inline ejit_node_t *
node_link(ejit_state_t *s, ejit_node_t *n);

static int
arg_i(ejit_frame_t *f, ejit_register_t **r);

static int
arg_f(ejit_frame_t *f, ejit_register_t **r);

static int
arg_d(ejit_frame_t *f, ejit_register_t **r);

static int
redundant_jump(ejit_state_t *s, ejit_node_t *p, ejit_node_t *n);

static int
reverse_jump(ejit_state_t *s, ejit_node_t *p, ejit_node_t *n);

static char *
is(int reg);

static char *
fs(int reg);

/*
 * Initialization
 */
ejit_register_t	ejit_gpr_regs[EJIT_NUM_HARD_GPR_REGS];
ejit_register_t	ejit_fpr_regs[EJIT_NUM_HARD_FPR_REGS];

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

static inline ejit_frame_t *
frame_new(void)
{
    ejit_frame_t	*f = calloc(1, sizeof(ejit_frame_t));
    return (f);
}

static inline void
frame_del(ejit_frame_t *f)
{
    free(f);
}

static inline void
node_del(ejit_state_t *s, ejit_node_t *p, ejit_node_t *n)
{
    if (p == n) {
	assert(p == s->head);
	s->head = n->next;
    }
    else
	p->next = n->next;
    free(n);
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

static int
arg_i(ejit_frame_t *f, ejit_register_t **r)
{
    int		o;

#if defined(__x86_64__)
    o = f->argi;
    if (f->argi < EJIT_NUM_GPR_ARGS) {
	*r = ejit_gpr_regs + EJIT_OFS_GPR_ARGS + o;
	++f->argi;
    }
    else {
	*r = NULL;
	o = f->size;
	f->size += sizeof(long);
    }
#elif defined(__mips__)
    o = (s->size - EJIT_FRAMESIZE) >> 2;
    if (o < 4) {
	s->argi = 1;
	*r = ejit_gpr_regs + EJIT_OFS_GPR_ARGS + o;
    }
    else {
	*r = NULL;
	o = f->size;
    }
    f->size += sizeof(long);
#else
    *r = NULL;
    o = f->size;
    f->size += sizeof(long);
#endif

    return (o);
}

static int
arg_f(ejit_frame_t *f, ejit_register_t **r)
{
    int		o;

#if defined(__x86_64__)
    o = f->argf;
    if (f->argf < EJIT_NUM_FPR_ARGS) {
	*r = ejit_fpr_regs + EJIT_OFS_FPR_ARGS + o;
	++f->argi;
    }
    else {
	*r = NULL;
	o = f->size;
	f->size += sizeof(double);
    }
#elif defined(__mips__)
    o = (f->size - EJIT_FRAMESIZE) >> 2;
    if (o < EJIT_NUM_GPR_ARGS) {
	if (!f->argi) {
	    if (o) {
		o = 1;
		f->argi = 1;
		*r = ejit_gpr_regs + EJIT_OFS_FPR_ARGS + 1;
	    }
	    else
		*r = ejit_gpr_regs + EJIT_OFS_FPR_ARGS;
	}
	else
	    *r = ejit_gpr_regs + EJIT_OFS_GPR_ARGS + o;
    }
    else {
	*r = NULL;
	o = f->size;
    }
    f->size += sizeof(float);
#else
    *r = NULL;
    o = f->size;
    f->size += sizeof(float);
#endif

    return (o);
}

static int
arg_d(ejit_frame_t *f, ejit_register_t **r)
{
    int		o;

#if defined(__x86_64__)
    o = f->argf;
    if (f->argf < EJIT_NUM_FPR_ARGS) {
	*r = ejit_fpr_regs + EJIT_OFS_FPR_ARGS + o;
	++f->argi;
    }
    else {
	*r = NULL;
	o = f->size;
	f->size += sizeof(double);
    }
#elif defined(__mips__)
    if (f->size & 7) {
	f->size += 4;
	f->argi = 1;
    }
    o = (f->size - EJIT_FRAMESIZE) >> 2;
    if (o < EJIT_NUM_GPR_ARGS) {
	if (!f->argi) {
	    if (o) {
		o = 1;
		f->rgi = 1;
		*r = ejit_gpr_regs + EJIT_OFS_FPR_ARGS + 1;
	    }
	    else
		*r = ejit_gpr_regs + EJIT_OFS_FPR_ARGS;
	}
	else
	    *r = ejit_gpr_regs + EJIT_OFS_GPR_ARGS + o;
    }
    else {
	*r = NULL;
	o = f->size;
    }
    f->size += sizeof(double);
#else
    *r = NULL;
    o = f->size;
    f->size += sizeof(double);
#endif

    return (o);
}

static int
redundant_jump(ejit_state_t *s, ejit_node_t *p, ejit_node_t *n)
{
    ejit_node_t	*lp, *ln;

    if (!(n->hint & EJIT_NODE_ARG))
	return (0);
    for (lp = n, ln = n->next; ln; lp = ln, ln = ln->next) {
	if (ln->code == code_label) {
	    if (n->u.n == ln) {
		if (ln->link == n) {
		    if ((ln->link = n->link) == NULL)
			node_del(s, lp, ln);
		}
		else {
		    for (lp = ln->link; lp->link != n; lp = lp->link)
			assert(lp != NULL);
		    lp->link = n->link;
		}
		node_del(s, p, n);
		return (1);
	    }
	}
	else if (ln->code != code_note)
	    break;
    }
    return (0);
}

static int
reverse_jump(ejit_state_t *s, ejit_node_t *p, ejit_node_t *n)
{
    ejit_node_t	*lp, *ln, *lc;

    if (!(n->hint & EJIT_NODE_ARG))
	return (0);
    /* =><cond_jump L0> <jump L1> <label L0> */

    ln = n->next;
    if (ln->code != code_jmpi)
	return (0);
    /* <cond_jump L0> =><jump L1> <label L0> */

    lc = ln->u.n;
    for (lp = ln, ln = ln->next; ln; lp = ln, ln = ln->next) {
	if (ln->code == code_label) {
	    if (n->u.n == ln) {
		/* <cond_jump L0> <jump L1> =><label L0> */
		if (ln->link == n) {

		    /* unlink the jump */
		    if ((ln->link = n->link) == NULL) {
			/* if next of first entry is NULL L0 cannot be L1,
			 * unless there is a dangling jump somewhere */
			assert(n->next->u.n != ln);
			node_del(s, lp, ln);
		    }
		}
		else {
		    for (lp = ln->link; lp->link != n; lp = lp->link)
			assert(lp != NULL);
		    lp->link = n->link;
		}
		node_del(s, n, n->next);

		switch (n->code) {
		    case code_bltr_i:	n->code = code_bger_i;		break;
		    case code_bltr_ui:	n->code = code_bger_ui;		break;
		    case code_bltr_l:	n->code = code_bger_l;		break;
		    case code_bltr_ul:	n->code = code_bger_ul;		break;
		    case code_bltr_p:	n->code = code_bger_p;		break;
		    case code_bltr_f:	n->code = code_bunger_f;	break;
		    case code_bltr_d:	n->code = code_bunger_d;	break;
		    case code_blti_i:	n->code = code_bgei_i;		break;
		    case code_blti_ui:	n->code = code_bgei_ui;		break;
		    case code_blti_l:	n->code = code_bgei_l;		break;
		    case code_blti_ul:	n->code = code_bgei_ul;		break;
		    case code_blti_p:	n->code = code_bgei_p;		break;
		    case code_bler_i:	n->code = code_bgtr_i;		break;
		    case code_bler_ui:	n->code = code_bgtr_ui;		break;
		    case code_bler_l:	n->code = code_bgtr_l;		break;
		    case code_bler_ul:	n->code = code_bgtr_ul;		break;
		    case code_bler_p:	n->code = code_bgtr_p;		break;
		    case code_bler_f:	n->code = code_bungtr_f;	break;
		    case code_bler_d:	n->code = code_bungtr_d;	break;
		    case code_blei_i:	n->code = code_bgti_i;		break;
		    case code_blei_ui:	n->code = code_bgti_ui;		break;
		    case code_blei_l:	n->code = code_bgti_l;		break;
		    case code_blei_ul:	n->code = code_bgti_ul;		break;
		    case code_blei_p:	n->code = code_bgti_p;		break;
		    case code_beqr_i:	n->code = code_bner_i;		break;
		    case code_beqr_ui:	n->code = code_bner_ui;		break;
		    case code_beqr_l:	n->code = code_bner_l;		break;
		    case code_beqr_ul:	n->code = code_bner_ul;		break;
		    case code_beqr_p:	n->code = code_bner_p;		break;
		    case code_beqr_f:	n->code = code_bltgtr_f;	break;
		    case code_beqr_d:	n->code = code_bltgtr_d;	break;
		    case code_beqi_i:	n->code = code_bnei_i;		break;
		    case code_beqi_ui:	n->code = code_bnei_ui;		break;
		    case code_beqi_l:	n->code = code_bnei_l;		break;
		    case code_beqi_ul:	n->code = code_bnei_ul;		break;
		    case code_beqi_p:	n->code = code_bnei_p;		break;
		    case code_bger_i:	n->code = code_bltr_i;		break;
		    case code_bger_ui:	n->code = code_bltr_ui;		break;
		    case code_bger_l:	n->code = code_bltr_l;		break;
		    case code_bger_ul:	n->code = code_bltr_ul;		break;
		    case code_bger_p:	n->code = code_bltr_p;		break;
		    case code_bger_f:	n->code = code_bunltr_f;	break;
		    case code_bger_d:	n->code = code_bunltr_d;	break;
		    case code_bgei_i:	n->code = code_blti_i;		break;
		    case code_bgei_ui:	n->code = code_blti_ui;		break;
		    case code_bgei_l:	n->code = code_blti_l;		break;
		    case code_bgei_ul:	n->code = code_blti_ul;		break;
		    case code_bgei_p:	n->code = code_blti_p;		break;
		    case code_bgtr_i:	n->code = code_bler_i;		break;
		    case code_bgtr_ui:	n->code = code_bler_ui;		break;
		    case code_bgtr_l:	n->code = code_bler_l;		break;
		    case code_bgtr_ul:	n->code = code_bler_ul;		break;
		    case code_bgtr_p:	n->code = code_bler_p;		break;
		    case code_bgtr_f:	n->code = code_bunler_f;	break;
		    case code_bgtr_d:	n->code = code_bunler_d;	break;
		    case code_bgti_i:	n->code = code_blei_i;		break;
		    case code_bgti_ui:	n->code = code_blei_ui;		break;
		    case code_bgti_l:	n->code = code_blei_l;		break;
		    case code_bgti_ul:	n->code = code_blei_ul;		break;
		    case code_bgti_p:	n->code = code_blei_p;		break;
		    case code_bner_i:	n->code = code_beqr_i;		break;
		    case code_bner_ui:	n->code = code_beqr_ui;		break;
		    case code_bner_l:	n->code = code_beqr_l;		break;
		    case code_bner_ul:	n->code = code_beqr_ul;		break;
		    case code_bner_p:	n->code = code_beqr_p;		break;
		    case code_bner_f:	n->code = code_buneqr_f;	break;
		    case code_bner_d:	n->code = code_buneqr_d;	break;
		    case code_bnei_i:	n->code = code_beqi_i;		break;
		    case code_bnei_ui:	n->code = code_beqi_ui;		break;
		    case code_bnei_l:	n->code = code_beqi_l;		break;
		    case code_bnei_ul:	n->code = code_beqi_ul;		break;
		    case code_bnei_p:	n->code = code_beqi_p;		break;
		    case code_bunltr_f:	n->code = code_bger_f;		break;
		    case code_bunltr_d:	n->code = code_bger_d;		break;
		    case code_bunler_f:	n->code = code_bgtr_f;		break;
		    case code_bunler_d:	n->code = code_bgtr_d;		break;
		    case code_buneqr_f:	n->code = code_bltgtr_f;	break;
		    case code_buneqr_d:	n->code = code_bltgtr_d;	break;
		    case code_bunger_f:	n->code = code_bltr_f;		break;
		    case code_bunger_d:	n->code = code_bltr_d;		break;
		    case code_bungtr_f:	n->code = code_bler_f;		break;
		    case code_bungtr_d:	n->code = code_bler_d;		break;
		    case code_bltgtr_f:	n->code = code_beqr_f;		break;
		    case code_bltgtr_d:	n->code = code_beqr_d;		break;
		    case code_bordr_f:	n->code = code_bunordr_f;	break;
		    case code_bordr_d:	n->code = code_bunordr_d;	break;
		    case code_bunordr_f:n->code = code_bordr_f;		break;
		    case code_bunordr_d:n->code = code_bordr_d;		break;
		    case code_bmsr_i:	n->code = code_bmcr_i;		break;
		    case code_bmsr_ui:	n->code = code_bmcr_ui;		break;
		    case code_bmsr_l:	n->code = code_bmcr_l;		break;
		    case code_bmsr_ul:	n->code = code_bmcr_ul;		break;
		    case code_bmsi_i:	n->code = code_bmci_i;		break;
		    case code_bmsi_ui:	n->code = code_bmci_ui;		break;
		    case code_bmsi_l:	n->code = code_bmci_l;		break;
		    case code_bmsi_ul:	n->code = code_bmci_ul;		break;
		    case code_bmcr_i:	n->code = code_bmsr_i;		break;
		    case code_bmcr_ui:	n->code = code_bmsr_ui;		break;
		    case code_bmcr_l:	n->code = code_bmsr_l;		break;
		    case code_bmcr_ul:	n->code = code_bmsr_ul;		break;
		    case code_bmci_i:	n->code = code_bmsi_i;		break;
		    case code_bmci_ui:	n->code = code_bmsi_ui;		break;
		    case code_bmci_l:	n->code = code_bmsi_l;		break;
		    case code_bmci_ul:	n->code = code_bmsi_ul;		break;
		    default:		abort();
		}

		/* link the jump */
		n->u.n = lc;
		n->link = lc->link;
		lc->link = n;

		return (1);
	    }
	}
	else if (ln->code != code_note)
	    break;
    }
    return (0);
}

ejit_state_t *
ejit_create_state(void)
{
    int		offset;
    static int	initialized;

    if (!initialized) {
#if defined(jit_get_cpu)
	jit_get_cpu();
#endif
	initialized = 1;

#if defined(__i386__)
	ejit_gpr_regs[0].regno = _RAX;		ejit_gpr_regs[0].name = "%eax";
	ejit_gpr_regs[1].regno = _RCX;		ejit_gpr_regs[1].name = "%ecx";
	ejit_gpr_regs[2].regno = _RDX;		ejit_gpr_regs[2].name = "%edx";
	ejit_gpr_regs[3].regno = _RBX;		ejit_gpr_regs[3].name = "%ebx";
	ejit_gpr_regs[4].regno = _RSI;		ejit_gpr_regs[4].name = "%esi";
	ejit_gpr_regs[5].regno = _RDI;		ejit_gpr_regs[5].name = "%edi";
	ejit_gpr_regs[6].regno = _RSP;		ejit_gpr_regs[6].name = "%esp";
	ejit_gpr_regs[7].regno = _RBP;		ejit_gpr_regs[7].name = "%ebp";
	if (jit_sse2_p()) {
	    ejit_fpr_regs[ 0].regno = _XMM0;	ejit_fpr_regs[ 0].name = "%xmm0";
	    ejit_fpr_regs[ 1].regno = _XMM1;	ejit_fpr_regs[ 1].name = "%xmm1";
	    ejit_fpr_regs[ 2].regno = _XMM2;	ejit_fpr_regs[ 2].name = "%xmm2";
	    ejit_fpr_regs[ 3].regno = _XMM3;	ejit_fpr_regs[ 3].name = "%xmm3";
	    ejit_fpr_regs[ 4].regno = _XMM4;	ejit_fpr_regs[ 4].name = "%xmm4";
	    ejit_fpr_regs[ 5].regno = _XMM5;	ejit_fpr_regs[ 5].name = "%xmm5";
	    ejit_fpr_regs[ 6].regno = _XMM6;	ejit_fpr_regs[ 6].name = "%xmm6";
	    ejit_fpr_regs[ 7].regno = _XMM7;	ejit_fpr_regs[ 7].name = "%xmm7";
	    ejit_fpr_regs[ 8].regno = _ST0;	ejit_fpr_regs[ 8].name = "%st0";
	    ejit_fpr_regs[ 9].regno = _ST1;	ejit_fpr_regs[ 9].name = "%st1";
	    ejit_fpr_regs[10].regno = _ST2;	ejit_fpr_regs[10].name = "%st2";
	    ejit_fpr_regs[11].regno = _ST3;	ejit_fpr_regs[11].name = "%st3";
	    ejit_fpr_regs[12].regno = _ST4;	ejit_fpr_regs[12].name = "%st4";
	    ejit_fpr_regs[13].regno = _ST5;	ejit_fpr_regs[13].name = "%st5";
	    ejit_fpr_regs[14].regno = _ST6;	ejit_fpr_regs[14].name = "%st6";
	    ejit_fpr_regs[15].regno = _ST7;	ejit_fpr_regs[15].name = "%st7";

	}
	else {
	    ejit_fpr_regs[0].regno = _ST0;	ejit_fpr_regs[0].name = "%st0";
	    ejit_fpr_regs[1].regno = _ST1;	ejit_fpr_regs[1].name = "%st1";
	    ejit_fpr_regs[2].regno = _ST2;	ejit_fpr_regs[2].name = "%st2";
	    ejit_fpr_regs[3].regno = _ST3;	ejit_fpr_regs[3].name = "%st3";
	    ejit_fpr_regs[4].regno = _ST4;	ejit_fpr_regs[4].name = "%st4";
	    ejit_fpr_regs[5].regno = _ST5;	ejit_fpr_regs[5].name = "%st5";
	    ejit_fpr_regs[5].regno = _ST6;	ejit_fpr_regs[6].name = "%st6";
	    ejit_fpr_regs[6].regno = _ST7;	ejit_fpr_regs[7].name = "%st7";
	}
#elif defined(__x86_64__)
	ejit_gpr_regs[ 0].regno = _RAX;		ejit_gpr_regs[ 0].name = "%rax";
	ejit_gpr_regs[ 1].regno = _R10;		ejit_gpr_regs[ 1].name = "%r10";
	ejit_gpr_regs[ 2].regno = _R11;		ejit_gpr_regs[ 2].name = "%r11";
	ejit_gpr_regs[ 3].regno = _RBX;		ejit_gpr_regs[ 3].name = "%rbx";
	ejit_gpr_regs[ 4].regno = _R13;		ejit_gpr_regs[ 4].name = "%r13";
	ejit_gpr_regs[ 5].regno = _R14;		ejit_gpr_regs[ 5].name = "%r14";
	ejit_gpr_regs[ 6].regno = _R15;		ejit_gpr_regs[ 6].name = "%r15";
	ejit_gpr_regs[ 7].regno = _RDI;		ejit_gpr_regs[ 7].name = "%rdi";
	ejit_gpr_regs[ 8].regno = _RSI;		ejit_gpr_regs[ 8].name = "%rsi";
	ejit_gpr_regs[ 9].regno = _RDX;		ejit_gpr_regs[ 9].name = "%rdx";
	ejit_gpr_regs[10].regno = _RCX;		ejit_gpr_regs[10].name = "%rcx";
	ejit_gpr_regs[11].regno = _R8;		ejit_gpr_regs[11].name = "%r8";
	ejit_gpr_regs[12].regno = _R9;		ejit_gpr_regs[12].name = "%r9";
	ejit_gpr_regs[13].regno = _RSP;		ejit_gpr_regs[13].name = "%rsp";
	ejit_gpr_regs[14].regno = _RBP;		ejit_gpr_regs[14].name = "%rbp";
	ejit_gpr_regs[15].regno = _R12;		ejit_gpr_regs[15].name = "%r12";
	ejit_fpr_regs[ 0].regno = _XMM8;	ejit_fpr_regs[ 0].name = "%xmm0";
	ejit_fpr_regs[ 1].regno = _XMM9;	ejit_fpr_regs[ 1].name = "%xmm1";
	ejit_fpr_regs[ 2].regno = _XMM10;	ejit_fpr_regs[ 2].name = "%xmm2";
	ejit_fpr_regs[ 3].regno = _XMM11;	ejit_fpr_regs[ 3].name = "%xmm3";
	ejit_fpr_regs[ 4].regno = _XMM12;	ejit_fpr_regs[ 4].name = "%xmm4";
	ejit_fpr_regs[ 5].regno = _XMM13;	ejit_fpr_regs[ 5].name = "%xmm5";
	ejit_fpr_regs[ 6].regno = _XMM0;	ejit_fpr_regs[ 6].name = "%xmm6";
	ejit_fpr_regs[ 7].regno = _XMM1;	ejit_fpr_regs[ 7].name = "%xmm7";
	ejit_fpr_regs[ 8].regno = _XMM2;	ejit_fpr_regs[ 8].name = "%xmm8";
	ejit_fpr_regs[ 9].regno = _XMM3;	ejit_fpr_regs[ 9].name = "%xmm9";
	ejit_fpr_regs[10].regno = _XMM4;	ejit_fpr_regs[10].name = "%xmm10";
	ejit_fpr_regs[11].regno = _XMM5;	ejit_fpr_regs[11].name = "%xmm11";
	ejit_fpr_regs[12].regno = _XMM6;	ejit_fpr_regs[12].name = "%xmm12";
	ejit_fpr_regs[13].regno = _XMM7;	ejit_fpr_regs[13].name = "%xmm13";
	ejit_fpr_regs[14].regno = _XMM14;	ejit_fpr_regs[14].name = "%xmm14";
	ejit_fpr_regs[15].regno = _XMM15;	ejit_fpr_regs[15].name = "%xmm15";
#elif defined(__mips__)
	ejit_gpr_regs[ 0].regno = _V0;		ejit_gpr_regs[ 0].name = "v0";
	ejit_gpr_regs[ 1].regno = _V1;		ejit_gpr_regs[ 1].name = "v1";
	ejit_gpr_regs[ 2].regno = _T0;		ejit_gpr_regs[ 2].name = "t0";
	ejit_gpr_regs[ 3].regno = _T1;		ejit_gpr_regs[ 3].name = "t1";
	ejit_gpr_regs[ 4].regno = _T2;		ejit_gpr_regs[ 4].name = "t2";
	ejit_gpr_regs[ 5].regno = _T3;		ejit_gpr_regs[ 5].name = "t3";
	ejit_gpr_regs[ 6].regno = _T4;		ejit_gpr_regs[ 6].name = "t4";
	ejit_gpr_regs[ 7].regno = _T5;		ejit_gpr_regs[ 7].name = "t5";
	ejit_gpr_regs[ 8].regno = _T6;		ejit_gpr_regs[ 8].name = "t6";
	ejit_gpr_regs[ 9].regno = _S0;		ejit_gpr_regs[ 9].name = "s0";
	ejit_gpr_regs[10].regno = _S1;		ejit_gpr_regs[10].name = "s1";
	ejit_gpr_regs[11].regno = _S2;		ejit_gpr_regs[11].name = "s2";
	ejit_gpr_regs[12].regno = _S3;		ejit_gpr_regs[12].name = "s3";
	ejit_gpr_regs[13].regno = _S4;		ejit_gpr_regs[13].name = "s4";
	ejit_gpr_regs[14].regno = _S5;		ejit_gpr_regs[14].name = "s5";
	ejit_gpr_regs[15].regno = _S6;		ejit_gpr_regs[15].name = "s6";
	ejit_gpr_regs[16].regno = _S7;		ejit_gpr_regs[16].name = "s7";
	ejit_gpr_regs[17].regno = _A0;		ejit_gpr_regs[17].name = "a0";
	ejit_gpr_regs[18].regno = _A1;		ejit_gpr_regs[18].name = "a1";
	ejit_gpr_regs[19].regno = _A2;		ejit_gpr_regs[19].name = "a2";
	ejit_gpr_regs[20].regno = _A3;		ejit_gpr_regs[20].name = "a3";
	ejit_gpr_regs[21].regno = _SP;		ejit_gpr_regs[21].name = "sp";
	ejit_gpr_regs[22].regno = _FP;		ejit_gpr_regs[22].name = "fp";
	ejit_gpr_regs[23].regno = _ZERO;	ejit_gpr_regs[23].name = "zero";
	ejit_gpr_regs[24].regno = _AT;		ejit_gpr_regs[24].name = "at";
	ejit_gpr_regs[25].regno = _T7;		ejit_gpr_regs[25].name = "t7";
	ejit_gpr_regs[26].regno = _T8;		ejit_gpr_regs[26].name = "t8";
	ejit_gpr_regs[27].regno = _T9;		ejit_gpr_regs[27].name = "t9";
	ejit_gpr_regs[28].regno = _K0;		ejit_gpr_regs[28].name = "k0";
	ejit_gpr_regs[29].regno = _K1;		ejit_gpr_regs[29].name = "k1";
	ejit_gpr_regs[30].regno = _GP;		ejit_gpr_regs[30].name = "gp";
	ejit_gpr_regs[31].regno = _RA;		ejit_gpr_regs[31].name = "ra";
	ejit_fpr_regs[ 0].regno = _F0;		ejit_fpr_regs[ 0].name = "$f0";
	ejit_fpr_regs[ 1].regno = _F2;		ejit_fpr_regs[ 1].name = "$f2";
	ejit_fpr_regs[ 2].regno = _F4;		ejit_fpr_regs[ 2].name = "$f4";
	ejit_fpr_regs[ 3].regno = _F6;		ejit_fpr_regs[ 3].name = "$f6";
	ejit_fpr_regs[ 4].regno = _F8;		ejit_fpr_regs[ 4].name = "$f8";
	ejit_fpr_regs[ 5].regno = _F10;		ejit_fpr_regs[ 5].name = "$f10";
	ejit_fpr_regs[ 6].regno = _F16;		ejit_fpr_regs[ 6].name = "$f16";
	ejit_fpr_regs[ 7].regno = _F18;		ejit_fpr_regs[ 7].name = "$f18";
	ejit_fpr_regs[ 8].regno = _F20;		ejit_fpr_regs[ 8].name = "$f20";
	ejit_fpr_regs[ 9].regno = _F22;		ejit_fpr_regs[ 9].name = "$f22";
	ejit_fpr_regs[10].regno = _F24;		ejit_fpr_regs[10].name = "$f24";
	ejit_fpr_regs[11].regno = _F26;		ejit_fpr_regs[11].name = "$f26";
	ejit_fpr_regs[12].regno = _F12;		ejit_fpr_regs[12].name = "$f12";
	ejit_fpr_regs[13].regno = _F14;		ejit_fpr_regs[13].name = "$f14";
	ejit_fpr_regs[14].regno = _F28;		ejit_fpr_regs[14].name = "$f28";
	ejit_fpr_regs[15].regno = _F30;		ejit_fpr_regs[15].name = "$f30";
#endif

	for (offset = 0; offset < EJIT_NUM_GPR_SAVE; offset++)
	    ejit_gpr_regs[offset + EJIT_OFS_GPR_SAVE].issav = 1;
	for (offset = 0; offset < EJIT_NUM_GPR_ARGS; offset++)
	    ejit_gpr_regs[offset + EJIT_OFS_GPR_ARGS].isarg = 1;

	for (offset = 0; offset < EJIT_NUM_HARD_FPR_REGS; offset++)
	    ejit_fpr_regs[offset].isflt = 1;
	for (offset = 0; offset < EJIT_NUM_FPR_SAVE; offset++)
	    ejit_fpr_regs[offset + EJIT_OFS_FPR_SAVE].issav = 1;
	for (offset = 0; offset < EJIT_NUM_FPR_ARGS; offset++)
	    ejit_fpr_regs[offset + EJIT_OFS_FPR_ARGS].isarg = 1;
    }

    return (calloc(1, sizeof(ejit_state_t)));
}

void
ejit_patch(ejit_state_t *s, ejit_node_t *label, ejit_node_t *instr)
{
    instr->hint |= EJIT_NODE_ARG;
    assert(label->code == code_label);
    switch (instr->code) {
	case code_movi_p:
	    instr->v.n = label;
	    break;
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
	    instr->u.n = label;
	    break;
	default:
	    fprintf(stderr, "unsupported patch\n");
	    abort();
    }
    /* link field is used as list of nodes to be patched
     * at actual jit generation */
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

ejit_node_t *
ejit_prolog(ejit_state_t *s)
{
    ejit_frame_t	*f = frame_new();
    ejit_node_t		*n = node_new(code_prolog);
    n = node_link(s, n);
    f->next = s->prolog;
    f->node = n;
    f->size = EJIT_FRAMESIZE;
    s->prolog = f;
    /* FIXME add appropriate nodes to callee saved registers and alloca */
    ejit_subi_p(s, EJIT_SP, EJIT_SP, (void *)EJIT_FRAMESIZE);
    ejit_movr_p(s, EJIT_FP, EJIT_SP);
    return (n);
}

int
ejit_arg_i(ejit_state_t *s, ejit_register_t **r)
{
    return (arg_i(s->prolog, r));
}

int
ejit_arg_f(ejit_state_t *s, ejit_register_t **r)
{
    return (arg_f(s->prolog, r));
}

int
ejit_arg_d(ejit_state_t *s, ejit_register_t **r)
{
    return (arg_d(s->prolog, r));
}

ejit_node_t *
ejit_epilog(ejit_state_t *s)
{
    ejit_frame_t	*f = s->prolog->next;
    ejit_node_t		*n = node_new(code_epilog);
    n = node_link(s, n);
    ejit_movr_p(s, EJIT_SP, EJIT_FP);
    /* FIXME reload callee save registers */
    ejit_addi_p(s, EJIT_SP, EJIT_SP, (void *)EJIT_FRAMESIZE);
    /* FIXME add architecture specific ret instruction when emiting code */
    free(s->prolog);
    s->prolog = f;
    return (n);
}

ejit_node_t *
ejit_prepare(ejit_state_t *s)
{
    ejit_frame_t	*f = frame_new();
    ejit_node_t		*n = node_new(code_prepare);

    n = node_link(s, n);
    f->next = s->prepare;
    f->node = n;
    s->prepare = f;

    return (n);
}

ejit_node_t *
ejit_pusharg(ejit_state_t *s, ejit_code_t c, int u)
{
    ejit_register_t	*r;
    ejit_node_t		*n = node_new(c);

    n->u.i = u;
    switch (c) {
	case code_pusharg_f:	(void)arg_f(s->prepare, &r);	break;
	case code_pusharg_d:	(void)arg_d(s->prepare, &r);	break;
	default:		(void)arg_i(s->prepare, &r);	break;
    }

    return (node_link(s, n));
}

ejit_node_t *
ejit_finish(ejit_state_t *s, void *u)
{
    ejit_frame_t	*f = s->prepare->next;
    ejit_node_t		*n = node_new(code_finish);
    n->u.p = u;
    free(s->prepare);
    s->prepare = f;
    return (n = node_link(s, n));
}

int
ejit_optimize(ejit_state_t *s)
{
    int		 change;
    ejit_node_t	*p, *c, *n;

    change = 0;
    for (p = c = s->head; c;) {
	n = c->next;
	switch (c->code) {
	    case code_note:		case code_label:
	    case code_addr_i:		case code_addr_ui:
	    case code_addr_l:		case code_addr_ul:
	    case code_addr_p:		case code_addr_f:
	    case code_addr_d:		case code_addi_i:
	    case code_addi_ui:		case code_addi_l:
	    case code_addi_ul:		case code_addi_p:
	    case code_addxr_ui:		case code_addxr_ul:
	    case code_addxi_ui:		case code_addxi_ul:
	    case code_addcr_ui:		case code_addcr_ul:
	    case code_addci_ui:		case code_addci_ul:
	    case code_subr_i:		case code_subr_ui:
	    case code_subr_l:		case code_subr_ul:
	    case code_subr_p:		case code_subr_f:
	    case code_subr_d:		case code_subi_i:
	    case code_subi_ui:		case code_subi_l:
	    case code_subi_ul:		case code_subi_p:
	    case code_subxr_ui:		case code_subxr_ul:
	    case code_subxi_ui:		case code_subxi_ul:
	    case code_subcr_ui:		case code_subcr_ul:
	    case code_subci_ui:		case code_subci_ul:
	    case code_rsbr_i:		case code_rsbr_ui:
	    case code_rsbr_l:		case code_rsbr_ul:
	    case code_rsbr_p:		case code_rsbr_f:
	    case code_rsbr_d:		case code_rsbi_i:
	    case code_rsbi_ui:		case code_rsbi_l:
	    case code_rsbi_ul:		case code_rsbi_p:
	    case code_mulr_i:		case code_mulr_ui:
	    case code_mulr_l:		case code_mulr_ul:
	    case code_mulr_f:		case code_mulr_d:
	    case code_muli_i:		case code_muli_ui:
	    case code_muli_l:		case code_muli_ul:
	    case code_hmulr_i:		case code_hmulr_ui:
	    case code_hmulr_l:		case code_hmulr_ul:
	    case code_hmuli_i:		case code_hmuli_ui:
	    case code_hmuli_l:		case code_hmuli_ul:
	    case code_divr_i:		case code_divr_ui:
	    case code_divr_l:		case code_divr_ul:
	    case code_divr_f:		case code_divr_d:
	    case code_divi_i:		case code_divi_ui:
	    case code_divi_l:		case code_divi_ul:
	    case code_modr_i:		case code_modr_ui:
	    case code_modr_l:		case code_modr_ul:
	    case code_modi_i:		case code_modi_ui:
	    case code_modi_l:		case code_modi_ul:
	    case code_andr_i:		case code_andr_ui:
	    case code_andr_l:		case code_andr_ul:
	    case code_andi_i:		case code_andi_ui:
	    case code_andi_l:		case code_andi_ul:
	    case code_orr_i:		case code_orr_ui:
	    case code_orr_l:		case code_orr_ul:
	    case code_ori_i:		case code_ori_ui:
	    case code_ori_l:		case code_ori_ul:
	    case code_xorr_i:		case code_xorr_ui:
	    case code_xorr_l:		case code_xorr_ul:
	    case code_xori_i:		case code_xori_ui:
	    case code_xori_l:		case code_xori_ul:
	    case code_lshr_i:		case code_lshr_ui:
	    case code_lshr_l:		case code_lshr_ul:
	    case code_lshi_i:		case code_lshi_ui:
	    case code_lshi_l:		case code_lshi_ul:
	    case code_rshr_i:		case code_rshr_ui:
	    case code_rshr_l:		case code_rshr_ul:
	    case code_rshi_i:		case code_rshi_ui:
	    case code_rshi_l:		case code_rshi_ul:
	    case code_absr_f:		case code_absr_d:
	    case code_negr_i:		case code_negr_l:
	    case code_negr_f:		case code_negr_d:
	    case code_notr_i:		case code_notr_l:
	    case code_ltr_i:		case code_ltr_ui:
	    case code_ltr_l:		case code_ltr_ul:
	    case code_ltr_p:		case code_ltr_f:
	    case code_ltr_d:		case code_lti_i:
	    case code_lti_ui:		case code_lti_l:
	    case code_lti_ul:		case code_lti_p:
	    case code_ler_i:		case code_ler_ui:
	    case code_ler_l:		case code_ler_ul:
	    case code_ler_p:		case code_ler_f:
	    case code_ler_d:		case code_lei_i:
	    case code_lei_ui:		case code_lei_l:
	    case code_lei_ul:		case code_lei_p:
	    case code_eqr_i:		case code_eqr_ui:
	    case code_eqr_l:		case code_eqr_ul:
	    case code_eqr_p:		case code_eqr_f:
	    case code_eqr_d:		case code_eqi_i:
	    case code_eqi_ui:		case code_eqi_l:
	    case code_eqi_ul:		case code_eqi_p:
	    case code_ger_i:		case code_ger_ui:
	    case code_ger_l:		case code_ger_ul:
	    case code_ger_p:		case code_ger_f:
	    case code_ger_d:		case code_gei_i:
	    case code_gei_ui:		case code_gei_l:
	    case code_gei_ul:		case code_gei_p:
	    case code_gtr_i:		case code_gtr_ui:
	    case code_gtr_l:		case code_gtr_ul:
	    case code_gtr_p:		case code_gtr_f:
	    case code_gtr_d:		case code_gti_i:
	    case code_gti_ui:		case code_gti_l:
	    case code_gti_ul:		case code_gti_p:
	    case code_ner_i:		case code_ner_ui:
	    case code_ner_l:		case code_ner_ul:
	    case code_ner_p:		case code_ner_f:
	    case code_ner_d:		case code_nei_i:
	    case code_nei_ui:		case code_nei_l:
	    case code_nei_ul:		case code_nei_p:
	    case code_unltr_f:		case code_unltr_d:
	    case code_unler_f:		case code_unler_d:
	    case code_uneqr_f:		case code_uneqr_d:
	    case code_unger_f:		case code_unger_d:
	    case code_ungtr_f:		case code_ungtr_d:
	    case code_ltgtr_f:		case code_ltgtr_d:
	    case code_ordr_f:		case code_ordr_d:
	    case code_unordr_f:		case code_unordr_d:
	    case code_movr_i:		case code_movr_ui:
	    case code_movr_l:		case code_movr_ul:
	    case code_movr_p:		case code_movr_f:
	    case code_movr_d:		case code_movi_i:
	    case code_movi_ui:		case code_movi_l:
	    case code_movi_ul:		case code_movi_p:
	    case code_movi_f:		case code_movi_d:
	    case code_extr_c_i:		case code_extr_uc_ui:
	    case code_extr_s_i:		case code_extr_us_ui:
	    case code_extr_c_l:		case code_extr_uc_ul:
	    case code_extr_s_l:		case code_extr_us_ul:
	    case code_extr_i_l:		case code_extr_ui_ul:
	    case code_extr_i_f:		case code_extr_i_d:
	    case code_extr_l_f:		case code_extr_l_d:
	    case code_extr_f_d:		case code_extr_d_f:
	    case code_movr_i_f:		case code_movr_l_d:
	    case code_movr_f_i:		case code_movr_d_l:
	    case code_roundr_f_i:	case code_roundr_f_l:
	    case code_roundr_d_i:	case code_roundr_d_l:
	    case code_truncr_f_i:	case code_truncr_f_l:
	    case code_truncr_d_i:	case code_truncr_d_l:
	    case code_floorr_f_i:	case code_floorr_f_l:
	    case code_floorr_d_i:	case code_floorr_d_l:
	    case code_ceilr_f_i:	case code_ceilr_f_l:
	    case code_ceilr_d_i:	case code_ceilr_d_l:
	    case code_hton_us_ui:	case code_ntoh_us_ui:
	    case code_ldr_c:		case code_ldr_uc:
	    case code_ldr_s:		case code_ldr_us:
	    case code_ldr_i:		case code_ldr_ui:
	    case code_ldr_l:		case code_ldr_ul:
	    case code_ldr_p:		case code_ldr_f:
	    case code_ldr_d:		case code_ldi_c:
	    case code_ldi_uc:		case code_ldi_s:
	    case code_ldi_us:		case code_ldi_i:
	    case code_ldi_ui:		case code_ldi_l:
	    case code_ldi_ul:		case code_ldi_p:
	    case code_ldi_f:		case code_ldi_d:
	    case code_ldxr_c:		case code_ldxr_uc:
	    case code_ldxr_s:		case code_ldxr_us:
	    case code_ldxr_i:		case code_ldxr_ui:
	    case code_ldxr_l:		case code_ldxr_ul:
	    case code_ldxr_p:		case code_ldxr_f:
	    case code_ldxr_d:		case code_ldxi_c:
	    case code_ldxi_uc:		case code_ldxi_s:
	    case code_ldxi_us:		case code_ldxi_i:
	    case code_ldxi_ui:		case code_ldxi_l:
	    case code_ldxi_ul:		case code_ldxi_p:
	    case code_ldxi_f:		case code_ldxi_d:
	    case code_str_c:		case code_str_uc:
	    case code_str_s:		case code_str_us:
	    case code_str_i:		case code_str_ui:
	    case code_str_l:		case code_str_ul:
	    case code_str_p:		case code_str_f:
	    case code_str_d:		case code_sti_c:
	    case code_sti_uc:		case code_sti_s:
	    case code_sti_us:		case code_sti_i:
	    case code_sti_ui:		case code_sti_l:
	    case code_sti_ul:		case code_sti_p:
	    case code_sti_f:		case code_sti_d:
	    case code_stxr_c:		case code_stxr_uc:
	    case code_stxr_s:		case code_stxr_us:
	    case code_stxr_i:		case code_stxr_ui:
	    case code_stxr_l:		case code_stxr_ul:
	    case code_stxr_p:		case code_stxr_f:
	    case code_stxr_d:		case code_stxi_c:
	    case code_stxi_uc:		case code_stxi_s:
	    case code_stxi_us:		case code_stxi_i:
	    case code_stxi_ui:		case code_stxi_l:
	    case code_stxi_ul:		case code_stxi_p:
	    case code_stxi_f:		case code_stxi_d:
	    case code_prepare:		case code_pusharg_c:
	    case code_pusharg_uc:	case code_pusharg_s:
	    case code_pusharg_us:	case code_pusharg_i:
	    case code_pusharg_ui:	case code_pusharg_l:
	    case code_pusharg_ul:	case code_pusharg_p:
	    case code_pusharg_f:	case code_pusharg_d:
	    case code_getarg_c:		case code_getarg_uc:
	    case code_getarg_s:		case code_getarg_us:
	    case code_getarg_i:		case code_getarg_ui:
	    case code_getarg_l:		case code_getarg_ul:
	    case code_getarg_p:		case code_getarg_f:
	    case code_getarg_d:		case code_retval_c:
	    case code_retval_uc:	case code_retval_s:
	    case code_retval_us:	case code_retval_i:
	    case code_retval_ui:	case code_retval_l:
	    case code_retval_ul:	case code_retval_p:
	    case code_retval_f:		case code_retval_d:
		p = c;
		c = n;
		break;

	    case code_bltr_i:		case code_bltr_ui:
	    case code_bltr_l:		case code_bltr_ul:
	    case code_bltr_p:		case code_bltr_f:
	    case code_bltr_d:		case code_blti_i:
	    case code_blti_ui:		case code_blti_l:
	    case code_blti_ul:		case code_blti_p:
	    case code_bler_i:		case code_bler_ui:
	    case code_bler_l:		case code_bler_ul:
	    case code_bler_p:		case code_bler_f:
	    case code_bler_d:		case code_blei_i:
	    case code_blei_ui:		case code_blei_l:
	    case code_blei_ul:		case code_blei_p:
	    case code_beqr_i:		case code_beqr_ui:
	    case code_beqr_l:		case code_beqr_ul:
	    case code_beqr_p:		case code_beqr_f:
	    case code_beqr_d:		case code_beqi_i:
	    case code_beqi_ui:		case code_beqi_l:
	    case code_beqi_ul:		case code_beqi_p:
	    case code_bger_i:		case code_bger_ui:
	    case code_bger_l:		case code_bger_ul:
	    case code_bger_p:		case code_bger_f:
	    case code_bger_d:		case code_bgei_i:
	    case code_bgei_ui:		case code_bgei_l:
	    case code_bgei_ul:		case code_bgei_p:
	    case code_bgtr_i:		case code_bgtr_ui:
	    case code_bgtr_l:		case code_bgtr_ul:
	    case code_bgtr_p:		case code_bgtr_f:
	    case code_bgtr_d:		case code_bgti_i:
	    case code_bgti_ui:		case code_bgti_l:
	    case code_bgti_ul:		case code_bgti_p:
	    case code_bner_i:		case code_bner_ui:
	    case code_bner_l:		case code_bner_ul:
	    case code_bner_p:		case code_bner_f:
	    case code_bner_d:		case code_bnei_i:
	    case code_bnei_ui:		case code_bnei_l:
	    case code_bnei_ul:		case code_bnei_p:
	    case code_bunltr_f:		case code_bunltr_d:
	    case code_bunler_f:		case code_bunler_d:
	    case code_buneqr_f:		case code_buneqr_d:
	    case code_bunger_f:		case code_bunger_d:
	    case code_bungtr_f:		case code_bungtr_d:
	    case code_bltgtr_f:		case code_bltgtr_d:
	    case code_bordr_f:		case code_bordr_d:
	    case code_bunordr_f:	case code_bunordr_d:
	    case code_bmsr_i:		case code_bmsr_ui:
	    case code_bmsr_l:		case code_bmsr_ul:
	    case code_bmsi_i:		case code_bmsi_ui:
	    case code_bmsi_l:		case code_bmsi_ul:
	    case code_bmcr_i:		case code_bmcr_ui:
	    case code_bmcr_l:		case code_bmcr_ul:
	    case code_bmci_i:		case code_bmci_ui:
	    case code_bmci_l:		case code_bmci_ul:
		if (redundant_jump(s, p, c)) {
		    /* remove c */
		    change = 1;
		    p = c;
		    c = n;
		}
		else if (reverse_jump(s, p, c))
		    /* remove n */
		    change = 1;
		else {
		    p = c;
		    c = n;
		}
		break;

	    case code_boaddr_i:
	    case code_boaddr_ui:
	    case code_boaddr_l:
	    case code_boaddr_ul:
	    case code_boaddi_i:
	    case code_boaddi_ui:
	    case code_boaddi_l:
	    case code_boaddi_ul:
	    case code_bosubr_i:
	    case code_bosubr_ui:
	    case code_bosubr_l:
	    case code_bosubr_ul:
	    case code_bosubi_i:
	    case code_bosubi_ui:
	    case code_bosubi_l:
	    case code_bosubi_ul:
		/* if jump is redundant, must convert to {add,sub}x_y
		 * because of side effects */

	    case code_finish:
	    case code_finishr:
	    case code_calli:
	    case code_callr:
		p = c;
		c = n;
		break;

	    case code_jmpi:
		if (redundant_jump(s, p, c))
		    /* remove c */
		    change = 1;
		else
		    p = c;
		c = n;
		break;

	    case code_jmpr:
	    case code_prolog:
	    case code_allocai:
	    case code_epilog:
		p = c;
		c = n;
		break;
	    default:
		abort();
	}
    }

    return (change);
}

static char *
is(int reg)
{
    if (reg >= 0 && reg < EJIT_NUM_HARD_GPR_REGS)
	return (ejit_gpr_regs[reg].name);
    return ("<NOREG>");
}

static char *
fs(int reg)
{
    if (reg >= 0 && reg < EJIT_NUM_HARD_FPR_REGS)
	return (ejit_fpr_regs[reg].name);
    return ("<NOREG>");
}

void
ejit_print(ejit_state_t *s)
{
    ejit_node_t	*n = s->head;
    for (; n; n = n->next) {
	printf("\n\t");
	switch (n->code) {
	ir:
	    printf(" %s", is(n->u.i));				continue;
	fr:
	    printf(" %s", fs(n->u.i));				continue;
	i:
	    printf(" %d", n->u.i);				continue;
	p:
	    printf(" %p", n->u.p);				continue;
	n:
	    printf(" %p", n);					continue;
	ir_i:
	    printf(" %s %d", is(n->u.i), n->v.i);		continue;
	ir_ui:
	    printf(" %s %u", is(n->u.i), n->v.ui);		continue;
	ir_l:
	    printf(" %s %ld", is(n->u.i), n->v.l);		continue;
	ir_ul:
	    printf(" %s %lu", is(n->u.i), n->v.ul);		continue;
	ir_p:
	    printf(" %s %p", is(n->u.i), n->v.p);		continue;
	ir_ir:
	    printf(" %s %s", is(n->u.i), is(n->v.i));		continue;
	ir_fr:
	    printf(" %s %s", is(n->u.i), fs(n->v.i));		continue;
	fr_f:
	    printf(" %s %f", fs(n->u.i), (double)n->v.f);	continue;
	fr_d:
	    printf(" %s %f", fs(n->u.i), n->v.d);		continue;
	fr_p:
	    printf(" %s %p", fs(n->u.i), n->v.p);		continue;
	fr_ir:
	    printf(" %s %s", fs(n->u.i), is(n->v.i));		continue;
	fr_fr:
	    printf(" %s %s", fs(n->u.i), fs(n->v.i));		continue;
	p_ir:
	    printf(" %p %s", n->u.p, is(n->v.i));		continue;
	p_fr:
	    printf(" %p %s", n->u.p, fs(n->v.i));		continue;
	ir_ir_i:
	    printf(" %s %s %d", is(n->u.i), is(n->v.i), n->w.i);
	    continue;
	ir_ir_ui:
	    printf(" %s %s %u", is(n->u.i), is(n->v.i), n->w.ui);
	    continue;
	ir_ir_l:
	    printf(" %s %s %ld", is(n->u.i), is(n->v.i), n->w.l);
	    continue;
	ir_ir_ul:
	    printf(" %s %s %lu", is(n->u.i), is(n->v.i), n->w.ul);
	    continue;
	ir_ir_p:
	    printf(" %s %s %p", is(n->u.i), is(n->v.i), n->w.p);
	    continue;
	ir_ir_ir:
	    printf(" %s %s %s", is(n->u.i), is(n->v.i), is(n->w.i));
	    continue;
	ir_ir_fr:
	    printf(" %s %s %s", is(n->u.i), is(n->v.i), fs(n->w.i));
	    continue;
	ir_fr_fr:
	    printf(" %s %s %s", is(n->u.i), fs(n->v.i), fs(n->w.i));
	    continue;
	fr_ir_l:
	    printf(" %s %s %ld", fs(n->u.i), is(n->v.i), n->w.l);
	    continue;
	fr_fr_fr:
	    printf(" %s %s %s", fs(n->u.i), fs(n->v.i), fs(n->w.i));
	    continue;
	l_ir_ir:
	    printf(" %ld %s %s", n->u.l, is(n->v.i), is(n->w.i));
	    continue;
	l_ir_fr:
	    printf(" %ld %s %s", n->u.l, is(n->v.i), fs(n->w.i));
	    continue;
	p_ir_ir:
	    printf(" %p %s %s", n->u.p, is(n->v.i), is(n->w.i));
	    continue;
	p_ir_i:
	    printf(" %p %s %d", n->u.p, is(n->v.i), n->w.i);	continue;
	p_ir_ui:
	    printf(" %p %s %u", n->u.p, is(n->v.i), n->w.ui);	continue;
	p_ir_l:
	    printf(" %p %s %ld", n->u.p, is(n->v.i), n->w.l);	continue;
	p_ir_ul:
	    printf(" %p %s %lu", n->u.p, is(n->v.i), n->w.ul);	continue;
	p_ir_p:
	    printf(" %p %s %p", n->u.p, is(n->v.i), n->w.p);	continue;
	p_fr_fr:
	    printf(" %p %s %s", n->u.p, fs(n->v.i), fs(n->w.i));
	    continue;
	    case code_note:		printf("note");		break;
	    case code_label:		printf("label");	goto n;
	    case code_addr_i:		printf("addr_i");	goto ir_ir_ir;
	    case code_addr_ui:		printf("addr_ui");	goto ir_ir_ir;
	    case code_addr_l:		printf("addr_l");	goto ir_ir_ir;
	    case code_addr_ul:		printf("addr_ul");	goto ir_ir_ir;
	    case code_addr_p:		printf("addr_p");	goto ir_ir_ir;
	    case code_addr_f:		printf("addr_f");	goto fr_fr_fr;
	    case code_addr_d:		printf("addr_d");	goto fr_fr_fr;
	    case code_addi_i:		printf("addi_i");	goto ir_ir_i;
	    case code_addi_ui:		printf("addi_ui");	goto ir_ir_ui;
	    case code_addi_l:		printf("addi_l");	goto ir_ir_l;
	    case code_addi_ul:		printf("addi_ul");	goto ir_ir_ul;
	    case code_addi_p:		printf("addi_p");	goto ir_ir_p;
	    case code_addxr_ui:		printf("addxr_ui");	goto ir_ir_ir;
	    case code_addxr_ul:		printf("addxr_ul");	goto ir_ir_ir;
	    case code_addxi_ui:		printf("addxi_ui");	goto ir_ir_ui;
	    case code_addxi_ul:		printf("addxi_ul");	goto ir_ir_ul;
	    case code_addcr_ui:		printf("addcr_ui");	goto ir_ir_ir;
	    case code_addcr_ul:		printf("addcr_ul");	goto ir_ir_ir;
	    case code_addci_ui:		printf("addci_ui");	goto ir_ir_ui;
	    case code_addci_ul:		printf("addci_ul");	goto ir_ir_ul;
	    case code_subr_i:		printf("subr_i");	goto ir_ir_ir;
	    case code_subr_ui:		printf("subr_ui");	goto ir_ir_ir;
	    case code_subr_l:		printf("subr_l");	goto ir_ir_ir;
	    case code_subr_ul:		printf("subr_ul");	goto ir_ir_ir;
	    case code_subr_p:		printf("subr_p");	goto ir_ir_ir;
	    case code_subr_f:		printf("subr_f");	goto fr_fr_fr;
	    case code_subr_d:		printf("subr_d");	goto fr_fr_fr;
	    case code_subi_i:		printf("subi_i");	goto ir_ir_i;
	    case code_subi_ui:		printf("subi_ui");	goto ir_ir_ui;
	    case code_subi_l:		printf("subi_l");	goto ir_ir_l;
	    case code_subi_ul:		printf("subi_ul");	goto ir_ir_ul;
	    case code_subi_p:		printf("subi_p");	goto ir_ir_p;
	    case code_subxr_ui:		printf("subxr_ui");	goto ir_ir_ir;
	    case code_subxr_ul:		printf("subxr_ul");	goto ir_ir_ir;
	    case code_subxi_ui:		printf("subxi_ui");	goto ir_ir_ui;
	    case code_subxi_ul:		printf("subxi_ul");	goto ir_ir_ul;
	    case code_subcr_ui:		printf("subcr_ui");	goto ir_ir_ir;
	    case code_subcr_ul:		printf("subcr_ul");	goto ir_ir_ir;
	    case code_subci_ui:		printf("subci_ui");	goto ir_ir_ui;
	    case code_subci_ul:		printf("subci_ul");	goto ir_ir_ul;
	    case code_rsbr_i:		printf("rsbr_i");	goto ir_ir_ir;
	    case code_rsbr_ui:		printf("rsbr_ui");	goto ir_ir_ir;
	    case code_rsbr_l:		printf("rsbr_l");	goto ir_ir_ir;
	    case code_rsbr_ul:		printf("rsbr_ul");	goto ir_ir_ir;
	    case code_rsbr_p:		printf("rsbr_p");	goto ir_ir_ir;
	    case code_rsbr_f:		printf("rsbr_f");	goto fr_fr_fr;
	    case code_rsbr_d:		printf("rsbr_d");	goto fr_fr_fr;
	    case code_rsbi_i:		printf("rsbi_i");	goto ir_ir_i;
	    case code_rsbi_ui:		printf("rsbi_ui");	goto ir_ir_ui;
	    case code_rsbi_l:		printf("rsbi_l");	goto ir_ir_l;
	    case code_rsbi_ul:		printf("rsbi_ul");	goto ir_ir_ul;
	    case code_rsbi_p:		printf("rsbi_p");	goto ir_ir_p;
	    case code_mulr_i:		printf("mulr_i");	goto ir_ir_ir;
	    case code_mulr_ui:		printf("mulr_ui");	goto ir_ir_ir;
	    case code_mulr_l:		printf("mulr_l");	goto ir_ir_ir;
	    case code_mulr_ul:		printf("mulr_ul");	goto ir_ir_ir;
	    case code_mulr_f:		printf("mulr_f");	goto fr_fr_fr;
	    case code_mulr_d:		printf("mulr_d");	goto fr_fr_fr;
	    case code_muli_i:		printf("muli_i");	goto ir_ir_i;
	    case code_muli_ui:		printf("muli_ui");	goto ir_ir_ui;
	    case code_muli_l:		printf("muli_l");	goto ir_ir_l;
	    case code_muli_ul:		printf("muli_ul");	goto ir_ir_ul;
	    case code_hmulr_i:		printf("hmulr_i");	goto ir_ir_ir;
	    case code_hmulr_ui:		printf("hmulr_ui");	goto ir_ir_ir;
	    case code_hmulr_l:		printf("hmulr_l");	goto ir_ir_ir;
	    case code_hmulr_ul:		printf("hmulr_ul");	goto ir_ir_ir;
	    case code_hmuli_i:		printf("hmuli_i");	goto ir_ir_i;
	    case code_hmuli_ui:		printf("hmuli_ui");	goto ir_ir_ui;
	    case code_hmuli_l:		printf("hmuli_l");	goto ir_ir_l;
	    case code_hmuli_ul:		printf("hmuli_ul");	goto ir_ir_ul;
	    case code_divr_i:		printf("divr_i");	goto ir_ir_ir;
	    case code_divr_ui:		printf("divr_ui");	goto ir_ir_ir;
	    case code_divr_l:		printf("divr_l");	goto ir_ir_ir;
	    case code_divr_ul:		printf("divr_ul");	goto ir_ir_ir;
	    case code_divr_f:		printf("divr_f");	goto fr_fr_fr;
	    case code_divr_d:		printf("divr_d");	goto fr_fr_fr;
	    case code_divi_i:		printf("divi_i");	goto ir_ir_i;
	    case code_divi_ui:		printf("divi_ui");	goto ir_ir_ui;
	    case code_divi_l:		printf("divi_l");	goto ir_ir_l;
	    case code_divi_ul:		printf("divi_ul");	goto ir_ir_ul;
	    case code_modr_i:		printf("modr_i");	goto ir_ir_ir;
	    case code_modr_ui:		printf("modr_ui");	goto ir_ir_ir;
	    case code_modr_l:		printf("modr_l");	goto ir_ir_ir;
	    case code_modr_ul:		printf("modr_ul");	goto ir_ir_ir;
	    case code_modi_i:		printf("modi_i");	goto ir_ir_i;
	    case code_modi_ui:		printf("modi_ui");	goto ir_ir_ui;
	    case code_modi_l:		printf("modi_l");	goto ir_ir_l;
	    case code_modi_ul:		printf("modi_ul");	goto ir_ir_ul;
	    case code_andr_i:		printf("andr_i");	goto ir_ir_ir;
	    case code_andr_ui:		printf("andr_ui");	goto ir_ir_ir;
	    case code_andr_l:		printf("andr_l");	goto ir_ir_ir;
	    case code_andr_ul:		printf("andr_ul");	goto ir_ir_ir;
	    case code_andi_i:		printf("andi_i");	goto ir_ir_i;
	    case code_andi_ui:		printf("andi_ui");	goto ir_ir_ui;
	    case code_andi_l:		printf("andi_l");	goto ir_ir_l;
	    case code_andi_ul:		printf("andi_ul");	goto ir_ir_ul;
	    case code_orr_i:		printf("orr_i");	goto ir_ir_ir;
	    case code_orr_ui:		printf("orr_ui");	goto ir_ir_ir;
	    case code_orr_l:		printf("orr_l");	goto ir_ir_ir;
	    case code_orr_ul:		printf("orr_ul");	goto ir_ir_ir;
	    case code_ori_i:		printf("ori_i");	goto ir_ir_i;
	    case code_ori_ui:		printf("ori_ui");	goto ir_ir_ui;
	    case code_ori_l:		printf("ori_l");	goto ir_ir_l;
	    case code_ori_ul:		printf("ori_ul");	goto ir_ir_ul;
	    case code_xorr_i:		printf("xorr_i");	goto ir_ir_ir;
	    case code_xorr_ui:		printf("xorr_ui");	goto ir_ir_ir;
	    case code_xorr_l:		printf("xorr_l");	goto ir_ir_ir;
	    case code_xorr_ul:		printf("xorr_ul");	goto ir_ir_ir;
	    case code_xori_i:		printf("xori_i");	goto ir_ir_i;
	    case code_xori_ui:		printf("xori_ui");	goto ir_ir_ui;
	    case code_xori_l:		printf("xori_l");	goto ir_ir_l;
	    case code_xori_ul:		printf("xori_ul");	goto ir_ir_ul;
	    case code_lshr_i:		printf("lshr_i");	goto ir_ir_ir;
	    case code_lshr_ui:		printf("lshr_ui");	goto ir_ir_ir;
	    case code_lshr_l:		printf("lshr_l");	goto ir_ir_ir;
	    case code_lshr_ul:		printf("lshr_ul");	goto ir_ir_ir;
	    case code_lshi_i:		printf("lshi_i");	goto ir_ir_i;
	    case code_lshi_ui:		printf("lshi_ui");	goto ir_ir_ui;
	    case code_lshi_l:		printf("lshi_l");	goto ir_ir_l;
	    case code_lshi_ul:		printf("lshi_ul");	goto ir_ir_ul;
	    case code_rshr_i:		printf("rshr_i");	goto ir_ir_ir;
	    case code_rshr_ui:		printf("rshr_ui");	goto ir_ir_ir;
	    case code_rshr_l:		printf("rshr_l");	goto ir_ir_ir;
	    case code_rshr_ul:		printf("rshr_ul");	goto ir_ir_ir;
	    case code_rshi_i:		printf("rshi_i");	goto ir_ir_i;
	    case code_rshi_ui:		printf("rshi_ui");	goto ir_ir_ui;
	    case code_rshi_l:		printf("rshi_l");	goto ir_ir_l;
	    case code_rshi_ul:		printf("rshi_ul");	goto ir_ir_ul;
	    case code_absr_f:		printf("absr_f");	goto fr_fr;
	    case code_absr_d:		printf("absr_d");	goto fr_fr;
	    case code_negr_i:		printf("negr_i");	goto ir_ir;
	    case code_negr_l:		printf("negr_l");	goto ir_ir;
	    case code_negr_f:		printf("negr_f");	goto fr_fr;
	    case code_negr_d:		printf("negr_d");	goto fr_fr;
	    case code_notr_i:		printf("notr_i");	goto ir_ir;
	    case code_notr_l:		printf("notr_l");	goto ir_ir;
	    case code_ltr_i:		printf("ltr_i");	goto ir_ir_ir;
	    case code_ltr_ui:		printf("ltr_ui");	goto ir_ir_ir;
	    case code_ltr_l:		printf("ltr_l");	goto ir_ir_ir;
	    case code_ltr_ul:		printf("ltr_ul");	goto ir_ir_ir;
	    case code_ltr_p:		printf("ltr_p");	goto ir_ir_ir;
	    case code_ltr_f:		printf("ltr_f");	goto ir_fr_fr;
	    case code_ltr_d:		printf("ltr_d");	goto ir_fr_fr;
	    case code_lti_i:		printf("lti_i");	goto ir_ir_i;
	    case code_lti_ui:		printf("lti_ui");	goto ir_ir_ui;
	    case code_lti_l:		printf("lti_l");	goto ir_ir_l;
	    case code_lti_ul:		printf("lti_ul");	goto ir_ir_ul;
	    case code_lti_p:		printf("lti_p");	goto ir_ir_p;
	    case code_ler_i:		printf("ler_i");	goto ir_ir_ir;
	    case code_ler_ui:		printf("ler_ui");	goto ir_ir_ir;
	    case code_ler_l:		printf("ler_l");	goto ir_ir_ir;
	    case code_ler_ul:		printf("ler_ul");	goto ir_ir_ir;
	    case code_ler_p:		printf("ler_p");	goto ir_ir_ir;
	    case code_ler_f:		printf("ler_f");	goto ir_fr_fr;
	    case code_ler_d:		printf("ler_d");	goto ir_fr_fr;
	    case code_lei_i:		printf("lei_i");	goto ir_ir_i;
	    case code_lei_ui:		printf("lei_ui");	goto ir_ir_ui;
	    case code_lei_l:		printf("lei_l");	goto ir_ir_l;
	    case code_lei_ul:		printf("lei_ul");	goto ir_ir_ul;
	    case code_lei_p:		printf("lei_p");	goto ir_ir_p;
	    case code_eqr_i:		printf("eqr_i");	goto ir_ir_ir;
	    case code_eqr_ui:		printf("eqr_ui");	goto ir_ir_ir;
	    case code_eqr_l:		printf("eqr_l");	goto ir_ir_ir;
	    case code_eqr_ul:		printf("eqr_ul");	goto ir_ir_ir;
	    case code_eqr_p:		printf("eqr_p");	goto ir_ir_ir;
	    case code_eqr_f:		printf("eqr_f");	goto ir_fr_fr;
	    case code_eqr_d:		printf("eqr_d");	goto ir_fr_fr;
	    case code_eqi_i:		printf("eqi_i");	goto ir_ir_i;
	    case code_eqi_ui:		printf("eqi_ui");	goto ir_ir_ui;
	    case code_eqi_l:		printf("eqi_l");	goto ir_ir_l;
	    case code_eqi_ul:		printf("eqi_ul");	goto ir_ir_ul;
	    case code_eqi_p:		printf("eqi_p");	goto ir_ir_p;
	    case code_ger_i:		printf("ger_i");	goto ir_ir_ir;
	    case code_ger_ui:		printf("ger_ui");	goto ir_ir_ir;
	    case code_ger_l:		printf("ger_l");	goto ir_ir_ir;
	    case code_ger_ul:		printf("ger_ul");	goto ir_ir_ir;
	    case code_ger_p:		printf("ger_p");	goto ir_ir_ir;
	    case code_ger_f:		printf("ger_f");	goto ir_fr_fr;
	    case code_ger_d:		printf("ger_d");	goto ir_fr_fr;
	    case code_gei_i:		printf("gei_i");	goto ir_ir_i;
	    case code_gei_ui:		printf("gei_ui");	goto ir_ir_ui;
	    case code_gei_l:		printf("gei_l");	goto ir_ir_l;
	    case code_gei_ul:		printf("gei_ul");	goto ir_ir_ul;
	    case code_gei_p:		printf("gei_p");	goto ir_ir_p;
	    case code_gtr_i:		printf("gtr_i");	goto ir_ir_ir;
	    case code_gtr_ui:		printf("gtr_ui");	goto ir_ir_ir;
	    case code_gtr_l:		printf("gtr_l");	goto ir_ir_ir;
	    case code_gtr_ul:		printf("gtr_ul");	goto ir_ir_ir;
	    case code_gtr_p:		printf("gtr_p");	goto ir_ir_ir;
	    case code_gtr_f:		printf("gtr_f");	goto ir_fr_fr;
	    case code_gtr_d:		printf("gtr_d");	goto ir_fr_fr;
	    case code_gti_i:		printf("gti_i");	goto ir_ir_i;
	    case code_gti_ui:		printf("gti_ui");	goto ir_ir_ui;
	    case code_gti_l:		printf("gti_l");	goto ir_ir_l;
	    case code_gti_ul:		printf("gti_ul");	goto ir_ir_ul;
	    case code_gti_p:		printf("gti_p");	goto ir_ir_p;
	    case code_ner_i:		printf("ner_i");	goto ir_ir_ir;
	    case code_ner_ui:		printf("ner_ui");	goto ir_ir_ir;
	    case code_ner_l:		printf("ner_l");	goto ir_ir_ir;
	    case code_ner_ul:		printf("ner_ul");	goto ir_ir_ir;
	    case code_ner_p:		printf("ner_p");	goto ir_fr_fr;
	    case code_ner_f:		printf("ner_f");	goto ir_fr_fr;
	    case code_ner_d:		printf("ner_d");	goto ir_ir_i;
	    case code_nei_i:		printf("nei_i");	goto ir_ir_i;
	    case code_nei_ui:		printf("nei_ui");	goto ir_ir_ui;
	    case code_nei_l:		printf("nei_l");	goto ir_ir_l;
	    case code_nei_ul:		printf("nei_ul");	goto ir_ir_ul;
	    case code_nei_p:		printf("nei_p");	goto ir_ir_p;
	    case code_unltr_f:		printf("unltr_f");	goto ir_fr_fr;
	    case code_unltr_d:		printf("unltr_d");	goto ir_fr_fr;
	    case code_unler_f:		printf("unler_f");	goto ir_fr_fr;
	    case code_unler_d:		printf("unler_d");	goto ir_fr_fr;
	    case code_uneqr_f:		printf("uneqr_f");	goto ir_fr_fr;
	    case code_uneqr_d:		printf("uneqr_d");	goto ir_fr_fr;
	    case code_unger_f:		printf("unger_f");	goto ir_fr_fr;
	    case code_unger_d:		printf("unger_d");	goto ir_fr_fr;
	    case code_ungtr_f:		printf("ungtr_f");	goto ir_fr_fr;
	    case code_ungtr_d:		printf("ungtr_d");	goto ir_fr_fr;
	    case code_ltgtr_f:		printf("ltgtr_f");	goto ir_fr_fr;
	    case code_ltgtr_d:		printf("ltgtr_d");	goto ir_fr_fr;
	    case code_ordr_f:		printf("ordr_f");	goto ir_fr_fr;
	    case code_ordr_d:		printf("ordr_d");	goto ir_fr_fr;
	    case code_unordr_f:		printf("unordr_f");	goto ir_fr_fr;
	    case code_unordr_d:		printf("unordr_d");	goto ir_fr_fr;
	    case code_movr_i:		printf("movr_i");	goto ir_ir;
	    case code_movr_ui:		printf("movr_ui");	goto ir_ir;
	    case code_movr_l:		printf("movr_l");	goto ir_ir;
	    case code_movr_ul:		printf("movr_ul");	goto ir_ir;
	    case code_movr_p:		printf("movr_p");	goto ir_ir;
	    case code_movr_f:		printf("movr_f");	goto fr_fr;
	    case code_movr_d:		printf("movr_d");	goto fr_fr;
	    case code_movi_i:		printf("movi_i");	goto ir_i;
	    case code_movi_ui:		printf("movi_ui");	goto ir_ui;
	    case code_movi_l:		printf("movi_l");	goto ir_l;
	    case code_movi_ul:		printf("movi_ul");	goto ir_ul;
	    case code_movi_p:		printf("movi_p");	goto ir_p;
	    case code_movi_f:		printf("movi_f");	goto fr_f;
	    case code_movi_d:		printf("movi_d");	goto fr_d;
	    case code_extr_c_i:		printf("extr_c_i");	goto ir_ir;
	    case code_extr_uc_ui:	printf("extr_uc_ui");	goto ir_ir;
	    case code_extr_s_i:		printf("extr_s_i");	goto ir_ir;
	    case code_extr_us_ui:	printf("extr_us_ui");	goto ir_ir;
	    case code_extr_c_l:		printf("extr_c_l");	goto ir_ir;
	    case code_extr_uc_ul:	printf("extr_uc_ul");	goto ir_ir;
	    case code_extr_s_l:		printf("extr_s_l");	goto ir_ir;
	    case code_extr_us_ul:	printf("extr_us_ul");	goto ir_ir;
	    case code_extr_i_l:		printf("extr_i_l");	goto ir_ir;
	    case code_extr_ui_ul:	printf("extr_ui_ul");	goto ir_ir;
	    case code_extr_i_f:		printf("extr_i_f");	goto fr_ir;
	    case code_extr_i_d:		printf("extr_i_d");	goto fr_ir;
	    case code_extr_l_f:		printf("extr_l_f");	goto fr_ir;
	    case code_extr_l_d:		printf("extr_l_d");	goto fr_ir;
	    case code_extr_f_d:		printf("extr_f_d");	goto fr_fr;
	    case code_extr_d_f:		printf("extr_d_f");	goto fr_fr;
	    case code_movr_i_f:		printf("movr_i_f");	goto fr_ir;
	    case code_movr_l_d:		printf("movr_l_d");	goto fr_ir;
	    case code_movr_f_i:		printf("movr_f_i");	goto ir_fr;
	    case code_movr_d_l:		printf("movr_d_l");	goto ir_fr;
	    case code_roundr_f_i:	printf("roundr_f_i");	goto ir_fr;
	    case code_roundr_f_l:	printf("roundr_f_l");	goto ir_fr;
	    case code_roundr_d_i:	printf("roundr_d_i");	goto ir_fr;
	    case code_roundr_d_l:	printf("roundr_d_l");	goto ir_fr;
	    case code_truncr_f_i:	printf("truncr_f_i");	goto ir_fr;
	    case code_truncr_f_l:	printf("truncr_f_l");	goto ir_fr;
	    case code_truncr_d_i:	printf("truncr_d_i");	goto ir_fr;
	    case code_truncr_d_l:	printf("truncr_d_l");	goto ir_fr;
	    case code_floorr_f_i:	printf("floorr_f_i");	goto ir_fr;
	    case code_floorr_f_l:	printf("floorr_f_l");	goto ir_fr;
	    case code_floorr_d_i:	printf("floorr_d_i");	goto ir_fr;
	    case code_floorr_d_l:	printf("floorr_d_l");	goto ir_fr;
	    case code_ceilr_f_i:	printf("ceilr_f_i");	goto ir_fr;
	    case code_ceilr_f_l:	printf("ceilr_f_l");	goto ir_fr;
	    case code_ceilr_d_i:	printf("ceilr_d_i");	goto ir_fr;
	    case code_ceilr_d_l:	printf("ceilr_d_l");	goto ir_fr;
	    case code_hton_us_ui:	printf("hton_us_ui");	goto ir_ir;
	    case code_ntoh_us_ui:	printf("ntoh_us_ui");	goto ir_ir;
	    case code_ldr_c:		printf("ldr_c");	goto ir_ir;
	    case code_ldr_uc:		printf("ldr_uc");	goto ir_ir;
	    case code_ldr_s:		printf("ldr_s");	goto ir_ir;
	    case code_ldr_us:		printf("ldr_us");	goto ir_ir;
	    case code_ldr_i:		printf("ldr_i");	goto ir_ir;
	    case code_ldr_ui:		printf("ldr_ui");	goto ir_ir;
	    case code_ldr_l:		printf("ldr_l");	goto ir_ir;
	    case code_ldr_ul:		printf("ldr_ul");	goto ir_ir;
	    case code_ldr_p:		printf("ldr_p");	goto ir_ir;
	    case code_ldr_f:		printf("ldr_f");	goto ir_ir;
	    case code_ldr_d:		printf("ldr_d");	goto ir_ir;
	    case code_ldi_c:		printf("ldi_c");	goto ir_p;
	    case code_ldi_uc:		printf("ldi_uc");	goto ir_p;
	    case code_ldi_s:		printf("ldi_s");	goto ir_p;
	    case code_ldi_us:		printf("ldi_us");	goto ir_p;
	    case code_ldi_i:		printf("ldi_i");	goto ir_p;
	    case code_ldi_ui:		printf("ldi_ui");	goto ir_p;
	    case code_ldi_l:		printf("ldi_l");	goto ir_p;
	    case code_ldi_ul:		printf("ldi_ul");	goto ir_p;
	    case code_ldi_p:		printf("ldi_p");	goto ir_p;
	    case code_ldi_f:		printf("ldi_f");	goto fr_p;
	    case code_ldi_d:		printf("ldi_d");	goto fr_p;
	    case code_ldxr_c:		printf("ldxr_c");	goto ir_ir_ir;
	    case code_ldxr_uc:		printf("ldxr_uc");	goto ir_ir_ir;
	    case code_ldxr_s:		printf("ldxr_s");	goto ir_ir_ir;
	    case code_ldxr_us:		printf("ldxr_us");	goto ir_ir_ir;
	    case code_ldxr_i:		printf("ldxr_i");	goto ir_ir_ir;
	    case code_ldxr_ui:		printf("ldxr_ui");	goto ir_ir_ir;
	    case code_ldxr_l:		printf("ldxr_l");	goto ir_ir_ir;
	    case code_ldxr_ul:		printf("ldxr_ul");	goto ir_ir_ir;
	    case code_ldxr_p:		printf("ldxr_p");	goto ir_ir_ir;
	    case code_ldxr_f:		printf("ldxr_f");	goto ir_ir_ir;
	    case code_ldxr_d:		printf("ldxr_d");	goto ir_ir_ir;
	    case code_ldxi_c:		printf("ldxi_c");	goto ir_ir_l;
	    case code_ldxi_uc:		printf("ldxi_uc");	goto ir_ir_l;
	    case code_ldxi_s:		printf("ldxi_s");	goto ir_ir_l;
	    case code_ldxi_us:		printf("ldxi_us");	goto ir_ir_l;
	    case code_ldxi_i:		printf("ldxi_i");	goto ir_ir_l;
	    case code_ldxi_ui:		printf("ldxi_ui");	goto ir_ir_l;
	    case code_ldxi_l:		printf("ldxi_l");	goto ir_ir_l;
	    case code_ldxi_ul:		printf("ldxi_ul");	goto ir_ir_l;
	    case code_ldxi_p:		printf("ldxi_p");	goto ir_ir_l;
	    case code_ldxi_f:		printf("ldxi_f");	goto fr_ir_l;
	    case code_ldxi_d:		printf("ldxi_d");	goto fr_ir_l;
	    case code_str_c:		printf("str_c");	goto ir_ir;
	    case code_str_uc:		printf("str_uc");	goto ir_ir;
	    case code_str_s:		printf("str_s");	goto ir_ir;
	    case code_str_us:		printf("str_us");	goto ir_ir;
	    case code_str_i:		printf("str_i");	goto ir_ir;
	    case code_str_ui:		printf("str_ui");	goto ir_ir;
	    case code_str_l:		printf("str_l");	goto ir_ir;
	    case code_str_ul:		printf("str_ul");	goto ir_ir;
	    case code_str_p:		printf("str_p");	goto ir_ir;
	    case code_str_f:		printf("str_f");	goto ir_fr;
	    case code_str_d:		printf("str_d");	goto ir_fr;
	    case code_sti_c:		printf("sti_c");	goto p_ir;
	    case code_sti_uc:		printf("sti_uc");	goto p_ir;
	    case code_sti_s:		printf("sti_s");	goto p_ir;
	    case code_sti_us:		printf("sti_us");	goto p_ir;
	    case code_sti_i:		printf("sti_i");	goto p_ir;
	    case code_sti_ui:		printf("sti_ui");	goto p_ir;
	    case code_sti_l:		printf("sti_l");	goto p_ir;
	    case code_sti_ul:		printf("sti_ul");	goto p_ir;
	    case code_sti_p:		printf("sti_p");	goto p_ir;
	    case code_sti_f:		printf("sti_f");	goto p_fr;
	    case code_sti_d:		printf("sti_d");	goto p_fr;
	    case code_stxr_c:		printf("stxr_c");	goto ir_ir_ir;
	    case code_stxr_uc:		printf("stxr_uc");	goto ir_ir_ir;
	    case code_stxr_s:		printf("stxr_s");	goto ir_ir_ir;
	    case code_stxr_us:		printf("stxr_us");	goto ir_ir_ir;
	    case code_stxr_i:		printf("stxr_i");	goto ir_ir_ir;
	    case code_stxr_ui:		printf("stxr_ui");	goto ir_ir_ir;
	    case code_stxr_l:		printf("stxr_l");	goto ir_ir_ir;
	    case code_stxr_ul:		printf("stxr_ul");	goto ir_ir_ir;
	    case code_stxr_p:		printf("stxr_p");	goto ir_ir_ir;
	    case code_stxr_f:		printf("stxr_f");	goto ir_ir_fr;
	    case code_stxr_d:		printf("stxr_d");	goto ir_ir_fr;
	    case code_stxi_c:		printf("stxi_c");	goto l_ir_ir;
	    case code_stxi_uc:		printf("stxi_uc");	goto l_ir_ir;
	    case code_stxi_s:		printf("stxi_s");	goto l_ir_ir;
	    case code_stxi_us:		printf("stxi_us");	goto l_ir_ir;
	    case code_stxi_i:		printf("stxi_i");	goto l_ir_ir;
	    case code_stxi_ui:		printf("stxi_ui");	goto l_ir_ir;
	    case code_stxi_l:		printf("stxi_l");	goto l_ir_ir;
	    case code_stxi_ul:		printf("stxi_ul");	goto l_ir_ir;
	    case code_stxi_p:		printf("stxi_p");	goto l_ir_ir;
	    case code_stxi_f:		printf("stxi_f");	goto l_ir_fr;
	    case code_stxi_d:		printf("stxi_d");	goto l_ir_fr;
	    case code_prepare:		printf("prepare");	goto i;
	    case code_pusharg_c:	printf("pusharg_c");	goto ir;
	    case code_pusharg_uc:	printf("pusharg_uc");	goto ir;
	    case code_pusharg_s:	printf("pusharg_s");	goto ir;
	    case code_pusharg_us:	printf("pusharg_us");	goto ir;
	    case code_pusharg_i:	printf("pusharg_i");	goto ir;
	    case code_pusharg_ui:	printf("pusharg_ui");	goto ir;
	    case code_pusharg_l:	printf("pusharg_l");	goto ir;
	    case code_pusharg_ul:	printf("pusharg_ul");	goto ir;
	    case code_pusharg_p:	printf("pusharg_p");	goto ir;
	    case code_pusharg_f:	printf("pusharg_f");	goto fr;
	    case code_pusharg_d:	printf("pusharg_d");	goto fr;
	    case code_getarg_c:		printf("getarg_c");	goto ir_p;
	    case code_getarg_uc:	printf("getarg_uc");	goto ir_p;
	    case code_getarg_s:		printf("getarg_s");	goto ir_p;
	    case code_getarg_us:	printf("getarg_us");	goto ir_p;
	    case code_getarg_i:		printf("getarg_i");	goto ir_p;
	    case code_getarg_ui:	printf("getarg_ui");	goto ir_p;
	    case code_getarg_l:		printf("getarg_l");	goto ir_p;
	    case code_getarg_ul:	printf("getarg_ul");	goto ir_p;
	    case code_getarg_p:		printf("getarg_p");	goto ir_p;
	    case code_getarg_f:		printf("getarg_f");	goto ir_p;
	    case code_getarg_d:		printf("getarg_d");	goto ir_p;
	    case code_retval_c:		printf("retval_c");	goto ir;
	    case code_retval_uc:	printf("retval_uc");	goto ir;
	    case code_retval_s:		printf("retval_s");	goto ir;
	    case code_retval_us:	printf("retval_us");	goto ir;
	    case code_retval_i:		printf("retval_i");	goto ir;
	    case code_retval_ui:	printf("retval_ui");	goto ir;
	    case code_retval_l:		printf("retval_l");	goto ir;
	    case code_retval_ul:	printf("retval_ul");	goto ir;
	    case code_retval_p:		printf("retval_p");	goto ir;
	    case code_retval_f:		printf("retval_f");	goto fr;
	    case code_retval_d:		printf("retval_d");	goto fr;
	    case code_bltr_i:		printf("bltr_i");	goto p_ir_ir;
	    case code_bltr_ui:		printf("bltr_ui");	goto p_ir_ir;
	    case code_bltr_l:		printf("bltr_l");	goto p_ir_ir;
	    case code_bltr_ul:		printf("bltr_ul");	goto p_ir_ir;
	    case code_bltr_p:		printf("bltr_p");	goto p_ir_ir;
	    case code_bltr_f:		printf("bltr_f");	goto p_fr_fr;
	    case code_bltr_d:		printf("bltr_d");	goto p_fr_fr;
	    case code_blti_i:		printf("blti_i");	goto p_ir_i;
	    case code_blti_ui:		printf("blti_ui");	goto p_ir_ui;
	    case code_blti_l:		printf("blti_l");	goto p_ir_l;
	    case code_blti_ul:		printf("blti_ul");	goto p_ir_ul;
	    case code_blti_p:		printf("blti_p");	goto p_ir_p;
	    case code_bler_i:		printf("bler_i");	goto p_ir_ir;
	    case code_bler_ui:		printf("bler_ui");	goto p_ir_ir;
	    case code_bler_l:		printf("bler_l");	goto p_ir_ir;
	    case code_bler_ul:		printf("bler_ul");	goto p_ir_ir;
	    case code_bler_p:		printf("bler_p");	goto p_ir_ir;
	    case code_bler_f:		printf("bler_f");	goto p_fr_fr;
	    case code_bler_d:		printf("bler_d");	goto p_fr_fr;
	    case code_blei_i:		printf("blei_i");	goto p_ir_i;
	    case code_blei_ui:		printf("blei_ui");	goto p_ir_ui;
	    case code_blei_l:		printf("blei_l");	goto p_ir_l;
	    case code_blei_ul:		printf("blei_ul");	goto p_ir_ul;
	    case code_blei_p:		printf("blei_p");	goto p_ir_p;
	    case code_beqr_i:		printf("beqr_i");	goto p_ir_ir;
	    case code_beqr_ui:		printf("beqr_ui");	goto p_ir_ir;
	    case code_beqr_l:		printf("beqr_l");	goto p_ir_ir;
	    case code_beqr_ul:		printf("beqr_ul");	goto p_ir_ir;
	    case code_beqr_p:		printf("beqr_p");	goto p_ir_ir;
	    case code_beqr_f:		printf("beqr_f");	goto p_fr_fr;
	    case code_beqr_d:		printf("beqr_d");	goto p_fr_fr;
	    case code_beqi_i:		printf("beqi_i");	goto p_ir_i;
	    case code_beqi_ui:		printf("beqi_ui");	goto p_ir_ui;
	    case code_beqi_l:		printf("beqi_l");	goto p_ir_l;
	    case code_beqi_ul:		printf("beqi_ul");	goto p_ir_ul;
	    case code_beqi_p:		printf("beqi_p");	goto p_ir_p;
	    case code_bger_i:		printf("bger_i");	goto p_ir_ir;
	    case code_bger_ui:		printf("bger_ui");	goto p_ir_ir;
	    case code_bger_l:		printf("bger_l");	goto p_ir_ir;
	    case code_bger_ul:		printf("bger_ul");	goto p_ir_ir;
	    case code_bger_p:		printf("bger_p");	goto p_ir_ir;
	    case code_bger_f:		printf("bger_f");	goto p_fr_fr;
	    case code_bger_d:		printf("bger_d");	goto p_fr_fr;
	    case code_bgei_i:		printf("bgei_i");	goto p_ir_i;
	    case code_bgei_ui:		printf("bgei_ui");	goto p_ir_ui;
	    case code_bgei_l:		printf("bgei_l");	goto p_ir_l;
	    case code_bgei_ul:		printf("bgei_ul");	goto p_ir_ul;
	    case code_bgei_p:		printf("bgei_p");	goto p_ir_p;
	    case code_bgtr_i:		printf("bgtr_i");	goto p_ir_ir;
	    case code_bgtr_ui:		printf("bgtr_ui");	goto p_ir_ir;
	    case code_bgtr_l:		printf("bgtr_l");	goto p_ir_ir;
	    case code_bgtr_ul:		printf("bgtr_ul");	goto p_ir_ir;
	    case code_bgtr_p:		printf("bgtr_p");	goto p_ir_ir;
	    case code_bgtr_f:		printf("bgtr_f");	goto p_fr_fr;
	    case code_bgtr_d:		printf("bgtr_d");	goto p_fr_fr;
	    case code_bgti_i:		printf("bgti_i");	goto p_ir_i;
	    case code_bgti_ui:		printf("bgti_ui");	goto p_ir_ui;
	    case code_bgti_l:		printf("bgti_l");	goto p_ir_l;
	    case code_bgti_ul:		printf("bgti_ul");	goto p_ir_ul;
	    case code_bgti_p:		printf("bgti_p");	goto p_ir_p;
	    case code_bner_i:		printf("bner_i");	goto p_ir_ir;
	    case code_bner_ui:		printf("bner_ui");	goto p_ir_ir;
	    case code_bner_l:		printf("bner_l");	goto p_ir_ir;
	    case code_bner_ul:		printf("bner_ul");	goto p_ir_ir;
	    case code_bner_p:		printf("bner_p");	goto p_ir_ir;
	    case code_bner_f:		printf("bner_f");	goto p_fr_fr;
	    case code_bner_d:		printf("bner_d");	goto p_fr_fr;
	    case code_bnei_i:		printf("bnei_i");	goto p_ir_i;
	    case code_bnei_ui:		printf("bnei_ui");	goto p_ir_ui;
	    case code_bnei_l:		printf("bnei_l");	goto p_ir_l;
	    case code_bnei_ul:		printf("bnei_ul");	goto p_ir_ul;
	    case code_bnei_p:		printf("bnei_p");	goto p_ir_p;
	    case code_bunltr_f:		printf("bunltr_f");	goto p_fr_fr;
	    case code_bunltr_d:		printf("bunltr_d");	goto p_fr_fr;
	    case code_bunler_f:		printf("bunler_f");	goto p_fr_fr;
	    case code_bunler_d:		printf("bunler_d");	goto p_fr_fr;
	    case code_buneqr_f:		printf("buneqr_f");	goto p_fr_fr;
	    case code_buneqr_d:		printf("buneqr_d");	goto p_fr_fr;
	    case code_bunger_f:		printf("bunger_f");	goto p_fr_fr;
	    case code_bunger_d:		printf("bunger_d");	goto p_fr_fr;
	    case code_bungtr_f:		printf("bungtr_f");	goto p_fr_fr;
	    case code_bungtr_d:		printf("bungtr_d");	goto p_fr_fr;
	    case code_bltgtr_f:		printf("bltgtr_f");	goto p_fr_fr;
	    case code_bltgtr_d:		printf("bltgtr_d");	goto p_fr_fr;
	    case code_bordr_f:		printf("bordr_f");	goto p_fr_fr;
	    case code_bordr_d:		printf("bordr_d");	goto p_fr_fr;
	    case code_bunordr_f:	printf("bunordr_f");	goto p_fr_fr;
	    case code_bunordr_d:	printf("bunordr_d");	goto p_fr_fr;
	    case code_bmsr_i:		printf("bmsr_i");	goto p_ir_ir;
	    case code_bmsr_ui:		printf("bmsr_ui");	goto p_ir_ir;
	    case code_bmsr_l:		printf("bmsr_l");	goto p_ir_ir;
	    case code_bmsr_ul:		printf("bmsr_ul");	goto p_ir_ir;
	    case code_bmsi_i:		printf("bmsi_i");	goto p_ir_i;
	    case code_bmsi_ui:		printf("bmsi_ui");	goto p_ir_ui;
	    case code_bmsi_l:		printf("bmsi_l");	goto p_ir_l;
	    case code_bmsi_ul:		printf("bmsi_ul");	goto p_ir_ul;
	    case code_bmcr_i:		printf("bmcr_i");	goto p_ir_ir;
	    case code_bmcr_ui:		printf("bmcr_ui");	goto p_ir_ir;
	    case code_bmcr_l:		printf("bmcr_l");	goto p_ir_ir;
	    case code_bmcr_ul:		printf("bmcr_ul");	goto p_ir_ir;
	    case code_bmci_i:		printf("bmci_i");	goto p_ir_ir;
	    case code_bmci_ui:		printf("bmci_ui");	goto p_ir_ui;
	    case code_bmci_l:		printf("bmci_l");	goto p_ir_l;
	    case code_bmci_ul:		printf("bmci_ul");	goto p_ir_ul;
	    case code_boaddr_i:		printf("boaddr_i");	goto p_ir_ir;
	    case code_boaddr_ui:	printf("boaddr_ui");	goto p_ir_ir;
	    case code_boaddr_l:		printf("boaddr_l");	goto p_ir_ir;
	    case code_boaddr_ul:	printf("boaddr_ul");	goto p_ir_ir;
	    case code_boaddi_i:		printf("boaddi_i");	goto p_ir_i;
	    case code_boaddi_ui:	printf("boaddi_ui");	goto p_ir_ui;
	    case code_boaddi_l:		printf("boaddi_l");	goto p_ir_l;
	    case code_boaddi_ul:	printf("boaddi_ul");	goto p_ir_ul;
	    case code_bosubr_i:		printf("bosubr_i");	goto p_ir_ir;
	    case code_bosubr_ui:	printf("bosubr_ui");	goto p_ir_ir;
	    case code_bosubr_l:		printf("bosubr_l");	goto p_ir_ir;
	    case code_bosubr_ul:	printf("bosubr_ul");	goto p_ir_ir;
	    case code_bosubi_i:		printf("bosubi_i");	goto p_ir_i;
	    case code_bosubi_ui:	printf("bosubi_ui");	goto p_ir_ui;
	    case code_bosubi_l:		printf("bosubi_l");	goto p_ir_l;
	    case code_bosubi_ul:	printf("bosubi_ul");	goto p_ir_ul;
	    case code_finish:		printf("finish");	goto p;
	    case code_finishr:		printf("finishr");	goto i;
	    case code_calli:		printf("calli");	goto p;
	    case code_callr:		printf("callr");	goto ir;
	    case code_jmpi:		printf("jmpi");		goto p;
	    case code_jmpr:		printf("jmpr");		goto ir;
	    case code_prolog:		printf("prolog");	break;
	    case code_allocai:		printf("allocai");	goto i;
	    case code_epilog:		printf("epilog");	break;
	    default:						abort();
	}
    }
}
