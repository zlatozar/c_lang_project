## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src/libs \
					src/bin

release: dist
	$(AM_V_GEN)sha256sum $(DIST_ARCHIVES) > $(PACKAGE)-$(VERSION).sha256sum

# 'html' target
DOXYGEN_DOXYFILE = $(top_srcdir)/tools-setup/Doxyfile
include $(top_srcdir)/m4/doxygen.mk

# ______________________________________________________________________________
#                                                                         Other

format:
	astyle --recursive "src/libs/*.c,*h" "src/bin/*.c,*h" --verbose --options="tools-setup/astylerc"

sync_submodules:
	@git submodule init
	@git submodule update

# ______________________________________________________________________________

MAINTAINERCLEANFILES = configure config.in aclocal.m4        \
build-aux/ar-lib build-aux/config.guess build-aux/depcomp    \
build-aux/ltmain.sh build-aux/test-driver build-aux/compile  \
build-aux/config.sub build-aux/missing build-aux/install-sh  \
Makefile.in

MOSTLYCLEANFILES = *.stackdump
