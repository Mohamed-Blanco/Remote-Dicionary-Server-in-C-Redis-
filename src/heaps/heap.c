
#include "heap.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include "../globals/globals.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 10


void heap_delete(Array *a , int position  )
{

    if (!a || position < 0 || position >= a->used ) return ;

    //swapping the last item with the first item
    a->array[position] = a->array[a->used-1] ;
    a->used--;

    heap_update(a,position , a->used);

    //shrink after delete
    if (a->used <= (int)(a->size/4) && a->size> INITIAL_CAPACITY )
    {
        a->size /= 2 ;
        HeapItem* tmp = realloc(a->array, a->size * sizeof(HeapItem));
        if (tmp) {
            a->array = tmp;
        } else {
            perror("Realloc failed while shrinking heap");
        }
    }

}

void heap_update(Array *a , int pos , size_t len )
{
    if (!a || len == 0 || pos < 0 || pos >= len) return;

    if (pos > 0 &&  a->array[pos].val < a->array[heap_parent(pos)].val )
    {
        bubble_up(a , pos );
    }else
    {
        bubble_down(a,pos, len ) ;
    }
}

//when adding :
//the new element might be the new min value , so we need to bubble up
static void bubble_up(Array *a , int position )
{
    HeapItem node = a->array[position] ;
    while (position > 0 && a->array[heap_parent(position)].val > node.val) // we will keep bubelling down the element until we find its place
    {
        a->array[position] = a->array[heap_parent(position)];
        if (a->array[position].ref)
            *a->array[position].ref = position;
        position = heap_parent(position);
    }

    a->array[position] = node ;
    if (a->array[position].ref)
        *a->array[position].ref = position;

}

//when remove :
//the new old element might be the min value , so we need to search for the next mean value
static void bubble_down(Array *a , int pos , size_t len )
{
    HeapItem t = a->array[pos] ;
    if (pos < 0) return ;
    while (true)
    {
        size_t l = heap_left_child(pos) ;
        size_t r = heap_right_child(pos) ;
        int min_pos = pos ;
        uint64_t min_val = t.val ;

        if (l < len && a->array[l].val < min_val )
        {
            min_pos = l ;
            min_val = a->array[l].val ;
        }

        if (r < len && a->array[r].val < min_val )
        {
            min_pos = r ;
        }

        if (min_pos == pos )
        {
            if (a->array[pos].ref)
                *a->array[pos].ref = pos;
            break ;
        }

        HeapItem temp = a->array[pos] ;
        a->array[pos] = a->array[min_pos] ;
        a->array[min_pos] = temp ;

        if (a->array[pos].ref)
            *a->array[pos].ref = pos;
        *a->array[min_pos].ref = min_pos ;

        pos = min_pos ;
    }
}

static size_t heap_left_child(size_t i )
{
    return i*2+1;
    /*
     * means that if the index is 1 , its left child is at : 1 * 2 + 1 = 3
     */
}

static size_t heap_right_child(size_t i)
{
    return i*2+2; /*
     * means that if the index is 1 , its right child is at : 1 * 2 + 2 = 3
     */
}

static size_t heap_parent(size_t i )
{
    return (i-1)/2;
    /*
     * if index is 3 the parent index is (3+1) /2 -1 = 1
     */
}

//dont forget to update the usage of them
//this funciton checks if the entry already has an heapItem or we should create a new one for this Entry
void heap_insert(Array *a, int pos, HeapItem t) {
    if (pos >= 0 && pos < a->used )
    {
        a->array[pos]= t ;
    }else
    {
        insertArray(a,t);
        pos = a->used -1 ;
    }

    heap_update(a, pos, a->used); // check if we need to bubble up or down
    }

/*
 * this is what array decoding means , no need for pointers no need for null pointer nothing
 * but it has multiple cons we will see later .
*/