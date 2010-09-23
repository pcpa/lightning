#define DEBUG_REGISTERS	1
#include "alu.inc"

.code	$(8 * 1024 * 1024)
	prolog 0

#define LSH(N, T, I0, I1, V)	ALU(N, T, lsh, I0, I1, V)
	LSH(0, i,	0x7f,	    1,		0xfe)
	LSH(1, i,	0x7fff,	    2,		0x1fffc)
	LSH(2, i,	0x7fffffff, 3,		0xfffffff8)
	LSH(3, i,	-0x7f,	    31,		0x80000000)
	LSH(4, i,	-0x7fff,    30,		0x40000000)
	LSH(5, i,	-0x7fffffff,29,		0x20000000)
	LSH(6, i,	0x80000001, 28,		0x10000000)
	LSH(7, i,	0x81,	    16,		0x810000)
	LSH(8, i,	0x8001,	    17,		0x20000)
	LSH(9, i,	0x80000001, 18,		0x40000)
	LSH(10, i,	0xff,	    15,		0x7f8000)
	LSH(11, i,	-0xffff,    24,		0x1000000)
	LSH(12, i,	0xffffffff, 8,		0xffffff00)
	LSH(13, i,	0x7fffffff, 0,		0x7fffffff)
	LSH(0, ui,	0x7f,	    1,		0xfe)
	LSH(1, ui,	0x7fff,	    2,		0x1fffc)
	LSH(2, ui,	0x7fffffff, 3,		0xfffffff8)
	LSH(3, ui,	0xffffff81, 31,		0x80000000)
	LSH(4, ui,	0xffff8001, 30,		0x40000000)
	LSH(5, ui,	0x80000001, 29,		0x20000000)
	LSH(6, ui,	0x80000001, 28,		0x10000000)
	LSH(7, ui,	0x81,	    16,		0x810000)
	LSH(8, ui,	0x8001,	    17,		0x20000)
	LSH(9, ui,	0x80000001, 18,		0x40000)
	LSH(10, ui,	0xff,	    15,		0x7f8000)
	LSH(11, ui,	0xffff0001, 24,		0x1000000)
	LSH(12, ui,	0xffffffff, 8,		0xffffff00)
	LSH(13, ui,	0x7fffffff, 0,		0x7fffffff)
#if __WORDSIZE == 32
	LSH(0, l,	0x7f,	    1,		0xfe)
	LSH(1, l,	0x7fff,	    2,		0x1fffc)
	LSH(2, l,	0x7fffffff, 3,		0xfffffff8)
	LSH(3, l,	-0x7f,	    31,		0x80000000)
	LSH(4, l,	-0x7fff,    30,		0x40000000)
	LSH(5, l,	-0x7fffffff,29,		0x20000000)
	LSH(6, l,	0x80000001, 28,		0x10000000)
	LSH(7, l,	0x81,	    16,		0x810000)
	LSH(8, l,	0x8001,	    17,		0x20000)
	LSH(9, l,	0x80000001, 18,		0x40000)
	LSH(10, l,	0xff,	    15,		0x7f8000)
	LSH(11, l,	-0xffff,    24,		0x1000000)
	LSH(12, l,	0xffffffff, 8,		0xffffff00)
	LSH(13, l,	0x7fffffff, 0,		0x7fffffff)
	LSH(0, ul,	0x7f,	    1,		0xfe)
	LSH(1, ul,	0x7fff,	    2,		0x1fffc)
	LSH(2, ul,	0x7fffffff, 3,		0xfffffff8)
	LSH(3, ul,	0xffffff81, 31,		0x80000000)
	LSH(4, ul,	0xffff8001, 30,		0x40000000)
	LSH(5, ul,	0x80000001, 29,		0x20000000)
	LSH(6, ul,	0x80000001, 28,		0x10000000)
	LSH(7, ul,	0x81,	    16,		0x810000)
	LSH(8, ul,	0x8001,	    17,		0x20000)
	LSH(9, ul,	0x80000001, 18,		0x40000)
	LSH(10, ul,	0xff,	    15,		0x7f8000)
	LSH(11, ul,	0xffff0001, 24,		0x1000000)
	LSH(12, ul,	0xffffffff, 8,		0xffffff00)
	LSH(13, ul,	0x7fffffff, 0,		0x7fffffff)
#else
	LSH(0, l,	0x7f,		    1,			0xfe)
	LSH(1, l,	0x7fff,		    2,			0x1fffc)
	LSH(2, l,	0x7fffffff,	    3,			0x3fffffff8)
	LSH(3, l,	-0x7f,		    31,			0xffffffc080000000)
	LSH(4, l,	-0x7fff,	    30,			0xffffe00040000000)
	LSH(5, l,	-0x7fffffff,	    29,			0xf000000020000000)
	LSH(6, l,	0x80000001,	    28,			0x800000010000000)
	LSH(7, l,	0x81,		    16,			0x810000)
	LSH(8, l,	0x8001,		    17,			0x100020000)
	LSH(9, l,	0x80000001,	    18,			0x2000000040000)
	LSH(10, l,	0xff,		    15,			0x7f8000)
	LSH(11, l,	-0xffff,	    24,			0xffffff0001000000)
	LSH(12, l,	0xffffffff,	    8,			0xffffffff00)
	LSH(13, l,	0x7fffffff,	    0,			0x7fffffff)
	LSH(14, l,	0x7f,		    33,			0xfe00000000)
	LSH(15, l,	0x7ffff,	    34,			0x1ffffc00000000)
	LSH(16, l,	0x7fffffff,	    35,			0xfffffff800000000)
	LSH(17, l,	-0x7f,		    63,			0x8000000000000000)
	LSH(18, l,	-0x7fff,	    62,			0x4000000000000000)
	LSH(19, l,	-0x7fffffff,	    61,			0x2000000000000000)
	LSH(20, l,	0x80000001,	    60,			0x1000000000000000)
	LSH(21, l,	0x81,		    48,			0x81000000000000)
	LSH(22, l,	0x8001,		    49,			0x2000000000000)
	LSH(23, l,	0x80000001,	    40,			0x10000000000)
	LSH(24, l,	0xff,		    47,			0x7f800000000000)
	LSH(25, l,	0xffff0001,	    56,			0x100000000000000)
	LSH(26, l,	0xffffffff,	    40,			0xffffff0000000000)
	LSH(27, l,	0x7fffffffff,	    33,			0xfffffffe00000000)
	LSH(28, l,	-0x7fffffffff,	    63,			0x8000000000000000)
	LSH(29, l,	0x8000000001,	    48,			0x1000000000000)
	LSH(30, l,	0xffffffffff,	    47,			0xffff800000000000)
	LSH(0, ul,	0x7f,		    1,			0xfe)
	LSH(1, ul,	0x7fff,		    2,			0x1fffc)
	LSH(2, ul,	0x7fffffff,	    3,			0x3fffffff8)
	LSH(3, ul,	0xffffffffffffff81, 31,			0xffffffc080000000)
	LSH(4, ul,	0xffffffffffff8001, 30,			0xffffe00040000000)
	LSH(5, ul,	0xffffffff80000001, 29,			0xf000000020000000)
	LSH(6, ul,	0x80000001,	    28,			0x800000010000000)
	LSH(7, ul,	0x81,		    16,			0x810000)
	LSH(8, ul,	0x8001,		    17,			0x100020000)
	LSH(9, ul,	0x80000001,	    18,			0x2000000040000)
	LSH(10, ul,	0xff,		    15,			0x7f8000)
	LSH(11, ul,	0xffffffffffff0001, 24,			0xffffff0001000000)
	LSH(12, ul,	0xffffffff,	    8,			0xffffffff00)
	LSH(13, ul,	0x7fffffff,	    0,			0x7fffffff)
	LSH(14, ul,	0x7f,		    33,			0xfe00000000)
	LSH(15, ul,	0x7ffff,	    34,			0x1ffffc00000000)
	LSH(16, ul,	0x7fffffff,	    35,			0xfffffff800000000)
	LSH(17, ul,	0xffffffffffffff81, 63,			0x8000000000000000)
	LSH(18, ul,	0xffffffffffff8001, 62,			0x4000000000000000)
	LSH(19, ul,	0xffffffff80000001, 61,			0x2000000000000000)
	LSH(20, ul,	0x80000001,	    60,			0x1000000000000000)
	LSH(21, ul,	0x81,		    48,			0x81000000000000)
	LSH(22, ul,	0x8001,		    49,			0x2000000000000)
	LSH(23, ul,	0x80000001,	    40,			0x10000000000)
	LSH(24, ul,	0xff,		    47,			0x7f800000000000)
	LSH(25, ul,	0xffff0001,	    56,			0x100000000000000)
	LSH(26, ul,	0xffffffff,	    40,			0xffffff0000000000)
	LSH(27, ul,	0x7fffffffff,	    33,			0xfffffffe00000000)
	LSH(28, ul,	0xffffff8000000001, 63,			0x8000000000000000)
	LSH(29, ul,	0x8000000001,	    48,			0x1000000000000)
	LSH(30, ul,	0xffffffffff,	    47,			0xffff800000000000)
#endif

#undef LSH
#define LSH(N, T, I0, V)	ALUC(N, T, lsh, I0, V)
	LSH(0, i,	0,			0)
	LSH(1, i,	15,			0x78000)
	LSH(2, i,	31,			0x80000000)
	LSH(0, ui,	0,			0)
	LSH(1, ui,	15,			0x78000)
	LSH(2, ui,	31,			0x80000000)
#if __WORDSIZE == 32
	LSH(0, l,	0,			0)
	LSH(1, l,	15,			0x78000)
	LSH(2, l,	31,			0x80000000)
	LSH(0, ul,	0,			0)
	LSH(1, ul,	15,			0x78000)
	LSH(2, ul,	31,			0x80000000)
#else
	LSH(0, l,	0,			0)
	LSH(1, l,	15,			0x78000)
	LSH(2, l,	31,			0xf80000000)
	LSH(3, l,	47,			0x17800000000000)
	LSH(4, l,	63,			0x8000000000000000)
	LSH(0, ul,	0,			0)
	LSH(1, ul,	15,			0x78000)
	LSH(2, ul,	31,			0xf80000000)
	LSH(3, ul,	47,			0x17800000000000)
	LSH(4, ul,	63,			0x8000000000000000)
#endif

	ret