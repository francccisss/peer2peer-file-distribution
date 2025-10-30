#include "peer_table.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // Initializing node's peer table
  bucket_t *table[MAX_SIZE_ARRAY];
  for (int i = 0; i < MAX_SIZE_ARRAY; ++i) {
    table[i] = new_array();
  };

  set(&table, "f", (peer_t){.id = "123", .ip = "192.168.1.1", .port = 3000});

  bucket_t *bucket_buf;
  get(&table, "f", &bucket_buf);

  printf("[TEST]: len->%ld\n", bucket_buf->len);
};
