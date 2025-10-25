#include <stddef.h>
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

// accessing cap and len by back tracking base pointer of data multiplied by offsets
typedef struct {
    size_t cap;
    size_t len;
    void **data;
}dynamic_array;


/*
 * Caller function should handle the pointer returned by these functions
 * Always free the array after using
 */

dynamic_array *new_array(size_t obj_size);

void resize(dynamic_array*d_arr, size_t cap, size_t obj_size);




#endif