BUILD_DIR = build
LIB_NAME  = libmath.a

vpath %.c src
vpath %.o $(BUILD_DIR)/obj

CFLAGS  := -I include -std=c99 -pedantic-errors -Wall -Wextra -Werror
LDFLAGS := -L $(BUILD_DIR)/lib
ARFLAGS := r

OUTPUT_OPTION = -MMD -MP -o $@

# Static libraries
LDLIBS   :=

lib_src  := $(notdir $(wildcard src/*.c))
lib_objs := $(foreach obj, $(lib_src:.c=.o), $(BUILD_DIR)/obj/$(obj))
lib_deps := $(foreach dep, $(lib_src:.c=.d), $(BUILD_DIR)/obj/$(dep))

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

-include $(lib_deps)

debug: CFLAGS += -DDEBUG -g
debug: clean all

release: CFLAGS += -O2
release: clean all

.PHONY: clean
clean:
	@rm -rvf $(BUILD_DIR)/lib/*.a
	@rm -rvf $(BUILD_DIR)/obj/*.o
	@rm -rvf $(BUILD_DIR)/obj/*.d

.PHONY: dist-clean
dist-clean:
	rm -rf $(BUILD_DIR)/

info:
	@echo
	@echo "Library Name: ${LIB_NAME} "
	@echo
	@echo "[*] Sources:      ${lib_src} "
	@echo "[*] Objects:      ${lib_objs}"
	@echo "[*] Dependencies: ${lib_deps}"
