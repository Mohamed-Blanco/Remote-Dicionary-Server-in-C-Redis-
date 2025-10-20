
#include "DynamicArrays.h"
#include <stdlib.h>
#include <stdio.h>
#include "../heaps/heap.h"  // for HeapItem definition

void initArray(Array *a , size_t intialSize)
{
    a->array = malloc(sizeof(HeapItem)*intialSize);
    a->size = intialSize ;
    a->used = 0 ;
}

void insertArray(Array *a , HeapItem item )
{
    if (!a)
    {
        perror("Unitialized Array Error ");
        return;
    }
    if (a->size == a->used)
    {
        a->size *= 2;
        HeapItem *tmp = realloc(a->array, a->size * sizeof(HeapItem));
        if (!tmp) {
            perror("Realloc failed");
            exit(EXIT_FAILURE);
        }
        a->array = tmp;
    }

    a->array[a->used++] = item ;
}


void freeArray(Array *a)
{
    free(a->array);
    a->array = NULL ; // removing the danger of this
    a->size = 0 ;
    a->used = 0 ;
}


void deleteFromArray(Array *a ,int position)
{

    if (!a || position < 0 || position >= a->used ) return ;
    a->used--;

    if (a->used < (int)(a->size/4))
    {
        printf("DOUBLING THE SIZE OF THE ARRAY \n") ;
        a->size /= 2 ;
        a->array= realloc(a->array , a->size*sizeof(HeapItem));
    }

    a->array[position].val = 0 ;
    a->array[position].ref = NULL ;
}