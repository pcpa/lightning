#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  SQRT(N, T, I, V)			 FOPU(N, T, sqrt, I, V)
#define USQRT(N, T, I, V)			UFOPU(N, T, sqrt, I, V)
	 SQRT(0, f,	-0.0,		 0.0)
	 SQRT(1, f,	 4.0,		 2.0)
	 SQRT(2, f,	 2.25,		 1.5)
	 SQRT(3, f,	$Inf,		$Inf)
	USQRT(0, f,	$NaN,		$NaN)
	 SQRT(0, d,	-0.0,		 0.0)
	 SQRT(1, d,	 4.0,		 2.0)
	 SQRT(2, d,	 2.25,		 1.5)
	 SQRT(3, d,	$Inf,		$Inf)
	USQRT(0, d,	$NaN,		$NaN)

	ret
