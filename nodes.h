#ifndef NODES_H
#define NODES_H

#include "peer_table.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define MAX_NEIGHBORS 20
#define PROXIMITY_THRESHOLD 300 // any XOR metric less than or equal to 300

typedef struct {
  char *id;
  char *ip; // used to send rpc via udp
  uint16_t port;
  uint32_t distance;
} node_t;

typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  node_t (*data)[MAX_NEIGHBORS];
  peer_bucket_t *(*unint_table)[MAX_SIZE_ARRAY];
} node_array;

void bootstrap_neigbors(node_array *src, size_t n_count, node_array *dst);

/*
 * returns back an array of neighbors within close proximity to the file
 * object's info_hash in a sorted order using the distance as the entry
 * for the node
 *
 * eg:[{node.id Min}, ...,{node.id Max}]
 *
 * this function should be called recursively until,
 * - exhausted all neighbors for current node
 * - already at the closest proximity amongst exsting neighbors
 * - no neighbors
 */

void compare_hash(node_array *neighbors, size_t n_count, char *hash_info);

void XORdistance(char *hash_info, node_t *node);

/*
 * responsible for connecting to the neigbors that are within close proximity
 * and recursively calls compare_hash and itself until a base case is reached
 *
 *  bootstrap node -> compare_hash() -> get_peers()
 *
 *  after bootstrapping, compare_hash() is called to grab closest neighbors
 * which then get_peers is called to check if any of the neighboring nodes has
 * an entry for the current info_hash, this is used for propagating searches
 *
 *  if it does exist then only a subset of those neighbors can stop propagating
 * and return the peers whilst the others can keep searching.
 *
 *  if it does not exist, then compare_hash() is called again which then
 *  propagates the search to its neighboring nodes,and checks its node's entry
 * table if the info_hash exists
 *
 * eg:
 *  peer_bucket_t *peer_bucket_buf;
 *  get(table, info_hash,&peer_bucket_buf);
 *  if (peer_bucket_buf != NULL ) {
 *     return_peers(peer_bucket_buf)
 *  }
 *  else{
 *     compare_hash(&neighbors_ptr, info_hash);
 *     get_peers(&neighbors_ptr, info_hash);
 *   }
 *
 * on success, store the peers within the node's peer_table, using the
 * associated hash_info as the key to the bucket of peers
 */

void get_peers(node_array *sorted_neigbors, char *info_hash);
// node joins as a peer which sets its peer_status to passive by default
// which then propagates a join procedure call to each peers in the peer bucket
void join();

node_array *new_node_array();
void resize_node_array(node_array *d_arr);
void push_node(node_array *d_arr, node_t data);
void pop_node(node_array *d_arr, node_t *peer_buf);

#endif
