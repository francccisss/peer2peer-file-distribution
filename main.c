#include "dynamic_array.h"
#include "nodes.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // each entry is a pointer to an array of peers
  peers (*peer_table)[MAX_SIZE_ARRAY] = malloc((sizeof(peers) * INITIAL_CAP) * MAX_SIZE_ARRAY);
  if (peer_table == NULL) {
    perror("malloc error"); return 1;
  };

  set(peer_table,"franz",(peer_t){.ip = "192.168.1.1", .id = "69", .port = 3000});
  free(peer_table);
};
