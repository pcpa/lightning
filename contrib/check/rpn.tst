//=======================================================================

.data	256
.$($int = 4)
C:
.c "\nC:"
F:
.c "\nF:"
format:
.c "%3d "
newline:
.c "\n"

//=======================================================================
.code	4096
	jmpi main

//-----------------------------------------------------------------------
c2f:
	leaf 1
	arg_i $in

	allocai $(32 * $int) $index

	getarg_i %r2 $in

	// 32x9*5/+
	movi_i %r0 32

	// x9*5/+
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movr_i %r0 %r2

	// 9*5/+
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movi_i %r0 9

	// *5/+
.	$($index = $index - $int)
	ldxi_i %r1 %fp $index
	mulr_i %r0 %r1 %r0

	// 5/+
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movi_i %r0 5

	// /+
.	$($index = $index - $int)
	ldxi_i %r1 %fp $index
	divr_i %r0 %r1 %r0

	// +
.	$($index = $index - $int)
	ldxi_i %r1 %fp $index
	addr_i %r0 %r1 %r0

	movr_i %ret %r0
	ret

//-----------------------------------------------------------------------
f2c:
	leaf 1
	arg_i $in

	allocai $(32 * $int) $index

	getarg_i %r2 $in

	// x32-5*9/
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movr_i %r0 %r2

	// 32-5*9/
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movi_i %r0 32

	// -5*9/
.	$($index = $index - $int)
	ldxi_i %r1 %fp $index
	subr_i %r0 %r1 %r0

	// 5*9/
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movi_i %r0 5

	// *9/
.	$($index = $index - $int)
	ldxi_i %r1 %fp $index
	mulr_i %r0 %r1 %r0

	// 9/
	stxi_i $index %fp %r0
.	$($index = $index + $int)
	movi_i %r0 9

	// /
.	$($index = $index - $int)
	ldxi_i %r1 %fp $index
	divr_i %r0 %r1 %r0

	movr_i %ret %r0
	ret

//-----------------------------------------------------------------------
main:
	prolog 0

	prepare 1
		movi_p %r0 C
		pusharg_p %r0
	finish @printf
	movi_i %v0 0
loopC:
	prepare 2
		pusharg_i %v0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf
	addi_i %v0 %v0 10
	blei_i loopC %v0 100
	prepare 1
		movi_p %r0 F
		pusharg_p %r0
	finish @printf
	movi_i %v0 0
loopC2F:
	prepare 1
		pusharg_i %v0
	finish c2f
	retval_i %r0
	prepare 2
		pusharg_i %r0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf
	addi_i %v0 %v0 10
	blei_i loopC2F %v0 100
	prepare 1
		movi_p %r0 newline
		pusharg_p %r0
	finish @printf

	prepare 1
		movi_p %r0 F
		pusharg_p %r0
	finish @printf
	movi_i %v0 32
loopF:
	prepare 2
		pusharg_i %v0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf
	addi_i %v0 %v0 18
	blei_i loopF %v0 212
	prepare 1
		movi_p %r0 C
		pusharg_p %r0
	finish @printf
	movi_i %v0 32
loopF2C:
	prepare 1
		pusharg_i %v0
	finish f2c
	retval_i %r0
	prepare 2
		pusharg_i %r0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf
	addi_i %v0 %v0 18
	blei_i loopF2C %v0 212
	prepare 1
		movi_p %r0 newline
		pusharg_p %r0
	finish @printf

	ret
