#include <stdlib.h>
#include <string.h>

#include "reflect-c.h"

size_t
reflectc_length(const struct reflectc *member)
{
    size_t length = 1;

    if (!member || !member->ptr_value) return 0;

    if (member->dimensions.len) {
        const char *ptr = member->dimensions.buf;
        while ((ptr = strchr(ptr, '[')) != NULL) {
            if (strchr(++ptr, ']') == NULL) return 0;
            length *= (size_t)strtoul(ptr, NULL, 10);
        }
    }
    return member->length > length ? member->length : length;
}

ptrdiff_t
reflectc_get_pos(const struct reflectc *root,
                 const char *const name,
                 const size_t len)
{
    const struct reflectc *f;
    for (f = &root->members.array[0];
         f != &root->members.array[root->members.len]; ++f)
    {
        if (f->name.len == len && !memcmp(f->name.buf, name, len)) {
            return (ptrdiff_t)(f - root->members.array);
        }
    }
    return -1;
}

unsigned
reflectc_get_pointer_depth(const struct reflectc *member)
{
    unsigned depth = 1;
    if (!member || !member->ptr_value) {
        return 0;
    }
    if (member->decorator.len) {
        const char *ptr = member->decorator.buf;
        while ((ptr = strchr(ptr++, '*'))) {
            ++depth;
            ++ptr;
        }
    }
    if (member->dimensions.len && strchr(member->dimensions.buf, '[')) {
        ++depth;
    }
    return depth;
}

const void *
reflectc_deref(const struct reflectc *member)
{
    if (!member) {
        return NULL;
    }
    /* If it looks like an array (e.g., int [N]), deref to get the pointer */
    if (member->dimensions.len && member->dimensions.buf
        && strchr(member->dimensions.buf, '['))
    {
        return (char(*)[])member->ptr_value;
    }
    /* If it looks like a pointer of any level (e.g., *, **, etc.), deref once
     */
    if (member->decorator.len && member->decorator.buf
        && strchr(member->decorator.buf, '*'))
    {
        return *(void **)member->ptr_value;
    }
    /* Already pointer, nothing to do */
    return member->ptr_value;
}
