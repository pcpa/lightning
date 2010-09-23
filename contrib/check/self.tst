// test basic features of testing tool itself
.data	0100
i_format:
.c	"%2d: %3d    == %3d\n"
f_format:
.c	"%2d: %6.2f == %6.2f\n"

.code	0x1000
	prolog 0

#define basic(count, value) \
	prepare 4 \
		movi_i %r0 $t \
		pusharg_i %r0 \
		movi_i %r0 value \
		pusharg_i %r0 \
		movi_i %r0 count \
		pusharg_i %r0 \
		movi_p %r0 i_format \
		pusharg_p %r0 \
	finish @printf
// test store
.	$($t = 1)
	basic( 0, 1)

// first test on setting value again
.	$($t *= 2)
	basic( 1, 2)

.	$($t += 2)
	basic( 2, 4)

.	$($t <<= 1)
	basic( 3, 8)

// test parenthesis
.	$($t = ($t - 1) & 3)
	basic( 4, 3)

.	$($t |= 4)
	basic( 5, 7)

.	$($t ^= 15)
	basic( 6, 8)

.	$($t = $t < 1)
	basic( 7, 0)

.	$($t = $t <= 1)
	basic( 8, 1)

.	$($t = $t == 1)
	basic( 9, 1)

.	$($t = $t >= 0)
	basic(10, 1)

.	$($t = $t > 1)
	basic(11, 0)

.	$($t = !$t)
	basic(12, 1)

.	$($t = ~$t)
	basic(13, -2)

.	$($t = -$t)
	basic(14, 2)

.	$($t = $t && 0)
	basic(15, 0)

.	$($t = $t || 1)
	basic(16, 1)

// $v set to zero
.	$($v = $t && 0)
	basic(17, 1)

// test short circuit side effect, that in this case, should not set $t
.	$($v && $t = 0)
	basic(18, 1)

// remaining operations
.	$($t = 15)
.	$($v = 4)
.	$($t /= $v)
	basic(19, 3)
.	$($t = 19)
.	$($v = 5)
.	$($t %= $v)
	basic(20, 4)

// extra integer tests
.	$($t = '@' | 1)
	basic(21, 'A')

.	$($t = 1)
.	$($v = ++$t)
	basic(22, 2)

.	$($t = $v--)
	basic(23, 2)

.	$($t = $v)
	basic(24, 1)

// some float operation tests
#undef basic
#define basic(count, value) \
	prepare 2 \
	prepare_d 2 \
		movi_d %f0 $t \
		pusharg_d %f0 \
		movi_d %f0 value \
		pusharg_d %f0 \
		movi_i %r0 count \
		pusharg_i %r0 \
		movi_p %r0 f_format \
		pusharg_p %r0 \
	finish @printf

.	$($t = 0.5)
	basic( 0, 0.5)

.	$($t *= 0.5)
	basic( 1, 0.25)

.	$($t = 4 * $t)
	basic( 2, 1)

.	$($v = 0b1)
.	$($t += $v)
	basic( 3, 2)

.	$($t = 5 - $v)
	basic( 4, 4.0)

.	$($t = ~5)
	basic( 5, -6.0)

.	$($t = 2 + $t * 4)
	basic( 6, -22.0)

.	$($t = -(($t - 3) * 2 + 2) / 3 + 3)
	basic( 7, 19.0)

.	$($t = i_format - f_format)
.	$($v = f_format - i_format)
	basic( 8, $((-$v * 2) >> 1))

.	$($t = 1e2)
	basic( 9, 1e2)

.	$($t = 1e-2)
	basic( 10, 1e-2)
L:
.	$($t = 1e+1)
	basic( 11, 1e+1)

	ret
