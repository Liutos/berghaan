#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

vector_t *
vector_new(void)
{
    vector_t *v = calloc(1, sizeof(vector_t));
    v->capacity = 0;
    v->length = 0;
    v->data = NULL;
    return v;
}

void *
vector_at(vector_t *v, size_t index)
{
    assert(v != NULL);
    assert(index < v->length);
    return v->data[index];
}

void *
vector_back(vector_t *v)
{
    assert(v != NULL);
    return v->data[v->length - 1];
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
    vector_reverse(v, v->length + 1);
    v->data[v->length] = data;
    v->length++;
}

void
vector_reverse(vector_t *v, size_t n)
{
    assert(v != NULL);
    if (v->capacity >= n)
        return;
    v->capacity = n;
    v->data = realloc(v->data, v->capacity * sizeof(void *));
}
