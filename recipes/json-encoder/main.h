#define JSON_ENCODER
#define COGCHEF_RETURN(_type) void

#define COGCHEF_int(b, buf, size, _var, _type)                                \
    if (0 > (code = jsonb_number(b, buf, size, _var))) return code
#define COGCHEF_PTR_char(b, buf, size, _var, _type)                           \
    if (0 > (code = jsonb_string(b, buf, size, _var,                          \
                                 _var ? strlen(_var) : 0)))                   \
        return code
#define COGCHEF_bool(b, buf, size, _var, _type)                               \
    if (0 > (code = jsonb_bool(b, buf, size, _var))) return code
#define COGCHEF_STRUCT_PTR(b, buf, size, _var, _type)                         \
    if (0 > (code = _type##_to_jsonb(b, buf, size, _var))) return code

#ifdef COGCHEF_JSON_ENCODER
#ifdef COGCHEF_HEADER

/*#! #define JSONB_HEADER */
/*#! #include "submodules/json-build/json-build.h" */

#define COGCHEF_STRUCT_public(_type)                                          \
    jsonbcode _type##_to_jsonb(jsonb *b, char buf[], size_t size,             \
                              const struct _type *self);                      \
    size_t _type##_to_json(char buf[], size_t size, const struct _type *self);
#define COGCHEF_LIST_public COGCHEF_STRUCT_public

#include "cogchef-assemble.ACTION.h"

#elif defined(COGCHEF_FORWARD)

#define COGCHEF_STRUCT_private(_type)                                         \
    static jsonbcode _type##_to_jsonb(jsonb *b, char buf[], size_t size,      \
                                         const struct _type *self);
#define COGCHEF_LIST_private COGCHEF_STRUCT_private

#include "cogchef-assemble.ACTION.h"

#else

#define COGCHEF_COND_WRITE(_condition)                                        \
    if (_condition) {
#define COGCHEF_COND_END                                                      \
    }

#define COGCHEF_STRUCT_public(_type)                                          \
    jsonbcode _type##_to_jsonb(jsonb *b, char buf[], size_t size,             \
                                         const struct _type *self)            \
    {                                                                         \
        jsonbcode code;                                                       \
        if (0 > (code = jsonb_object(b, buf, size))) return code;             \
        if (self != NULL) {
#define COGCHEF_FIELD_CUSTOM(_name, __type, _decor, _encoder, _default_value) \
        if (0 > (code = jsonb_key(b, buf, size, #_name, sizeof(#_name) - 1))) \
            return code;                                                      \
        _encoder(b, buf, size, self->_name, _type);
#define COGCHEF_FIELD_PRINTF(_name, _type, _printf_type, _scanf_type)         \
        if (0 > (code = jsonb_key(b, buf, size, #_name, sizeof(#_name) - 1))) \
            return code;                                                      \
        else {                                                                \
            char tok[64];                                                     \
            int toklen = sprintf(tok, _printf_type, self->_name);             \
            if (0 > (code = jsonb_token(b, buf, size, tok, toklen)))          \
                return code;                                                  \
        }
#define COGCHEF_STRUCT_END                                                    \
        }                                                                     \
        if (0 > (code = jsonb_object_pop(b, buf, size))) return code;         \
        return code;                                                          \
    }
#define COGCHEF_STRUCT_private static COGCHEF_STRUCT_public

#define COGCHEF_LIST_public(_type)                                            \
    jsonbcode _type##_to_jsonb(jsonb *b, char buf[], size_t size,             \
                                         const struct _type *self)            \
    {                                                                         \
        jsonbcode code;                                                       \
        if (0 > (code = jsonb_array(b, buf, size))) return code;              \
        if (self != NULL) {                                                   \
            int i;
#define COGCHEF_ELEMENT(_type)                                                \
        for (i = 0; i < self->size; ++i)                                      \
            COGCHEF_##_type(b, buf, size, self->array[i], _type);
#define COGCHEF_ELEMENT_STRUCT(_type)                                         \
        for (i = 0; i < self->size; ++i)                                      \
            if (0 > (code = _type##_to_jsonb(b, buf, size, &self->array[i]))) \
                return code;
#define COGCHEF_ELEMENT_PTR(_type, _decor)                                    \
        for (i = 0; i < self->size; ++i)                                      \
            COGCHEF_PTR_##_type(b, buf, size, self->array[i], _type);
#define COGCHEF_LIST_END                                                      \
        }                                                                     \
        if (0 > (code = jsonb_array_pop(b, buf, size))) return code;          \
        return code;                                                          \
    }
#define COGCHEF_LIST_private static COGCHEF_LIST_public

#include "cogchef-assemble.ACTION.h"

#define COGCHEF_STRUCT_public(_type)                                          \
    size_t _type##_to_json(char buf[], size_t size,                           \
                              const struct _type *self)                       \
    {                                                                         \
        jsonb b;                                                              \
        jsonbcode code;                                                       \
        jsonb_init(&b);                                                       \
        code = _type##_to_jsonb(&b, buf, size, self);                         \
        return code < 0 ? 0 : b.pos;                                          \
    }

#include "cogchef-assemble.ACTION.h"

#endif /* COGCHEF_HEADER */
#endif /* COGCHEF_JSON_ENCODER */

#undef JSON_ENCODER
#undef COGCHEF_RETURN

#undef COGCHEF_int
#undef COGCHEF_PTR_char
#undef COGCHEF_bool
#undef COGCHEF_STRUCT_PTR
