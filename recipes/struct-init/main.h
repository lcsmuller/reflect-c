#define INIT
#define REFLECTC_RETURN(_type) void

#ifdef REFLECTC_STRUCT_INIT
#ifdef REFLECTC_HEADER

#define REFLECTC_STRUCT_public(_type)                                         \
    void _type##_init(struct _type *self);                                    \
    void _type##_cleanup(struct _type *self);
#define REFLECTC_LIST_public(_type)                                           \
    void _type##_cleanup(struct _type *self);

#include "reflect-c_EXPAND.h"

#elif defined(REFLECTC_FORWARD)

/*#! #include <stdio.h> */
/*#! #include <stdlib.h> */
/*#! #include <string.h> */

#define REFLECTC_STRUCT_private(_type)                                        \
    static void _type##_init(struct _type *self);                             \
    static void _type##_cleanup(struct _type *self);
#define REFLECTC_LIST_private(_type)                                          \
    static void _type##_cleanup(struct _type *self);

#include "reflect-c_EXPAND.h"

#else

#define REFLECTC_STRUCT_public(_type)                                         \
    void _type##_init(struct _type *self)                                     \
    {
#define REFLECTC_FIELD_CUSTOM(_name, _type, _decor, _func, _default_value)    \
        self->_name = _default_value;
#define REFLECTC_FIELD_PRINTF(_name, _type, printf_type, _scanf_type)         \
        self->_name = (_type)0;
#define REFLECTC_STRUCT_END                                                   \
    }
#define REFLECTC_STRUCT_private static REFLECTC_STRUCT_public

#include "reflect-c_EXPAND.h"

#define REFLECTC_STRUCT_public(_type)                                         \
    void _type##_cleanup(struct _type *self)                                  \
    {
#define REFLECTC_FIELD(_name, _type, _default_value)
#define REFLECTC_FIELD_STRUCT_PTR(_name, _type, _decor)                       \
        if (self->_name) {                                                    \
            _type##_cleanup(self->_name);                                     \
            free(self->_name);                                                \
        }
#define REFLECTC_FIELD_PTR(_name, _type, _decor)                              \
        if (self->_name) free(self->_name);
#define REFLECTC_STRUCT_END                                                   \
    }
#define REFLECTC_STRUCT_private static REFLECTC_STRUCT_public

#define REFLECTC_LIST_public(_type)                                           \
    void _type##_cleanup(struct _type *self)                                  \
    {                                                                         \
        while (--self->size >= 0)
#define REFLECTC_ELEMENT_STRUCT(_type)                                        \
            _type##_cleanup(self->array[self->size]);
#define REFLECTC_ELEMENT_PTR(_type, _decor)                                   \
            free(self->array[self->size]);
#define REFLECTC_LIST_END                                                     \
        free(self->array);                                                    \
    }
#define REFLECTC_LIST_private static REFLECTC_LIST_public

#include "reflect-c_EXPAND.h"

#endif /* REFLECTC_HEADER */
#endif /* REFLECTC_STRUCT_INIT */

#undef STRUCT_INIT
#undef REFLECTC_RETURN
