#include "reflect-c_internal.h"

void *
reflectc_derefer_max(const struct reflectc *field)
{
    void *value = (void *)field->value;
    unsigned depth;
    for (depth = reflectc_get_pointer_depth(field); depth > 0; --depth) {
        if (value == NULL) return NULL;
        value = *(void **)value;
    }
    return value;
}
