#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nodes.h"
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u
#define MAX_SIZE_ARRAY 1000
#define MAX_NEIGHBORS 20


/*
 *  Functions and structs defined in should only be used by the DHT Nodes
 */

uint32_t hash(const char *input) {
  char b;
  uint32_t hash = HASH_OFFSET;
  while ((b = *input++)) {
    hash ^= (unsigned)b;
    hash *= PRIME;
  }

  printf("hash value=%d\n", hash);
  printf("hash mod value=%d \n", hash % MAX_SIZE_ARRAY);

  return hash % MAX_SIZE_ARRAY;
}


/*
 * Sets peers on each bucket of the hashmap
 *
 *
 * TODO: need dynamic array for peers to allocate more memory for incoming peers when peer len > capacity of bucket
 * change structure of bucket as a bucket type
 *
 * typedef struct {
 *   peer_t *peer_ptr; // dynamically allocate size of peer_t array
 *   size_t len;
 *   size_t capacity;
 * } bucket_t;
 *
 *
 * if (len is 80% of capacity){
 *  reallocate more memory
 * }
 *
 */

// TODO:
// Handling linear probing identification:
// say #sdka & #dkfi has the same key
// eg: key = 4 since #sdka already occupies the 5th bucket
// we need to iterate until we find an available bucket that
// has not been occupied yet eg: len == 0
// if so then add the new data in there


void set(bucket_t (*table)[MAX_SIZE_ARRAY], const char *key, peer_t data) {
  const uint32_t hash_key = hash(key);
  // current bucket is occupied given by hash
  if (table[hash_key]->len > 0 ) {
    size_t index = hash_key;
    // iterate through the buckets using linear probing
    while (index < MAX_SIZE_ARRAY) {
      // if current bucket is not occupied
      if (table[index]->len == 0) {
        table[index]->key =hash_key;
        push(table[hash_key],data);
        break;
      };
      index++;
    };
    return;
  }
  table[hash_key]->key =hash_key;
  push(table[hash_key],data);
}

/*
 *
 *  key represents the info_hash which is used as a peer_list entry to the table
 *  it returns the list of peers within that network for the new node to connect to
 *
 */

void get(const peer_t (*table)[], const char *key, bucket_t *peers_buf) {
  const uint32_t hash_key = hash(key);
  const peer_t *buf = memcpy(peers_buf,table[hash_key],sizeof(peer_t) * INITIAL_CAP);
  if (buf == NULL) {
    perror("Unable to copy peer list unto buffer");
    exit(EXIT_FAILURE);
  }
}

bucket_t *new_array() {
  bucket_t *dyn_arr = malloc(sizeof(bucket_t));
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
void resize(bucket_t *d_arr) {
  constexpr size_t twice = 2;
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

void push(bucket_t *d_arr, const peer_t data) {
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

void pop(bucket_t *d_arr, peer_t *peer_buf) {
  if (d_arr->len < 1){
    return;
  }
  *peer_buf = (*d_arr->data)[d_arr->len - 1];
  (*d_arr->data)[d_arr->len - 1] = (peer_t){0};
  d_arr->len--;
}
