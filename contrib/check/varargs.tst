#include "float_branch.inc"

/*====================================================================*/
.data	1024
ierr:
.c	"integer error at offset %d\n"
.align	4
ival:
.i	0 1 2 3 4 5 6 7 8 9
ichk:
.i	9 8 7 6 5 4 3 2 1 0
ifmt:
.c	"%d %d %d %d %d %d %d %d %d %d\n"

derr:
.c	"float error at offset %d\n"
.align	8
dval:
.d	0 1 2 3 4 5 6 7 8 9
dchk:
.d	9 8 7 6 5 4 3 2 1 0
dfmt:
.c	"%f %f %f %f %f %f %f %f %f %f\n"
dsfmt:
.c	"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n"

idfmt:
.c	"%d %f %d %f %d %f %d %f %d %f %d %f %d %f %d %f %d %f %d %f\n"
idsfmt:
.c	"%d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf %d %lf\n"

buff:
.size	256


/*====================================================================*/
.code	8192
	jmpi main

/*--------------------------------------------------------------------*/
test_id:
	prolog 20
	arg_i $iarg0
	arg_d $darg0
	arg_i $iarg1
	arg_d $darg1
	arg_i $iarg2
	arg_d $darg2
	arg_i $iarg3
	arg_d $darg3
	arg_i $iarg4
	arg_d $darg4
	arg_i $iarg5
	arg_d $darg5
	arg_i $iarg6
	arg_d $darg6
	arg_i $iarg7
	arg_d $darg7
	arg_i $iarg8
	arg_d $darg8
	arg_i $iarg9
	arg_d $darg9
#define check_arg_id(num)					\
	movi_i %r2 num						\
	getarg_i %r0 $iarg##num					\
	bnei_i fail_test_id_i %r0 num				\
	movi_d %f1 num						\
	getarg_d %f0 $darg##num					\
	double_cmp_branch_false(fail_test_id_d, r0, f0, f1)
	check_arg_id(0)
	check_arg_id(1)
	check_arg_id(2)
	check_arg_id(3)
	check_arg_id(4)
	check_arg_id(5)
	check_arg_id(6)
	check_arg_id(7)
	check_arg_id(8)
	check_arg_id(9)
	ret
fail_test_id_i:
	prepare 2
		pusharg_i %r2
		movi_p %r0 ierr
		pusharg_p %r0
	finish @printf
	ret
fail_test_id_d:
	prepare 2
		pusharg_i %r2
		movi_p %r0 derr
		pusharg_p %r0
	finish @printf
	ret

/*--------------------------------------------------------------------*/
test_i:
	prolog 10
	arg_i $iarg0
	arg_i $iarg1
	arg_i $iarg2
	arg_i $iarg3
	arg_i $iarg4
	arg_i $iarg5
	arg_i $iarg6
	arg_i $iarg7
	arg_i $iarg8
	arg_i $iarg9
#define check_arg_i(num)					\
	movi_i %r2 num						\
	getarg_i %r0 $iarg##num					\
	bnei_i fail_test_i %r0 num
	check_arg_i(0)
	check_arg_i(1)
	check_arg_i(2)
	check_arg_i(3)
	check_arg_i(4)
	check_arg_i(5)
	check_arg_i(6)
	check_arg_i(7)
	check_arg_i(8)
	check_arg_i(9)
	ret
fail_test_i:
	prepare 2
		pusharg_i %r2
		movi_p %r0 ierr
		pusharg_p %r0
	finish @printf
	ret

/*--------------------------------------------------------------------*/
test_d:
	prolog 0
	prolog_d 10
	arg_d $darg0
	arg_d $darg1
	arg_d $darg2
	arg_d $darg3
	arg_d $darg4
	arg_d $darg5
	arg_d $darg6
	arg_d $darg7
	arg_d $darg8
	arg_d $darg9
#define check_arg_d(num)					\
	movi_i %r2 num						\
	movi_d %f1 num						\
	getarg_d %f0 $darg##num					\
	double_cmp_branch_false(fail_test_d, r0, f0, f1)
	check_arg_d(0)
	check_arg_d(1)
	check_arg_d(2)
	check_arg_d(3)
	check_arg_d(4)
	check_arg_d(5)
	check_arg_d(6)
	check_arg_d(7)
	check_arg_d(8)
	check_arg_d(9)
	ret
fail_test_d:
	prepare 2
		pusharg_i %r2
		movi_p %r0 derr
		pusharg_p %r0
	finish @printf
	ret

/*--------------------------------------------------------------------*/
main:
	prolog 0

/*
 * Test1:
 *	store in a string the value:
 *		"0 1 2 3 4 5 6 7 8 9\n"
 *	parse string and check that the read values match
 */

	/*
	    sprintf(buff, "%d %d %d %d %d %d %d %d %d %d\n",
		    0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	 */
	prepare 12
		movi_p %v0 ival
#define push_i(off)			\
		ldxi_i %r0 %v0 off	\
		pusharg_i %r0
		push_i(36)
		push_i(32)
		push_i(28)
		push_i(24)
		push_i(20)
		push_i(16)
		push_i(12)
		push_i(8)
		push_i(4)
#undef push_i
		ldr_i %r0 %v0
		pusharg_i %r0
		movi_p %r0 ifmt
		pusharg_p %r0
		movi_p %r0 buff
		pusharg_p %r0
	finish @sprintf

	/*
		sscanf(buff, "%d %d %d %d %d %d %d %d %d %d\n",
		       ichk+0, ichk+1, ichk+2, ichk+3, ichk+4,
		       ichk+5, ichk+6, ichk+7, ichk+8, ichk+9);
	 */
	prepare 12
		movi_p %v0 ichk
#define push_i(off)			\
		addi_p %r0 %v0 off	\
		pusharg_p %r0
		push_i(36)
		push_i(32)
		push_i(28)
		push_i(24)
		push_i(20)
		push_i(16)
		push_i(12)
		push_i(8)
		push_i(4)
#undef push_i
		pusharg_p %v0
		movi_p %r0 ifmt
		pusharg_p %r0
		movi_p %r0 buff
		pusharg_p %r0
	finish @sscanf

	/*
		v0 = ival;
		v1 = ichk;
		r2 = 0;
	 */
	movi_p %v0 ival
	movi_p %v1 ichk
	movi_i %r2 0

loop_i:
	/*
	loop:
		r0 = v0[r2];
		r1 = v1[r2];
		if (r0 != r1)
			goto fail_i;
		r2 += sizeof(int);
		if (r2 >= 40)
			goto scan_d;
		goto loop_i;
	 */
	ldxr_i %r0 %v0 %r2
	ldxr_i %r1 %v1 %r2
	bner_i fail_i %r0 %r1
	addi_i %r2 %r2 4
	bgei_i scan_d %r2 40
	jmpi loop_i

fail_i:
	/*
	fail:
		printf("integer error at offset %d\n", r2);
	 */
	prepare 2
		pusharg_i %r2
		movi_p %r0 ierr
		pusharg_p %r0
	finish @printf



/*
 * Test2:
 *	store in a string the value:
 *		"0.0 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0\n"
 *	parse string and check that the read values match
 */
scan_d:
	/*
	    sprintf(buff, "%f %f %f %f %f %f %f %f %f %f\n",
		    0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
	 */
	prepare 2
	prepare_d 10
		movi_p %v0 dval
#define push_d(off)			\
		ldxi_d %f0 %v0 off	\
		pusharg_d %f0
		push_d(72)
		push_d(64)
		push_d(56)
		push_d(48)
		push_d(40)
		push_d(32)
		push_d(24)
		push_d(16)
		push_d(8)
#undef push_d
		ldr_d %f0 %v0
		pusharg_d %f0
		movi_p %r0 dfmt
		pusharg_p %r0
		movi_p %r0 buff
		pusharg_p %r0
	finish @sprintf

	/*
		sscanf(buff, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
		       dchk+0, dchk+1, dchk+2, dchk+3, dchk+4,
		       dchk+5, dchk+6, dchk+7, dchk+8, dchk+9);
	 */
	prepare 12
		movi_p %v0 dchk
#define push_d(off)			\
		addi_p %r0 %v0 off	\
		pusharg_p %r0
		push_d(72)
		push_d(64)
		push_d(56)
		push_d(48)
		push_d(40)
		push_d(32)
		push_d(24)
		push_d(16)
		push_d(8)
#undef push_d
		pusharg_p %v0
		movi_p %r0 dsfmt
		pusharg_p %r0
		movi_p %r0 buff
		pusharg_p %r0
	finish @sscanf


	/*
		v0 = ival;
		v1 = ichk;
		r2 = 0;
	 */
	movi_p %v0 dval
	movi_p %v1 dchk
	movi_i %r2 0

loop_d:
	/*
	loop:
		f0 = v0[r2];
		f1 = v1[r2];
		if (f0 != f1)
			goto fail_d;
		r2 += sizeof(double);
		if (r2 >= 80)
			goto call_test_i;
		goto loop_d;
	 */
	ldxr_d %f0 %v0 %r2
	ldxr_d %f1 %v1 %r2
	double_cmp_branch_false(fail_d, r0, f0, f1)
	addi_i %r2 %r2 8
	bgei_i call_test_i %r2 80
	jmpi loop_d

fail_d:
	/*
	fail:
		printf("float error at offset %d\n", r0);
	 */
	prepare 2
		pusharg_i %r2
		movi_p %r0 derr
		pusharg_p %r0
	finish @printf


/*
 * Test3:
 *	call jit function test_i as:
 *		test_i(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
 *	and test_i checks that it receives those arguments
 */
call_test_i:
	prepare 10
#define push_i(num)						\
		movi_i %r0 num					\
		pusharg_i %r0
		push_i(9)
		push_i(8)
		push_i(7)
		push_i(6)
		push_i(5)
		push_i(4)
		push_i(3)
		push_i(2)
		push_i(1)
		push_i(0)
#undef push_i
	finish test_i


/*
 * Test4:
 *	call jit function test_d as:
 *		test_d(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0)
 *	and test_d checks that it receives those arguments
 */
call_test_d:
	prepare 0
	prepare_d 10
#define push_d(num)						\
		movi_d %f0 num					\
		pusharg_d %f0
		push_d(9)
		push_d(8)
		push_d(7)
		push_d(6)
		push_d(5)
		push_d(4)
		push_d(3)
		push_d(2)
		push_d(1)
		push_d(0)
#undef push_d
	finish test_d


/*
 * Test4:
 *	call jit function test_id as:
 *		test_id(0, 0.0, 1, 1.0, 2, 2.0, 3, 3.0, 4, 4.0,
 *			5, 5.0, 6, 6.0, 7, 7.0, 8, 8.0, 9, 9.0)
 *	and test_id checks that it receives those arguments
 */
call_test_id:
	prepare 10
	prepare_d 10
#define push_id(num)						\
		movi_d %f0 num					\
		pusharg_d %f0					\
		movi_i %r0 num					\
		pusharg_i %r0
		push_id(9)
		push_id(8)
		push_id(7)
		push_id(6)
		push_id(5)
		push_id(4)
		push_id(3)
		push_id(2)
		push_id(1)
		push_id(0)
#undef push_id
	finish test_id

/*
 * Test5:
 *	store in a string the value:
 *		"0 0.0 1 1.0 2 2.0 3 3.0 4 4.0 5 5.0 6 6.0 7 7.0 8 8.0 9 9.0\n"
 *	parse string and check that the read values match
 */
	/*
	    sprintf(buff,
		   "%d %f %d %f %d %f %d %f %d %f "
		   "%d %f %d %f %d %f %d %f %d %f\n",
		   0, 0.0, 1, 1.0, 2, 2.0, 3, 3.0, 4, 4.0,
		   5, 5.0, 6, 6.0, 7, 7.0, 8, 8.0, 9, 9.0);
	 */
	prepare 12
	prepare_d 10
		movi_p %v0 ival
		movi_p %v1 dval
#define push_id(off)				\
		ldxi_d %f0 %v1 $(off << 1)	\
		pusharg_d %f0			\
		ldxi_i %r0 %v0 off		\
		pusharg_i %r0
		push_id(36)
		push_id(32)
		push_id(28)
		push_id(24)
		push_id(20)
		push_id(16)
		push_id(12)
		push_id(8)
		push_id(4)
#undef push_id
		ldr_d %f0 %v1
		pusharg_d %f0
		ldr_i %r0 %v0
		pusharg_i %r0
		movi_p %r0 idfmt
		pusharg_p %r0
		movi_p %r0 buff
		pusharg_p %r0
	finish @sprintf

	/*
		sscanf(buff,
		      "%d %lf %d %lf %d %lf %d %lf %d %lf "
		      "%d %lf %d %lf %d %lf %d %lf %d %lf\n",
		      ichk+0, dchk+0, ichk+1, dchk+1, ichk+2,
		      dchk+2, ichk+3, dchk+3, ichk+4, dchk+4,
		      ichk+5, dchk+5, ichk+6, dchk+6, ichk+7,
		      dchk+7, ichk+8, dchk+8, ichk+9, dchk+9);
	 */
	prepare 22
		movi_p %v0 ichk
		movi_p %v1 ichk
#define push_id(off)				\
		addi_p %r0 %v1 $(off << 1)	\
		pusharg_p %r0			\
		addi_p %r0 %v0 off		\
		pusharg_p %r0
		push_id(36)
		push_id(32)
		push_id(28)
		push_id(24)
		push_id(20)
		push_id(16)
		push_id(12)
		push_id(8)
		push_id(4)
#undef push_id
		pusharg_p %v1
		pusharg_p %v0
		movi_p %r0 ifmt
		pusharg_p %r0
		movi_p %r0 buff
		pusharg_p %r0
	finish @sscanf

	movi_p %v0 ival
	movi_p %v1 ichk
	movi_i %r2 0
loop_id_i:
	ldxr_i %r0 %v0 %r2
	ldxr_i %r1 %v1 %r2
	bner_i fail_id_i %r0 %r1
	addi_i %r2 %r2 4
	bgei_i check_id_d %r2 40
	jmpi loop_id_i
fail_id_i:
	prepare 2
		pusharg_i %r2
		movi_p %r0 ierr
		pusharg_p %r0
	finish @printf

check_id_d:
	movi_p %v0 dval
	movi_p %v1 dchk
	movi_i %r2 0
loop_id_d:
	ldxr_d %f0 %v0 %r2
	ldxr_d %f1 %v1 %r2
	double_cmp_branch_false(fail_d, r0, f0, f1)
	addi_i %r2 %r2 8
	bgei_i done %r2 80
	jmpi loop_id_d
fail_id_d:
	prepare 2
		pusharg_i %r2
		movi_p %r0 derr
		pusharg_p %r0
	finish @printf

done:
	ret
