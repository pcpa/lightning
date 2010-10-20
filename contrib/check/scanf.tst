.data	256
char:
.c	0
.align	4
int:
.i	0
.align	8
float:
.d	0.75
string:
.c	"01234567890123456789012345678901234567890123456789"
ask_char:
.c	"Type a character: "
get_char:
.c	"%c"
show_char:
.c	"Value read is '%c'\n"
ask_int:
.c	"Type an integer: "
get_int:
.c	"%d"
show_int:
.c	"Value read is %d\n"
ask_float:
.c	"Type a float: "
get_float:
.c	"%lf"
show_float:
.c	"Value read is %f\n"
ask_string:
.c	"Type a string: "
get_string:
.c	"%50s"
show_string:
.c	"Value read is \"%s\"\n"

.code	1024
main:
	prolog 0
	prepare 1
		movi_p %r0 ask_char
		pusharg_p %r0
	finish @printf
	prepare 2
		movi_p %r0 char
		pusharg_p %r0
		movi_p %r0 get_char
		pusharg_p %r0
	finish @scanf
	prepare 2
		movi_p %r0 char
		ldr_c %r0 %r0
		pusharg_c %r0
		movi_p %r0 show_char
		pusharg_p %r0
	finish @printf

	prepare 1
		movi_p %r0 ask_int
		pusharg_p %r0
	finish @printf
	prepare 2
		movi_p %r0 int
		pusharg_p %r0
		movi_p %r0 get_int
		pusharg_p %r0
	finish @scanf
	prepare 2
		movi_p %r0 int
		ldr_i %r0 %r0
		pusharg_i %r0
		movi_p %r0 show_int
		pusharg_p %r0
	finish @printf

	prepare 1
		movi_p %r0 ask_float
		pusharg_p %r0
	finish @printf
	prepare 2
		movi_p %r0 float
		pusharg_p %r0
		movi_p %r0 get_float
		pusharg_p %r0
	finish @scanf
	prepare 1
	prepare_d 1
		movi_p %r0 float
		ldr_d %f0 %r0
		pusharg_d %f0
		movi_p %r0 show_float
		pusharg_p %r0
	finish @printf

	prepare 1
		movi_p %r0 ask_string
		pusharg_p %r0
	finish @printf
	prepare 2
		movi_p %r0 string
		pusharg_p %r0
		movi_p %r0 get_string
		pusharg_p %r0
	finish @scanf
	prepare 2
		movi_p %r0 string
		pusharg_p %r0
		movi_p %r0 show_string
		pusharg_p %r0
	finish @printf

	ret
