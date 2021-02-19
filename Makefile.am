## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src/libs \
					src/bin

format:
	astyle --recursive "src/libs/*.c,*h" "src/bin/*.c,*h" --verbose --options="tools-setup/astylerc"

# `cppcheck --help` for more options
cppcheck:
	@cppcheck --language=c --std=c11 --enable=all --quiet                    \
						--suppressions-list="$(top_srcdir)/tools-setup/cppcheck.supp"  \
						$(top_srcdir)/src/libs $(top_srcdir)/src/bin                   \
						--error-exitcode=1

# Global 'memcheck' target
memcheck:
	@echo "Start analyzing code with Valgrind..."

# 'html' target
DOXYGEN_DOXYFILE = $(top_srcdir)/tools-setup/Doxyfile
include $(top_srcdir)/m4/doxygen.mk

release: dist
	$(AM_V_GEN)sha256sum $(DIST_ARCHIVES) > $(PACKAGE)-$(VERSION).sha256sum

# ______________________________________________________________________________
#                                                                         Other

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

help:
	@echo
	@echo -e "Usage:\n  make <target>"
	@echo
	@echo "(default) all ............. Build project"
	@echo "          sync_submodules . Update `git` submodules"
	@echo "          check ........... Run all unit tests"
	@echo "          cppcheck ........ Statically checks all .c files"
	@echo "          memcheck ........ Run unit test using Valgirnd to check memory leaks"
	@echo "          format .......... Format code using `astyle`"
	@echo "          html ............ Generate API documentation in `Docs/api/html`"
	@echo "          release ......... Build projcect and create distribution as `tar.gz`"
	@echo
