
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

#define proto0(t)	t t##0(void);
#define proto1(t)	proto0(t)	t t##1(t);
#define proto2(t)	proto1(t)	t t##2(t,t);
#define proto3(t)	proto2(t)	t t##3(t,t,t);
#define proto4(t)	proto3(t)	t t##4(t,t,t,t);
#define proto5(t)	proto4(t)	t t##5(t,t,t,t,t);
#define proto6(t)	proto5(t)	t t##6(t,t,t,t,t,t);
#define proto7(t)	proto6(t)	t t##7(t,t,t,t,t,t,t);
#define proto8(t)	proto7(t)	t t##8(t,t,t,t,t,t,t,t);
#define proto9(t)	proto8(t)	t t##9(t,t,t,t,t,t,t,t,t);
#define proto10(t)	proto9(t)	t t##10(t,t,t,t,t,t,t,t,t,t);
#define proto11(t)	proto10(t)	t t##11(t,t,t,t,t,t,t,t,t,t,t);
#define proto12(t)	proto11(t)	t t##12(t,t,t,t,t,t,t,t,t,t,t,t);
#define proto13(t)	proto12(t)	t t##13(t,t,t,t,t,t,t,t,t,t,t,t,t);
#define proto14(t)	proto13(t)	t t##14(t,t,t,t,t,t,t,t,t,t,t,t,t,t);
#define proto15(t)	proto14(t)	t t##15(t,t,t,t,t,t,t,t,t,t,t,t,t,t,t);
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

#define decln0(t)							\
t t##0(void) {								\
    return 0;								\
}
#define decln1(t)							\
decln0(t)								\
t t##1(t t1) {								\
    return t1;								\
}
#define decln2(t)							\
decln1(t)								\
t t##2(t t1, t t2) {							\
    return t1 + t2;							\
}
#define decln3(t)							\
decln2(t)								\
t t##3(t t1, t t2, t t3) {						\
    return t1 + t2 + t3;						\
}
#define decln4(t)							\
decln3(t)								\
t t##4(t t1, t t2, t t3, t t4) {					\
    return t1 + t2 + t3 + t4;						\
}
#define decln5(t)							\
decln4(t)								\
t t##5(t t1, t t2, t t3, t t4, t t5) {					\
    return t1 + t2 + t3 + t4 + t5;					\
}
#define decln6(t)							\
decln5(t)								\
t t##6(t t1, t t2, t t3, t t4, t t5, t t6) {				\
    return t1 + t2 + t3 + t4 + t5 + t6;					\
}
#define decln7(t)							\
decln6(t)								\
t t##7(t t1, t t2, t t3, t t4, t t5, t t6, t t7) {			\
    return t1 + t2 + t3 + t4 + t5 + t6 + t7;				\
}
#define decln8(t)							\
decln7(t)								\
t t##8(t t1, t t2, t t3, t t4, t t5, t t6, t t7, t t8) {		\
    return t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8;			\
}
#define decln9(t)							\
decln8(t)								\
t t##9(t t1, t t2, t t3, t t4, t t5, t t6, t t7, t t8,			\
       t t9) {								\
    return t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 +			\
	   t9;								\
}
#define decln10(t)							\
decln9(t)								\
t t##10(t t1, t t2, t t3, t t4, t t5, t t6, t t7, t t8,			\
	t t9, t t10) {							\
    return t1 +  t2 + t3 + t4 + t5 + t6 + t7 + t8 +			\
	   t9 + t10;							\
}
#define decln11(t)							\
decln10(t)								\
t t##11(t t1, t  t2, t t3, t t4, t t5, t t6, t t7, t t8,		\
	t t9, t t10,t t11) {						\
    return t1 +  t2 +  t3 + t4 + t5 + t6 + t7 + t8 +			\
	   t9 + t10 + t11;						\
}
#define decln12(t)							\
decln11(t)								\
t t##12(t t1, t  t2, t  t3, t  t4, t t5, t t6, t t7, t t8,		\
	t t9, t t10, t t11, t t12) {					\
    return t1 +  t2 +  t3 +  t4 + t5 + t6 + t7 + t8 +			\
	   t9 + t10 + t11 + t12;					\
}
#define decln13(t)							\
decln12(t)								\
t t##13(t t1, t  t2, t  t3, t  t4, t  t5, t t6, t t7, t t8,		\
	t t9, t t10, t t11, t t12, t t13) {				\
    return t1 +  t2 +  t3 +  t4 +  t5 + t6 + t7 + t8 +			\
	   t9 + t10 + t11 + t12 + t13;					\
}
#define decln14(t)							\
decln13(t)								\
t t##14(t t1, t  t2, t  t3, t  t4, t  t5, t  t6, t t7, t t8,		\
	t t9, t t10, t t11, t t12, t t13, t t14) {			\
    return t1 +  t2 +  t3 +  t4 +  t5 +  t6 + t7 + t8 +			\
	   t9 + t10 + t11 + t12 + t13 + t14;				\
}
#define decln15(t)							\
decln14(t)								\
t t##15(t t1, t  t2, t  t3, t  t4, t  t5, t  t6, t  t7, t t8,		\
	t t9, t t10, t t11, t t12, t t13, t t14, t t15) {		\
    return t1 +  t2 +  t3 +  t4 +  t5 +  t6 +  t7 + t8 +		\
	   t9 + t10 + t11 + t12 + t13 + t14 + t15;			\
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
