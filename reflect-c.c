#include <stdlib.h>
#include <string.h>

#include "reflect-c.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

size_t
REFLECTC_NS(_length)(const struct REFLECTC_PREFIX *member)
{
    struct REFLECTC_NS(_mut) *mut_member = (struct REFLECTC_NS(_mut) *)member;
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
REFLECTC_NS(_get_pos)(const struct REFLECTC_PREFIX *root,
                      const char *const name,
                      const size_t length)
{
    const struct REFLECTC_PREFIX *m;
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
REFLECTC_NS(_get_pointer_depth)(const struct REFLECTC_PREFIX *member)
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

static unsigned
REFLECTC_NS(_pointer_levels)(const struct REFLECTC_PREFIX *member)
{
    unsigned levels = 0;
    const char *ptr;

    if (!member) {
        return 0;
    }
    if (!member->decorator.length || !member->decorator.buf) {
        return 0;
    }

    ptr = member->decorator.buf;
    while ((ptr = strchr(ptr, '*')) != NULL) {
        ++levels;
        ++ptr;
    }

    return levels;
}

const void *
REFLECTC_NS(_deref)(const struct REFLECTC_PREFIX *member)
{
    int has_dimensions;
    unsigned pointer_levels;

    if (!member || !member->ptr_value) {
        return NULL;
    }
    has_dimensions = member->dimensions.length && member->dimensions.buf
                     && strchr(member->dimensions.buf, '[');
    pointer_levels = REFLECTC_NS(_pointer_levels)(member);

    if (has_dimensions) {
        /* Embedded arrays yield their storage address */
        if (pointer_levels > 0) {
            /* Pointer-to-array: unwrap one pointer level */
            return *(void **)member->ptr_value;
        }
        return member->ptr_value;
    }

    if (pointer_levels > 0) {
        return *(void **)member->ptr_value;
    }

    return member->ptr_value;
}

void *
REFLECTC_NS(_resolve)(const struct REFLECTC_PREFIX *member)
{
    unsigned pointer_levels;
    const void *addr;
    void *value;

    if (!member || !member->ptr_value) {
        return NULL;
    }

    pointer_levels = REFLECTC_NS(_pointer_levels)(member);
    addr = member->ptr_value;
    value = (void *)addr;

    while (pointer_levels--) {
        if (!value) {
            return NULL;
        }
        value = *(void **)value;
    }

    return value;
}

int
REFLECTC_NS(_expand)(const struct REFLECTC_PREFIX *member)
{
    struct REFLECTC_NS(_mut) * mut;
    void *child;

    if (!member || !member->from_cb) {
        return 0;
    }
    if (member->members.length) {
        return 1;
    }

    child = REFLECTC_NS(_resolve)(member);
    if (!child) {
        return 0;
    }
    mut = (struct REFLECTC_NS(_mut) *)member;
    mut->from_cb(child, (struct REFLECTC_PREFIX *)mut);
    return member->members.length != 0;
}

const void *
REFLECTC_NS(_memcpy)(const struct REFLECTC_PREFIX *dest,
                     const void *src,
                     const size_t size)
{
    if (!dest || !src || size > dest->size) {
        return NULL;
    }
    if (!dest->ptr_value) {
        return NULL;
    }
    return memcpy((void *)REFLECTC_NS(_deref)(dest), src, size);
}

const void *
REFLECTC_NS(_get)(const struct REFLECTC_PREFIX *member)
{
    return REFLECTC_NS(_deref)(member);
}

const void *
REFLECTC_NS(_set)(const struct REFLECTC_PREFIX *member,
                  const void *value,
                  size_t size)
{
    return REFLECTC_NS(_memcpy)(member, value, size);
}

const void *
REFLECTC_NS(_get_member)(const struct REFLECTC_PREFIX *root, size_t pos)
{
    if (!root || pos >= root->members.length) {
        return NULL;
    }
    return REFLECTC_NS(_deref)(root->members.array + pos);
}

const void *
REFLECTC_NS(_set_member)(const struct REFLECTC_PREFIX *root,
                         size_t pos,
                         const void *value,
                         size_t size)
{
    if (!root || pos >= root->members.length) {
        return NULL;
    }
    return REFLECTC_NS(_memcpy)(root->members.array + pos, value, size);
}

const char *
REFLECTC_NS(_string)(const struct REFLECTC_PREFIX *dest,
                     const char src[],
                     const size_t size)
{
    struct REFLECTC_NS(_mut) *mut_dest = (struct REFLECTC_NS(_mut) *)dest;
    unsigned depth;
    int has_dimensions;
    char *target;

    if (!dest || !src) {
        return NULL;
    }

    depth = REFLECTC_NS(_get_pointer_depth)(dest);
    has_dimensions = dest->dimensions.length && dest->dimensions.buf
                     && strchr(dest->dimensions.buf, '[');

    if (dest->type != REFLECTC_NS_UPPER(_TYPES__char) && depth < 2) {
        return NULL;
    }
    if (!dest->ptr_value) {
        return NULL;
    }

    if (has_dimensions) {
        target = (char *)REFLECTC_NS(_deref)(dest);
        if (!target) {
            return NULL;
        }
        memcpy(target, src, size);
        target[size] = '\0';
        return target;
    }

    if (depth >= 2) {
        char **slot = (char **)mut_dest->ptr_value;
        char *buf;
        if (!slot) {
            return NULL;
        }
        buf = *slot;
        if (!buf) {
            buf = calloc(1u, size + 1u);
            if (!buf) {
                return NULL;
            }
            *slot = buf;
        }
        memcpy(buf, src, size);
        buf[size] = '\0';
        return buf;
    }

    target = (char *)dest->ptr_value;
    memcpy(target, src, size);
    target[size] = '\0';
    return target;
}

void
REFLECTC_NS(_array)(const struct REFLECTC_PREFIX *root, const size_t length)
{
    struct REFLECTC_NS(_mut) *mut_root = (struct REFLECTC_NS(_mut) *)root;
    if (!root) return;

    if (length > REFLECTC_NS(_length)(root)) {
        void *tmp;
        size_t i;
        if (!(tmp = realloc(mut_root, length * sizeof *mut_root))) {
            return;
        }
        mut_root = tmp;
        for (i = REFLECTC_NS(_length)((struct REFLECTC_PREFIX *)mut_root);
             i < length; ++i)
        {
            memcpy(mut_root + i, &mut_root[0], sizeof *mut_root);
            mut_root[i].ptr_value = NULL;
            mut_root[i].length = length - i;
        }
    }
    mut_root->length = length;
}

static void REFLECTC_NS(_cleanup_member_block)(struct REFLECTC_NS(_mut)
                                                   * members,
                                               size_t count);

static void
REFLECTC_NS(_cleanup_node)(struct REFLECTC_NS(_mut) * node)
{
    size_t span;
    size_t i;

    if (!node) {
        return;
    }

    span = node->length ? node->length : 1;
    for (i = 0; i < span; ++i) {
        struct REFLECTC_NS(_mut) *current = node + i;

        if (!current->members.array || !current->members.length) {
            continue;
        }

        REFLECTC_NS(_cleanup_member_block)(current->members.array,
                                           current->members.length);

        if (i == 0) {
            free(current->members.array);
        }

        current->members.array = NULL;
        current->members.length = 0;
    }
}

static void
REFLECTC_NS(_cleanup_member_block)(struct REFLECTC_NS(_mut) * members,
                                   size_t count)
{
    size_t i;

    if (!members) return;

    for (i = 0; i < count; ++i) {
        REFLECTC_NS(_cleanup_node)(members + i);
    }
}

void
REFLECTC_NS(_cleanup)(struct REFLECTC_PREFIX *member)
{
    if (!member) return;

    REFLECTC_NS(_cleanup_node)((struct REFLECTC_NS(_mut) *)member);
    free(member);
}

void
REFLECTC_NS(_cleanup_members)(struct REFLECTC_PREFIX *member)
{
    if (!member) return;

    REFLECTC_NS(_cleanup_node)((struct REFLECTC_NS(_mut) *)member);
}

static int
REFLECTC_NS(_for_each_impl)(const struct REFLECTC_PREFIX *member,
                            REFLECTC_NS(_visit_cb) cb,
                            void *ctx)
{
    size_t span;
    size_t i;

    if (!member || !cb) {
        return 1;
    }

    span = member->length ? member->length : 1;
    for (i = 0; i < span; ++i) {
        const struct REFLECTC_PREFIX *current = member + i;
        if (!cb(current, ctx)) {
            return 0;
        }
        if (current->members.array && current->members.length) {
            if (!REFLECTC_NS(_for_each_impl)(current->members.array, cb, ctx))
            {
                return 0;
            }
        }
    }
    return 1;
}

int
REFLECTC_NS(_for_each)(const struct REFLECTC_PREFIX *root,
                       REFLECTC_NS(_visit_cb) cb,
                       void *ctx)
{
    if (!root || !cb) {
        return 0;
    }
    return REFLECTC_NS(_for_each_impl)(root, cb, ctx);
}

const struct REFLECTC_PREFIX *
REFLECTC_NS(_require_member)(const struct REFLECTC_PREFIX *root,
                             const char *name,
                             size_t len)
{
    size_t pos;

    if (!root || !root->members.array || !root->members.length) {
        return NULL;
    }
    if (!name || !len) {
        return NULL;
    }

    pos = REFLECTC_NS(_get_pos)(root, name, len);
    if (pos == (size_t)-1) {
        return NULL;
    }

    return root->members.array + pos;
}

int
REFLECTC_NS(_is_pointer_type)(const struct REFLECTC_PREFIX *member)
{
    size_t i;

    if (!member) {
        return 0;
    }

    if (member->decorator.buf && member->decorator.length) {
        for (i = 0; i < member->decorator.length; ++i) {
            if (member->decorator.buf[i] == '*') {
                return 1;
            }
        }
    }

    if (member->dimensions.buf && member->dimensions.length
        && strchr(member->dimensions.buf, '['))
    {
        return 1;
    }

    return 0;
}

int
REFLECTC_NS(_is_null)(const struct REFLECTC_PREFIX *member)
{
    if (!member) {
        return 1;
    }
    if (!REFLECTC_NS(_is_pointer_type)(member)) {
        return 0;
    }
    if (!member->ptr_value) {
        return 1;
    }
    return REFLECTC_NS(_resolve)(member) == NULL;
}

static int
REFLECTC_NS(_expand_all_impl)(struct REFLECTC_PREFIX *member)
{
    size_t span;
    size_t i;
    int expanded = 0;

    if (!member) return 0;

    span = member->length ? member->length : 1;
    for (i = 0; i < span; ++i) {
        struct REFLECTC_PREFIX *current = member + i;
        if (REFLECTC_NS(_expand)(current)) {
            ++expanded;
        }
        if (current->members.array && current->members.length) {
            expanded += REFLECTC_NS(_expand_all_impl)(
                (struct REFLECTC_PREFIX *)current->members.array);
        }
    }
    return expanded;
}

int
REFLECTC_NS(_expand_all)(struct REFLECTC_PREFIX *root)
{
    if (!root) return 0;
    return REFLECTC_NS(_expand_all_impl)((struct REFLECTC_PREFIX *)root);
}
