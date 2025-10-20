
#pragma once


#include <stddef.h>
#include <stdint.h>
#include "../dynamicArrays/DynamicArrays.h"

typedef struct HeapItem
{
    uint64_t val ;
    int *ref ;
} HeapItem;


void heap_insert(Array *a, int pos, HeapItem t);
void heap_delete(Array*a , int pos  );
void heap_update(Array *a , int pos , size_t len );
static size_t heap_left_child(size_t i );
static size_t heap_right_child(size_t i ) ;
static size_t heap_parent(size_t i );
static void bubble_up(Array *items , int position );
static void bubble_down(Array *items , int pos , size_t len );
