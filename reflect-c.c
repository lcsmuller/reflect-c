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
_reflectc_get(struct reflectc *root,
              const char *const name,
              const size_t len,
              const int index)
{
    const struct reflectc *f;
    for (f = &root->fields.array[index];
         f != &root->fields.array[root->fields.len]; ++f)
    {
        if (f->name.len == len && !memcmp(f->name.buf, name, len)) {
            return f->ptr_value;
        }
    }
    return NULL;
}

unsigned
reflectc_get_pointer_depth(const struct reflectc *field)
{
    unsigned depth = 1;
    if (!field || !field->ptr_value) {
        return 0;
    }
    if (field->decorator.len) {
        const char *ptr = field->decorator.buf;
        while ((ptr = strchr(ptr++, '*'))) {
            ++depth;
            ++ptr;
        }
    }
    if (field->dimensions.len && strchr(field->dimensions.buf, '[')) {
        ++depth;
    }
    return depth;
}

void *
reflectc_deref(const struct reflectc *field, void *ptr)
{
    if (!field) {
        return NULL;
    }
    if (!ptr && field->ptr_value) {
        ptr = field->ptr_value;
    }
    if (!ptr) {
        return NULL;
    }

    /* If it looks like an array (e.g., int [N]), deref to get the pointer */
    if (field->dimensions.len && strchr(field->dimensions.buf, '[')) {
        return (char(*)[])ptr;
    }
    /* If it looks like a pointer of any level (e.g., *, **, etc.), deref once
     */
    if (field->decorator.len && strchr(field->decorator.buf, '*')) {
        return *(void **)ptr;
    }
    /* Already _type *, nothing to do */
    return ptr;
}
