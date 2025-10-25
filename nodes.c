#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u
#define MAX_SIZE_ARRAY 1000
#define MAX_PEERS 20 // max peers per info_hash?
#define MAX_NEIGHBORS 20


typedef struct {
  char *node_id; // corresponds to a node ID
  char *ip;      // used to send rpc via udp
  u_int16_t port;
} neighbor_t;

typedef struct {
  neighbor_t neighbors[MAX_NEIGHBORS]; // subset properties of a node
  char *id;
} node_t;


typedef struct {
  char *ip;
  char *id;
  uint16_t port;
} peer_t;


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
 */

// setting the same key will result in the same hash value
// check if current bucket is empty or not (check first element if properties are not set to 0)
// if current bucket is empty add then do nothing
void set(peer_t (*bucket)[MAX_SIZE_ARRAY], char *key) {
  uint32_t hash_key = hash(key);
  if (bucket[hash_key][hash_key].id != "") {
    size_t index = hash_key;
    while (bucket[hash_key])


  }
}

// coerce the type that you expect to retrieve from hashmap
void *get(char *data[MAX_SIZE_ARRAY], const char *key, const unsigned long data_size) {
  char *buf = malloc(data_size);
  uint32_t hash_key = hash(key);
  strcpy(buf, data[hash_key]);
  return buf;
}


int main() {
  peer_t (*peer_table)[MAX_PEERS]= malloc((sizeof(peer_t) * MAX_PEERS) * MAX_SIZE_ARRAY);
  free(peer_table);
  // base + MAX_PEERS * (sizeof(peer_t)*scale_index)
  // access row if j is included in the expression
  // base + (MAX_PEERS * (sizeof(peer_t)*scale_index) + j)
  return 0;
}