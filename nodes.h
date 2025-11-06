#ifndef NODES_H
#define NODES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define MAX_NEIGHBORS 20
#define PROXIMITY_THRESHOLD 300 // any XOR metric less than or equal to 300

typedef struct {
  char *id;
  char *ip; // used to send rpc via udp
  uint16_t port;
} node_t;

typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  node_t (*data)[MAX_NEIGHBORS];
} node_array;

node_array *new_node_array();
void resize_node_array(node_array *d_arr);
void push_node(node_array *d_arr, node_t data);
void pop_node(node_array *d_arr, node_t *peer_buf);

/*
 * returns back an array of neighbors within close proximity to the file
 * object's info_hash this
 *
 * this function should be called recursively until,
 * - exhausted all neighbors for current node
 * - already at the closest proximity amongst exsting neighbors
 * - no neighbors
 */

void compare_hash(node_array *neighbors, size_t n_count, char *hash_info,
                  node_array *closest_neigbors);

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
 *  bucket_t *bucket_buf;
 *  get(table, info_hash,&bucket_buf);
 *  if (bucket_buf != NULL ) {
 *     return_peers(bucket_buf)
 *  }
 *  else{
 *     compare_hash(&neighbors_ptr, info_hash);
 *     get_peers(&neighbors_ptr, info_hash);
 *   }
 *
 */
void get_peers(node_t **closest_neigbors, char *info_hash);

void bootstrap_neigbors(node_array *boot_neighbors, size_t n_count,
                        node_array *node_neighbors);
#endif
