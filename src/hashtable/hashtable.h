#pragma once
#include "../conn.h"
#include "../heaps/heap.h"

/* What do i need to do ?
 * I need first to define an mechanism to parse and handle the SET and GET commands .
 * ALSO i need to implement an hashmap to store these values .
 *
 * Step 1 :
 *  - can a set function have multiple Key values at once or just one ?
 *  -> SET
 */



//Data Node
typedef struct HNode
{
    struct HNode *next ;
    uint64_t hcode ;
} HNode ;

//Bucket array holding all the nodes
typedef struct HTable
{
    struct HNode **tab  ; //array of buckets
    int mask ;
    int size ;
} HTable ;

//Mapper between the Old buckets array and the new One .
typedef struct HMap
{
    struct HTable newer ;
    struct HTable older ;
    int migrate_position ;
} HMap ;

typedef struct Entry
{
    HNode node ;
    char *key ;
    char *value ;

    int heap_idx ;
} Entry ;

typedef struct Global_database
{
    HMap db ;
    struct Conn clients_cons[MAX_CLIENTS];
    DList idle_list ;
    HeapArray *heap ;

}  Global_database;


//initializer functions zeroing the structure , from garbage memory
/*void initialize_HNode(struct HNode *hnod);
void initialize_Hashtable(struct HTable *table);*/
void h_init(HTable *htab , int n );
void h_insert( HTable *htab ,  HNode *node );
HNode **h_lookup( HTable *htab,  HNode *key , bool (*eq)( HNode *, HNode *));
HNode *h_detach( HTable *htab ,  HNode **from );
void hm_trigger_rehashing(HMap *hmap);
void hm_help_rehashing(HMap *hmap);
HNode *hm_lookup(HMap *hmap ,HNode *key , bool (*eq)(HNode *, HNode *));
void hm_insert(HMap *hmap , HNode *node );
HNode *hm_delete(HMap *hmap , HNode *key , bool (*eq)(HNode *, HNode *));
int hm_size(HMap *hmap) ;
void hm_clear(HMap *hmap);
bool entry_eq(HNode *lhs , HNode *rhs );
void entry_del_ttl(Entry *ent);
void entry_set_ttl(Entry *ent , int64_t ttl_ms);
bool hnode_same(HNode *node, HNode *key);










