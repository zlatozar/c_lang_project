## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

# Common for sources and tests
LIB_HEADER = -I$(top_srcdir)/src/libs/data_structs/include

noinst_LTLIBRARIES = libdatastructs.la

# The files to add to the library and to the source distribution
libdatastructs_la_SOURCES = list.c      \
														array_stack.c

libdatastructs_la_CFLAGS = $(LIB_HEADER)                           \
													 -I$(top_srcdir)/src/libs/lang/include   \
													 -I$(top_srcdir)/src/libs/logger/include

libdatastructs_la_LIBADD = $(top_srcdir)/src/libs/lang/liblang.la     \
													 $(top_srcdir)/src/libs/logger/liblogger.la

# ______________________________________________________________________________
#                                                                    Unit tests

CHECK_CFLAGS = -I$(top_srcdir)/src/ext-libs/greatest/include \
							 -I$(top_srcdir)/src/libs/lang/include         \
							 $(LIB_HEADER)

CHECK_LDADD = $(top_srcdir)/src/libs/lang/liblang.la  \
							libdatastructs.la

if MEMDEV_MODE
VALRGIND_SUPPRESSIONS = $(top_srcdir)/tools-setup/valgrind-mem-dev.supp
endif

if HAVE_CHECK

TESTS = $(check_PROGRAMS)
check_PROGRAMS = test/list.run      \
								 test/array_stack.run

test_list_run_SOURCES = test/list.c
test_list_run_CFLAGS = $(CHECK_CFLAGS)
test_list_run_LDADD = $(CHECK_LDADD)

test_array_stack_run_SOURCES = test/array_stack.c
test_array_stack_run_CFLAGS = $(CHECK_CFLAGS)
test_array_stack_run_LDADD = $(CHECK_LDADD)

# 'test/<test name>.run.debug' target
include $(top_srcdir)/m4/gdb.mk

endif

# 'memcheck' target
include $(top_srcdir)/m4/valgrind.mk


# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
