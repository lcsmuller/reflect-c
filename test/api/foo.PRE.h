#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stdbool.h>
#define BYE 2
*/
#endif

PUBLIC(struct, foo, 4, (
        (_, _, bool, _, boolean, _),
        (_, _, int, _, number, [4]),
        (_, _, char, *, string, _),
        (_, _, void, *, native, _)
    )
)
