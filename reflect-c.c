#include <stdlib.h>
#include <string.h>

#include "reflect-c_internal.h"

struct reflectc *
reflectc_get(struct reflectc *reflectc, const char *key, const size_t key_len)
{
    return oa_hash_get(&reflectc->ht, key, key_len);
}

struct reflectc *
reflectc_set(struct reflectc *reflectc,
             const char *key,
             const size_t key_len,
             void *value)
{
    return (reflectc->mode & REFLECTC_MODES_WRITE)
               ? oa_hash_set(&reflectc->ht, key, key_len, value)
               : NULL;
}
