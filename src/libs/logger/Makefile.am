## Process this file with automake to produce Makefile.in

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/logger/include

# Enable all warnings
AM_CFLAGS += -Wformat=2 -Wno-unused-parameter -Wshadow -Wundef           \
						 -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
						 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs   \
						 -Wjump-misses-init -Wlogical-op -Wmaybe-uninitialized       \
						 -Wconversion

noinst_LIBRARIES = liblogger.a
liblogger_a_SOURCES = log.c

AM_LDFLAGS = -version-info @LIBVERSION@

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
