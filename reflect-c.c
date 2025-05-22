#include <stdlib.h>
#include <string.h>

#include "reflect-c.h"

size_t
reflectc_length(const struct reflectc *field)
{
    size_t length = 1;

    if (!field || !field->ptr_value) return 0;

    if (field->dimensions.len) {
        const char *ptr = field->dimensions.buf;
        while ((ptr = strchr(ptr, '[')) != NULL) {
            if (strchr(++ptr, ']') == NULL) return 0;
            length *= (size_t)strtoul(ptr, NULL, 10);
        }
    }
    return field->length > length ? field->length : length;
}

void *
_reflectc_get(struct reflectc *root, const char *const name, const size_t len)
{
    const struct reflectc *field = oa_hash_get(&root->ht, name, len);
    return field ? field->ptr_value : NULL;
}

void *
reflectc_append(struct reflectc *root, struct reflectc *field, void *ptr_value)
{
    struct reflectc *new_field = malloc(sizeof *new_field);
    memcpy(new_field, field, sizeof *field);
    new_field->ptr_value = ptr_value;
    if ((field = oa_hash_set(&root->ht, new_field->name.buf,
                             new_field->name.len, new_field)))
    {
        return field->ptr_value;
    }
    return NULL;
}

unsigned
reflectc_get_pointer_depth(const struct reflectc *field)
{
    unsigned depth = 1;

    if (!field || !field->ptr_value) return 0;

    if (field->decorator.len) {
        const char *ptr = field->decorator.buf;
        while (strchr(ptr++, '*') != NULL)
            ++depth;
    }
    if (field->dimensions.len && strchr(field->dimensions.buf, '[')) {
        ++depth;
    }
    return depth;
}
