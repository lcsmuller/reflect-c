#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stddef.h>
#include <stdbool.h>

#define BYE 2
*/
#endif

PUBLIC(struct, foo, 3, (
        (_, _, bool, _, boolean, _),
        (_, _, int, _, number, _),
        (_, _, char, *, string, _)
    )
)
