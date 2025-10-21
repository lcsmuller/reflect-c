# Recipe Format Reference

This document explains how to describe your types so that Reflect-C can generate reflection metadata. Recipes are plain headers with a `.recipe.h` suffix that remain safe to include in normal compilation units; during generation they are replayed multiple times under different macro definitions.

## Goals

- Provide a declarative, compiler-friendly way to define structs, unions, and enums.
- Keep recipes as close as possible to standard C syntax to minimise cognitive load.
- Allow the generator to emit several artefacts (real type definitions, metadata tables, wrappers) from the same source of truth.

## File Layout

Put your recipes under `api/` (the default path consumed by `make gen`). Each file should end with `.recipe.h` so the helper makefile can pick it up automatically.

```text
api/
├── domain.recipe.h
└── messaging.recipe.h
```

The main dispatcher `reflect-c_RECIPES.recipe.h` includes every recipe when the generator runs, so you normally do **not** include recipe files elsewhere yourself.

## Core Macros

| Macro | Meaning | Notes |
| --- | --- | --- |
| `PUBLIC(container, name, count, (members))` | Exposes the generated symbols (type definition, wrappers, lookup enums). | Use for types that must be visible to other translation units. |
| `PRIVATE(container, name, count, (members))` | Generates the same metadata but keeps the symbols `static`. | Useful for internal helper structs. |

The `container` argument must be one of `struct`, `union`, or `enum`. The `count` is the total number of member tuples inside the list and is checked at generation time.

## Member Tuple Anatomy

Structs and unions share the same tuple signature:

```c
(_, qualifier, container, type, decorator, name, dimensions)
```

- **qualifier** - `const`, `volatile`, or `_` (underscore) for none.
- **container** - `struct`, `union`, or `_` when referencing a fundamental type.
- **type** - the raw type identifier (e.g., `int`, `foo`, `bar`).
- **decorator** - pointer suffix (`*`, `**`, etc.) or `_` for scalars.
- **name** - member name.
- **dimensions** - array declarators such as `[4]` or `_` when absent.
- **attrs** - user-defined attribute mask (any `unsigned long` constant expression, e.g. `0ul`).

Examples:

```c
PUBLIC(struct, person, 3, (
    (_, _, char, *, name, _, 0ul),
    (_, _, int, _, age, _, 0ul),
    (_, struct, address, _, address, _, 0ul)
))

PRIVATE(struct, bucket, 1, (
    (_, _, int, _, items, [32], 0ul)
))
```

Enum tuples omit qualifiers and dimensions:

```c
PUBLIC(enum, color, 4, (
    (COLOR_RED, =, 1),
    (COLOR_GREEN, =, 2),
    (COLOR_BLUE, =, 3),
    (COLOR_COUNT, _, _)
))
```

Use `_` when a field should be left empty; the generator normalises underscores to empty tokens.

## Conditional Directives (`/*#! ... */`)

**Any `#include` or `#define` directive that the generator needs to see must be wrapped in `/*#! ... */` blocks.** This is a **required** pattern for recipes to work correctly.

The `reflect-c_EXPAND_COMMENTS` utility strips the `/*#!` prefix, effectively "activating" the directive at generation time while keeping the recipe safe to include elsewhere. Without these directives, the preprocessor never sees your helper includes and generation fails silently.

### Basic Example

```c
#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stdbool.h>
*/
#endif

PUBLIC(struct, person, 2, (
    (_, _, char, *, name, _, 0ul),
    (_, _, bool, _, active, _, 0ul)
))
```

### Multiple Includes

```c
#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
*/
#endif
```

### Helper Macros

```c
#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stdbool.h>
#define MAGIC_NUMBER 42
*/
#endif
```

### When to Use

Use `/*#! ... */` directives whenever you need to:

- Include standard library headers (e.g., `<stdbool.h>`, `<stddef.h>`, `<stdint.h>`)
- Include custom headers for nested types
- Define helper macros for the definitions pass
- Declare custom typedefs used by your recipes

### Important for Tooling Authors

If you're writing scripts or tools that generate recipes programmatically, always emit `/*#! ... */` directives around any `#include` or `#define` statements. This is the contract that ensures your generated recipes will work with Reflect-C's preprocessor pipeline.

### Known Limitation: Backslashes in Comments

**Do not use backslash (`\`) characters inside `/*#! ... */` comment blocks.** The C preprocessor expands backslashes for line continuation regardless of whether they appear inside comments, which can cause issues during Reflect-C's post-expansion processing.

For example, this will cause problems:

```c
/*#!
#define MESSAGE "hello \
world"
*/
```

The preprocessor will join the lines before Reflect-C processes the comment, potentially leading to malformed output. Keep all content within `/*#! ... */` blocks on single lines, or avoid using backslashes for line continuation.

## Replayed Roles

The generator includes every recipe multiple times by setting these flags:

- `REFLECTC_DEFINITIONS` - emits the actual type definitions (struct/union/enum).
- `REFLECTC_WRAPPER` + `REFLECTC_HEADER` - declares `reflectc_from_<type>` functions and lookup enums for static access.
- `REFLECTC_WRAPPER` + `REFLECTC_FORWARD` - produces forward declarations for nested types.
- `REFLECTC_WRAPPER` - emits the implementation of `reflectc_from_<type>`.

You rarely need to gate on these macros manually, but it is useful when a recipe should only emit specific statements in certain passes.

## Namespaces and Lookups

For each `PUBLIC` type named `foo` the generator produces:

- `struct foo` / `union foo` / `enum foo` definitions.
- `reflectc_from_foo(foo *self, struct reflectc *root)` wrapper.
- `enum { __REFLECTC_LOOKUP__foo_field__, ... }` for fast field indexing.
- `REFLECTC_DEFINED__foo` macro, useful when other recipes need to know if a type is available.

You can statically guard nested recipes using `#ifdef REFLECTC_DEFINED__foo` to avoid forward-declaration cycles.

## Best Practices

- **Always wrap `#include` and `#define` directives in `/*#! ... */` blocks** - this is the most important rule for recipe authors and tooling. Without these directives, the generator will fail silently.
- Keep recipe files small and cohesive; one top-level type per file works well.
- Prefer `_` over empty tokens to avoid accidental macro pasting issues.
- Remember to update the member count when adding or removing tuples (the generator does not infer it).
- Use standard headers inside `/*#! */` blocks rather than including them from the runtime.
- Run `make gen` whenever recipes change so the generated metadata stays in sync.

With these guidelines you can confidently extend the reflection metadata without diving into the generator internals.
