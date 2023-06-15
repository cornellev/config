# config

Config is simple library for parsing configuration files.

```
# This is a configuration file

KEY = VALUE                 # This is an example setting
My\=Setting = Potato = Food # You can escape the delimiter

# You can even have custom delimiters
OUTPUT:c++
```

## Install

### Unix-like systems (macOS, Linux, etc.)

```
git clone https://github.com/ethanuppal/config
    && cd config
    && make install
```

This command will install the static (`config.a`) and dynamic (`config.so`) libraries in `/usr/local/lib` and the header file `config.h` in `/usr/local/include`.

### Windows

If you have `clang` installed, you can run

```
git clone https://github.com/ethanuppal/config
    && cd config
    && make all
```

This will build the static and dynamic libraries in the current directory.

## Usage

### Section contents

* Example
* Callback
* Parsing files
* Configuring config

### Example

The directory [test/](test) has a working example with:

* [config.txt](test/config.txt): A sample configuation file
* [main.c](test/main.c): A program that uses config to parse it

### Callback

Create a callback for when config finds a new variable as follows (in order that it be of type `conf_parse_handler_t`). The parameters besides `user_data` are read-only and valid for only as long as config is parsing, so be sure to copy them if you want to use it outside of the callback.

```c
void callback(const char *var, const char *data, void *user_data) {
    myapi_t *api = (myapi_t*)user_data;
    if (strcmp(var, "PATH") == 0) {
        api->path = data;
    }
}
```

Then call `conf_parse_file` or `conf_parse_text` with the appropriate arguments.

### Parsing files

#### `conf_parse_file`

```c
int conf_parse_file(FILE* f, conf_parse_handler_t handler, void* user_data);
```
Parses the given file as a configuration file.

Param | Description
----- | -----------
`f` | The configution file handle.
`handler` | The callback for when a variable is found.
`user_data` | Optional data passed to the callback.

#### `conf_parse_text`

```c
int conf_parse_text(const char* s, conf_parse_handler_t handler, void* user_data);
```
Parses the given text as a configuation file.

Param | Description
----- | -----------
`s` | A string in config syntax.
`handler` | The callback for when a variable is found.
`user_data` | Optional data passed to the callback.

### Configuring config

#### `conf_set_comments`

```c
void conf_set_comments(const char* comments);
```
Tells config what comments look like.

Param | Description
----- | -----------
`comments` | A list of characters. Any line starting with these characters will be ignores. Empty lines are automatically ignored.

If this function is not invoked, config will behave as if it was run as
```c
conf_set_comments("\n#");
```

Example:
```c
// Ignore lines starting with, spaces, tabs, exclamation points, and hashtags.
conf_set_comments(" \t#!");
```

#### `conf_set_delimiters`

```c
void conf_set_delimiters(const char* delimiters, char escape);
```
Tells config what the variable delimiters look like.

Param | Description
----- | -----------
`delimiters` | A list of variable delimiters.
`escape` An optional escape character. Set this to 0 if you don't want any escaping. If specified, variable names can include a delimiter character provided it is prefixed with the escape character. It will show up in the callback without the escape. A sequence of two escape characters will show up as a single one.

If this function is not invoked, config will behave as if it was run as
```c
conf_set_delimiters("=", '\\');
```

Example 1:
```c
// Variables defined with '=', escapes with '\'
conf_set_delimiters("=", '\\');
```

Example 2:
```c
// Variables defined with ':', no escaping
conf_set_delimiters(":", 0);
```

#### `conf_set_trimming`

```c
void conf_set_trimming(const char* whitespace);
```
Tells config whether to trim leading and trailing whitespace from entries.

Param | Description
----- | -----------
`whitespace` | A list of characters to trim. If this value is `NULL`, trimming is disabled.

If this function is not invoked, config will behave as if it was run as
```c
conf_set_trimming(" \t");
```

Example:
```
// Disable trimming
conf_set_trimming(NULL);
```
