/*
 *	XvN:		a X value
 *	XsYN:		Y sign extended to X value
 */

#if __WORDSIZE == 64
# define lv0		0x8000000000000001
# define lsc0		0xffffffffffffff81
# define lss0		0xffffffffffff8001
# define lsi0		0xffffffff80000001
# define lv1		0x8000000000000000
# define lsc1		0xffffffffffffff80
# define lss1		0xffffffffffff8000
# define lsi1		0xffffffff80000000
# define lv2		0x7fffffffffffffff
# define lsc2		0x000000000000007f
# define lss2		0x0000000000007fff
# define lsi2		0x000000007fffffff
# define lv3		0xffffffffffffffff
#else
# define lv0		0x80000001
# define lsc0		0xffffff81
# define lss0		0xffff8001
# define lsi0		lv0
# define lv1		0x80000000
# define lsc1		0xffffff80
# define lss1		0xffff8000
# define lsi1		lv1
# define lv2		0x7fffffff
# define lsc2		0x0000007f
# define lss2		0x00007fff
# define lsi2		0x7fffffff
# define lv3		0xffffffff
#endif

#define lsc3		lv3
#define lss3		lv3
#define lsi3		lv3

# define iv0		0x80000001
# define isc0		0xffffff81
# define iss0		0xffff8001

# define iv1		0x80000000
# define isc1		0xffffff80
# define iss1		0xffff8000

# define iv2		0x7fffffff
# define isc2		0x0000007f
# define iss2		0x00007fff

# define iv3		0xffffffff
# define isc3		iv3
# define iss3		iv3

# define sv0		0x8001
# define sv1		0x8000
# define sv2		0x7fff
# define sv3		0xffff

#define cv0		0x81
#define cv1		0x80
#define cv2		0x7f
#define cv3		0xff

#define pv0		lv0
#define pv1		lv1
#define pv2		lv2
#define pv3		lv3
#define fv0		 0.25
#define fv1		 0.75
#define fv2		-0.25
#define fv3		-0.75
#define dv0		fv0
#define dv1		fv1
#define dv2		fv2
#define dv3		fv3
