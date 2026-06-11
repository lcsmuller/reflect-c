#ifdef REFLECTC_PRE_HOOKS
/*#!
#include <stdbool.h>
#define BYE 2
*/
#endif

PUBLIC(struct, foo, 4, (
        (_, _, bool, _, boolean, _, _, 0ul),
        (_, _, int, _, number, _, [4], 0ul),
        (_, _, char, *, string, text, _, 0ul),
        (_, _, void, *, native, _, _, 0ul)
    )
)
