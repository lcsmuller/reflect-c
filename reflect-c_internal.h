#ifndef REFLECT_C_INTERNAL_H
#define REFLECT_C_INTERNAL_H

#include <stddef.h>

#include "submodules/oa-hash/oa_hash.h"

#include "reflect-c.h"

typedef struct reflectc *(*const reflectc_from_type_t)(void *,
                                                       enum reflectc_modes,
                                                       struct reflectc *);

struct reflectc {
    REFLECTC_FIELD_ATTRIBUTES;
    void *value;
    reflectc_from_type_t init;
    struct oa_hash ht;
};

#endif /* REFLECT_C_INTERNAL_H */
