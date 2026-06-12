OBJS = reflect-c.o
CC   = cc

# Resulting library
LIB_NO_EXT = libreflectc
ARLIB = $(LIB_NO_EXT).a

ARFLAGS = -cqsv

CFLAGS   ?= -O2
CFLAGS   += -I. -Wall -Wextra -Wpedantic -std=c89

API_DIR = api
OUT     = reflect-c_GENERATED
DFLAGS  = -DREFLECTC_WRAPPER

# Convert %.recipe.h -> %.h
HEADERS_EXPAND       = "$$(echo $(API_DIR)/*.recipe.h | sed -e 's/\.recipe\.h/.h/g')"
# Join all API_DIR files together (LC_ALL=C sort: glob order is
# locale/shell dependent, normalize it so output is reproducible)
TEMPFILE        = reflect-c_GENERATED.recipe.h
TEMPFILE_EXPAND = headers=$$(echo $(API_DIR)/*.recipe.h | tr ' ' '\n' | LC_ALL=C sort); for header in $$headers; do printf '\#include "%s"\n' "$$header" >> $(TEMPFILE); done
# Build reflect-c.mk with the correct variables
BUILD = $(MAKE) OUT_NO_EXT=$(OUT) DFLAGS="$(DFLAGS)" -f reflect-c.mk

REFLECTC_TUPLE_MAX = 16
TOOLS_DIR          = tools

all: $(ARLIB)

$(ARLIB): $(OBJS)
	@ echo "Creating $@"
	$(AR) $(ARFLAGS) $@ $^

gen: all
	@ touch $(TEMPFILE)  && $(TEMPFILE_EXPAND)
	$(BUILD) TEMPFILE=$(TEMPFILE) || rm -f $(TEMPFILE)
	@ rm -f $(TEMPFILE)

debug:
	@ echo "Building on debug mode"
	CFLAGS="-g" $(MAKE) 

debug-gen:
	@ echo "Building on debug mode"
	CFLAGS="-g" $(MAKE) gen

headers:
	$(BUILD) HEADERS=$(HEADERS_EXPAND) $@

echo:
	@ echo 'API_DIR: $(API_DIR)'
	$(BUILD) HEADERS=$(HEADERS_EXPAND) $@

# host-run generator tool: never build with target CFLAGS
HOSTCC ?= cc
$(TOOLS_DIR)/gen_tuples: $(TOOLS_DIR)/gen_tuples.c
	$(HOSTCC) -O2 $< -o $@

tuples: $(TOOLS_DIR)/gen_tuples
	@echo "Generating reflect-c_TUPLE.h with MAX=$(REFLECTC_TUPLE_MAX)"
	@./$(TOOLS_DIR)/gen_tuples $(REFLECTC_TUPLE_MAX) > reflect-c_TUPLE.h

clean:
	@ rm -f $(TEMPFILE) $(OBJS) $(ARLIB)
	$(BUILD) $@

purge: clean

.PHONY: all gen debug debug-gen headers echo clean purge tuples
