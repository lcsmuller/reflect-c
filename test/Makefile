TOP  = ..
CC  ?= gcc

API_DIR = test/api
OUT	    = reflect-c_GENERATED

OUT_C = $(TOP)/$(OUT).c
OUT_H = $(TOP)/$(OUT).h
OUT_O = $(TOP)/$(OUT).o
EXES  = test

CFLAGS  += -Wall -Wextra -Wpedantic -I$(TOP) -g -std=c99
LDFLAGS += -L$(TOP)
LDLIBS  += -lreflectc

all: $(EXES)

$(EXES): $(OUT_O) submodules
	@ echo "Creating $@"
	$(CC) $(CFLAGS) $< $@.c -o $@ $(LDFLAGS) $(LDLIBS)

$(OUT_O): $(OUT_C)
	@ echo "Compiling generated source for tests"
	$(CC) $(CFLAGS) -c $(OUT_C) -o $@

$(OUT_C) $(OUT_H):
	@ $(MAKE) API_DIR=$(API_DIR) OUT=$(OUT) -C $(TOP) debug-gen

submodules:
	@ echo "Updating submodules"
	git submodule update --init --recursive

clean:
	@ rm -f $(EXES) $(OUT_O)
	@ $(MAKE) -C $(TOP) $@

purge: clean
	@ echo "Purging submodules"
	git submodule deinit --all -f
	@ $(MAKE) -C $(TOP) $@

.PHONY: all submodules clean purge
.NOTPARALLEL: all
