## Project Root
PROJ_ROOT = ../../../..
DEPS_ROOT = $(PROJ_ROOT)/deps

# Should contains 'unity.h' and 'unity_internals.h'
UNITY_NAME = libunity.a
UNITY_H    = $(DEPS_ROOT)/builds/unity/include
UNITY_LIB  = $(DEPS_ROOT)/builds/unity/lib

LIB_DIR = ../build/lib

CFLAGS   = -g -Og -std=c99 -pedantic-errors -Wall -Wextra -Werror -DDEBUG
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --quiet --error-exitcode=1 \
					 --suppressions=$(PROJ_ROOT)/valgrind-test.supp

# Form names of the executable
BUILD_DIR = bin

# ______________________________________________________________________________
#                                                                       Specify

LIB_NAME = data-structs.a
LIB_H    = -I ../include
LIB      = -L $(LIB_DIR) -l:$(LIB_NAME)

MACROS_H   = -I../../../macros/include

DEP_LIBS_H = -I../../lang/include
DEPS       = -L ../../lang/build/lib -l:lang.a

UNITY    = -L $(UNITY_LIB) -l:$(UNITY_NAME)

BUILD_NEEDED = $(LIB_H) $(MACROS_H) $(DEP_LIBS_H) -I$(UNITY_H) $(LIB) $(DEPS) $(UNITY)

# ______________________________________________________________________________
#                                                                       Targets

.PHONY: test
test: build_src test_runners

build_src:
	@echo "Build library first"
	@cd .. && $(MAKE)

test_runners: test_list.run test_stack.run

test_list.run:
	@mkdir -p $(BUILD_DIR)/$(@D)
	$(LINK.c) test_list.c $(BUILD_NEEDED) -o $(BUILD_DIR)/$@
	$(VALGRIND) ./$(BUILD_DIR)/$@

test_stack.run:
	@mkdir -p $(BUILD_DIR)/$(@D)
	$(LINK.c) test_stack.c $(BUILD_NEEDED) -o $(BUILD_DIR)/$@
	$(VALGRIND) ./$(BUILD_DIR)/$@

.PHONY: clean
clean:
	@rm -f $(BUILD_DIR)/*.run

.PHONY: clean-all
clean-all:
	@rm -rf $(BUILD_DIR)

.PRECIOUS: $(BUILD_DIR)/%.run
