#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u
#define MAX_SIZE_ARRAY 1000
#define MAX_PEERS 20 // max peers per info_hash?
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
    char *ip;
    char *id;
    uint16_t port;
} peer_t;

typedef struct {
    size_t cap;
    size_t len;
    void *ptr; // can be used for any array type
} array;

void set(peer_t (*table)[MAX_PEERS], const char *key, peer_t data);
void get(const peer_t (*table)[MAX_PEERS], const char *key,peer_t *buf_ptr);
uint32_t hash(const char *input);

#endif
