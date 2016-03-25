#include "object.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
object_int_print(object_t *n)
{
    assert(n->type == OBJECT_INT);
    printf("%d", OBJECT_INT_VALUE(n));
}

object_t *
object_int_new(int value)
{
    object_t *n = calloc(1, sizeof(object_t));
    n->type = OBJECT_INT;
    OBJECT_INT_VALUE(n) = value;
    return n;
}

void
object_print(object_t *x)
{
    switch (x->type) {
        case OBJECT_INT:
            object_int_print(x);
            break;
    }
}
