#ifndef PEERS
#define PEERS

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define HASH_OFFSET 2166136261u
#define PRIME 16777619u
#define MAX_PEER_BUCKETS 10
#define INITIAL_CAP 8

// determines the current state of the peer
// used for notifying other peers what the current peer
// wants to do
typedef enum {
  PASSIVE_ST = 0,
  LEECH_ST = 1,
  SEED_ST = 2,
} PEER_STATE;

typedef struct {
  char ip[INET_ADDRSTRLEN];
  uint16_t port;
  char job_id[16]; // determine what it is doing currently
  PEER_STATE state;
} peer_t;

// a peer bucket that contains an array of peers
typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  peer_t (*data)[INITIAL_CAP];
} peer_bucket_t;

void leech();
void seed();
void ping();

void init_peer_table(peer_bucket_t *(*unint_table)[MAX_PEER_BUCKETS]);
void set_peer(peer_bucket_t *(*table)[MAX_PEER_BUCKETS], const char *key,
              peer_t data);
void get_peer_bucket(peer_bucket_t *(*table)[MAX_PEER_BUCKETS], const char *key,
                     peer_bucket_t **peer_bucket_buf);
uint32_t hash(const char *input);
void print_peers_from_bucket(peer_bucket_t *bucket, const char *key);

peer_bucket_t *new_peer_array();
void resize_peer(peer_bucket_t *d_arr);
void push_peer(peer_bucket_t *d_arr, peer_t data);
void pop_peer(peer_bucket_t *d_arr, peer_t *peer_buf);

#endif
