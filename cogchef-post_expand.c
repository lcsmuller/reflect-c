#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char COMMENT_OPEN[] = "/*#!";
static const char COMMENT_CLOSE[] = "*/";

enum cogchef_comment_state { COGCHEF_COMMENT_OFF = 0, COGCHEF_COMMENT_ON };

void
expandComments(char *buf, enum cogchef_comment_state state)
{
    while (*buf != '\0') {
        switch (state) {
        case COGCHEF_COMMENT_OFF:
            if (0 != strncmp(buf, COMMENT_OPEN, sizeof(COMMENT_OPEN) - 1))
                break;
            /* make sure directive is on a new line */
            buf[sizeof(COMMENT_OPEN) - 1] = '\n';
            buf += sizeof(COMMENT_OPEN) - 1;
            state = COGCHEF_COMMENT_ON;

            continue;
        case COGCHEF_COMMENT_ON:
            if (0 != strncmp(buf, COMMENT_CLOSE, sizeof(COMMENT_CLOSE) - 1))
                break;

            buf += sizeof(COMMENT_CLOSE) - 1;
            state = COGCHEF_COMMENT_OFF;

            continue;
        }

        fputc(*buf, stdout);
        ++buf;
    }
}

int
main()
{
    enum cogchef_comment_state state = COGCHEF_COMMENT_OFF;
    size_t bufsize = 2048;
    size_t count;
    char *buf = malloc(bufsize);

    if (!buf) return EXIT_FAILURE;

    while ((count = fread(buf, sizeof(char), bufsize - 1, stdin))) {
        if (count >= bufsize - 1) {
            char *tmp = realloc(buf, bufsize * 2);
            if (!tmp) {
                free(buf);
                return EXIT_FAILURE;
            }
            buf = tmp;
            bufsize *= 2;
        }
        buf[count] = '\0';
        expandComments(buf, state);
    }

    free(buf);
    return EXIT_SUCCESS;
}
