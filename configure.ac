dnl Required autoconf version
AC_PREREQ([2.69])

dnl Declare project and version
AC_INIT([cute], [0.1])

dnl Ensure src/cute.ld is there to make sure that the directory that it is told
dnl contains the source code in fact does.
AC_CONFIG_SRCDIR([src/cute.lds])

dnl Tell autoconf where to search for additional macros
AC_CONFIG_MACRO_DIR([m4])

dnl Locate auxiliary build tools in a separate directory
AC_CONFIG_AUX_DIR([build-aux])

dnl Initialize automake. Just to allow autoreconf to install auxiliary build
dnl tools needed by some other AC macros such as AC_FUNC_STRNLEN for example.
AM_INIT_AUTOMAKE

dnl Generate Makefile from Makefile.in
AC_CONFIG_FILES([Makefile])

dnl So that config.status ensures that config.h is considered up to date
AC_CONFIG_FILES([stamp-h], [echo timestamp > stamp-h])

dnl Generate config.h.in using autoreconf / autoheader
AC_CONFIG_HEADERS([config.h])

dnl Check presence archive tool, use Link Time Optimization compliant AR by
dnl default
AC_CHECK_TOOL([AR], [gcc-ar], [:])

dnl install and mkdir -p portability
AC_PROG_INSTALL
AC_PROG_MKDIR_P

dnl Check presence of a working compiler with GNU extensions if available
dnl and generate _GNU_SOURCE CFLAGS accordingly into config.h
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC

dnl Check compiler characteristics and requires GNU extensions
AC_PROG_CC_STDC
AC_C_CONST
AC_C_RESTRICT
AC_C_INLINE

dnl Check wether compiler allows us to prevent from top-level statements
dnl reordering
NO_TOPLEVEL_REORDER_CFLAGS=""

dnl Check __attribute__((no_reorder)) support first
AX_GCC_VAR_ATTRIBUTE([no_reorder])
if test "x$ax_cv_have_var_attribute_no_reorder" != xyes; then
	dnl no_reorder attribute not supported, try -fno-toplevel-reorder
	dnl compiler option
	AX_CHECK_COMPILE_FLAG([-fno-toplevel-reorder],
	                      [NO_TOPLEVEL_REORDER_CFLAGS="-fno-toplevel-reorder"],
	                      [AC_MSG_ERROR([neither -fno-toplevel-reorder nor __attribute__((no_reorder)) supported])])
fi
dnl Now that NO_TOPLEVEL_REORDER_CFLAGS is properly set, allow substitution
dnl within Makefile.in
AC_SUBST(NO_TOPLEVEL_REORDER_CFLAGS)

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

dnl Check assert.h, will also add the --disable-assert option and generate
dnl NDEBUG CFLAGS accordingly into config.h
AC_HEADER_ASSERT

dnl generate all files specified by AC_CONFIG_ macros
AC_OUTPUT

dnl Small macro to print out a nice, pretty section title.
define(SECTION_TITLE,
[
	echo >& AS_MESSAGE_FD
	echo '   $1   ' | sed -e's/./-/g' >&AS_MESSAGE_FD
	echo '   $1' >& AS_MESSAGE_FD
	echo '   $1   ' | sed -e's/./-/g' >&AS_MESSAGE_FD
	echo >& AS_MESSAGE_FD
])

SECTION_TITLE([Configuration results])
echo "prefix       $prefix" >& AS_MESSAGE_FD
echo "exec_prefix  $exec_prefix" >& AS_MESSAGE_FD
echo "bindir       $bindir" >& AS_MESSAGE_FD
echo "libdir       $libdir" >& AS_MESSAGE_FD
echo "includedir   $includedir" >& AS_MESSAGE_FD
echo "libexecdir   $libexecdir" >& AS_MESSAGE_FD
echo "" >& AS_MESSAGE_FD
echo "assertions   $ac_enable_assert" >& AS_MESSAGE_FD
