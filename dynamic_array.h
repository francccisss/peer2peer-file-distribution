#include <stddef.h>
#include "nodes.h"

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#define INITIAL_CAP 8
typedef struct {
    size_t cap;
    size_t len; // always access last element at len-1 because of 0 indexing
    peer_t (*data)[INITIAL_CAP];
} peer_arr_t;



/*
 * Caller function should handle the pointer returned by these functions
 * Always free the array after using
 */

peer_arr_t *new_array();
void resize(peer_arr_t*d_arr);
void push(peer_arr_t *d_arr, peer_t data) ;
void pop(peer_arr_t *d_arr, peer_t *peer_buf);




#endif
