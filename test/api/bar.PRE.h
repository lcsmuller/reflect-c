#ifdef REFLECTC_DEFINITIONS
/*#! #define HELLO 1 */
#endif

STRUCT(private, bar)
    FIELD(boolean, bool, BLANK, false)
    FIELD(number, int, BLANK, 0)
    FIELD(string, char, *, NULL)
STRUCT_END

STRUCT(public, baz)
    FIELD_STRUCT(a, bar, *, NULL)
    FIELD_STRUCT(b, bar, *, NULL)
    FIELD_STRUCT(c, bar, *, NULL)
    FIELD(d, char, *, NULL)
STRUCT_END

LIST(public, bazz)
    ELEMENT(char, *)
LIST_END