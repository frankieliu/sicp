#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include "hash.h"

#define debugLevel 0
#define debug(level,etc) {if (level <= debugLevel) etc}
#define printline(level) debug(level, printf("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);)

void ht_free ();
int ht_key_compare ();
/* Create a new hashtable. */
hashtable_t *ht_create (size_t size,
			void (*freefn)(hashtable_value_t),
			int (*cmp)(hashtable_key_t *,hashtable_key_t *)) {
  hashtable_t *hashtable;
  if (size < 1) return NULL;
  if ((hashtable = malloc (sizeof (hashtable_t))) == NULL) { return NULL; }
  if ((hashtable->table = malloc (sizeof (hashtable_entry_t *) * size)) == NULL) { return NULL; }
  for (size_t i = 0; i < size; i++) { hashtable->table[i] = NULL; }
  hashtable->size = size;
  if (freefn == NULL) hashtable->free = &ht_free; else hashtable->free = freefn;
  if (cmp == NULL) hashtable->cmp = &ht_key_compare; else hashtable->cmp = cmp;
  return hashtable;	
}

/* Hash a string for a particular hash table. */
uint32_t ht_hash (hashtable_t *hashtable, hashtable_key_t *key) {
  uint32_t hash, i;
  // Jenkins hash
  for (hash = i = 0; i < key->length; i++) {
    hash += key->s[i], hash += (hash << 10), hash ^= (hash >> 6); }
  hash += (hash << 3), hash ^= (hash >> 11), hash += (hash << 15);
  return hash % hashtable->size;
}

/* Create a key-value pair. */
hashtable_entry_t *ht_newpair (hashtable_key_t *key_p, hashtable_value_t value) {
  hashtable_entry_t *newpair;
  if ((newpair = malloc (sizeof (hashtable_entry_t))) == NULL) { return NULL; }
  newpair->key_p = key_p; newpair->value = value; newpair->next = NULL;
  return newpair;
}

void ht_free (hashtable_value_t value) {}

int ht_key_compare (hashtable_key_t *key1, hashtable_key_t *key2) {
  if (key1->length > key2->length) return 1;
  else if (key1->length < key2->length) return -1;
  else {
    for (size_t i = 0; i < key1->length; i++) {
      if (key1->s[i] > key2->s[i]) return 1;
      else if (key1->s[i] < key2->s[i]) return -1;
    }
    return 0;
  }
}
  
/* Insert a key-value pair into a hash table. */
void ht_set (hashtable_t *hashtable, hashtable_key_t *key, hashtable_value_t value) {
  size_t bin = 0;

  hashtable_entry_t *next = NULL;
  hashtable_entry_t *last = NULL;

  bin = ht_hash (hashtable, key);
  last = next = hashtable->table[bin];

  int cmp = 0;
  while (next != NULL && next->key_p != NULL && (cmp = hashtable->cmp (key, next->key_p)) > 0) {
    last = next; next = next->next;}
  if (next != NULL && next->key_p != NULL && cmp == 0) {    // same key
    hashtable->free (next->value);
    next->value = value;
  } else {                                                // new key
    hashtable_entry_t *newpair = ht_newpair (key, value);
    newpair->next = next;
    if (last == hashtable->table[bin]) {
      hashtable->table[bin] = newpair;
    } else {
      last->next = newpair;
    }
  }
}

/* Retrieve a key-value pair from a hash table. */
hashtable_value_t ht_get (hashtable_t *ht_p, hashtable_key_t *key_p) {
  printline (1);

  debug (1, printf ("Hash table: %ld\n",(size_t) ht_p););
  
  // Get the bin number
  uint32_t bin = 0;
  bin = ht_hash (ht_p, key_p);
  debug (1, printf ("Hash bin: %d\n",bin);)

  // Search in bin
  hashtable_entry_t *pair;
  pair = ht_p->table[bin];
  debug (1,printf ("Hash table[bin]: %ld\n",(size_t) ht_p->table[bin]);)

  int cmp = 0;
  while (pair != NULL && pair->key_p != NULL && (cmp = ht_p->cmp (key_p, pair->key_p)) > 0)
    pair = pair->next;

  if (pair == NULL || pair->key_p == NULL || cmp != 0) 
    return NULL;
  else
    return pair->value;
}

// Helpers:
hashtable_key_t *ht_new_key(char *s) {
  hashtable_key_t *n = (hashtable_key_t *) calloc (1, sizeof(hashtable_key_t));
  *n = (hashtable_key_t){strlen(s),s}; // we don't allocate space for the char *
  return n;
}

// User should create their own casters
// Casters
void ht_set_char (hashtable_t *ht_p, hashtable_key_t *key_p, char *s) {
  ht_set (ht_p, key_p, (hashtable_value_t) s);
}
char *ht_get_char(hashtable_t *ht_p, hashtable_key_t *key_p) {
  return (char *) ht_get (ht_p, key_p);
}

int hashtable_main_test() {

  hashtable_t *ht_p = ht_create (65536, &ht_free, &ht_key_compare);

  hashtable_key_t *key1 = ht_new_key ("key1");
  hashtable_key_t *key2 = ht_new_key ("key2");
  hashtable_key_t *key3 = ht_new_key ("key3");
  hashtable_key_t *key4 = ht_new_key ("key4");

  ht_set_char (ht_p, key1, "inky");
  ht_set_char (ht_p, key2, "pinky");
  ht_set_char (ht_p, key3, "blinky");
  ht_set_char (ht_p, key4, "floyd");
  
  printf( "%s\n", ht_get_char (ht_p, key1));
  printf( "%s\n", ht_get_char (ht_p, key2));
  printf( "%s\n", ht_get_char (ht_p, key3));
  printf( "%s\n", ht_get_char (ht_p, key4));

  return 0;
}

void main0 (int argc, char **argv) {
  hashtable_main_test ();
}

