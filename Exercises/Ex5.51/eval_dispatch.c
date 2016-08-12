/*
http://howtowriteaprogram.blogspot.com/2010/11/lisp-interpreter-in-90-lines-of-c.html
*/

#include <stdio.h>
// Needed for malloc
#include <stdlib.h>
// Needed for memcpy
#include <string.h>

typedef enum { false, true } bool;

/*----------------------------------------------------------------------
  Create a generic type
  ----------------------------------------------------------------------*/

typedef enum {
  NumberT,
  StringT,
  FnT,
  PairT,
  BrokenHeartT
} Ttype;

typedef struct T { Ttype type; void (* fn)(void); } T;

bool numberP(T t){ return t.type == NumberT; }
bool stringP(T t){ return t.type == StringT; }
bool brokenHeartP(T t){ return t.type == BrokenHeartT; }
bool self_evaluatingP(T t){ return numberP(t) || stringP(t); }

/*----------------------------------------------------------------------
  Define registers
  ----------------------------------------------------------------------*/

T val;
T expr;
T cont;

/*----------------------------------------------------------------------
  Linked list
 *----------------------------------------------------------------------*/

typedef struct Node { void *data; struct Node *next; } Node;
typedef Node* LinkedList;

void push(LinkedList* head, void *data, size_t size){
  // Make a new node
  Node *new = (Node *) malloc(sizeof(Node));
  new->data = malloc(size);
  memcpy(new->data,data,size);
  new->next = *head;
  *head = new;
}

void printList(LinkedList l,void (*fn)(void *)){
  while(l != NULL){
    fn(l->data);
    l = l->next;
  }
}

void printInt(void *n){ printf("%d",*(int *)n); }

void printString(void *n){ printf("%s",(char *)n); }

/*----------------------------------------------------------------------
  Define memory
  ----------------------------------------------------------------------*/

#define MEMORY-SIZE 1024
typedef struct Memory {
  vector cars;
  vector cdrs;
} Memory;

// 733
Memory working_memory;
Memory free_memory;
Memory *root;
T* free
T* scan;

T old;
T root;
T new;
T relocate_continue;
T oldcr;
T temp;
T the_cdrs;
T the_cars;
T new_cdrs;
T new_cars;

void reassign_root();
void relocate_old_result_in_new();
void gc_loop();
void gc_flip();
void vector_ref();
void vector_setM();
void pair();
void already_moved();

void begin_garbage_collection(){

  free = NULL;
  scan = NULL;

  // root is the "root" of all the data structures
  // we need to move to the new location

  // Old selects the next element for relocation  
  old = root;

  // As the name suggests,
  // we move what is pointed
  // in old to new

  relocate_continue = &reassign_root;
  relocate_old_result_in_new();
  
}

void relocate_old_result_in_new(){

  // If the pointer is a pair then
  // we relocate the pair
  if (pointer_to_pairP(old)) {

    pair();
    
  } else {

    // If the object to be relocated (pointed at by old)
    // is not a pair, then we return the same pointer to
    // the object unchanged (in new).
    //
    // For example:
    // We may be scanning a pair whose car is the number 4
    // If we represent the car by n4, then we want the
    // "relocated" car pointer to still be n4.
    
    new = old;
    relocate_continue();

  }
}

void reassign_root(){
  root = new;
  gc_loop();
}

void pair(){

  // The elemented pointed by old
  oldcr = vector_ref(the_cars, old);

  // If the car contains a broken-heart tag,
  // then the pair has in fact already been moved,
  // so we retrieve the forwarding address
  
  if (oldcr == broken_heartC)

    already_moved();

  else {

    // Actually move data
    
    new = free; // new location for pair
    free++;     // update free pointer
    
    // Copy the car and cdr to new memory
    vector_setM(new_cars, new, oldcr);  // Copy the car
    oldcr = vector_ref(the_cdrs, old);
    vector_setM(new_cdrs, new, oldcr);  // Copy the cdr
    
    // Construct the broken-heart
    vector_setM(the_cars, old, broken_heartC);  // the 'old' car to a broken_heart
    vector_setM(the_cdrs, old, new);            // the 'old' cdr to new location

    relocate_continue();

  }

}

void already_moved(){

  // Since the object was already moved, set new to the saved location
  new = vector_ref(the_cdrs, old);
  relocate_continue();

}

void gc_loop(){

  if (scan == free) {  // Done garbage collecting

    gc_flip();

  } else {
    
    old = vector_ref(new_cars, scan);  // Get the element at scan
    relocate_continue = &update_car;
    relocate_old_results_in_new();
    
  }

}

void update_car(){
  vector_setM(new_cars, scan, new);  // Update the car at scan point to new (new carries the results)
  old = vector_ref(new_cdrs, scan);  // Get a new "old"
  relocate_continue = &update_cdr;   // Relocate "old" (follow the linked list)
  relocate_old_result_in_new();      // After relocating the "old" update_cdr
}

void update_cdr(){
  vector_setM(new_cdrs, scan, new);  // Update the cdr at scan point to new (new carries the results)
  scan++;                            // Scan the next element
  gc_loop();                         /
}

void gc_flip(){
  
  // gc_flip:
  temp = the_cdrs;
  the_cdrs = new_cdrs;
  new_cdrs = temp;
  
  temp = the_cars;
  the_cars = new_cars;
  new_cars = temp;
  
}

/*----------------------------------------------------------------------*
  Memory as vectors (5.3.1)
 *----------------------------------------------------------------------*/
typedef unsigned long VectorIndex;

void vector_ref(Vector v, VectorIndex n){
}
void vector_setM(Vector v, VectorIndex n, T t){
  
}

// page 725
T car(T t){
  if (t.type == PairT) {
    return memory->the-cars[t.value];
  } else {
    error("car: t is not a pair");
  }
}

T cdr(T t){
  if (t.type = PairT) {
    return memory->cdrs[t.value];
  } else {
    error("cdr: t is not a pair");
 }
}

/*----------------------------------------------------------------------
  Define symbols
  ----------------------------------------------------------------------*/

typedef Node S;
LinkedList sTable;
S sQuote;

void symbolTableInit(){
  char a[]="quote";
  push(&sTable,(void *) a, sizeof(a));
}
/*----------------------------------------------------------------------
  Dispatcher
  ----------------------------------------------------------------------*/

// bool tagged_listP(T t,S s){}
bool tagged_listP(T expr,S tag){
  if (pairP(expr)) {
    return (car(expr) == 
}
  
bool quotedP(T t){ return tagged_listP(t,sQuote) }

void ev_self_eval(){
  printf("ev-self-eval:\n");
  val = expr;
  (void *) (*cont.value(void *))();
}

void ev_quoted(){
 
}
	       
void eval_dispatch() {
  /*
   This section is the dispatch handler
   eval-dispatch
   (test (op variable?) (reg exp))
   (branch (label ev-variable))
   (test (op quoted?) (reg exp))
   (branch (label ev-quoted))
   (test (op assignment?) (reg exp))
   (branch (label ev-assignment))
   (test (op definition?) (reg exp))
   (branch (label ev-definition))
   (test (op if?) (reg exp))
   (branch (label ev-if))
   (test (op lambda?) (reg exp))
   (branch (label ev-lambda))
   (test (op begin?) (reg exp))
   (branch (label ev-begin))
   (test (op application?) (reg exp))
   (branch (label ev-application))
   (goto (label unknown-expression-type))
  */

  printf("eval-dispatch:\n");
  expr.type = NumberT;
  if(self_evaluatingP(expr)) ev_self_eval();
  // if(quotedP(expr)) ev_quoted();
}

void end() { printf("end:\n"); }

void linkedListTest(){
  int arr[]={1,2,3,4,5};
  LinkedList l = NULL;
  for (int i=0;i<=10;i++)
    push(&l, (void *) &i, sizeof(int));
  printList(l,&printInt);
}

void main(){
  printf("main:\n");
  cont.fn = &end;
  eval_dispatch();
  symbolTableInit();
  printList(sTable,&printString);
}
