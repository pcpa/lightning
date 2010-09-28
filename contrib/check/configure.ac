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
AC_INIT(check, [0.0.1], paulo.cesar.pereira.de.andrade@gmail.com, check)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CHECK_HEADER([lightning.h], [LIGHTNING="yes"], [LIGHTNING="no"])
if test "x$LIGHTNING" = "xno"; then
    AC_MSG_ERROR([lightning not found, see http://www.gnu.org/software/lightning/])
fi
AC_ARG_ENABLE(disassembler,
	      AS_HELP_STRING([--enable-disassembler],
			     [Enable jit disassembler using binutils]),
	      [DISASSEMBLER=$enableval], [DISASSEMBLER=auto])
if test "x$DISASSEMBLER" != "xno"; then
    # FIXME need to check for libiberty first or will fail to link
    AC_CHECK_LIB(iberty, htab_try_create, ,
		 [IBERTY="no"])
    AC_CHECK_LIB(bfd, bfd_init, ,
		 [BFD="no"])
    AC_CHECK_LIB(opcodes, init_disassemble_info, ,
		 [OPCODES="no"])
    if test	"x$IBERTY"	 = "xno" -o \
		"x$BFD"	 	 = "xno" -o \
		"x$OPCODES"	 = "xno"; then
	if test "x$DISASSEMBLER" != "xauto"; then
	    AC_MSG_ERROR([binutils not found, see http://www.gnu.org/software/binutils/])
	else
	    AC_MSG_WARN([binutils not found, see http://www.gnu.org/software/binutils/])
	    DISASSEMBLER="no"
	fi
    fi
fi
if test "x$DISASSEMBLER" != "xno"; then
    LIGHTNING_CFLAGS="$LIGHTNING_CFLAGS -DDISASSEMBLER=1"
fi

AC_CHECK_HEADER([dlfcn.h], [DYNAMIC="yes"], [DYNAMIC="no"])
if test "x$DYNAMIC" != "xno"; then
    LIGHTNING_LIBS="$LIGHTNING_LIBS -ldl"
    LIGHTNING_CFLAGS="$LIGHTNING_CFLAGS -DDYNAMIC=1"
fi

AC_ARG_ENABLE(preprocessor,
	      AS_HELP_STRING([--enable-preprocessor],
			     [Enable filtering input through cpp]),
	      [PREPROCESSOR=$enableval], [PREPROCESSOR=auto])
if test "x$PREPROCESSOR" != "xno"; then
    LIGHTNING_CFLAGS="$LIGHTNING_CFLAGS -DPREPROCESSOR=1"
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

LIGHTNING_LIBS="$LIGHTNING_LIBS -lm"
LIGHTNING_CFLAGS="$LIGHTNING_CFLAGS -std=c99 -pthread $VISIBILITY_FLAGS -D_GNU_SOURCE=1"

case "$target_cpu" in
  i?86)    cpu=i386	;;
  x86_64)  cpu=x86_64	;;
  sparc*)  cpu=sparc	;;
  powerpc) cpu=ppc	;;
  *)		        ;;
esac
AC_SUBST(cpu)

AC_SUBST([LIGHTNING_LIBS])
AC_SUBST([LIGHTNING_CFLAGS])
AC_OUTPUT([Makefile])
