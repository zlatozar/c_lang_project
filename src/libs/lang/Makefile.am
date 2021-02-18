## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/lang/include
# checked - conventions are safe
AM_CFLAGS += -Wno-conversion

noinst_LTLIBRARIES = liblang.la

# The files to add to the library and to the source distribution
liblang_la_SOURCES = assert.c  \
										 \
										 except.c  \
										 \
										 mem-dev.c \
										 mem.c
# The 'version info' is applicable only to shared libraries...
# liblang_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

# ______________________________________________________________________________
#                                                                    Unit tests

CHECK_CFLAGS = -I$(top_srcdir)/src/ext-libs/greatest/include
CHECK_LDADD = liblang.la

# If you need to mock something
# CHECK_LDFLAGS_EXTRA = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc

if HAVE_CHECK

TESTS = $(check_PROGRAMS)
check_PROGRAMS = test/assert.run

test_assert_run_SOURCES = test/assert.c
test_assert_run_CFLAGS = $(CHECK_CFLAGS)
test_assert_run_LDADD = $(CHECK_LDADD)

endif

# 'memcheck' target
include $(top_srcdir)/m4/valgrind.mk

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
