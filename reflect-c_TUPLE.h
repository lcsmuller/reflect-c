#ifndef REFLECTC_TUPLE_H
#define REFLECTC_TUPLE_H

#define EVAL(_x)     _x
#define JOIN(_1, _2) _1 _2

#define REFLECTC_TUPLE_FOREACH(_expr, _extra, _ntuple, _tuple)                \
    EVAL(JOIN(REFLECTC_TUPLE_FOREACH_##_ntuple,                               \
              REFLECTC_TUPLE_PREPEND(2, (_expr, _extra), _ntuple, _tuple)))

#define REFLECTC_TUPLE_APPEND(_ntuple, _tuple, _nvalues, _values)             \
    (REFLECTC_TUPLE_SPREAD_##_ntuple _tuple,                                  \
     REFLECTC_TUPLE_SPREAD_##_nvalues values)
#define REFLECTC_TUPLE_PREPEND(_nvalues, _values, _ntuple, _tuple)            \
    (REFLECTC_TUPLE_SPREAD_##_nvalues _values,                                \
     REFLECTC_TUPLE_SPREAD_##_ntuple _tuple)

#define REFLECTC_TUPLE_SPREAD_0
#define REFLECTC_TUPLE_SPREAD_1(_1)                     _1
#define REFLECTC_TUPLE_SPREAD_2(_1, _2)                 _1, _2
#define REFLECTC_TUPLE_SPREAD_3(_1, _2, _3)             _1, _2, _3
#define REFLECTC_TUPLE_SPREAD_4(_1, _2, _3, _4)         _1, _2, _3, _4
#define REFLECTC_TUPLE_SPREAD_5(_1, _2, _3, _4, _5)     _1, _2, _3, _4, _5
#define REFLECTC_TUPLE_SPREAD_6(_1, _2, _3, _4, _5, _6) _1, _2, _3, _4, _5, _6
#define REFLECTC_TUPLE_SPREAD_7(_1, _2, _3, _4, _5, _6, _7)                   \
    _1, _2, _3, _4, _5, _6, _7
#define REFLECTC_TUPLE_SPREAD_8(_1, _2, _3, _4, _5, _6, _7, _8)               \
    _1, _2, _3, _4, _5, _6, _7, _8
#define REFLECTC_TUPLE_SPREAD_9(_1, _2, _3, _4, _5, _6, _7, _8, _9)           \
    _1, _2, _3, _4, _5, _6, _7, _8, _9
#define REFLECTC_TUPLE_SPREAD_10(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10)     \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10
#define REFLECTC_TUPLE_SPREAD_11(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,     \
                                 _11)                                         \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11
#define REFLECTC_TUPLE_SPREAD_12(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,     \
                                 _11, _12)                                    \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12
#define REFLECTC_TUPLE_SPREAD_13(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,     \
                                 _11, _12, _13)                               \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13
#define REFLECTC_TUPLE_SPREAD_14(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,     \
                                 _11, _12, _13, _14)                          \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14
#define REFLECTC_TUPLE_SPREAD_15(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,     \
                                 _11, _12, _13, _14, _15)                     \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15
#define REFLECTC_TUPLE_SPREAD_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,     \
                                 _11, _12, _13, _14, _15, _16)                \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16

#define REFLECTC_TUPLE_FOREACH_0
#define REFLECTC_TUPLE_FOREACH_1(_expr, _extra, _1) _expr(_extra, _1)
#define REFLECTC_TUPLE_FOREACH_2(_expr, _extra, _1, _2)                       \
    _expr(_extra, _1) _expr(_extra, _2)
#define REFLECTC_TUPLE_FOREACH_3(_expr, _extra, _1, _2, _3)                   \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3)
#define REFLECTC_TUPLE_FOREACH_4(_expr, _extra, _1, _2, _3, _4)               \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)
#define REFLECTC_TUPLE_FOREACH_5(_expr, _extra, _1, _2, _3, _4, _5)           \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5)
#define REFLECTC_TUPLE_FOREACH_6(_expr, _extra, _1, _2, _3, _4, _5, _6)       \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6)
#define REFLECTC_TUPLE_FOREACH_7(_expr, _extra, _1, _2, _3, _4, _5, _6, _7)   \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)
#define REFLECTC_TUPLE_FOREACH_8(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,   \
                                 _8)                                          \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8)
#define REFLECTC_TUPLE_FOREACH_9(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,   \
                                 _8, _9)                                      \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9)
#define REFLECTC_TUPLE_FOREACH_10(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10)                                \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)
#define REFLECTC_TUPLE_FOREACH_11(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10, _11)                           \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)            \
                _expr(_extra, _11)
#define REFLECTC_TUPLE_FOREACH_12(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10, _11, _12)                      \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)            \
                _expr(_extra, _11) _expr(_extra, _12)
#define REFLECTC_TUPLE_FOREACH_13(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10, _11, _12, _13)                 \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)            \
                _expr(_extra, _11) _expr(_extra, _12) _expr(_extra, _13)
#define REFLECTC_TUPLE_FOREACH_14(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10, _11, _12, _13, _14)            \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)            \
                _expr(_extra, _11) _expr(_extra, _12) _expr(_extra, _13)      \
                    _expr(_extra, _14)
#define REFLECTC_TUPLE_FOREACH_15(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10, _11, _12, _13, _14, _15)       \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)            \
                _expr(_extra, _11) _expr(_extra, _12) _expr(_extra, _13)      \
                    _expr(_extra, _14) _expr(_extra, _15)
#define REFLECTC_TUPLE_FOREACH_16(_expr, _extra, _1, _2, _3, _4, _5, _6, _7,  \
                                  _8, _9, _10, _11, _12, _13, _14, _15, _16)  \
    _expr(_extra, _1) _expr(_extra, _2) _expr(_extra, _3) _expr(_extra, _4)   \
        _expr(_extra, _5) _expr(_extra, _6) _expr(_extra, _7)                 \
            _expr(_extra, _8) _expr(_extra, _9) _expr(_extra, _10)            \
                _expr(_extra, _11) _expr(_extra, _12) _expr(_extra, _13)      \
                    _expr(_extra, _14) _expr(_extra, _15) _expr(_extra, _16)

#endif /* REFLECTC_TUPLE_H */
