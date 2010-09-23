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

bin_PROGRAMS = lightning

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(WARN_CFLAGS) $(DEBUG_CFLAGS) $(LIGHTNING_CFLAGS)

lightning_LDADD = $(LIGHTNING_LIBS)

lightning_SOURCES =		\
	lightning.c
