#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  ABS(N, T, I, V)		 FOPU(N, T, abs, I, V)
#define UABS(N, T, I, V)		UFOPU(N, T, abs, I, V)
	 ABS(0, f,	-0.0,		 0.0)
	 ABS(1, f,	 0.5,		 0.5)
	 ABS(2, f,	-0.5,		 0.5)
	 ABS(3, f,	$Inf,		$Inf)
	ABS(4, f,	$nInf,		$Inf)
	 ABS(5, f,	 1.25,		1.25)
	 ABS(6, f,	-1.25,		1.25)
	 ABS(7, f,	$nInf,		$Inf)
	UABS(0, f,	$NaN,		$NaN)
	ABS(0, d,	-0.0,		 0.0)
	ABS(1, d,	 0.5,		 0.5)
	ABS(2, d,	-0.5,		 0.5)
	ABS(3, d,	$Inf,		$Inf)
	ABS(4, d,	$nInf,		$Inf)
	ABS(5, d,	 1.25,		 1.25)
	ABS(6, d,	-1.25,		 1.25)
	ABS(7, d,	$nInf,		$Inf)
	UABS(0, d,	$NaN,		$NaN)

	ret
