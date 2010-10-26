#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#if defined(__i386__) || defined(__x86_64__)
#define  COS(N, T, I, V)			 FOPU(N, T, cos, I, V)
#define UCOS(N, T, I, V)			UFOPU(N, T, cos, I, V)
	 COS(0, f,	-0.0,		 1.0)
	 COS(1, f,	 2.0,		-0.416146836547142387)
	 COS(2, f,	-2.0,		-0.416146836547142387)
	 COS(3, f,	 0.5,		 0.87758256189037271613)
	 COS(4, f,	-0.5,		 0.87758256189037271613)
	UCOS(5, f,	$Inf,		$NaN)
	UCOS(6, f,	$NaN,		$NaN)
	 COS(0, d,	-0.0,		 1.0)
	 COS(1, d,	 2.0,		-0.416146836547142387)
	 COS(2, d,	-2.0,		-0.416146836547142387)
	 COS(3, d,	 0.5,		 0.87758256189037271613)
	 COS(4, d,	-0.5,		 0.87758256189037271613)
	UCOS(5, d,	$Inf,		$NaN)
	UCOS(6, d,	$NaN,		$NaN)
#endif

	ret
