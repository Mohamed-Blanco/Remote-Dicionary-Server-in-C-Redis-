#include "hashtable.h"
#define THRESHOLD 1
#define REHASHING_BUDGET 10 //this is the number of the elements that we will move during every operation , when rehashing
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "../heaps/heap.h"
#include "../globals/globals.h"


bool hnode_same(HNode *node, HNode *key) {
    return node == key;
}


void h_init(HTable *htab , int n )
{
    assert(n>0);
    htab->tab = ( HNode **)calloc(n,sizeof( HNode*));
    htab->mask = n-1;
    htab->size = 0 ;
}

void h_insert( HTable *htab ,  HNode *node )
{
    int index = node->hcode & htab->mask; // this is just a BITWISE Operation ensuring that the index fits the in the array
    HNode *next = htab->tab[index];
    node->next = next ;  //pointing to the existing node in the
    htab->tab[index] = node ; //pointing to the existing node in the
    htab->size++;
}

// hashtable look up subroutine.
// Pay attention to the return value. It returns the address of
// the parent pointer that owns the target node,
// which can be used to delete the target node.
 HNode **h_lookup( HTable *htab,  HNode *key , bool (*eq)( HNode *, HNode *))
{
    if (!htab->tab )
    {
        return NULL ;
    }

     size_t index = key->hcode & htab->mask ;
     HNode **from = &htab->tab[index]; //pointer to the target bucket

     HNode *current = *from ;
    while (current != NULL)
    {
        if (current->hcode == key->hcode && eq(current,key)) return from ;
        from = &current->next ;
        current = current->next ;
    }

    return NULL ;
}


//removing the Node from the Chain
HNode *h_detach( HTable *htab ,  HNode **from )
{
    HNode *node = *from ;
    *from = node->next ; //skiping the actual Node
    htab->size--;
    return node ;
}

//intializing the rehashing process ,basicly the current table will be moved into the older one , and we will initailize a new one with double the size of the older .
void hm_trigger_rehashing(HMap *hmap)
{
    assert(hmap->older.tab == NULL );
    hmap->older = hmap->newer ;
    h_init(&hmap->newer , (hmap->newer.mask +1 )*2); //doubling the size of the buckets array
    hmap->migrate_position = 0 ;
}

//progressive rehashing
void hm_help_rehashing(HMap *hmap)
{
    int nwork = 0 ;
    while (nwork < REHASHING_BUDGET && hmap->older.size > 0 ) //confusion res : we do > 0 beacause if older.size == 0 it means that we already rehashed all the elemnts .
    {
        HNode **from = &hmap->older.tab[hmap->migrate_position];
        if (*from == NULL )
        {
            hmap->migrate_position++;
            continue; //empty_slot
        }

        h_insert(&hmap->newer,h_detach(&hmap->older,from));
        nwork++ ;
    }

    if (hmap->older.size == 0 && hmap->older.tab )
    {
        free(hmap->older.tab);
        hmap->older = (HTable){NULL,0,0};
    }
}


//we are checking both Hashmaps
HNode *hm_lookup(HMap *hmap ,HNode *key , bool (*eq)(HNode *, HNode *))
{
    hm_help_rehashing(hmap); // trigger rehashing on every lookup
    HNode **from = h_lookup(&hmap->newer, key , eq);
    if (!from)
    {
        from = h_lookup(&hmap->newer , key , eq);
    }
    return from ? *from : NULL ;
}

void hm_insert(HMap *hmap , HNode *node )
{
    if (!hmap->newer.tab) // the newer is the acutal hashtable , so we are checking if we have initalized it or not ?
    {
        h_init(&hmap->newer,4); //storing at first 4
    }
    h_insert(&hmap->newer ,node);

    if (!hmap->older.tab)
    {
        size_t shreshold = (hmap->newer.mask + 1) * THRESHOLD; //calculating the threshold
        if (hmap->newer.size >= shreshold) //if it surpases the actual size then we need to start rehashing
        {
            hm_trigger_rehashing(hmap);
        }
    }
    hm_help_rehashing(hmap);
}




//this delete the node from the HMap (older or newer) ,
// Note: it returns an pointer to the Parent Node , if not it returns a pointer to the bucket
HNode *hm_delete(HMap *hmap , HNode *key , bool (*eq)(HNode *, HNode *))
{
    hm_help_rehashing(hmap);
    HNode **from = h_lookup(&hmap->newer, key , eq);
    if (from != NULL)
    {
        return h_detach(&hmap->older , from );
    }
    from = h_lookup(&hmap->older, key , eq);
    if (from != NULL)
    {
        return h_detach(&hmap->older, from );
    }
    return NULL ;
}

int hm_size(HMap *hmap)
{
    return hmap->newer.size + hmap->older.size ;
}


void hm_clear(HMap *hmap)
{
    free(hmap->newer.tab);
    free(hmap->older.tab);
    *hmap = (HMap){NULL,0,0,NULL,0,0,0};
}


bool entry_eq(HNode *lhs , HNode *rhs )
{
    Entry *le = container_of(lhs, struct Entry , node );
    Entry *re = container_of(lhs, struct Entry , node );
    return le->key == re->key ;
}

void entry_del_ttl(Entry *ent)
{
    entry_set_ttl(ent, -1);

    if (ent->key) free(ent->key);
    if (ent->value) free(ent->value);

    free(ent);
}


static uint64_t get_monotonic_msec() {
    struct timespec tv = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_nsec / 1000 / 1000;
}


void entry_set_ttl(Entry *ent , int64_t ttl_ms)
{
    if (ttl_ms < 0 && ent->heap_idx != -1)
    {
        heap_delete(&glob_db.heap, ent->heap_idx);
        ent->heap_idx = -1 ;
    }else if (ttl_ms >= 0 )
    {
        uint64_t expire_at = get_monotonic_msec() + (uint64_t) ttl_ms ;
        HeapItem item = {expire_at , &ent->heap_idx };
        heap_insert(&glob_db.heap, ent->heap_idx, item);
    }
}

