// allocate 512 bytes for data

.data	512

/* variables can be created anywhere
 * but a expression without side effects (other then creating the variable)
 * need help of '.'
 */
.$($bottles = 99)

many_bottles:
.c "%d bottles of beer on the wall,\n%d bottles of beer,\n"
less_bottles:
.c "take one down, pass it around,\n%d bottles of beer on the wall.\n\n"

two_bottles:
.c "take one down, pass it around,\none bottle of beer on the wall.\n\n"

one_bottle:
.c "one bottle of beer on the wall,\none bottle of beer,\n"
last_one:
.c "take one down, pass it around,\nno more bottles of beer on the wall.\n\n"

no_more:
.c "no more bottles of beer on the wall,\nno more bottles of beer,\n"
buy_more:
.c "go to the store and buy some more,\n%d bottles of beer on the wall.\n\n"

// allocate 256 bytes for code
.code	512
	prolog 0
	// keep bottles in %v0
	movi_i %v0 $bottles

use_bottles:
	blti_i out_of_bottles %v0 1
	beqi_i only_one_left %v0 1

	prepare 3
		pusharg_i %v0
		pusharg_i %v0
		movi_p %r0 many_bottles
		pusharg_p %r0
	finish @printf

	subi_i %v0 %v0 1

	beqi_i only_two_left %v0 1

	prepare 2
		pusharg_i %v0
		movi_p %r0 less_bottles
		pusharg_p %r0
	finish @printf

	// loop
	jmpi use_bottles

only_two_left:
	prepare 1
		movi_p %r0 two_bottles
		pusharg_p %r0
	finish @printf

	subi_i %v0 %v0 1

only_one_left:
	prepare 1
		movi_p %r0 one_bottle
		pusharg_p %r0
	finish @printf
	prepare 1
		movi_p %r0 last_one
		pusharg_p %r0
	finish @printf

out_of_bottles:
	prepare	1
		movi_p %r0 no_more
		pusharg_p %r0
	finish @printf
	prepare 2
		movi_i %r0 $bottles
		pusharg_i %r0
		movi_p %r0 buy_more
		pusharg_p %r0
	finish @printf
	ret
