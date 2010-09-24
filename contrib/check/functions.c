typedef signed char	 c;
typedef unsigned char	 uc;
typedef signed short	 s;
typedef unsigned short	 us;
typedef signed int	 i;
typedef unsigned int	 ui;
typedef signed long	 l;
typedef unsigned long	 ul;
typedef float		 f;
typedef double		 d;

/* prototypes of C functions that sum arguments */
#define proto0(t)	t C_##t##0(void);
#define proto1(t)	proto0(t)	t C_##t##1(t);
#define proto2(t)	proto1(t)	t C_##t##2(t,t);
#define proto3(t)	proto2(t)	t C_##t##3(t,t,t);
#define proto4(t)	proto3(t)	t C_##t##4(t,t,t,t);
#define proto5(t)	proto4(t)	t C_##t##5(t,t,t,t,t);
#define proto6(t)	proto5(t)	t C_##t##6(t,t,t,t,t,t);
#define proto7(t)	proto6(t)	t C_##t##7(t,t,t,t,t,t,t);
#define proto8(t)	proto7(t)	t C_##t##8(t,t,t,t,t,t,t,t);
#define proto9(t)	proto8(t)	t C_##t##9(t,t,t,t,t,t,t,t,t);
#define proto10(t)	proto9(t)	t C_##t##10(t,t,t,t,t,t,t,t,t,t);
#define proto11(t)	proto10(t)	t C_##t##11(t,t,t,t,t,t,t,t,t,t,t);
#define proto12(t)	proto11(t)	t C_##t##12(t,t,t,t,t,t,t,t,t,t,t,t);
#define proto13(t)	proto12(t)	t C_##t##13(t,t,t,t,t,t,t,t,t,t,t,t,t);
#define proto14(t)	proto13(t)	t C_##t##14(t,t,t,t,t,t,t,t,t,t,t,t,t,t);
#define proto15(t)	proto14(t)	t C_##t##15(t,t,t,t,t,t,t,t,t,t,t,t,t,t,t);
#define proto(t)	proto15(t)
proto(c)
proto(uc)
proto(s)
proto(us)
proto(i)
proto(ui)
proto(l)
proto(ul)
proto(f)
proto(d)
#undef proto0
#undef proto1
#undef proto2
#undef proto3
#undef proto4
#undef proto5
#undef proto6
#undef proto7
#undef proto8
#undef proto9
#undef proto10
#undef proto11
#undef proto12
#undef proto13
#undef proto14
#undef proto15
#undef proto

/* definitions of C functions that sum arguments */
#define decln0(t)							\
t C_##t##0(void) {							\
    return 0;								\
}
#define decln1(t)							\
decln0(t)								\
t C_##t##1(t t1) {							\
    return t1;								\
}
#define decln2(t)							\
decln1(t)								\
t C_##t##2(t t1, t t2) {						\
    return t1 - t2;							\
}
#define decln3(t)							\
decln2(t)								\
t C_##t##3(t t1, t t2, t t3) {						\
    return t1 - t2 - t3;						\
}
#define decln4(t)							\
decln3(t)								\
t C_##t##4(t t1, t t2, t t3, t t4) {					\
    return t1 - t2 - t3 - t4;						\
}
#define decln5(t)							\
decln4(t)								\
t C_##t##5(t t1, t t2, t t3, t t4, t t5) {				\
    return t1 - t2 - t3 - t4 - t5;					\
}
#define decln6(t)							\
decln5(t)								\
t C_##t##6(t t1, t t2, t t3, t t4, t t5, t t6) {			\
    return t1 - t2 - t3 - t4 - t5 - t6;					\
}
#define decln7(t)							\
decln6(t)								\
t C_##t##7(t t1, t t2, t t3, t t4, t t5, t t6, t t7) {			\
    return t1 - t2 - t3 - t4 - t5 - t6 - t7;				\
}
#define decln8(t)							\
decln7(t)								\
t C_##t##8(t t1, t t2, t t3, t t4, t t5, t t6, t t7, t t8) {		\
    return t1 - t2 - t3 - t4 - t5 - t6 - t7 - t8;			\
}
#define decln9(t)							\
decln8(t)								\
t C_##t##9(t t1, t t2, t t3, t t4, t t5, t t6, t t7, t t8,		\
	     t t9) {							\
    return t1 - t2 - t3 - t4 - t5 - t6 - t7 - t8 -			\
	   t9;								\
}
#define decln10(t)							\
decln9(t)								\
t C_##t##10(t t1, t t2, t t3, t t4, t t5, t t6, t t7, t t8,		\
	      t t9, t t10) {						\
    return t1 -  t2 - t3 - t4 - t5 - t6 - t7 - t8 -			\
	   t9 - t10;							\
}
#define decln11(t)							\
decln10(t)								\
t C_##t##11(t t1, t  t2, t t3, t t4, t t5, t t6, t t7, t t8,		\
	      t t9, t t10,t t11) {					\
    return t1 -  t2 -  t3 - t4 - t5 - t6 - t7 - t8 -			\
	   t9 - t10 - t11;						\
}
#define decln12(t)							\
decln11(t)								\
t C_##t##12(t t1, t  t2, t  t3, t  t4, t t5, t t6, t t7, t t8,		\
	      t t9, t t10, t t11, t t12) {				\
    return t1 -  t2 -  t3 -  t4 - t5 - t6 - t7 - t8 -			\
	   t9 - t10 - t11 - t12;					\
}
#define decln13(t)							\
decln12(t)								\
t C_##t##13(t t1, t  t2, t  t3, t  t4, t  t5, t t6, t t7, t t8,		\
	      t t9, t t10, t t11, t t12, t t13) {			\
    return t1 -  t2 -  t3 -  t4 -  t5 - t6 - t7 - t8 -			\
	   t9 - t10 - t11 - t12 - t13;					\
}
#define decln14(t)							\
decln13(t)								\
t C_##t##14(t t1, t  t2, t  t3, t  t4, t  t5, t  t6, t t7, t t8,	\
	      t t9, t t10, t t11, t t12, t t13, t t14) {		\
    return t1 -  t2 -  t3 -  t4 -  t5 -  t6 - t7 - t8 -			\
	   t9 - t10 - t11 - t12 - t13 - t14;				\
}
#define decln15(t)							\
decln14(t)								\
t C_##t##15(t t1, t  t2, t  t3, t  t4, t  t5, t  t6, t  t7, t t8,	\
	      t t9, t t10, t t11, t t12, t t13, t t14, t t15) {		\
    return t1 -  t2 -  t3 -  t4 -  t5 -  t6 -  t7 - t8 -		\
	   t9 - t10 - t11 - t12 - t13 - t14 - t15;			\
}
#define decln(t)	decln15(t)
decln(c)
decln(uc)
decln(s)
decln(us)
decln(i)
decln(ui)
decln(l)
decln(ul)
decln(f)
decln(d)
#undef decln0
#undef decln1
#undef decln2
#undef decln3
#undef decln4
#undef decln5
#undef decln6
#undef decln7
#undef decln8
#undef decln9
#undef decln10
#undef decln11
#undef decln12
#undef decln13
#undef decln14
#undef decln15
#undef decln

/* prototypes of C functions that call jit functions that sum arguments */
#define proton(n, t)	t Cjit_##t##n(void);
#define proto0(t)	proton(0, t)
#define proto1(t)	proto0(t)	proton(1, t)
#define proto2(t)	proto1(t)	proton(2, t)
#define proto3(t)	proto2(t)	proton(3, t)
#define proto4(t)	proto3(t)	proton(4, t)
#define proto5(t)	proto4(t)	proton(5, t)
#define proto6(t)	proto5(t)	proton(6, t)
#define proto7(t)	proto6(t)	proton(7, t)
#define proto8(t)	proto7(t)	proton(8, t)
#define proto9(t)	proto8(t)	proton(9, t)
#define proto10(t)	proto9(t)	proton(10, t)
#define proto11(t)	proto10(t)	proton(11, t)
#define proto12(t)	proto11(t)	proton(12, t)
#define proto13(t)	proto12(t)	proton(13, t)
#define proto14(t)	proto13(t)	proton(14, t)
#define proto15(t)	proto14(t)	proton(15, t)
#define proto(t)	proto15(t)
proto(c)
proto(uc)
proto(s)
proto(us)
proto(i)
proto(ui)
proto(l)
proto(ul)
proto(f)
proto(d)
#undef proton
#undef proto0
#undef proto1
#undef proto2
#undef proto3
#undef proto4
#undef proto5
#undef proto6
#undef proto7
#undef proto8
#undef proto9
#undef proto10
#undef proto11
#undef proto12
#undef proto13
#undef proto14
#undef proto15
#undef proto

/* pointers to jit functions that sum arguments, to be initialized by jit  */
#define dcln0(t)	t (*jit_##t##0)(void);
#define dcln1(t)	dcln0(t)	t (*jit_##t##1)(t);
#define dcln2(t)	dcln1(t)	t (*jit_##t##2)(t,t);
#define dcln3(t)	dcln2(t)	t (*jit_##t##3)(t,t,t);
#define dcln4(t)	dcln3(t)	t (*jit_##t##4)(t,t,t,t);
#define dcln5(t)	dcln4(t)	t (*jit_##t##5)(t,t,t,t,t);
#define dcln6(t)	dcln5(t)	t (*jit_##t##6)(t,t,t,t,t,t);
#define dcln7(t)	dcln6(t)	t (*jit_##t##7)(t,t,t,t,t,t,t);
#define dcln8(t)	dcln7(t)	t (*jit_##t##8)(t,t,t,t,t,t,t,t);
#define dcln9(t)	dcln8(t)	t (*jit_##t##9)(t,t,t,t,t,t,t,t,t);
#define dcln10(t)	dcln9(t)	t (*jit_##t##10)(t,t,t,t,t,t,t,t,t,t);
#define dcln11(t)	dcln10(t)	t (*jit_##t##11)(t,t,t,t,t,t,t,t,t,t,t);
#define dcln12(t)	dcln11(t)	t (*jit_##t##12)(t,t,t,t,t,t,t,t,t,t,t,t);
#define dcln13(t)	dcln12(t)	t (*jit_##t##13)(t,t,t,t,t,t,t,t,t,t,t,t,t);
#define dcln14(t)	dcln13(t)	t (*jit_##t##14)(t,t,t,t,t,t,t,t,t,t,t,t,t,t);
#define dcln15(t)	dcln14(t)	t (*jit_##t##15)(t,t,t,t,t,t,t,t,t,t,t,t,t,t,t);
#define dcln(t)		dcln15(t)
dcln(c)
dcln(uc)
dcln(s)
dcln(us)
dcln(i)
dcln(ui)
dcln(l)
dcln(ul)
dcln(f)
dcln(d)
#undef decln0
#undef decln1
#undef decln2
#undef decln3
#undef decln4
#undef decln5
#undef decln6
#undef decln7
#undef decln8
#undef decln9
#undef decln10
#undef decln11
#undef decln12
#undef decln13
#undef decln14
#undef decln15
#undef decln

/* definitions of C functions that call jit functions that sum arguments */
#define decln0(t)							\
t Cjit_##t##0(void) {							\
    return ((*jit_##t##0)());						\
}
#define decln1(t)							\
decln0(t)								\
t Cjit_##t##1(void) {							\
    return ((*jit_##t##1)(1));						\
}
#define decln2(t)							\
decln1(t)								\
t Cjit_##t##2(void) {							\
    return ((*jit_##t##2)(1, 2));					\
}
#define decln3(t)							\
decln2(t)								\
t Cjit_##t##3(void) {							\
    return ((*jit_##t##3)(1, 2, 3));					\
}
#define decln4(t)							\
decln3(t)								\
t Cjit_##t##4(void) {							\
    return ((*jit_##t##4)(1, 2, 3, 4));					\
}
#define decln5(t)							\
decln4(t)								\
t Cjit_##t##5(void) {							\
    return ((*jit_##t##5)(1, 2, 3, 4, 5));				\
}
#define decln6(t)							\
decln5(t)								\
t Cjit_##t##6(void) {							\
    return ((*jit_##t##6)(1, 2, 3, 4, 5, 6));				\
}
#define decln7(t)							\
decln6(t)								\
t Cjit_##t##7(void) {							\
    return ((*jit_##t##7)(1, 2, 3, 4, 5, 6, 7));			\
}
#define decln8(t)							\
decln7(t)								\
t Cjit_##t##8(void) {							\
    return ((*jit_##t##8)(1, 2, 3, 4, 5, 6, 7, 8));			\
}
#define decln9(t)							\
decln8(t)								\
t Cjit_##t##9(void) {							\
    return ((*jit_##t##9)(1, 2, 3, 4, 5, 6, 7, 8,			\
			  9));						\
}
#define decln10(t)							\
decln9(t)								\
t Cjit_##t##10(void) {							\
    return ((*jit_##t##10)(1,  2, 3, 4, 5, 6, 7, 8,			\
			   9, 10));					\
}
#define decln11(t)							\
decln10(t)								\
t Cjit_##t##11(void) {							\
    return ((*jit_##t##11)(1,  2,  3, 4, 5, 6, 7, 8,			\
			   9, 10, 11));					\
}
#define decln12(t)							\
decln11(t)								\
t Cjit_##t##12(void) {							\
    return ((*jit_##t##12)(1,  2,  3,  4, 5, 6, 7, 8,			\
			   9, 10, 11, 12));				\
}
#define decln13(t)							\
decln12(t)								\
t Cjit_##t##13(void) {							\
    return ((*jit_##t##13)(1,  2,  3,  4,  5, 6, 7, 8,			\
			   9, 10, 11, 12, 13));				\
}
#define decln14(t)							\
decln13(t)								\
t Cjit_##t##14(void) {							\
    return ((*jit_##t##14)(1,  2,  3,  4,  5,  6, 7, 8,			\
			   9, 10, 11, 12, 13, 14));			\
}
#define decln15(t)							\
decln14(t)								\
t Cjit_##t##15(void) {							\
    return ((*jit_##t##15)(1,  2,  3,  4,  5,  6,  7, 8,		\
			   9, 10, 11, 12, 13, 14, 15));			\
}
#define decln(t)	decln15(t)
decln(c)
decln(uc)
decln(s)
decln(us)
decln(i)
decln(ui)
decln(l)
decln(ul)
decln(f)
decln(d)
#undef decln0
#undef decln1
#undef decln2
#undef decln3
#undef decln4
#undef decln5
#undef decln6
#undef decln7
#undef decln8
#undef decln9
#undef decln10
#undef decln11
#undef decln12
#undef decln13
#undef decln14
#undef decln15
#undef decln
