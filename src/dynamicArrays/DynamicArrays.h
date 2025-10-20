#ifndef DYNAMICARRAYS_H
#define DYNAMICARRAYS_H

#include <stddef.h>

typedef struct HeapItem HeapItem; // forward declaration

typedef struct {
    HeapItem *array;
    size_t used;
    size_t size;
} Array;

void initArray(Array *a, size_t initialSize);
void insertArray(Array *a, HeapItem item);
void freeArray(Array *a);
void deleteFromArray(Array *a , int position ) ;


#endif

