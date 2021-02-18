# gdb.m4 - Check for GDB.

# AX_PROG_GDB
# -----------
# Setup GDB and the "%.debug" target.
AC_DEFUN([AX_PROG_GDB],
[dnl
  AC_ARG_VAR([GDB_PATH], [Path to the GDB executable.])
  AC_ARG_VAR([GDB_FLAGS], [Flags for GDB.])
  AC_ARG_VAR([GDB_ENVIRONMENT],
       [Environment variables for GDB.])
  AC_CHECK_PROGS([GDB_PATH], [gdb], [:])
  AM_CONDITIONAL([HAVE_GDB], [test "x$GDB_PATH" != "x:"])
])dnl
