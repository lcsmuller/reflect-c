#include <stdlib.h>
#include <string.h>

#include "reflect-c_internal.h"

struct reflectc_field *
reflectc_get_field(struct reflectc *reflectc, const struct reflectc_name name)
{
    return oa_hash_get(&reflectc->ht, name.buf, name.len);
}

void
reflectc_add_field(struct reflectc *reflectc,
                   struct reflectc_field *field,
                   void *value)
{
    struct reflectc *new_field = calloc(1, sizeof *new_field);
    memcpy(new_field, field, sizeof *field);
    if (value && !field->value) new_field->value = value;
    oa_hash_set(&reflectc->ht, new_field->name.buf, new_field->name.len,
                new_field);
}
