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
  node_t (*data)[INITIAL_CAP];
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

void compare_hash(node_array *neighbors, size_t n_count,
                  char info_hash[ID_SIZE]);

void XORdistance(char info_hash[ID_SIZE], node_t *node);

// calling get_peers on behalf of the initial caller:
// - pass around the ip and port of the original caller
// - make recursive call, handle the return of the results when it
// reaches a base case but need to modify the REPLY section to define
// what it should do, but then we also need every call or reply to carry
// some specific metadata specifically for GET_PEERS, eg: the callee
// node should know if wether or not if it should store the peers or
// reply back to its caller with the results as the payload, which would
// incur multiple recursive calls if it is empty which makes it a waste
// so passing the `absolute address` or address of the intitial caller
// would make things much more faster since every node would have a
// direct contact instead of backtracking and making things more
// complicated.
// flow: initial call to get_peers, the original node wil pass its own
// port and address as the abs_address, node1 -> neighbor1 receives the
// GET_PEER request from node1, checks its entry and if it doesn't exist
// it calls to get_peers again but passes the reply_to which is the caller's
// address instead of its own to the its own neighbor
//
// TODO: start a timer for the initiator (pseudo reliable transmission protocol)
// - fixed time for when datagram leaves the host
// - at timeout, resend datagram since the receiver will just throw away the
// request if
// - it fails internally.
//

int get_peers(int s_fd, node_t *node, node_array *sorted_neigbors,
               char info_hash[ID_SIZE], origin abs_address);
int join_peers(int s_fd, node_t *node, char info_hash[ID_SIZE]);

node_array *new_node_array();
void resize_node_array(node_array *d_arr);
void push_node(node_array *d_arr, node_t data);
void pop_node(node_array *d_arr, node_t *peer_buf);

#endif
