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

bin_PROGRAMS = cjit

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS =					\
	-I$(srcdir)/../..			\
	-I$(srcdir)/../../lightning		\
	-I$(srcdir)/../../lightning/$(cpu)
AM_CFLAGS = $(WARN_CFLAGS) $(DEBUG_CFLAGS) $(CJIT_CFLAGS)

cjit_LDADD = $(CJIT_LIBS)

cjit_SOURCES =		\
	cjit.c		\
	cjit_alloc.c	\
	cjit_debug.c	\
	cjit_emit.c	\
	cjit_error.c	\
	cjit_eval.c	\
	cjit_expr.c	\
	cjit_hash.c	\
	cjit_parser.c	\
	cjit_read.c	\
	cjit_stack.c	\
	cjit_type.c	\
	thunder.c

EXTRA_DIST =		\
	cjit.h		\
	thunder.h
