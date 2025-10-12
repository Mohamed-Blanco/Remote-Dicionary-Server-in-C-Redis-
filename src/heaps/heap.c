
#include "heap.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include "../globals/globals.h"
#include <stdlib.h>


void heap_delete(HeapArray *a , int pos  )
{
    if (pos >= a->size) return;  // safety check
    //swapping the last item with the first item
    a->items[pos] = a->items[a->size-1] ;

    //deleting the item
    free(a->items);
    a->size--;

    if (pos < a->size)
    {
        heap_update(a,pos , a->size);
    }
}

void heap_update(HeapArray *a , int pos , size_t len )
{
    if (pos > 0 && a->items[heap_parent(pos)].val > a->items[pos].val  )
    {
        bubble_up(a , pos );
    }else
    {
        bubble_down(a,pos, len ) ;
    }
}

//when adding :
//the new element might be the new min value , so we need to bubble down
static void bubble_up(HeapArray *a , int position )
{
    HeapItem node = a->items[position] ;
    while (position > 0 && a->items[position].val > node.val) // we will keep bubelling down the element until we find its place
    {
        a->items[position] = a->items[heap_parent(position)];
        *a->items[position].ref = position ;
        position = heap_parent(position);
    }

    a->items[position] = node ;
    *a->items[position].ref = position ;

}

//when remove :
//the new old element might be the min value , so we need to search for the next mean value
static void bubble_down(HeapArray *a , int pos , size_t len )
{
    HeapItem t = a->items[pos] ;

    while (true)
    {
        size_t l = heap_left_child(pos) ;
        size_t r = heap_right_child(pos) ;
        size_t min_pos = pos ;
        uint64_t min_val = t.val ;

        if (l < len && a->items[l].val < min_val )
        {
            min_pos = l ;
            min_val = a->items[l].val ;
        }

        if (r < len && a->items[r].val < min_val )
        {
            min_pos = r ;
        }

        if (min_pos == pos )
        {
            break ;
        }

        //swap with the kid
        a->items[pos] = a->items[min_pos] ;
        pos = min_pos ;
        *a->items[pos].ref = pos ;

    }
    a->items[pos] = t ;
    *a->items[pos].ref = pos ;
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
    return (i+1)/2-1;
    /*
     * if index is 3 the parent index is (3+1) /2 -1 = 1
     */
}


//this funciton checks if the entry already has an heapItem or we should create a new one for this Entry
void heap_upsert(HeapArray **heap, int pos, HeapItem t)
{
    if (*heap == NULL)
    {
        *heap = malloc(sizeof(HeapArray));
        (*heap)->size = 0;
        (*heap)->items = NULL;
    }

    HeapArray *h = *heap;

    if (pos == -1 && h->size == 0 )
    {
        h->size = 1;
        h->items = realloc(h->items, sizeof(HeapItem) * h->size);
        *t.ref = 0;
        h->items[0] = t;
        return;
    }
    if (pos < h->size )
    {
        h->items[pos] = t ;
    }else
    {
        h->size += 1;
        h->items = realloc(h->items, sizeof(HeapItem) * h->size);
        h->items[h->size - 1] = t;
    }
    heap_update(h, pos, h->size); // what is the role of this now ?
}








/*
 * this is what array decoding means , no need for pointers no need for null pointer nothing
 * but it has multiple cons we will see later .
*/