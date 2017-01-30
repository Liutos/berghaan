#include "store.h"

#include <assert.h>
#include <stdlib.h>

store_t *
store_new(int size, store_t *outer)
{
    store_t *s = calloc(1, sizeof(store_t));
    s->outer = outer;
    s->pool = vector_new();
    vector_reserve(s->pool, size);
    return s;
}

void
store_set(store_t *s, size_t x, size_t y, object_t *val)
{
    while (x > 0) {
        assert(s != NULL);
        s = s->outer;
    }
    vector_set(s->pool, y, val);
    if (y >= s->pool->length)
        s->pool->length = y + 1;
}

object_t *
store_get(store_t *s, int x, int y)
{
    while (x > 0) {
        assert(s != NULL);
        s = s->outer;
        x--;
    }
    return (object_t *)vector_at(s->pool, y);
}
