#ifdef REFLECTC_DEFINITIONS
/*#! #define BYE 2 */
#endif

STRUCT(public, foo)
    FIELD(boolean, bool, BLANK, false)
    FIELD(number, int, BLANK, 0)
    FIELD(string, char, *, NULL)
STRUCT_END