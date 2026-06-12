# Expect the following to be pre-defined when invoking this Makefile:
# - OUT_NO_EXT: Path to the output file without extension
# - HEADERS: List of header files to be generated
# - TEMPFILE: Temporary file to be used for header generation

# executable that expands comments
# example: /*#! #define HELLO 1 */  becomes  #define HELLO 1
EXPAND_COMMENTS = reflect-c_EXPAND_COMMENTS

# The comment expander runs on the HOST at generation time: build it with
# host flags, never the target CFLAGS (cross-compiling or sanitizer flags
# would otherwise break or poison the generator)
HOSTCC      ?= cc
HOSTCFLAGS  ?= -O2 -I.

RECIPES               = reflect-c_RECIPES.recipe.h
RECIPES_NO_EXT_EXPAND = "$$(echo $(RECIPES) | sed -e 's/\.recipe\.h//')"

# Resulting single-file amalgamations after preprocessing input file
OUT_C          = $(OUT_NO_EXT).c
OUT_H          = $(OUT_NO_EXT).h
OUT_H_BASENAME = "$$(basename $(OUT_H))"

CFLAGS   ?= -O2
CFLAGS   += -I. -Wall -Wextra -Wpedantic -std=c89
CPP       = cc -E # If you run into issues, try changing this to 'cpp'
CPPFLAGS += -nostdinc -P -CC

# Convert 'foo/bar_baz-tuna.recipe.h' -> 'FOO_BAR_BAZ_TUNA_H'
# Map every non-alphanumeric to '_' (paths may carry '.', '/', '-', etc.),
# then strip leading '_' so '../' prefixes can't yield reserved identifiers
HEADER_TAG_EXPAND   = "$$(echo '$<' | sed -e 's/\.recipe\.h$$//' -e 's/[^[:alnum:]]/_/g' -e 's/^_*//' | tr '[:lower:]' '[:upper:]')_H"
# Doxygen file description (printf format string; file name passed via %s)
DOXYGEN_DESC_FMT = '/**\n * @file %s\n * @author Reflect-C\n * @brief Reflect-C generated code\n */\n'

all: $(OUT_C) $(OUT_H)

$(EXPAND_COMMENTS):
	$(HOSTCC) $(HOSTCFLAGS) $@.c -o $@

$(OUT_H): $(TEMPFILE) $(RECIPES) $(EXPAND_COMMENTS)
	@ echo "Generating $@"
	@ printf '#ifndef %s\n' $(HEADER_TAG_EXPAND) > $@
	@ printf '#define %s\n\n' $(HEADER_TAG_EXPAND) >> $@
	$(CPP) $(CFLAGS) -DREFLECTC_DEFINITIONS -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_HEADER -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	@ printf '\n#endif /* %s */\n' $(HEADER_TAG_EXPAND) >> $@
$(OUT_C): $(TEMPFILE) $(RECIPES) $(EXPAND_COMMENTS)
	@ echo "Adding forward definitions to $@"
	@ printf '#include "%s"\n' $(OUT_H_BASENAME) > $@
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_FORWARD -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	@ echo "Generating $@"	
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@

headers: $(HEADERS)

$(HEADERS): $(RECIPES) $(EXPAND_COMMENTS)

.SUFFIXES: .recipe.h .h
.recipe.h.h:
	@ echo "Generating $@"
	@ printf $(DOXYGEN_DESC_FMT) "$@" > $@
	@ printf '#ifndef %s\n' $(HEADER_TAG_EXPAND) >> $@
	@ printf '#define %s\n\n' $(HEADER_TAG_EXPAND) >> $@
	$(CPP) $(CFLAGS) -DREFLECTC_DEFINITIONS -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_HEADER -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	@ printf '\n#endif /* %s */\n' $(HEADER_TAG_EXPAND) >> $@

echo:
	@ echo 'RECIPES_NO_EXT: $(RECIPES_NO_EXT)'
	@ echo 'OUT_H: $(OUT_H)'
	@ echo 'OUT_C: $(OUT_C)'
	@ echo 'HEADERS: $(HEADERS)'

clean:
	@ rm -f $(OUT_H) $(OUT_C)
	@ rm -f $(HEADERS) $(EXPAND_COMMENTS)

.PHONY: headers clean
