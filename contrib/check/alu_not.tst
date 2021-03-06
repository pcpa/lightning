#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define NOT(N, T, I, V)			ALUU(N, T, not, I, V)
	NOT(0, i,	0,		0xffffffff)
	NOT(1, i,	1,		0xfffffffe)
	NOT(2, i,	0xffffffff,	0)
	NOT(3, i,	0x80000000,	0x7fffffff)
	NOT(4, i,	0x7fffffff,	0x80000000)
	NOT(5, i,	0x80000001,	0x7ffffffe)
	NOT(0, ui,	0,		0xffffffff)
	NOT(1, ui,	1,		0xfffffffe)
	NOT(2, ui,	0xffffffff,	0)
	NOT(3, ui,	0x80000000,	0x7fffffff)
	NOT(4, ui,	0x7fffffff,	0x80000000)
	NOT(5, ui,	0x80000001,	0x7ffffffe)
#if __WORDSIZE == 32
	NOT(0, l,	0,		0xffffffff)
	NOT(1, l,	1,		0xfffffffe)
	NOT(2, l,	0xffffffff,	0)
	NOT(3, l,	0x80000000,	0x7fffffff)
	NOT(4, l,	0x7fffffff,	0x80000000)
	NOT(5, l,	0x80000001,	0x7ffffffe)
	NOT(0, ul,	0,		0xffffffff)
	NOT(1, ul,	1,		0xfffffffe)
	NOT(2, ul,	0xffffffff,	0)
	NOT(3, ul,	0x80000000,	0x7fffffff)
	NOT(4, ul,	0x7fffffff,	0x80000000)
	NOT(5, ul,	0x80000001,	0x7ffffffe)
#else
	NOT(0, l,		0,			0xffffffffffffffff)
	NOT(1, l,		1,			0xfffffffffffffffe)
	NOT(2, l,		0xffffffff,		0xffffffff00000000)
	NOT(3, l,		0x80000000,		0xffffffff7fffffff)
	NOT(4, l,		0x7fffffff,		0xffffffff80000000)
	NOT(5, l,		0x80000001,		0xffffffff7ffffffe)
	NOT(6, l,		0xffffffffffffffff,	0)
	NOT(7, l,		0x8000000000000000,	0x7fffffffffffffff)
	NOT(8, l,		0x7fffffffffffffff,	0x8000000000000000)
	NOT(9, l,		0x8000000000000001,	0x7ffffffffffffffe)
	NOT(0, ul,		0,			0xffffffffffffffff)
	NOT(1, ul,		1,			0xfffffffffffffffe)
	NOT(2, ul,		0xffffffff,		0xffffffff00000000)
	NOT(3, ul,		0x80000000,		0xffffffff7fffffff)
	NOT(4, ul,		0x7fffffff,		0xffffffff80000000)
	NOT(5, ul,		0x80000001,		0xffffffff7ffffffe)
	NOT(6, ul,		0xffffffffffffffff,	0)
	NOT(7, ul,		0x8000000000000000,	0x7fffffffffffffff)
	NOT(8, ul,		0x7fffffffffffffff,	0x8000000000000000)
	NOT(9, ul,		0x8000000000000001,	0x7ffffffffffffffe)
#endif

	ret
