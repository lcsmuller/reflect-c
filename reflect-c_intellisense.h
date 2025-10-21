/**
 * @file reflect-c_intellisense.h
 * @brief Editor-only stubs so recipe headers parse under IntelliSense.
 *
 * This header is intentionally excluded from real builds; it is meant to be
 * force-included by IDE tooling (e.g., VS Code) when opening `.recipe.h`
 * files. By redefining the recipe-generation macros as no-ops, it prevents
 * IntelliSense from complaining about missing `PUBLIC`, `PRIVATE`, `RCF`, and
 * related symbols while leaving the actual preprocessing pipeline untouched.
 */

#ifndef REFLECTC_INTELLISENSE_H
#define REFLECTC_INTELLISENSE_H

#ifndef PUBLIC
/**
 * @def PUBLIC(_container, _name, _count, _tuple)
 * @brief Declare a public recipe container (struct/union/enum).
 *
 * @param _container One of `struct`, `union`, or `enum`.
 * @param _name      Logical type name being described.
 * @param _count     Number of members/enumerators in the `_tuple` block.
 * @param _tuple     Parenthesised list of member tuples.
 */
#define PUBLIC(_container, _name, _count, _tuple)
#endif

#ifndef PRIVATE
/**
 * @def PRIVATE(_container, _name, _count, _tuple)
 * @brief Declare a private recipe container (struct/union/enum).
 *
 * @param _container One of `struct`, `union`, or `enum`.
 * @param _name      Logical type name being described.
 * @param _count     Number of members/enumerators in the `_tuple` block.
 * @param _tuple     Parenthesised list of member tuples.
 */
#define PRIVATE(_container, _name, _count, _tuple)
#endif

#ifndef REFLECTC_PUBLIC
/** Keep guards that rely on REFLECTC_PUBLIC enabled inside the editor. */
#define REFLECTC_PUBLIC 1
#endif

#ifndef REFLECTC_PRIVATE
/** Keep guards that rely on REFLECTC_PRIVATE enabled inside the editor. */
#define REFLECTC_PRIVATE 1
#endif

#ifndef REFLECTC_STRUCT
/**
 * @def REFLECTC_STRUCT(_name)
 * @brief Begin a struct recipe declaration
 *
 * @param _name Logical type name being described.
 */
#define REFLECTC_STRUCT(_name)
#endif

#ifndef REFLECTC_STRUCT_END
/**
 * @def REFLECTC_STRUCT_END(_namespace)
 * @brief End a struct recipe declaration
 *
 * @param _namespace Namespace or prefix used for the struct.
 */
#define REFLECTC_STRUCT_END(_namespace)
#endif

#ifndef REFLECTC_UNION
/**
 * @def REFLECTC_UNION(_name)
 * @brief Begin a union recipe declaration
 *
 * @param _name Logical type name being described.
 */
#define REFLECTC_UNION(_name)
#endif

#ifndef REFLECTC_UNION_END
/**
 * @def REFLECTC_UNION_END(_namespace)
 * @brief End a union recipe declaration
 *
 * @param _namespace Namespace or prefix used for the union.
 */
#define REFLECTC_UNION_END(_namespace)
#endif

#ifndef REFLECTC_ENUM
/**
 * @def REFLECTC_ENUM(_name)
 * @brief Begin an enum recipe declaration
 *
 * @param _name Logical type name being described.
 */
#define REFLECTC_ENUM(_name)
#endif

#ifndef REFLECTC_ENUM_END
/**
 * @def REFLECTC_ENUM_END(_namespace)
 * @brief End an enum recipe declaration
 *
 * @param _namespace Namespace or prefix used for the enum.
 */
#define REFLECTC_ENUM_END(_namespace)
#endif

#ifndef RCF
/**
 * @def RCF(...)
 * @brief Field tuple expander (stubbed to no-op for IntelliSense).
 */
#define RCF(...)
#endif

#ifndef RCE
/**
 * @def RCE(...)
 * @brief Enumerator tuple expander (stubbed to no-op for IntelliSense).
 */
#define RCE(...)
#endif

#ifndef _
/** Recipes use `_` as a blank placeholder; define it for the editor. */
#define _
#endif

#endif /* REFLECTC_INTELLISENSE_H */
