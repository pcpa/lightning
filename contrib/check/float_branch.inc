/* corrected in lightning git master */
#define FLOAT_BRANCH_WORKS	1
#if FLOAT_BRANCH_WORKS
#  define float_cmp_branch_true(LABEL, R0, F0, F1)	\
	beqr_f	LABEL	%F0 %F1
#  define float_cmp_branch_false(LABEL, R0, F0, F1)	\
	bner_f	LABEL	%F0 %F1
#  define double_cmp_branch_true(LABEL, R0, F0, F1)	\
	beqr_d	LABEL	%F0 %F1
#  define double_cmp_branch_false(LABEL, R0, F0, F1)	\
	bner_d	LABEL	%F0 %F1
#else
#  define float_cmp_branch_true(LABEL, R0, F0, F1)	\
	eqr_f %R0 %F0 %F1				\
	bnei_i LABEL %R0 0
#  define float_cmp_branch_false(LABEL, R0, F0, F1)	\
	eqr_f %R0 %F0 %F1				\
	beqi_i LABEL %R0 0
#  define double_cmp_branch_true(LABEL, R0, F0, F1)	\
	eqr_d %R0 %F0 %F1				\
	bnei_i LABEL %R0 0
#  define double_cmp_branch_false(LABEL, R0, F0, F1)	\
	eqr_d %R0 %F0 %F1				\
	beqi_i LABEL %R0 0
#endif
