#include "dynamic_array.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAP 8
/*
 * don't use with hashmaps, the hash value
 */


dynamic_array *new_array(const size_t obj_size) {
    void* arr_ptr = malloc(obj_size * INITIAL_CAP);
    if (arr_ptr == NULL) return NULL;

    dynamic_array *dyn_arr = malloc(sizeof(dynamic_array));
    if (dyn_arr == NULL) return NULL;

    // data contains raw memory
    dyn_arr->data = arr_ptr;
    dyn_arr->cap = INITIAL_CAP;

    return dyn_arr;
};

void resize(dynamic_array *d_arr, const size_t cap, const size_t obj_size) {
    void* ptr = realloc(*(d_arr->data), (cap * obj_size)*2);
    if (ptr == NULL) {
        return;
    }
    d_arr = ptr;
    d_arr->cap = cap * 2; // double the size of the capacity of the array
}



