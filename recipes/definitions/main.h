#ifdef REFLECTC_DEFINITIONS

/*#!
#include "reflect-c.h"

#define _
*/

#define _pick_container(_container, _type)                                    \
    _container _type {
#define _pick_field(                                                          \
        _qualifier, _container, _type, _decorator, _name, _dimensions)        \
        _qualifier _container _type _decorator _name _dimensions;
#define _pick_container_end                                                   \
    };


#define REFLECTC_PUBLIC 1
#define REFLECTC_PRIVATE 1
#define REFLECTC_STRUCT(_type) _pick_container(struct, _type)
#define REFLECTC_UNION(_type) _pick_container(union, _type)
#define RCF _pick_field
#define REFLECTC_STRUCT_END _pick_container_end
#define REFLECTC_UNION_END _pick_container_end
#define REFLECTC_ENUM(_name)                                                  \
    enum _name {                                                              \
        __##_name ##_##INITIAL##__= -1
#define RCE(_enumerator, _eq, _value)                                         \
        , _enumerator _eq _value
#define REFLECTC_ENUM_END                                                     \
    };
#include "reflect-c_EXPAND.h"
#undef _pick_container
#undef _pick_field
#undef _pick_container_end

/*#!
#undef _
*/

#endif /* REFLECTC_DEFINITIONS */
