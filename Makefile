#-------------------------------------------------------------------------
#  Copyright 2021, Zlatozar Zhelyazkov
#-------------------------------------------------------------------------

.PHONY : clean dist init tests main

ifndef $(VARIANT)
VARIANT=debug
endif

OUTDIR     = bin/$(VARIANT)
INCLUDES   = -Iinclude

ifeq ($(VARIANT),release)
CCFLAGS    = $(CCFLAGSOPT) -DNDEBUG $(INCLUDES)
CXXFLAGS   = $(CXXFLAGSOPT) -DNDEBUG $(INCLUDES)
else ifeq ($(VARIANT),testopt)
CCFLAGS    = $(CCFLAGSOPT) $(INCLUDES)
CXXFLAGS   = $(CXXFLAGSOPT) $(INCLUDES)
else ifeq ($(VARIANT),debug)
CCFLAGS    = $(CCFLAGSDEBUG) $(INCLUDES)
CXXFLAGS   = $(CXXFLAGSDEBUG) $(INCLUDES)
else
VARIANTUNKNOWN=1
endif

# Use VALGRIND=1 to memory check under valgrind
ifeq ($(VALGRIND),1)
VALGRINDX=yes
else ifeq ($(VALGRIND),yes)
VALGRINDX=yes
else
VALGRINDX=
endif

ifeq ($(VALGRINDX),yes)
VALGRINDX=valgrind --leak-check=full --show-leak-kinds=all --suppressions=./valgrind.supp
endif

# -------------------------------------
# Sources
# -------------------------------------

SRCFILES =

CTESTS   =
TESTFILES= # main-tests.c	$(CTESTS)

# -------------------------------------
# Main targets
# -------------------------------------

main: init

tests: init staticlib testmain
	@echo ""
	@echo "run tests"

bench: init staticlib benchmain
	@echo ""
	@echo "run benchmark"

# -------------------------------------
# Build tests
# -------------------------------------


# -------------------------------------
# Build benchmark
# -------------------------------------


# -------------------------------------
# Build the static library
# -------------------------------------


# -------------------------------------
# other targets
# -------------------------------------

sync_submodules:
	@git submodule init
	@git submodule update

docs:

clean:
	rm -rf $(CONFIGDIR)/*/*

init:
	@echo "use 'make help' for help"
	@echo "build variant: $(VARIANT), configuration: $(CONFIG)"

help:
	@echo "Usage: make <target>"
	@echo "Or   : make VARIANT=<variant> <target>"
	@echo "Or   : make VALGRIND=1 tests"
	@echo ""
	@echo "Variants:"
	@echo "  debug       : Build a debug version (default)"
	@echo "  testopt     : Build an optimized version but with assertions"
	@echo "  release     : Build an optimized release version"
	@echo ""
	@echo "Targets:"
	@echo "  main        : Build a static library (default)"
	@echo "  tests       : Run tests"
	@echo "  bench       : Run benchmarks, use 'VARIANT=release'"
	@echo "  clean       : Clean output directory"
	@echo "  depend      : Generate dependencies"
	@echo ""
	@echo "Configuration:"
	@echo "  output dir  : $(OUTDIR)"
	@echo "  c-compiler  : $(CC) $(CCFLAGS)"
	@echo ""
