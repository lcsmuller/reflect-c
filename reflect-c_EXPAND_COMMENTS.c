#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char COMMENT_OPEN[] = "/*#!";
static const char COMMENT_CLOSE[] = "*/";

enum reflectc_comment_state { REFLECTC_COMMENT_OFF = 0, REFLECTC_COMMENT_ON };

static enum reflectc_comment_state
expand_comments(char *buf, enum reflectc_comment_state state)
{
    while (*buf != '\0') {
        switch (state) {
        case REFLECTC_COMMENT_OFF:
            if (0 != strncmp(buf, COMMENT_OPEN, sizeof(COMMENT_OPEN) - 1))
                break;
            /* make sure directive is on a new line */
            buf[sizeof(COMMENT_OPEN) - 1] = '\n';
            buf += sizeof(COMMENT_OPEN) - 1;
            state = REFLECTC_COMMENT_ON;

            continue;
        case REFLECTC_COMMENT_ON:
            if (0 != strncmp(buf, COMMENT_CLOSE, sizeof(COMMENT_CLOSE) - 1))
                break;

            buf += sizeof(COMMENT_CLOSE) - 1;
            state = REFLECTC_COMMENT_OFF;

            continue;
        }

        fputc(*buf, stdout);
        ++buf;
    }
    return state;
}

int
main()
{
    enum reflectc_comment_state state = REFLECTC_COMMENT_OFF;
    size_t bufsize = 0x1000;
    char *buf = malloc(bufsize);

    if (!buf) {
        fputs("Failed to allocate buffer\n", stderr);
        return EXIT_FAILURE;
    }

    while (1) {
        const size_t count = fread(buf, sizeof(char), bufsize - 1, stdin);
        if (count == 0) break;

        buf[count] = '\0';
        state = expand_comments(buf, state);

        /* Grow buffer if needed for next read */
        if (count >= bufsize - 1) {
            char *tmp = realloc(buf, bufsize * 2);
            if (!tmp) {
                fputs("Failed to reallocate buffer\n", stderr);
                return free(buf), EXIT_FAILURE;
            }
            buf = tmp;
            bufsize *= 2;
        }
    }
    return free(buf), EXIT_SUCCESS;
}
