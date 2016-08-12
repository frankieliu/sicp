#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef struct hashtable_key_s {
  size_t length;
  char *s;                         // For now assume char * keys
} hashtable_key_t;

typedef void * hashtable_value_t;  // Hashtable only stores the pointers to objects

typedef struct hashtable_entry_s {
  hashtable_key_t  *key_p;
  hashtable_value_t value;       // value is a pointer to a data struct
  struct hashtable_entry_s *next;
} hashtable_entry_t;

typedef struct hashtable_s {
  size_t size;
  hashtable_entry_t **table;
  void (*free)(hashtable_value_t);           // A function to free the data
  int (*cmp)(hashtable_key_t*,hashtable_key_t*); // A function to compare hashkeys
} hashtable_t;

/* Create a new hashtable. */
hashtable_t *ht_create (size_t size,
			void (*freefn)(hashtable_value_t),
			int (*cmp)(hashtable_key_t *,hashtable_key_t *));

/* Hash a string for a particular hash table. */
uint32_t ht_hash (hashtable_t* hashtable, hashtable_key_t* key);

/* Create a key-value pair. */
hashtable_entry_t *ht_newpair (hashtable_key_t* key, hashtable_value_t value );

/* Insert a key-value pair into a hash table. */
void ht_set (hashtable_t* hashtable, hashtable_key_t* key, hashtable_value_t value );

/* Retrieve a key-value pair from a hash table. */
hashtable_value_t ht_get (hashtable_t* hashtable, hashtable_key_t* key);

/* Helpers */
hashtable_key_t *ht_new_key(char *s);

#endif
