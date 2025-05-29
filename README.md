# Reflect-C

A library that adds reflection capabilities to C structs through code generation.

## Features
- Zero runtime overhead with metadata generated at compile time
- Struct field introspection and manipulation
- Support for nested structs, unions, and enums
- Pointer handling with automatic dereferencing
- Field type validation and safety
- Support for field decorators (qualifiers, pointers, arrays)
- Compatible with C89/ANSI C
- No external dependencies
- Easily integrate with JSON serialization/deserialization

## Installation

Clone the repository:

```bash
git clone https://github.com/lcsmuller/reflect-c.git
cd reflect-c
```

The library is header-only, so you can simply include it in your project.

## Basic Usage

1. Define your structures with reflection capabilities using the `PUBLIC` or `PRIVATE` macros:

```c
// my_types.PRE.h
#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stdbool.h>
*/
#endif

PUBLIC(struct, person, 4, (
    (_, _, char, *, name, _),
    (_, _, int, _, age, _),
    (_, _, bool, _, active, _),
    (_, _, char, *, email, _)
))
```

2. Generate the reflection code:

```bash
# Using your preferred method to run the code generator
./reflect-c-gen my_types.PRE.h
```

3. Include the generated headers in your code:

```c
#include "reflect-c.h"
#include "reflect-c_GENERATED.h"

int main() {
    struct person p = {"John Doe", 30, true, "john@example.com"};

    // Create a reflection wrapper for the person struct
    struct reflectc *r_person = reflectc_from_person(&p, NULL);

    // Access fields statically (fast access)
    char *name_field = (char *)reflectc_get_fast(person, name, r_person);
    printf("Name: %s\n", name_field);
    // Access fields dynamically (using string names)
    char *email_field = (char *)reflectc_get(r_person, "email", strlen("email"));
    printf("Email: %s\n", name_field);

    // Clean up
    reflectc_free(r_person);
    return 0;
}
```

## Example: JSON Serialization

The library can be easily used for JSON serialization, as demonstrated in the test code:

```c
#include <stdio.h>
#include <stdbool.h>
#include "json-build/json-build.h"
#include "reflect-c.h"
#include "reflect-c_GENERATED.h"

void json_stringify(struct jsonb *jb, const struct reflectc *field,
                   char buf[], const size_t bufsize)
{
    if (field->decorator.len && field->ptr_value == NULL) {
        jsonb_null(jb, buf, bufsize);
        return;
    }

    switch (field->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize, field->ptr_value, strlen(field->ptr_value));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize, *(int *)field->ptr_value);
        break;
    case REFLECTC_TYPES__bool:
        jsonb_bool(jb, buf, bufsize, *(bool *)field->ptr_value);
        break;
    case REFLECTC_TYPES__float:
        jsonb_number(jb, buf, bufsize, *(float *)field->ptr_value);
        break;
    case REFLECTC_TYPES__struct: {
        jsonb_object(jb, buf, bufsize);
        for (size_t i = 0; i < field->fields.len; ++i) {
            const struct reflectc *f = &field->fields.array[i];
            jsonb_key(jb, buf, bufsize, f->name.buf, f->name.len);
            json_stringify(jb, f, buf, bufsize);
        }
        jsonb_object_pop(jb, buf, bufsize);
    } break;
    default:
        break;
    }
}

int main() {
    // Create a sample struct
    struct bar a = { true, 42, "hello world" }, *aa = &a, **aaa = &aa;
    struct baz baz = { &a, &a, &aaa, "hello world" };

    // Create a reflection wrapper
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);

    // Serialize to JSON
    char json[1024] = {0};
    struct jsonb jb;
    jsonb_init(&jb);
    json_stringify(&jb, wrapped_baz, json, sizeof(json));

    printf("JSON: %s\n", json);
    // Output: {"a":{"boolean":true,"number":42,"string":"hello world"},...}

    // Clean up
    reflectc_free(wrapped_baz);
    return 0;
}
```

## API Reference

### Core Macros

- `PUBLIC(container_type, name, field_count, fields_tuple)`: Defines a public struct/enum/union with reflection
- `PRIVATE(container_type, name, field_count, fields_tuple)`: Defines a private struct/enum/union with reflection

### Field Definition

Fields are defined as tuples with the following format:
- For struct/union fields: `(qualifier, container, type, decorator, name, dimensions)`
- For enum fields: `(enumerator, assignment_op, value)`

### Generated Functions

For each reflected type `foo`, these functions are generated:
- `reflectc_from_foo(foo *self, struct reflectc *root)`: Creates reflection data for a foo instance

### Core Types

- `struct reflectc`: Holds reflection metadata for a struct field
  - `size_t size`: Size of the type in bytes
  - `struct { const char *buf; size_t len } qualifier`: Field qualifier
  - `struct { const char *buf; size_t len } decorator`: Field decorator (* for pointers)
  - `struct { const char *buf; size_t len } name`: Field name
  - `struct { const char *buf; size_t len } dimensions`: Field dimensions (for arrays)
  - `enum reflectc_types type`: Field type
  - `void *ptr_value`: Pointer to the actual value
  - `struct { struct reflectc *array; size_t len } fields`: Nested fields (for structs/unions)
  - `reflectc_from_cb from_cb`: Callback for generating child reflectc objects

## License

This project is licensed under the MIT License - see the LICENSE file for details.
