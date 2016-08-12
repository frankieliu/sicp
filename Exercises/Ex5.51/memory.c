#include <stdlib.h>
#include <stdio.h>

/***********************************************************************
 * (725) Representing Lisp data
 *
 * "A pointer to a pair is an index into the two vectors.  The car of
 * the pair is the entry in the-cars with the designated index, and the
 * cdr of the pair is in the entry in the-cdrs with the designated
 * index.
 *
 ***********************************************************************/

/* Some basic types */
typedef enum {
  Tnumber, Tpair, Tnull, Tbroken_heart
} TypeT;

typedef unsigned long Tval;

typedef struct T {
  TypeT type;
  Tval val;
} T;

typedef unsigned int Tidx;

/*----------------------------------------------------------------------
 *
 * the_cars and the_cdrs ...
 *
 *----------------------------------------------------------------------*/

Tidx memory_size;
T* the_cars;
T* the_cdrs;
Tidx free_idx;   // Free pointer (index)
T root;          // Element pointing to the root

// Garbage collection
T* new_cars;
T* new_cdrs;
T broken_heartT;
T nullT;

Tidx scan_idx;   // Scanning index

T vector_set(T* root, Tidx i, T e){
  root[i].type = e.type;
  root[i].val = e.val;
  return root[i];
}

T vector_ref(T* root, Tidx i) {
  return root[i];
}

void print_T(T e){
  switch(e.type){

  case Tnumber:
    printf("n%d",e.val);
    break;
  case Tpair:
    printf("p%d",e.val);
    break;
  case Tnull:
    printf("e0"); // page 727 Fig 5.14
    break;
  case Tbroken_heart:
    printf("bh");

  default:
    break;
  }

}

T cons(T a, T b){
  Tidx i = free_idx;

  vector_set(the_cars, i, a);
  vector_set(the_cdrs, i, b);
  free_idx = i+1;

  T p = {Tpair, (Tval) i};
  return p;
}

void init_memory(Tidx mem_size){
  memory_size = mem_size;
  the_cars = (T *) calloc(memory_size, sizeof(T));
  the_cdrs = (T *) calloc(memory_size, sizeof(T));
  new_cars = (T *) calloc(memory_size, sizeof(T));
  new_cdrs = (T *) calloc(memory_size, sizeof(T));
  free_idx = 0;
  broken_heartT.type = Tbroken_heart;
  broken_heartT.val = (Tval) 0;
  nullT.type = Tnull;
  nullT.val = (Tval) 0;
}

void print_memory(){
  Tidx i;
  for(i=0; i<free_idx; i++){
    printf("(%d, ",i);
    print_T(vector_ref(the_cars,i));
    printf(", ");
    print_T(vector_ref(the_cdrs,i));
    printf(")\n");
  }
}

/*----------------------------------------------------------------------
 * Garbage collection
 *----------------------------------------------------------------------*/

T relocate_old_result_in_new(T old);
void gc_loop();

void begin_garbage_collection() {
  free_idx = 0;
  scan_idx = 0;
  root = relocate_old_result_in_new(root);
  // Move the root contents over, and point the
  // new root_idx to it
  gc_loop();
}

/*
  The gc_loop takes an element from
  new_cars -- which is might be pointing to
  the old stuff, and have that information
  relocated.  And then change it to point
  to the new location.
  
  It gets kind of confusing, because we
  can be dealing with an index or actual
  pointer to the element.

  (insert-image
  (create-image
  "~/Dev/Chicken/Images/Ex5.51.png")
  )

 */    	 

void gc_loop() {

  while (scan_idx != free_idx) {

    T old;
    T new;

    // operate on the car
    old = vector_ref(new_cars, scan_idx);
    new = relocate_old_result_in_new(old);
    if (old.type != Tpair)
      vector_set(new_cars, scan_idx, new);

    // operate on the cdr
    old = vector_ref(new_cdrs, scan_idx);
    new = relocate_old_result_in_new(old);
    if (old.type != Tpair)
      vector_set(new_cdrs, scan_idx, new);

    scan_idx++;

  }

  // gc_flip():
  T* temp;
  temp = the_cars; the_cars = new_cars; new_cars = temp;
  temp = the_cdrs; the_cdrs = new_cdrs; new_cdrs = temp;

}

T pair(T old);

// This should return the pointer to the new item
T relocate_old_result_in_new(T old) {
  if (old.type == Tpair) {
    return pair(old);
  } else { // if it is not a pair we just return the element
    return old;
  }
}

/*

  A subtle point here: old from new_cars supposedly contains the "old"
  pair from the_cars (or the_cdrs), and in fact it does!  The point
  here is that we need to check whether the "old" pair in the original
  has been modified by garbage collection, i.e. has it been
  transformed into a broken-heart or not.

 */

T pair(T old){

  Tidx old_idx = old.val;
  T oldcar = vector_ref(the_cars, old_idx);
  T new;

  if (oldcar.type == Tbroken_heart) {    // a pair that already moved

    return vector_ref(the_cdrs, old_idx); // the_cdrs points to a new_cars pair

  } else { // a pair that has not been moved yet

    // copy over to new_cars and new_cdrs
    vector_set(new_cars, free_idx, oldcar);
    vector_set(new_cdrs, free_idx, vector_ref(the_cdrs, old_idx));

    // we need a new element to refer to this pair
    new.type = Tpair;
    new.val = free_idx;
    
    // put a broken-heart to mark that it has already been moved
    vector_set(the_cars, old_idx, broken_heartT);
    vector_set(the_cdrs, old_idx, new);

    free_idx++;

    return new;
  }
}

/*----------------------------------------------------------------------
 *
 * One of the problems with this implementation is
 * what goes into the memory and what is retained
 * for example if I say x = (list 1 2 3)
 * we can create a list containing
 *
 * n1/p2 n2/p3 n3/e0
 * 
 * The question is whether we also create a p1/e0
 * which would point to this list, I suppose that
 * if we create a symbol table containing x then
 * it would have contain p1/e0, so maybe we can
 * "wait" until such symbol table is created.
 *
 *----------------------------------------------------------------------*/


int main(){

  // First let's create a data structure
  
  T a = {Tnumber, (Tval) 1};
  T b = {Tnumber, (Tval) 2};

  T c = {Tnumber, (Tval) 3};
  T d = {Tnumber, (Tval) 4};

  Tidx mem_size = 1024;
  init_memory(mem_size);

  T p1 = cons(a, (cons(b, nullT)));
  T p2 = cons(c, (cons(d, nullT)));
  T p3 = cons(p1, p2);
  root = cons(p1, nullT);
  print_memory();
  begin_garbage_collection();
  print_memory();

}

