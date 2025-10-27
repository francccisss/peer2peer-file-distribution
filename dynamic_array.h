#include <stddef.h>
#include "nodes.h"
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#define INITIAL_CAP 8
typedef struct {
    size_t cap;
    size_t len;
    peer_t (*data)[INITIAL_CAP];
} peer_arr_t;



/*
 * Caller function should handle the pointer returned by these functions
 * Always free the array after using
 */

peer_arr_t *new_array();

void resize(peer_arr_t*d_arr);

// auto resize
// if pushing directly from d_arr->data, overflow might not get caught so handle seg fault
void push(peer_arr_t *d_arr, const peer_t *data) ;
peer_t *pop(peer_arr_t *d_arr);




#endif