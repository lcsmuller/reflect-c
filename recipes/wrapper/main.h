#ifdef REFLECTC_WRAPPER

#define _pick_declaration(_container, _type)                                  \
    struct REFLECTC_PREFIX *REFLECTC_NS(_from_##_type)(                       \
        _container _type *self, struct REFLECTC_PREFIX *root);

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

static struct REFLECTC_PREFIX*
_from_noop(void *self, struct REFLECTC_PREFIX *root)
{
    (void)self;
    return root;
}
*/

#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) static _pick_declaration(struct, _type)
#define REFLECTC_UNION(_type) static _pick_declaration(union, _type)
#include "reflect-c_EXPAND.h"

#define _pick_table(_container, _type)                                        \
    static const struct REFLECTC_PREFIX _type##__members[] = {
#define _pick_member__(_namespace, _qualifier, _container, _type, _decorator, \
                      _name, _dimensions, _attrs)                             \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            (enum REFLECTC_NS(_types))(REFLECTC_NS_UPPER(_TYPES__##_type)),   \
            (unsigned long)(_attrs), 0, NULL, { 0, NULL }, _from_noop },
#define _pick_member_container(_namespace, _qualifier, _container, _type,     \
                              _decorator, _name, _dimensions, _attrs)         \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            (enum REFLECTC_NS(_types))(REFLECTC_NS_UPPER(                     \
                                        _TYPES__##_container)),               \
            (unsigned long)(_attrs), 0, NULL, { 0, NULL },                    \
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
            (enum REFLECTC_NS(_types))(REFLECTC_NS_UPPER(                     \
                                        _TYPES__##_container)),               \
            (unsigned long)(_attrs), 0, NULL, { 0, NULL }, _from_noop },
#define _pick_table_end(_container, _type)                                    \
    };                                                                        \
                                                                              \
    static const struct REFLECTC_PREFIX _type##__root =                       \
        { sizeof(_container _type), { NULL, 0 }, { NULL, 0 },                 \
            { #_type, sizeof(#_type) - 1 }, { NULL, 0 },                      \
            (enum REFLECTC_NS(_types))(REFLECTC_NS_UPPER(                     \
                _TYPES__##_container)), 0ul, 0, NULL, { 0, NULL },            \
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

static struct REFLECTC_NS(_mut) *
REFLECTC_NS(_wrapper_prepare_root)(struct REFLECTC_NS(_mut) *root,
                              const struct REFLECTC_PREFIX *root_template,
                              void *self,
                              size_t elem_size,
                              size_t length)
{
    size_t i;

    if (root) {
        return root;
    }

    root = calloc(length, sizeof *root);
    if (!root) {
        return NULL;
    }

    for (i = 0; i < length; ++i) {
        memcpy(root + i, root_template, sizeof *root);
        root[i].ptr_value =
            self ? (void *)((char *)self + i * elem_size) : NULL;
        root[i].length = length - i;
    }

    return root;
}

static struct REFLECTC_NS(_mut) *
REFLECTC_NS(_wrapper_alloc_members)(size_t members_size, size_t length)
{
    if (!length || !members_size) {
        return NULL;
    }
    return malloc(members_size * length);
}

static struct REFLECTC_NS(_mut) *
REFLECTC_NS(_wrapper_prepare_members)(struct REFLECTC_NS(_mut) *root_entry,
                                 struct REFLECTC_NS(_mut) *members_buf,
                                 size_t index,
                                 const struct REFLECTC_PREFIX *template_members,
                                 size_t members_size,
                                 size_t n_members)
{
    struct REFLECTC_NS(_mut) *block = members_buf + index * n_members;
    root_entry->members.array = block;
    root_entry->members.length = n_members;
    memcpy(block, template_members, members_size);
    return block;
}

#define _pick_implementation(_container, _type)                               \
    struct REFLECTC_PREFIX *                                                  \
    REFLECTC_NS(_from_##_type)(                                               \
        _container _type *self, struct REFLECTC_PREFIX *_root)                \
    {                                                                         \
        struct REFLECTC_NS(_mut) *root = (struct REFLECTC_NS(_mut) *)_root;   \
        const size_t length = _root ? REFLECTC_NS(_length)(_root) : 1;        \
        root = REFLECTC_NS(_wrapper_prepare_root)(root, &_type##__root, self, \
                                             sizeof *self, length);           \
        if (!root) {                                                          \
            return NULL;                                                      \
        }                                                                     \
        if (self) {                                                           \
            static const size_t n_members =                                   \
                sizeof(_type##__members) / sizeof *_type##__members;          \
            struct REFLECTC_NS(_mut) *members_buf =                           \
                REFLECTC_NS(_wrapper_alloc_members)(sizeof(_type##__members), \
                                               length);                       \
            size_t i;                                                         \
            if (!members_buf) {                                               \
                if (!_root) {                                                 \
                    free(root);                                               \
                }                                                             \
                return NULL;                                                  \
            }                                                                 \
            for (i = 0; i < length; ++i) {                                    \
                struct REFLECTC_NS(_mut) *m =                                 \
                    REFLECTC_NS(_wrapper_prepare_members)(root + i,           \
                                                          members_buf,        \
                                                          i, _type##__members,\
                                                          sizeof(             \
                                                            _type##__members),\
                                                          n_members);
#define _pick_member__(_name, _type, _decorator, _dimensions, _attrs)         \
                m->ptr_value = &self->_name;                                  \
                ++m;
#define _pick_member_container(_name, _type, _decorator, _dimensions, _attrs) \
                m->ptr_value = &self->_name;                                  \
                if (REFLECTC_NS(_get_pointer_depth)(                          \
                        (struct REFLECTC_PREFIX *)m) <= 2) {                  \
                    void *child =                                             \
                        REFLECTC_NS(_resolve)((struct REFLECTC_PREFIX *)m);   \
                    if (child) {                                              \
                        m->from_cb(child, (struct REFLECTC_PREFIX *)m);       \
                    }                                                         \
                }                                                             \
                ++m;
#define _pick_member_struct _pick_member_container
#define _pick_member_union _pick_member_container
#define _pick_member_enum _pick_member__
#define _pick_container_end(_namespace)                                       \
            }                                                                 \
        }                                                                     \
        return (struct REFLECTC_PREFIX *)root;                                \
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
            _pick_member_##_container(_name, _type, _decorator, _dimensions, \
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
