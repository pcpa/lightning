.data	32
format:
.c "%d + 1 = %d\n"
.$($num = 5)

.code	256
	jmpi main

incr:
	leaf 1
	arg_i $in
	getarg_i %r0 $in
	addi_i %ret %r0 1
	ret

main:
	prolog 0

	prepare 1
		movi_i %r0 $num
		pusharg_i %r0
	finish incr

	prepare 3
		pusharg_i %ret
		movi_i %r0 $num
		pusharg_i %r0
		movi_p %r0 format
		pusharg_p %r0
	finish @printf

	ret
