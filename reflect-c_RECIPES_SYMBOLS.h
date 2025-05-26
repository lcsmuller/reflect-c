#ifndef REFLECTC_RECIPES_SYMBOLS_H
#define REFLECTC_RECIPES_SYMBOLS_H

#include "reflect-c_TUPLE.h"

#if 0
The following allows symbols usage without REFLECTC_ prefix
#endif
#ifndef REFLECTC_USE_PREFIX
#define BLANK _

#define PUBLIC  REFLECTC_PUBLIC
#define PRIVATE REFLECTC_PRIVATE
#endif /* REFLECTC_USE_PREFIX */

#define REFLECTC_RCF_SPREAD(_name, _field)                                    \
    RCF REFLECTC_TUPLE_PREPEND(1, (_name), 6, _field)
#define REFLECTC_RCE_SPREAD(_name, _enumerator)                               \
    RCE REFLECTC_TUPLE_PREPEND(1, (_name), 3, _enumerator)

#define REFLECTC_struct(_name, _nfields, _fields)                             \
    REFLECTC_STRUCT(_name)                                                    \
    REFLECTC_TUPLE_FOREACH(REFLECTC_RCF_SPREAD, _name, _nfields, _fields)     \
    REFLECTC_STRUCT_END(_name)
#define REFLECTC_union(_name, _nfields, _fields)                              \
    REFLECTC_UNION(_name)                                                     \
    REFLECTC_TUPLE_FOREACH(REFLECTC_RCF_SPREAD, _name, _nfields, _fields)     \
    REFLECTC_UNION_END(_name)
#define REFLECTC_enum(_name, _nenumerators, _enumerators)                     \
    REFLECTC_ENUM(_name)                                                      \
    REFLECTC_TUPLE_FOREACH(REFLECTC_RCE_SPREAD, _name, _nenumerators,         \
                           _enumerators)                                      \
    REFLECTC_ENUM_END(_name)

#endif /* REFLECTC_RECIPES_SYMBOLS_H */
