## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = polymorphic

polymorphic_SOURCES = first_impl.c  \
											second_impl.c \
											main.c

polymorphic_CFLAGS = -I$(top_srcdir)/src/bin/paradigms/polymorphic/include \
										 -I$(top_srcdir)/src/libs/logger/include               \
										 -I$(top_srcdir)/src/libs/lang/include

polymorphic_LDADD = $(top_builddir)/src/libs/lang/liblang.la   \
										$(top_builddir)/src/libs/logger/liblogger.la

# 'polymorphic.debug' target
include $(top_srcdir)/m4/gdb.mk

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
