#ifndef PEERS
#define PEERS

#include <stdint.h>

// determines the current state of the peer
// used for notifying other peers what the current peer
// wants to do
typedef enum {

  PASSIVE = 0,
  LEECH = 1,
  SEED = 2,

} PEER_STATE;

typedef struct {
  char ip[15];
  uint16_t port;
  char *job_id; // determine what it is doing currently
  PEER_STATE state;
} peer_t;

void leech();
void seed();
void ping();

#endif
