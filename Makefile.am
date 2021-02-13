## Process this file with automake to produce Makefile.in

SUBDIRS = src/libs \
	  src/bin

# --------------------------------------
# other targets
# --------------------------------------

format:
	astyle --recursive "src/libs/*.c,*h" "src/bin/*.c,*h" --verbose --options="tools-setup/astylerc"

sync_submodules:
	@git submodule init
	@git submodule update

MAINTAINERCLEANFILES = configure config.in aclocal.m4        \
build-aux/ar-lib build-aux/config.guess build-aux/depcomp    \
build-aux/ltmain.sh build-aux/test-driver build-aux/compile  \
build-aux/config.sub build-aux/missing build-aux/install-sh  \
Makefile.in

MOSTLYCLEANFILES = *.stackdump
