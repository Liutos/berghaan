#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAPACITY 100

static unsigned int
bkdr(const char *s)
{
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*s) {
        hash = hash * seed + *s;
        s++;
    }
    return hash & 0x7FFFFFFF;
}

hash_table_t *
hash_table_new(void)
{
    hash_table_t *tbl = calloc(1, sizeof(hash_table_t));
    tbl->capacity = DEFAULT_CAPACITY;
    tbl->pool = calloc(tbl->capacity, sizeof(slot_t *));
    return tbl;
}

void *
hash_table_get(hash_table_t *tbl, const char *key)
{
    unsigned int index = bkdr(key) % tbl->capacity;
    slot_t *slot = tbl->pool[index];
    while (slot != NULL) {
        if (strcmp(slot->key, key) == 0)
            return slot->value;
        slot = slot->next;
    }
    return NULL;
}

void
hash_table_set(hash_table_t *tbl, const char *key, void *value)
{
    unsigned int index = bkdr(key) % tbl->capacity;
    slot_t *slot = tbl->pool[index];
    while (slot != NULL) {
        if (strcmp(slot->key, key) == 0) {
            slot->value = value;
            return;
        }
        slot = slot->next;
    }
    slot = calloc(1, sizeof(slot_t));
    slot->key = strdup(key);
    slot->value = value;
    slot->next = tbl->pool[index];
    tbl->pool[index] = slot;
}
