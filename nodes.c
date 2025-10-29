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

void set(peer_t (*table)[INITIAL_CAP], const char *key, const peer_t data) {
  const uint32_t hash_key = hash(key);
  // current bucket is occupied given by hash
  if (table[hash_key][0].id != NULL  ) {
    size_t index = hash_key;
    // iterate through the buckets using linear probing
    while (index < MAX_SIZE_ARRAY) {
      // if current bucket is not occupied
      if (table[index][0].id== NULL) {
        size_t j = 0;
        // find space for new data to fit within the array bucket
        while (j < INITIAL_CAP) {
          if (table[index][j].id == NULL ) {
             table[index][j] = data;
            break;
          };
          j++;
        }
        break;
      };
      index++;
    };
  }
    table[hash_key][0] = data;
}

/*
 *
 *  key represents the info_hash which is used as a peer_list entry to the table
 *  it returns the list of peers within that network for the new node to connect to
 *
 */

void get(const peer_t (*table)[INITIAL_CAP], const char *key, peer_t *peer_buf) {
  const uint32_t hash_key = hash(key);
  const peer_t *buf = memcpy(peer_buf,table[hash_key],sizeof(peer_t) * INITIAL_CAP);
  if (buf == NULL) {
    perror("Unable to copy peer list unto buffer");
    exit(EXIT_FAILURE);
  }
}