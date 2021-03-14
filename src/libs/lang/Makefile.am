## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

# Common for sources and tests
LIB_HEADER = -I$(top_srcdir)/src/libs/lang/include

noinst_LTLIBRARIES = liblang.la

# The files to add to the library and to the source distribution
liblang_la_SOURCES = assert.c  \
										 \
										 except.c  \
										 \
										 mem-dev.c \
										 mem.c     \
										 \
										 atom.c    \
										 arena.c

liblang_la_CFLAGS = $(LIB_HEADER)

# The 'version info' is applicable only to shared libraries...
# liblang_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# ______________________________________________________________________________
#                                                                    Unit tests

GREATEST = -I$(top_srcdir)/src/ext-libs/greatest/include
CHECK_LDADD = liblang.la

# If you need to simulate memory fails.
# CHECK_LDFLAGS_EXTRA = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc

if MEMDEV_MODE
VALRGIND_SUPPRESSIONS = $(top_srcdir)/tools-setup/valgrind-mem-dev.supp
endif

if HAVE_CHECK

TESTS = $(check_PROGRAMS)
check_PROGRAMS = test/except_h.run \
								 test/except.run   \
								 test/assert_h.run \
								 test/assert.run   \
								 test/macros_h.run \
								 \
								 test/mem_h.run    \
								 test/mem_dev.run

test_except_h_run_SOURCES = test/except_h.c
test_except_h_run_CFLAGS = $(LIB_HEADER)

test_except_run_SOURCES = test/except.c
test_except_run_CFLAGS = $(LIB_HEADER) $(GREATEST)
test_except_run_LDADD = $(CHECK_LDADD)

test_assert_h_run_SOURCES = test/assert_h.c
test_assert_h_run_CFLAGS = $(LIB_HEADER)

test_assert_run_SOURCES = test/assert.c
test_assert_run_CFLAGS = $(LIB_HEADER) $(GREATEST)

test_macros_h_run_SOURCES = test/macros_h.c
test_macros_h_run_CFLAGS = $(LIB_HEADER) $(GREATEST)

test_mem_h_run_SOURCES = test/mem_h.c
test_mem_h_run_CFLAGS = $(LIB_HEADER)

test_mem_dev_run_SOURCES = test/mem-dev.c
test_mem_dev_run_CFLAGS = $(LIB_HEADER) $(GREATEST)
test_mem_dev_run_LDADD = $(CHECK_LDADD)

# 'test/<test name>.run.debug' target
include $(top_srcdir)/m4/gdb.mk

endif

# 'memcheck' target
include $(top_srcdir)/m4/valgrind.mk

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
