#include "consts.inc"

/*------------------------------------------------------------------------*/
.data	512
/*
struct types {
    long		 pad;
    signed char		 c;
    unsigned char	 uc;
    signed short	 s;
    unsigned short	 us;
    signed int		 i;
    unsigned int	 ui;
    signed long		 l;
    unsigned long	 ul;
    void		*p;
    float		 f;
    double		 d;
};
 */
t0:
.l	0
c0:
.c	0
uc0:
.c	0
.align	2
s0:
.s	0
us0:
.s	0
.align	4
i0:
.i	0
ui0:
.i	0
.align	$(__WORDSIZE / 8)
l0:
.l	0
ul0:
.l	0
p0:
.p	0
f0:
.f	0
.align	$(__WORDSIZE / 8)
d0:
.d	0

t1:
.l	0
c1:
.c	0
uc1:
.c	0
.align	2
s1:
.s	0
us1:
.s	0
.align	4
i1:
.i	0
ui1:
.i	0
.align	$(__WORDSIZE / 8)
l1:
.l	0
ul1:
.l	0
p1:
.p	0
f1:
.f	0
.align	$(__WORDSIZE / 8)
d1:
.d	0

fmt_c:
.c	" c: %x != %x\n"
fmt_uc:
.c	"uc: %x != %x\n"
fmt_s:
.c	" s: %x != %x\n"
fmt_us:
.c	"us: %x != %x\n"
fmt_i:
.c	" i: %x != %x\n"
fmt_ui:
.c	"ui: %lx != %lx\n"
fmt_l:
.c	" l: %lx != %lx\n"
fmt_ul:
.c	"ll: %lx != %lx\n"
fmt_p:
.c	" p: %p != %p\n"
fmt_f:
.c	" f: %3.12f != %3.12f\n"
fmt_d:
.c	" d: %3.12f != %3.12f\n"
fmt_fd:
.c	"fd: %3.12f != %3.12f\n"
fmt_df:
.c	"df: %3.12f != %3.12f\n"

.	$($offc  = c0  - t0)
.	$($offuc = uc0 - t0)
.	$($offs  = s0  - t0)
.	$($offus = us0 - t0)
.	$($offi  = i0  - t0)
.	$($offui = ui0 - t0)
.	$($offl  = l0  - t0)
.	$($offul = ul0 - t0)
.	$($offp  = p0  - t0)
.	$($offf  = f0  - t0)
.	$($offd  = d0  - t0)

#include "float_branch.inc"
