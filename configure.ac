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

# Library versioning
# Making releases:
#     micro_version += 1
#     interface_age += 1
# If functions have been added or backward-compatibility has been broken:
#     interface_age = 0
m4_define([major_version], [0])
m4_define([minor_version], [2])
m4_define([micro_version], [0])

m4_define([interface_age], [0])

# Initialize autoconf & automake
AC_INIT([C_lang_project], [major_version.minor_version.micro_version],
        [https://github.com/zlatozar/c_lang_project/issues], [c_lang_project])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([1.9 foreign subdir-objects])
AM_SILENT_RULES([no])

AC_CONFIG_SRCDIR([src/bin/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_TESTDIR(test)

dnl Checks CC and friends
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AR
AC_PROG_INSTALL

##################################
####  Libtool configuration.  ####
##################################

LT_PREREQ([2.2])
LT_INIT

# Libtool versioning
LT_CURRENT=m4_eval(100 * minor_version + micro_version - interface_age)
LT_REVISION=interface_age

LT_AGE=m4_eval(m4_eval(100 * minor_version + micro_version) - interface_age)

AC_SUBST([LT_CURRENT])
AC_SUBST([LT_REVISION])
AC_SUBST([LT_AGE])

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

AX_PTHREAD

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
    semaphore.h   \
    regex.h       \
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

AC_CHECK_FILE(/dev/random,
    AC_DEFINE([HAVE_DEV_RANDOM], [1],
        [Define to 1 if you have the /dev/random file.]))

##########################################
####   Checks for library functions.  ####
##########################################

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memcpy])

##################################
###  Checks for build process. ###
##################################

AX_PROG_VALGRIND
# OS-specific settings
AS_CASE([$host_os],
        [darwin*], [VALGRIND_FLAGS="--dsymutil=yes"])

AX_PROG_DOXYGEN
AX_PROG_GDB

# Setup unit-test environment
PKG_CHECK_MODULES([CHECK], [check >= 0.9],
                  [have_check=yes
                    VALGRIND_ENVIRONMENT="CK_FORK=no"
                    GDB_ENVIRONMENT="CK_FORK=no"
                    # Some versions of the Check header file use
                    # GNU-style token pasting.
                    AX_TRY_CFLAGS([-Wno-gnu],
                    [CHECK_CFLAGS="$CHECK_CFLAGS -Wno-gnu"])],
                  [have_check=no
                    AC_MSG_WARN([Unable to run unit tests.])])
AM_CONDITIONAL([HAVE_CHECK], [test "x$have_check" = "xyes"])

##################################
###  Setup compilation flags.  ###
##################################

# NOTE: Reset flags that are used for checks
CFLAGS=

AX_TRY_CFLAGS([-Werror], [AX_CFLAGS([-Werror])])

AX_TRY_CFLAGS([-Wall], [AX_CFLAGS([-Wall])])
AX_TRY_CFLAGS([-Wextra], [AX_CFLAGS([-Wextra])])
AX_TRY_CFLAGS([-pedantic], [AX_CFLAGS([-pedantic])])

dnl Enable all warnings
AX_TRY_CFLAGS([-Wformat], [AX_CFLAGS([-Wformat=2])])
AX_TRY_CFLAGS([-Wno-unused-parameter], [AX_CFLAGS([-Wno-unused-parameter])])
AX_TRY_CFLAGS([-Wundef], [AX_CFLAGS([-Wundef])])
AX_TRY_CFLAGS([-Wshadow], [AX_CFLAGS([-Wshadow])])
AX_TRY_CFLAGS([-Wwrite-strings], [AX_CFLAGS([-Wwrite-strings])])
AX_TRY_CFLAGS([-Wstrict-prototypes], [AX_CFLAGS([-Wstrict-prototypes])])
AX_TRY_CFLAGS([-Wold-style-definition], [AX_CFLAGS([-Wold-style-definition])])
AX_TRY_CFLAGS([-Wredundant-decls], [AX_CFLAGS([-Wredundant-decls])])
AX_TRY_CFLAGS([-Wnested-externs], [AX_CFLAGS([-Wnested-externs])])
AX_TRY_CFLAGS([-Wmaybe-uninitialized], [AX_CFLAGS([-Wmaybe-uninitialized])])
AX_TRY_CFLAGS([-Wlogical-op], [AX_CFLAGS([-Wlogical-op])])
AX_TRY_CFLAGS([-Wjump-misses-init], [AX_CFLAGS([-Wjump-misses-init])])
AX_TRY_CFLAGS([-Wmissing-include-dirs], [AX_CFLAGS([-Wmissing-include-dirs])])
AX_TRY_CFLAGS([-fno-common], [AX_CFLAGS([-fno-common])])

AX_TRY_CFLAGS([-Wcast-align], [AX_CFLAGS([-Wcast-align])])

AX_TRY_LDFLAGS([-Wl,--no-undefined], [AX_LDFLAGS([-Wl,--no-undefined])])

##################################
####     Add debug support    ####
##################################

AC_MSG_CHECKING([Whether to build with debug information])
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
        [enable debug data generation (def=no)])],
    [debugit="$enableval"],
    [debugit=no])
AC_MSG_RESULT([$debugit])

if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    CFLAGS="$CFLAGS -g -Og -DDEBUG -fno-inline -fmax-errors=5"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$CFLAGS -g -O3 -DNDEBUG"
fi

##################################
###  Add memory check support  ###
##################################

AM_CONDITIONAL([MEMDEV_MODE], [test "x$debugit" = "xyes"])

##################################
####    Chain build files.    ####
##################################

dnl Project libraries
AC_CONFIG_FILES([src/libs/lang/Makefile])
AC_CONFIG_FILES([src/libs/logger/Makefile])
AC_CONFIG_FILES([src/libs/Makefile])

dnl API documentation
AC_CONFIG_FILES([tools-setup/Doxyfile])

dnl Project executable
AC_CONFIG_FILES([src/bin/Makefile])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
AC_MSG_RESULT([
'c_lang_project' configuration
--------------------
version                 : ${PACKAGE_VERSION}
source code location    : ${srcdir}
compiler                : ${CC}
compiler flags          : ${CFLAGS}
linker flags            : ${LDFLAGS} ${LIBS}
])
