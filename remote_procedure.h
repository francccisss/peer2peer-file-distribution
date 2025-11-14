#ifndef REMOTE_PROCEDURE
#define REMOTE_PROCEDURE

#include "nodes.h"
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#define MAX_PAYLOAD_SIZE 1024

#define CORRELATAION_ID_SIZE 16

/*
 * method is read by the receiver, to issue the appropriate routine to
 * execute based on the call type of the sender.
 *
 * each host/node will have lookup table or a call table where it processes the
 * datagram from a host, extracts the method and uses that as an entry
 * to the corresponding routine pointer in that table
 *
 */
typedef enum {
  GET_PEERS,
  JOIN,
  LEECH,
  SEED,
} METHOD;

typedef enum {
  ERR = -1,
  OK,
} MSG_STATUS;

typedef struct {
  uint8_t payload[MAX_PAYLOAD_SIZE];
  METHOD method;
} call_body;

/*
 *
 *
 * eg: call is GET_PEERS, receiver processes the request, returns the result
 * local host checks the status, and the type
 *
 * if MSG_STATUS > 0
 *  check the type
 *  shape the type into the appropriate expected type
 *  since the payload is just an array of bytes
 *  and recvfrom populates the (void *payload) arg
 *
 */

typedef struct {
  METHOD method;
  MSG_STATUS status;
  uint8_t payload[MAX_PAYLOAD_SIZE];
} reply_body;

typedef enum {
  REPLY,
  CALL,
} MSG_TYPE;

typedef struct {
  uint16_t port;
  char ip[INET_ADDRSTRLEN];
} origin;

/*
 *
 * the correlation_id is used to distinguish the caller of the send() function,
 * or more accurately will be used for demultiplexing the incoming datagrams
 * from another host to the specific thread that called the send() routine
 *
 * segment_count is essential to determine the size of the rpc_msg that the a
 * host will need to receive if the payload has an excessive size eg: file
 * downloads .mp4 .pdf etc...
 *
 * FILE MESSAGES
 * if the file exceeds some N amount, the rpc protocol will have to create
 * segments of the `body.payload`, in which the segment_count can tell the
 * receiving host how many datagrams from the correlation_id are we waiting for
 * before we can process the whole thing, and as well as using the the segment
 * number to assimilate and assemble the rpc messages
 */

typedef struct {
  size_t segment_count;
  MSG_TYPE msg_type;
  origin origin;
  uint16_t segment_number;
  union {
    call_body cbody;
    reply_body rbody;
  } body;
  char correlation_id[CORRELATAION_ID_SIZE];
} rpc_msg;

/*
 * Calls dont need to take up that much memory to be used in arg, unless it
 * is a response/reply from the call, the arg is used to pass in additional
 * information.
 *
 * The rpc_msg needs to tell the receiver where to send the
 * payload back to, the node which is the current node process, stores the host
 * ip and port.
 *
 */
int call_rpc(int s_fd, METHOD method, void *arg, size_t payload_sz,
             origin d_host, node_t *node);

int reply_rpc(int s_fd, METHOD method, void *payload, size_t payload_sz,
              origin d_host, char correlation_id[CORRELATAION_ID_SIZE],
              MSG_STATUS msg_status);

int recv_rpc(int sf_d, node_t *node, char file_hash[ID_SIZE], rpc_msg *reply,
             node_array *sorted_neighbors);
#endif
