#include "globals.h"
#include <stdlib.h>

/*
 * Initializing The Global Database :
 *         - What do we need to intialize it ? :
 *         - Before we need to know what does it contain ?? :
 *              1. Hmap , Holds both the newer and the older Hashtables during the rehashing process .
 *              2. Connected CLients clients_cons[MAX_clients]
 *              3. Dlist the doubly Linked List (Min Heap)  that holds all the Connected Clients Idle Min-Heap
 *              4. HeapArray Min-Heap holding data nodes sorted by ttl .
 *
 *         - HMap : we need to initialize it with an NULL , since its init is Handled with h_init .
 *         - client_cons : yes here we need to start an array of N cons zeroed with memset , also i need to replce the array in the main with this !!!
 *         - Yeah also this one init is handled by dlist_init , so just NULL .
 *         - HeapArray : Yeah also this need to be NULL
 *         --> heap index to -1 but this for the entry
 *
 *         vro ? so this all need to be NULL : YEP
 */


Global_database glob_db = {
        .idle_list = NULL,
        .db = NULL ,
        .heap = NULL ,
} ;



