#ifndef REFLECTC_H
#define REFLECTC_H

#define OA_HASH_HEADER
#include "core/oa_hash.h"

enum reflectc_types {
    REFLECTC_TYPES__UNKNOWN,
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

struct reflectc {
    struct oa_hash ht;
    const size_t size;
    const struct {
        const char *const buf;
        const size_t len;
    } qualifier, decorator, name, dimensions;
    const enum reflectc_types type;
    size_t length;
    void *value;
};

struct reflectc *reflectc_get_field(struct reflectc *root,
                                    const char *const name,
                                    const size_t len);

struct reflectc *reflectc_add_field(struct reflectc *root,
                                    struct reflectc *field,
                                    void *value);

unsigned reflectc_get_pointer_depth(const struct reflectc *field);

#endif /* REFLECTC_H */
