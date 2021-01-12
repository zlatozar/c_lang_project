## Project Root
DEPS_ROOT = ../../../../deps

# Should contains 'unity.h' and 'unity_internals.h'
UNITY_NAME = libunity.a
UNITY_H    = $(DEPS_ROOT)/builds/unity/include
UNITY_LIB  = $(DEPS_ROOT)/builds/unity/lib

LIB_DIR = ../build/lib

CFLAGS  = -std=c99 -pedantic-errors -Wall -Wextra -Werror -DDEBUG

# Form names of the executable
BUILD_DIR = bin

test_src  := $(wildcard test_*.c)
test_runs := $(notdir $(patsubst test_%.c,$(BUILD_DIR)/test_%.run,$(test_src)))

# ______________________________________________________________________________
#                                                                       Specify

LIB_NAME = data-structs.a
LIB_H    = -I ../include
LIB      = -L $(LIB_DIR) -l:$(LIB_NAME)

MACROS_H   = -I../../../macros/include

DEP_LIBS_H = -I../../lang/include
DEPS       = -L ../../lang/build/lib -l:lang.a

UNITY    = -L $(UNITY_LIB) -l:$(UNITY_NAME)

# ______________________________________________________________________________
#                                                                       Targets

.PHONY: test
test: build_src run

run: $(BUILD_DIR)/$(test_runs)
	@-./$<

build_src:
	@echo "Build project first"
	@cd .. && $(MAKE)

$(BUILD_DIR)/%.run: %.c
	@mkdir -p $(@D)
	$(LINK.c) $< $(LIB_H) $(MACROS_H) $(DEP_LIBS_H) -I$(UNITY_H) $(LIB) $(DEPS) $(UNITY) -o $@

.PHONY: clean
clean:
	@rm -f $(BUILD_DIR)/*.run

.PHONY: clean-all
clean-all:
	@rm -rf $(BUILD_DIR)

.PRECIOUS: $(BUILD_DIR)/%.run
