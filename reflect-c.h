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
        const size_t length;                                                  \
    } qualifier, decorator, name, dimensions;                                 \
    const enum reflectc_types type;                                           \
    _qualifier size_t length;                                                 \
    _qualifier void *_qualifier ptr_value;                                    \
    _qualifier struct {                                                       \
        _qualifier size_t length;                                             \
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

size_t reflectc_get_pos(const struct reflectc *root,
                        const char *const name,
                        const size_t len);

void reflectc_array(const struct reflectc *root, const size_t length);

#define reflectc_get_pos_fast(_container, _namespace, _member_name, _root)    \
    (size_t)(__REFLECTC_LOOKUP__##_namespace##__##_member_name##__)

unsigned reflectc_get_pointer_depth(const struct reflectc *member);

const void *reflectc_deref(const struct reflectc *field);
const void *reflectc_memcpy(const struct reflectc *field,
                            const void *map,
                            const size_t size);
const char *reflectc_string(const struct reflectc *dest,
                            const char src[],
                            const size_t size);
void *reflectc_resolve(const struct reflectc *member);
int reflectc_expand(const struct reflectc *member);

#define reflectc_get reflectc_deref
#define reflectc_set reflectc_memcpy

#define reflectc_get_member(_root, _pos)                                      \
    (((_root) && (_pos) < (_root)->members.length)                            \
         ? reflectc_deref((_root)->members.array + (_pos))                    \
         : NULL)
#define reflectc_set_member(_root, _pos, _value, _size)                       \
    (((_root) && (_pos) < (_root)->members.length)                            \
         ? reflectc_memcpy((_root)->members.array + (_pos), (_value),         \
                           (_size))                                           \
         : NULL)

#endif /* REFLECTC_H */
