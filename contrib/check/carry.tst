.data	256
#define ix0		0
#define lx0		0
#define ix1		1
#define lx1		1
#define ix2		2
#define lx2		2
#define ix4		4
#define lx4		4
#define iff		0xffffffff
#define ife		0xfffffffe
#define ifd		0xfffffffd
#define ifc		0xfffffffc
#if __WORDSIZE == 64
#  define lff		0xffffffffffffffff
#  define lfe		0xfffffffffffffffe
#  define lfd		0xfffffffffffffffd
#  define lfc		0xfffffffffffffffc
#else
#  define lff		iff
#  define lfe		ife
#  define lfd		ifd
#  define lfc		ifc
#endif
fmtci:
.c	"%d ic %x %x\n"
fmtxi:
.c	"%d ix %x %x\n"
fmtcl:
.c	"%d lc %lx %lx\n"
fmtxl:
.c	"%d lx %lx %lx\n"

.code	$(1024 * 1024)
	prolog 0

#define xopr6(line,t,op,r0,r1,r2,r3,r4,r5,llo,lhi,rlo,rhi,vlo,vhi)	\
	movi_##t %r1 llo						\
	movi_##t %r2 lhi						\
	movi_##t %r4 rlo						\
	movi_##t %r5 rhi						\
	op##cr_u##t %r0 %r1 %r4						\
	op##xr_u##t %r3 %r2 %r5						\
	beqi_u##t op##t##l##line##r0##r1##r2##r3##r4##r5 %r0 vlo	\
	prepare 4							\
		pusharg_##t %r0						\
		movi_##t %r0 vlo					\
		pusharg_##t %r0						\
		movi_i %r0 line						\
		pusharg_i %r0						\
		movi_p %r0 fmtc##t					\
		pusharg_p %r0						\
	finish @printf							\
op##t##l##line##r0##r1##r2##r3##r4##r5:					\
	beqi_u##t op##t##h##line##r0##r1##r2##r3##r4##r5 %r3 vhi	\
	prepare 4							\
		pusharg_##t %r3						\
		movi_##t %r3 vlo					\
		pusharg_##t %r3						\
		movi_i %r0 line						\
		pusharg_i %r0						\
		movi_p %r3 fmtx##t					\
		pusharg_p %r3						\
	finish @printf							\
op##t##h##line##r0##r1##r2##r3##r4##r5:

#define xopr4_(line,t,op,r0,r1,r2,r3,llo,lhi,rlo,rhi,vlo,vhi)		\
	movi_##t %r0 llo						\
	movi_##t %r1 lhi						\
	movi_##t %r2 rlo						\
	movi_##t %r3 rhi						\
	op##cr_u##t %r0 %r0 %r2						\
	op##xr_u##t %r1 %r1 %r3						\
	beqi_u##t op##t##_##l##line##r0##r1##r2##r3 %r0 vlo		\
	prepare 4							\
		pusharg_##t %r0						\
		movi_##t %r0 vlo					\
		pusharg_##t %r0						\
		movi_i %r0 line						\
		pusharg_i %r0						\
		movi_p %r0 fmtc##t					\
		pusharg_p %r0						\
	finish @printf							\
op##t##_##l##line##r0##r1##r2##r3:					\
	beqi_u##t op##t##_##h##line##r0##r1##r2##r3 %r1 vhi		\
	prepare 4							\
		pusharg_##t %r3						\
		movi_##t %r3 vlo					\
		pusharg_##t %r3						\
		movi_i %r0 line						\
		pusharg_i %r0						\
		movi_p %r3 fmtx##t					\
		pusharg_p %r3						\
	finish @printf							\
op##t##_##h##line##r0##r1##r2##r3:

#define xopr_4(line,t,op,r0,r1,r2,r3,llo,lhi,rlo,rhi,vlo,vhi)		\
	movi_##t %r0 rlo						\
	movi_##t %r1 rhi						\
	movi_##t %r2 llo						\
	movi_##t %r3 lhi						\
	op##cr_u##t %r0 %r2 %r0						\
	op##xr_u##t %r1 %r3 %r1						\
	beqi_u##t op##_##t##l##line##r0##r1##r2##r3 %r0 vlo		\
	prepare 4							\
		pusharg_##t %r0						\
		movi_##t %r0 vlo					\
		pusharg_##t %r0						\
		movi_i %r0 line						\
		pusharg_i %r0						\
		movi_p %r0 fmtc##t					\
		pusharg_p %r0						\
	finish @printf							\
op##_##t##l##line##r0##r1##r2##r3:					\
	beqi_u##t op##_##t##h##line##r0##r1##r2##r3 %r1 vhi		\
	prepare 4							\
		pusharg_##t %r3						\
		movi_##t %r3 vlo					\
		pusharg_##t %r3						\
		movi_i %r0 line						\
		pusharg_i %r0						\
		movi_p %r3 fmtx##t					\
		pusharg_p %r3						\
	finish @printf							\
op##_##t##h##line##r0##r1##r2##r3:

#define xaddr(line,llo,lhi,rlo,rhi,vlo,vhi)				\
	xopr6(line,i,add,r0,r1,r2,v0,v1,v2,i##llo,i##lhi,i##rlo,i##rhi,i##vlo,i##vhi)	\
	xopr6(line,l,add,r0,r1,r2,v0,v1,v2,l##llo,l##lhi,l##rlo,l##rhi,l##vlo,l##vhi)	\
	xopr4_(line,i,add,r0,r1,r2,v0,i##llo,i##lhi,i##rlo,i##rhi,i##vlo,i##vhi)	\
	xopr4_(line,l,add,r0,r1,r2,v0,l##llo,l##lhi,l##rlo,l##rhi,l##vlo,l##vhi)	\
	xopr_4(line,i,add,r0,r1,r2,v0,i##llo,i##lhi,i##rlo,i##rhi,i##vlo,i##vhi)	\
	xopr_4(line,l,add,r0,r1,r2,v0,l##llo,l##lhi,l##rlo,l##rhi,l##vlo,l##vhi)

#define xsubr(line,llo,lhi,rlo,rhi,vlo,vhi)				\
	xopr6(line,i,sub,r0,r1,r2,v0,v1,v2,i##llo,i##lhi,i##rlo,i##rhi,i##vlo,i##vhi)	\
	xopr6(line,l,sub,r0,r1,r2,v0,v1,v2,l##llo,l##lhi,l##rlo,l##rhi,l##vlo,l##vhi)	\
	xopr4_(line,i,sub,r0,r1,r2,v0,i##llo,i##lhi,i##rlo,i##rhi,i##vlo,i##vhi)	\
	xopr4_(line,l,sub,r0,r1,r2,v0,l##llo,l##lhi,l##rlo,l##rhi,l##vlo,l##vhi)	\
	xopr_4(line,i,sub,r0,r1,r2,v0,i##llo,i##lhi,i##rlo,i##rhi,i##vlo,i##vhi)	\
	xopr_4(line,l,sub,r0,r1,r2,v0,l##llo,l##lhi,l##rlo,l##rhi,l##vlo,l##vhi)

	/* 0xffffffffffffffff + 1 = 0x10000000000000000 */
	xaddr(__LINE__, ff, ff, x1, x0, x0, x0)

	/* 1 + 0xffffffffffffffff = 0x10000000000000000 */
	xaddr(__LINE__, x1, x0, ff, ff, x0, x0)

	/* 0xfffffffeffffffff + 1 = 0xffffffff00000000 */
	xaddr(__LINE__, ff, fe, x1, x0, x0, ff)

	/* 1 + 0xfffffffeffffffff = 0xffffffff00000000 */
	xaddr(__LINE__, x1, x0, ff, fe, x0, ff)

	/* 0xfffffffefffffffe + 2 = 0xffffffff00000000 */
	xaddr(__LINE__, fe, fe, x2, x0, x0, ff)

	/* 2 + 0xfffffffefffffffe = 0xffffffff00000000 */
	xaddr(__LINE__, x2, x0, fe, fe, x0, ff)

	/* 0xffffffffffffffff - 1 = 0xfffffffffffffffe */
	xsubr(__LINE__, ff, ff, x1, x0, fe, ff)

	/* 1 - 0xffffffffffffffff = -0xfffffffffffffffe */
	xsubr(__LINE__, x1, x0, ff, ff, x2, x0)

	/* 0xfffffffeffffffff - 1 = 0xfffffffefffffffe */
	xsubr(__LINE__, ff, fe, x1, x0, fe, fe)

	/* 1 - 0xfffffffeffffffff = -0xfffffffefffffffe */
	xsubr(__LINE__, x1, x0, ff, fe, x2, x1)

	/* 0xfffffffefffffffe - 2 = 0xfffffffefffffffc */
	xsubr(__LINE__, fe, fe, x2, x0, fc, fe)

	/* 2 + 0xfffffffefffffffe = -0xfffffffefffffffc */
	xsubr(__LINE__, x2, x0, fe, fe, x4, x1)

	ret
