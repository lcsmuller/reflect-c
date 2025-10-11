# Runtime API Reference

The runtime half of Reflect-C lives in `reflect-c.h` and `reflect-c.c`. It provides utilities to inspect and mutate the metadata generated from recipes. This document complements the inline comments with detailed behaviour descriptions.

## Data Structures

### `struct reflectc`

Immutable view of a type member. Key fields:

- `size` - size in bytes of the *declared* type.
- `qualifier`, `decorator`, `name`, `dimensions` - string slices describing the original declaration. Empty fields carry `length == 0`.
- `type` - `enum reflectc_types` value identifying the category (`REFLECTC_TYPES__int`, `REFLECTC_TYPES__struct`, ...).
- `length` - number of elements covered by this node. For scalars it is `1`; for arrays it is expanded on demand.
- `ptr_value` - pointer to the underlying data (may be `NULL` for optional pointers).
- `members` - nested reflection nodes when `type` is `struct` or `union`.
- `from_cb` - callback used to hydrate nested nodes on demand.

The mutable counterpart `struct reflectc_mut` shares the same layout but allows the generator to populate fields.

## Constructor Functions

Each `PUBLIC` or `PRIVATE` recipe spawns the following helper:

```c
struct reflectc *reflectc_from_<type>(<type> *self, struct reflectc *reuse);
```

- Pass `self != NULL` to wrap an existing instance.
- Pass `self == NULL` to clone metadata without binding it to a value (useful when preparing array slots).
- Provide `reuse` to hydrate an existing buffer (typically via `reflectc_array`) and avoid reallocations.
- Returns `NULL` on allocation failure.

### Lifetime

The returned pointer is allocated with `calloc`/`malloc`. Call `free()` on the original pointer when you are done. Nested `members.array` buffers are stored alongside the parent slice and reclaimed at the same time.

## Utility Functions

| Function | Description |
| --- | --- |
| `size_t reflectc_length(const struct reflectc *member)` | Returns effective length for array-like nodes. Auto-expands `length` when the declaration has dimensions (e.g., `[4]`). Returns `0` if `member` or its `ptr_value` is `NULL`. |
| `size_t reflectc_get_pos(const struct reflectc *root, const char *name, size_t len)` | Performs a linear search through `root->members` for the given name. Returns `SIZE_MAX` when not found. |
| `reflectc_get_pos_fast(struct, foo, bar, root)` | Macro emitted per type; resolves to a constant index for `bar` inside `struct foo`. |
| `unsigned reflectc_get_pointer_depth(const struct reflectc *member)` | Counts pointer levels inferred from the decorator and dimensions. Arrays count as one additional level. Returns `0` when the member or its `ptr_value` is `NULL`. |
| `const void *reflectc_deref(const struct reflectc *member)` | Provides a convenient view of `member->ptr_value`. Automatically dereferences `T (*)[N]` and multi-level pointers (`**`, `***`) once. |
| `void *reflectc_resolve(const struct reflectc *member)` | Walks through every pointer level encoded in the decorator until it reaches the concrete value. Returns `NULL` when any intermediate pointer is `NULL`. |
| `int reflectc_expand(const struct reflectc *member)` | Lazily hydrates nested metadata by invoking the generator-provided `from_cb`. Returns non-zero once `member->members` has been populated. |
| `const void *reflectc_memcpy(const struct reflectc *dest, const void *src, size_t size)` | Copies raw bytes into the location described by `dest`. Guards against size mismatches and `NULL` pointers. Returns the destination address on success or `NULL` on failure. |
| `const char *reflectc_string(const struct reflectc *dest, const char src[], size_t size)` | Ensures the target can accept string data (character pointer with depth ≥ 2), allocates storage when needed, then copies `size` bytes. Returns the written buffer. |
| `void reflectc_array(const struct reflectc *root, size_t length)` | Extends the metadata buffer to describe `length` consecutive items. Useful for dynamic arrays when parsing unbounded inputs. |
| `const void *reflectc_get_member(const struct reflectc *root, size_t pos)` | Shorthand for `reflectc_deref(&root->members.array[pos])` with bounds checking. Returns `NULL` when out of range. |
| `const void *reflectc_set_member(const struct reflectc *root, size_t pos, const void *value, size_t size)` | Copies data into a member selected by index. Returns the destination pointer or `NULL` on error. |

## Error Handling

- Most functions return `NULL` or `SIZE_MAX` on invalid inputs; they never `abort`.
- Deep pointer dereferencing only occurs when the decorator explicitly encodes pointer levels. Attempting to dereference `NULL` pointers yields `NULL`.
- `reflectc_array` silently returns when reallocation fails, leaving the original metadata untouched.

## Extending the Runtime

- The generator populates `from_cb` with child constructors for nested structs/unions. You can override `from_cb` manually for custom hydration logic.
- To integrate with custom allocators, link against replacements for `malloc`, `calloc`, `realloc`, and `free`, or wrap constructor calls behind your own allocation layer.
- Additional utility helpers (e.g., typed getters/setters) can be layered on top using the raw operations above without modifying generated code.
Need to describe custom scalars? Drop an enum alongside your typedefs:

```c
enum reflectc_custom_types {
    REFLECTC_TYPES__reflectc_words_t = REFLECTC_TYPES__EXTEND,
    REFLECTC_TYPES__reflectc_numbers_t,
};
```

Because enumerators live in the same namespace, generated metadata for those typedefs reports `type == REFLECTC_TYPES__reflectc_words_t` (or whichever alias you introduce), giving your runtime a stable tag for project-specific primitives while keeping the core generator untouched.
