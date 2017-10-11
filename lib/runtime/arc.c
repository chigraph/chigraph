#include <stdlib.h>
#include <stdatomic.h>

// Stores an reference to a automitic reference counted object
// Memory layout of an arc object:
// 
// [ sizeof(size_t) bytes ] volitile reference count
// [ n bytes ] object
typedef void* arc_ref;

// Derefernece an arc object
void* chi_arc_deref(arc_ref obj) {
  return obj + 8;
}

// Get the ref count
size_t chi_arc_refcount(arc_ref obj) {
  return atomic_load((_Atomic size_t*)obj);
}

// creates a arc object and initialized it with a ref count of 1
// warning: the object itsself is filled with crap
arc_ref chi_arc_create(size_t object_size) {
  arc_ref ret;
  
  // allocate memory for it
  ret = malloc(sizeof(size_t) + object_size);
  
  // initialize the atomic
  atomic_init((_Atomic size_t*)ret, 1);
  
  return ret;
}

void chi_arc_add_ref(arc_ref obj) {
  
  // increment
  atomic_fetch_add((_Atomic size_t*)obj, 1);
}

// deallocates if refcount is now zero
void chi_arc_remove_ref(arc_ref obj) {
  
  size_t oldVal = atomic_fetch_sub((_Atomic size_t*)obj, 1);
  
  // see if pointer is dead
  if (oldVal == 1) {
    // deallocate
    free(obj);
  }
}
