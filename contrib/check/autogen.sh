#!/bin/sh

# make autoreconf happy
[ -d m4 ] || mkdir m4
[ -f NEWS ] || touch NEWS
[ -f AUTHORS ] || touch AUTHORS
[ -f ChangeLog ] || touch ChangeLog

autoreconf -v --install || exit 1
./configure "$@"
