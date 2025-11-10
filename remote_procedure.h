#ifndef REMOTE_PROCEDURE
#define REMOTE_PROCEDURE

#include <stddef.h>
#include <stdint.h>
#define MAX_BUFFER_SIZE 1024

typedef enum {
  GET_PEERS,
  JOIN,
  LEECH,
  SEED,
} CALL_TYPE;

typedef enum {
  ERR = -1,
  OK,
} STATUS;
/*
 * call_type is read by the receiver, to issue the appropriate routine to
 * execute based on the call type of the sender.
 *
 * each host/node will have lookup table or a call table where it processes the
 * datagram from a host, extracts the call_type and uses that as an entry
 * to the corresponding routine pointer in that table
 *
 * payload is the data iteself that will be populated by the
 *
 */
typedef struct {
  CALL_TYPE type;
  uint8_t payload[MAX_BUFFER_SIZE];
} call_body;

/*
 *
 *
 * eg: call is GET_PEERS, receiver processes the request, returns the result
 * local host checks the status, and the type
 *
 * if STATUS > 0
 *  check the type
 *  shape the type into the appropriate expected type
 *  since the payload is just an array of bytes
 *  and recvfrom populates the (void *buffer) arg
 *
 */

typedef struct {
  CALL_TYPE type;
  uint8_t payload[MAX_BUFFER_SIZE];
  STATUS status;
} reply_body;

typedef enum {
  REPLY,
  CALL,
} MSG_TYPE;

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
 * segments of the `body.payload`, in which the segment_count can tell the receiving host how
 * many datagrams from the correlation_id are we waiting for before we can
 * process the whole thing, and as well as using the the segment number to
 * assimilate and assemble the rpc messages
 */

typedef struct {
  char correlation_id[36];
  uint16_t segment_number;
  size_t segment_count;
  MSG_TYPE type;
  union body {
    call_body cbody;
    reply_body rbody;
  };
} rpc_msg;

/*
 * params buffer wil lbe used to populate unknown data type
 * which will be returned to the caller and assume it's shape
 *
 * as per RFC definition, the send function needs to wait for a reply
 */

int send(void *buffer);

#endif
