#ifdef COGCHEF_DEFINITIONS
/*#! #define HELLO 1 */
#endif

STRUCT(private, bar)
    FIELD(boolean, bool, false)
    FIELD(number, int, 0)
    FIELD_PTR(string, char, *)
STRUCT_END

STRUCT(public, baz)
    FIELD_STRUCT_PTR(a, bar, *)
    FIELD_STRUCT_PTR(b, bar, *)
    FIELD_STRUCT_PTR(c, bar, *)
    FIELD_PTR(d, char, *)
STRUCT_END

LIST(public, bazz)
    ELEMENT_PTR(char, *)
LIST_END