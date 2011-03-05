/* Should generate an exception for NaN/Inf conversion to integer?
 * Or calling should code ensure it is never generated? */
#define TEST_EXCEPTION	0

#if TEST_EXCEPTION
# define EXCEPT_ROUND(N, IF)	EXCEPT_EXTFI3(N, round, IF)
# define EXCEPT_TRUNC(N, IF)	EXCEPT_EXTFI3(N, trunc, IF)
# define EXCEPT_FLOOR(N, IF)	EXCEPT_EXTFI3(N, floor, IF)
# define  EXCEPT_CEIL(N, IF)	EXCEPT_EXTFI3(N, ceil,  IF)
#else
# define EXCEPT_ROUND(N, IF)	/**/
# define EXCEPT_TRUNC(N, IF)	/**/
# define EXCEPT_FLOOR(N, IF)	/**/
# define  EXCEPT_CEIL(N, IF)	/**/
#endif

/*--------------------------------------------------------------------*/
# define i7f		0x7fffffff
# define i80		0x80000000
# define i81		0x80000001
# define iff		0xffffffff
#if __WORDSIZE == 64
#  define l7f		0x7fffffffffffffff
#  define l80		0x8000000000000000
#  define l81		0x8000000000000001
#  define lff		0xffffffffffffffff
#  define il7f		lff
#  define ui7f		0xffffffff
#  define il80		0
#  define ui80		0
#  define il81		1
#  define ui81		1
#else
#  define l7f		i7f
#  define l80		i80
#  define l81		i81
#  define lff		iff
#  define il7f		i7f
#  define ui7f		i7f
#  define il80		i80
#  define ui80		i80
#  define il81		i81
#  define ui81		i81
#endif

#define ci7f		iff
#define cl7f		lff
#define uc7f		0xff
#define si7f		iff
#define sl7f		lff
#define us7f		0xffff

#define ci80		0
#define cl80		0
#define uc80		0
#define si80		0
#define sl80		0
#define us80		0

#define ci81		1
#define cl81		1
#define uc81		1
#define si81		1
#define sl81		1
#define us81		1

#define ciff		iff
#define clff		lff
#define ucff		0xff
#define siff		iff
#define slff		lff
#define usff		0xffff

#define uiff		iff

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define sh2n		0x1234
#define sn2h		0x3412
#define ih2n		0x12345678
#define in2h		0x78563412
#else
#define sh2n		0x1234
#define sn2h		0x1234
#define ih2n		0x12345678
#define in2h		0x12345678
#endif

/*--------------------------------------------------------------------*/
.data	2048
fmtci:
.c	"extr_c_i   (%lx) %lx = %lx\n"
fmtcui:
.c	"extr_c_ui  (%lx) %lx = %lx\n"
fmtcl:
.c	"extr_c_l   (%lx) %lx = %lx\n"
fmtcul:
.c	"extr_c_ul  (%lx) %lx = %lx\n"
fmtuci:
.c	"extr_uc_i  (%lx) %lx = %lx\n"
fmtucui:
.c	"extr_uc_ui (%lx) %lx = %lx\n"
fmtucl:
.c	"extr_uc_l  (%lx) %lx = %lx\n"
fmtucul:
.c	"extr_uc_ul (%lx) %lx = %lx\n"
fmtsi:
.c	"extr_s_i   (%lx) %lx = %lx\n"
fmtsui:
.c	"extr_s_ui  (%lx) %lx = %lx\n"
fmtsl:
.c	"extr_s_l   (%lx) %lx = %lx\n"
fmtsul:
.c	"extr_s_ul  (%lx) %lx = %lx\n"
fmtusi:
.c	"extr_us_i  (%lx) %lx = %lx\n"
fmtusui:
.c	"extr_us_ui (%lx) %lx = %lx\n"
fmtusl:
.c	"extr_us_l  (%lx) %lx = %lx\n"
fmtusul:
.c	"extr_us_ul (%lx) %lx = %lx\n"
fmtil:
.c	"extr_i_l   (%lx) %lx = %lx\n"
fmtiul:
.c	"extr_i_ul  (%lx) %lx = %lx\n"
fmtuil:
.c	"extr_ui_l  (%lx) %lx = %lx\n"
fmtuiul:
.c	"extr_ui_ul (%lx) %lx = %lx\n"
fmtif:
.c	"extr_i_f   (%lx) %f = %lx\n"
fmtid:
.c	"extr_i_d   (%lx) %f = %lx\n"
fmtlf:
.c	"extr_l_f   (%f) %lx = %f\n"
fmtld:
.c	"extr_l_d   (%f) %lx = %f\n"
fmtroundif:
.c	"roundr_i_f (%lx) %f = %lx\n"
fmtroundlf:
.c	"roundr_l_f (%lx) %f = %lx\n"
fmttruncif:
.c	"truncr_i_f (%lx) %f = %lx\n"
fmttrunclf:
.c	"truncr_l_f (%lx) %f = %lx\n"
fmtfloorif:
.c	"floorr_i_f (%lx) %f = %lx\n"
fmtfloorlf:
.c	"floorr_l_f (%lx) %f = %lx\n"
fmtceilif:
.c	"ceilr_f_i  (%lx) %f = %lx\n"
fmtceillf:
.c	"ceilr_f_l  (%lx) %f = %lx\n"
fmtroundid:
.c	"roundr_d_i (%lx) %f = %lx\n"
fmtroundld:
.c	"roundr_d_l (%lx) %f = %lx\n"
fmttruncid:
.c	"truncr_d_i (%lx) %f = %lx\n"
fmttruncld:
.c	"truncr_d_l (%lx) %f = %lx\n"
fmtfloorid:
.c	"floorr_d_i (%lx) %f = %lx\n"
fmtfloorld:
.c	"floorr_d_l (%lx) %f = %lx\n"
fmtceilid:
.c	"ceilr_d_i  (%lx) %f = %lx\n"
fmtceilld:
.c	"ceilr_d_l  (%lx) %f = %lx\n"
.	$($NaN   =  0.0 / 0.0)
.	$($pInf  =  1.0 / 0.0)
.	$($nInf  = -1.0 / 0.0)
fmthus:
.c	"(%lx) hton_us %lx = %lx\n"
fmthui:
.c	"(%lx) hton_ui %lx = %lx\n"
fmtnus:
.c	"(%lx) ntoh_us %lx = %lx\n"
fmtnui:
.c	"(%lx) ntoh_ui %lx = %lx\n"

/*--------------------------------------------------------------------*/
/* r0 = (type)r1 	->	RR = (TT)(FT)FR */
#define EXTII(N, FT, TT, FR, TR, RR, IF, IT)	\
	movi_##TT %FR IF			\
	movi_##TT %TR IT			\
	extr_##FT##_##TT %RR %FR		\
	beqr_##TT FR##TR##RR##FT##TT##N %RR %TR	\
	prepare 4				\
		pusharg_##TT %RR		\
		pusharg_##TT %FR		\
		pusharg_##FT %TR		\
		movi_p %RR fmt##FT##TT		\
		pusharg_p %RR			\
	finish @printf				\
FR##TR##RR##FT##TT##N:

/* r0 = (type)r0 	->	RR = (TT)(FT)RR */
#define EXTIIC(N, FT, TT, FR, TR, RR, IF, IT)		\
	movi_##TT %FR IF				\
	movi_##TT %TR IT				\
	movr_##TT %RR %FR				\
	extr_##FT##_##TT %RR %RR			\
	beqr_##TT FR##TR##RR##FT##TT##N##c %RR %TR	\
	prepare 4					\
		pusharg_##TT %RR			\
		pusharg_##TT %FR			\
		pusharg_##FT %TR			\
		movi_p %RR fmt##FT##TT			\
		pusharg_p %RR				\
	finish @printf					\
FR##TR##RR##FT##TT##N##c:

/*--------------------------------------------------------------------*/
/* f0 = (float)r0 	->	RR = (float)(TT)FR */
#define EXTIF(N, FT, FR, TR, RR, IF, IT)	\
	movi_##FT %FR IF			\
	movi_f %TR IT				\
	extr_##FT##_f %RR %FR			\
	beqr_f FR##TR##RR##f##FT##N %RR %TR	\
	prepare 2				\
	prepare_d 2				\
		extr_f_d %RR %RR		\
		pusharg_d %RR			\
		pusharg_##FT %FR		\
		extr_f_d %TR %TR		\
		pusharg_d %TR			\
		movi_p %FR fmt##FT##f		\
		pusharg_p %FR			\
	finish @printf				\
FR##TR##RR##f##FT##N:

/* f0 = (double)r0 	->	RR = (double)(TT)FR */
#define EXTID(N, FT, FR, TR, RR, IF, IT)	\
	movi_##FT %FR IF			\
	movi_d %TR IT				\
	extr_##FT##_d %RR %FR			\
	beqr_d FR##TR##RR##d##FT##N %RR %TR	\
	prepare 2				\
	prepare_d 2				\
		pusharg_d %RR			\
		pusharg_##FT %FR		\
		pusharg_d %TR			\
		movi_p %FR fmt##FT##d		\
		pusharg_p %FR			\
	finish @printf				\
FR##TR##RR##d##FT##N:

/*--------------------------------------------------------------------*/
/* r0 = function(f0) 	->	RR = function((float)FR) */
#define EXTFI(N, FN, TT, FR, TR, RR, IF, IT)		\
	movi_f %FR IF					\
	movi_##TT %TR IT				\
	FN##r_f_##TT %RR %FR				\
	beqr_##TT FN##FR##TR##RR##f##TT##N %RR %TR	\
	prepare 3					\
	prepare_d 1					\
		pusharg_##TT %RR			\
		extr_f_d %FR %FR			\
		pusharg_d %FR				\
		pusharg_##TT %TR			\
		movi_p %RR fmt##FN##TT##f		\
		pusharg_p %RR				\
	finish @printf					\
FN##FR##TR##RR##f##TT##N:

/* r0 = function(f0) 	->	RR = function((float)FR) */
#define EXTDI(N, FN, TT, FR, TR, RR, IF, IT)		\
	movi_d %FR IF					\
	movi_##TT %TR IT				\
	FN##r_d_##TT %RR %FR				\
	beqr_##TT FN##FR##TR##RR##d##TT##N %RR %TR	\
	prepare 3					\
	prepare_d 1					\
		pusharg_##TT %RR			\
		pusharg_d %FR				\
		pusharg_##TT %TR			\
		movi_p %RR fmt##FN##TT##f		\
		pusharg_p %RR				\
	finish @printf					\
FN##FR##TR##RR##d##TT##N:

/*--------------------------------------------------------------------*/
.code	$(4 * 1024 * 1024)
	prolog 0

/*--------------------------------------------------------------------*/
#define EXTII0(N, FT, TT, FR, TR, RR, IF, IT)		\
	 EXTII(N, FT, TT, FR, TR, RR, IF, IT)		\
	EXTIIC(N, FT, TT, FR, TR, RR, IF, IT)

#define EXTII1(		  R0, R1, R2)			\
	EXTII0(0,  c,  i, R0, R1, R2, i7f, ci7f)	\
	EXTII0(0,  c, ui, R0, R1, R2, i7f, uc7f)	\
	EXTII0(0,  c,  l, R0, R1, R2, i7f, cl7f)	\
	EXTII0(0,  c, ul, R0, R1, R2, i7f, uc7f)	\
	EXTII0(0, uc,  i, R0, R1, R2, i7f, uc7f)	\
	EXTII0(0, uc, ui, R0, R1, R2, i7f, uc7f)	\
	EXTII0(0, uc,  l, R0, R1, R2, i7f, uc7f)	\
	EXTII0(0, uc, ul, R0, R1, R2, i7f, uc7f)	\
	EXTII0(0,  s,  i, R0, R1, R2, i7f, si7f)	\
	EXTII0(0,  s, ui, R0, R1, R2, i7f, us7f)	\
	EXTII0(0,  s,  l, R0, R1, R2, i7f, sl7f)	\
	EXTII0(0,  s, ul, R0, R1, R2, i7f, us7f)	\
	EXTII0(0, us,  i, R0, R1, R2, i7f, us7f)	\
	EXTII0(0, us, ui, R0, R1, R2, i7f, us7f)	\
	EXTII0(0, us,  l, R0, R1, R2, i7f, us7f)	\
	EXTII0(0, us, ul, R0, R1, R2, i7f, us7f)	\
	EXTII0(0,  i,  l, R0, R1, R2, l7f, il7f)	\
	EXTII0(0, ui, ul, R0, R1, R2, l7f, ui7f)	\
	EXTII0(1,  c,  i, R0, R1, R2, i80, ci80)	\
	EXTII0(1,  c, ui, R0, R1, R2, i80, uc80)	\
	EXTII0(1,  c,  l, R0, R1, R2, i80, cl80)	\
	EXTII0(1,  c, ul, R0, R1, R2, i80, uc80)	\
	EXTII0(1, uc,  i, R0, R1, R2, i80, uc80)	\
	EXTII0(1, uc, ui, R0, R1, R2, i80, uc80)	\
	EXTII0(1, uc,  l, R0, R1, R2, i80, uc80)	\
	EXTII0(1, uc, ul, R0, R1, R2, i80, uc80)	\
	EXTII0(1,  s,  i, R0, R1, R2, i80, si80)	\
	EXTII0(1,  s, ui, R0, R1, R2, i80, us80)	\
	EXTII0(1,  s,  l, R0, R1, R2, i80, sl80)	\
	EXTII0(1,  s, ul, R0, R1, R2, i80, us80)	\
	EXTII0(1, us,  i, R0, R1, R2, i80, us80)	\
	EXTII0(1, us, ui, R0, R1, R2, i80, us80)	\
	EXTII0(1, us,  l, R0, R1, R2, i80, us80)	\
	EXTII0(1, us, ul, R0, R1, R2, i80, us80)	\
	EXTII0(1,  i,  l, R0, R1, R2, l80, il80)	\
	EXTII0(1, ui, ul, R0, R1, R2, l80, il80)	\
	EXTII0(2,  c,  i, R0, R1, R2, i81, ci81)	\
	EXTII0(2,  c, ui, R0, R1, R2, i81, uc81)	\
	EXTII0(2,  c,  l, R0, R1, R2, i81, cl81)	\
	EXTII0(2,  c, ul, R0, R1, R2, i81, uc81)	\
	EXTII0(2, uc,  i, R0, R1, R2, i81, uc81)	\
	EXTII0(2, uc, ui, R0, R1, R2, i81, uc81)	\
	EXTII0(2, uc,  l, R0, R1, R2, i81, uc81)	\
	EXTII0(2, uc, ul, R0, R1, R2, i81, uc81)	\
	EXTII0(2,  s,  i, R0, R1, R2, i81, si81)	\
	EXTII0(2,  s, ui, R0, R1, R2, i81, us81)	\
	EXTII0(2,  s,  l, R0, R1, R2, i81, sl81)	\
	EXTII0(2,  s, ul, R0, R1, R2, i81, us81)	\
	EXTII0(2, us,  i, R0, R1, R2, i81, us81)	\
	EXTII0(2, us, ui, R0, R1, R2, i81, us81)	\
	EXTII0(2, us,  l, R0, R1, R2, i81, us81)	\
	EXTII0(2, us, ul, R0, R1, R2, i81, us81)	\
	EXTII0(2,  i,  l, R0, R1, R2, l81, il81)	\
	EXTII0(2, ui, ul, R0, R1, R2, l81, il81)	\
	EXTII0(3,  c,  i, R0, R1, R2, iff, ciff)	\
	EXTII0(3,  c, ui, R0, R1, R2, iff, ucff)	\
	EXTII0(3,  c,  l, R0, R1, R2, iff, clff)	\
	EXTII0(3,  c, ul, R0, R1, R2, iff, ucff)	\
	EXTII0(3, uc,  i, R0, R1, R2, iff, ucff)	\
	EXTII0(3, uc, ui, R0, R1, R2, iff, ucff)	\
	EXTII0(3, uc,  l, R0, R1, R2, iff, ucff)	\
	EXTII0(3, uc, ul, R0, R1, R2, iff, ucff)	\
	EXTII0(3,  s,  i, R0, R1, R2, iff, siff)	\
	EXTII0(3,  s, ui, R0, R1, R2, iff, usff)	\
	EXTII0(3,  s,  l, R0, R1, R2, iff, slff)	\
	EXTII0(3,  s, ul, R0, R1, R2, iff, usff)	\
	EXTII0(3, us,  i, R0, R1, R2, iff, usff)	\
	EXTII0(3, us, ui, R0, R1, R2, iff, usff)	\
	EXTII0(3, us,  l, R0, R1, R2, iff, usff)	\
	EXTII0(3, us, ul, R0, R1, R2, iff, usff)	\
	EXTII0(3,  i,  l, R0, R1, R2, lff,  lff)	\
	EXTII0(3, ui, ul, R0, R1, R2, lff,  iff)

#define EXTII2(R0, R1, R2)	\
	EXTII1(R0, R1, R2)	\
	EXTII1(R0, R2, R1)

#define EXTII3(R0, R1, R2)	\
	EXTII2(R0, R1, R2)	\
	EXTII2(R1, R0, R2)	\
	EXTII2(R2, R1, R0)

/*--------------------------------------------------------------------*/
#define EXTIF0(	     R0, F0, F1)		\
	 EXTIF(0, i, R0, F0, F1, iff, -1.0)	\
	 EXTIF(0, l, R0, F0, F1, lff, -1.0)	\
	 EXTID(0, i, R0, F0, F1, iff, -1.0)	\
	 EXTID(0, l, R0, F0, F1, lff, -1.0)	\
	 EXTIF(1, i, R0, F0, F1, 0,    0.0)	\
	 EXTIF(1, l, R0, F0, F1, 0,    0.0)	\
	 EXTID(1, i, R0, F0, F1, 0,    0.0)	\
	 EXTID(1, l, R0, F0, F1, 0,    0.0)	\
	 EXTIF(2, i, R0, F0, F1, 1,    1.0)	\
	 EXTIF(2, l, R0, F0, F1, 1,    1.0)	\
	 EXTID(2, i, R0, F0, F1, 1,    1.0)	\
	 EXTID(2, l, R0, F0, F1, 1,    1.0)

#define EXTIF1(F0, F1)		\
	EXTIF0(v0, F0, F1)	\
	EXTIF0(v1, F0, F1)	\
	EXTIF0(v2, F0, F1)	\
	EXTIF0(r0, F0, F1)	\
	EXTIF0(r1, F0, F1)	\
	EXTIF0(r2, F0, F1)

#define EXTIF2(F0, F1)	\
	EXTIF1(F0, F1)	\
	EXTIF1(F1, F0)

#define EXTFI0(N, FN, F0, R0, R1, IF, IT)	\
	 EXTFI(N, FN, i, F0, R0, R1, IF, IT)	\
	 EXTDI(N, FN, i, F0, R0, R1, IF, IT)	\
	 EXTFI(N, FN, l, F0, R0, R1, IF, IT)	\
	 EXTDI(N, FN, l, F0, R0, R1, IF, IT)

#define EXTFI1(N, FN, R0, R1, IF, IT)		\
	EXTFI0(N, FN, f0, R0, R1, IF, IT)	\
	EXTFI0(N, FN, f1, R0, R1, IF, IT)	\
	EXTFI0(N, FN, f2, R0, R1, IF, IT)	\
	EXTFI0(N, FN, f3, R0, R1, IF, IT)	\
	EXTFI0(N, FN, f4, R0, R1, IF, IT)	\
	EXTFI0(N, FN, f5, R0, R1, IF, IT)

#define EXTFI2(N, FN, R0, R1, IF, IT)	\
	EXTFI1(N, FN, R0, R1, IF, IT)	\
	EXTFI1(N, FN, R1, R0, IF, IT)

#define EXTFI3(N, FN, IF, IT)		\
	EXTFI2(N, FN, v0, v1, IF, IT)	\
	EXTFI2(N, FN, v0, v2, IF, IT)	\
	EXTFI2(N, FN, v0, r0, IF, IT)	\
	EXTFI2(N, FN, v0, r1, IF, IT)	\
	EXTFI2(N, FN, v0, r2, IF, IT)	\
	EXTFI2(N, FN, v1, v2, IF, IT)	\
	EXTFI2(N, FN, v1, r0, IF, IT)	\
	EXTFI2(N, FN, v1, r1, IF, IT)	\
	EXTFI2(N, FN, v1, r2, IF, IT)	\
	EXTFI2(N, FN, r0, r1, IF, IT)	\
	EXTFI2(N, FN, r0, r2, IF, IT)	\
	EXTFI2(N, FN, r1, r2, IF, IT)

/*--------------------------------------------------------------------*/
#define EXCEPT_EXTFI0(N, FN, F0, R0, R1, IF)	\
	 EXTFI(N, FN, i, F0, R0, R1, IF, i80)	\
	 EXTDI(N, FN, i, F0, R0, R1, IF, i80)	\
	 EXTFI(N, FN, l, F0, R0, R1, IF, l80)	\
	 EXTDI(N, FN, l, F0, R0, R1, IF, l80)

#define EXCEPT_EXTFI1(N, FN, R0, R1, IF)	\
	EXCEPT_EXTFI0(N, FN, f0, R0, R1, IF)	\
	EXCEPT_EXTFI0(N, FN, f1, R0, R1, IF)	\
	EXCEPT_EXTFI0(N, FN, f2, R0, R1, IF)	\
	EXCEPT_EXTFI0(N, FN, f3, R0, R1, IF)	\
	EXCEPT_EXTFI0(N, FN, f4, R0, R1, IF)	\
	EXCEPT_EXTFI0(N, FN, f5, R0, R1, IF)

#define EXCEPT_EXTFI2(N, FN, R0, R1, IF)	\
	EXCEPT_EXTFI1(N, FN, R0, R1, IF)	\
	EXCEPT_EXTFI1(N, FN, R1, R0, IF)

#define EXCEPT_EXTFI3(N, FN, IF)		\
	EXCEPT_EXTFI2(N, FN, v0, v1, IF)	\
	EXCEPT_EXTFI2(N, FN, v0, v2, IF)	\
	EXCEPT_EXTFI2(N, FN, v0, r0, IF)	\
	EXCEPT_EXTFI2(N, FN, v0, r1, IF)	\
	EXCEPT_EXTFI2(N, FN, v0, r2, IF)	\
	EXCEPT_EXTFI2(N, FN, v1, v2, IF)	\
	EXCEPT_EXTFI2(N, FN, v1, r0, IF)	\
	EXCEPT_EXTFI2(N, FN, v1, r1, IF)	\
	EXCEPT_EXTFI2(N, FN, v1, r2, IF)	\
	EXCEPT_EXTFI2(N, FN, r0, r1, IF)	\
	EXCEPT_EXTFI2(N, FN, r0, r2, IF)	\
	EXCEPT_EXTFI2(N, FN, r1, r2, IF)

/*--------------------------------------------------------------------*/
#define HTONR(V, T, I, R0, R1, H, N)	\
	movi_##I %R1 H			\
	hton_##T %R0 %R1		\
	beqi_##I V##R0##R1 %R0 N	\
	prepare 4			\
		pusharg_##I %R0		\
		pusharg_##I %R1		\
		movi_##I %R0 N		\
		pusharg_##I %R0		\
		movi_p %R0 fmth##T	\
		pusharg_p %R0		\
	finish @printf			\
V##R0##R1:

#define HTONC(V, T, I, R0, R1, H, N)	\
	movi_##I %R0 H			\
	movr_##I %R1 %R0		\
	hton_##T %R0 %R1		\
	beqi_##I V##R0##R1 %R0 N	\
	prepare 4			\
		pusharg_##I %R0		\
		pusharg_##I %R1		\
		movi_##I %R0 N		\
		pusharg_##I %R0		\
		movi_p %R0 fmth##T	\
		pusharg_p %R0		\
	finish @printf			\
V##R0##R1:

#define NTOHR(V, T, I, R0, R1, H, N)	\
	movi_##I %R1 H			\
	ntoh_##T %R0 %R1		\
	beqi_##I V##R0##R1 %R0 N	\
	prepare 4			\
		pusharg_##I %R0		\
		pusharg_##I %R1		\
		movi_##I %R0 N		\
		pusharg_##I %R0		\
		movi_p %R0 fmth##T	\
		pusharg_p %R0		\
	finish @printf			\
V##R0##R1:

#define NTOHC(V, T, I, R0, R1, H, N)	\
	movi_##I %R0 H			\
	movr_##I %R1 %R0		\
	ntoh_##T %R0 %R1		\
	beqi_##I V##R0##R1 %R0 N	\
	prepare 4			\
		pusharg_##I %R0		\
		pusharg_##I %R1		\
		movi_##I %R0 N		\
		pusharg_##I %R0		\
		movi_p %R0 fmth##T	\
		pusharg_p %R0		\
	finish @printf			\
V##R0##R1:

#define HTONS(R0, R1)				\
	HTONR(H0, us, ui, R0, R1, sh2n, sn2h)	\
	HTONC(H1, us, ui, R0, R1, sh2n, sn2h)	\
	NTOHR(H2, us, ui, R0, R1, sn2h, sh2n)	\
	NTOHC(H3, us, ui, R0, R1, sn2h, sh2n)

#define HTONI(R0, R1)				\
	HTONR(N0, ui, ui, R0, R1, ih2n, in2h)	\
	HTONC(N1, ui, ui, R0, R1, ih2n, in2h)	\
	NTOHR(N2, ui, ui, R0, R1, in2h, ih2n)	\
	NTOHC(N3, ui, ui, R0, R1, in2h, ih2n)

#define HTON1(R0, R1)				\
	HTONS(R0, R1)				\
	HTONI(R0, R1)

#define HTON2(R0, R1)				\
	HTON1(R0, R1)				\
	HTON1(R1, R0)

/*--------------------------------------------------------------------*/
#define ROUND(N, IF, IT)	EXTFI3(N, round, IF, IT)
#define TRUNC(N, IF, IT)	EXTFI3(N, trunc, IF, IT)
#define FLOOR(N, IF, IT)	EXTFI3(N, floor, IF, IT)
#define  CEIL(N, IF, IT)	EXTFI3(N, ceil,  IF, IT)

/*--------------------------------------------------------------------*/
	EXTII3(v0, v1, v2)
	EXTII3(v0, v1, r0)
	EXTII3(v0, v1, r1)
	EXTII3(v0, v1, r2)
	EXTII3(v1, v2, r0)
	EXTII3(v1, v2, r1)
	EXTII3(v1, v2, r2)
	EXTII3(v2, r0, r1)
	EXTII3(v2, r0, r2)
	EXTII3(r0, r1, r2)
	EXTIF2(f0, f1)
	EXTIF2(f0, f2)
	EXTIF2(f0, f3)
	EXTIF2(f0, f4)
	EXTIF2(f0, f5)
	EXTIF2(f1, f2)
	EXTIF2(f1, f3)
	EXTIF2(f1, f4)
	EXTIF2(f1, f5)
	EXTIF2(f2, f3)
	EXTIF2(f2, f4)
	EXTIF2(f2, f5)
	EXTIF2(f3, f4)
	EXTIF2(f3, f5)
	EXTIF2(f4, f5)
	ROUND(0,  0.25,   0)
	ROUND(1, -0.25,   0)
	ROUND(2,  1.75,   2)
	ROUND(3, -1.75,  -2)
	EXCEPT_ROUND(4,  $NaN)
	EXCEPT_ROUND(5,  $pInf)
	EXCEPT_ROUND(6,  $nInf)
	TRUNC(0,  0.25,   0)
	TRUNC(1, -0.25,   0)
	TRUNC(2,  1.75,   1)
	TRUNC(3, -1.75,  -1)
	EXCEPT_TRUNC(4,  $NaN)
	EXCEPT_TRUNC(5,  $pInf)
	EXCEPT_TRUNC(6,  $nInf)
	FLOOR(0,  0.25,   0)
	FLOOR(1, -0.25,  -1)
	FLOOR(2,  1.75,   1)
	FLOOR(3, -1.75,  -2)
	EXCEPT_FLOOR(4,  $NaN)
	EXCEPT_FLOOR(5,  $pInf)
	EXCEPT_FLOOR(6,  $nInf)
	CEIL (0,  0.25,   1)
	CEIL (1, -0.25,   0)
	CEIL (2,  1.75,   2)
	CEIL (3, -1.75,  -1)
	EXCEPT_CEIL (4,  $NaN)
	EXCEPT_CEIL (5,  $pInf)
	EXCEPT_CEIL (6,  $nInf)
	HTON2(v0, v1)
	HTON2(v0, v2)
	HTON2(v0, r0)
	HTON2(v0, r1)
	HTON2(v0, r2)
	HTON2(v1, v2)
	HTON2(v1, r0)
	HTON2(v1, r1)
	HTON2(v1, r2)
	HTON2(v2, r0)
	HTON2(v2, r1)
	HTON2(v2, r2)
	HTON2(r0, r1)
	HTON2(r0, r2)
	HTON2(r1, r2)

	ret
