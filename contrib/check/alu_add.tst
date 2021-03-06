#define DEBUG_REGISTERS	1
#include "alu.inc"

.code	$(8 * 1024 * 1024)
	prolog 0

#define ADD(N, T, I0, I1, V)	ALU(N, T, add, I0, I1, V)
	ADD(0, i,	0x7fffffff, 1,		0x80000000)
	ADD(1, i,	1,	    0x7fffffff, 0x80000000)
	ADD(2, i,	0x80000000, 1,		0x80000001)
	ADD(3, i,	1,	    0x80000000, 0x80000001)
	ADD(4, i,	0x7fffffff, 0x80000000, 0xffffffff)
	ADD(5, i,	0x80000000, 0x7fffffff, 0xffffffff)
	ADD(6, i,	0x7fffffff, 0xffffffff, 0x7ffffffe)
	ADD(7, i,	0xffffffff, 0x7fffffff, 0x7ffffffe)
	ADD(9, i,	0xffffffff, 0xffffffff, 0xfffffffe)
	ADD(10, i,	0x7fffffff, 0,		0x7fffffff)
	ADD(11, i,	0,	    0x7fffffff,	0x7fffffff)
	ADD(0, ui,	0x7fffffff, 1,		0x80000000)
	ADD(1, ui,	1,	    0x7fffffff, 0x80000000)
	ADD(2, ui,	0x80000000, 1,		0x80000001)
	ADD(3, ui,	1,	    0x80000000, 0x80000001)
	ADD(4, ui,	0x7fffffff, 0x80000000, 0xffffffff)
	ADD(5, ui,	0x80000000, 0x7fffffff, 0xffffffff)
	ADD(6, ui,	0x7fffffff, 0xffffffff, 0x7ffffffe)
	ADD(7, ui,	0xffffffff, 0x7fffffff, 0x7ffffffe)
	ADD(9, ui,	0xffffffff, 0xffffffff, 0xfffffffe)
	ADD(10,ui,	0x7fffffff, 0,		0x7fffffff)
	ADD(11,ui,	0,	    0x7fffffff,	0x7fffffff)
#if __WORDSIZE == 32
	ADD(0, l,	0x7fffffff, 1,		0x80000000)
	ADD(1, l,	1,	    0x7fffffff, 0x80000000)
	ADD(2, l,	0x80000000, 1,		0x80000001)
	ADD(3, l,	1,	    0x80000000, 0x80000001)
	ADD(4, l,	0x7fffffff, 0x80000000, 0xffffffff)
	ADD(5, l,	0x80000000, 0x7fffffff, 0xffffffff)
	ADD(6, l,	0x7fffffff, 0xffffffff, 0x7ffffffe)
	ADD(7, l,	0xffffffff, 0x7fffffff, 0x7ffffffe)
	ADD(9, l,	0xffffffff, 0xffffffff, 0xfffffffe)
	ADD(10, l,	0x7fffffff, 0,		0x7fffffff)
	ADD(11, l,	0,	    0x7fffffff,	0x7fffffff)
	ADD(0, ul,	0x7fffffff, 1,		0x80000000)
	ADD(1, ul,	1,	    0x7fffffff, 0x80000000)
	ADD(2, ul,	0x80000000, 1,		0x80000001)
	ADD(3, ul,	1,	    0x80000000, 0x80000001)
	ADD(4, ul,	0x7fffffff, 0x80000000, 0xffffffff)
	ADD(5, ul,	0x80000000, 0x7fffffff, 0xffffffff)
	ADD(6, ul,	0x7fffffff, 0xffffffff, 0x7ffffffe)
	ADD(7, ul,	0xffffffff, 0x7fffffff, 0x7ffffffe)
	ADD(9, ul,	0xffffffff, 0xffffffff, 0xfffffffe)
	ADD(10,ul,	0x7fffffff, 0,		0x7fffffff)
	ADD(11,ul,	0,	    0x7fffffff,	0x7fffffff)
	ADD(0, p,	0x7fffffff, 1,		0x80000000)
	ADD(1, p,	1,	    0x7fffffff, 0x80000000)
	ADD(2, p,	0x80000000, 1,		0x80000001)
	ADD(3, p,	1,	    0x80000000, 0x80000001)
	ADD(4, p,	0x7fffffff, 0x80000000, 0xffffffff)
	ADD(5, p,	0x80000000, 0x7fffffff, 0xffffffff)
	ADD(6, p,	0x7fffffff, 0xffffffff, 0x7ffffffe)
	ADD(7, p,	0xffffffff, 0x7fffffff, 0x7ffffffe)
	ADD(9, p,	0xffffffff, 0xffffffff, 0xfffffffe)
	ADD(10, p,	0x7fffffff, 0,		0x7fffffff)
	ADD(11, p,	0,	    0x7fffffff,	0x7fffffff)
#else
	ADD(0, l,	0x7fffffff,	    1,			0x80000000)
	ADD(1, l,	1,		    0x7fffffff,		0x80000000)
	ADD(2, l,	0x80000000,	    1,			0x80000001)
	ADD(3, l,	1,		    0x80000000, 	0x80000001)
	ADD(4, l,	0x7fffffff,	    0x80000000,		0xffffffff)
	ADD(5, l,	0x80000000,	    0x7fffffff,		0xffffffff)
	ADD(6, l,	0x7fffffff,	    0xffffffff,		0x17ffffffe)
	ADD(7, l,	0xffffffff,	    0x7fffffff,		0x17ffffffe)
	ADD(9, l,	0xffffffff,	    0xffffffff,		0x1fffffffe)
	ADD(10, l,	0x7fffffff,	    0,			0x7fffffff)
	ADD(11, l,	0,		    0x7fffffff,		0x7fffffff)
	ADD(12, l,	0x7fffffffffffffff, 1,			0x8000000000000000)
	ADD(13, l,	1,		    0x7fffffffffffffff,	0x8000000000000000)
	ADD(14, l,	0x8000000000000000, 1,			0x8000000000000001)
	ADD(15, l,	1,		    0x8000000000000000,	0x8000000000000001)
	ADD(16, l,	0x7fffffffffffffff, 0x8000000000000000,	0xffffffffffffffff)
	ADD(17, l,	0x8000000000000000, 0x7fffffffffffffff,	0xffffffffffffffff)
	ADD(18, l,	0x7fffffffffffffff, 0xffffffffffffffff,	0x7ffffffffffffffe)
	ADD(19, l,	0x7fffffffffffffff, 0x7fffffffffffffff,	0xfffffffffffffffe)
	ADD(20, l,	0xffffffffffffffff, 0xffffffffffffffff,	0xfffffffffffffffe)
	ADD(0, ul,	0x7fffffff,	    1,			0x80000000)
	ADD(1, ul,	1,		    0x7fffffff,		0x80000000)
	ADD(2, ul,	0x80000000,	    1,			0x80000001)
	ADD(3, ul,	1,		    0x80000000,		0x80000001)
	ADD(4, ul,	0x7fffffff,	    0x80000000,		0xffffffff)
	ADD(5, ul,	0x80000000,	    0x7fffffff,		0xffffffff)
	ADD(6, ul,	0x7fffffff,	    0xffffffff,		0x17ffffffe)
	ADD(7, ul,	0xffffffff,	    0x7fffffff,		0x17ffffffe)
	ADD(9, ul,	0xffffffff,	    0xffffffff,		0x1fffffffe)
	ADD(10, ul,	0x7fffffff,	    0,			0x7fffffff)
	ADD(11, ul,	0,		    0x7fffffff,		0x7fffffff)
	ADD(12, ul,	0x7fffffffffffffff, 1,			0x8000000000000000)
	ADD(13, ul,	1,		    0x7fffffffffffffff,	0x8000000000000000)
	ADD(14, ul,	0x8000000000000000, 1,			0x8000000000000001)
	ADD(15, ul,	1,		    0x8000000000000000,	0x8000000000000001)
	ADD(16, ul,	0x7fffffffffffffff, 0x8000000000000000,	0xffffffffffffffff)
	ADD(17, ul,	0x8000000000000000, 0x7fffffffffffffff,	0xffffffffffffffff)
	ADD(18, ul,	0x7fffffffffffffff, 0xffffffffffffffff,	0x7ffffffffffffffe)
	ADD(19, ul,	0xffffffffffffffff, 0x7fffffffffffffff,	0x7ffffffffffffffe)
	ADD(20, ul,	0xffffffffffffffff, 0xffffffffffffffff,	0xfffffffffffffffe)
	ADD(0, p,	0x7fffffff,	    1,			0x80000000)
	ADD(1, p,	1,		    0x7fffffff,		0x80000000)
	ADD(2, p,	0x80000000,	    1,			0x80000001)
	ADD(3, p,	1,		    0x80000000,		0x80000001)
	ADD(4, p,	0x7fffffff,	    0x80000000,		0xffffffff)
	ADD(5, p,	0x80000000,	    0x7fffffff,		0xffffffff)
	ADD(6, p,	0x7fffffff,	    0xffffffff,		0x17ffffffe)
	ADD(7, p,	0xffffffff,	    0x7fffffff,		0x17ffffffe)
	ADD(9, p,	0xffffffff,	    0xffffffff,		0x1fffffffe)
	ADD(10, p,	0x7fffffff,	    0,			0x7fffffff)
	ADD(11, p,	0,		    0x7fffffff,		0x7fffffff)
	ADD(12, p,	0x7fffffffffffffff, 0xffffffffffffffff,	0x7ffffffffffffffe)
	ADD(13, p,	0xffffffffffffffff, 0x7fffffffffffffff,	0x7ffffffffffffffe)
	ADD(14, p,	0xffffffffffffffff, 0xffffffffffffffff,	0xfffffffffffffffe)
	ADD(15, p,	1,		    0x8000000000000000,	0x8000000000000001)
	ADD(16, p,	0x7fffffffffffffff, 0x8000000000000000,	0xffffffffffffffff)
	ADD(17, p,	0x8000000000000000, 0x7fffffffffffffff,	0xffffffffffffffff)
	ADD(18, p,	0x7fffffffffffffff, 0xffffffffffffffff,	0x7ffffffffffffffe)
	ADD(19, p,	0xffffffffffffffff, 0x7fffffffffffffff,	0x7ffffffffffffffe)
	ADD(20, p,	0xffffffffffffffff, 0xffffffffffffffff,	0xfffffffffffffffe)
#endif

#undef ADD
#define ADD(N, T, I0, V)	ALUC(N, T, add, I0, V)
	ADD(0, i,	0,			0)
	ADD(1, i,	0x7fffffff,		0xfffffffe)
	ADD(2, i,	0x80000000,		0)
	ADD(3, i,	0xffffffff,		0xfffffffe)
	ADD(0, ui,	0,			0)
	ADD(1, ui,	0x7fffffff,		0xfffffffe)
	ADD(2, ui,	0x80000000,		0)
	ADD(3, ui,	0xffffffff,		0xfffffffe)
#if __WORDSIZE == 32
	ADD(0, l,	0,			0)
	ADD(1, l,	0x7fffffff,		0xfffffffe)
	ADD(2, l,	0x80000000,		0)
	ADD(3, l,	0xffffffff,		0xfffffffe)
	ADD(0, ul,	0,			0)
	ADD(1, ul,	0x7fffffff,		0xfffffffe)
	ADD(2, ul,	0x80000000,		0)
	ADD(3, ul,	0xffffffff,		0xfffffffe)
	ADD(0, p,	0,			0)
	ADD(1, p,	0x7fffffff,		0xfffffffe)
	ADD(2, p,	0x80000000,		0)
	ADD(3, p,	0xffffffff,		0xfffffffe)
#else
	ADD(0, l,	0,			0)
	ADD(1, l,	0x7fffffff,		0xfffffffe)
	ADD(2, l,	0x80000000,		0x100000000)
	ADD(3, l,	0xffffffff,		0x1fffffffe)
	ADD(4, l,	0x7fffffffffffffff,	0xfffffffffffffffe)
	ADD(5, l,	0x8000000000000000,	0)
	ADD(6, l,	0xffffffffffffffff,	0xfffffffffffffffe)
	ADD(0, ul,	0,			0)
	ADD(1, ul,	0x7fffffff,		0xfffffffe)
	ADD(2, ul,	0x80000000,		0x100000000)
	ADD(3, ul,	0xffffffff,		0x1fffffffe)
	ADD(4, ul,	0x7fffffffffffffff,	0xfffffffffffffffe)
	ADD(5, ul,	0x8000000000000000,	0)
	ADD(6, ul,	0xffffffffffffffff,	0xfffffffffffffffe)
	ADD(0, p,	0,			0)
	ADD(1, p,	0x7fffffff,		0xfffffffe)
	ADD(2, p,	0x80000000,		0x100000000)
	ADD(3, p,	0xffffffff,		0x1fffffffe)
	ADD(4, p,	0x7fffffffffffffff,	0xfffffffffffffffe)
	ADD(5, p,	0x8000000000000000,	0)
	ADD(6, p,	0xffffffffffffffff,	0xfffffffffffffffe)
#endif

#undef ADD
#define ADD(N, T, I0, I1, V)	FOP(N, T, add, I0, I1, V)
	ADD(0, f,	-0.5,	    0.5,	0.0)
	ADD(1, f,	 0.25,	    0.75,	1.0)
	ADD(0, d,	-0.5,	    0.5,	0.0)
	ADD(1, d,	 0.25,	    0.75,	1.0)

	ret
