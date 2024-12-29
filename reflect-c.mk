# Expect the following to be pre-defined when invoking this Makefile:
# - OUT_NO_EXT: Path to the output file without extension
# - HEADERS: List of header files to be generated
# - TEMPFILE: Temporary file to be used for header generation

# executable that expands comments
# example: /*#! #define HELLO 1 */  becomes  #define HELLO 1
EXPAND_COMMENTS = reflect-c_EXPAND_COMMENTS

RECIPES               = reflect-c_RECIPES.PRE.h
RECIPES_NO_EXT_EXPAND = "$$(echo $(RECIPES) | sed -e 's/\.PRE\.h//')"

# Resulting single-file amalgamations after preprocessing input file
OUT_C  = $(OUT_NO_EXT).c
OUT_H  = $(OUT_NO_EXT).h
OUT_O  = $(OUT_NO_EXT).o

CFLAGS   ?= -O2
CFLAGS   += -I. -Wall -Wextra -Wpedantic -std=c89
CPP       = cc -E # If you run into issues, try changing this to 'cpp'
CPPFLAGS += -nostdinc -P -CC

# Convert 'foo/bar_baz-tuna.PRE.h' -> 'FOO_BAR_BAZ_TUNA_H'
# TODO: Evaluate if the sed expressions can be joined (see https://unix.stackexchange.com/a/145409)
HEADER_TAG_EXPAND   = "$$(echo '$<' | sed -e 's/\(.*\)\.PRE.h/\1/' | sed -e 's/\//_/g' | sed -e 's/-/_/g' | tr '[:lower:]' '[:upper:]')_H"
# Doxygen file description
DOXYGEN_DESC_EXPAND = "/**\n * @file $@\n * @author Reflect-C\n * @brief Reflect-C generated code\n */\n"

all: $(OUT_O)

$(OUT_O): $(OUT_C) $(OUT_H)
	@ echo "Creating $@"
	$(CC) -c $(CFLAGS) $< -o $@

.c.o:
	@ echo "Creating $@"
	$(CC) -c $(CFLAGS) $< -o $@

$(EXPAND_COMMENTS):
	$(CC) $(CFLAGS) $@.c -o $@

$(OUT_H): $(TEMPFILE) $(RECIPES) $(EXPAND_COMMENTS)
	@ echo "Generating $@"
	@ echo "#ifndef $(HEADER_TAG_EXPAND)" > $@
	@ echo "#define $(HEADER_TAG_EXPAND)\n" >> $@
	$(CPP) $(CFLAGS) -DREFLECTC_DEFINITIONS -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_HEADER -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	@ echo "\n#endif /* $(HEADER_TAG_EXPAND) */" >> $@
$(OUT_C): $(TEMPFILE) $(RECIPES) $(EXPAND_COMMENTS)
	@ echo "Adding forward definitions to $@"
	@ echo "#include \"$(OUT_H)\"" > $@
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_FORWARD -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	@ echo "Generating $@"	
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) >> $@

headers: $(HEADERS)

$(HEADERS): $(RECIPES) $(EXPAND_COMMENTS)

.SUFFIXES: .PRE.h .h
.PRE.h.h:
	@ echo "Generating $@"
	@ echo $(DOXYGEN_DESC_EXPAND) > $@
	@ echo "#ifndef $(HEADER_TAG_EXPAND)" >> $@
	@ echo "#define $(HEADER_TAG_EXPAND)\n" >> $@
	$(CPP) $(CFLAGS) -DREFLECTC_DEFINITIONS -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	$(CPP) $(CFLAGS) $(DFLAGS) -DREFLECTC_HEADER -DREFLECTC_ENTRY=\"$<\" $(CPPFLAGS) $(RECIPES) | ./$(EXPAND_COMMENTS) >> $@
	@ echo "\n#endif /* $(HEADER_TAG_EXPAND) */" >> $@

echo:
	@ echo 'RECIPES_NO_EXT: $(RECIPES_NO_EXT)'
	@ echo 'OUT_H: $(OUT_H)'
	@ echo 'OUT_C: $(OUT_C)'
	@ echo 'OUT_O: $(OUT_O)'
	@ echo 'HEADERS: $(HEADERS)'

clean:
	@ rm -f $(OUT_H) $(OUT_C) $(OUT_O)
	@ rm -f $(HEADERS) $(EXPAND_COMMENTS)

.PHONY: headers clean
