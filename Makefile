#-------------------------------------------------------------------------
#  Copyright 2021, Zlatozar Zhelyazkov
#-------------------------------------------------------------------------

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

# -------------------------------------
# Main targets
# -------------------------------------

# Top level Makefile, the real is src/Makefile

default: all

# Preserve order
.DEFAULT:
	cd src/common/lang && $(MAKE) $@
	cd src/common/data_structs && $(MAKE) $@

# -------------------------------------
# Build the static library
# -------------------------------------


# -------------------------------------
# other targets
# -------------------------------------

sync_submodules:
	@git submodule init
	@git submodule update
