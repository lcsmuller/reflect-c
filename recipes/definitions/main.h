#ifdef REFLECTC_DEFINITIONS

/*#!
#include "reflect-c.h"

#define _
*/

#define _pick_container(_container, _type)                                    \
    _container _type {
#define _pick_field(_namespace, _qualifier, _container, _type, _decorator,    \
                    _name, _dimensions)                                       \
    _qualifier _container _type _decorator _name _dimensions;
#define _pick_container_end(_namespace)                                       \
    };
#define REFLECTC_PUBLIC 1
#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) _pick_container(struct, _type)
#define REFLECTC_UNION(_type) _pick_container(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
             _dimensions)                                                     \
    _pick_field(_namespace, _qualifier, _container, _type, _decorator,        \
                _name, _dimensions)
#define REFLECTC_STRUCT_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_UNION_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_ENUM(_name)                                                  \
    enum _name {
#define RCE(_namespace, _enumerator, _eq, _value)                             \
        _enumerator _eq _value,
#define REFLECTC_ENUM_END(_namespace)                                         \
        __##_namespace##_MAX__                                                \
    };
#include "reflect-c_EXPAND.h"
#undef _pick_container
#undef _pick_field
#undef _pick_container_end

#define _pick_container(_container, _type)                                    \
    enum {
#define _pick_field(_namespace, _qualifier, _container, _type, _decorator,    \
                    _name, _dimensions)                                       \
        __REFLECTC_LOOKUP__##_namespace##_##_name##__,
#define _pick_container_end(_namespace)                                       \
        __REFLECTC_LOOKUP__##_namespace##_MAX__                               \
    };
#define REFLECTC_PUBLIC 1
#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) _pick_container(struct, _type)
#define REFLECTC_UNION(_type) _pick_container(union, _type)
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
             _dimensions)                                                     \
    _pick_field(_namespace, _qualifier, _container, _type, _decorator, _name, \
                _dimensions)
#define REFLECTC_STRUCT_END(_namespace) _pick_container_end(_namespace)
#define REFLECTC_UNION_END(_namespace) _pick_container_end(_namespace)
#include "reflect-c_EXPAND.h"
#undef _pick_container
#undef _pick_field
#undef _pick_container_end

/*#!
#undef _
*/

#endif /* REFLECTC_DEFINITIONS */
