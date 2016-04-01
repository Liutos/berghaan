/**
 * 提供哈希表类型及其操作函数
 */
#pragma once

typedef struct _slot_t {
    char *key;
    void *value;
    struct _slot_t *next;
} slot_t;

typedef struct {
    int capacity;
    slot_t **pool;
} hash_table_t;

extern hash_table_t *hash_table_new(void);
extern void *hash_table_get(hash_table_t *, const char *);
extern void hash_table_set(hash_table_t *, const char *, void *);
