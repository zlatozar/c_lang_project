## Project Root
DEPS_ROOT = ../../../../deps

# Should contains 'unity.h' and 'unity_internals.h'
UNITY_H   = $(DEPS_ROOT)/builds/unity/include
UNITY_LIB = $(DEPS_ROOT)/builds/unity/lib/libunity.a

BUILD     = build/
BUILD_OBJ = build/objs/
BUILD_RES = build/results/

src         = ../src/
src_headers = ../include

test_src     = $(wildcard *.c)
test_results = $(patsubst test_%.c,$(BUILD_RES)test_%.tmp,$(test_src))

CFLAGS  = -g -std=c99 -I $(src_headers)

PASSED  = `grep -s PASS $(BUILD_RES)*.tmp`
FAILED  = `grep -s FAIL $(BUILD_RES)*.tmp`
IGNORED = `grep -s IGNORE $(BUILD_RES)*.tmp`

.PHONY: test
test: $(BUILD) $(BUILD_OBJ) $(BUILD_RES) $(test_results)
	@echo -e "\nFAILED:\n--------------------------------------------------"
	@echo "$(FAILED)"
	@echo -e "\nPassed:\n--------------------------------------------------"
	@echo "$(PASSED)"
	@echo -e "\nDONE"

$(BUILD_RES)%.tmp: $(BUILD)%.run
	@-./$< > $@ 2>&1

$(BUILD)test_%.run: $(BUILD_OBJ)test_%.o $(BUILD_OBJ)%.o
	$(LINK.o) -o $@ $^ $(UNITY_LIB)

# src
$(BUILD_OBJ)%.o:: $(src)%.c
	$(COMPILE.c) -DTEST $< -o $@

# test
$(BUILD_OBJ)%.o:: %.c
	$(COMPILE.c) -I $(UNITY_H) $< -o $@

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD_OBJ):
	@mkdir -p $(BUILD_OBJ)

$(BUILD_RES):
	@mkdir -p $(BUILD_RES)

.PHONY: clean
clean:
	@rm -f $(BUILD_OBJ)*.o
	@rm -f $(BUILD)*.run
	@rm -f $(BUILD_RES)*.tmp

.PHONY: clean-all
clean-all:
	@rm -rf $(BUILD)

.PRECIOUS: $(BUILD_OBJ)%.o
.PRECIOUS: $(BUILD)test_%.run
.PRECIOUS: $(BUILD_RES)%.tmp
