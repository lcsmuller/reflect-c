TOP  = ..
CC  ?= gcc

API_DIR = test/api
OUT	    = reflect-c_GENERATED

OUT_C = $(TOP)/$(OUT).c
OUT_H = $(TOP)/$(OUT).h
OUT_O = $(TOP)/$(OUT).o
EXES  = runtime
CHECKS = compile
CHECKS_CFLAGS = -DREFLECTC_PREFIX=rc_compile -DREFLECTC_PREFIX_UPPER=RC_COMPILE

CFLAGS  += -Wall -Wextra -Wpedantic -I$(TOP) -I$(TOP)/test -g -std=c99
LDFLAGS += -L$(TOP)
LDLIBS  += -lreflectc

all: $(EXES) $(CHECKS)

$(EXES): $(OUT_O) submodules
	@ echo "Creating $@"
	$(CC) $(CFLAGS) $< $@.c -o $@ $(LDFLAGS) $(LDLIBS)

$(OUT_O): $(OUT_C)
	@ echo "Compiling generated source for tests"
	$(CC) $(CFLAGS) -c $(OUT_C) -o $@

$(OUT_C) $(OUT_H):
	@ $(MAKE) API_DIR=$(API_DIR) OUT=$(OUT) -C $(TOP) debug-gen

$(CHECKS): $(OUT_H)
	@ echo "Checking compile-time checks with custom prefix"
	$(CC) $(CFLAGS) $(CHECKS_CFLAGS) -fsyntax-only compile.c

submodules:
	@ echo "Updating submodules"
	git submodule update --init --recursive

clean:
	@ rm -f $(EXES) $(OUT_O) $(CHECKS)
	@ $(MAKE) -C $(TOP) $@

purge: clean
	@ echo "Purging submodules"
	git submodule deinit --all -f
	@ $(MAKE) -C $(TOP) $@

.PHONY: all submodules clean purge $(CHECKS)
.NOTPARALLEL: all
