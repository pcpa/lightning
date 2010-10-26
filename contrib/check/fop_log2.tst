#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#if defined(__i386__) || defined(__x86_64__)
#define  LOG2(N, T, I, V)			 FOPU(N, T, log2, I, V)
#define ULOG2(N, T, I, V)			UFOPU(N, T, log2, I, V)
	 LOG2(0, f,	 0.0,		$nInf)
	 LOG2(1, f,	 2.0,		 1.0)
	 LOG2(2, f,	 0.5,		-1.0)
	ULOG2(3, f,	$Inf,		$Inf)
	ULOG2(4, f,	$NaN,		$NaN)
	 LOG2(0, d,	 0.0,		$nInf)
	 LOG2(1, d,	 2.0,		 1.0)
	 LOG2(2, d,	 0.5,		-1.0)
	ULOG2(3, d,	$Inf,		$Inf)
	ULOG2(4, d,	$NaN,		$NaN)
#endif

	ret
