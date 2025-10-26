#include <stddef.h>
#include "nodes.h"
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#define INITIAL_CAP 8
// accessing cap and len by back tracking base pointer of data multiplied by offsets
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

void resize(peer_arr_t*d_arr, size_t cap);




#endif