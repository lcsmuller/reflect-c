#include <stdlib.h>
#include <string.h>

#include "reflect-c.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t) - 1)
#endif

size_t
reflectc_length(const struct reflectc *member)
{
    struct reflectc_mut *mut_member = (struct reflectc_mut *)member;
    size_t length = 1;

    if (!member || !member->ptr_value) return 0;

    if (member->dimensions.length) {
        const char *ptr = member->dimensions.buf;
        while ((ptr = strchr(ptr, '[')) != NULL) {
            if (strchr(++ptr, ']') == NULL) return 0;
            length *= (size_t)strtoul(ptr, NULL, 10);
        }
    }
    if (member->length < length) {
        mut_member->length = length;
    }
    return member->length;
}

size_t
reflectc_get_pos(const struct reflectc *root,
                 const char *const name,
                 const size_t length)
{
    const struct reflectc *m;
    for (m = &root->members.array[0];
         m != &root->members.array[root->members.length]; ++m)
    {
        if (m->name.length == length && !memcmp(m->name.buf, name, length)) {
            return (size_t)(m - root->members.array);
        }
    }
    return SIZE_MAX;
}

unsigned
reflectc_get_pointer_depth(const struct reflectc *member)
{
    unsigned depth = 1;
    if (!member || !member->ptr_value) {
        return 0;
    }
    if (member->decorator.length) {
        const char *ptr = member->decorator.buf;
        while ((ptr = strchr(ptr++, '*'))) {
            ++depth;
            ++ptr;
        }
    }
    if (member->dimensions.length && strchr(member->dimensions.buf, '[')) {
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
    /* If its a pointer to array (e.g. int (*)[N]), deref once */
    if (member->dimensions.length && member->dimensions.buf
        && strchr(member->dimensions.buf, '['))
    {
        return (char(*)[])member->ptr_value;
    }
    /* If its a pointer of any level (e.g., *, **, etc.), deref once */
    if (member->decorator.length && member->decorator.buf
        && strchr(member->decorator.buf, '*'))
    {
        return *(void **)member->ptr_value;
    }
    /* Already pointer, nothing to do */
    return member->ptr_value;
}

const void *
reflectc_memcpy(const struct reflectc *dest, const void *src, size_t size)
{
    if (!dest || !src || !dest->ptr_value || size > dest->size) {
        return NULL;
    }
    return memcpy((void *)dest->ptr_value, src, size);
}
