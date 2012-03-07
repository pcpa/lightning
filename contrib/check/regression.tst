/* Test known regression that were not detected by the existing
 * test cases but were detected later. Bugs already corrected but
 * better to ensure it has been commited and verified by make check.
 */
.data	32
regression:
.c	"%d: regression!\n"
.code	256
	prolog 0

#define printf_regression()						\
	movi_i %r0 $line						\
	movi_p %r1 regression						\
	prepare 2							\
		pusharg_i %r0						\
		pusharg_p %r1						\
	finish @printf

#if __arm__
	/* bad encoding for sub.w */
	movi_i %r1 1
.$($line = __LINE__)
	subi_i %r0 %r1 512
	beqi_i sub_w_ok %r0 -511
	printf_regression()
sub_w_ok:

	movi_i %r1 0x80000000
.$($line = __LINE__)
	rshi_i %r0 %r1 31
	beqi_i asr_ok %r0 -1
	printf_regression()
asr_ok:

	movi_i %r0 0x5a5a5a5a
.$($line = __LINE__)
	subi_p %sp %sp 8
	stxi_i 4 %sp %r0
	ldxi_i %r1 %sp 4
	beqr_i spidx_ok1 %r0 %r1
	printf_regression()
spidx_ok1:

	movi_i %r0 0xa5a5a5a5
.$($line = __LINE__)
	movr_i %r2 %sp
	stxi_i 4 %r2 %r0
	ldxi_i %r1 %sp 4
	beqr_i spidx_ok %r0 %r1
	printf_regression()
spidx_ok:
	addi_i %sp %sp 8

#endif
	ret
