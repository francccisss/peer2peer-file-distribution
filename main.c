#include "dynamic_array.h"
#include "nodes.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // each entry is a pointer to an array of peers
  bucket_t *(*peer_table)[MAX_SIZE_ARRAY] = malloc(sizeof(bucket_t) * MAX_SIZE_ARRAY);
  for (int i = 0; i < MAX_SIZE_ARRAY; ++i)
  {
  (*peer_table)[i]= new_array();
  };
  printf("[TEST]: len =%ld",(*peer_table)[0]->len);


  if (*peer_table == NULL) {
    perror("malloc error"); return 1;
  };

  set(peer_table,"f",(peer_t){.ip = "192.168.1.1", .id = "69", .port = 3000});
  // free(peer_table);
};
