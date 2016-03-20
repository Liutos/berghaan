#include "string.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

string_t *string_new(void)
{
    string_t *s = calloc(1, sizeof(string_t));
    s->capacity = 0;
    s->length = 0;
    s->data = NULL;
    return s;
}

void string_clear(string_t *s)
{
    s->length = 0;
    if (s->data != NULL)
        s->data[0] = '\0';
}

void string_push_back(string_t *s, char c)
{
    assert(s != NULL);
    string_reverse(s, s->length + 1);
    size_t index = s->length;
    s->data[index] = c;
    s->data[index + 1] = '\0';
    s->length++;
}

void string_reverse(string_t *s, size_t n)
{
    assert(s != NULL);
    size_t capacity = s->capacity;
    if (capacity >= n + 1)
        return;
    capacity = n + 1;
    s->data = realloc(s->data, capacity * sizeof(char));
}
