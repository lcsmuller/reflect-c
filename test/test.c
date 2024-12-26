#include <stdio.h>
#include <stdlib.h>

#include "greatest.h"
#include "json-build/json-build.h"

#include "out.h"

static const char *
json_stringify(struct jsonb *jb,
               struct cogchef *cogchef,
               char buf[],
               const size_t bufsize)
{
    struct oa_hash *ht = &cogchef->ht;

    jsonb_object(jb, buf, bufsize);
    for (size_t i = 0; i < ht->capacity; ++i) {
        struct oa_hash_entry *entry = &ht->buckets[i];
        struct cogchef *field = entry->value;

        if (entry->state != OA_HASH_ENTRY_OCCUPIED
            || (field->name.len == 5
                && 0 == memcmp(field->name.buf, "$self", 5)))
        {
            continue;
        }

        jsonb_key(jb, buf, bufsize, field->name.buf, field->name.len);
        if (field->ptr_depth != 0 && field->value == NULL) {
            jsonb_null(jb, buf, bufsize);
            continue;
        }

        switch (field->type) {
        case COGCHEF_TYPES__char:
            jsonb_string(jb, buf, bufsize, *(char **)field->value,
                         strlen(*(char **)field->value));
            break;
        case COGCHEF_TYPES__int:
            jsonb_number(jb, buf, bufsize, *(int *)field->value);
            break;
        case COGCHEF_TYPES__bool:
            jsonb_bool(jb, buf, bufsize, *(bool *)field->value);
            break;
        case COGCHEF_TYPES__struct:
            json_stringify(jb, field, buf, bufsize);
            break;
        default:
            break;
        }
    }
    jsonb_object_pop(jb, buf, bufsize);
    return buf;
}

TEST
check_json_serializer(void)
{
    const char *expected =
        "{\"a\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"b\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"c\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"d\":\"hello world\"}";
    char buf[2048] = { 0 };

    struct bar a = { true, 42, "hello world" };
    struct baz baz = { &a, &a, &a, "hello world" };
    struct cogchef *cogchef =
        cogchef_from_baz(&baz, COGCHEF_MODES_READONLY, NULL);
    struct jsonb jb;

    jsonb_init(&jb);

    ASSERT_EQ(true,
              (*(struct bar **)cogchef_get(cogchef, "a", 1)->value)->boolean);
    ASSERT_EQ(42,
              (*(struct bar **)cogchef_get(cogchef, "b", 1)->value)->number);
    ASSERT_STR_EQ(
        "hello world",
        (*(struct bar **)cogchef_get(cogchef, "c", 1)->value)->string);
    ASSERT_STR_EQ("hello world",
                  (*(char **)cogchef_get(cogchef, "d", 1)->value));

    ASSERT_STR_EQ(expected, json_stringify(&jb, cogchef, buf, sizeof(buf)));

    PASS();
}

TEST
check_loop_through(void)
{
    struct bar a, b, c;
    char d[] = "hello world";

    struct baz baz = { &a, &b, &c, d };
    struct cogchef *cogchef =
        cogchef_from_baz(&baz, COGCHEF_MODES_READONLY, NULL);

    ASSERT_EQ(&a, (*(struct bar **)cogchef_get(cogchef, "a", 1)->value));
    ASSERT_EQ(&b, (*(struct bar **)cogchef_get(cogchef, "b", 1)->value));
    ASSERT_EQ(&c, (*(struct bar **)cogchef_get(cogchef, "c", 1)->value));
    ASSERT_STR_EQ(&d, (*(char **)cogchef_get(cogchef, "d", 1)->value));

    PASS();
}

SUITE(helper)
{
    RUN_TEST(check_json_serializer);
    RUN_TEST(check_loop_through);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(helper);

    GREATEST_MAIN_END();
}
