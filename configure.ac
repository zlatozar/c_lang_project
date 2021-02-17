dnl Process this file with autoconf to produce a configure script.
dnl
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without
dnl modifications, as long as this notice is preserved.
dnl
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ([2.69])

AC_INIT([c_lang_project], [0.1], [zlatozar@gmail.com])

dnl Must come before AM_INIT_AUTOMAKE.
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_TESTDIR(test)

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_CONFIG_SRCDIR([src/bin/main.c])
AC_CONFIG_HEADERS([config.h])

dnl Checks CC and friends
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_PROG_AR

##################################
#### Checks for integer size. ####
##################################

AC_CHECK_SIZEOF(void*)

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

dnl Use the C language and compiler for the checks
AC_LANG([C])

##################################
####   Checks for libraries.  ####
##################################

AC_SEARCH_LIBS(gethostbyname, nsl)
AC_SEARCH_LIBS(gethostname, nsl)
AC_SEARCH_LIBS(socket, nsl)
AC_SEARCH_LIBS(pthread_barrier_wait, nsl)

##################################
#### Checks for header files. ####
##################################

AC_HEADER_STDC
AC_CHECK_HEADERS( \
    stddef.h      \
    ctype.h       \
    errno.h       \
    limits.h      \
    pthread.h     \
    regex.h       \
    semaphore.h   \
    signal.h      \
    stdarg.h      \
    stdio.h       \
    stdint.h      \
    stdlib.h      \
    string.h      \
    strings.h     \
    time.h        \
    sys/ioctl.h
)

##########################################
#### Checks for typedefs, structures, ####
####  and compiler characteristics.   ####
##########################################

AC_TYPE_SIZE_T
AC_C_BIGENDIAN

AC_CHECK_FILE(/dev/random,
    AC_DEFINE([HAVE_DEV_RANDOM], [1],
        [Define to 1 if you have the /dev/random file.]))

##########################################
####   Checks for library functions.  ####
##########################################

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

##################################
####     Add debug support    ####
##################################

AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    CFLAGS="$AM_CFLAGS -g -Og -DDEBUG -fno-inline -fmax-errors=5 \
-Wall -Werror -Wextra -Wpedantic -fno-common"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$AM_CFLAGS -O3 -DNDEBUG \
-Wall -Werror -Wextra -Wpedantic -fno-common"
fi

dnl Distribute to the final Makefiles
AC_SUBST([CFLAGS])

##################################
####    Chain build files.    ####
##################################

dnl Project libraries
AC_CONFIG_FILES([src/libs/lang/Makefile])
AC_CONFIG_FILES([src/libs/logger/Makefile])
AC_CONFIG_FILES([src/libs/Makefile])

dnl Project
AC_CONFIG_FILES([src/bin/Makefile])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
echo "
'c_lang_project' configuration
--------------------
version                 : ${PACKAGE_VERSION}
source code location    : ${srcdir}
compiler                : ${CC}
compiler flags          : ${CFLAGS}
linker flags            : ${LDFLAGS} ${LIBS}
"
