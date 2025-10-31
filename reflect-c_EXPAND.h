#if REFLECTC_PUBLIC == 1
#undef REFLECTC_PUBLIC
#define REFLECTC_PUBLIC(_container, _name, _n, _tuple)                        \
    REFLECTC_##_container(_name, _n, _tuple)
#else
#define REFLECTC_PUBLIC(_container, _name, _n, _tuple)
#endif

#if REFLECTC_PRIVATE == 1
#undef REFLECTC_PRIVATE
#define REFLECTC_PRIVATE(_container, _name, _n, _tuple)                       \
    REFLECTC_##_container(_name, _n, _tuple)
#else
#define REFLECTC_PRIVATE(_container, _name, _n, _tuple)
#endif

#ifndef REFLECTC_STRUCT
#define REFLECTC_STRUCT(_name)
#endif
#ifndef REFLECTC_STRUCT_END
#define REFLECTC_STRUCT_END(_namespace)
#endif
#ifndef REFLECTC_UNION
#define REFLECTC_UNION(_name)
#endif
#ifndef REFLECTC_UNION_END
#define REFLECTC_UNION_END(_namespace)
#endif
#ifndef REFLECTC_ENUM
#define REFLECTC_ENUM(_name)
#endif
#ifndef REFLECTC_ENUM_END
#define REFLECTC_ENUM_END(_namespace)
#endif

#ifndef RCF
#define RCF(_namespace, _qualifier, _container, _type, _decorator, _name,     \
            _dimensions, _attrs)
#endif

#ifndef RCE
#define RCE(_namespace, _enumerator, _eq, _value)
#endif

#include REFLECTC_ENTRY

#ifndef REFLECTC_INTELLISENSE_H
#undef REFLECTC_PRIVATE
#undef REFLECTC_PUBLIC
#undef REFLECTC_STRUCT
#undef REFLECTC_STRUCT_END
#undef REFLECTC_UNION
#undef REFLECTC_UNION_END
#undef REFLECTC_ENUM
#undef REFLECTC_ENUM_END
#undef RCF
#undef RCE

#undef REFLECTC_PRE_HOOKS
#undef REFLECTC_POST_HOOKS
#endif /* REFLECTC_INTELLISENSE_H */
