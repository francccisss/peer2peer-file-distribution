#include "dynamic_array.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nodes.h"

/*
 * don't use with hashmaps, the hash value
 */


peer_arr_t *new_array() {
    peer_arr_t* dyn_arr = malloc(sizeof(peer_arr_t));
    if (dyn_arr == NULL) return NULL;

    peer_t (*arr)[INITIAL_CAP] = malloc(sizeof(peer_t) * INITIAL_CAP);
    if (arr == NULL) return NULL;

    dyn_arr->data = arr;
    dyn_arr->len = 0;
    dyn_arr->cap = INITIAL_CAP;

    return dyn_arr;
};

// within maximum capacity, need to manually call resize
// this function will be called within push function, to auto resize
// on pushing new items in the array
void resize(peer_arr_t *d_arr) {
    constexpr size_t twice = 2 ;
    // pointing to a block of memory
    void* ptr = realloc((*d_arr->data), (d_arr->cap * sizeof(peer_t))*twice);
    if (ptr == NULL) {
        perror("[ERROR]: realloc");
        exit(1);
    }

    d_arr->data = ptr; // data should be pointing to the reallocated memory

    printf("[TEST]: OLD cap=%ld\n",d_arr->cap);
    d_arr->cap = d_arr->cap * twice; // double the size of the capacity of the array
    printf("[TEST]: NEW cap=%ld\n",d_arr->cap);
}
void push(peer_arr_t *d_arr, const peer_t *data) {
    if (d_arr->len == d_arr->cap) {
        resize(d_arr);
    }
    peer_t *end_ptr = (*d_arr->data) + (sizeof(peer_t) * d_arr->len);
    memcpy(end_ptr,data ,sizeof(peer_t));
    d_arr->len++;
    printf("[TEST]: NEW len=%ld\n",d_arr->len);
    printf("[TEST]: cap=%ld\n",d_arr->cap);
}

peer_t *pop(peer_arr_t *d_arr) {
    return NULL;
}




