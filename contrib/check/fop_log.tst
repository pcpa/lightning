#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  LOG(N, T, I, V)			 FOPU(N, T, log, I, V)
#define ULOG(N, T, I, V)			UFOPU(N, T, log, I, V)
	LOG(0, f,	 0.0,		$nInf)
	 LOG(1, f,	 2.0,		 0.69314718055994530943)
	 LOG(2, f,	 0.5,		-0.69314718055994530943)
	ULOG(3, f,	$Inf,		$Inf)
	ULOG(4, f,	$NaN,		$NaN)
	 LOG(0, d,	 0.0,		$nInf)
	 LOG(1, d,	 2.0,		 0.69314718055994530943)
	 LOG(2, d,	 0.5,		-0.69314718055994530943)
	ULOG(3, d,	$Inf,		$Inf)
	ULOG(4, d,	$NaN,		$NaN)

	ret
