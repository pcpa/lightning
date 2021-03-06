#include "alu.inc"

.code	$(4 * 1024 * 1024)
	prolog 0

#define RSH(N, T, I0, I1, V)	ALU(N, T, rsh, I0, I1, V)
	RSH(0, i,	0xfe,	    1,		0x7f)
	RSH(1, i,	0x1fffc,    2,		0x7fff)
	RSH(2, i,	0xfffffff8, 3,		0xffffffff)
	RSH(3, i,	0x80000000, 31,		0xffffffff)
	RSH(4, i,	0x40000000, 30,		1)
	RSH(5, i,	0x20000000, 29,		1)
	RSH(6, i,	0x10000000, 28,		1)
	RSH(7, i,	0x810000,   16,		0x81)
	RSH(8, i,	0x20000,    17,		1)
	RSH(9, i,	0x40000,    18,		1)
	RSH(10, i,	0x7f8000,   15,		0xff)
	RSH(11, i,	0x1000000,  24,		1)
	RSH(12, i,	0xffffff00, 8,		0xffffffff)
	RSH(13, i,	0x7fffffff, 0,		0x7fffffff)
	RSH(0, ui,	0xfe,	    1,		0x7f)
	RSH(1, ui,	0x1fffc,    2,		0x7fff)
	RSH(2, ui,	0xfffffff8, 3,		0x1fffffff)
	RSH(3, ui,	0x80000000, 31,		1)
	RSH(4, ui,	0x40000000, 30,		1)
	RSH(5, ui,	0x20000000, 29,		1)
	RSH(6, ui,	0x10000000, 28,		1)
	RSH(7, ui,	0x810000,   16,		0x81)
	RSH(8, ui,	0x20000,    17,		1)
	RSH(9, ui,	0x40000,    18,		1)
	RSH(10, ui,	0x7f8000,   15,		0xff)
	RSH(11, ui,	0x1000000,  24,		1)
	RSH(12, ui,	0xffffff00, 8,		0xffffff)
	RSH(13, ui,	0x7fffffff, 0,		0x7fffffff)
#if __WORDSIZE == 32
	RSH(0, l,	0xfe,	    1,		0x7f)
	RSH(1, l,	0x1fffc,    2,		0x7fff)
	RSH(2, l,	0xfffffff8, 3,		0xffffffff)
	RSH(3, l,	0x80000000, 31,		0xffffffff)
	RSH(4, l,	0x40000000, 30,		1)
	RSH(5, l,	0x20000000, 29,		1)
	RSH(6, l,	0x10000000, 28,		1)
	RSH(7, l,	0x810000,   16,		0x81)
	RSH(8, l,	0x20000,    17,		1)
	RSH(9, l,	0x40000,    18,		1)
	RSH(10, l,	0x7f8000,   15,		0xff)
	RSH(11, l,	0x1000000,  24,		1)
	RSH(12, l,	0xffffff00, 8,		0xffffffff)
	RSH(13, l,	0x7fffffff, 0,		0x7fffffff)
	RSH(0, ul,	0xfe,	    1,		0x7f)
	RSH(1, ul,	0x1fffc,    2,		0x7fff)
	RSH(2, ul,	0xfffffff8, 3,		0x1fffffff)
	RSH(3, ul,	0x80000000, 31,		1)
	RSH(4, ul,	0x40000000, 30,		1)
	RSH(5, ul,	0x20000000, 29,		1)
	RSH(6, ul,	0x10000000, 28,		1)
	RSH(7, ul,	0x810000,   16,		0x81)
	RSH(8, ul,	0x20000,    17,		1)
	RSH(9, ul,	0x40000,    18,		1)
	RSH(10, ul,	0x7f8000,   15,		0xff)
	RSH(11, ul,	0x1000000,  24,		1)
	RSH(12, ul,	0xffffff00, 8,		0xffffff)
	RSH(13, ul,	0x7fffffff, 0,		0x7fffffff)
#else
	RSH(0, l,	0xfe,		    1,			0x7f)
	RSH(1, l,	0x1fffc,	    2,			0x7fff)
	RSH(2, l,	0x3fffffff8,	    3,			0x7fffffff)
	RSH(3, l,	0xffffffc080000000, 31,			0xffffffffffffff81)
	RSH(4, l,	0x40000000,	    30,			1)
	RSH(5, l,	0x20000000,	    29,			1)
	RSH(6, l,	0x10000000,	    28,			1)
	RSH(7, l,	0x810000,	    16,			0x81)
	RSH(8, l,	0x20000,	    17,			1)
	RSH(9, l,	0x40000,	    18,			1)
	RSH(10, l,	0x7f8000,	    15,			0xff)
	RSH(11, l,	0x1000000,	    24,			1)
	RSH(12, l,	0xffffff00,	     8,			0xffffff)
	RSH(13, l,	0x7fffffff,	    0,			0x7fffffff)
	RSH(14, l,	0xfe00000000,	    33,			0x7f)
	RSH(15, l,	0x1ffffc00000000,   34,			0x7ffff)
	RSH(16, l,	0xfffffff800000000, 29,			0xffffffffffffffc0)
	RSH(17, l,	0x8000000000000000, 63,			0xffffffffffffffff)
	RSH(18, l,	0x4000000000000000, 62,			1)
	RSH(19, l,	0x2000000000000000, 61,			1)
	RSH(20, l,	0x1000000000000000, 60,			1)
	RSH(21, l,	0x81000000000000,   48,			0x81)
	RSH(22, l,	0x2000000000000,    49,			1)
	RSH(23, l,	0x10000000000,	    40,			1)
	RSH(24, l,	0x7f800000000000,   47,			0xff)
	RSH(25, l,	0x100000000000000,  56,			1)
	RSH(26, l,	0xffffff0000000000, 40,			0xffffffffffffffff)
	RSH(27, l,	0xfffffffe00000000, 33,			0xffffffffffffffff)
	RSH(28, l,	0x8000000000000001, 63,			0xffffffffffffffff)
	RSH(29, l,	0x1000000000000,    48,			1)
	RSH(30, l,	0xffff800000000000, 47,			0xffffffffffffffff)
	RSH(0, ul,	0xfe,		    1,			0x7f)
	RSH(1, ul,	0x1fffc,	    2,			0x7fff)
	RSH(2, ul,	0x3fffffff8,	    3,			0x7fffffff)
	RSH(3, ul,	0xffffffc080000000, 31,			0x1ffffff81)
	RSH(4, ul,	0x40000000,	    30,			1)
	RSH(5, ul,	0x20000000,	    29,			1)
	RSH(6, ul,	0x10000000,	    28,			1)
	RSH(7, ul,	0x810000,	    16,			0x81)
	RSH(8, ul,	0x20000,	    17,			1)
	RSH(9, ul,	0x40000,	    18,			1)
	RSH(10, ul,	0x7f8000,	    15,			0xff)
	RSH(11, ul,	0x1000000,	    24,			1)
	RSH(12, ul,	0xffffff00,	     8,			0xffffff)
	RSH(13, ul,	0x7fffffff,	    0,			0x7fffffff)
	RSH(14, ul,	0xfe00000000,	    33,			0x7f)
	RSH(15, ul,	0x1ffffc00000000,   34,			0x7ffff)
	RSH(16, ul,	0xfffffff800000000, 29,			0x7ffffffc0)
	RSH(17, ul,	0x8000000000000000, 63,			1)
	RSH(18, ul,	0x4000000000000000, 62,			1)
	RSH(19, ul,	0x2000000000000000, 61,			1)
	RSH(20, ul,	0x1000000000000000, 60,			1)
	RSH(21, ul,	0x81000000000000,   48,			0x81)
	RSH(22, ul,	0x2000000000000,    49,			1)
	RSH(23, ul,	0x10000000000,	    40,			1)
	RSH(24, ul,	0x7f800000000000,   47,			0xff)
	RSH(25, ul,	0x100000000000000,  56,			1)
	RSH(26, ul,	0xffffff0000000000, 40,			0xffffff)
	RSH(27, ul,	0xfffffffe00000000, 33,			0x7fffffff)
	RSH(28, ul,	0x8000000000000001, 63,			1)
	RSH(29, ul,	0x1000000000000,    48,			1)
	RSH(30, ul,	0xffff800000000000, 47,			0x1ffff)
#endif

#undef RSH
#define RSH(N, T, I0, V)	ALUC(N, T, rsh, I0, V)
	RSH(0, i,	0,			0)
	RSH(1, i,	15,			0)
	RSH(2, i,	31,			0)
	RSH(0, ui,	0,			0)
	RSH(1, ui,	15,			0)
	RSH(2, ui,	31,			0)
#if __WORDSIZE == 32
	RSH(0, l,	0,			0)
	RSH(1, l,	15,			0)
	RSH(2, l,	31,			0)
	RSH(0, ul,	0,			0)
	RSH(1, ul,	15,			0)
	RSH(2, ul,	31,			0)
#else
	RSH(0, l,	0,			0)
	RSH(1, l,	15,			0)
	RSH(2, l,	31,			0)
	RSH(3, l,	47,			0)
	RSH(4, l,	63,			0)
	RSH(0, ul,	0,			0)
	RSH(1, ul,	15,			0)
	RSH(2, ul,	31,			0)
	RSH(3, ul,	47,			0)
	RSH(4, ul,	63,			0)
#endif

	ret
