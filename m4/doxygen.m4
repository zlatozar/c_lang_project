# gdb.m4 - Check for Doxygen.

# AX_PROG_DOXYGEN
# ---------------
# Setup Doxygen and the "html" target.
AC_DEFUN([AX_PROG_DOXYGEN],
[dnl
  AC_ARG_VAR([DOXYGEN_PATH], [Path to the Doxygen executable.])
  AC_CHECK_PROGS([DOXYGEN_PATH], [doxygen], [:])
  AM_CONDITIONAL([HAVE_DOXYGEN], [test "x$DOXYGEN_PATH" != "x:"])
])dnl
