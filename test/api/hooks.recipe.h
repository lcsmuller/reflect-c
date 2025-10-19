#if defined(REFLECTC_DEFINITIONS) || defined(REFLECTC_WRAPPER)
/*#!
#include <stdbool.h>
#include <stddef.h>
#ifndef REFLECTC_CUSTOM_TYPES_DEFINED
#define REFLECTC_CUSTOM_TYPES_DEFINED 1

typedef size_t words_t;
typedef unsigned long numbers_t;

enum {
    REFLECTC_NS_UPPER(_TYPES__words_t) = REFLECTC_NS_UPPER(_TYPES__EXTEND),
    REFLECTC_NS_UPPER(_TYPES__numbers_t)
};
#endif
*/
#endif

PUBLIC(struct, hooks, 4, (
    (_, _, int, _, value, _),
    (_, _, bool, _, flag, _),
    (_, _, words_t, _, words, _),
    (_, _, numbers_t, _, numbers, _)
    )
)
