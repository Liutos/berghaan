/**
 * 提供线性表类型及其操作函数
 */
#pragma once

typedef struct _assoc_node_t {
    char *key;
    void *value;
    struct _assoc_node_t *next;
} assoc_node_t;

typedef struct {
    assoc_node_t *first;
} assoc_list_t;

extern assoc_list_t *assoc_list_new(void);
extern assoc_list_t *assoc_list_push(assoc_list_t *, const char *, void *);
extern void *assoc_list_search(assoc_list_t *, const char *);
