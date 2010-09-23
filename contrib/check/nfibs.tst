// Non recursive version - adapted from lightning.info

.data	32
void_start:
.p	0
void_end:
// ... get sizeof(void *) - should have a predefined value...
.$($pointer = void_end - void_start)
format:
.c	"nfibs(%d) = %d\n"

//=======================================================================
.code	256
	jmpi main

//-----------------------------------------------------------------------
nfibs:
	leaf 1
	arg_ui $in
	getarg_ui %r2 $in	// R2 = n
	movi_ui %r1 1
	blti_ui ref %r2 2
	subi_ui %r2 %r2 1
	movi_ui %r0 1
loop:
	subi_ui %r2 %r2 1	// decr. counter
	addr_ui %v0 %r0 %r1	// V0 = R0 + R1
	movr_ui %r0 %r1		// R0 = R1
	addi_ui %r1 %v0 1	// R1 = V0 + 1
	bnei_ui loop %r2 0	// if (R2) goto loop
ref:
	movr_ui %ret %r1	// RET = R1
	ret

//-----------------------------------------------------------------------
main:
	prolog 2
	arg_i $argc
	arg_p $argv

	getarg_i %r0 $argc
	blei_i default %r0 1
	getarg_p %r0 $argv
	addi_p %r0 %r0 $pointer
	ldr_p %r0 %r0
	prepare 1
		pusharg_p %r0
	finish @atoi
	retval_i %r0
	jmpi call

default:
	movi_i %r0 36

call:
	movr_i %v0 %r0
	prepare 1
		pusharg_i %r0
	finish nfibs
	retval_i %r0
	prepare 3
		pusharg_i %r0
		pusharg_i %v0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf
	ret
