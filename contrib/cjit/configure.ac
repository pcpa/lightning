dnl
dnl Copyright (C) 2010  Paulo Cesar Pereira de Andrade.
dnl
dnl This is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This software is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl Authors:
dnl	Paulo Cesar Pereira de Andrade
dnl

dnl  Process this file with autoconf to create configure.

AC_PREREQ([2.57])
AC_INIT(cjit, [0.0.1], paulo.cesar.pereira.de.andrade@gmail.com, cjit)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CHECK_HEADER([dlfcn.h], [DYNAMIC="yes"], [DYNAMIC="no"])
if test "x$DYNAMIC" != "xno"; then
    CJIT_LIBS="$CJIT_LIBS -ldl"
fi

dnl Debug/production build enable/disable
AC_ARG_ENABLE(debug,
	      AS_HELP_STRING([--enable-debug],
			     [Enable debug code, consistency checking, and assertions]),
	      [DEBUG=$enableval], [DEBUG=auto])
if test "x$DEBUG" != "xno"; then
    DEBUG_CFLAGS="-DDEBUG=1 -D_ASM_SAFETY=1"
else
    DEBUG_CFLAGS="-DNDEBUG=1"
fi
AC_SUBST([DEBUG_CFLAGS])

WARN_CFLAGS=""
VISIBILITY_FLAGS=""
if test "x$GCC" = "xyes"; then
    WARN_CFLAGS="\
-Wall \
-Wbad-function-cast \
-Wdeclaration-after-statement \
-Wmissing-prototypes \
-Wmissing-declarations \
-Wnested-externs \
-fno-strict-aliasing \
-Wold-style-definition \
-Wpointer-arith \
-Wstrict-prototypes"
    VISIBILITY_FLAGS="-fvisibility=hidden"
fi
AC_SUBST([WARN_CFLAGS])

CJIT_LIBS="$CJIT_LIBS -lm"
CJIT_CFLAGS="$CJIT_CFLAGS -std=c99 -pthread $VISIBILITY_FLAGS -D_GNU_SOURCE=1"

case "$target_cpu" in
  i?86|x86_64)	cpu=i386	;;
  sparc*)	cpu=sparc	;;
  powerpc)	cpu=ppc		;;
  mips*)	cpu=mips	;;
  *)				;;
esac
AC_SUBST(cpu)

AC_SUBST([CJIT_LIBS])
AC_SUBST([CJIT_CFLAGS])
AC_OUTPUT([Makefile])
