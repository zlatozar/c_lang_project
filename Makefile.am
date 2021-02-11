## Process this file with automake to produce Makefile.in

SUBDIRS = src/libs \
	  src/bin

MAINTAINERCLEANFILES = configure config.in aclocal.m4        \
build-aux/ar-lib build-aux/config.guess build-aux/depcomp    \
build-aux/ltmain.sh build-aux/test-driver build-aux/compile  \
build-aux/config.sub build-aux/missing build-aux/install-sh  \
Makefile.in

MOSTLYCLEANFILES = *.stackdump

# --------------------------------------
# other targets
# --------------------------------------

sync_submodules:
	@git submodule init
	@git submodule update
