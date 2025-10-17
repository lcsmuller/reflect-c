#include <stdio.h>
#include <stdlib.h>

static void
print_prelude(int N)
{
    puts("#ifndef REFLECTC_TUPLE_H");
    puts("#define REFLECTC_TUPLE_H");
    puts("");
    puts("#define EVAL(_x)     _x");
    puts("#define JOIN(_1, _2) _1 _2");
    puts("");
    puts("#define REFLECTC_TUPLE_FOREACH(_expr, _extra, _ntuple, _tuple)      "
         "          \\");
    puts("    EVAL(JOIN(REFLECTC_TUPLE_FOREACH_##_ntuple,                     "
         "          \\");
    puts("              REFLECTC_TUPLE_PREPEND(2, (_expr, _extra), _ntuple, "
         "_tuple)))");
    puts("");
    puts("#define REFLECTC_TUPLE_APPEND(_ntuple, _tuple, _nvalues, _values)   "
         "          \\");
    puts("    (REFLECTC_TUPLE_SPREAD_##_ntuple _tuple,                        "
         "          \\");
    puts("     REFLECTC_TUPLE_SPREAD_##_nvalues _values)");
    puts("#define REFLECTC_TUPLE_PREPEND(_nvalues, _values, _ntuple, _tuple)  "
         "          \\");
    puts("    (REFLECTC_TUPLE_SPREAD_##_nvalues _values,                      "
         "          \\");
    puts("     REFLECTC_TUPLE_SPREAD_##_ntuple _tuple)");
    puts("");
}

static void
print_footer(void)
{
    puts("");
    puts("#endif /* REFLECTC_TUPLE_H */");
}

static void
print_spread_macros(int N)
{
    int i, k;
    puts("#define REFLECTC_TUPLE_SPREAD_0");
    for (i = 1; i <= N; ++i) {
        printf("#define REFLECTC_TUPLE_SPREAD_%d(", i);
        for (k = 1; k <= i; ++k) {
            if (k > 1) fputs(", ", stdout);
            printf("_%d", k);
        }
        fputs(") ", stdout);
        if (i <= 6) {
            for (k = 1; k <= i; ++k) {
                if (k > 1) fputs(", ", stdout);
                printf("_%d", k);
            }
            fputc('\n', stdout);
        }
        else {
            puts("\\");
            fputs("    ", stdout);
            for (k = 1; k <= i; ++k) {
                if (k > 1) fputs(", ", stdout);
                printf("_%d", k);
            }
            fputc('\n', stdout);
        }
    }
    puts("");
}

static void
print_foreach_macros(int N)
{
    int i, k;
    puts("#define REFLECTC_TUPLE_FOREACH_0");
    for (i = 1; i <= N; ++i) {
        printf("#define REFLECTC_TUPLE_FOREACH_%d(_expr, _extra, ", i);
        for (k = 1; k <= i; ++k) {
            if (k > 1) fputs(", ", stdout);
            printf("_%d", k);
        }
        puts(") \\");
        for (k = 1; k <= i; ++k) {
            printf("    _expr(_extra, _%d)", k);
            if (k < i)
                puts(" \\");
            else
                puts("");
        }
    }
}

int
main(int argc, char **argv)
{
    int N = 16;
    if (argc >= 2) {
        N = (int)strtol(argv[1], NULL, 10);
        if (N <= 0) N = 16;
    }
    print_prelude(N);
    print_spread_macros(N);
    print_foreach_macros(N);
    print_footer();
    return 0;
}
