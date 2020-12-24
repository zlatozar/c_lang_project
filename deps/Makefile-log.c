BUILD_DIR = ../builds/log.c
LIB_NAME  = liblogc.a

# Used in .c includes directives
INCLUDE_DIR = $(BUILD_DIR)/include/deps/logging

vpath %.c src
vpath %.o $(BUILD_DIR)/obj

CFLAGS  := -I include -g -std=c99
LDFLAGS := -L $(BUILD_DIR)/lib
LDLIBS  :=
ARFLAGS := r

lib_h    := $(notdir $(wildcard src/*.h))
lib_src  := $(notdir $(wildcard src/*.c))
lib_objs := $(foreach obj, $(lib_src:.c=.o), $(BUILD_DIR)/obj/$(obj))
lib_deps := $(foreach dep, $(lib_src:.c=.d), $(BUILD_DIR)/dep/$(dep))

.PHONY: all
all: $(BUILD_DIR)/lib/$(LIB_NAME)
	@mkdir -p $(INCLUDE_DIR)
	cp src/$(lib_h) $(INCLUDE_DIR)

$(BUILD_DIR)/lib/liblogc.a: $(lib_objs)
	@mkdir -p $(@D)
	@$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/bin/%: %.o
	@mkdir -p $(@D)
	@$(LINK.o) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/obj/%.o: %.c
	@mkdir -p $(@D)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(BUILD_DIR)/dep/%.d: %.c
	@mkdir -p $(@D)
	@$(COMPILE.c) -MMD -MF $@ $<

-include $(lib_deps)

.PHONY: clean
clean:
	@-rm -rf $(BUILD_DIR)/
