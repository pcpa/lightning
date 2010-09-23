#define TEST_ILLOGICAL	1
#define DEBUG_REGISTERS	1
#include "alu.inc"

.code	$(4 * 1024 * 1024)
	prolog 0

#define SUBX(N, T, I0, I1, V)		ALUX(N, T, sub, I0, I1, V)
#define USUBX(N, T, I0, I1, V)		UALUX(N, T, sub, I0, I1, V)
	/* nothing */
	USUBX(0, i,	0,		0,		0)
	SUBX(0, ui,	0,		0,		0)
	/* carry */
	USUBX(1, i,	0x7fffffff,	0xffffffff,	-1)
	SUBX(1, ui,	0x7fffffff,	0xffffffff,	-1)
	/* overflow */
	USUBX(2, i,	0x80000000,	1,		0)
	SUBX(2, ui,	0x80000000,	1,		0)
	/* carry */
	USUBX(3, i,	0x7fffffff,	0x80000000,	-1)
	SUBX(3, ui,	0x7fffffff,	0x80000000,	-1)
	/* overflow */
	USUBX(4, i,	0x80000000,	0x7fffffff,	0)
	SUBX(4, ui,	0x80000000,	0x7fffffff,	0)
	/* carry+overflow */
	USUBX(5, i,	1,		0x80000000,	-1)
	SUBX(5, ui,	1,		0x80000000,	-1)
#if __WORDSIZE == 32
	/* nothing */
	USUBX(0, l,	0,		0,		0)
	SUBX(0, ul,	0,		0,		0)
	/* carry */
	USUBX(1, l,	0x7fffffff,	0xffffffff,	0xffffffff)
	SUBX(1, ul,	0x7fffffff,	0xffffffff,	0xffffffff)
	/* overflow */
	USUBX(2, l,	0x80000000,	1,		0)
	SUBX(2, ul,	0x80000000,	1,		0)
	/* carry */
	USUBX(3, l,	0x7fffffff,	0x80000000,	0xffffffff)
	SUBX(3, ul,	0x7fffffff,	0x80000000,	0xffffffff)
	/* overflow */
	USUBX(4, l,	0x80000000,	0x7fffffff,	0)
	SUBX(4, ul,	0x80000000,	0x7fffffff,	0)
	/* carry+overflow */
	USUBX(5, l,	1,		0x80000000,	0xffffffff)
	SUBX(5, ul,	1,		0x80000000,	0xffffffff)
#else
	/* nothing */
	USUBX(0, l,	0,			0,			0)
	SUBX(0, ul,	0,			0,			0)
	/* carry */
	USUBX(1, l,	0x7fffffff,		0xffffffff,		-1)
	SUBX(1, ul,	0x7fffffff,		0xffffffff,		-1)
	/* nothing */
	USUBX(2, l,	0x80000000,		1,			0)
	SUBX(2, ul,	0x80000000,		1,			0)
	/* carry */
	USUBX(3, l,	0x7fffffff,		0x80000000,		-1)
	SUBX(3, ul,	0x7fffffff,		0x80000000,		-1)
	/* nothing */
	USUBX(4, l,	0x80000000,		0x7fffffff,		0)
	SUBX(4, ul,	0x80000000,		0x7fffffff,		0)
	/* carry */
	USUBX(5, l,	1,			0x80000000,		-1)
	SUBX(5, ul,	1,			0x80000000,		-1)
	/* carry */
	USUBX(6, l,	0x7fffffffffffffff,	0xffffffffffffffff,	-1)
	SUBX(6, ul,	0x7fffffffffffffff,	0xffffffffffffffff,	-1)
	/* overflow */
	USUBX(7, l,	0x8000000000000000,	1,			0)
	SUBX(7, ul,	0x8000000000000000,	1,			0)
	/* carry */
	USUBX(8, l,	0x7fffffffffffffff,	0x8000000000000000,	-1)
	SUBX(8, ul,	0x7fffffffffffffff,	0x8000000000000000,	-1)
	/* overflow */
	USUBX(9, l,	0x8000000000000000,	0x7fffffffffffffff,	0)
	SUBX(9, ul,	0x8000000000000000,	0x7fffffffffffffff,	0)
	/* carry+overflow */
	USUBX(10, l,	1,			0x8000000000000000,	-1)
	SUBX(10, ul,	1,			0x8000000000000000,	-1)
#endif
	ret
