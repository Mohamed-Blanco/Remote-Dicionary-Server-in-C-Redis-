#pragma once

#include <stdbool.h>


//cercular Doubly Linked List
typedef struct DList  {
    struct DList *prev;
    struct DList *next;
} DList;


static inline void dlist_init(DList *node) {
    node->prev = node->next = node;
}

static inline bool dlist_empty(DList *node) {
    return node->next == node;
}

static inline void dlist_insert_before(DList *target, DList *newnode) {
    DList *prev = target->prev;
    prev->next = newnode;
    newnode->prev = prev;
    newnode->next = target;
    target->prev = newnode;
}

static inline void dlist_detach(DList *node) //just skip the node
{
    DList *prev = node->prev ;
    DList *next = node->next ;

    prev->next = next ;
    next->prev = prev ;
}
