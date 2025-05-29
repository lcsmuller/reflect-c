#ifdef REFLECTC_DEFINITIONS
/*#!
#include <stdbool.h>
#define HELLO 1
*/
#endif

PRIVATE(struct, bar, 3, (
        (_, _, bool, _, boolean, _),
        (_, _, int, _, number, _),
        (_, _, char,  *, string, _)
    )
)

PUBLIC(struct, baz, 4, (
        (_, struct, bar, *, a, _),
        (_, struct, bar, *, b, _),
        (_, struct, bar,         * *       *, c, _),
        (_, _, char, *, d, _)
    )
)

PUBLIC(union, barbaz, 2, (
        (_, struct, bar, _, bar, _),
        (_, struct, baz, _, baz, _)
    )
)

PRIVATE(enum, levels, 3, (
        (LEVELS_ONE, =, 1),
        (LEVELS_TWO, _, _),
        (LEVELS_THREE, _, _)
    )
)
