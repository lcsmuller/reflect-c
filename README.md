# Reflect-C

A library that adds reflection capabilities to C structs through code generation.

## Features
- Zero runtime overhead - all metadata generated at compile time
- Struct field access by name
- JSON serialization support
- Support for nested structs, unions, and enums
- Field type validation
- C89 compatible
- MIT licensed

## Basic Usage

1. Create a `.PRE.h` file (e.g. `api/bar.PRE.h`) with your struct definitions:

```c
#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stddef.h>
#include <stdbool.h>
*/
#endif

PRIVATE(struct, bar, 3, (
        (_, _, bool, _, boolean, _),
        (_, _, int, _, number, _),
        (_, _, char, *, string, _)
    )
)

PUBLIC(struct, baz, 4, (
        (_, struct, bar, *, a, _),
        (_, struct, bar, *, b, _),
        (_, struct, bar, ***, c, _),
        (_, _, char, *, d, _)
    )
)
```

2. Generate the reflection code using your build system to process the `.PRE.h` files.

3. Use the generated code:

```c
#include "reflect-c_GENERATED.h"

// Create and initialize structs
struct bar a = { true, 42, "hello world" };
struct bar *aa = &a;
struct bar **aaa = &aa;
struct baz baz = { &a, &a, &aaa, "hello world" };

// Create reflection wrapper
struct reflectc *wrapped_baz = reflectc_from_baz(&baz, 1, NULL);

// Access fields by name
char *str_d = *(char **)reflectc_get_field(wrapped_baz, "d", 1)->value;
struct bar *bar_a = *(struct bar **)reflectc_get_field(wrapped_baz, "a", 1)->value;

// Access nested fields
struct reflectc *field_a = reflectc_get_field(wrapped_baz, "a", 1);
int number = *(int *)reflectc_get_field(field_a, "number", 6)->value;
```

## Example JSON Serialization

Reflect-C reflection capabilities make it possible to create a generic JSON serializer function in plain C:

```c
#include "json-build.h"

// Serialize a reflected struct to JSON
void json_stringify(struct jsonb *jb, struct reflectc *field, char buf[], const size_t bufsize) {
    switch (field->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize, *(char **)field->value, strlen(*(char **)field->value));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize, *(int *)field->value);
        break;
    case REFLECTC_TYPES__bool:
        jsonb_bool(jb, buf, bufsize, *(bool *)field->value);
        break;
    case REFLECTC_TYPES__struct: {
        jsonb_object(jb, buf, bufsize);
        for (size_t i = 0; i < field->capacity; ++i) {
            const struct oa_hash_entry *entry = &field->buckets[i];
            if (entry->state != OA_HASH_ENTRY_OCCUPIED) continue;

            struct reflectc *field = entry->value;
            jsonb_key(jb, buf, bufsize, field->name.buf, field->name.len);
            json_stringify(jb, field, buf, bufsize);
        }
        jsonb_object_pop(jb, buf, bufsize);
    } break;
    // Add other types as needed
    }
}
```

## API Reference

- `reflectc_from_X()` - Creates a reflection wrapper for struct X
- `reflectc_get_field()` - Gets a field by name from a reflected struct
- `reflectc_add_field()` - Adds a field to a reflected struct
- `reflectc_get_pointer_depth()` - Gets the pointer indirection level of a field
- `reflectc_derefer_max()` - Dereferences a pointer field to its maximum depth

## License

This project is licensed under the MIT License - see the LICENSE file for details.
