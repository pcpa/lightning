#
# Copyright (C) 2010  Paulo Cesar Pereira de Andrade
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Authors:
#	Paulo Cesar Pereira de Andrade
#

check_PROGRAMS = lightning

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS =					\
	-I$(srcdir)/../..			\
	-I$(srcdir)/../../lightning		\
	-I$(srcdir)/../../lightning/$(cpu)
AM_CFLAGS = $(WARN_CFLAGS) $(DEBUG_CFLAGS) $(LIGHTNING_CFLAGS)

lightning_LDADD = $(LIGHTNING_LIBS)

lightning_SOURCES =		\
	lightning.c

# hackish schema to get it to build a loadable module to be
# used with LD_PRELOAD for the test of calls to C functions
functions_la_CFLAGS = -O0 -g3
functions_la_LTLIBRARIES = functions.la
functions_la_LDFLAGS = -module -avoid-version
functions_la_SOURCES = functions.c
functions_ladir = $(libdir)
# do not install functions.so
install:

TESTS=test.sh

EXTRA_DIST = 			\
	test.sh			\
	alu_add.tst		\
	alu_and.tst		\
	alu_div.tst		\
	alu_hmul.tst		\
	alu.inc			\
	alu_lsh.tst		\
	alu_mod.tst		\
	alu_mul.tst		\
	alu_neg.tst		\
	alu_not.tst		\
	alu_or.tst		\
	alu_rsb.tst		\
	alu_rsh.tst		\
	alu_sub.tst		\
	alux_add.tst		\
	alu_xor.tst		\
	alux_sub.tst		\
	args.tst		\
	bottles.tst		\
	branch_cmp.tst		\
	branch.inc		\
	branch.tst		\
	call.tst		\
	carry.tst		\
	c_call.tst		\
	clobber.tst		\
	consts.inc		\
	cvt.tst			\
	fcvt.tst		\
	float_branch.inc	\
	fop_abs.tst		\
	fop_atan.tst		\
	fop_cos.tst		\
	fop_log10.tst		\
	fop_log2.tst		\
	fop_log.tst		\
	fop_sin.tst		\
	fop_sqrt.tst		\
	fop_tan.tst		\
	hello.tst		\
	incr.tst		\
	ldst_consts.inc		\
	ldst_i.tst		\
	ldst.tst		\
	ldst_xi.tst		\
	ldst_xr-clobber.tst	\
	ldst_xr.tst		\
	movi.tst		\
	nfibs.tst		\
	printf.tst		\
	rfibs.tst		\
	rpn-non-portable.tst	\
	rpn.tst			\
	scanf.tst		\
	self.tst		\
	stack.tst		\
	varargs.tst		\
	regression.tst
