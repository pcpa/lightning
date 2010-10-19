.data	32
hello:
.c	"Hello world!\n"
.code	256
	prolog 0
	prepare 1
		movi_p %r0 hello
		pusharg_p %r0
	finish @printf
	ret
