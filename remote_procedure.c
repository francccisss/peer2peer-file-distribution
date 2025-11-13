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

int call_rpc(int s_fd, METHOD method, void *payload, size_t payload_sz,
             origin send_to, node_t *node) {

  call_body c_body = {
      .method = htons(method),
  };

  memcpy(c_body.payload, payload, payload_sz);
  rpc_msg msg = {
      .correlation_id = "random value",
      .msg_type = htons(CALL),
      .body.cbody = c_body,
      .origin.port = htons(node->port),
  };
  strcpy(msg.origin.ip, node->ip);

  struct sockaddr_in dest;

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(send_to.port);
  inet_pton(AF_INET, send_to.ip, &(dest.sin_addr));

  long n_sent = sendto(s_fd, &msg, sizeof(msg), 0, (struct sockaddr *)&dest,
                       sizeof(dest));
  if (n_sent == -1) {
    printf("[ERROR] client unable to send message");
    return n_sent;
  };
  return 0;
}

int reply_rpc(int s_fd, METHOD method, void *payload, size_t payload_sz,
              origin reply_to, char correlation_id[CORRELATAION_ID_SIZE],
              MSG_STATUS msg_status) {

  struct sockaddr_in dest;

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(reply_to.port);
  inet_pton(AF_INET, reply_to.ip, &(dest.sin_addr));

  reply_body r_body = {
      .method = htons(method),
      .status = htons(msg_status),
  };

  /*
   * segment the payload else ip fragmentation since udp does not
   * automatically segment the size
   *
   * if payload_sz > sizeof(rpc_msg)struct : exceeds maximum segment size
   *
   *
   *
   */

  memcpy(r_body.payload, payload, sizeof(payload_sz));

  rpc_msg msg = {
      .segment_count = htonl(0),
      .segment_number = htonl(0),
      .msg_type = htons(REPLY),
      .body.rbody = r_body,
  };
  strcpy(msg.correlation_id, correlation_id);

  long n_sent = sendto(s_fd, &msg, sizeof(msg), 0, (struct sockaddr *)&dest,
                       sizeof(dest));
  if (n_sent == -1) {
    printf("[ERROR] client unable to send message");
    return n_sent;
  };
  return 0;
};

int recv_rpc(int s_fd, node_t *node, rpc_msg *rpc_msg,
             node_array *sorted_neighbors) {

  // grab the port and address of the sender defined by call_rpc
  origin reply_to = {.port = ntohs(rpc_msg->origin.port)};

  strcpy(reply_to.ip, rpc_msg->origin.ip);

  printf("[TEST NETWORK]: sender destination ip=%s, port=%d\n", reply_to.ip,
         reply_to.port);

  rpc_msg->body.cbody.method = ntohs(rpc_msg->body.cbody.method);
  rpc_msg->segment_count = ntohl(rpc_msg->segment_count);
  rpc_msg->segment_number = ntohl(rpc_msg->segment_number);
  rpc_msg->msg_type = ntohs(rpc_msg->msg_type);

  if (rpc_msg->msg_type == CALL) {

    printf("[RPC TYPE]: CALL\n");
    switch (rpc_msg->body.cbody.method) {
    case GET_PEERS: {
      printf("[METHOD CALL]: GET_PEERS \n");
      char *hash = (char *)rpc_msg->body.cbody.payload;
      printf("[TEST] incoming hash %s\n", hash);

      if (strcmp(hash, "") < 0) {
        printf("[ERROR]: hash is empty");
        reply_rpc(s_fd, rpc_msg->body.cbody.method, NULL, 0, reply_to,
                  rpc_msg->correlation_id, ERR);
        break;
      }

      peer_bucket_t *peer_bucket_buf = malloc(sizeof(peer_t));
      if (peer_bucket_buf == NULL) {
        perror("[ERROR]: malloc");
        exit(1);
      };

      get(&node->peer_table, hash, &peer_bucket_buf);
      if (peer_bucket_buf == NULL) {
        printf("[TEST]: table does not exist call get peers\n");
        get_peers(s_fd, node, sorted_neighbors, hash);
        break;
      }

      printf("[TEST]: peer bucket len to be sent =%ld\n", peer_bucket_buf->len);
      for (int i = 0; i < peer_bucket_buf->len; i++) {
        peer_t cp = (*peer_bucket_buf->data)[i];
        printf("[TEST]: ip=%s, port=%d\n", cp.ip, cp.port);
      }

      // replies back to the requester with the
      // call result expected from a method
      reply_rpc(s_fd, rpc_msg->body.cbody.method, peer_bucket_buf->data,
                sizeof(peer_t) * peer_bucket_buf->len, reply_to,
                rpc_msg->correlation_id, OK);
      return 0;
    }
    default:
      printf("no existing call type");
      return 0;
    }
  } else {

    printf("[RPC TYPE]: REPLY\n");
    switch (rpc_msg->body.rbody.method) {
    case GET_PEERS:
      // make this into a reusable function that takes in an input
      if (rpc_msg->body.rbody.status != OK) {
        printf("Unable to retrieve peers from nodes\n");
        return -1;
      }
      printf("AYAYAYA\n");
      break;
    default:
      break;
    };
  }

  return 0;
};
