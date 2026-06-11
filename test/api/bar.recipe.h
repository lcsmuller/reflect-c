#ifdef REFLECTC_PRE_HOOKS
/*#!
#include <stdbool.h>
#define HELLO 1
#define TEST_ATTR_NULLABLE (1ul << 0)
*/
#endif

PRIVATE(struct, bar, 3, (
        (_, _, bool, _, boolean, _, _, 0ul),
        (_, _, int, _, number, _, _, 0ul),
        (_, _, char,  *, string, _, _, 0ul)
    )
)

PUBLIC(struct, baz, 5, (
        (_, struct, bar, *, a, _, _, 0ul),
        (_, struct, bar, *, b, _, _, 0ul),
        (_, struct, bar, ***, c, _, _, 0ul),
        (_, _, char, *, d, _, _, TEST_ATTR_NULLABLE),
        (_, enum, levels, _, e, _, _, 0ul)
    )
)

PUBLIC(union, barbaz, 2, (
        (_, struct, bar, _, bar, _, _, 0ul),
        (_, struct, baz, _, baz, _, _, 0ul)
    )
)

PRIVATE(enum, levels, 3, (
        (LEVELS_ONE, =, 1),
        (LEVELS_TWO, _, _),
        (LEVELS_THREE, _, _)
    )
)
