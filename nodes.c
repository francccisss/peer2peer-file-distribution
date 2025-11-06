#include "nodes.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void bootstrap_neigbors(node_t **boot_neighbors, size_t n_count,
                        node_t **node_neighbors) {
  *node_neighbors = malloc(sizeof(node_t) * n_count);
  for (int i = 0; i < n_count; i++) {
    printf("%d\n", i);
    void *l =
        memcpy(&node_neighbors[0][i], &boot_neighbors[0][i], sizeof(node_t));
    if (l == NULL)
      break;
  }
};

void compare_hash(node_t **neighbors,size_t n_count, char *hash_info, node_t **closest_neigbors) {
	for (int i = 0; i < n_count; i ++){

	}
};
