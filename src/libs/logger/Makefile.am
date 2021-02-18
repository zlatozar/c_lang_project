## Process this file with automake to produce Makefile.in

AM_CFLAGS = -std=c11 -I$(top_srcdir)/src/libs/logger/include
AM_CFLAGS += -Wconversion

noinst_LTLIBRARIES = liblogger.la
liblogger_la_SOURCES = log.c

# ______________________________________________________________________________

MAINTAINERCLEANFILES = Makefile.in
