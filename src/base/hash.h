/**
 * 提供哈希表类型及其操作函数
 */
#pragma once

#include <stdbool.h>

typedef bool (*cmp_fun_t)(const void *, const void *);
typedef unsigned int (*hash_fun_t)(const void *);

typedef struct _slot_t {
    char *key;
    void *value;
    struct _slot_t *next;
} slot_t;

typedef struct {
    int capacity;
    cmp_fun_t cmp_fun;
    hash_fun_t hash_fun;
    slot_t **pool;
} hash_table_t;

extern bool hash_pointer_equal(const void *, const void *);
extern bool hash_string_equal(const void *, const void *);
extern unsigned int hash_pointer_hash(const void *);
extern unsigned int hash_string_hash(const void *);
extern hash_table_t *hash_table_new(cmp_fun_t, hash_fun_t);
extern void *hash_table_get(hash_table_t *, const void *);
extern void hash_table_set(hash_table_t *, const void *, void *);
