#!/bin/sh

PROGRAM=./lightning

if [ -f $PROGRAM ]; then
    for test in	ldst ldst_xi ldst_xr ldst_xr-clobber ldst_i movi \
    		cvt \
		branch branch_cmp \
		varargs args stack call \
		alu_add alu_sub alu_rsb alux_add alux_sub \
		alu_mul alu_div alu_mod alu_hmul \
		alu_and alu_or alu_xor \
		alu_lsh alu_rsh \
		alu_neg alu_not \
		clobber \
		fcvt fop_abs fop_sqrt fop_sin fop_cos fop_tan;
    do
	if [ -f $test.tst ]; then
	    $PROGRAM $test.tst
	    [ $? -ne 0 ] && echo "$test.tst: Bad exit status"
	else
	    echo "$test.tst missing"
	fi
    done
else
    echo "$PROGRAM missing"
    exit -1
fi
