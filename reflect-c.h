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

struct reflectc_name {
    const size_t len;
    const char *const buf;
};

#define REFLECTC_FIELD_ATTRIBUTES                                             \
    const unsigned ptr_depth;                                                 \
    const size_t size;                                                        \
    const struct reflectc_name name;                                          \
    const enum reflectc_types type;                                           \
    enum reflectc_modes mode

struct reflectc_field {
    REFLECTC_FIELD_ATTRIBUTES;
    const void *value;
};

struct reflectc_field *reflectc_get_field(struct reflectc *reflectc,
                                          const struct reflectc_name name);

void reflectc_add_field(struct reflectc *reflectc,
                        struct reflectc_field *field,
                        void *value);

#endif /* REFLECTC_H */
