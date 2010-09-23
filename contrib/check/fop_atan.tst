#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  ATAN(N, T, I, V)			 FOPU(N, T, atan, I, V)
#define UATAN(N, T, I, V)			UFOPU(N, T, atan, I, V)
	 ATAN(0, f,	-0.0,		 0.0)
	 ATAN(1, f,	 2.0,		 1.1071487177940905031)
	 ATAN(2, f,	-2.0,		-1.1071487177940905031)
	 ATAN(3, f,	 0.5,		 0.46364760900080611615)
	 ATAN(4, f,	-0.5,		-0.46364760900080611615)
	UATAN(5, f,	$Inf,		$NaN)
	UATAN(6, f,	$NaN,		$NaN)
	 ATAN(0, d,	-0.0,		 0.0)
	 ATAN(1, d,	 2.0,		 1.1071487177940905031)
	 ATAN(2, d,	-2.0,		-1.1071487177940905031)
	 ATAN(3, d,	 0.5,		 0.46364760900080611615)
	 ATAN(4, d,	-0.5,		-0.46364760900080611615)
	UATAN(5, d,	$Inf,		$NaN)
	UATAN(6, d,	$NaN,		$NaN)

	ret
