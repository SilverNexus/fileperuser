dnl This file is processed by autoconf to produce a configure script.

AC_INIT([FilePeruser], [0.02-dev], [silvernexus@users.sourceforge.net])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([./main.c])
AC_CONFIG_MACRO_DIR([macros])
AC_PREFIX_DEFAULT([/usr/local])
AM_INIT_AUTOMAKE

AM_SILENT_RULES([yes])

dnl
dnl Configuration options
dnl

dnl Check to see if debugging is to be enabled
AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug],
              [Build with debugging enabled])],
              [enable_debug=yes], [enable_debug=no])
AM_CONDITIONAL(DEBUG, [test "$enable_debug" = yes])

dnl
dnl Program Checks
dnl
AC_PROG_CC_C99
AC_PROG_LIBTOOL

dnl
dnl Header File, Type, Structure, and Compiler Checks
dnl

dnl Checks for header files.
AC_CHECK_HEADERS([ftw.h])
AC_CHECK_HEADERS([dirent.h])

dnl
dnl Library Function Checks
dnl
AC_CHECK_FUNCS([nftw mmap strdup])

AS_BOX([Generate Output])
AC_CONFIG_FILES([Makefile
                 doc/Makefile])

AC_OUTPUT

dnl
dnl Configuration Summary
dnl
AC_MSG_NOTICE([Configuration Summary ----------------------------------------------])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([  Paths])
AC_MSG_NOTICE([    Installation prefix                  $prefix])
AC_MSG_NOTICE([    Will put executables in              $bindir])
AC_MSG_NOTICE([    Will put resources in                $datadir])
AC_MSG_NOTICE()
AC_MSG_NOTICE([  Debugging                              $enable_debug])
