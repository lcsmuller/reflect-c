#include <stdlib.h>
#include <string.h>

#include "reflect-c_internal.h"

struct reflectc *
reflectc_get_field(struct reflectc *root,
                   const char *const name,
                   const size_t len)
{
    return oa_hash_get((struct oa_hash *)root, name, len);
}

struct reflectc *
reflectc_add_field(struct reflectc *root, struct reflectc *field, void *value)
{
    struct reflectc *new_field = malloc(sizeof *new_field);
    memcpy(new_field, field, sizeof *field);
    if (value && !field->value) new_field->value = value;
    return oa_hash_set((struct oa_hash *)root, new_field->name.buf,
                       new_field->name.len, new_field);
}

unsigned
reflectc_get_pointer_depth(const struct reflectc *field)
{
    const char *ptr = field->decorator.buf;
    unsigned depth = 0;

    if (field && field->decorator.len) {
        do {
            if (*ptr == '*') ++depth;
        } while (*++ptr != '\0');
    }
    return depth;
}
