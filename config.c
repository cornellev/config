// Copyright (C) 2023 Ethan Uppal. All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "config.h"

#define ESC '\033'

static struct {
    const char* comments;
    const char* delimiters;
    const char* whitespace;
    char escape;
} conf = {
    .comments = "\n#", .delimiters = "=", .whitespace = " \t", .escape = '\\'};

/**
 * @private
 */
void conf_parse_internal(char* s, size_t length, conf_parse_handler_t handler,
    void* user_data);

void conf_set_comments(const char* comments) {
    conf.comments = comments;
}

void conf_set_delimiters(const char* delimiters, char escape) {
    conf.delimiters = delimiters;
    conf.escape = escape;
}

void conf_set_trimming(const char* whitespace) {
    conf.whitespace = whitespace;
}

int conf_parse_file(FILE* f, conf_parse_handler_t handler, void* user_data) {
    if (fseek(f, 0, SEEK_END) != 0) {
        return 1;
    }
    long length = ftell(f);
    if (length == -1) {
        return 1;
    }
    rewind(f);

    char* buffer = malloc(length + 1);
    if (!buffer) {
        return 1;
    }
    buffer[length] = 0;

    if (fread(buffer, sizeof(char), length, f) != (size_t)length) {
        return 1;
    }

    conf_parse_internal(buffer, (size_t)length, handler, user_data);

    return 0;
}

int conf_parse_text(const char* s, conf_parse_handler_t handler,
    void* user_data) {
    size_t length = strlen(s);
    char* s_copy = malloc(length + 1);
    if (!s_copy) {
        return 1;
    }
    memcpy(s_copy, s, length + 1);

    conf_parse_internal(s_copy, length, handler, user_data);

    free(s_copy);

    return 0;
}

/**
 * @private
 */
bool conf_should_skip_line(char c) {
    return c == '\n' || (strchr(conf.comments, c) != NULL);
}

/**
 * @private
 */
bool conf_should_end_var(char c) {
    return strchr(conf.delimiters, c) != NULL;
}

/**
 * @private
 * @pre `conf.whitespace` is not `NULL`.
 */
char* conf_trim(char* str) {
    // Strip leading whitespace
    while (strchr(conf.whitespace, *str) != NULL) {
        str++;
    }

    // Strip trailing whitespace
    size_t length = strlen(str);
    if (length > 0) {
        while (strchr(conf.whitespace, str[length - 1]) != NULL) {
            length--;
        }
        str[length] = 0;
    }

    return str;
}

/**
 * @private
 */
void conf_clean(char* str) {
    ssize_t i;
    ssize_t offset = 0;
    for (i = 0; str[i]; i++) {
        str[i - offset] = str[i];
        if (str[i] == ESC) {
            offset++;
        }
    }
    str[i - offset] = 0;
}

/**
 * @private
 */
void conf_process_var(char* s, size_t var_start, size_t var_end,
    size_t data_start, size_t data_end, conf_parse_handler_t handler,
    void* user_data) {
    char* var = s + var_start;
    var[var_end - var_start] = 0;

    char* data = s + data_start;
    data[data_end - data_start] = 0;

    conf_clean(var);
    conf_clean(data);

    if (conf.whitespace) {
        var = conf_trim(var);
        data = conf_trim(data);
    }

    handler(var, data, user_data);
}

void conf_parse_internal(char* s, size_t length, conf_parse_handler_t handler,
    void* user_data) {
    enum {
        CONF_SKIP_LINE,
        CONF_EXPECT_VAR,
        CONF_VAR_ESCAPED,
        CONF_EXPECT_DELIM,
        CONF_EXPECT_DATA
    } state = CONF_EXPECT_VAR;

    size_t var_start = 0, var_end = 0;
    size_t data_start = 0, data_end = 0;

    for (size_t i = 0; i < length; i++) {
        const char c = s[i];
        if (conf_should_skip_line(c)) {
            if (state == CONF_EXPECT_DATA) {
                conf_process_var(s, var_start, var_end, data_start, data_end,
                    handler, user_data);
            }
            state = CONF_SKIP_LINE;
        }
        switch (state) {
            case CONF_SKIP_LINE: {
                if (c == '\n') {
                    state = CONF_EXPECT_VAR;
                    var_start = i + 1;
                }
                break;
            }
            case CONF_EXPECT_VAR: {
                if (c == conf.escape) {
                    state = CONF_VAR_ESCAPED;
                    s[i] = ESC;
                } else if (conf_should_end_var(c)) {
                    var_end = i;
                    state = CONF_EXPECT_DELIM;
                }
                break;
            }
            case CONF_VAR_ESCAPED: {
                // we just found an escape character so ignore this char
                state = CONF_EXPECT_VAR;
                break;
            }
            case CONF_EXPECT_DELIM: {
                // we already know we're at a delim because that's what
                // conf_should_end_var checks, so we just move on to
                // CONF_EXPECT_DATA
                state = CONF_EXPECT_DATA;
                data_start = i;
                break;
            }
            case CONF_EXPECT_DATA: {
                data_end = i + 1;
                break;
            }
        }
    }
}
