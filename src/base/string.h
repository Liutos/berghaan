#pragma once

#include <stddef.h>

typedef struct {
    char *data;
    size_t capacity;
    size_t length;
} string_t;

extern string_t *string_new(void);
extern void string_append(string_t *, const char *);
extern void string_clear(string_t *);
extern void string_push_back(string_t *, char);
extern void string_reverse(string_t *, size_t);
