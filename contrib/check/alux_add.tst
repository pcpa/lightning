/* Expand ilogical operations.
 *	addc,addx and subc,subx are only defined for unsigned values
 *
 *	boadd and bosub are already properly implemented as:
 *		bo{add,sub}{r,i}_{i,l}
 *			branch if overflow
 *		bo{add,sub}{r,i}_{ui,ul}
 *			branch if carry
 */
#define TEST_ILLOGICAL	1

/* Print extra register name information after an error */
#define DEBUG_REGISTERS	1

#include "alu.inc"

.code	$(4 * 1024 * 1024)
	prolog 0

#define ADDX(N, T, I0, I1, V)		ALUX(N, T, add, I0, I1, V)
#define UADDX(N, T, I0, I1, V)		UALUX(N, T, add, I0, I1, V)
	/* nothing */
	UADDX(0, i,	0,		0,		0)
	ADDX(0, ui,	0,		0,		0)
	/* carry */
	UADDX(1, i,	0xffffffff,	0xffffffff,	1)
	ADDX(1, ui,	0xffffffff,	0xffffffff,	1)
	/* overflow */
	UADDX(2, i,	0x7fffffff,	1,		0)
	ADDX(2, ui,	0x7fffffff,	1,		0)
	/* overflow */
	UADDX(3, i,	0x7fffffff,	0x7fffffff,	0)
	ADDX(3, ui,	0x7fffffff,	0x7fffffff,	0)
	/* overflow */
	UADDX(4, i,	0x7fffffff,	0x80000000,	0)
	ADDX(4, ui,	0x7fffffff,	0x80000000,	0)
	/* carry+overflow */
	UADDX(5, i,	0x80000000,	0x80000000,	1)
	ADDX(5, ui,	0x80000000,	0x80000000,	1)
#if __WORDSIZE == 32
	/* nothing */
	UADDX(0, l,	0,		0,		0)
	ADDX(0, ul,	0,		0,		0)
	/* carry */
	UADDX(1, l,	0xffffffff,	0xffffffff,	1)
	ADDX(1, ul,	0xffffffff,	0xffffffff,	1)
	/* overflow */
	UADDX(2, l,	0x7fffffff,	1,		0)
	ADDX(2, ul,	0x7fffffff,	1,		0)
	/* overflow */
	UADDX(3, l,	0x7fffffff,	0x7fffffff,	0)
	ADDX(3, ul,	0x7fffffff,	0x7fffffff,	0)
	/* carry */
	UADDX(4, l,	0x7fffffff,	0x80000000,	0)
	ADDX(4, ul,	0x7fffffff,	0x80000000,	0)
	/* carry+overflow */
	UADDX(5, l,	0x80000000,	0x80000000,	1)
	ADDX(5, ul,	0x80000000,	0x80000000,	1)
#else
	/* nothing */
	UADDX(0, l,	0,		0,		0)
	ADDX(0, ul,	0,		0,		0)
	/* nothing */
	UADDX(1, l,	0xffffffff,		0xffffffff,		0)
	ADDX(1, ul,	0xffffffff,		0xffffffff,		0)
	/* nothing */
	UADDX(2, l,	0x7fffffff,		1,			0)
	ADDX(2, ul,	0x7fffffff,		1,			0)
	/* nothing */
	UADDX(3, l,	0x7fffffff,		0x7fffffff,		0)
	ADDX(3, ul,	0x7fffffff,		0x7fffffff,		0)
	/* nothing */
	UADDX(4, l,	0x7fffffff,		0x80000000,		0)
	ADDX(4, ul,	0x7fffffff,		0x80000000,		0)
	/* nothing */
	UADDX(5, l,	0x80000000,		0x80000000,		0)
	ADDX(5, ul,	0x80000000,		0x80000000,		0)
	/* carry */
	UADDX(6, l,	0xffffffffffffffff,	0xffffffffffffffff,	1)
	ADDX(6, ul,	0xffffffffffffffff,	0xffffffffffffffff,	1)
	/* overflow */
	UADDX(7, l,	0x7fffffffffffffff,	1,			0)
	ADDX(7, ul,	0x7fffffffffffffff,	1,			0)
	/* overflow */
	UADDX(8, l,	0x7fffffffffffffff,	0x7fffffffffffffff,	0)
	ADDX(8, ul,	0x7fffffffffffffff,	0x7fffffffffffffff,	0)
	/* overflow */
	UADDX(9, l,	0x7fffffffffffffff,	0x8000000000000000,	0)
	ADDX(9, ul,	0x7fffffffffffffff,	0x8000000000000000,	0)
	/* carry+overflow */
	UADDX(10, l,	0x8000000000000000,	0x8000000000000000,	1)
	ADDX(10, ul,	0x8000000000000000,	0x8000000000000000,	1)
#endif

	ret
