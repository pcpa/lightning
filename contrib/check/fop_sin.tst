#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  SIN(N, T, I, V)			 FOPU(N, T, sin, I, V)
#define USIN(N, T, I, V)			UFOPU(N, T, sin, I, V)
	 SIN(0, f,	-0.0,		 0.0)
	 SIN(1, f,	 2.0,		 0.9092974268256816954)
	 SIN(2, f,	-2.0,		-0.9092974268256816954)
	 SIN(3, f,	 0.5,		 0.4794255386042030002)
	 SIN(4, f,	-0.5,		-0.4794255386042030002)
	USIN(5, f,	$Inf,		$NaN)
	USIN(6, f,	$NaN,		$NaN)
	 SIN(0, d,	-0.0,		 0.0)
	 SIN(1, d,	 2.0,		 0.9092974268256816954)
	 SIN(2, d,	-2.0,		-0.9092974268256816954)
	 SIN(3, d,	 0.5,		 0.4794255386042030002)
	 SIN(4, d,	-0.5,		-0.4794255386042030002)
	USIN(5, d,	$Inf,		$NaN)
	USIN(6, d,	$NaN,		$NaN)

	ret
