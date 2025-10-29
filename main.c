#include "dynamic_array.h"
#include "nodes.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  peer_t (*peer_table)[INITIAL_CAP] = malloc((sizeof(peer_t) * INITIAL_CAP) * MAX_SIZE_ARRAY);
  if (peer_table == NULL) {
    perror("malloc error"); return 1;
  };

  set(peer_table,"franz",(peer_t){.ip = "192",.id="francois",.port=3000});
  peer_t *peer_list = malloc(sizeof(peer_t) * INITIAL_CAP);
  get(peer_table,"franz",peer_list);
  printf("[TEST]: key: franz, value:");
  for (int i = 0; i < INITIAL_CAP; i++) {
    printf("Location: %p",(void*)peer_list++);
  }

  free(peer_list);
  free(peer_table);
};
