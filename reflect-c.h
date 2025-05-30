#ifndef REFLECTC_H
#define REFLECTC_H

#include <stddef.h>

enum reflectc_types {
    REFLECTC_TYPES__void,
    REFLECTC_TYPES__bool,
    REFLECTC_TYPES__char,
    REFLECTC_TYPES__short,
    REFLECTC_TYPES__int,
    REFLECTC_TYPES__long,
    REFLECTC_TYPES__float,
    REFLECTC_TYPES__double,
    REFLECTC_TYPES__struct,
    REFLECTC_TYPES__union,
    REFLECTC_TYPES__enum,
    REFLECTC_TYPES__EXTEND
};

typedef struct reflectc *(*reflectc_from_cb)(void *self,
                                             struct reflectc *root);

struct reflectc {
    const size_t size;
    const struct {
        const char *const buf;
        const size_t len;
    } qualifier, decorator, name, dimensions;
    const enum reflectc_types type;
    size_t length;
    void *ptr_value;
    struct {
        size_t len;
        const struct reflectc *array;
    } fields;
    const reflectc_from_cb from_cb;
};

size_t reflectc_length(const struct reflectc *field);

ptrdiff_t reflectc_get_pos(struct reflectc *root,
                           const char *const name,
                           const size_t len);

unsigned reflectc_get_pointer_depth(const struct reflectc *field);

void *reflectc_deref(const struct reflectc *field);

#define reflectc_get(_root, _pos)                                             \
    reflectc_deref((_root)->fields.array + (_pos))
#define reflectc_set(_root, _pos, _value, _size)                              \
    (((_root)->size == _size && (_root)->fields.len)                          \
         ? (memcpy((_root)->fields.array[_pos].ptr_value, &_value, _size), 0) \
         : -1)

#define reflectc_get_fast(_namespace, _member_name, _root)                    \
    reflectc_deref((_root)->fields.array                                      \
                   + __REFLECTC_LOOKUP__##_namespace##_##_member_name##__)
#define reflectc_set_fast(_namespace, _member_name, _root, _value, _size)      \
    (((_root)->size == _size && (_root)->fields.len)                           \
         ? (memcpy(                                                            \
                (_root)                                                        \
                    ->fields                                                   \
                    .array                                                     \
                        [__REFLECTC_LOOKUP__##_namespace##_##_member_name##__] \
                    .ptr_value,                                                \
                &_value, _size),                                               \
            0)                                                                 \
         : -1)

#endif /* REFLECTC_H */
