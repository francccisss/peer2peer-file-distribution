#ifndef NODES_H
#define NODES_H

#include "peers.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define PROXIMITY_THRESHOLD 300 // any XOR metric less than or equal to 300
#define ID_SIZE 36

typedef struct {
  char id[ID_SIZE];
  char ip[INET_ADDRSTRLEN];
  uint16_t port;
  uint32_t distance;
  peer_bucket_t *peer_table[MAX_PEER_BUCKETS];
} node_t;

// used as neighbors of the current node
typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  node_t (*data)[INITIAL_CAP]; // pointer to a block of node_t * BLOCK_SIZE
} node_array;

typedef struct {
  uint16_t port;
  char ip[INET_ADDRSTRLEN];
} origin;

void bootstrap_neigbors(node_array *src, size_t n_count, node_array *dst);

/*
 * returns back an array of neighbors within close proximity to the f0ile
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

// a fild descriptor returned by call socket()
typedef int SOCKET_FILE_DESCRIPTOR;

void compare_hash(node_array *neighbors, size_t n_count,
                  char info_hash[ID_SIZE]);

void XORdistance(char info_hash[ID_SIZE], node_t *node);

// calling get_peers on behalf of the initial caller:
// - `s_fd` socket file descriptor
// - `node` extract the `origin` of the current node to be used
// as the sender information through the `rpc_call`
// - `sorted_neighbors` the array of the neighboring nodes of the current `node`
// will iterate to send an `rpc_call` to `get_peers`
// - `info_hash` well.
// - `src_addr` indicates the src of the caller that initated the `get_peers`
// - `reply_to` is the origin of the node that called this current node to
// `get_peers`. not to be confused with the `src_addr` which is the original
// caller that initated the `GET_PEERS` chain call.
// The existence of `reply_to` is to check if the current node is a neighbor to
// the node that called it, and if so it does not send a `GET_PEERS` to that
// neighbor to prevent an infinite call loop between the 2 nodes.
//
//
// How Inifinite call occur:
// node_caller calls to its neighbor for get_peers, neighbor's peer table is
// empty and sends a get_peers to its own list of neighbors, it sees that the
// sender (node_caller) is one of it's neighbor, it sends an rpc call back to it
// to get peers, but since the node_caller also has no peers, it then checks its
// own neighbor, and the process repeats infinitely
int get_peers(SOCKET_FILE_DESCRIPTOR s_fd, node_t *node,
              node_array *sorted_neigbors, char info_hash[ID_SIZE],
              origin src_addr, origin reply_to);
int join_peers(int s_fd, node_t *node, char info_hash[ID_SIZE]);

node_array *new_node_array();
void resize_node_array(node_array *d_arr);
void push_node(node_array *d_arr, node_t data);
void pop_node(node_array *d_arr, node_t *peer_buf);

#endif
