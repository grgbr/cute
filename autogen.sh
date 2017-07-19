#!/bin/sh

# Autoreconf will produce necessary files needed to run configure.
# It will complain (and fail) since we use no Makefile.am setup...
autoreconf --install --force >/dev/null 2>&1 || :;

# Generate autoconf macros into aclocal.m4, otherwise they will not be seen
# by autoconf
#aclocal --force

# Generate configure script using macros defined above
#autoconf

# Generate config.h.in
#autoheader --force

# Generate auxiliary files
#mkdir build-aux
#automake --add-missing --copy --force-missing --foreign >/dev/null 2>&1 || :;
