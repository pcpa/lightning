#include "alu.inc"

.code	$(1024 * 1024)
	prolog 0

#define  TAN(N, T, I, V)			 FOPU(N, T, tan, I, V)
#define UTAN(N, T, I, V)			UFOPU(N, T, tan, I, V)
	 TAN(0, f,	-0.0,		 0.0)
	 TAN(1, f,	 2.0,		-2.1850398632615189917)
	 TAN(2, f,	-2.0,		 2.1850398632615189917)
	 TAN(3, f,	 0.5,		 0.54630248984379051327)
	 TAN(4, f,	-0.5,		-0.54630248984379051327)
	UTAN(5, f,	$Inf,		$NaN)
	UTAN(6, f,	$NaN,		$NaN)
	 TAN(0, d,	-0.0,		 0.0)
	 TAN(1, d,	 2.0,		-2.1850398632615189917)
	 TAN(2, d,	-2.0,		 2.1850398632615189917)
	 TAN(3, d,	 0.5,		 0.54630248984379051327)
	 TAN(4, d,	-0.5,		-0.54630248984379051327)
	UTAN(5, d,	$Inf,		$NaN)
	UTAN(6, d,	$NaN,		$NaN)

	ret
