#include "remote_procedure.h"
#include "nodes.h"
#include "peers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// TODO: set call and replies data payload hton
// TODO: set recv from ntoh
int call_rpc(RPC_TYPE rpc_type, void *buffer, size_t buf_sz,
             destination_host reply_to) {

  call_body c_body = {
      .type = htons(rpc_type),
  };

  memcpy(c_body.payload, buffer, sizeof(buf_sz));

  rpc_msg msg = {
      .correlation_id = "random value",
      .msg_type = htons(CALL),
      .body.cbody = c_body,
  };

  struct sockaddr_in dest;

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(reply_to.port);
  inet_pton(AF_INET, reply_to.ip, &(dest.sin_addr));

  long n_sent = sendto(reply_to.s_fd, &msg, sizeof(msg), 0,
                       (struct sockaddr *)&dest, sizeof(dest));
  if (n_sent == -1) {
    printf("[ERROR] client unable to send message");
    return n_sent;
  };
  return 0;
}

int reply_rpc(RPC_TYPE rpc_type, void *buffer, size_t buf_sz,
              destination_host reply_to, char *correlation_id,
              MSG_STATUS msg_status) {

  struct sockaddr_in dest;

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(reply_to.port);
  inet_pton(AF_INET, reply_to.ip, &(dest.sin_addr));

  reply_body r_body = {
      .type = htons(rpc_type),
      .status = htons(msg_status),
  };

  memcpy(r_body.payload, buffer, sizeof(buf_sz));

  // segment the payload
  rpc_msg msg = {
      .correlation_id = correlation_id,
      .segment_count = htonl(0),
      .segment_number = htonl(0),
      .msg_type = htons(CALL),
      .body.rbody = r_body,
  };

  long n_sent = sendto(reply_to.s_fd, &msg, sizeof(msg), 0,
                       (struct sockaddr *)&dest, sizeof(dest));
  if (n_sent == -1) {
    printf("[ERROR] client unable to send message");
    return n_sent;
  };
  return 0;
};

int recv_rpc(int s_fd, rpc_msg *call, node_array *sorted_neighbors,
             node_t *node) {
  destination_host reply_to = {.port = call->origin.port, .s_fd = s_fd};
  printf("[TEST]: sender destination ip=%s, port=%d\n", reply_to.ip,
         reply_to.port);

  switch (call->body.cbody.type) {
  case GET_PEERS: {
    printf("GET PEERS BROO\n");
    char *hash = (char *)call->body.cbody.payload;
    printf("[TEST] incoming hash %s\n", hash);

    if (strcmp(hash, "") < 0) {
      reply_rpc(call->body.cbody.type, NULL, 0, reply_to, call->correlation_id,
                ERR);
      break;
    }

    peer_bucket_t *peer_bucket_buf = malloc(sizeof(peer_t));
    if (peer_bucket_buf == NULL) {
      perror("[ERROR] malloc");
      exit(1);
    };
    get(&node->peer_table, hash, &peer_bucket_buf);
    if (peer_bucket_buf == NULL) {
      printf("[TEST]: table does not exist call get peers\n");
      // get_peers(s_fd, sorted_neighbors, hash);
      break;
    }
    break;
  }
  default:
    printf("no existing call type");
  }

  return 0;
};
