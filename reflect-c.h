#ifndef REFLECTC_H
#define REFLECTC_H

enum reflectc_modes { REFLECTC_MODES_WRITE, REFLECTC_MODES_READONLY };

enum reflectc_types {
    REFLECTC_TYPES__char,
    REFLECTC_TYPES__short,
    REFLECTC_TYPES__int,
    REFLECTC_TYPES__long,
    REFLECTC_TYPES__float,
    REFLECTC_TYPES__double,
    REFLECTC_TYPES__bool,
    REFLECTC_TYPES__struct,
    REFLECTC_TYPES__EXTEND
};

/* forward definition */
struct reflectc;
/**/

typedef struct reflectc *(*const reflectc_from_type_t)(void *,
                                                       enum reflectc_modes,
                                                       struct reflectc *);

struct reflectc *reflectc_get(struct reflectc *reflectc,
                              const char *key,
                              const size_t key_len);

struct reflectc *reflectc_set(struct reflectc *reflectc,
                              const char *key,
                              const size_t key_len,
                              void *value);

#endif /* REFLECTC_H */
