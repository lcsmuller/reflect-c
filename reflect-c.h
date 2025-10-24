#ifndef REFLECTC_H
#define REFLECTC_H

#include <stddef.h>

/* Allow consumers to rename the runtime namespace (`reflectc_*` /
 * `REFLECTC_*`). */
#ifndef REFLECTC_PREFIX
#define REFLECTC_PREFIX reflectc
#endif /* REFLECTC_PREFIX */

#ifndef REFLECTC_PREFIX_UPPER
#define REFLECTC_PREFIX_UPPER REFLECTC
#endif /* REFLECTC_PREFIX_UPPER */

#define __CAT(_a, _b)           _a##_b
#define _CAT(_a, _b)            __CAT(_a, _b)
#define REFLECTC_NS(_sym)       _CAT(REFLECTC_PREFIX, _sym)
#define REFLECTC_NS_UPPER(_sym) _CAT(REFLECTC_PREFIX_UPPER, _sym)

/* Forward declarations */
struct REFLECTC_PREFIX;
struct REFLECTC_NS(_template);
struct REFLECTC_NS(_wrap);
struct REFLECTC_NS(_wrap_mut);

#define REFLECTC_TYPES(_sym) REFLECTC_NS_UPPER(_TYPES__##_sym)

enum REFLECTC_NS(_types) {
    REFLECTC_TYPES(void),
    REFLECTC_TYPES(bool),
    REFLECTC_TYPES(char),
    REFLECTC_TYPES(short),
    REFLECTC_TYPES(int),
    REFLECTC_TYPES(long),
    REFLECTC_TYPES(float),
    REFLECTC_TYPES(double),
    REFLECTC_TYPES(struct),
    REFLECTC_TYPES(union),
    REFLECTC_TYPES(enum),
    REFLECTC_TYPES(EXTEND)
};

typedef struct REFLECTC_NS(_wrap)
    * (*REFLECTC_NS(_from_cb))(struct REFLECTC_PREFIX *registry,
                               void *self,
                               struct REFLECTC_NS(_wrap) * root);

struct REFLECTC_NS(_template) {
    const size_t size;
    const struct {
        const char *const buf;
        const size_t length;
    } qualifier, decorator, name, dimensions;
    const enum REFLECTC_NS(_types) type;
    const unsigned long attrs;
    const struct {
        const size_t length;
        const struct REFLECTC_NS(_template) * array;
    } members;
    const REFLECTC_NS(_from_cb) from_cb;
};

#define _REFLECTC_PICKER_const     struct REFLECTC_NS(_wrap)
#define _REFLECTC_PICKER___BLANK__ struct REFLECTC_NS(_wrap_mut)

#define REFLECTC_INSTANCE_FIELDS(_qualifier)                                  \
    _qualifier size_t length;                                                 \
    _qualifier void *_qualifier ptr_value;                                    \
    _qualifier struct {                                                       \
        _qualifier size_t length;                                             \
        _qualifier _REFLECTC_PICKER_##_qualifier *_qualifier array;           \
    } members

struct REFLECTC_NS(_wrap) {
    const struct REFLECTC_NS(_template) * tmpl;
    REFLECTC_INSTANCE_FIELDS(const);
};

#define __BLANK__
struct REFLECTC_NS(_wrap_mut) {
    const struct REFLECTC_NS(_template) * tmpl;
    REFLECTC_INSTANCE_FIELDS(__BLANK__);
};
#undef __BLANK__
#undef REFLECTC_INSTANCE_FIELDS
#undef _REFLECTC_PICKER_const
#undef _REFLECTC_PICKER___BLANK__

struct REFLECTC_PREFIX *REFLECTC_NS(_init)(void);
void REFLECTC_NS(_dispose)(struct REFLECTC_PREFIX *table);
struct REFLECTC_NS(_wrap)
    * REFLECTC_NS(_find)(const struct REFLECTC_PREFIX *table,
                         const void *value);
int REFLECTC_NS(_put)(struct REFLECTC_PREFIX *table,
                      void *value,
                      struct REFLECTC_NS(_wrap) * wrapper);
void REFLECTC_NS(_erase)(struct REFLECTC_PREFIX *table, const void *value);

size_t REFLECTC_NS(_length)(const struct REFLECTC_NS(_wrap) * member);

size_t REFLECTC_NS(_get_pos)(const struct REFLECTC_NS(_wrap) * root,
                             const char *const name,
                             const size_t len);

void REFLECTC_NS(_array)(const struct REFLECTC_NS(_wrap) * root,
                         const size_t length);

unsigned REFLECTC_NS(_get_pointer_depth)(const struct REFLECTC_NS(_wrap)
                                         * member);

const void *REFLECTC_NS(_deref)(const struct REFLECTC_NS(_wrap) * field);
const void *REFLECTC_NS(_memcpy)(const struct REFLECTC_NS(_wrap) * field,
                                 const void *map,
                                 const size_t size);
const char *REFLECTC_NS(_string)(const struct REFLECTC_NS(_wrap) * dest,
                                 const char src[],
                                 const size_t size);
void *REFLECTC_NS(_resolve)(const struct REFLECTC_NS(_wrap) * member);
int REFLECTC_NS(_expand)(struct REFLECTC_PREFIX *registry,
                         const struct REFLECTC_NS(_wrap) * member);

typedef int (*REFLECTC_NS(_visit_cb))(const struct REFLECTC_NS(_wrap) * member,
                                      void *ctx);

void REFLECTC_NS(_cleanup)(struct REFLECTC_PREFIX *registry,
                           struct REFLECTC_NS(_wrap) * member);
void REFLECTC_NS(_cleanup_members)(struct REFLECTC_PREFIX *registry,
                                   struct REFLECTC_NS(_wrap) * member);
int REFLECTC_NS(_for_each)(const struct REFLECTC_NS(_wrap) * root,
                           REFLECTC_NS(_visit_cb) cb,
                           void *ctx);
const struct REFLECTC_NS(_wrap)
    * REFLECTC_NS(_require_member)(const struct REFLECTC_NS(_wrap) * root,
                                   const char *name,
                                   size_t len);
int REFLECTC_NS(_is_pointer_type)(const struct REFLECTC_NS(_wrap) * member);
int REFLECTC_NS(_is_null)(const struct REFLECTC_NS(_wrap) * member);
int REFLECTC_NS(_expand_all)(struct REFLECTC_PREFIX *registry,
                             struct REFLECTC_NS(_wrap) * root);

#define REFLECTC_LOOKUP(_container, _namespace, _member_name, _root)          \
    (size_t)(REFLECTC_NS_UPPER(_LOOKUP__##_namespace##__##_member_name))

const void *REFLECTC_NS(_get)(const struct REFLECTC_NS(_wrap) * member);
const void *REFLECTC_NS(_set)(const struct REFLECTC_NS(_wrap) * member,
                              const void *value,
                              size_t size);
const void *REFLECTC_NS(_get_member)(const struct REFLECTC_NS(_wrap) * root,
                                     size_t pos);
const void *REFLECTC_NS(_set_member)(const struct REFLECTC_NS(_wrap) * root,
                                     size_t pos,
                                     const void *value,
                                     size_t size);

#endif /* REFLECTC_H */
