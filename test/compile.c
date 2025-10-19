#include <stdbool.h>
#include <stddef.h>

#define STATIC_ASSERT(_cond, _name)                                           \
    typedef char static_assert_##_name[((_cond) ? 1 : -1)]

#define REFLECTC_PREFIX       rc_compile
#define REFLECTC_PREFIX_UPPER RC_COMPILE
#include "reflect-c_GENERATED.h"

STATIC_ASSERT(sizeof(&rc_compile_get) > 0, rc_compile_get_symbol);
STATIC_ASSERT(sizeof(&rc_compile_set) > 0, rc_compile_set_symbol);
STATIC_ASSERT(sizeof(&rc_compile_get_member) > 0,
              rc_compile_get_member_symbol);
STATIC_ASSERT(sizeof(&rc_compile_set_member) > 0,
              rc_compile_set_member_symbol);
STATIC_ASSERT(sizeof(&rc_compile_from_baz) > 0, rc_compile_from_api_symbol);

/* Metadata table shape: member counts should match the recipe declarations. */
STATIC_ASSERT(RC_COMPILE_LOOKUP__bar_MAX == 3, bar_member_count);
STATIC_ASSERT(RC_COMPILE_LOOKUP__baz_MAX == 5, baz_member_count);
STATIC_ASSERT(RC_COMPILE_LOOKUP__foo_MAX == 4, foo_member_count);
STATIC_ASSERT(RC_COMPILE_LOOKUP__tuna_MAX == 4, tuna_member_count);

/* Spill-free lookup indices emitted by REFLECTC_LOOKUP. */
STATIC_ASSERT(REFLECTC_LOOKUP(struct, bar, boolean, NULL) == 0,
              bar_boolean_index);
STATIC_ASSERT(REFLECTC_LOOKUP(struct, bar, string, NULL) == 2,
              bar_string_index);
STATIC_ASSERT(REFLECTC_LOOKUP(struct, baz, c, NULL) == 2, baz_c_index);

/* Custom scalar extensions must remain anchored after REFLECTC_TYPES__EXTEND.
 */
STATIC_ASSERT((int)RC_COMPILE_TYPES__words_t == (int)RC_COMPILE_TYPES__EXTEND,
              custom_words_base);
STATIC_ASSERT((int)RC_COMPILE_TYPES__numbers_t
                  == (int)RC_COMPILE_TYPES__words_t + 1,
              custom_numbers_increment);

/* Sanity-check generated member types. */
STATIC_ASSERT(sizeof(((struct bar *)0)->boolean) == sizeof(bool),
              bar_boolean_is_bool);
STATIC_ASSERT(sizeof(((struct bar *)0)->number) == sizeof(int),
              bar_number_is_int);
STATIC_ASSERT(sizeof(((struct foo *)0)->number) == sizeof(int[4]),
              foo_number_array_length);

/* Ensure const and mutable metadata wrappers stay layout-compatible. */
STATIC_ASSERT(sizeof(struct rc_compile) == sizeof(struct rc_compile_mut),
              metadata_mut_layout);

#undef STATIC_ASSERT
