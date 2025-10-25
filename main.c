#include <stdio.h>
#include <stdlib.h>

#include "dynamic_array.h"

int main(int arg, char **argv) {
    dynamic_array *arr = new_array(sizeof(char));
    if (arr == NULL) {
        perror("dynamic array");
        return 1;
    }
    char **data_ptr =(char **) arr->data;

    data_ptr[0] = "wtf";
    data_ptr[1] = "shit";

    printf("[TEST]: %s\n", data_ptr[0]);
    printf("[TEST]: %s\n", data_ptr[1]);

    free(arr->data);
    free(arr);
    return 0;
}


