#ifndef REFLECTC_RECIPES_SYMBOLS_H
#define REFLECTC_RECIPES_SYMBOLS_H

#if 0
The following allows symbols usage without REFLECTC_ prefix
#endif
#ifndef REFLECTC_USE_PREFIX
#define BLANK _

#define PUBLIC  REFLECTC_PUBLIC
#define PRIVATE REFLECTC_PRIVATE
#endif /* REFLECTC_USE_PREFIX */

#define REFLECTC_struct(_name, _n, _fields)                                   \
    REFLECTC_STRUCT(_name)                                                    \
    REFLECTC_SPREAD_FIELDS_##_n _fields REFLECTC_STRUCT_END
#define REFLECTC_union(_name, _n, _fields)                                    \
    REFLECTC_UNION(_name)                                                     \
    REFLECTC_SPREAD_FIELDS_##_n _fields REFLECTC_UNION_END
#define REFLECTC_enum(_name, _n, _enumerators)                                \
    REFLECTC_ENUM(_name)                                                      \
    REFLECTC_SPREAD_ENUMERATORS_##_n _enumerators REFLECTC_ENUM_END

#define REFLECTC_SPREAD_FIELDS_0
#define REFLECTC_SPREAD_FIELDS_1(_1)             RCF _1
#define REFLECTC_SPREAD_FIELDS_2(_1, _2)         RCF _1 RCF _2
#define REFLECTC_SPREAD_FIELDS_3(_1, _2, _3)     RCF _1 RCF _2 RCF _3
#define REFLECTC_SPREAD_FIELDS_4(_1, _2, _3, _4) RCF _1 RCF _2 RCF _3 RCF _4
#define REFLECTC_SPREAD_FIELDS_5(_1, _2, _3, _4, _5)                          \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5
#define REFLECTC_SPREAD_FIELDS_6(_1, _2, _3, _4, _5, _6)                      \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6
#define REFLECTC_SPREAD_FIELDS_7(_1, _2, _3, _4, _5, _6, _7)                  \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7
#define REFLECTC_SPREAD_FIELDS_8(_1, _2, _3, _4, _5, _6, _7, _8)              \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8
#define REFLECTC_SPREAD_FIELDS_9(_1, _2, _3, _4, _5, _6, _7, _8, _9)          \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9
#define REFLECTC_SPREAD_FIELDS_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10)    \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10
#define REFLECTC_SPREAD_FIELDS_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
                                  _11)                                        \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10    \
        RCF _11
#define REFLECTC_SPREAD_FIELDS_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
                                  _11, _12)                                   \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10    \
        RCF _11 RCF _12
#define REFLECTC_SPREAD_FIELDS_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
                                  _11, _12, _13)                              \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10    \
        RCF _11 RCF _12 RCF _13
#define REFLECTC_SPREAD_FIELDS_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
                                  _11, _12, _13, _14)                         \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10    \
        RCF _11 RCF _12 RCF _13 RCF _14
#define REFLECTC_SPREAD_FIELDS_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
                                  _11, _12, _13, _14, _15)                    \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10    \
        RCF _11 RCF _12 RCF _13 RCF _14 RCF _15
#define REFLECTC_SPREAD_FIELDS_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,    \
                                  _11, _12, _13, _14, _15, _16)               \
    RCF _1 RCF _2 RCF _3 RCF _4 RCF _5 RCF _6 RCF _7 RCF _8 RCF _9 RCF _10    \
        RCF _11 RCF _12 RCF _13 RCF _14 RCF _15 RCF _16

#define REFLECTC_SPREAD_ENUMERATORS_0
#define REFLECTC_SPREAD_ENUMERATORS_1(_1)         RCE _1
#define REFLECTC_SPREAD_ENUMERATORS_2(_1, _2)     RCE _1 RCE _2
#define REFLECTC_SPREAD_ENUMERATORS_3(_1, _2, _3) RCE _1 RCE _2 RCE _3
#define REFLECTC_SPREAD_ENUMERATORS_4(_1, _2, _3, _4)                         \
    RCE _1 RCE _2 RCE _3 RCE _4
#define REFLECTC_SPREAD_ENUMERATORS_5(_1, _2, _3, _4, _5)                     \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5
#define REFLECTC_SPREAD_ENUMERATORS_6(_1, _2, _3, _4, _5, _6)                 \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6
#define REFLECTC_SPREAD_ENUMERATORS_7(_1, _2, _3, _4, _5, _6, _7)             \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7
#define REFLECTC_SPREAD_ENUMERATORS_8(_1, _2, _3, _4, _5, _6, _7, _8)         \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8
#define REFLECTC_SPREAD_ENUMERATORS_9(_1, _2, _3, _4, _5, _6, _7, _8, _9)     \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9
#define REFLECTC_SPREAD_ENUMERATORS_10(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10)                                   \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10
#define REFLECTC_SPREAD_ENUMERATORS_11(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10, _11)                              \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10    \
        RCE _11
#define REFLECTC_SPREAD_ENUMERATORS_12(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10, _11, _12)                         \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10    \
        RCE _11 RCE _12
#define REFLECTC_SPREAD_ENUMERATORS_13(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10, _11, _12, _13)                    \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10    \
        RCE _11 RCE _12 RCE _13
#define REFLECTC_SPREAD_ENUMERATORS_14(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10, _11, _12, _13, _14)               \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10    \
        RCE _11 RCE _12 RCE _13 RCE _14
#define REFLECTC_SPREAD_ENUMERATORS_15(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10, _11, _12, _13, _14, _15)          \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10    \
        RCE _11 RCE _12 RCE _13 RCE _14 RCE _15
#define REFLECTC_SPREAD_ENUMERATORS_16(_1, _2, _3, _4, _5, _6, _7, _8, _9,    \
                                       _10, _11, _12, _13, _14, _15, _16)     \
    RCE _1 RCE _2 RCE _3 RCE _4 RCE _5 RCE _6 RCE _7 RCE _8 RCE _9 RCE _10    \
        RCE _11 RCE _12 RCE _13 RCE _14 RCE _15 RCE _16

#endif /* REFLECTC_RECIPES_SYMBOLS_H */
