
#pragma once
#include <stddef.h>
#include <stdbool.h>

const char* do_get(char *search_key , int key_length );
const char* do_set(char *search_key , int key_length , char *value  );
const bool do_expire(char *key, int key_length, size_t ttl  );