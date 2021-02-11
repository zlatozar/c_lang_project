## Process this file with automake to produce Makefile.in

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/lang/include

# Enable all warnings
AM_CFLAGS += -Wformat=2 -Wno-unused-parameter -Wshadow -Wundef           \
						 -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
						 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs   \
						 -Wjump-misses-init -Wlogical-op

bin_PROGRAMS = c-lang-project
c_lang_project_SOURCES = main.c

c_lang_project_LDADD = $(top_builddir)/src/libs/lang/liblang.a $(AM_LDFLAGS)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
