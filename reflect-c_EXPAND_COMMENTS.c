#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static const char COMMENT_OPEN[] = "/*#!";
static const char COMMENT_CLOSE[] = "*/";

enum reflectc_comment_state { REFLECTC_COMMENT_OFF = 0, REFLECTC_COMMENT_ON };

int
main()
{
    enum reflectc_comment_state state = REFLECTC_COMMENT_OFF;
    size_t open_match = 0;
    size_t close_match = 0;
    char open_buf[sizeof(COMMENT_OPEN)] = { 0 };
    size_t i;
    int ch;

    while ((ch = fgetc(stdin)) != EOF) {
        if (state == REFLECTC_COMMENT_OFF) {
            if (open_match > 0) {
                if (ch == COMMENT_OPEN[open_match]) {
                    open_buf[open_match++] = (char)ch;
                    if (open_match == sizeof(COMMENT_OPEN) - 1) {
                        putchar('\n');
                        open_match = 0;
                        close_match = 0;
                        state = REFLECTC_COMMENT_ON;
                    }
                    continue;
                }
                for (i = 0; i < open_match; ++i) {
                    putchar(open_buf[i]);
                }
                open_match = 0;
            }

            if (ch == COMMENT_OPEN[0]) {
                open_buf[open_match++] = (char)ch;
                continue;
            }

            putchar(ch);
            continue;
        }

        if (close_match == 0) {
            if (ch == COMMENT_CLOSE[0]) {
                close_match = 1;
                continue;
            }
            putchar(ch);
            continue;
        }

        if (ch == COMMENT_CLOSE[1]) {
            close_match = 0;
            state = REFLECTC_COMMENT_OFF;
            continue;
        }

        putchar(COMMENT_CLOSE[0]);
        close_match = 0;

        if (ch == COMMENT_CLOSE[0]) {
            close_match = 1;
            continue;
        }

        putchar(ch);
    }

    if (open_match > 0) {
        for (i = 0; i < open_match; ++i) {
            putchar(open_buf[i]);
        }
    }

    if (state == REFLECTC_COMMENT_ON && close_match == 1) {
        putchar(COMMENT_CLOSE[0]);
    }

    return EXIT_SUCCESS;
}
