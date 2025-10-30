#ifndef PEER_TABLE_H
#define PEER_TABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u
#define MAX_SIZE_ARRAY 10
#define INITIAL_CAP 8
#define MAX_NEIGHBORS 20

/*
 *  Functions and structs defined in should only be used by the DHT Nodes
 */

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
  char ip[15];
  char id[46];
  uint16_t port;
} peer_t;

typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  peer_t (*data)[INITIAL_CAP];
} bucket_t;

void set(bucket_t *(*table)[MAX_SIZE_ARRAY], const char *key, peer_t data);
void get(bucket_t *(*table)[MAX_SIZE_ARRAY], const char *key,
         bucket_t **bucket_buf);
uint32_t hash(const char *input);

bucket_t *new_array();
void resize(bucket_t *d_arr);
void push(bucket_t *d_arr, peer_t data);
void pop(bucket_t *d_arr, peer_t *peer_buf);

#endif
