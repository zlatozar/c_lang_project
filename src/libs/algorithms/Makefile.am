## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

noinst_LTLIBRARIES = libalgorithms.la

# The files to add to the library and to the source distribution
libalgorithms_la_SOURCES = minmax.c
libalgorithms_la_CFLAGS = -I$(top_srcdir)/src/libs/algorithms/include \
													-I$(top_srcdir)/src/libs/logger/include     \
													-I$(top_srcdir)/src/libs/lang/include

libalgorithms_la_LIBADD = $(top_srcdir)/src/libs/logger/liblogger.la \
													$(top_srcdir)/src/libs/lang/liblang.la

# ______________________________________________________________________________
#                                                                    Unit tests

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
