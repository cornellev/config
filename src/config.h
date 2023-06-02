// Copyright (C) 2023 Ethan Uppal. All rights reserved.

#pragma once

#include <stdio.h>

/**
 * A callback for when config finds a new variable. Implement your code in this callback. The parameters besides `user_data` are read-only and valid for only as long as config is parsing, so be sure to copy them if you want to use it outside of the callback.
 *
 * @param var The variable name.
 * @param data The variable's value.
 *
 * @code
 * void callback(const char* var, const char *data, void *user_data) {
 *     myapi_t *api = (myapi_t*)user_data;
 *     if (strcmp(var, "PATH") == 0) {
 *         api->path = data;
 *     }
 * }
 * @endcode
 */
typedef void (*conf_parse_handler_t)(const char* var, const char* data, void* user_data);

/**
 * Tells config what comments look like.
 *
 * If this function is not invoked, config will behave as if it was run as
 * @code
 * conf_set_comments(" #");
 * @endcode
 *
 * @param comments A list of characters. Any line starting with these characters will be ignores. Empty lines are automatically ignored.
 *
 * @code
 * // Ignore lines starting with, spaces, tabs, exclamation points, and hashtags.
 * conf_set_comments(" \t#!");
 * @endcode
 */
void conf_set_comments(const char* comments);

/**
 * Tells config what the variable delimiters look like.
 *
 * If this function is not invoked, config will behave as if it was run as
 * @code
 * conf_set_delimiters("=", '\\');
 * @endcode
 *
 * @param delimiters A list of variable delimiters.
 * @param escape An optional escape character. Set this to 0 if you don't want any escaping. If specified, variable names can include a delimiter character provided it is prefixed with the escape character. It will show up in the callback without the escape. A sequence of two escape characters will show up as a single one.
 *
 * @code
 * // Example 1:
 * // Variables defined with '=', escapes with '\'
 * conf_set_delimiters("=", '\\');
 * @endcode
 *
 * @code
 * // Example 2
 * // Variables defined with ':', no escaping
 * conf_set_delimiters(":", 0);
 * @endcode
 */
void conf_set_delimiters(const char* delimiters, char escape);

/**
 * Parses the given file as a configuration file. See @ref conf_parse_handler_t for more information on usage.
 *
 * @param f The configution file handle.
 * @param handler The callback for when a variable is found.
 * @param user_data Optional data passed to the callback.
 */
int conf_parse_file(FILE* f, conf_parse_handler_t handler, void* user_data);

/**
 * Parses the given text as a configuation file. See @ref conf_parse_handler_t for more information on usage.
 *
 * @param s A string in config syntax.
 * @param handler The callback for when a variable is found.
 * @param user_data Optional data passed to the callback.
 */
int conf_parse_text(const char* s, conf_parse_handler_t handler, void* user_data);
