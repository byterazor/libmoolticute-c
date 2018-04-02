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

AC_OUTPUT
