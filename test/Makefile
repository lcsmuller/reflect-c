TOP  = ..
CC  ?= gcc

API_DIR = test/api
OUT     = out
DFLAGS  = -DCOGCHEF_HELPER

BUILD = $(MAKE) API_DIR=$(API_DIR) OUT=$(OUT) -C $(TOP)
EXES  = test
OUT_H = $(OUT).h

CFLAGS  += -Wall -Wextra -Wpedantic -I$(TOP) -g -std=c99
LDFLAGS += -L$(TOP)
LDLIBS  += -lcogchef

all: submodules $(OUT_H) $(EXES)

submodules:
	@ echo "Updating submodules"
	git submodule update --init --recursive

$(OUT_H):
	@ $(BUILD) debug

clean:
	@ rm -f $(EXES)
	@ $(BUILD) $@

purge: clean
	@ echo "Purging submodules"
	git submodule deinit --all -f

.PHONY: clean submodules
.NOTPARALLEL: all
