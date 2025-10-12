#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct HeapItem
{
    uint64_t val ;
    int *ref ;
} HeapItem;


//dynamique Heap array
typedef struct HeapArray
{
    HeapItem *items;
    size_t size ;
} HeapArray;


void heap_upsert(HeapArray **heap, int pos, HeapItem t);
void heap_insert(HeapItem *a , HeapItem item );
void heap_delete(HeapArray *a , int pos  );
void heap_update(HeapArray *a , int pos , size_t len );
static size_t heap_left_child(size_t i );
static size_t heap_right_child(size_t i ) ;
static size_t heap_parent(size_t i );
static void bubble_up(HeapArray *items , int position );
static void bubble_down(HeapArray *items , int pos , size_t len );
