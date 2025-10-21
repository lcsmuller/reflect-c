#ifdef REFLECTC_PRE_HOOKS
/*#!
#include <stdbool.h>
#include <stddef.h>

typedef size_t words_t;
typedef unsigned long numbers_t;
*/
#endif

PUBLIC(struct, tuna, 4, (
    (_, _, int, _, value, _, 0ul),
    (_, _, bool, _, flag, _, 0ul),
    (_, _, words_t, _, words, _, 0ul),
    (_, _, numbers_t, _, numbers, _, 0ul)
    )
)

#ifdef REFLECTC_POST_HOOKS
/*#!
enum {
    REFLECTC_NS_UPPER(_TYPES__words_t) = REFLECTC_NS_UPPER(_TYPES__EXTEND),
    REFLECTC_NS_UPPER(_TYPES__numbers_t)
};
*/
#endif
