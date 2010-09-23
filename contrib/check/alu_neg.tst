#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define NEG(N, T, I, V)			ALUU(N, T, neg, I, V)
	NEG(0, i,	0,		0)
	NEG(1, i,	1,		0xffffffff)
	NEG(2, i,	0xffffffff,	1)
	NEG(3, i,	0x80000000,	0x80000000)
	NEG(4, i,	0x7fffffff,	0x80000001)
	NEG(5, i,	0x80000001,	0x7fffffff)
#if __WORDSIZE == 32
	NEG(0, l,	0,		0)
	NEG(1, l,	1,		0xffffffff)
	NEG(2, l,	0xffffffff,	1)
	NEG(3, l,	0x80000000,	0x80000000)
	NEG(4, l,	0x7fffffff,	0x80000001)
	NEG(5, l,	0x80000001,	0x7fffffff)
#else
	NEG(0, l,		0,			0)
	NEG(1, l,		1,			0xffffffffffffffff)
	NEG(2, l,		0xffffffff,		0xffffffff00000001)
	NEG(3, l,		0x80000000,		0xffffffff80000000)
	NEG(4, l,		0x7fffffff,		0xffffffff80000001)
	NEG(5, l,		0x80000001,		0xffffffff7fffffff)
	NEG(6, l,		0xffffffffffffffff,	1)
	NEG(7, l,		0x8000000000000000,	0x8000000000000000)
	NEG(8, l,		0x7fffffffffffffff,	0x8000000000000001)
#endif

#undef NEG
#define NEG(N, T, I, V)			FOPU(N, T, neg, I, V)
	NEG(0, f,	0.0,		-0.0)
	NEG(1, f,	0.5,		-0.5)
	NEG(2, f,	$(1 / 0.0),	$(-1.0 / 0))
	NEG(3, f,	-1.25,		 1.25)
	NEG(0, d,	 0.0,		-0.0)
	NEG(1, d,	 0.5,		-0.5)
	NEG(2, d,	 $(1.0 / 0),	$(-1 / 0.0))
	NEG(3, d,	-1.25,		 1.25)

	ret
