#include "list.h"

#include <stdlib.h>
#include <string.h>

assoc_list_t *
assoc_list_new(void)
{
    assoc_list_t *lst = calloc(1, sizeof(assoc_list_t));
    lst->first = NULL;
    return lst;
}

assoc_list_t *
assoc_list_push(assoc_list_t *lst, const char *key, void *value)
{
    assoc_node_t *node = calloc(1, sizeof(assoc_node_t));
    node->key = strdup(key);
    node->value = value;
    node->next = lst->first;
    lst->first = node;
    return lst;
}

assoc_list_t *
assoc_list_push_back(assoc_list_t *lst, const char *key, void *val)
{
    assoc_node_t *node = lst->first;
    if (node == NULL) {
        return assoc_list_push(lst, key, val);
    }
    while (node->next != NULL)
        node = node->next;
    assoc_node_t *n = calloc(1, sizeof(assoc_node_t));
    n->key = strdup(key);
    n->value = val;
    n->next = NULL;
    node->next = n;
    return lst;
}

void *
assoc_list_at(assoc_list_t *lst, int index)
{
    assoc_node_t *node = lst->first;
    while (index > 0 && node != NULL) {
        index--;
        node = node->next;
    }
    return node->value;
}

void *
assoc_list_search(assoc_list_t *lst, const char *key)
{
    assoc_node_t *node = lst->first;
    while (node != NULL) {
        if (strcmp(node->key, key) == 0)
            return node->value;
        node = node->next;
    }
    return NULL;
}

int
assoc_list_position(assoc_list_t *lst, const char *key)
{
    int index = 0;
    assoc_node_t *node = lst->first;
    while (node != NULL) {
        if (strcmp(node->key, key) == 0)
            return index;
        node = node->next;
        index++;
    }
    return -1;
}
