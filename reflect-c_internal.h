#ifndef REFLECT_C_INTERNAL_H
#define REFLECT_C_INTERNAL_H

#include "reflect-c.h"

struct reflectc_mut {
    struct oa_hash ht;
    REFLECTC_FIELD_ATTRIBUTES;
    void *value;
};

void *reflectc_derefer_max(const struct reflectc *field);

#endif /* REFLECT_C_INTERNAL_H */
