# valgrind.m4 - Check for Valgrind.

# AX_PROG_VALGRIND
# ----------------
# Setup Valgrind and the "memcheck" target(s).
AC_DEFUN([AX_PROG_VALGRIND],
[dnl
  AC_ARG_VAR([VALGRIND_PATH], [Path to the Valgrind executable.])
  AC_ARG_VAR([VALGRIND_FLAGS], [Flags for Valgrind.])
  AC_ARG_VAR([VALGRIND_ENVIRONMENT],
       [Environment variables for Valgrind.])
  AC_CHECK_PROGS([VALGRIND_PATH], [valgrind], [:])
  AM_CONDITIONAL([HAVE_VALGRIND], [test "x$VALGRIND_PATH" != "x:"])
])dnl
