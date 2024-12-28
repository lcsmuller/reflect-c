#ifdef REFLECTC_DEFINITIONS
/*#! #define BYE 2 */
#endif

STRUCT(public, foo)
    FIELD(boolean, bool, false)
    FIELD(number, int, 0)
    FIELD_PTR(string, char, *)
STRUCT_END