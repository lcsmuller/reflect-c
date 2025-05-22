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
    if (field->decorator.len && field->ptr_value == NULL) {
        jsonb_null(jb, buf, bufsize);
        return;
    }

    switch (field->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize, *(char **)field->ptr_value,
                     strlen(*(char **)field->ptr_value));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize, *(int *)field->ptr_value);
        break;
    case REFLECTC_TYPES__bool:
        jsonb_bool(jb, buf, bufsize, *(bool *)field->ptr_value);
        break;
    case REFLECTC_TYPES__float:
        jsonb_number(jb, buf, bufsize, *(float *)field->ptr_value);
        break;
    case REFLECTC_TYPES__struct: {
        jsonb_object(jb, buf, bufsize);
        for (size_t i = 0; i < field->ht.capacity; ++i) {
            const struct oa_hash_entry *entry = &field->ht.buckets[i];
            if (entry->state != OA_HASH_ENTRY_OCCUPIED) continue;

            struct reflectc *field = entry->value;
            jsonb_key(jb, buf, bufsize, entry->key.buf, entry->key.length);
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
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);
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
    ASSERT_MEM_EQ(expected_json + expected_pair->v->start,
                  got_json + got_pair->v->start,
                  got_pair->v->end - got_pair->v->start);

    expected_pair = jsmnf_find(expected_l.root, "b", 1);
    got_pair = jsmnf_find(got_l.root, "b", 1);
    ASSERT_MEM_EQ(expected_json + expected_pair->v->start,
                  got_json + got_pair->v->start,
                  got_pair->v->end - got_pair->v->start);

    expected_pair = jsmnf_find(expected_l.root, "c", 1);
    got_pair = jsmnf_find(got_l.root, "c", 1);
    ASSERT_MEM_EQ("{}", got_json + got_pair->v->start,
                  got_pair->v->end - got_pair->v->start);

    expected_pair = jsmnf_find(expected_l.root, "d", 1);
    got_pair = jsmnf_find(got_l.root, "d", 1);
    ASSERT_MEM_EQ(expected_json + expected_pair->v->start,
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
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);

    ASSERT_EQ(&a, *reflectc_get(struct bar **)(wrapped_baz, "a", 1));
    ASSERT_EQ(&b, *reflectc_get(struct bar **)(wrapped_baz, "b", 1));
    ASSERT_EQ(&ccc, *reflectc_get(struct bar ****)(wrapped_baz, "c", 1));
    ASSERT_MEM_EQ(&d, *reflectc_get(char **)(wrapped_baz, "d", 1), sizeof(d));

    PASS();
}

TEST
check_array(void)
{
    struct foo foo = { true, { 42, 43, 44, 45 }, "hello world" };
    struct reflectc *wrapped_foo = reflectc_from_foo(&foo, NULL);

    int(*ptr_numbers)[4] = reflectc_get(int(*)[4])(wrapped_foo, "number", 6);
    ASSERT_EQ(42, (*ptr_numbers)[0]);
    ASSERT_EQ(43, (*ptr_numbers)[1]);
    ASSERT_EQ(44, (*ptr_numbers)[2]);
    ASSERT_EQ(45, (*ptr_numbers)[3]);

    PASS();
}

SUITE(wrapper)
{
    RUN_TEST(check_json_serializer);
    RUN_TEST(check_loop_through);
    RUN_TEST(check_array);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(wrapper);

    GREATEST_MAIN_END();
}
