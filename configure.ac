dnl Required autoconf version
dnlAC_PREREQ([2.69])

dnl Declare project and version
AC_INIT([cute], [0.1])
dnl Ensure src/cute.ld is there to make sure that the directory that it is told
dnl contains the source code in fact does.
dnl AC_CONFIG_SRCDIR([src/cute.ld])

dnl Generate Makefile from Makefile.in
dnl AC_CONFIG_FILES([Makefile])

dnl Generate config.h
dnl AC_CONFIG_HEADERS([config.h])

dnl AC_CONFIG_AUX_DIR([build-aux])

dnl AM_INIT_AUTOMAKE
dnl see AM_PROG_AR([act-if-fail])



dnl Check presence of a working compiler and archive tool
dnl Use Link Time Optimization compliant AR by default
AC_PROG_CC
AC_CHECK_TOOL([AR], [gcc-ar], [:])

# Requires GNU extensions
dnlAC_GNU_SOURCE

# Checks for header files.
dnlAC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
dnl AC_CHECK_HEADER_STDBOOL
dnl AC_C_INLINE
dnl AC_TYPE_PID_T
dnl AC_TYPE_SIZE_T
dnl AC_TYPE_SSIZE_T

# Checks for library functions.
dnlAC_FUNC_FORK
dnlAC_FUNC_MALLOC
dnlAC_FUNC_STRNLEN
dnlAC_CHECK_FUNCS([alarm dup2 strdup strerror strndup strtoul])

dnl Check assert.h, will also add the --disable-assert option
dnlAC_HEADER_ASSERT

dnl generate Makefile
AC_OUTPUT(Makefile)
