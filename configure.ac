dnl Required autoconf version
AC_PREREQ([2.69])

dnl Declare project and version
AC_INIT([cute], [0.1])

dnl Ensure src/cute.ld is there to make sure that the directory that it is told
dnl contains the source code in fact does.
AC_CONFIG_SRCDIR([src/cute.ld])

dnl Locate auxiliary build tools in a separate directory
AC_CONFIG_AUX_DIR([build-aux])

dnl Initialize automake. Just to allow autoreconf to install auxiliary build
dnl tools needed by some other AC macros such as AC_FUNC_STRNLEN for example.
AM_INIT_AUTOMAKE

dnl Generate Makefile from Makefile.in
AC_CONFIG_FILES([Makefile])

dnl Generate config.h.in using autoreconf / autoheader
AC_CONFIG_HEADERS([config.h])

dnl Check presence archive tool, use Link Time Optimization compliant AR by
dnl default
AC_CHECK_TOOL([AR], [gcc-ar], [:])

dnl install and mkdir -p portability
AC_PROG_INSTALL
AC_PROG_MKDIR_P

dnl Check presence of a working compiler with GNU extensions if available
AC_GNU_SOURCE
AC_PROG_CC

# Check compiler characteristics and requires GNU extensions
AC_PROG_CC_STDC
AC_C_CONST
AC_C_RESTRICT
AC_C_INLINE

# Checks for typedefs, structures, and header files.
AC_CHECK_HEADER_STDBOOL
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STRNLEN

for func in alarm dup2 asprintf strdup strerror strndup strtoul; do
	AC_CHECK_FUNCS([$func],, AC_MSG_ERROR([$func function not found]))
done

dnl Check assert.h, will also add the --disable-assert option
AC_HEADER_ASSERT

dnl generate Makefile
AC_OUTPUT
