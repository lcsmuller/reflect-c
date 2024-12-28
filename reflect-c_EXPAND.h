#define BLANK

#ifndef REFLECTC_COND_WRITE
#   define REFLECTC_COND_WRITE(_condition)
#endif
#ifndef REFLECTC_COND_END
#   define REFLECTC_COND_END
#endif

#ifndef REFLECTC_STRUCT_public
#   define REFLECTC_STRUCT_public(_type)
#endif
#ifndef REFLECTC_STRUCT_private
#   define REFLECTC_STRUCT_private(_type)
#endif
#ifndef REFLECTC_FIELD_CUSTOM
#   define REFLECTC_FIELD_CUSTOM(_name, _type, _decor, _func, _default_value)
#endif
#ifndef REFLECTC_FIELD
#   define REFLECTC_FIELD(_name, _type, _default_value)                       \
        REFLECTC_FIELD_CUSTOM(_name, _type, BLANK, REFLECTC_##_type,          \
                             _default_value)
#endif
#ifndef REFLECTC_FIELD_STRUCT_PTR
#   define REFLECTC_FIELD_STRUCT_PTR(_name, _type, _decor)                    \
        REFLECTC_FIELD_CUSTOM(_name, _type, _decor, REFLECTC_STRUCT_PTR, NULL)
#endif
#ifndef REFLECTC_FIELD_PTR
#   define REFLECTC_FIELD_PTR(_name, _type, _decor)                           \
        REFLECTC_FIELD_CUSTOM(_name, _type, _decor, REFLECTC_PTR_##_type, NULL)
#endif
#ifndef REFLECTC_FIELD_PRINTF
#   define REFLECTC_FIELD_PRINTF(_name, _type, _printf_type, _scanf_type)
#endif
#ifndef REFLECTC_FIELD_ENUM
#   define REFLECTC_FIELD_ENUM(_name, _type) REFLECTC_FIELD(_name, int, 0)
#endif
#ifndef REFLECTC_STRUCT_END
#   define REFLECTC_STRUCT_END
#endif

#ifndef REFLECTC_LIST_public
#   define REFLECTC_LIST_public(_type)
#endif
#ifndef REFLECTC_LIST_private
#   define REFLECTC_LIST_private(_type)
#endif
#ifndef REFLECTC_ELEMENT
#   define REFLECTC_ELEMENT(_type)
#endif
#ifndef REFLECTC_ELEMENT_STRUCT
#   define REFLECTC_ELEMENT_STRUCT(_type)
#endif
#ifndef REFLECTC_ELEMENT_PTR
#   define REFLECTC_ELEMENT_PTR(_type, _decor)
#endif
#ifndef REFLECTC_LIST_END
#   define REFLECTC_LIST_END
#endif

#ifndef REFLECTC_ENUM
#   define REFLECTC_ENUM(_name)
#endif
#ifndef REFLECTC_ENUM_END
#   define REFLECTC_ENUM_END
#endif
#ifndef REFLECTC_ENUMERATOR
#   define REFLECTC_ENUMERATOR(_enumerator, _value)
#endif
#ifndef REFLECTC_ENUMERATOR_LAST
#   define REFLECTC_ENUMERATOR_LAST(_enumerator, _value)
#endif
#ifndef REFLECTC_ENUMERATOR_END
#   define REFLECTC_ENUMERATOR_END(_name)
#endif

#include REFLECTC_ENTRY

#undef REFLECTC_COND_WRITE
#undef REFLECTC_COND_END

#undef REFLECTC_STRUCT_public
#undef REFLECTC_STRUCT_private
#undef REFLECTC_FIELD
#undef REFLECTC_FIELD_STRUCT_PTR
#undef REFLECTC_FIELD_PTR
#undef REFLECTC_FIELD_CUSTOM
#undef REFLECTC_FIELD_PRINTF
#undef REFLECTC_FIELD_ENUM
#undef REFLECTC_STRUCT_END

#undef REFLECTC_LIST_public
#undef REFLECTC_LIST_private
#undef REFLECTC_ELEMENT
#undef REFLECTC_ELEMENT_STRUCT
#undef REFLECTC_ELEMENT_PTR
#undef REFLECTC_LIST_END

#undef REFLECTC_ENUM
#undef REFLECTC_ENUM_END
#undef REFLECTC_ENUMERATOR
#undef REFLECTC_ENUMERATOR_LAST
#undef REFLECTC_ENUMERATOR_END
