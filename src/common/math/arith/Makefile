BUILD_DIR = build
LIB_NAME  = libarith.a

vpath %.c src
vpath %.o $(BUILD_DIR)/obj

CFLAGS  := -I include -g -std=c99
LDFLAGS := -L $(BUILD_DIR)/lib
LDLIBS  :=
ARFLAGS := r

lib_src  := $(notdir $(wildcard src/*.c))
lib_objs := $(foreach obj, $(lib_src:.c=.o), $(BUILD_DIR)/obj/$(obj))
lib_deps := $(foreach dep, $(lib_src:.c=.d), $(BUILD_DIR)/dep/$(dep))

.PHONY: all
all: $(BUILD_DIR)/lib/$(LIB_NAME)

$(BUILD_DIR)/lib/$(LIB_NAME): $(lib_objs)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(BUILD_DIR)/bin/%: %.o
	@mkdir -p $(@D)
	$(LINK.o) $^ $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/obj/%.o: %.c
	@mkdir -p $(@D)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(BUILD_DIR)/dep/%.d: %.c
	@mkdir -p $(@D)
	@$(COMPILE.c) -MM -MF $@ $<
	@sed -i '1s|^|obj/|' $@

-include $(lib_deps)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/
