#include <stdlib.h>
#include <string.h>

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

size_t
reflectc_length(const struct reflectc *field)
{
    size_t length = 1;

    if (!field) return 0;

    if (field->dimensions.len) {
        const char *ptr = field->dimensions.buf;
        while ((ptr = strchr(ptr, '[')) != NULL) {
            if (strchr(++ptr, ']') == NULL) return 0;
            length *= (size_t)strtoul(ptr, NULL, 10);
        }
    }
    return field->length > length ? field->length : length;
}
