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
extern assoc_list_t *assoc_list_push_back(assoc_list_t *, const char *, void *);
extern assoc_list_t *assoc_list_set(assoc_list_t *, int, void *);
extern void *assoc_list_at(assoc_list_t *, int);
extern void *assoc_list_search(assoc_list_t *, const char *);
extern int assoc_list_position(assoc_list_t *, const char *);
