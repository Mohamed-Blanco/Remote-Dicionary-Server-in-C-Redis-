

#include "../conn.h"


/* What do i need to do ?
 * I need first to define an mechanism to parse and handle the SET and GET commands .
 * ALSO i need to implement an hashmap to store these values .
 *
 * Step 1 :
 *  - can a set function have multiple Key values at once or just one ?
 *  -> SET
 */


bool store_kv(char *key , int key_length , char *value , int value_length );
char *retreive_value(char *key , int key_length );


