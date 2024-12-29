TOP  = ..
CC  ?= gcc

API_DIR = test/api
OUT	    = reflect-c_GENERATED

OUT_O = $(OUT).o
EXES  = test

CFLAGS  += -Wall -Wextra -Wpedantic -I$(TOP) -g -std=c99
LDFLAGS += -L$(TOP)
LDLIBS  += -lreflectc

all: $(EXES)

$(EXES): $(OUT_O) submodules
	@ echo "Creating $@"
	$(CC) $(CFLAGS) $(TOP)/$< $@.c -o $@ $(LDFLAGS) $(LDLIBS)

$(OUT_O):
	@ $(MAKE) API_DIR=$(API_DIR) OUT=$(OUT) -C $(TOP) debug-gen

submodules:
	@ echo "Updating submodules"
	git submodule update --init --recursive

clean:
	@ rm -f $(EXES)
	@ $(MAKE) -C $(TOP) $@

purge: clean
	@ echo "Purging submodules"
	git submodule deinit --all -f
	@ $(MAKE) -C $(TOP) $@

.PHONY: all submodules clean purge
.NOTPARALLEL: all
