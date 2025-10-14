#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "greatest.h"

#include "json-build/json-build.h"
#include "jsmn-find/test/jsmn_1.1.0.h"
#include "jsmn-find/jsmn-find.h"

#include "reflect-c.h"

#include "reflect-c_GENERATED.h"

#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

static void
json_stringify(struct jsonb *jb,
               const struct reflectc *member,
               char buf[],
               const size_t bufsize)
{
    const void *value = reflectc_get(member);
    if (member->decorator.length && member->ptr_value == NULL) {
        jsonb_null(jb, buf, bufsize);
        return;
    }

    switch (member->type) {
    case REFLECTC_TYPES__char:
        jsonb_string(jb, buf, bufsize, value, strlen(value));
        break;
    case REFLECTC_TYPES__int:
        jsonb_number(jb, buf, bufsize, *(int *)value);
        break;
    case REFLECTC_TYPES__bool:
        jsonb_bool(jb, buf, bufsize, *(bool *)value);
        break;
    case REFLECTC_TYPES__float:
        jsonb_number(jb, buf, bufsize, *(float *)value);
        break;
    case REFLECTC_TYPES__enum:
        jsonb_number(jb, buf, bufsize, (double)*(const int *)value);
        break;
    case REFLECTC_TYPES__struct:
    case REFLECTC_TYPES__union: {
        if (reflectc_length(member) > 1) {
            jsonb_array(jb, buf, bufsize);
            for (size_t i = 0; i < member->length; ++i) {
                json_stringify(jb, &member->members.array[i], buf, bufsize);
            }
            jsonb_array_pop(jb, buf, bufsize);
        }
        else {
            jsonb_object(jb, buf, bufsize);
            for (size_t i = 0; i < member->members.length; ++i) {
                const struct reflectc *f = &member->members.array[i];
                jsonb_key(jb, buf, bufsize, f->name.buf, f->name.length);
                json_stringify(jb, f, buf, bufsize);
            }
            jsonb_object_pop(jb, buf, bufsize);
        }
    } break;
    default:
        /* In a real implementation we would have an user callback here for
         *  custom types */
        break;
    }
}

static void
_json_parse(const jsmnf_pair *p,
            const char *json,
            size_t length,
            const struct reflectc *member)
{
    if (!p || !member) {
        printf("Invalid parameters to _json_parse\n");
        return;
    }

    if (p->k) {
        printf("Type: %d, Length: %zu, Key: %.*s\n", p->v->type, p->length,
               p->k->end - p->k->start, json + p->k->start);
    }
    printf("Value: %.*s\n", p->v->end - p->v->start, json + p->v->start);
    switch (p->v->type) {
    case JSMN_STRING: {
        if (member->type != REFLECTC_TYPES__char
            || reflectc_get_pointer_depth(member) != 2)
        {
            return;
        }
        reflectc_string(member, json + p->v->start, p->v->end - p->v->start);
        printf("Parsing string: %.*s\n", (int)(p->v->end - p->v->start),
               json + p->v->start);
    } break;
    case JSMN_PRIMITIVE: {
        switch (json[p->v->start]) {
        case 't':
        case 'f': {
            const bool value = json[p->v->start] == 't';
            if (member->type != REFLECTC_TYPES__bool) {
                return;
            }
            reflectc_set(member, &value, sizeof(value));
            printf("Parsing boolean: %s\n", value ? "true" : "false");
        } break;
        case 'n': {
            const void *value = NULL;
            if (reflectc_get_pointer_depth(member) < 2) {
                return;
            }
            reflectc_set(member, &value, sizeof(value));
        } break;
        default: {
            switch (member->type) {
            case REFLECTC_TYPES__float: {
                const float value = strtof(json + p->v->start, NULL);
                reflectc_set(member, &value, sizeof(value));
            } break;
            case REFLECTC_TYPES__double: {
                const double value = strtod(json + p->v->start, NULL);
                reflectc_set(member, &value, sizeof(value));
            } break;
            case REFLECTC_TYPES__int: {
                const int value = (int)strtol(json + p->v->start, NULL, 10);
                reflectc_set(member, &value, sizeof(value));
            } break;
            case REFLECTC_TYPES__enum: {
                const int value = (int)strtol(json + p->v->start, NULL, 10);
                reflectc_set(member, &value, member->size);
            } break;
            case REFLECTC_TYPES__long: {
                const long value = strtol(json + p->v->start, NULL, 10);
                reflectc_set(member, &value, sizeof(value));
            } break;
            case REFLECTC_TYPES__short: {
                const short value =
                    (short)strtol(json + p->v->start, NULL, 10);
                reflectc_set(member, &value, sizeof(value));
            } break;
            case REFLECTC_TYPES__char: {
                const char value = (char)strtol(json + p->v->start, NULL, 10);
                reflectc_set(member, &value, sizeof(value));
            } break;
            default:
                break;
            }
        } break;
        }
        break;
    }
    case JSMN_OBJECT:
        if (member->type != REFLECTC_TYPES__struct) {
            printf("Expected a struct type for JSON object\nGot: %d\n",
                   member->type);
            return;
        }
        reflectc_expand(member);
        for (size_t i = 0; i < p->capacity; ++i) {
            const size_t pos = reflectc_get_pos(member, p->buckets[i].key.buf,
                                                p->buckets[i].key.length);
            printf("1. Parsing field (%zu): %.*s\n", pos,
                   (int)p->buckets[i].key.length, p->buckets[i].key.buf);
            if (pos != SIZE_MAX) {
                const jsmnf_pair *p_m = jsmnf_find(p, p->buckets[i].key.buf,
                                                   p->buckets[i].key.length);
                const struct reflectc *m = member->members.array + pos;
                printf("2. Parsing field: %.*s\n",
                       (int)p->buckets[i].key.length, p->buckets[i].key.buf);
                _json_parse(p_m, json, length, m);
            }
        }
        break;
    case JSMN_ARRAY:
        reflectc_array(member, p->length);
        for (size_t i = 0; i < p->length; ++i) {
            _json_parse(&p->fields[i], json, length, &member[i]);
        }
        break;
    default:
        break;
    }
}

static void
json_parse(const char *json, size_t len, struct reflectc *root)
{
    jsmnf_loader l;
    jsmnf_table *pairs = NULL;
    size_t pairs_len = 0;
    jsmnf_init(&l);
    if (jsmnf_load_auto(&l, json, len, &pairs, &pairs_len) > 0) {
        _json_parse(l.root, json, len, root);
        free(pairs);
    }
}

TEST
check_json_serializer(void)
{
    static const char expected_json[] =
        "{\"a\":{\"boolean\":true,\"number\":42,\"string\":\"hello world\"},"
        "\"b\":{\"boolean\":true,\"number\":42,\"string\":\"hello world\"},"
        "\"c\":{\"boolean\":true,\"number\":42,\"string\":\"hello world\"},"
        "\"d\":\"hello world\",\"e\":1}";
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
    struct baz baz = { &a, &a, &aaa, "hello world", LEVELS_ONE };
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
    expected_pair = jsmnf_find(expected_l.root, "e", 1);
    got_pair = jsmnf_find(got_l.root, "e", 1);
    ASSERT_MEM_EQ(expected_json + expected_pair->v->start,
                  got_json + got_pair->v->start,
                  got_pair->v->end - got_pair->v->start);

    PASS();
}

TEST
check_json_parser(void)
{
    static const char json[] =
        "{\"a\":{\"boolean\":true,\"number\":42,\"string\":\"hello world\"},"
        "\"b\":{\"boolean\":false,\"number\":123,\"string\":\"test string\"},"
        "\"c\":{\"boolean\":true,\"number\":99,\"string\":\"another string\"},"
        "\"d\":\"parsed text\",\"e\":3}";

    struct bar a = { 0 }, b = { 0 }, c = { 0 }, *cc = &c, **ccc = &cc;
    char d[32] = { 0 };

    struct baz baz = { &a, &b, &ccc, d, LEVELS_ONE };
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);

    json_parse(json, sizeof(json) - 1, wrapped_baz);

    ASSERT_EQ(true, a.boolean);
    ASSERT_EQ(42, a.number);
    ASSERT_STR_EQ("hello world", a.string);

    ASSERT_EQ(false, b.boolean);
    ASSERT_EQ(123, b.number);
    ASSERT_STR_EQ("test string", b.string);

    ASSERT_EQ(true, c.boolean);
    ASSERT_EQ(99, c.number);
    ASSERT_STR_EQ("another string", c.string);

    ASSERT_STR_EQ("parsed text", d);
    ASSERT_EQ(LEVELS_THREE, baz.e);

    PASS();
}

TEST
check_resolve_and_expand(void)
{
    struct bar value = { true, 7, "hydrate" };
    struct bar *first = &value;
    struct bar **second = &first;
    struct baz baz = { &value, &value, &second, "hydrate", LEVELS_TWO };
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);
    size_t c_pos = reflectc_get_pos_fast(struct, baz, c, wrapped_baz);
    const struct reflectc *c_member = &wrapped_baz->members.array[c_pos];
    size_t before = c_member->members.length;
    size_t boolean_pos;
    size_t number_pos;

    ASSERT_EQ(&value, reflectc_resolve(c_member));
    ASSERT_EQ(1, reflectc_expand(c_member));
    if (before == 0) {
        ASSERT_NEQ(0, c_member->members.length);
    }

    boolean_pos = reflectc_get_pos(c_member, "boolean", strlen("boolean"));
    ASSERT_NEQ(SIZE_MAX, boolean_pos);
    ASSERT_EQ(true, *(bool *)reflectc_get_member(c_member, boolean_pos));

    number_pos = reflectc_get_pos(c_member, "number", strlen("number"));
    ASSERT_NEQ(SIZE_MAX, number_pos);
    ASSERT_EQ(value.number, *(int *)reflectc_get_member(c_member, number_pos));

    PASS();
}

TEST
check_resolve_null_chain(void)
{
    struct bar a = { 0 };
    struct bar b = { 0 };
    struct bar *null_leaf = NULL;
    struct bar **null_chain = &null_leaf;
    struct bar ***null_root = &null_chain;
    struct baz baz = { &a, &b, null_root, NULL, LEVELS_ONE };
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);
    size_t c_pos = reflectc_get_pos_fast(struct, baz, c, wrapped_baz);
    const struct reflectc *c_member = &wrapped_baz->members.array[c_pos];

    ASSERT_EQ(NULL, reflectc_resolve(c_member));
    ASSERT_EQ(0, reflectc_expand(c_member));

    PASS();
}

TEST
check_extended_type_metadata(void)
{
    struct hooks sample = { 21, true, (size_t)512, (unsigned long)7 };
    struct reflectc *wrapper = reflectc_from_hooks(&sample, NULL);
    size_t words_pos = reflectc_get_pos_fast(struct, hooks, words, wrapper);
    const struct reflectc *words_member = &wrapper->members.array[words_pos];
    const reflectc_words_t *words_ptr =
        reflectc_get_member(wrapper, words_pos);
    size_t numbers_pos =
        reflectc_get_pos_fast(struct, hooks, numbers, wrapper);
    const struct reflectc *numbers_member =
        &wrapper->members.array[numbers_pos];
    const reflectc_numbers_t *numbers_ptr =
        reflectc_get_member(wrapper, numbers_pos);

    ASSERT_EQ((enum reflectc_types)REFLECTC_TYPES__reflectc_words_t,
              words_member->type);
    ASSERT_EQ(sizeof(reflectc_words_t), words_member->size);
    ASSERT_NEQ(NULL, words_ptr);
    ASSERT_EQ(sample.words, *words_ptr);

    ASSERT_EQ((enum reflectc_types)REFLECTC_TYPES__reflectc_numbers_t,
              numbers_member->type);
    ASSERT_EQ(sizeof(reflectc_numbers_t), numbers_member->size);
    ASSERT_NEQ(NULL, numbers_ptr);
    ASSERT_EQ(sample.numbers, *numbers_ptr);

    reflectc_from_hooks(NULL, wrapper);
    words_member = &wrapper->members.array[words_pos];
    words_ptr = reflectc_get_member(wrapper, words_pos);
    ASSERT_NEQ(NULL, words_member);
    ASSERT_NEQ(NULL, words_ptr);
    ASSERT_EQ((enum reflectc_types)REFLECTC_TYPES__reflectc_words_t,
              words_member->type);

    numbers_member = &wrapper->members.array[numbers_pos];
    numbers_ptr = reflectc_get_member(wrapper, numbers_pos);
    ASSERT_NEQ(NULL, numbers_member);
    ASSERT_NEQ(NULL, numbers_ptr);
    ASSERT_EQ((enum reflectc_types)REFLECTC_TYPES__reflectc_numbers_t,
              numbers_member->type);

    free(wrapper);
    PASS();
}

TEST
check_loop_through(void)
{
    struct bar a, b, c, *cc = &c, **ccc = &cc;
    char d[] = "hello world";

    struct baz baz = { &a, &b, &ccc, d, LEVELS_ONE };
    struct reflectc *wrapped_baz = reflectc_from_baz(&baz, NULL);
    size_t pos = reflectc_get_pos_fast(struct, baz, a, wrapped_baz);
    ASSERT_EQ(&a, reflectc_get_member(wrapped_baz, pos));
    pos = reflectc_get_pos_fast(struct, baz, b, wrapped_baz);
    ASSERT_EQ(&b, reflectc_get_member(wrapped_baz, pos));
    pos = reflectc_get_pos_fast(struct, baz, c, wrapped_baz);
    ASSERT_EQ(&ccc, reflectc_get_member(wrapped_baz, pos));
    pos = reflectc_get_pos_fast(struct, baz, d, wrapped_baz);
    ASSERT_MEM_EQ(&d, reflectc_get_member(wrapped_baz, pos), sizeof(d));

    PASS();
}

TEST
check_array(void)
{
    struct {
        int number;
    } native = { 42 };
    struct foo foo = { true, { 42, 43, 44, 45 }, "hello world", &native };
    struct reflectc *wrapped_foo = reflectc_from_foo(&foo, NULL);
    size_t pos = reflectc_get_pos_fast(struct, foo, boolean, wrapped_foo);
    ASSERT_EQ(foo.boolean, *(bool *)reflectc_get_member(wrapped_foo, pos));
    pos = reflectc_get_pos_fast(struct, foo, number, wrapped_foo);
    ASSERT_MEM_EQ(foo.number, reflectc_get_member(wrapped_foo, pos),
                  sizeof(foo.number));
    pos = reflectc_get_pos_fast(struct, foo, string, wrapped_foo);
    ASSERT_STR_EQ(foo.string, reflectc_get_member(wrapped_foo, pos));
    PASS();
}

SUITE(wrapper)
{
    RUN_TEST(check_json_serializer);
    RUN_TEST(check_json_parser);
    RUN_TEST(check_resolve_and_expand);
    RUN_TEST(check_resolve_null_chain);
    RUN_TEST(check_extended_type_metadata);
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
