// Copyright (C) 2023 Ethan Uppal. All rights reserved.
// config: main.c: Example program to illustate the use of the config library.
//
// Warning: This test program uses GNU C extensions as follows:
//     L16: The two ## before the __VA_ARGS__

#include <stdio.h>         // fprintf, FILE routines
#include <stdlib.h>        // malloc, free
#include <string.h>        // strerror
#include <errno.h>         // errno
#include <config/config.h> /* Change this path if necessary. */

// Error macros (GNU-extended)
#define ERROR(fmt, ...)                                                        \
    fprintf(stderr, "error: %s::%s:%d: " fmt, __FILE__, __func__, __LINE__,    \
        ##__VA_ARGS__);
#define PERROR(msg) ERROR("%s: %s\n", msg, strerror(errno))

void print_help(const char* self) {
    printf("usage: %s [FILE|-h]\n", self);
}

void parse_callback(const char* var, const char* data, void* user_data) {
    printf("\"%s\" = \"%s\"\n", var, data);
}

int main(int argc, char const* argv[]) {
    // Parse arguments
    const char* config_file_path;
    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0) {
            print_help(argv[0]);
            return 0;
        } else {
            config_file_path = argv[1];
        }
    } else {
        fprintf(stderr, "error: unknown options\n");
        print_help(argv[0]);
        return 1;
    }

    FILE* f = fopen(config_file_path, "r");
    if (!f) {
        PERROR("fopen");
        return 1;
    }

    if (conf_parse_file(f, parse_callback, NULL) != 0) {
        PERROR("conf_parse_file");
        return 1;
    }

    if (fclose(f) == EOF) {
        PERROR("fclose");
        return 1;
    }

    return 0;
}
