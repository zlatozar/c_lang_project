## Process this file with automake to produce Makefile.in

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/lang/include

# Enable all warnings
AM_CFLAGS += -Wformat=2 -Wno-unused-parameter -Wshadow -Wundef           \
						 -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
						 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs   \
						 -Wjump-misses-init -Wlogical-op

noinst_LIBRARIES = liblang.a

# The files to add to the library and to the source distribution
liblang_a_SOURCES = assert.c  \
										\
										except.c  \
										\
										mem-dev.c \
										mem.c

AM_LDFLAGS = -version-info @LIBVERSION@

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
