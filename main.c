#include "dynamic_array.h"
#include "nodes.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // each entry is a pointer to an array of peers
  bucket_t *(*table)[MAX_SIZE_ARRAY] = malloc(sizeof(bucket_t) * MAX_SIZE_ARRAY);
  for (int i = 0; i < MAX_SIZE_ARRAY; ++i)
  {
  (*table)[i]= new_array();
  };


  if (*table == NULL) {
    perror("malloc error"); return 1;
  };

  set(table,"f",(peer_t){.ip = "192.168.1.1", .id = "69", .port = 3000});
  printf("[TEST]: len =%ld\n",(*table)[0]->len);

  bucket_t *bucket_buf ;
  get(table,"f",&bucket_buf);
  printf("[TEST]: len =%ld\n",bucket_buf->len);

  free(table);
};
