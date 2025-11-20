#include "remote_procedure.h"
#include "nodes.h"
#include "peers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define K_PEERS 8

int call_rpc(int s_fd, METHOD method, void *payload, size_t payload_sz,
             origin send_to, origin host) {

  call_body c_body = {
      .method = htons(method),
  };

  memcpy(c_body.payload, payload, payload_sz);

  rpc_msg msg = {
      .correlation_id = "random value",
      .msg_type = htons(CALL),
      .body.cbody = c_body,
      .origin.port = htons(host.port),
  };
  strcpy(msg.origin.ip, host.ip);

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
   * if payload_sz > sizeof(msg_buffer)struct : exceeds maximum segment size
   */

  memcpy(&r_body.payload, payload, payload_sz);

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

int recv_rpc(int s_fd, node_t *node, char file_hash[ID_SIZE],
             rpc_msg *msg_buffer, node_array *sorted_neighbors) {

  // grab the port and address of the caller that initiated the request defined
  // by call_rpc
  origin reply_to = {.port = ntohs(msg_buffer->origin.port)};

  strcpy(reply_to.ip, msg_buffer->origin.ip);

  printf("[TEST NETWORK]: sender destination ip=%s, port=%d\n", reply_to.ip,
         reply_to.port);

  msg_buffer->body.cbody.method = ntohs(msg_buffer->body.cbody.method);
  msg_buffer->segment_count = ntohl(msg_buffer->segment_count);
  msg_buffer->segment_number = ntohl(msg_buffer->segment_number);
  msg_buffer->msg_type = ntohs(msg_buffer->msg_type);

  if (msg_buffer->msg_type == CALL) {
    printf("[RPC TYPE]: CALL\n");
    switch (msg_buffer->body.cbody.method) {

    case GET_PEERS: {

      // In this case where GET_PEERS is called, the reply_to will contain the
      // address of the node that initiated the recursive call

      // since the get_peers goes through each neighbors, and sends an rpc call,
      // // then each neighbors can independently send their own peers, but the
      // initiator does not know how many peers to wait for until it can call
      // join_peers, the stop wait mechanism would work here, first send rpc to
      // the first closest neighbor, else if it does not return anything at the
      // alloted time, we move on to the next one until a list peers is returned
      // to the initiator, or when the number of peers < k peers then we do the
      // same until peers.len == k

      printf("[METHOD CALL]: GET_PEERS \n");
      char *hash = (char *)msg_buffer->body.cbody.payload;
      printf("[TEST] incoming hash %s\n", hash);

      if (strcmp(hash, "") < 0) {
        printf("[ERROR]: hash is empty");
        uint8_t buffer[1];
        buffer[0] = 0;
        // should i read the payload? or add a message to the struct
        reply_rpc(s_fd, msg_buffer->body.cbody.method, buffer, 1, reply_to,
                  msg_buffer->correlation_id, ERR);
        return 0;
      }

      peer_bucket_t *peer_bucket_buf = malloc(sizeof(peer_bucket_t));
      if (peer_bucket_buf == NULL) {
        perror("[ERROR]: malloc");
        return -1;
      };

      get_peer_bucket(&node->peer_table, hash, &peer_bucket_buf);

      // check if table exists
      if (peer_bucket_buf == NULL) {
        // only send the len which is the first byte of the buffer
        printf("[NOTIF]: no peer table entry\n");
        uint8_t buffer[1];
        buffer[0] = 0;
        int r = reply_rpc(s_fd, msg_buffer->body.cbody.method, buffer, 1,
                          reply_to, msg_buffer->correlation_id, OK);
        if (r < 0) {
          printf("[ERROR]: Unable to send datagram back to caller\n");
        }
        free(peer_bucket_buf);
        return r;
      }

      if (sorted_neighbors->len == 0 && peer_bucket_buf->len == 0) {
        // reply back to the caller with empty peer and 0 len
        // only send the len which is the first byte of the buffer
        printf("[NOTIF]: no peer table entry\n");
        uint8_t buffer[1];
        buffer[0] = 0;
        int r = reply_rpc(s_fd, msg_buffer->body.cbody.method, buffer, 1,
                          reply_to, msg_buffer->correlation_id, OK);
        if (r < 0) {
          printf("[ERROR]: Unable to send datagram back to caller\n");
        }
        free(peer_bucket_buf);
        return r;
      }

      if (peer_bucket_buf->len == 0 && sorted_neighbors->len > 0) {
        get_peers(s_fd, node, sorted_neighbors, file_hash, reply_to);
        printf("[NOTIF]: peer bucket is empty, search neighbors.\n");
        return 0;
      }

      printf("[TEST]: peer bucket len to be sent =%ld\n", peer_bucket_buf->len);

      // replies back to the requester with the
      // call result expected from a method
      uint8_t buffer[MAX_PAYLOAD_SIZE];
      buffer[0] = peer_bucket_buf->len; // first byte stores length
                                        // populate data into the buffer
      memcpy(buffer + 1, peer_bucket_buf->data,
             sizeof(peer_t) * peer_bucket_buf->len);

      reply_rpc(s_fd, msg_buffer->body.cbody.method, buffer,
                sizeof(peer_t) * peer_bucket_buf->len + 1, reply_to,
                msg_buffer->correlation_id, OK);
      free(peer_bucket_buf);
      return 0;
    }
    case JOIN: {
      // when a node/peer receives a JOIN call from caller
      peer_t new_peer = {
          .port = reply_to.port,
          .state = PASSIVE_ST,
      };
      strcpy(new_peer.ip, reply_to.ip);
      int r = reply_rpc(s_fd, JOIN, NULL, 0, reply_to,
                        msg_buffer->correlation_id, OK);

      if (r < 0) {
        printf("[ERROR REPLY]: Unable to send UDP datagram to "
               "destination\nHost unreachable.");
        return 1;
      }
      set_peer(&node->peer_table, file_hash, new_peer);
      break;
    }

    default:
      printf("no existing call type");
      return 0;
    }
  } else {

    printf("[RPC TYPE]: REPLY\n");
    switch (msg_buffer->body.rbody.method) {
    case GET_PEERS: {

      // make this into a reusable function that takes in an input
      if (msg_buffer->body.rbody.status != OK) {
        printf("Unable to retrieve peers from nodes\n");
        return -1;
      };

      peer_t p_buf[MAX_PEER_BUCKETS];
      uint8_t len = msg_buffer->body.rbody.payload[0];

      // first check if there are peers with the payload, the first byte of the
      // payload in GET PEERS will always be the length of the peer array
      printf("[NOTIF]: peer array length=%d\n", len);
      // Guard to close the function when incoming peer array is empty
      if (len == 0) {
        printf("[NOTIF]: returned with 0 peers in the bucket\n");
        return 0;
      };

      // get_peers call will propagate request to all neighbors starting from
      // the closest. the caller will wait for independent incoming peer arrays
      // from unknown nodes that responds to the request. the node will only
      // join if atleast len == PEERS

      bool timeout = true; // need to implement stop wait mechanism
      peer_bucket_t *peer_bucket_buf = malloc(sizeof(peer_bucket_t));
      if (peer_bucket_buf == NULL) {
        perror("[ERROR] malloc");
        return -1;
      };

      get_peer_bucket(&node->peer_table, file_hash, &peer_bucket_buf);

      if (peer_bucket_buf == NULL) {
        printf("[ERROR] empty bucket, could be an unintialized peer_table\n");
        return -1;
      };

      // timeout would only work if this function is called again.
      // eg: new datagram arrives -> is timedout?
      // then that means we are hoping for a new rpc reply to get_peers
      // before we can even call join_peers
      // what if the last neighbor returned the peers < K_PEERS while timeout
      // never occured? then join peers is never called

      memcpy(&p_buf, msg_buffer->body.rbody.payload + 1, sizeof(peer_t) * len);
      for (int i = 0; i < len; i++) {
        set_peer(&node->peer_table, file_hash, p_buf[i]);
      };
      for (int i = 0; i < len; i++) {
        printf("[TEST CASTED BUF]: ip=%s, port=%d\n", p_buf[i].ip,
               p_buf[i].port);
      };
      free(peer_bucket_buf);
      return 0;
    };
    case JOIN: {

      // need to be smart about status,
      // does the status describe that if the message was sent or not?
      // or if it has been rejected by the receiving host?
      // or it has been received, and has been accepted but an error occured
      // that does not concern the sendiing host?

      // OR using Reliable data transfer by implementing a timer mechanism that
      // waits for a response from the receiver, if it has been received or not,
      // so if not we resend after times_sent >= maximum_retries
      // if exhausted then fail
      //
      // - using this method we dont need to keep states nor does the receiver
      // need to let us know that the request was a fail the status can then
      // only represent if it was reject by the host in based on the arguments
      // passed by the sender, eg: wrong hash file, or hash file does not exist
      // in the table of the receiver

      if (msg_buffer->body.rbody.status != OK) {
        printf("Unable to retrieve peers from nodes\n");
        return -1;
      };
      break;
    }
    default:
      break;
    };
  }

  return 0;
};
