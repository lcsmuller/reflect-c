# Usage Examples

This guide complements the README by showing how the generated metadata can be used in real programs. All snippets assume that you have already run `make gen` so that `reflect-c_GENERATED.h` and the corresponding wrappers exist.

## Printing Struct Metadata

```c
#include <stdio.h>
#include "reflect-c.h"
#include "reflect-c_GENERATED.h"

void dump_struct(struct bar *value)
{
    struct reflectc *root = reflectc_from_bar(value, NULL);
    for (size_t i = 0; i < root->members.length; ++i) {
        const struct reflectc *field = &root->members.array[i];
        printf("%.*s (%zu bytes)\n",
               (int)field->name.length,
               field->name.buf,
               field->size);
    }
    free(root);
}
```

## Updating a Field by Name

```c
#include <string.h>

void set_number(struct bar *value, int replacement)
{
    struct reflectc *root = reflectc_from_bar(value, NULL);
    size_t pos = reflectc_get_pos(root, "number", strlen("number"));
    if (pos != SIZE_MAX) {
        reflectc_set_member(root, pos, &replacement, sizeof replacement);
    }
    free(root);
}
```

## Serialising to JSON

The integration test `test/test.c` contains a fully-working serializer. The core logic looks like this:

```c
void json_stringify(struct jsonb *jb,
                    const struct reflectc *member,
                    char buf[],
                    size_t bufsize)
{
    if (member->decorator.length && member->ptr_value == NULL) {
        jsonb_null(jb, buf, bufsize);
        return;
    }

    switch (member->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize,
                     reflectc_deref(member),
                     strlen(reflectc_deref(member)));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize,
                     *(int *)reflectc_get_member(member, 0));
        break;
    case REFLECTC_TYPES__struct:
        jsonb_object(jb, buf, bufsize);
        for (size_t i = 0; i < member->members.length; ++i) {
            const struct reflectc *child = &member->members.array[i];
            jsonb_key(jb, buf, bufsize, child->name.buf, child->name.length);
            json_stringify(jb, child, buf, bufsize);
        }
        jsonb_object_pop(jb, buf, bufsize);
        break;
    default:
        break;
    }
}
```

Compile and run the demo with:

```sh
make -C test
./test/test
```

## Parsing JSON Back Into Structs

```c
void parse_bar(struct bar *out, const char *json, size_t len)
{
    struct reflectc *root = reflectc_from_bar(out, NULL);
    jsmnf_loader loader;
    jsmnf_table *pairs = NULL;
    size_t pairs_len = 0;

    jsmnf_init(&loader);
    if (jsmnf_load_auto(&loader, json, len, &pairs, &pairs_len) <= 0) {
        free(root);
        return;
    }

    const jsmnf_pair *fields = loader.root->fields;
    for (size_t i = 0; i < loader.root->length; ++i) {
        const jsmnf_pair *pair = &fields[i];
        size_t pos = reflectc_get_pos(root,
                                      pair->key.buf,
                                      pair->key.length);
        if (pos == SIZE_MAX) continue;
        const struct reflectc *field = &root->members.array[pos];
        /* ... convert based on field->type ... */
    }

    free(pairs);
    free(root);
}
```

## Treating Structs as Arrays

`reflectc_array` lets you reuse a metadata buffer to describe arrays of the same type dynamically—handy when parsing JSON sequences:

```c
struct reflectc *rows = reflectc_from_bar(NULL, NULL);
reflectc_array(rows, length);
for (size_t i = 0; i < length; ++i) {
    rows[i].ptr_value = values + i;
    /* nested members are already cloned from index 0 */
}
```

## Running the Examples

All snippets compile as part of the existing test harness. After editing the recipes, regenerate metadata and rebuild the tests:

```sh
make gen
make -C test
./test/test
```
