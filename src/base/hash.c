#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAPACITY 100

static bool
is_key_equal(hash_table_t *tbl, const void *key1, const void *key2)
{
    cmp_fun_t cmp_fun = tbl->cmp_fun;
    return cmp_fun(key1, key2);
}

static unsigned int
compute_index(hash_table_t *tbl, const void *key)
{
    hash_fun_t hash_fun = tbl->hash_fun;
    return hash_fun(key) % tbl->capacity;
}

static slot_t *
locate_slot(hash_table_t *tbl, const void *key)
{
    unsigned int index = compute_index(tbl, key);
    return tbl->pool[index];
}

bool
hash_pointer_equal(const void *_s1, const void *_s2)
{
    return _s1 == _s2;
}

bool
hash_string_equal(const void *_s1, const void *_s2)
{
    const char *s1 = (const char *)_s1;
    const char *s2 = (const char *)_s2;
    return strcmp(s1, s2) == 0;
}

unsigned int
hash_pointer_hash(const void *_s)
{
    return (unsigned int)_s;
}

unsigned int
hash_string_hash(const void *_s)
{
    const char *s = (const char *)_s;
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*s) {
        hash = hash * seed + *s;
        s++;
    }
    return hash & 0x7FFFFFFF;
}

hash_table_t *
hash_table_new(cmp_fun_t cmp_fun, hash_fun_t hash_fun)
{
    hash_table_t *tbl = calloc(1, sizeof(hash_table_t));
    tbl->capacity = DEFAULT_CAPACITY;
    tbl->cmp_fun = cmp_fun;
    tbl->hash_fun = hash_fun;
    tbl->pool = calloc(tbl->capacity, sizeof(slot_t *));
    return tbl;
}

void *
hash_table_get(hash_table_t *tbl, const void *key)
{
    slot_t *slot = locate_slot(tbl, key);
    while (slot != NULL) {
        if (is_key_equal(tbl, slot->key, key))
            return slot->value;
        slot = slot->next;
    }
    return NULL;
}

void
hash_table_set(hash_table_t *tbl, const void *key, void *value)
{
    slot_t *slot = locate_slot(tbl, key);
    while (slot != NULL) {
        if (is_key_equal(tbl, slot->key, key)) {
            slot->value = value;
            return;
        }
        slot = slot->next;
    }
    unsigned int index = compute_index(tbl, key);
    slot = calloc(1, sizeof(slot_t));
    slot->key = key;
    slot->value = value;
    slot->next = tbl->pool[index];
    tbl->pool[index] = slot;
}
