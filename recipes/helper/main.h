#define HELPER

#ifdef REFLECTC_HELPER
#ifdef REFLECTC_HEADER

/*#! #include "submodules/oa-hash/oa_hash.h" */

enum reflectc_modes { REFLECTC_MODES_WRITE, REFLECTC_MODES_READONLY };

enum reflectc_types {
    REFLECTC_TYPES__char,
    REFLECTC_TYPES__short,
    REFLECTC_TYPES__int,
    REFLECTC_TYPES__long,
    REFLECTC_TYPES__float,
    REFLECTC_TYPES__double,
    REFLECTC_TYPES__bool,
    REFLECTC_TYPES__struct,
    REFLECTC_TYPES__EXTEND
};

/* forward definition */
struct reflectc;
/**/

typedef struct reflectc *(*const reflectc_from_type_t)(
                            void *, enum reflectc_modes, struct reflectc *);

struct reflectc {
    const unsigned ptr_depth;
    const size_t size;
    const struct {
        const size_t len;
        const char *const buf;
    } name;
    const enum reflectc_types type;
    reflectc_from_type_t init;
    void *value;
    struct oa_hash ht;
    enum reflectc_modes mode;
};

struct reflectc *reflectc_get(struct reflectc *reflectc,
                            const char *key,
                            const size_t key_len);

struct reflectc *reflectc_set(struct reflectc *reflectc,
                            const char *key,
                            const size_t key_len,
                            void *value);

#define REFLECTC_STRUCT_public(_type)                                         \
    struct reflectc *reflectc_from_##_type(struct _type *self,                \
                                         enum reflectc_modes mode,            \
                                         struct reflectc *root);

#include "reflect-c_EXPAND.h"

#elif defined(REFLECTC_FORWARD)

/*#! #include <stdlib.h> */
/*#! #include <string.h> */

struct reflectc *
reflectc_get(struct reflectc *reflectc, const char *key, const size_t key_len)
{
    return oa_hash_get(&reflectc->ht, key, key_len);
}

struct reflectc *
reflectc_set(struct reflectc *reflectc,
            const char *key,
            const size_t key_len,
            void *value)
{
    return (reflectc->mode & REFLECTC_MODES_WRITE)
               ? oa_hash_set(&reflectc->ht, key, key_len, value)
               : NULL;
}

#define REFLECTC_STRUCT_private(_type)                                        \
    static struct reflectc *reflectc_from_##_type(struct _type *self,         \
                                                enum reflectc_modes mode,     \
                                                struct reflectc *root);

#include "reflect-c_EXPAND.h"

#define PTR_DEPTH(_decor) #_decor[0] == '*' ? sizeof(#_decor) - 1 : 0

#define REFLECTC_STRUCT_private(_type)                                        \
    static const struct reflectc _type##__root =                              \
        { 0, sizeof(struct _type), { sizeof(#_type) - 1, #_type },            \
            REFLECTC_TYPES__struct, NULL, NULL, { 0 }, 0 };                   \
                                                                              \
    static const struct reflectc _type##__fields[] = {
#define REFLECTC_FIELD_CUSTOM(_name, _type, _decor, _func, _default_value)    \
        { PTR_DEPTH(_decor), sizeof(_type _decor),                            \
            { sizeof(#_name) - 1, #_name }, REFLECTC_TYPES__##_type, NULL,    \
            NULL, { 0 }, 0 },
#define REFLECTC_FIELD_STRUCT_PTR(_name, _type, _decor)                       \
        { PTR_DEPTH(_decor), sizeof(struct _type _decor),                     \
            { sizeof(#_name) - 1, #_name }, REFLECTC_TYPES__struct,           \
            (reflectc_from_type_t)reflectc_from_##_type, NULL, { 0 }, 0 },
#define REFLECTC_FIELD_PRINTF(_name, _type, _printf_type, _scanf_type)        \
        { 0, sizeof(_type), { sizeof(#_name) - 1, #_name },                   \
            REFLECTC_TYPES__##_type, NULL, NULL, { 0 }, 0 },
#define REFLECTC_FIELD_ENUM(_name, _type)                                     \
        { 0, sizeof(enum _type), { sizeof(#_name) - 1, #_name },              \
            REFLECTC_TYPES__int, NULL, NULL, { 0 }, 0 },
#define REFLECTC_STRUCT_END                                                   \
    };
#define REFLECTC_STRUCT_public REFLECTC_STRUCT_private

#include "reflect-c_EXPAND.h"

#else

#define REFLECTC_STRUCT_public(_type)                                         \
    struct reflectc *                                                         \
    reflectc_from_##_type(                                                    \
        struct _type *self,                                                   \
        enum reflectc_modes mode,                                             \
        struct reflectc *root)                                                \
    {                                                                         \
        static const size_t capacity =                                        \
            (sizeof(_type##__fields) / sizeof *_type##__fields) * 2;          \
        struct oa_hash_entry *buckets = malloc(sizeof *buckets * capacity);   \
        struct reflectc *fields = malloc(sizeof(_type##__fields));            \
        if (!root && (root = malloc(sizeof *root))) {                         \
            memcpy(root, &_type##__root, sizeof *root);                       \
            root->value = self;                                               \
        }                                                                     \
        root->mode = mode;                                                    \
        memcpy(fields, _type##__fields, sizeof(_type##__fields));             \
        oa_hash_init(&root->ht, buckets, capacity);
#define REFLECTC_FIELD_CUSTOM(_name, _type, _decor, _func, _default_value)    \
        oa_hash_set(&root->ht, #_name, sizeof(#_name) - 1,                    \
                    ((fields->value = &self->_name), fields++));
#define REFLECTC_FIELD_PTR(_name, _type, __decor)                             \
        oa_hash_set(&root->ht, #_name, sizeof(#_name) - 1,                    \
                    ((fields->value = self->_name), fields++));
#define REFLECTC_FIELD_STRUCT_PTR(_name, _type, _decor)                       \
        oa_hash_set(                                                          \
            &root->ht, #_name, sizeof(#_name) - 1,                            \
            ((fields->value =                                                 \
                (reflectc_from_bar(self->_name, mode, fields), self->_name),  \
            fields++)));
#define REFLECTC_STRUCT_END                                                   \
        return root;                                                          \
    }
#define REFLECTC_STRUCT_private static REFLECTC_STRUCT_public

#include "reflect-c_EXPAND.h"

#endif /* REFLECTC_HEADER */
#endif /* REFLECTC_HELPER */

#undef HELPER
