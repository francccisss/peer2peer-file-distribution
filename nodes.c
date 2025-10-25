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
void set(peer_t (*table)[MAX_PEERS], const char *key, const peer_t data) {
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
        while (j < MAX_PEERS) {
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

// coerce the type that you expect to retrieve from hashmap
void *get(char *data[MAX_SIZE_ARRAY], const char *key, const unsigned long data_size) {
  char *buf = malloc(data_size);
  uint32_t hash_key = hash(key);
  strcpy(buf, data[hash_key]);
  return buf;
}


int main() {
  peer_t (*peer_table)[MAX_PEERS] = malloc((sizeof(peer_t) * MAX_PEERS) * MAX_SIZE_ARRAY);
  // peer_t peer_table[MAX_SIZE_ARRAY][MAX_PEERS];
  if (peer_table == NULL) {
    perror("malloc error");
    return 1;
  };
  set(peer_table,"franz",(peer_t){.ip = "192",.id="francois",.port=3000});
  free(peer_table);
  // base + MAX_PEERS * (sizeof(peer_t)*scale_index)
  // access row if j is included in the expression
  // base + (MAX_PEERS * (sizeof(peer_t)*scale_index) + j)
  return 0;
}