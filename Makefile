# User modifiable variables
EXTEND  = reflect-c_MAIN_INCLUDES.PRE.h
API_DIR = api
OUT     = out
DFLAGS  = -DREFLECTC_HELPER -DREFLECTC_STRUCT_INIT

# Remove suffix from EXTEND
EXTEND_NO_EXT_EXPAND = "$$(echo $(EXTEND) | sed -e 's/\.PRE\.h//')"
# Convert %.PRE.h -> %.h
HEADERS_EXPAND       = "$$(echo $(API_DIR)/*.PRE.h | sed -e 's/\.PRE\.h/.h/g')"
# Join all API_DIR files together
TEMPFILE        = reflect-c_AMALGAMATION.PRE.h
TEMPFILE_EXPAND = headers=$$(echo $(API_DIR)/*.PRE.h); for header in $$headers; do echo "\#include \"$$header\"" >> $(TEMPFILE); done

BUILD = $(MAKE) EXTEND_NO_EXT=$(EXTEND_NO_EXT_EXPAND) OUT_NO_EXT=$(OUT) DFLAGS="$(DFLAGS)" -f reflect-c.mk

all:
	@ touch $(TEMPFILE)  && $(TEMPFILE_EXPAND)
	$(BUILD) TEMPFILE=$(TEMPFILE) || rm -f $(TEMPFILE)
	@ rm -f $(TEMPFILE)

debug:
	@ echo "Building on debug mode"
	CFLAGS="-g" $(MAKE)

headers:
	$(BUILD) HEADERS=$(HEADERS_EXPAND) $@

echo:
	@ echo 'EXTEND: $(EXTEND)'
	@ echo 'API_DIR: $(API_DIR)'
	$(BUILD) HEADERS=$(HEADERS_EXPAND) $@

clean:
	@ rm -f $(TEMPFILE)
	$(MAKE) -f reflect-c.mk $@

purge: clean
	$(MAKE) -f reflect-c.mk $@

.PHONY: headers echo clean
