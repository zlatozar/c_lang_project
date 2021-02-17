## Process this file with automake to produce Makefile.in

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/lang/include

# Enable all warnings
AM_CFLAGS += -Wformat=2 -Wno-unused-parameter -Wshadow -Wundef           \
						 -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
						 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs   \
						 -Wjump-misses-init -Wlogical-op -Wmaybe-uninitialized       \
						 \
						 -Wno-conversion  # checked - conventions are safe

noinst_LIBRARIES = liblang.a

# The files to add to the library and to the source distribution
liblang_a_SOURCES = assert.c  \
										\
										except.c  \
										\
										mem-dev.c \
										mem.c

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump

# ______________________________________________________________________________
#                                                                    Unit tests

CHECK_CFLAGS = -g -O0 -DDEBUG -I$(top_srcdir)/src/ext-libs/greatest/include
CHECK_LDADD = -L..
# CHECK_LDFLAGS_EXTRA = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc

UNIT_TESTS = test/assert

check_PROGRAMS = test/assert
test_assert_CFLAGS = $(CHECK_CFLAGS)
test_assert_LDADD = $(CHECK_LDADD)
test_assert_SOURCES = test/assert.c

TESTS = $(UNIT_TESTS)

# ______________________________________________________________________________

TESTS_ENVIRONMENT_WITH_VALGRIND="libtool --mode=execute valgrind --leak-check=full"

.PHONY: valgrind-check
valgrind-check:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS=$(UNIT_TESTS) TESTS_ENVIRONMENT=$(TESTS_ENVIRONMENT_WITH_VALGRIND)
