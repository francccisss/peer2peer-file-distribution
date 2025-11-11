#ifndef PEER_TABLE_H
#define PEER_TABLE_H

#include "peers.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u
#define MAX_SIZE_ARRAY 10
#define INITIAL_CAP 8

/*
 *  Functions and structs defined in should only be used by the DHT Nodes
 */

typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  peer_t (*data)[INITIAL_CAP];
} peer_bucket_t;

void init_table(peer_bucket_t *(*unint_table)[MAX_SIZE_ARRAY]);
void set(peer_bucket_t *(*table)[MAX_SIZE_ARRAY], const char *key, peer_t data);
void get(peer_bucket_t *(*table)[MAX_SIZE_ARRAY], const char *key,
         peer_bucket_t **peer_bucket_buf);
uint32_t hash(const char *input);

peer_bucket_t *new_array();
void resize(peer_bucket_t *d_arr);
void push(peer_bucket_t *d_arr, peer_t data);
void pop(peer_bucket_t *d_arr, peer_t *peer_buf);

#endif
