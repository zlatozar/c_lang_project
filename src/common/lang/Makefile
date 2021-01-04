BUILD_DIR = build

vpath %.c src
vpath %.o $(BUILD_DIR)/obj

CFLAGS  := -I include -std=c99 -pedantic-errors -Wall -Wextra -Werror
LDFLAGS := -L $(BUILD_DIR)/lib
ARFLAGS := r

OUTPUT_OPTION = -MMD -MP -o $@

# Sources
SRC := $(notdir $(wildcard src/*.c))

# ______________________________________________________________________________
#                                                                       Specify

ifeq ($(DEBUG),true)

LIB_NAME  = lang-dev.a
lib_src  := $(filter-out mem.c, $(SRC))

else
LIB_NAME  = lang.a
lib_src  := $(filter-out mem-dev.c, $(SRC))
endif

lib_objs := $(foreach obj, $(lib_src:.c=.o), $(BUILD_DIR)/obj/$(obj))
lib_deps := $(foreach dep, $(lib_src:.c=.d), $(BUILD_DIR)/obj/$(dep))

# ______________________________________________________________________________
#                                                                       Targets

.PHONY: dev
dev: CFLAGS += -DDEBUG -g
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

release: CFLAGS += -O2
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
