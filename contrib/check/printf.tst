.data	32
format:
.c "generated %d bytes\n"

.code	512
start:
	jmpi main

myFunction:
	prolog 1
	arg_i $in
	movi_p %r0 format
	getarg_i %r1 $in
	prepare 2
		pusharg_i %r1
		pusharg_p %r0
	finish @printf
	ret

main:
	prolog 0
/* FIXME (but unlikely for a testing tool...)
 * no support for patching forward arguments of expressions
 * so, this does not really account all the code
 */
	prepare 1
end:
	movi_i %r0 $(end - start)
		pusharg_i %r0
	finish myFunction
	ret
