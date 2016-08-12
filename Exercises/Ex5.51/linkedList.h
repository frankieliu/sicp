#ifdef LINKEDLIST_H
#define LINKEDLIST_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

//-----------------------------------------------------------------------

typedef void * node_data;

//-----------------------------------------------------------------------
// Sample data type

typedef struct Int { int x; } Int;
node_data Int_new(int n, ...);
bool      Int_equal(node_data n, node_data m);
void      Int_destroy(node_data i);
void      Int_print(node_data n);

//----------------------------------------------------------------------

typedef struct linkedList_node {
  node_data data;
  struct linkedList_node *next;
} linkedList_node;

typedef struct linkedList {
  linkedList_node *head;
  node_data (*new) (int,...);
  bool (*equal)(node_data, node_data);
  void (*destroy)(node_data);
  void (*print)(node_data);
  struct linkedList *self;
} linkedList;

int linkedList_insert_at_begin(linkedList* l, node_data val);
int linkedList_insert_at_index(linkedList *l, node_data val, int pos);
int linkedList_insert_after_data_match(linkedList *l, node_data val, node_data loc);
int linkedList_insert_at_end(linkedList *l, node_data val);
int linkedList_delete_at_begin(linkedList* l);
int linkedList_delete_at_end(linkedList* l);
int linkedList_delete_at_index(linkedList *l, int pos);
int linkedList_delete_data_match(linkedList *l, node_data loc);
int linkedList_display(linkedList *l);
int linkedList_modify_data_match(linkedList *l, node_data loc, node_data val);
int linkedList_find_data_match(linkedList *l, node_data val);

#endif
