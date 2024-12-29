#define DEFINITIONS
#define REFLECTC_RETURN(_type) void

#ifdef REFLECTC_DEFINITIONS

/*#! #include <stddef.h> */
/*#! #include <stdbool.h> */
/*#! #include "reflect-c.h" */

#define REFLECTC_STRUCT_private(_type)                                        \
    struct _type {
#define REFLECTC_FIELD(_name, _type, _default_value)                          \
        _type _name;
#define REFLECTC_FIELD_STRUCT_PTR(_name, _type, _decor)                       \
        struct _type _decor _name;
#define REFLECTC_FIELD_PTR(_name, _type, _decor)                              \
        _type _decor _name;
#define REFLECTC_FIELD_CUSTOM(_name, __type, _decor, _func, _default_value)   \
        _type _decor _name;
#define REFLECTC_FIELD_PRINTF(_name, _type, printf_type, _scanf_type)         \
        _type _name;
#define REFLECTC_FIELD_ENUM(_name, _type)                                     \
        enum _type _name;
#define REFLECTC_STRUCT_END                                                   \
    };

#define REFLECTC_LIST_private(_type)                                          \
    struct _type {                                                            \
        int size;
#define REFLECTC_ELEMENT(_type)                                               \
        _type *array;
#define REFLECTC_ELEMENT_STRUCT(_type)                                        \
        struct _type *array;
#define REFLECTC_ELEMENT_PTR(_type, _decor)                                   \
        _type * _decor array;
#define REFLECTC_LIST_END                                                     \
        /** @private */                                                       \
        int realsize;                                                         \
    };
#define REFLECTC_LIST_public REFLECTC_LIST_private

#define REFLECTC_ENUM(_name)                                                  \
    enum _name {
#define REFLECTC_ENUMERATOR(_enumerator, _value)                              \
        _enumerator _value,
#define REFLECTC_ENUMERATOR_LAST(_enumerator, _value)                         \
        _enumerator _value
#define REFLECTC_ENUM_END                                                     \
    };

#define REFLECTC_STRUCT_public REFLECTC_STRUCT_private
#define REFLECTC_LIST_public REFLECTC_LIST_private

#include "reflect-c_EXPAND.h"

#endif /* REFLECTC_DEFINITIONS */

#undef DEFINITIONS
#undef REFLECTC_RETURN
