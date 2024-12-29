#ifndef REFLECT_C_INTERNAL_H
#define REFLECT_C_INTERNAL_H

#include <stddef.h>

#include "submodules/oa-hash/oa_hash.h"

#include "reflect-c.h"

struct reflectc {
    const unsigned ptr_depth;
    const size_t size;
    const struct {
        const size_t len;
        const char *const buf;
    } name;
    const enum reflectc_types type;
    reflectc_from_type_t init;
    void *value;
    struct oa_hash ht;
    enum reflectc_modes mode;
};

#endif /* REFLECT_C_INTERNAL_H */
