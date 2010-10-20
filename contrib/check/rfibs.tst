// Recursive version - adapted from lightning.info

.data	32
format:
.c	"rfibs(%d) = %d\n"

//=======================================================================
.code	512
	jmpi main

//-----------------------------------------------------------------------
rfibs:
	prolog 1
	arg_ui $in
	getarg_ui %v0 $in	// V0 = n
	blti_ui ref %v0 2
	subi_ui %v1 %v0 1	// V1 = n-1
	subi_ui %v2 %v0 2	// V2 = n-2

	prepare 1
		pusharg_ui %v1
	finish rfibs
	retval_ui %v1		// V1 = nfibs(n-1)

	prepare 1
		pusharg_ui %v2
	finish rfibs
	retval_ui %v2		// V2 = nfibs(n-2)

	addi_ui %v1 %v1 1
	addr_ui %ret %v1 %v2	// RET = V1 + V2 + 1
	ret

ref:
	movi_i %ret 1		// RET = 1
	ret

//-----------------------------------------------------------------------
main:
	prolog 2
	arg_i $argc
	arg_p $argv

	getarg_i %r0 $argc
	blei_i default %r0 1
	getarg_p %r0 $argv
	addi_p %r0 %r0 $(__WORDSIZE >> 3)
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
	finish rfibs
	retval_i %r0
	prepare 3
		pusharg_i %r0
		pusharg_i %v0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf
	ret
