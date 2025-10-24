#ifdef REFLECTC_WRAPPER

#define _pick_declaration(_container, _type)                                  \
    struct REFLECTC_NS(_wrap) *REFLECTC_NS(_from_##_type)(                    \
        struct REFLECTC_PREFIX *registry,                                     \
        _container _type *self,                                               \
        struct REFLECTC_NS(_wrap) *root);

#ifdef REFLECTC_HEADER

#define REFLECTC_PUBLIC 1
#define REFLECTC_STRUCT(_type) _pick_declaration(struct, _type)
#define REFLECTC_UNION(_type) _pick_declaration(union, _type)
#include "reflect-c_EXPAND.h"


#elif defined(REFLECTC_FORWARD)

/*#!
#include <stdlib.h>
#include <string.h>
#include "reflect-c.h"

#define _
#define _is_empty(_arg) (#_arg[0] == '\0')
#define _str(_arg) { _is_empty(_arg) ? NULL : #_arg, _is_empty(_arg) ? 0 : sizeof(#_arg) - 1 }

static struct REFLECTC_NS(_wrap) *
_from_noop(struct REFLECTC_PREFIX *registry,
           void *self,
           struct REFLECTC_NS(_wrap) *root)
{
    (void)registry;
    (void)self;
    return root;
}
*/

#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) static _pick_declaration(struct, _type)
#define REFLECTC_UNION(_type) static _pick_declaration(union, _type)
#include "reflect-c_EXPAND.h"

#define _pick_table(_container, _type)                                        \
    static const struct REFLECTC_NS(_template)                                \
        _type##__member_templates[] = {
#define _pick_member__(_namespace, _qualifier, _container, _type, _decorator, \
                      _name, _dimensions, _attrs)                             \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            (enum REFLECTC_NS(_types))(REFLECTC_TYPES(_type)),                \
            (unsigned long)(_attrs), { 0, NULL }, _from_noop },
#define _pick_member_container(_namespace, _qualifier, _container, _type,     \
                              _decorator, _name, _dimensions, _attrs)         \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            (enum REFLECTC_NS(_types))(REFLECTC_TYPES(_container)),           \
            (unsigned long)(_attrs), { 0, NULL },                             \
/*#! #ifdef */ REFLECTC_DEFINED##__##_type /*#! */                            \
            (REFLECTC_NS(_from_cb))REFLECTC_NS(_from_##_type)                 \
/*#! #else */ /*#! */                                                         \
            _from_noop                                                        \
/*#! #endif */ /*#! */                                                        \
         },
#define _pick_member_struct _pick_member_container
#define _pick_member_union _pick_member_container
#define _pick_member_enum(_namespace, _qualifier, _container, _type,          \
                          _decorator, _name, _dimensions, _attrs)             \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            (enum REFLECTC_NS(_types))(REFLECTC_TYPES(_container)),           \
            (unsigned long)(_attrs), { 0, NULL }, _from_noop },
#define _pick_table_end(_container, _type)                                    \
    };                                                                        \
                                                                              \
    static const struct REFLECTC_NS(_template)                                \
        _type##__root_template = {                                            \
            sizeof(_container _type),                                         \
            { NULL, 0 },                                                      \
            { NULL, 0 },                                                      \
            { #_type, sizeof(#_type) - 1 },                                   \
            { NULL, 0 },                                                      \
            (enum REFLECTC_NS(_types))(REFLECTC_TYPES(_container)),           \
            0ul,                                                              \
            { sizeof(_type##__member_templates)                               \
                  / sizeof *_type##__member_templates,                        \
              _type##__member_templates },                                    \
            (REFLECTC_NS(_from_cb))REFLECTC_NS(_from_##_type) };

#define REFLECTC_PUBLIC  1
#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) _pick_table(struct, _type)
#define REFLECTC_UNION(_type) _pick_table(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions, _attrs)                                              \
        _pick_member_##_container(_namespace, _qualifier, _container, _type,  \
                                 _decorator, _name, _dimensions, _attrs)
#define REFLECTC_STRUCT_END(_namespace) _pick_table_end(struct, _namespace)
#define REFLECTC_UNION_END(_namespace) _pick_table_end(union, _namespace)
#include "reflect-c_EXPAND.h"
#undef _pick_table
#undef _pick_member__
#undef _pick_member_container
#undef _pick_member_struct
#undef _pick_member_union
#undef _pick_member_enum
#undef _pick_table_end

#else

static struct REFLECTC_NS(_wrap_mut) *
_reflectc_wrapper_prepare_root(struct REFLECTC_NS(_wrap_mut) *root,
                              const struct REFLECTC_NS(_template) *root_template,
                              void *self,
                              size_t elem_size,
                              size_t length)
{
    size_t i;

    if (root) return root;
    if (!(root = calloc(length, sizeof *root))) return NULL;

    for (i = 0; i < length; ++i) {
        root[i].tmpl = root_template;
        root[i].ptr_value =
            self ? (void *)((char *)self + i * elem_size) : NULL;
        root[i].length = length - i;
        root[i].members.array = NULL;
        root[i].members.length = 0;
    }

    return root;
}

static struct REFLECTC_NS(_wrap_mut) *
_reflectc_wrapper_alloc_members(size_t n_members, size_t length)
{
    size_t total;

    if (!length || !n_members) {
        return NULL;
    }
    total = length * n_members;
    return (struct REFLECTC_NS(_wrap_mut) *)calloc(
        total, sizeof(struct REFLECTC_NS(_wrap_mut)));
}

static struct REFLECTC_NS(_wrap_mut) *
_reflectc_wrapper_prepare_members(
    struct REFLECTC_NS(_wrap_mut) *root_entry,
    struct REFLECTC_NS(_wrap_mut) *members_buf,
    size_t index,
    const struct REFLECTC_NS(_template) *template_members,
    size_t n_members)
{
    struct REFLECTC_NS(_wrap_mut) *block = members_buf + index * n_members;
    size_t i;
    root_entry->members.array = block;
    root_entry->members.length = n_members;
    for (i = 0; i < n_members; ++i) {
        block[i].tmpl = template_members + i;
        block[i].ptr_value = NULL;
        block[i].length = 0;
        block[i].members.array = NULL;
        block[i].members.length = 0;
    }
    return block;
}

#define _pick_implementation(_container, _type)                               \
    struct REFLECTC_NS(_wrap) *                                               \
    REFLECTC_NS(_from_##_type)(                                               \
        struct REFLECTC_PREFIX *registry,                                     \
        _container _type *self,                                               \
        struct REFLECTC_NS(_wrap) *_root)                                     \
    {                                                                         \
        struct REFLECTC_NS(_wrap_mut) *root =                                 \
            (struct REFLECTC_NS(_wrap_mut) *)_root;                           \
        const int has = registry != NULL;                                     \
        if (has && self && !_root) {                                          \
            struct REFLECTC_NS(_wrap) *cached =                               \
                REFLECTC_NS(_find)(registry, self);                           \
            if (cached) {                                                     \
                return cached;                                                \
            }                                                                 \
        }                                                                     \
        const size_t length = _root ? REFLECTC_NS(_length)(_root) : 1;        \
        root = _reflectc_wrapper_prepare_root(root, &_type##__root_template,  \
                                              self, sizeof *self, length);    \
        if (!root) {                                                          \
            return NULL;                                                      \
        }                                                                     \
        if (self) {                                                           \
            static const size_t n_members =                                   \
                sizeof(_type##__member_templates)                             \
                / sizeof *_type##__member_templates;                          \
            struct REFLECTC_NS(_wrap_mut) *members_buf =                      \
                _reflectc_wrapper_alloc_members(n_members, length);           \
            size_t i;                                                         \
            if (!members_buf) {                                               \
                if (!_root) {                                                 \
                    free(root);                                               \
                }                                                             \
                return NULL;                                                  \
            }                                                                 \
            for (i = 0; i < length; ++i) {                                    \
                struct REFLECTC_NS(_wrap_mut) *m =                            \
                    _reflectc_wrapper_prepare_members(                        \
                        root + i, members_buf, i, _type##__member_templates,  \
                        n_members);
#define _pick_member__(_name, _type, _decorator, _dimensions, _attrs)         \
                m->ptr_value = &self->_name;                                  \
                ++m;
#define _pick_member_container(_name, _type, _decorator, _dimensions, _attrs) \
                m->ptr_value = &self->_name;                                  \
                if (REFLECTC_NS(_get_pointer_depth)(                          \
                        (struct REFLECTC_NS(_wrap) *)m) <= 2) {               \
                    void *child =                                             \
                        REFLECTC_NS(_resolve)((struct REFLECTC_NS(_wrap) *)m);\
                    if (child && m->tmpl && m->tmpl->from_cb) {               \
                        m->tmpl->from_cb(registry, child,                     \
                                          (struct REFLECTC_NS(_wrap) *)m);    \
                    }                                                         \
                }                                                             \
                ++m;
#define _pick_member_struct _pick_member_container
#define _pick_member_union _pick_member_container
#define _pick_member_enum _pick_member__
#define _pick_container_end(_namespace)                                       \
            }                                                                 \
        }                                                                     \
        if (has && self) {                                                    \
            REFLECTC_NS(_put)(registry, self,                                 \
                                       (struct REFLECTC_NS(_wrap) *)root);    \
        }                                                                     \
        return (struct REFLECTC_NS(_wrap) *)root;                             \
    }

#define REFLECTC_PUBLIC 1
#define REFLECTC_STRUCT(_type) _pick_implementation(struct, _type)
#define REFLECTC_UNION(_type) _pick_implementation(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions, _attrs)                                              \
        _pick_member_##_container(_name, _type, _decorator, _dimensions,      \
                                  _attrs)
#define REFLECTC_STRUCT_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_UNION_END(_namespace) _pick_container_end(_namespace)
#include "reflect-c_EXPAND.h"

#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) static _pick_implementation(struct, _type)
#define REFLECTC_UNION(_type) static _pick_implementation(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions, _attrs)                                              \
            _pick_member_##_container(_name, _type, _decorator, _dimensions,  \
                                      _attrs)
#define REFLECTC_STRUCT_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_UNION_END(_namespace) _pick_container_end(_namespace)
#include "reflect-c_EXPAND.h"
#undef _pick_implementation
#undef _pick_member__
#undef _pick_member_container
#undef _pick_member_struct
#undef _pick_member_union
#undef _pick_member_enum
#undef _pick_container_end

#endif /* REFLECTC_HEADER */

#undef _pick_declaration

#endif /* REFLECTC_WRAPPER */
