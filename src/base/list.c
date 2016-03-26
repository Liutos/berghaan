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
