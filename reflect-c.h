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

#define _REFLECTC_PICKER_const     struct reflectc
#define _REFLECTC_PICKER___BLANK__ struct reflectc_mut
#define REFLECTC_ATTRIBUTES(_qualifier)                                       \
    const size_t size;                                                        \
    const struct {                                                            \
        const char *const buf;                                                \
        const size_t len;                                                     \
    } qualifier, decorator, name, dimensions;                                 \
    const enum reflectc_types type;                                           \
    _qualifier size_t length;                                                 \
    _qualifier void *_qualifier ptr_value;                                    \
    _qualifier struct {                                                       \
        _qualifier size_t len;                                                \
        _qualifier _REFLECTC_PICKER_##_qualifier *_qualifier array;           \
    } members;                                                                \
    const reflectc_from_cb from_cb

struct reflectc {
    REFLECTC_ATTRIBUTES(const);
};

#define __BLANK__
struct reflectc_mut {
    REFLECTC_ATTRIBUTES(__BLANK__);
};
#undef __BLANK__
#undef _REFLECTC_PICKER_const
#undef _REFLECTC_PICKER___BLANK__

size_t reflectc_length(const struct reflectc *member);

ptrdiff_t reflectc_get_pos(const struct reflectc *root,
                           const char *const name,
                           const size_t len);

unsigned reflectc_get_pointer_depth(const struct reflectc *member);

const void *reflectc_deref(const struct reflectc *member);

#define reflectc_get reflectc_deref
#define reflectc_set(_member, _value, _size)                                  \
    (((_member) && (_member)->ptr_value && (_member)->size == (_size))        \
         ? (memcpy((_member)->ptr_value, &_value, (_member)->size), 0)        \
         : -1)

#define reflectc_get_member(_root, _pos)                                      \
    (((_root) && (_root)->members.array)                                      \
         ? reflectc_deref((_root)->members.array + (_pos))                    \
         : NULL)
#define reflectc_set_member(_root, _pos, _value, _size)                       \
    ((_root) && (_root)->size == (_size) && (_root)->members.len)             \
        ? (memcpy(((void *)(_root)->members.array[_pos].ptr_value, &_value,   \
                   _size),                                                    \
                  0)                                                          \
           : -1)

#define reflectc_get_member_fast(_namespace, _member_name, _root)             \
    (((_root) && (_root)->members.array)                                      \
         ? reflectc_deref(                                                    \
               (_root)->members.array                                         \
               + __REFLECTC_LOOKUP__##_namespace##_##_member_name##__)        \
         : NULL)
#define reflectc_set_member_fast(_namespace, _member_name, _root, _value,      \
                                 _size)                                        \
    (((_root) && (_root)->size == _size && (_root)->members.len)               \
         ? (memcpy(                                                            \
                (void *)(_root)                                                \
                    ->members                                                  \
                    .array                                                     \
                        [__REFLECTC_LOOKUP__##_namespace##_##_member_name##__] \
                    .ptr_value,                                                \
                &_value, _size),                                               \
            0)                                                                 \
         : -1)

#endif /* REFLECTC_H */
