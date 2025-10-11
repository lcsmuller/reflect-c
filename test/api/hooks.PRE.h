#if defined(REFLECTC_DEFINITIONS) || defined(REFLECTC_WRAPPER)
/*#!
#include <stdbool.h>
#include <stddef.h>
#ifndef REFLECTC_CUSTOM_TYPES_DEFINED
#define REFLECTC_CUSTOM_TYPES_DEFINED 1
typedef size_t reflectc_words_t;
typedef unsigned long reflectc_numbers_t;
enum reflectc_custom_types {
    REFLECTC_TYPES__reflectc_words_t = REFLECTC_TYPES__EXTEND,
    REFLECTC_TYPES__reflectc_numbers_t
};
#endif
*/
#endif

PUBLIC(struct, hooks, 4, (
    (_, _, int, _, value, _),
    (_, _, bool, _, flag, _),
    (_, _, reflectc_words_t, _, words, _),
    (_, _, reflectc_numbers_t, _, numbers, _)
    )
)
