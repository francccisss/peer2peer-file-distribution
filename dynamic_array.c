#include "dynamic_array.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "nodes.h"

/*
 * don't use with hashmaps, the hash value
 */


// typedef struct {
//     size_t cap;
//     size_t len;
//     peer_t (*data)[INITIAL_CAP];
// } peer_arr_t;
peer_arr_t *new_array() {
    peer_arr_t* dyn_arr = malloc(sizeof(peer_arr_t));
    if (dyn_arr == NULL) return NULL;

    peer_t (*arr)[INITIAL_CAP] = malloc(sizeof(peer_t) * INITIAL_CAP);
    if (arr == NULL) return NULL;

    // data contains raw memory
    dyn_arr->data = arr;
    dyn_arr->cap = INITIAL_CAP;

    return dyn_arr;
};

// within maximum capacity, need to manually call resize
void resize(peer_arr_t *d_arr, const size_t cap) {
    constexpr size_t twice = 2 ;
    void* ptr = realloc(d_arr->data, (cap * sizeof(peer_t))*twice);
    if (ptr == NULL) {
        free(ptr);
        return;
    }
    d_arr = ptr;
    d_arr->cap = cap * twice; // double the size of the capacity of the array
}



