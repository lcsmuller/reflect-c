#ifdef REFLECTC_PRE_HOOKS
/*#!
#include <stdbool.h>
#define BYE 2
*/
#endif

PUBLIC(struct, foo, 4, (
        (_, _, bool, _, boolean, _, 0ul),
        (_, _, int, _, number, [4], 0ul),
        (_, _, char, *, string, _, 0ul),
        (_, _, void, *, native, _, 0ul)
    )
)
