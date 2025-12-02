#include "nodes.h"
#include "remote_procedure.h"
#include <iso646.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int join_peers(int s_fd, node_t *node, char info_hash[ID_SIZE]) {
  peer_bucket_t *bucket_buf = malloc(sizeof(peer_bucket_t));
  get_peer_bucket(&(node->peer_table), info_hash, &bucket_buf);

  if (bucket_buf == NULL) {
    printf("[INFO]: bucket does not exist for hash=%s\n", info_hash);
    return 0;
  };
  printf("[TEST LEN]: bucket_len=%ld\n", bucket_buf->len);
  if (bucket_buf->len == 0) {
    printf("[INFO]: there are no peers to join in hash=%s\n", info_hash);
    return 0;
  };

  origin host = {.port = node->port};
  strcpy(host.ip, node->ip);
  for (int i = 0; i < bucket_buf->len; i++) {
    peer_t cur_peer = (*bucket_buf->data)[i];
    printf("[TEST CASTED BUF]: ip=%s, port=%d\n", cur_peer.ip, cur_peer.port);
    origin destination = {.port = cur_peer.port};
    strcpy(destination.ip, cur_peer.ip);
    call_rpc(s_fd, JOIN, NULL, 0, destination, host);
  };
  free(bucket_buf);
  return 0;
}

int get_peers(int s_fd, node_t *node, node_array *sorted_neighbors,
              char info_hash[ID_SIZE], origin src_addr, origin reply_to) {

  origin host = {
      .port = node->port,
  };
  strcpy(host.ip, node->ip);

  // GET_PEERS initator will be in payload
  origin src = {
      .port = htons(src_addr.port),
  };
  strcpy(src.ip, src_addr.ip);

  uint8_t payload[MAX_PAYLOAD_SIZE];

  // embed the src and info_hash within the payload
  memcpy(&payload, &src, sizeof(origin));
  memcpy(&payload[sizeof(origin)], info_hash, ID_SIZE);

  for (int i = 0; i < sorted_neighbors->len; i++) {

    node_t n = (*sorted_neighbors->data)[i];

    // compare this to the reply_to not the abs
    if (strcmp(n.ip, reply_to.ip) == 0 && n.port == reply_to.port) {
      printf("[TEST]: Don't send back to this neighbor\n");
      continue;
    }

    origin d_host = {
        .port = n.port,
    };
    strcpy(d_host.ip, n.ip);

    // passing `src_addr` to `call_rpc` so that the receiver will be able to
    // send it directly to the iniator instead of the nodes subsequent to it
    // after its callto reduce RTT
    int rs =
        call_rpc(s_fd, GET_PEERS, &payload, MAX_PAYLOAD_SIZE, d_host, host);

    if (rs < 0) {
      printf("[WARN]: unable to initiate GET_PEERS call with distance=%d\n",
             n.distance);
    };

    return rs;
  };

  return 0;
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

void XORdistance(char info_hash[ID_SIZE], node_t *node) {

  for (int i = 0; i < strlen(info_hash); i++) {
    node->distance += info_hash[i] ^ node->id[i];
  };
}

void compare_hash(node_array *neighbors, size_t n_count,
                  char info_hash[ID_SIZE]) {
  // sorts neighbors by closest

  for (int i = 0; i < n_count; i++) {
    XORdistance(info_hash, &(*neighbors->data)[i]);
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
  node_t(*arr)[INITIAL_CAP] = malloc(sizeof(node_t) * INITIAL_CAP);
  if (arr == NULL)
    return NULL;
  dyn_arr->data = arr;
  dyn_arr->len = 0;
  dyn_arr->cap = INITIAL_CAP;
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
