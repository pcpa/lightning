#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  LOG10(N, T, I, V)			 FOPU(N, T, log10, I, V)
#define ULOG10(N, T, I, V)			UFOPU(N, T, log10, I, V)
	 LOG10(0, f,	 0.0,		$nInf)
	 LOG10(1, f,	 2.0,		 0.3010299956639812)
	 LOG10(2, f,	 0.5,		-0.3010299956639812)
	ULOG10(3, f,	$Inf,		$Inf)
	ULOG10(4, f,	$NaN,		$NaN)
	 LOG10(0, d,	 0.0,		$nInf)
	 LOG10(1, d,	 2.0,		 0.3010299956639812)
	 LOG10(2, d,	 0.5,		-0.3010299956639812)
	ULOG10(3, d,	$Inf,		$Inf)
	ULOG10(4, d,	$NaN,		$NaN)

	ret
