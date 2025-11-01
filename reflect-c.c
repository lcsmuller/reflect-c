#include <stdlib.h>
#include <string.h>

#include "reflect-c.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

struct _reflectc_entry {
    void *value;
    struct REFLECTC_NS(_wrap) * wrapper;
};

struct REFLECTC_PREFIX {
    struct _reflectc_entry *entries;
    size_t length;
    size_t capacity;
};

static size_t
_reflectc_find_index(const struct REFLECTC_PREFIX *table, const void *value)
{
    size_t i;

    if (!table || !value) {
        return SIZE_MAX;
    }

    for (i = 0; i < table->length; ++i) {
        if (table->entries[i].value == value) {
            return i;
        }
    }

    return SIZE_MAX;
}

struct REFLECTC_PREFIX *
REFLECTC_NS(_init)(void)
{
    return calloc(1, sizeof(struct REFLECTC_PREFIX));
}

void
REFLECTC_NS(_dispose)(struct REFLECTC_PREFIX *table)
{
    if (!table) return;
    free(table->entries);
    free(table);
}

struct REFLECTC_NS(_wrap)
    * REFLECTC_NS(_find)(const struct REFLECTC_PREFIX *table,
                         const void *value)
{
    const size_t idx = _reflectc_find_index(table, value);
    return (idx == SIZE_MAX) ? NULL : table->entries[idx].wrapper;
}

int
REFLECTC_NS(_put)(struct REFLECTC_PREFIX *table,
                  void *value,
                  struct REFLECTC_NS(_wrap) * wrapper)
{
    size_t idx;
    struct _reflectc_entry *entries;
    size_t capacity;

    if (!table || !value || !wrapper) {
        return 0;
    }

    if ((idx = _reflectc_find_index(table, value)) != SIZE_MAX) {
        table->entries[idx].wrapper = wrapper;
        return 1;
    }
    if (table->length == table->capacity) {
        capacity = table->capacity ? table->capacity * 2u : 4u;
        if (!(entries =
                  realloc(table->entries, capacity * sizeof *table->entries)))
        {
            return 0;
        }
        table->entries = entries;
        table->capacity = capacity;
    }
    table->entries[table->length].value = value;
    table->entries[table->length++].wrapper = wrapper;
    return 1;
}

void
REFLECTC_NS(_erase)(struct REFLECTC_PREFIX *table, const void *value)
{
    const size_t idx = _reflectc_find_index(table, value);
    if (idx == SIZE_MAX || !table->length) return;

    table->entries[idx] = table->entries[--table->length];
    if (table->length == 0) {
        table->capacity = 0;
        free(table->entries);
        table->entries = NULL;
    }
}

size_t
REFLECTC_NS(_length)(const struct REFLECTC_NS(_wrap) * member)
{
    struct REFLECTC_NS(_wrap_mut) *mut_member =
        (struct REFLECTC_NS(_wrap_mut) *)member;
    size_t length = 1;

    if (!member || !member->ptr_value || !member->tmpl) return 0;

    if (member->tmpl->dimensions.length) {
        const char *ptr = member->tmpl->dimensions.buf;
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
REFLECTC_NS(_get_pos)(const struct REFLECTC_NS(_wrap) * root,
                      const char *const name,
                      const size_t length)
{
    const struct REFLECTC_NS(_wrap) * m;

    if (!root || !name || !length) {
        return SIZE_MAX;
    }

    if (!root->members.array || !root->members.length) {
        REFLECTC_NS(_expand)((struct REFLECTC_PREFIX *)root->registry, root);
    }

    if (!root->members.array || !root->members.length) {
        return SIZE_MAX;
    }

    for (m = &root->members.array[0];
         m != &root->members.array[root->members.length]; ++m)
    {
        if (m->tmpl && m->tmpl->name.length == length
            && !memcmp(m->tmpl->name.buf, name, length))
        {
            return (size_t)(m - root->members.array);
        }
    }
    return SIZE_MAX;
}

unsigned
REFLECTC_NS(_get_pointer_depth)(const struct REFLECTC_NS(_wrap) * member)
{
    unsigned depth = 1;
    if (!member || !member->ptr_value || !member->tmpl) {
        return 0;
    }
    if (member->tmpl->decorator.length) {
        const char *ptr = member->tmpl->decorator.buf;
        while ((ptr = strchr(ptr++, '*'))) {
            ++depth;
            ++ptr;
        }
    }
    if (member->tmpl->dimensions.length
        && strchr(member->tmpl->dimensions.buf, '['))
    {
        ++depth;
    }
    return depth;
}

static unsigned
_reflectc_pointer_levels(const struct REFLECTC_NS(_wrap) * member)
{
    unsigned levels = 0;
    const char *ptr;

    if (!member || !member->tmpl) {
        return 0;
    }
    if (!member->tmpl->decorator.length || !member->tmpl->decorator.buf) {
        return 0;
    }

    ptr = member->tmpl->decorator.buf;
    while ((ptr = strchr(ptr, '*')) != NULL) {
        ++levels;
        ++ptr;
    }

    return levels;
}

const void *
REFLECTC_NS(_deref)(const struct REFLECTC_NS(_wrap) * member)
{
    int has_dimensions;
    unsigned pointer_levels;

    if (!member || !member->ptr_value || !member->tmpl) {
        return NULL;
    }
    has_dimensions = member->tmpl->dimensions.length
                     && member->tmpl->dimensions.buf
                     && strchr(member->tmpl->dimensions.buf, '[');
    pointer_levels = _reflectc_pointer_levels(member);

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
REFLECTC_NS(_resolve)(const struct REFLECTC_NS(_wrap) * member)
{
    unsigned pointer_levels;
    const void *addr;
    void *value;

    if (!member || !member->ptr_value) {
        return NULL;
    }

    pointer_levels = _reflectc_pointer_levels(member);
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
REFLECTC_NS(_expand)(struct REFLECTC_PREFIX *registry,
                     const struct REFLECTC_NS(_wrap) * member)
{
    struct REFLECTC_NS(_wrap_mut) *mut =
        (struct REFLECTC_NS(_wrap_mut) *)member;
    struct REFLECTC_PREFIX *ctx;
    void *child;

    if (!member || !member->tmpl || !member->tmpl->from_cb) {
        return 0;
    }
    if (member->members.length) {
        return 1;
    }

    ctx = registry ? registry : mut->registry;
    child = REFLECTC_NS(_resolve)(member);
    if (!child) {
        return 0;
    }
    member->tmpl->from_cb(ctx, child, (struct REFLECTC_NS(_wrap) *)mut);
    return member->members.length != 0;
}

const void *
REFLECTC_NS(_memcpy)(const struct REFLECTC_NS(_wrap) * dest,
                     const void *src,
                     const size_t size)
{
    void *target;

    if (!dest || !dest->tmpl || !src || size > dest->tmpl->size) {
        return NULL;
    }
    if (!dest->ptr_value) {
        return NULL;
    }
    target = (void *)REFLECTC_NS(_deref)(dest);
    if (!target) {
        return NULL;
    }
    return memcpy(target, src, size);
}

const void *
REFLECTC_NS(_get)(const struct REFLECTC_NS(_wrap) * member)
{
    return REFLECTC_NS(_deref)(member);
}

const void *
REFLECTC_NS(_set)(const struct REFLECTC_NS(_wrap) * member,
                  const void *value,
                  size_t size)
{
    return REFLECTC_NS(_memcpy)(member, value, size);
}

const void *
REFLECTC_NS(_get_member)(const struct REFLECTC_NS(_wrap) * root, size_t pos)
{
    if (!root) {
        return NULL;
    }
    if (!root->members.array || pos >= root->members.length) {
        (void)REFLECTC_NS(_expand)((struct REFLECTC_PREFIX *)root->registry,
                                   root);
    }
    if (!root->members.array || pos >= root->members.length) {
        return NULL;
    }
    return REFLECTC_NS(_deref)(root->members.array + pos);
}

const void *
REFLECTC_NS(_set_member)(const struct REFLECTC_NS(_wrap) * root,
                         size_t pos,
                         const void *value,
                         size_t size)
{
    if (!root) {
        return NULL;
    }
    if (!root->members.array || pos >= root->members.length) {
        (void)REFLECTC_NS(_expand)((struct REFLECTC_PREFIX *)root->registry,
                                   root);
    }
    if (!root->members.array || pos >= root->members.length) {
        return NULL;
    }
    return REFLECTC_NS(_memcpy)(root->members.array + pos, value, size);
}

const char *
REFLECTC_NS(_string)(const struct REFLECTC_NS(_wrap) * dest,
                     const char src[],
                     const size_t size)
{
    struct REFLECTC_NS(_wrap_mut) *mut_dest =
        (struct REFLECTC_NS(_wrap_mut) *)dest;
    unsigned depth;
    int has_dimensions;
    char *target;

    if (!dest || !dest->tmpl || !src) {
        return NULL;
    }

    depth = REFLECTC_NS(_get_pointer_depth)(dest);
    has_dimensions = dest->tmpl->dimensions.length
                     && dest->tmpl->dimensions.buf
                     && strchr(dest->tmpl->dimensions.buf, '[');

    if (dest->tmpl->type != REFLECTC_NS_UPPER(_TYPES__char) && depth < 2) {
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
REFLECTC_NS(_array)(const struct REFLECTC_NS(_wrap) * root,
                    const size_t length)
{
    struct REFLECTC_NS(_wrap_mut) *mut_root =
        (struct REFLECTC_NS(_wrap_mut) *)root;
    if (!root) return;

    if (length > REFLECTC_NS(_length)(root)) {
        void *tmp;
        size_t i;
        if (!(tmp = realloc(mut_root, length * sizeof *mut_root))) {
            return;
        }
        mut_root = tmp;
        for (i = REFLECTC_NS(_length)((struct REFLECTC_NS(_wrap) *)mut_root);
             i < length; ++i)
        {
            memcpy(mut_root + i, &mut_root[0], sizeof *mut_root);
            mut_root[i].ptr_value = NULL;
            mut_root[i].length = length - i;
        }
    }
    mut_root->length = length;
}

static void _reflectc_cleanup_member_block(struct REFLECTC_NS(_wrap_mut)
                                               * members,
                                           size_t count);

static void _reflectc_cleanup_node(struct REFLECTC_NS(_wrap_mut) * node)
{
    size_t span;
    size_t i;

    if (!node) {
        return;
    }

    span = node->length ? node->length : 1;
    for (i = 0; i < span; ++i) {
        struct REFLECTC_NS(_wrap_mut) *current = node + i;

        if (!current->members.array || !current->members.length) {
            continue;
        }

        _reflectc_cleanup_member_block(current->members.array,
                                       current->members.length);

        if (i == 0) {
            free(current->members.array);
        }

        current->members.array = NULL;
        current->members.length = 0;
    }
}

static void
_reflectc_cleanup_member_block(struct REFLECTC_NS(_wrap_mut) * members,
                               size_t count)
{
    size_t i;

    if (!members) return;

    for (i = 0; i < count; ++i) {
        _reflectc_cleanup_node(members + i);
    }
}

void
REFLECTC_NS(_cleanup)(struct REFLECTC_PREFIX *registry,
                      struct REFLECTC_NS(_wrap) * member)
{
    if (!member) return;

    if (registry && member->ptr_value) {
        REFLECTC_NS(_erase)(registry, member->ptr_value);
    }
    _reflectc_cleanup_node((struct REFLECTC_NS(_wrap_mut) *)member);
    free(member);
}

void
REFLECTC_NS(_cleanup_members)(struct REFLECTC_PREFIX *registry,
                              struct REFLECTC_NS(_wrap) * member)
{
    if (!member) return;

    (void)registry;
    _reflectc_cleanup_node((struct REFLECTC_NS(_wrap_mut) *)member);
}

static int
_reflectc_for_each_impl(const struct REFLECTC_NS(_wrap) * member,
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
        const struct REFLECTC_NS(_wrap) *current = member + i;
        if (!cb(current, ctx)) {
            return 0;
        }
        REFLECTC_NS(_expand)((struct REFLECTC_PREFIX *)current->registry,
                             current);
        if (current->members.array && current->members.length) {
            if (!_reflectc_for_each_impl(current->members.array, cb, ctx)) {
                return 0;
            }
        }
    }
    return 1;
}

int
REFLECTC_NS(_for_each)(const struct REFLECTC_NS(_wrap) * root,
                       REFLECTC_NS(_visit_cb) cb,
                       void *ctx)
{
    return (!root || !cb) ? 0 : _reflectc_for_each_impl(root, cb, ctx);
}

const struct REFLECTC_NS(_wrap)
    * REFLECTC_NS(_require_member)(const struct REFLECTC_NS(_wrap) * root,
                                   const char *name,
                                   size_t len)
{
    size_t pos;
    const struct REFLECTC_NS(_wrap) * member;

    if (!root || !name || !len) {
        return NULL;
    }
    if (!root->members.array || !root->members.length) {
        REFLECTC_NS(_expand)((struct REFLECTC_PREFIX *)root->registry, root);
    }

    pos = REFLECTC_NS(_get_pos)(root, name, len);
    if (pos == (size_t)-1) {
        return NULL;
    }

    if (!root->members.array || pos >= root->members.length) {
        return NULL;
    }

    member = root->members.array + pos;
    REFLECTC_NS(_expand)((struct REFLECTC_PREFIX *)member->registry, member);
    return member;
}

int
REFLECTC_NS(_is_pointer_type)(const struct REFLECTC_NS(_wrap) * member)
{
    size_t i;

    if (!member || !member->tmpl) {
        return 0;
    }

    if (member->tmpl->decorator.buf && member->tmpl->decorator.length) {
        for (i = 0; i < member->tmpl->decorator.length; ++i) {
            if (member->tmpl->decorator.buf[i] == '*') {
                return 1;
            }
        }
    }

    if (member->tmpl->dimensions.buf && member->tmpl->dimensions.length
        && strchr(member->tmpl->dimensions.buf, '['))
    {
        return 1;
    }

    return 0;
}

int
REFLECTC_NS(_is_null)(const struct REFLECTC_NS(_wrap) * member)
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
_reflectc_expand_all_impl(struct REFLECTC_PREFIX *registry,
                          struct REFLECTC_NS(_wrap) * member,
                          size_t count)
{
    size_t i;
    int expanded = 0;

    if (!member || !count) return 0;

    for (i = 0; i < count; ++i) {
        struct REFLECTC_NS(_wrap) *current = member + i;
        if (REFLECTC_NS(_expand)(registry, current)) {
            ++expanded;
        }
        if (current->members.array && current->members.length) {
            expanded += _reflectc_expand_all_impl(
                registry, (struct REFLECTC_NS(_wrap) *)current->members.array,
                current->members.length);
        }
    }
    return expanded;
}

int
REFLECTC_NS(_expand_all)(struct REFLECTC_PREFIX *registry,
                         struct REFLECTC_NS(_wrap) * root)
{
    size_t count;
    if (!root) return 0;
    count = root->length ? root->length : 1;
    return _reflectc_expand_all_impl(registry, root, count);
}
