TOP  = ..
CC  ?= gcc

API_DIR = test/api
OUT     = out
DFLAGS  = -DCOGCHEF_HELPER

BUILD = $(MAKE) API_DIR=$(API_DIR) OUT=$(OUT) -C $(TOP)
EXES  = test
OUT_H = $(OUT).h

CFLAGS  += -Wall -Wextra -Wpedantic -I$(TOP) -g -std=c89
LDFLAGS += -L$(TOP)
LDLIBS  += -lcogchef

all: $(OUT_H) $(EXES)

$(OUT_H):
	@ $(BUILD) debug
#	@ $(BUILD) DFLAGS="$(DFLAGS)" debug

clean:
	@ rm -f $(EXES)
	@ $(BUILD) $@

.PHONY: clean
.NOTPARALLEL: all