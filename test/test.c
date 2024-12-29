#include <stdio.h>
#include <stdlib.h>

#include "greatest.h"

#include "json-build/json-build.h"
#include "jsmn-find/test/jsmn_1.1.0.h"
#include "jsmn-find/jsmn-find.h"

#include "reflect-c.h"
#include "reflect-c_internal.h"

#include "reflect-c_GENERATED.h"

static void
json_stringify(struct jsonb *jb,
               struct reflectc *reflectc,
               char buf[],
               const size_t bufsize)
{
    if (reflectc->ptr_depth != 0 && reflectc->value == NULL) {
        jsonb_null(jb, buf, bufsize);
        return;
    }

    switch (reflectc->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize, reflectc->value,
                     strlen(reflectc->value));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize, *(int *)reflectc->value);
        break;
    case REFLECTC_TYPES__bool:
        jsonb_bool(jb, buf, bufsize, *(bool *)reflectc->value);
        break;
    case REFLECTC_TYPES__float:
        jsonb_number(jb, buf, bufsize, *(float *)reflectc->value);
        break;
    case REFLECTC_TYPES__struct: {
        const struct oa_hash *ht = &reflectc->ht;
        jsonb_object(jb, buf, bufsize);
        for (size_t i = 0; i < ht->capacity; ++i) {
            const struct oa_hash_entry *entry = &ht->buckets[i];
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
    const char expected[] =
        "{\"a\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"b\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"c\":{\"string\":\"hello world\",\"boolean\":true,\"number\":42},"
        "\"d\":\"hello world\"}";

    // Tokenize the expected JSON string
    jsmn_parser expected_p;
    jsmntok_t expected_tokens[64];
    jsmn_init(&expected_p);
    jsmn_parse(&expected_p, expected, sizeof(expected) - 1, expected_tokens,
               64);
    jsmnf_loader expected_l;
    jsmnf_pair expected_pairs[64], *expected_pair = NULL;
    jsmnf_init(&expected_l);
    jsmnf_load(&expected_l, expected, expected_tokens, expected_p.toknext,
               expected_pairs, 64);

    // Generate the JSON string
    struct bar a = { true, 42, "hello world" };
    struct baz baz = { &a, &a, &a, "hello world" };
    struct reflectc *reflectc =
        reflectc_from_baz(&baz, REFLECTC_MODES_READONLY, NULL);
    char json[sizeof(expected)] = { 0 };
    struct jsonb jb;
    jsonb_init(&jb);
    json_stringify(&jb, reflectc, json, sizeof(json));

    // Tokenize the generated JSON string
    jsmn_parser p;
    jsmntok_t tokens[64];
    jsmn_init(&p);
    jsmn_parse(&p, json, sizeof(json) - 1, tokens, 64);
    jsmnf_loader l;
    jsmnf_pair pairs[64], *pair = NULL;
    jsmnf_init(&l);
    jsmnf_load(&l, json, tokens, p.toknext, pairs, 64);

    // Compare the tokenized JSON strings
    expected_pair = jsmnf_find(expected_pairs, expected, "a", 1);
    pair = jsmnf_find(pairs, json, "a", 1);
    ASSERT_EQ(expected_pair->v.len, pair->v.len);
    ASSERT_STRN_EQ(expected + expected_pair->v.pos, json + pair->v.pos,
                   pair->v.len);

    expected_pair = jsmnf_find(expected_pairs, expected, "b", 1);
    pair = jsmnf_find(pairs, json, "b", 1);
    ASSERT_EQ(expected_pair->v.len, pair->v.len);
    ASSERT_STRN_EQ(expected + expected_pair->v.pos, json + pair->v.pos,
                   pair->v.len);

    expected_pair = jsmnf_find(expected_pairs, expected, "c", 1);
    pair = jsmnf_find(pairs, json, "c", 1);
    ASSERT_EQ(expected_pair->v.len, pair->v.len);
    ASSERT_STRN_EQ(expected + expected_pair->v.pos, json + pair->v.pos,
                   pair->v.len);

    expected_pair = jsmnf_find(expected_pairs, expected, "d", 1);
    pair = jsmnf_find(pairs, json, "d", 1);
    ASSERT_EQ(expected_pair->v.len, pair->v.len);
    ASSERT_STRN_EQ(expected + expected_pair->v.pos, json + pair->v.pos,
                   pair->v.len);

    PASS();
}

TEST
check_loop_through(void)
{
    struct bar a, b, c;
    char d[] = "hello world";

    struct baz baz = { &a, &b, &c, d };
    struct reflectc *reflectc =
        reflectc_from_baz(&baz, REFLECTC_MODES_READONLY, NULL);

    ASSERT_EQ(&a, ((struct bar *)reflectc_get_field(
                       reflectc, (struct reflectc_name){ 1, "a" })
                       ->value));
    ASSERT_EQ(&b, ((struct bar *)reflectc_get_field(
                       reflectc, (struct reflectc_name){ 1, "b" })
                       ->value));
    ASSERT_EQ(&c, ((struct bar *)reflectc_get_field(
                       reflectc, (struct reflectc_name){ 1, "c" })
                       ->value));
    ASSERT_STR_EQ(&d, ((char *)reflectc_get_field(
                           reflectc, (struct reflectc_name){ 1, "d" })
                           ->value));

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
