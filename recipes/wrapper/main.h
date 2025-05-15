#ifdef REFLECTC_WRAPPER

#define _pick_declaration(_container, _type)                                  \
    struct reflectc *reflectc_from_##_type(_container _type *self,            \
                                           size_t length,                     \
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
#include "reflect-c_internal.h"

#define _
#define _is_empty(_arg) (#_arg[0] == '\0')
#define _str(_arg) { _is_empty(_arg) ? NULL : #_arg, _is_empty(_arg) ? 0 : sizeof(#_arg) - 1 }
*/

#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) static _pick_declaration(struct, _type)
#define REFLECTC_UNION(_type) static _pick_declaration(union, _type)
#include "reflect-c_EXPAND.h"


#define _pick_table(_container, _type)                                        \
    static const struct reflectc_mut _type##__root =                          \
        { { 0 }, sizeof(_container _type), { NULL, 0 }, { NULL, 0 },          \
            { #_type, sizeof(#_type) - 1 }, { NULL, 0 },                      \
            REFLECTC_TYPES__##_container, NULL };                             \
                                                                              \
    static const struct reflectc_mut _type##__fields[] = {
#define _pick_field__(                                                        \
            _qualifier, _container, _type, _decorator, _name, _dimensions)    \
        { { 0 }, sizeof(_container _type _decorator _dimensions),             \
            _str(_qualifier), _str(_decorator), _str(_name),                  \
            _str(_dimensions), REFLECTC_TYPES__##_type, NULL },
#define _pick_field_container(                                                \
            _qualifier, _container, _type, _decorator, _name, _dimensions)    \
        { { 0 }, sizeof(_container _type _decorator _dimensions),             \
            _str(_qualifier), _str(_decorator), _str(_name),                  \
            _str(_dimensions), REFLECTC_TYPES__##_container, NULL },
#define _pick_field_struct _pick_field_container
#define _pick_field_union _pick_field_container
#define _pick_table_end                                                       \
    };

#define REFLECTC_PUBLIC  1
#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) _pick_table(struct, _type)
#define REFLECTC_UNION(_type) _pick_table(union, _type)
#define RCF(_qualifier, _container, _type, _decorator, _name, _dimensions)    \
        _pick_field_##_container(                                             \
            _qualifier, _container, _type, _decorator, _name, _dimensions)
#define REFLECTC_STRUCT_END _pick_table_end
#define REFLECTC_UNION_END _pick_table_end
#include "reflect-c_EXPAND.h"
#undef _pick_table
#undef _pick_field__
#undef _pick_field_container
#undef _pick_field_struct
#undef _pick_field_union

#else

#define _pick_implementation(_container, _type)                               \
    struct reflectc *                                                         \
    reflectc_from_##_type(                                                    \
        _container _type *self,                                               \
        size_t length,                                                        \
        struct reflectc *root)                                                \
    {                                                                         \
        static const size_t bkt_capacity =                                    \
            (sizeof(_type##__fields) / sizeof *_type##__fields) * 2;          \
        if (self != NULL) {                                                   \
            struct oa_hash_entry *bkts =                                      \
                                    malloc(                                   \
                                        sizeof *bkts * bkt_capacity * length);\
            struct reflectc *fields_arr =                                     \
                                    malloc(sizeof(_type##__fields) * length); \
            size_t i;                                                         \
            if (!root && (root = malloc(sizeof *root))) {                     \
                memcpy(root, &_type##__root, sizeof *root);                   \
                root->value = self;                                           \
            }                                                                 \
            oa_hash_init((struct oa_hash *)root, bkts, bkt_capacity);         \
            for (i = 0; i < length; ++i) {                                    \
                struct reflectc *fields = fields_arr + length * i;            \
                memcpy(fields, _type##__fields, sizeof(_type##__fields));
#define _pick_field__(_name, _type)                                           \
                oa_hash_set((struct oa_hash *)root, #_name,                   \
                    sizeof(#_name) - 1, ((fields->value = &self->_name),      \
                                            fields++));
#define _pick_field_container(_name, _type)                                   \
                oa_hash_set((struct oa_hash *)root, #_name,                   \
                    sizeof(#_name) - 1, ((fields->value = &self->_name),      \
                                            reflectc_from_##_type(            \
                                                reflectc_derefer_max(fields), \
                                                    1, fields))), ++fields;
#define _pick_field_struct _pick_field_container
#define _pick_field_union _pick_field_container
#define _pick_container_end                                                   \
            }                                                                 \
        }                                                                     \
        return (struct reflectc *)root;                                       \
    }

#define REFLECTC_PUBLIC 1
#define REFLECTC_STRUCT(_type) _pick_implementation(struct, _type)
#define REFLECTC_UNION(_type) _pick_implementation(union, _type)
#define RCF(_qualifier, _container, _type, _decorator, _name, _dimensions)    \
                _pick_field_##_container(_name, _type)
#define REFLECTC_STRUCT_END _pick_container_end
#define REFLECTC_UNION_END _pick_container_end
#include "reflect-c_EXPAND.h"

#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) static _pick_implementation(struct, _type)
#define REFLECTC_UNION(_type) static _pick_implementation(union, _type)
#define RCF(_qualifier, _container, _type, _decorator, _name, _dimensions)    \
                _pick_field_##_container(_name, _type)
#define REFLECTC_STRUCT_END _pick_container_end
#define REFLECTC_UNION_END _pick_container_end
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
