# Reflect-C

A C89 library that adds reflection capabilities to C structs through code generation.

## Features
- Zero runtime overhead - all metadata generated at compile time
- Struct field access by name
- No dynamic memory allocation during serialization
- Support for nested structs and arrays
- Field type validation
- C89 compatible
- MIT licensed

## Basic Usage

1. Create a `.PRE.h` file (e.g. `api/person.PRE.h`) with your struct definitions:

```c
// File: api/person.PRE.h
STRUCT(public, person)
    FIELD(name, char, *)       // String field
    FIELD(age, int, 0)         // Integer field 
    FIELD(active, bool, false) // Boolean field
STRUCT_END
```

```c
// File: api/employee.PRE.h  
STRUCT(public, employee)
    FIELD_STRUCT_PTR(info, person, *)  // Nested person struct
    FIELD(id, int, 0)                  // Employee ID
    FIELD_PTR(department, char, *)     // Department name
    FIELD(salary, float, 0.0)          // Salary
STRUCT_END
```

2. Generate the reflection code:

```bash
# API_DIR points to folder containing .PRE.h files
# OUT specifies output filename (will create .c and .h)
make API_DIR=api OUT=generated_code
```

3. Use the generated code:

```c
#include "generated_code.h"

// Create and initialize structs
struct person p = {
    .name = "John",
    .age = 25,
    .active = true
};

struct employee e = {
    .info = &p,
    .id = 12345,
    .department = "Engineering",
    .salary = 75000.0
};

// Access fields by name at runtime
struct reflectc *r = reflectc_from_employee(&e, REFLECTC_MODES_READONLY, NULL);

// Access fields including nested structs
int *id = reflectc_get(r, "id", strlen("id"))->value;
char *dept = reflectc_get(r, "department", strlen("department"))->value;

// Access nested person struct fields
struct reflectc *person_info = reflectc_get(r, "info", strlen("info"))->value;
char *name = reflectc_get(person_info, "name", strlen("name"))->value;
```

The reflection code is generated at compile time with zero runtime overhead. The generated structures can be used normally or accessed dynamically through the reflection API.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.