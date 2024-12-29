SUBMODULES_DIR  = submodules
OA_HASH_DIR	    = $(SUBMODULES_DIR)/oa-hash

# Dependencies
OBJS_SUBMODULES = $(OA_HASH_DIR)/oa_hash.o
OBJS_REFLECTC   = reflect-c.o
OBJS            = $(OBJS_REFLECTC) $(OBJS_SUBMODULES)

# Resulting library
LIB_NO_EXT = libreflectc
ARLIB = $(LIB_NO_EXT).a

ARFLAGS = -cqsv

CFLAGS   ?= -O2
CFLAGS   += -I. -Wall -Wextra -Wpedantic -std=c89

API_DIR = api
OUT     = reflect-c_GENERATED
DFLAGS  = -DREFLECTC_HELPER -DREFLECTC_STRUCT_INIT

# Convert %.PRE.h -> %.h
HEADERS_EXPAND       = "$$(echo $(API_DIR)/*.PRE.h | sed -e 's/\.PRE\.h/.h/g')"
# Join all API_DIR files together
TEMPFILE        = reflect-c_GENERATED.PRE.h
TEMPFILE_EXPAND = headers=$$(echo $(API_DIR)/*.PRE.h); for header in $$headers; do echo "\#include \"$$header\"" >> $(TEMPFILE); done
# Build reflect-c.mk with the correct variables
BUILD = $(MAKE) OUT_NO_EXT=$(OUT) DFLAGS="$(DFLAGS)" -f reflect-c.mk

all: $(ARLIB)

$(ARLIB): $(OBJS)
	@ echo "Creating $@"
	$(AR) $(ARFLAGS) $@ $^

$(OBJS_REFLECTC): $(OBJS_SUBMODULES)

$(OBJS_SUBMODULES):
	@ echo "Building $@"
	git submodule update --init --recursive
	$(MAKE) -C $(OA_HASH_DIR)

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

clean:
	@ rm -f $(TEMPFILE) $(OBJS) $(ARLIB)
	$(BUILD) $@

purge: clean
	@ echo "Purging submodules"
	git submodule deinit --all -f

.PHONY: all gen debug debug-gen headers echo clean purge
