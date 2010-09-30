#!/bin/sh

PROGRAM=./lightning
STATUS=0
[ "x$srcdir" = "x" ] && srcdir=.

for test in \
    ldst ldst_xi ldst_xr ldst_xr-clobber ldst_i movi \
    cvt \
    branch branch_cmp \
    varargs args stack call \
    alu_add alu_sub alu_rsb alux_add alux_sub \
    alu_mul alu_div alu_mod alu_hmul \
    alu_and alu_or alu_xor \
    alu_lsh alu_rsh \
    alu_neg alu_not \
    clobber \
    fcvt fop_abs fop_sqrt \
    fop_sin fop_cos fop_tan fop_atan \
    fop_log fop_log2 fop_log10;
do
    echo "$test"
    ERROR=`$PROGRAM $srcdir/$test.tst`
    if [ $? -ne 0 -o "x$ERROR" != "x" ]; then
	STATUS=1
	echo "  ERROR"
    fi
done

test=c_call
echo "$test"
ERROR=`LD_PRELOAD=.libs/functions.so $PROGRAM $srcdir/$test.tst`
if [ $? -ne 0 -o "x$ERROR" != "x" ]; then
    STATUS=1
    echo "  ERROR"
fi

exit $STATUS
