#include "peer_table.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u

// requires a table pointer to an array of pointers of type peer
// eg: init(node.peer_table);  to initialize the node peer_table
void init_table(peer_bucket_t *(*unint_table)[MAX_PEERS]) {
  // Initializing node's peer table
  for (int i = 0; i < MAX_PEERS; ++i) {
    (*unint_table)[i] = new_array();
  };
};

uint32_t hash(const char *input) {
  char b;
  uint32_t hash = HASH_OFFSET;
  while ((b = *input++)) {
    hash ^= (unsigned)b;
    hash *= PRIME;
  }

  printf("hash value=%d\n", hash);
  printf("hash mod value=%d\n", hash % MAX_PEERS);

  return hash % MAX_PEERS;
}

void set(peer_bucket_t *(*table)[MAX_PEERS], const char *key,
         const peer_t data) {
  const uint32_t hash_key = hash(key);
  // current peer_bucket is occupied given by hash
  printf("[TEST]: peer_bucket key value: %d\n", (*table)[hash_key]->key);
  // just because it's not equal doesn't mean it does not exist

  /// does a peer_bucket with this hash_key exist
  if ((*table)[hash_key]->active) {
    if ((*table)[hash_key]->key == hash_key) {
      push((*table)[hash_key], data);
      return;
    }
    size_t index = hash_key + 1;
    while (index < MAX_PEERS) {
      // find one that is not occupied
      if ((*table)[index]->len == 0) {
        (*table)[index]->key = index;
        push((*table)[index], data);
        return;
      };
      index++;
    };
  }
  (*table)[hash_key]->key = hash_key;
  (*table)[hash_key]->active = true;
  push((*table)[hash_key], data);
}

/*
 *  key represents the info_hash which is used as a peer_bucket entry to the
 * hash table, it returns the list of peers within that network for the new node
 * to connect to
 */

/// This is bad, because the insertion in this hashmap, where 2 keys might
/// collide which results into a linear probing, so insertion time == retrieval
/// time as insertion increases linearly
void get(peer_bucket_t *(*table)[MAX_PEERS], const char *key,
         peer_bucket_t **peer_bucket_buf) {
  const uint32_t hash_key = hash(key);
  uint32_t current_key = (*table)[hash_key]->key;
  do {
    if (hash_key == current_key) {
      *peer_bucket_buf = (*table)[hash_key];
      return;
    }
    current_key++;
  } while (current_key < MAX_PEERS);

  // (current_key...MAX_PEERS-1) yikes
  printf("[INFO]: peer_bucket does not exist");
}

peer_bucket_t *new_array() {
  peer_bucket_t *dyn_arr = malloc(sizeof(peer_bucket_t));
  if (dyn_arr == NULL)
    return NULL;
  peer_t(*arr)[INITIAL_CAP] = malloc(sizeof(peer_t) * INITIAL_CAP);
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
void resize(peer_bucket_t *d_arr) {
  const size_t twice = 2;
  // pointing to a block of memory
  void *ptr = realloc((*d_arr->data), (d_arr->cap * sizeof(peer_t)) * twice);
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

void push(peer_bucket_t *d_arr, const peer_t data) {
  if (d_arr->len == d_arr->cap) {
    resize(d_arr);
  }
  // d_arr->data +1;
  // increments the pointer by its stride N * sizeof(peer_t)
  // (*d_arr->data)[0];
  // increments from the array itself by sizeof(peer_t)
  memcpy(&(*d_arr->data)[d_arr->len], &data, sizeof(peer_t));
  d_arr->len++;
}

void pop(peer_bucket_t *d_arr, peer_t *peer_buf) {
  if (d_arr->len < 1) {
    return;
  }
  *peer_buf = (*d_arr->data)[d_arr->len - 1];
  (*d_arr->data)[d_arr->len - 1] = (peer_t){0};
  d_arr->len--;
}
