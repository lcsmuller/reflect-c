#ifdef REFLECTC_WRAPPER

#define _pick_declaration(_container, _type)                                  \
    struct reflectc *reflectc_from_##_type(_container _type *self,            \
                                           struct reflectc *root);

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

static struct reflectc*
_from_noop(void *self, struct reflectc *root)
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
    static const struct reflectc _type##__fields[] = {
#define _pick_field__(_namespace, _qualifier, _container, _type, _decorator,  \
                      _name, _dimensions)                                     \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            REFLECTC_TYPES__##_type, 0, NULL, { 0, NULL }, _from_noop },
#define _pick_field_container(_namespace, _qualifier, _container, _type,      \
                              _decorator, _name, _dimensions)                 \
        { sizeof(_container _type _decorator _dimensions), _str(_qualifier),  \
            _str(_decorator), _str(_name), _str(_dimensions),                 \
            REFLECTC_TYPES__##_container, 0, NULL, { 0, NULL },               \
/*#! #ifdef */ REFLECTC_DEFINED##__##_type /*#! */                            \
            (reflectc_from_cb)reflectc_from_##_type                           \
/*#! #else */ /*#! */                                                         \
            _from_noop                                                        \
/*#! #endif */ /*#! */                                                        \
         },
#define _pick_field_struct _pick_field_container
#define _pick_field_union _pick_field_container
#define _pick_table_end(_container, _type)                                    \
    };                                                                        \
                                                                              \
    static const struct reflectc _type##__root =                              \
        { sizeof(_container _type), { NULL, 0 }, { NULL, 0 },                 \
            { #_type, sizeof(#_type) - 1 }, { NULL, 0 },                      \
            REFLECTC_TYPES__##_container, 0, NULL, { 0, NULL },               \
            (reflectc_from_cb)reflectc_from_##_type };

#define REFLECTC_PUBLIC  1
#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) _pick_table(struct, _type)
#define REFLECTC_UNION(_type) _pick_table(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions)                                                      \
        _pick_field_##_container(_namespace, _qualifier, _container, _type,   \
                                 _decorator, _name, _dimensions)
#define REFLECTC_STRUCT_END(_namespace) _pick_table_end(struct, _namespace)
#define REFLECTC_UNION_END(_namespace) _pick_table_end(union, _namespace)
#include "reflect-c_EXPAND.h"
#undef _pick_table
#undef _pick_field__
#undef _pick_field_container
#undef _pick_field_struct
#undef _pick_field_union
#undef _pick_table_end

#else

#define _pick_implementation(_container, _type)                               \
    struct reflectc *                                                         \
    reflectc_from_##_type(_container _type *self, struct reflectc *_root)     \
    {                                                                         \
        struct reflectc_mut *root = (struct reflectc_mut *)_root;             \
        const size_t length = _root ? reflectc_length(_root) : 1;             \
        if (!root) {                                                          \
            size_t i;                                                         \
            if (!(root = calloc(length, sizeof *root))) {                     \
                return NULL;                                                  \
            }                                                                 \
            for (i = 0; i < length; ++i) {                                    \
                memcpy(root + i, &_type##__root, sizeof *root);               \
                root[i].ptr_value = self + i;                                 \
                root[i].length = length - i;                                  \
            }                                                                 \
        }                                                                     \
        if (self) {                                                           \
            static const size_t n_fields =                                    \
                sizeof(_type##__fields) / sizeof *_type##__fields;            \
            struct reflectc_mut *fields_buf = malloc(sizeof(_type##__fields)  \
                                                     * length);               \
            size_t i;                                                         \
            for (i = 0; i < length; ++i) {                                    \
                struct reflectc_mut *f = fields_buf + i * n_fields;           \
                root[i].fields.array = f;                                     \
                root[i].fields.len = n_fields;                                \
                memcpy(f, _type##__fields, sizeof(_type##__fields));
#define _pick_field__(_name, _type, _decorator, _dimensions)                  \
                f->ptr_value = &self->_name;                                  \
                ++f;
#define _pick_field_container(_name, _type, _decorator, _dimensions)          \
                f->ptr_value = &self->_name;                                  \
                if (reflectc_get_pointer_depth((struct reflectc *)f) <= 2) {  \
                    f->from_cb((void *)reflectc_deref((struct reflectc *)f),  \
                               (struct reflectc *)f);                         \
                }                                                             \
                ++f;
#define _pick_field_struct _pick_field_container
#define _pick_field_union _pick_field_container
#define _pick_container_end(_namespace)                                       \
            }                                                                 \
        }                                                                     \
        return (struct reflectc *)root;                                       \
    }

#define REFLECTC_PUBLIC 1
#define REFLECTC_STRUCT(_type) _pick_implementation(struct, _type)
#define REFLECTC_UNION(_type) _pick_implementation(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions)                                                      \
            _pick_field_##_container(_name, _type, _decorator, _dimensions)
#define REFLECTC_STRUCT_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_UNION_END(_namespace) _pick_container_end(_namespace)
#include "reflect-c_EXPAND.h"

#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) static _pick_implementation(struct, _type)
#define REFLECTC_UNION(_type) static _pick_implementation(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions)                                                      \
            _pick_field_##_container(_name, _type, _decorator, _dimensions)
#define REFLECTC_STRUCT_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_UNION_END(_namespace) _pick_container_end(_namespace)
#include "reflect-c_EXPAND.h"
#undef _pick_implementation
#undef _pick_field__
#undef _pick_field_container
#undef _pick_field_struct
#undef _pick_field_union
#undef _pick_container_end

#endif /* REFLECTC_HEADER */

#undef _pick_declaration

#endif /* REFLECTC_WRAPPER */
