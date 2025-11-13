#include "nodes.h"
#include "remote_procedure.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void get_peers(int s_fd, node_array *sorted_neighbors, char *hash_info) {
  for (int i = 0; i < sorted_neighbors->len; i++) {

    node_t n = (*sorted_neighbors->data)[i];
    origin d_host = {
        .port = n.port,
    };
    strcpy(d_host.ip, n.ip);

    int rs = call_rpc(s_fd, GET_PEERS, "wtf", 0, d_host);

    if (rs < 0) {
      printf("[WARN]: unable to initiate GET_PEERS call with distance=%d\n",
             n.distance);
      continue;
    };
  }
};

void bootstrap_neigbors(node_array *src, size_t n_count, node_array *dst) {
  for (int i = 0; i < n_count; i++) {
    push_node(dst, (*src->data)[i]);
  }
};

// xor binary representation of both hash and node id
// return an integer that represents the logical distance
// between the node and the file
//
// using uuid for now cause why not :D

void XORdistance(char *hash_info, node_t *node) {

  for (int i = 0; i < strlen(hash_info); i++) {
    node->distance += hash_info[i] ^ node->id[i];
  };
}

void compare_hash(node_array *neighbors, size_t n_count, char *hash_info) {
  // sorts neighbors by closest

  for (int i = 0; i < n_count; i++) {
    XORdistance(hash_info, &(*neighbors->data)[i]);
  }

  int i = 0;
  while (i < n_count) {
    int j = i;
    while (j > 0 && (*neighbors->data)[j - 1].distance >
                        (*neighbors->data)[j].distance) {
      node_t *current_node = &(*neighbors->data)[j];
      node_t tmp = *current_node;
      *current_node = (*neighbors->data)[j - 1];
      (*neighbors->data)[j - 1] = tmp;
      j--;
    };
    i++;
  }
};

node_array *new_node_array() {
  node_array *dyn_arr = malloc(sizeof(node_array));
  if (dyn_arr == NULL)
    return NULL;
  node_t(*arr)[MAX_NEIGHBORS] = malloc(sizeof(node_t) * MAX_NEIGHBORS);
  if (arr == NULL)
    return NULL;
  dyn_arr->data = arr;
  dyn_arr->len = 0;
  dyn_arr->cap = MAX_NEIGHBORS;
  return dyn_arr;
};

// within maximum capacity, need to manually call resize
// this function will be called within push function, to auto resize
// on pushing new items in the array
void resize_node_array(node_array *d_arr) {
  const size_t twice = 2;
  // pointing to a block of memory
  void *ptr = realloc((*d_arr->data), (d_arr->cap * sizeof(node_t)) * twice);
  if (ptr == NULL) {
    perror("[ERROR]: realloc");
    exit(1);
  }

  d_arr->data = ptr; // data should be pointing to the reallocated memory

  printf("[TEST]: OLD cap=%ld\n", d_arr->cap);
  d_arr->cap =
      d_arr->cap * twice; // double the size of the capacity of the array
  printf("[TEST]: NEW cap=%ld\n", d_arr->cap);
}

void push_node(node_array *d_arr, const node_t data) {
  if (d_arr->len == d_arr->cap) {
    resize_node_array(d_arr);
  }
  // d_arr->data +1;
  // increments the pointer by its stride N * sizeof(node_t)
  // (*d_arr->data)[0];
  // increments from the array itself by sizeof(node_t)
  memcpy(&(*d_arr->data)[d_arr->len], &data, sizeof(node_t));
  d_arr->len++;
}

void pop_node(node_array *d_arr, node_t *peer_buf) {
  if (d_arr->len < 1) {
    return;
  }
  *peer_buf = (*d_arr->data)[d_arr->len - 1];
  (*d_arr->data)[d_arr->len - 1] = (node_t){0};
  d_arr->len--;
}
