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

ptrdiff_t
reflectc_get_pos(const struct reflectc *root,
                 const char *const name,
                 const size_t len)
{
    const struct reflectc *f;
    for (f = &root->fields.array[0];
         f != &root->fields.array[root->fields.len]; ++f)
    {
        if (f->name.len == len && !memcmp(f->name.buf, name, len)) {
            return (ptrdiff_t)(f - root->fields.array);
        }
    }
    return -1;
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

const void *
reflectc_deref(const struct reflectc *field)
{
    if (!field) {
        return NULL;
    }
    /* If it looks like an array (e.g., int [N]), deref to get the pointer */
    if (field->dimensions.len && field->dimensions.buf
        && strchr(field->dimensions.buf, '['))
    {
        return (char(*)[])field->ptr_value;
    }
    /* If it looks like a pointer of any level (e.g., *, **, etc.), deref once
     */
    if (field->decorator.len && field->decorator.buf
        && strchr(field->decorator.buf, '*'))
    {
        return *(void **)field->ptr_value;
    }
    /* Already pointer, nothing to do */
    return field->ptr_value;
}
