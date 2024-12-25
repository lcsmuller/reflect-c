#define JSON_DECODER
#define COGCHEF_RETURN(_type) _type

#define COGCHEF_int(_f, _js, _var, _type)                                     \
    if (_f && _f->type == JSMN_PRIMITIVE)                                     \
        _var = (int)strtol(_js + _f->v.pos, NULL, 10)
#define COGCHEF_bool(_f, _js, _var, _type)                                    \
    if (_f && _f->type == JSMN_PRIMITIVE)                                     \
        _var = ('t' == _js[_f->v.pos])
#define COGCHEF_PTR_char(_f, _js, _var, _type)                                \
    if (_f && _f->type == JSMN_STRING) {                                      \
        long _ret;                                                            \
        _var = calloc(1, _f->v.len + 1);                                      \
        if (NULL == _var) return JSMN_ERROR_NOMEM;                            \
        _ret = jsmnf_unescape(_var, _f->v.len, _js + _f->v.pos, _f->v.len);   \
        if (_ret < 0) return _ret;                                            \
        ret += _ret;                                                          \
    }
#define COGCHEF_STRUCT_PTR(_f, _js, _var, _type)                              \
    if (_f && (_f->type == JSMN_OBJECT || _f->type == JSMN_ARRAY)) {          \
        long _ret;                                                            \
        _var = calloc(1, sizeof *_var);                                       \
        if (NULL == _var) return JSMN_ERROR_NOMEM;                            \
        _ret = _type##_from_jsmnf(_f, _js, _var);                             \
        if (_ret < 0) return _ret;                                            \
        ret += sizeof *_var + _ret;                                           \
    }

#ifdef COGCHEF_JSON_DECODER
#ifdef COGCHEF_HEADER

/*#! #define JSMN_STRICT */
/*#! #define JSMN_HEADER */
/*#! #include "submodules/jsmn/jsmn.h" */
/*#! #include "submodules/jsmn-find/jsmn-find.h" */

#define COGCHEF_STRUCT_public(_type)                                          \
    long _type##_from_jsmnf(jsmnf_pair *root, const char *js,                 \
                            struct _type *self);                              \
    size_t _type##_from_json(const char buf[], size_t size,                   \
                             struct _type *self);
#define COGCHEF_LIST_public COGCHEF_STRUCT_public

#include "cogchef-assemble.ACTION.h"

#elif defined(COGCHEF_FORWARD)

#define COGCHEF_STRUCT_private(_type)                                         \
    static long _type##_from_jsmnf(jsmnf_pair *root, const char *js,          \
                                   struct _type *self);
#define COGCHEF_LIST_private COGCHEF_STRUCT_private

#include "cogchef-assemble.ACTION.h"

static char *
_cc_strndup(const char *src, size_t len)
{
    char *dest = malloc(len + 1);
    if (!dest) return NULL;
    memcpy(dest, src, len);
    dest[len] = '\0';
    return dest;
}

#else

#define COGCHEF_STRUCT_public(_type)                                          \
    long _type##_from_jsmnf(jsmnf_pair *root, const char *js,                 \
                            struct _type *self)                               \
    {                                                                         \
        jsmnf_pair *f;                                                        \
        long ret = 0;
#define COGCHEF_FIELD_CUSTOM(_name, _type, _decor, _decoder, _default_value)  \
        f = jsmnf_find(root, js, #_name, sizeof(#_name) - 1);                 \
        _decoder(f, js, self->_name, _type);
#define COGCHEF_FIELD_PRINTF(_name, _type, _printf_type, _scanf_type)         \
        f = jsmnf_find(root, js, #_name, sizeof(#_name) - 1);                 \
        if (f) sscanf(js + f->v.pos, _scanf_type, &self->_name);
#define COGCHEF_STRUCT_END                                                    \
        return ret;                                                           \
    }
#define COGCHEF_STRUCT_private static COGCHEF_STRUCT_public

#define ARRAY_INSERT(i, value)                                                \
    if (self->size == self->realsize) {                                       \
        void *tmp;                                                            \
        self->realsize = 1 + self->realsize * 2;                              \
        tmp = realloc(self->array, sizeof *self->array * self->realsize);     \
        if (!tmp) return NULL;                                                \
        self->array = tmp;                                                    \
    }                                                                         \
    memmove(self->array + i + 1, self->array + i,                             \
            sizeof *self->array * (self->size - i));                          \
    self->array[i] = value;                                                   \
    self->size++

#define COGCHEF_LIST_public(_type)                                            \
    long _type##_from_jsmnf(jsmnf_pair *root, const char *js,                 \
                            struct _type *self)                               \
    {                                                                         \
        long ret = sizeof *self * root->size;                                 \
        int i;                                                                \
        if (!ret) return 0;                                                   \
        self->array = calloc((self->realsize = root->size), sizeof *self);    \
        if (!self->array) return 0;                                           \
        self->size = 0;                                                       \
        for (i = 0; i < root->size; ++i) {                                    \
            jsmnf_pair *f = root->fields + i;
#define COGCHEF_ELEMENT(_type)                                                \
            _type o;                                                          \
            COGCHEF_##_type(f, js, o, _type);
#define COGCHEF_ELEMENT_STRUCT(_type)                                         \
            struct _type o = { 0 };                                           \
            long _ret = _type##_from_jsmnf(f, js, &o);                        \
            if (_ret < 0) return _ret;                                        \
            ret += _ret;
#define COGCHEF_ELEMENT_PTR(_type, _decor)                                    \
            _type *o;                                                         \
            COGCHEF_PTR_##_type(f, js, o, _type);
#define COGCHEF_LIST_END                                                      \
            ARRAY_INSERT(i, o);                                               \
        }                                                                     \
        return ret;                                                           \
    }
#define COGCHEF_LIST_private static COGCHEF_LIST_public

#include "cogchef-assemble.ACTION.h"

#define COGCHEF_STRUCT_public(_type)                                          \
    size_t _type##_from_json(const char buf[], size_t size,                   \
                             struct _type *self)                              \
    {                                                                         \
        size_t nbytes = 0;                                                    \
        jsmn_parser parser;                                                   \
        jsmntok_t *tokens = NULL;                                             \
        unsigned tmp = 0;                                                     \
        jsmn_init(&parser);                                                   \
        if (0 < jsmn_parse_auto(&parser, buf, size, &tokens, &tmp)) {         \
            jsmnf_loader loader;                                              \
            jsmnf_pair *pairs = NULL;                                         \
            tmp = 0;                                                          \
            jsmnf_init(&loader);                                              \
            if (0 < jsmnf_load_auto(&loader, buf, tokens, parser.toknext,     \
                                    &pairs, &tmp)) {                          \
                long ret;                                                     \
                if (0 < (ret = _type##_from_jsmnf(pairs, buf, self)))         \
                    nbytes = ret;                                             \
                free(pairs);                                                  \
            }                                                                 \
            free(tokens);                                                     \
        }                                                                     \
        return nbytes;                                                        \
    }
#define COGCHEF_LIST_public COGCHEF_STRUCT_public

#include "cogchef-assemble.ACTION.h"

#endif /* COGCHEF_HEADER */
#endif /* COGCHEF_JSON_DECODER */

#undef JSON_DECODER
#undef COGCHEF_RETURN

#undef COGCHEF_int
#undef COGCHEF_PTR_char
#undef COGCHEF_bool
#undef COGCHEF_STRUCT_PTR
