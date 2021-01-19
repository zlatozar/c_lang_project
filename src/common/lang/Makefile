BUILD_DIR = build

vpath %.c src
vpath %.o $(BUILD_DIR)/obj

CFLAGS  := -I include -std=c99 -pedantic-errors -Wall -Wextra \
           -Wshadow -Wmaybe-uninitialized -fno-common

LDFLAGS := -L $(BUILD_DIR)/lib
ARFLAGS := r

OUTPUT_OPTION = -MMD -MP -o $@

# Sources
lib_src := $(notdir $(wildcard src/*.c))

# ______________________________________________________________________________
#                                                                       Specify

LIB_NAME  = lang.a

lib_objs := $(foreach obj, $(lib_src:.c=.o), $(BUILD_DIR)/obj/$(obj))
lib_deps := $(foreach dep, $(lib_src:.c=.d), $(BUILD_DIR)/obj/$(dep))

MACROS_H  = -I../../macros/include

# ______________________________________________________________________________
#                                                                       Targets

.PHONY: dev
dev: CFLAGS += -g -Og -fno-inline -DDEBUG -fmax-errors=5 $(MACROS_H)
dev: all

all: $(BUILD_DIR)/lib/$(LIB_NAME)
	@echo -e "$(GREEN)>>> Library "\'$(LIB_NAME)\'" was build successfully!$(COLOR_END)\n"

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

debug: clean dev

release: CFLAGS += -O2 $(MACROS_H)
release: clean all

.PHONY: clean
clean:
	@rm -rvf $(BUILD_DIR)/lib/*.a
	@rm -rvf $(BUILD_DIR)/obj/*.o
	@rm -rvf $(BUILD_DIR)/obj/*.d
	@echo

.PHONY: dist-clean
dist-clean:
	rm -rf $(BUILD_DIR)/
	@echo

# ______________________________________________________________________________
#                                                                          Help

GREEN     := $(shell tput -Txterm setaf 2)
COLOR_END := $(shell tput -Txterm sgr0)

info:
	@echo
	@echo "Library Name: $(GREEN)${LIB_NAME}$(COLOR_END)"
	@echo
	@echo "[*] Sources:       ${lib_src}"
	@echo "[*] Objects:      ${lib_objs}"
	@echo "[*] Dependencies: ${lib_deps}"
	@echo

help:
	@echo "Usage:"
	@echo
	@echo "    Development:            make                 "
	@echo "    Releases:               make release         "
	@echo "    More serious problems:  make debug DEBUG=true"
	@echo
