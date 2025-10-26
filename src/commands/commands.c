#include "commands.h"
#define OK "+OK\r\n"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../conn.h"
#include "../hashtable/hashtable.h"
#include "../globals/globals.h"

uint32_t str_hash(const uint8_t *data , size_t len )
{
    uint32_t hash_code = 0x811C9DC5 ;
    for (size_t i = 0 ; i < len ; i++ )
    {
        hash_code = (hash_code + data[i]) * 0x01100193 ;
    }
    return hash_code;
}


const char* do_get(char *search_key , int key_length )
{
    Entry key ;
    key.key = search_key ;
    key.node.hcode = str_hash((uint8_t *)search_key , key_length);

    HNode *node = hm_lookup(&glob_db.db, &key.node , &entry_eq );
    if (!node) //if it was not found
    {
        perror("KEY WAS NOT FOUND !! \n ");
        return "NX";
    }

    const char *val = container_of(node , Entry , node)->value ;
    assert(strlen(val) <= K_MAX_MSG);
    printf("VALUE : %s FOUND WITH KEY : %s \n "  ,val, key.key ) ;
    return val ;

}

const char* do_set(char *search_key , int key_length , char *value  )
{
    Entry entry ;
    entry.key = search_key ;
    entry.node.hcode = str_hash((uint8_t *)search_key , key_length);

    HNode *node = hm_lookup(&glob_db.db , &entry.node , &entry_eq);
    if (node)
    {
        container_of(node,Entry,node)->value = value;
        perror("VALUE IS ALREADY DEFINED STORED !! \n");
    } else
    {
        Entry *ent = malloc(sizeof(Entry));
        ent->key = strdup(search_key);
        ent->value = strdup(value);
        ent->node.hcode = entry.node.hcode ;
        ent->heap_idx = -1 ;
        hm_insert(&glob_db.db , &ent->node);
        printf("KEY : %s , VALUE : %s , STORED \n " , ent->key , ent->value);
    }

    return OK;
}


const bool do_expire(char *key, int key_length, size_t ttl )
{
    Entry entry ;
    entry.key = key ;
    entry.node.hcode = str_hash((uint8_t *)key , key_length);

    HNode *node = hm_lookup(&glob_db.db , &entry.node , &entry_eq);

    if (node) //if its a valid node
    {
        Entry *ent = container_of(node,Entry,node); // getting the Actual Entry From the Hmap in the global db
        entry_set_ttl(ent, ttl); //need to multiply by 1000 to convert from sec to ms

        return true  ;
    }

    return false ;
}