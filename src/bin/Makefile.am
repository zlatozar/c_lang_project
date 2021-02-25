## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = c-lang-project
c_lang_project_SOURCES = main.c

c_lang_project_CFLAGS = -I$(top_srcdir)/src/libs/logger/include          \
												-I$(top_srcdir)/src/libs/lang/include            \
												-I$(top_builddir)/src/libs/data_structs/include  \
												-I$(top_builddir)/src/libs/algorithms/include

c_lang_project_LDADD = $(top_builddir)/src/libs/lang/liblang.la                 \
											 $(top_builddir)/src/libs/logger/liblogger.la             \
											 $(top_builddir)/src/libs/data_structs/libdatastructs.la  \
											 $(top_builddir)/src/libs/algorithms/libalgorithms.la

# 'c-lang-project.debug' target
include $(top_srcdir)/m4/gdb.mk

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
