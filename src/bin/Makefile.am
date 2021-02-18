## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/lang/include  \
						-I$(top_srcdir)/src/libs/logger/include

# Safe casting
AM_CFLAGS += -Wconversion

bin_PROGRAMS = c-lang-project
c_lang_project_SOURCES = main.c

c_lang_project_LDADD = $(top_builddir)/src/libs/lang/liblang.la      \
											 $(top_builddir)/src/libs/logger/liblogger.la

# 'c-lang-project.debug' target
include $(top_srcdir)/m4/gdb.mk

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
