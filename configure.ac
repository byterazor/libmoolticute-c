#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_LANG_PUSH([C])
AC_INIT([libmoolticute-c], [0.0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR
LT_INIT
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
])


# Checks for programs.
AC_PROG_CC
CFLAGS=-Wall

AC_SEARCH_LIBS([pthread_create], [pthread], [], [
  AC_MSG_ERROR([unable to find pthread library])
])

AC_SEARCH_LIBS([json_object_new_object], [json-c], [], [
  AC_MSG_ERROR([unable to find json-c library])
])

AC_SEARCH_LIBS([lws_create_context], [websockets], [], [
  AC_MSG_ERROR([unable to find websockets library])
])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h stdio.h ])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strtol])


AC_ARG_ENABLE(valgrind,
	AC_HELP_STRING([--enable-valgrind],
	[Run libmoolticute-c using valgrind]))

AC_MSG_CHECKING([valgrind])
valgrind_status="yes"

AM_CONDITIONAL(WITH_VALGRIND, test "$enable_valgrind" = "run")

# Build valgrind support into code
if test "$enable_valgrind" = "yes"; then
  CFLAGS="$CFLAGS -O0 -ggdb3"
	AC_MSG_RESULT(yes)
# No valgrind
else
	AC_MSG_RESULT(no)
	valgrind_status="no"
fi



AC_OUTPUT
