#include <stdlib.h>
#include <string.h>

#include "reflect-c.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
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

static unsigned
reflectc_pointer_levels(const struct reflectc *member)
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
reflectc_deref(const struct reflectc *member)
{
    int has_dimensions;
    unsigned pointer_levels;

    if (!member || !member->ptr_value) {
        return NULL;
    }
    has_dimensions = member->dimensions.length && member->dimensions.buf
                     && strchr(member->dimensions.buf, '[');
    pointer_levels = reflectc_pointer_levels(member);

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
reflectc_resolve(const struct reflectc *member)
{
    unsigned pointer_levels;
    const void *addr;
    void *value;

    if (!member || !member->ptr_value) {
        return NULL;
    }

    pointer_levels = reflectc_pointer_levels(member);
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
reflectc_expand(const struct reflectc *member)
{
    struct reflectc_mut *mut;
    void *child;

    if (!member || !member->from_cb) {
        return 0;
    }
    if (member->members.length) {
        return 1;
    }

    child = reflectc_resolve(member);
    if (!child) {
        return 0;
    }
    mut = (struct reflectc_mut *)member;
    mut->from_cb(child, (struct reflectc *)mut);
    return member->members.length != 0;
}

const void *
reflectc_memcpy(const struct reflectc *dest,
                const void *src,
                const size_t size)
{
    if (!dest || !src || size > dest->size) {
        return NULL;
    }
    if (!dest->ptr_value) {
        return NULL;
    }
    return memcpy((void *)reflectc_deref(dest), src, size);
}

const char *
reflectc_string(const struct reflectc *dest,
                const char src[],
                const size_t size)
{
    struct reflectc_mut *mut_dest = (struct reflectc_mut *)dest;
    unsigned depth;
    int has_dimensions;
    char *target;

    if (!dest || !src) {
        return NULL;
    }

    depth = reflectc_get_pointer_depth(dest);
    has_dimensions = dest->dimensions.length && dest->dimensions.buf
                     && strchr(dest->dimensions.buf, '[');

    if (dest->type != REFLECTC_TYPES__char && depth < 2) {
        return NULL;
    }
    if (!dest->ptr_value) {
        return NULL;
    }

    if (has_dimensions) {
        target = (char *)reflectc_deref(dest);
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
reflectc_array(const struct reflectc *root, const size_t length)
{
    struct reflectc_mut *mut_root = (struct reflectc_mut *)root;
    if (!root) return;

    if (length > reflectc_length(root)) {
        void *tmp;
        size_t i;
        if (!(tmp = realloc(mut_root, length * sizeof *mut_root))) {
            return;
        }
        mut_root = tmp;
        for (i = reflectc_length((struct reflectc *)mut_root); i < length; ++i)
        {
            memcpy(mut_root + i, &mut_root[0], sizeof *mut_root);
            mut_root[i].ptr_value = NULL;
            mut_root[i].length = length - i;
        }
    }
    mut_root->length = length;
}

static void reflectc_cleanup_member_block(struct reflectc_mut *members,
                                          size_t count);

static void
reflectc_cleanup_node(struct reflectc_mut *node)
{
    size_t span;
    size_t i;

    if (!node) {
        return;
    }

    span = node->length ? node->length : 1;
    for (i = 0; i < span; ++i) {
        struct reflectc_mut *current = node + i;

        if (!current->members.array || !current->members.length) {
            continue;
        }

        reflectc_cleanup_member_block(current->members.array,
                                      current->members.length);

        if (i == 0) {
            free(current->members.array);
        }

        current->members.array = NULL;
        current->members.length = 0;
    }
}

static void
reflectc_cleanup_member_block(struct reflectc_mut *members, size_t count)
{
    size_t i;

    if (!members) return;

    for (i = 0; i < count; ++i) {
        reflectc_cleanup_node(members + i);
    }
}

void
reflectc_cleanup(struct reflectc *member)
{
    if (!member) return;

    reflectc_cleanup_node((struct reflectc_mut *)member);
    free(member);
}

void
reflectc_cleanup_members(struct reflectc *member)
{
    if (!member) return;

    reflectc_cleanup_node((struct reflectc_mut *)member);
}

static int
reflectc_for_each_impl(const struct reflectc *member,
                       reflectc_visit_cb cb,
                       void *ctx)
{
    size_t span;
    size_t i;

    if (!member || !cb) {
        return 1;
    }

    span = member->length ? member->length : 1;
    for (i = 0; i < span; ++i) {
        const struct reflectc *current = member + i;
        if (!cb(current, ctx)) {
            return 0;
        }
        if (current->members.array && current->members.length) {
            if (!reflectc_for_each_impl(current->members.array, cb, ctx)) {
                return 0;
            }
        }
    }
    return 1;
}

int
reflectc_for_each(const struct reflectc *root, reflectc_visit_cb cb, void *ctx)
{
    if (!root || !cb) {
        return 0;
    }
    return reflectc_for_each_impl(root, cb, ctx);
}

const struct reflectc *
reflectc_require_member(const struct reflectc *root,
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

    pos = reflectc_get_pos(root, name, len);
    if (pos == (size_t)-1) {
        return NULL;
    }

    return root->members.array + pos;
}

int
reflectc_is_pointer_type(const struct reflectc *member)
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
reflectc_is_null(const struct reflectc *member)
{
    if (!member) {
        return 1;
    }
    if (!reflectc_is_pointer_type(member)) {
        return 0;
    }
    if (!member->ptr_value) {
        return 1;
    }
    return reflectc_resolve(member) == NULL;
}

static int
reflectc_expand_all_impl(struct reflectc *member)
{
    size_t span;
    size_t i;
    int expanded = 0;

    if (!member) return 0;

    span = member->length ? member->length : 1;
    for (i = 0; i < span; ++i) {
        struct reflectc *current = member + i;
        if (reflectc_expand(current)) {
            ++expanded;
        }
        if (current->members.array && current->members.length) {
            expanded += reflectc_expand_all_impl(
                (struct reflectc *)current->members.array);
        }
    }
    return expanded;
}

int
reflectc_expand_all(struct reflectc *root)
{
    if (!root) return 0;
    return reflectc_expand_all_impl((struct reflectc *)root);
}
