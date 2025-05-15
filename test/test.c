#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "greatest.h"

#include "json-build/json-build.h"
#include "jsmn-find/test/jsmn_1.1.0.h"
#include "jsmn-find/jsmn-find.h"

#include "reflect-c.h"

#include "reflect-c_GENERATED.h"

/* partial implementation just for testing purposes (missing array parsing and
 * more) */
static void
json_stringify(struct jsonb *jb,
               struct reflectc *field,
               char buf[],
               const size_t bufsize)
{
    if (field->decorator.len && *(char **)field->value == NULL) {
        jsonb_null(jb, buf, bufsize);
        return;
    }

    switch (field->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize, *(char **)field->value,
                     strlen(*(char **)field->value));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize, *(int *)field->value);
        break;
    case REFLECTC_TYPES__bool:
        jsonb_bool(jb, buf, bufsize, *(bool *)field->value);
        break;
    case REFLECTC_TYPES__float:
        jsonb_number(jb, buf, bufsize, *(float *)field->value);
        break;
    case REFLECTC_TYPES__struct: {
        jsonb_object(jb, buf, bufsize);
        for (size_t i = 0; i < field->capacity; ++i) {
            const struct oa_hash_entry *entry = &field->buckets[i];
            if (entry->state != OA_HASH_ENTRY_OCCUPIED) continue;

            struct reflectc *field = entry->value;
            jsonb_key(jb, buf, bufsize, field->name.buf, field->name.len);
            json_stringify(jb, field, buf, bufsize);
        }
        jsonb_object_pop(jb, buf, bufsize);
    } break;
    default:
        break;
    }
}

TEST
check_json_serializer(void)
{
    static const char expected_json[] =
        "{\"a\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"b\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"c\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"d\":\"hello world\"}";
    char got_json[sizeof(expected_json)] = { 0 };

    jsmnf_loader expected_l, got_l;
    jsmnf_table expected_pairs[64],
        got_pairs[sizeof(expected_pairs) / sizeof *expected_pairs];
    const jsmnf_pair *expected_pair = NULL, *got_pair = NULL;
    jsmnf_init(&expected_l);
    jsmnf_load(&expected_l, expected_json, sizeof(expected_json) - 1,
               expected_pairs,
               sizeof(expected_pairs) / sizeof *expected_pairs);

    // Generate the JSON string
    struct bar a = { true, 42, "hello world" }, *aa = &a, **aaa = &aa;
    struct baz baz = { &a, &a, &aaa, "hello world" };
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, 1, NULL);
    struct jsonb jb;
    jsonb_init(&jb);
    json_stringify(&jb, wrapped_baz, got_json, sizeof(got_json));

    // Tokenize the generated JSON string
    jsmnf_init(&got_l);
    jsmnf_load(&got_l, got_json, sizeof(got_json) - 1, got_pairs,
               sizeof(got_pairs) / sizeof *got_pairs);

    // Compare the tokenized JSON strings
    expected_pair = jsmnf_find(expected_l.root, "a", 1);
    got_pair = jsmnf_find(got_l.root, "a", 1);
    ASSERT_EQ(expected_pair->v->end - expected_pair->v->start,
              got_pair->v->end - got_pair->v->start);
    ASSERT_STRN_EQ(expected_json + expected_pair->v->start,
                   got_json + got_pair->v->start,
                   got_pair->v->end - got_pair->v->start);

    expected_pair = jsmnf_find(expected_l.root, "b", 1);
    got_pair = jsmnf_find(got_l.root, "b", 1);
    ASSERT_EQ(expected_pair->v->end - expected_pair->v->start,
              got_pair->v->end - got_pair->v->start);
    ASSERT_STRN_EQ(expected_json + expected_pair->v->start,
                   got_json + got_pair->v->start,
                   got_pair->v->end - got_pair->v->start);

    expected_pair = jsmnf_find(expected_l.root, "c", 1);
    got_pair = jsmnf_find(got_l.root, "c", 1);
    ASSERT_EQ(expected_pair->v->end - expected_pair->v->start,
              got_pair->v->end - got_pair->v->start);
    ASSERT_STRN_EQ(expected_json + expected_pair->v->start,
                   got_json + got_pair->v->start,
                   got_pair->v->end - got_pair->v->start);

    expected_pair = jsmnf_find(expected_l.root, "d", 1);
    got_pair = jsmnf_find(got_l.root, "d", 1);
    ASSERT_EQ(expected_pair->v->end - expected_pair->v->start,
              got_pair->v->end - got_pair->v->start);
    ASSERT_STRN_EQ(expected_json + expected_pair->v->start,
                   got_json + got_pair->v->start,
                   got_pair->v->end - got_pair->v->start);

    PASS();
}

TEST
check_loop_through(void)
{
    struct bar a, b, c, *cc = &c, **ccc = &cc;
    char d[] = "hello world";

    struct baz baz = { &a, &b, &ccc, d };
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, 1, NULL);

    ASSERT_MEM_EQ(&d,
                  (*(char **)reflectc_get_field(wrapped_baz, "d", 1)->value),
                  sizeof(d));
    ASSERT_EQ(
        &a, *((struct bar **)reflectc_get_field(wrapped_baz, "a", 1)->value));
    ASSERT_EQ(
        &b, *((struct bar **)reflectc_get_field(wrapped_baz, "b", 1)->value));
    ASSERT_EQ(
        &ccc,
        *((struct bar ****)reflectc_get_field(wrapped_baz, "c", 1)->value));

    PASS();
}

SUITE(wrapper)
{
    RUN_TEST(check_loop_through);
    RUN_TEST(check_json_serializer);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(wrapper);

    GREATEST_MAIN_END();
}
