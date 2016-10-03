#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

bool
vector_is_empty(vector_t *v)
{
    return v->length == 0;
}

vector_t *
vector_new(void)
{
    vector_t *v = calloc(1, sizeof(vector_t));
    v->capacity = 0;
    v->length = 0;
    v->data = NULL;
    return v;
}

vector_t *
vector_concat(vector_t *v1, vector_t *v2)
{
    for (size_t i = 0; i < v2->length; i++)
        vector_push_back(v1, vector_at(v2, i));
    return v1;
}

void *
vector_at(vector_t *v, size_t index)
{
    assert(v != NULL);
    assert(index < v->length);
    return vector_unsafe_at(v, index);
}

void *
vector_back(vector_t *v)
{
    assert(v != NULL);
    return v->data[v->length - 1];
}

void *
vector_set(vector_t *v, size_t index, void *value)
{
    assert(index < v->capacity);
    v->data[index] = value;
    return value;
}

void *
vector_unsafe_at(vector_t *v, size_t index)
{
    assert(v != NULL);
    return v->data[index];
}

void
vector_pop_back(vector_t *v)
{
    assert(v != NULL);
    assert(v->length > 0);
    v->length--;
}

void
vector_push_back(vector_t *v, void *data)
{
    assert(v != NULL);
    vector_reserve(v, v->length + 1);
    v->data[v->length] = data;
    v->length++;
}

void
vector_reserve(vector_t *v, size_t n)
{
    assert(v != NULL);
    if (v->capacity >= n)
        return;
    v->capacity = n;
    v->data = realloc(v->data, v->capacity * sizeof(void *));
}
