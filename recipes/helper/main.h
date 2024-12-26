#define HELPER

#ifdef COGCHEF_HELPER
#ifdef COGCHEF_HEADER

/*#! #include "submodules/oa-hash/oa_hash.h" */

enum cogchef_modes { COGCHEF_MODES_WRITE, COGCHEF_MODES_READONLY };

enum cogchef_types {
    COGCHEF_TYPES__char,
    COGCHEF_TYPES__short,
    COGCHEF_TYPES__int,
    COGCHEF_TYPES__long,
    COGCHEF_TYPES__float,
    COGCHEF_TYPES__double,
    COGCHEF_TYPES__bool,
    COGCHEF_TYPES__struct,
    COGCHEF_TYPES__EXTEND
};

/* forward definition */
struct cogchef;
/**/

typedef struct cogchef *(*const cogchef_from_type_t)(void *, enum cogchef_modes, struct cogchef *);

struct cogchef {
    const unsigned ptr_depth;
    const size_t size;
    const struct {
        const size_t len;
        const char *const buf;
    } name;
    const enum cogchef_types type;
    cogchef_from_type_t init;
    void *value;
    struct oa_hash ht;
    enum cogchef_modes mode;
};

struct cogchef *cogchef_get(struct cogchef *cogchef,
                            const char *key,
                            const size_t key_len);

struct cogchef *cogchef_set(struct cogchef *cogchef,
                            const char *key,
                            const size_t key_len,
                            void *value);

#define COGCHEF_STRUCT_public(_type)                                          \
    struct cogchef *cogchef_from_##_type(struct _type *self,                  \
                                         enum cogchef_modes mode,             \
                                         struct cogchef *saveptr);

#include "cogchef-assemble.ACTION.h"

#elif defined(COGCHEF_FORWARD)

/*#! #include <stdlib.h> */
/*#! #include <string.h> */

struct cogchef *
cogchef_get(struct cogchef *cogchef, const char *key, const size_t key_len)
{
    return oa_hash_get(&cogchef->ht, key, key_len);
}

struct cogchef *
cogchef_set(struct cogchef *cogchef,
            const char *key,
            const size_t key_len,
            void *value)
{
    return (cogchef->mode & COGCHEF_MODES_WRITE)
               ? oa_hash_set(&cogchef->ht, key, key_len, value)
               : NULL;
}

#define COGCHEF_STRUCT_private(_type)                                         \
    static struct cogchef *cogchef_from_##_type(struct _type *self,           \
                                                enum cogchef_modes mode,      \
                                                struct cogchef *saveptr);

#include "cogchef-assemble.ACTION.h"

#define PTR_DEPTH(_decor) #_decor[0] == '*' ? sizeof(#_decor) - 1 : 0

#define COGCHEF_STRUCT_private(_type)                                         \
    static const struct cogchef _type##__fields[] = {                         \
        { 0, sizeof(struct _type), { 5, "$self" }, COGCHEF_TYPES__struct,     \
            NULL, NULL, { 0 }, 0 },
#define COGCHEF_FIELD_CUSTOM(_name, _type, _decor, _func, _default_value)     \
        { PTR_DEPTH(_decor), sizeof(_type _decor),                            \
            { sizeof(#_name) - 1, #_name }, COGCHEF_TYPES__##_type, NULL,     \
            NULL, { 0 }, 0 },
#define COGCHEF_FIELD_STRUCT_PTR(_name, _type, _decor)                        \
        { PTR_DEPTH(_decor), sizeof(struct _type _decor),                     \
            { sizeof(#_name) - 1, #_name }, COGCHEF_TYPES__struct,            \
            (cogchef_from_type_t)cogchef_from_##_type, NULL, { 0 }, 0 },
#define COGCHEF_FIELD_PRINTF(_name, _type, _printf_type, _scanf_type)         \
        { 0, sizeof(_type), { sizeof(#_name) - 1, #_name },                   \
            COGCHEF_TYPES__##_type, NULL, NULL, { 0 }, 0 },
#define COGCHEF_FIELD_ENUM(_name, _type)                                      \
        { 0, sizeof(enum _type), { sizeof(#_name) - 1, #_name },              \
            COGCHEF_TYPES__int, NULL, NULL, { 0 }, 0 },
#define COGCHEF_STRUCT_END                                                    \
    };
#define COGCHEF_STRUCT_public COGCHEF_STRUCT_private

#include "cogchef-assemble.ACTION.h"

#else

#define COGCHEF_STRUCT_public(_type)                                          \
    struct cogchef *                                                          \
    cogchef_from_##_type(                                                     \
        struct _type *self, enum cogchef_modes mode, struct cogchef *saveptr) \
    {                                                                         \
        static const size_t capacity =                                        \
            (sizeof(_type##__fields) / sizeof *_type##__fields) * 2;          \
        struct oa_hash_entry *buckets = malloc(sizeof *buckets * capacity);   \
        struct cogchef *fields = malloc(sizeof(_type##__fields));             \
        if (!saveptr && !(saveptr = malloc(sizeof *saveptr))) return NULL;    \
        memcpy(fields++, _type##__fields, sizeof *_type##__fields);           \
        memcpy(fields, &_type##__fields[1],                                   \
               sizeof(_type##__fields) - sizeof *_type##__fields);            \
        oa_hash_init(&saveptr->ht, buckets, capacity);
#define COGCHEF_FIELD_CUSTOM(_name, _type, _decor, _func, _default_value)     \
        oa_hash_set(&saveptr->ht, #_name, sizeof(#_name) - 1,                 \
                    ((fields->value = &self->_name), fields++));
#define COGCHEF_FIELD_STRUCT_PTR(_name, _type, _decor)                        \
        oa_hash_set(&saveptr->ht, #_name, sizeof(#_name) - 1,                 \
                    ((fields->value = (                                       \
                        cogchef_from_##_type(self->_name, mode, fields),      \
                                                          &self->_name        \
                        ), fields++)));
#define COGCHEF_STRUCT_END                                                    \
        saveptr->mode = mode;                                                 \
        return saveptr;                                                       \
    }
#define COGCHEF_STRUCT_private static COGCHEF_STRUCT_public

#include "cogchef-assemble.ACTION.h"

#endif /* COGCHEF_HEADER */
#endif /* COGCHEF_HELPER */

#undef HELPER
