#include "object.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
object_bool_print(object_t *b)
{
    assert(b->type == OBJECT_BOOL);
    printf("%s", OBJECT_BOOL_VALUE(b) ? "true" : "false");
}

static void
object_int_print(object_t *n)
{
    assert(n->type == OBJECT_INT);
    printf("%d", OBJECT_INT_VALUE(n));
}

static void
object_nil_print(object_t *n)
{
    assert(n->type == OBJECT_NIL);
    printf("nil");
}

object_t *
object_bool_new(bool value)
{
    object_t *b = calloc(1, sizeof(object_t));
    b->type = OBJECT_BOOL;
    OBJECT_BOOL_VALUE(b) = value;
    return b;
}

object_t *
object_int_new(int value)
{
    object_t *n = calloc(1, sizeof(object_t));
    n->type = OBJECT_INT;
    OBJECT_INT_VALUE(n) = value;
    return n;
}

object_t *
object_nil_new(void)
{
    object_t *n = calloc(1, sizeof(object_t));
    n->type = OBJECT_NIL;
    return n;
}

void
object_print(object_t *x)
{
    switch (x->type) {
        case OBJECT_BOOL:
            object_bool_print(x);
            break;
        case OBJECT_INT:
            object_int_print(x);
            break;
        case OBJECT_NIL:
            object_nil_print(x);
            break;
    }
}
