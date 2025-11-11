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

int call_rpc(CALL_TYPE call_type, void *buffer, size_t buf_sz,
             destination_host d_host) {

  call_body c_body = {
      .type = call_type,
  };

  memcpy(c_body.payload, buffer, sizeof(buf_sz));

  rpc_msg msg = {
      .correlation_id = "random value",
      .segment_count = 0,
      .segment_number = 0,
      .msg_type = CALL,
      .body.cbody = c_body,
  };

  struct sockaddr_in dest;

  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(d_host.port);
  inet_pton(AF_INET, d_host.ip, &(dest.sin_addr));

  long n_sent = sendto(d_host.s_fd, &msg, sizeof(msg), 0,
                       (struct sockaddr *)&dest, sizeof(dest));
  if (n_sent == -1) {
    printf("[ERROR] client unable to send message");
    return n_sent;
  };
  return 0;
}

int recv_rpc(int s_fd, rpc_msg *call, node_array *sorted_neighbors,
             node_t *node) {
  switch (call->body.rbody.type) {
  case GET_PEERS: {
    printf("GET PEERS BROO\n");
    char *hash = (char *)call->body.cbody.payload;
    peer_bucket_t *peer_bucket_buf = malloc(sizeof(peer_t));
    if (peer_bucket_buf == NULL) {
      perror("[ERROR] malloc");
      exit(1);
    };
    get(node->peer_table, hash, &peer_bucket_buf);
    // if (peer_bucket_buf == NULL) {
    //   get_peers(s_fd, sorted_neighbors, hash);
    // }
    break;
  }
  default:
    printf("no existing call type");
  }

  return 0;
};
