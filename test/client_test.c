
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <bits/types/struct_timeval.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "../nodes.h"
#include "../remote_procedure.h"

typedef struct {
  char file_hash[ID_SIZE];
  char *date_created;
  struct node_t **known_nodes;
} file_info;

int main(int argc, char **argv) {
  struct sockaddr_in src;

  if (argc < 2) {
    printf("[ERROR] port not defined\n");
    return -1;
  }
  int host_port = atoi(argv[1]);
  int neighbor_port = atoi(argv[2]);

  printf("host_port=%d\n", host_port);
  node_t node = {
      .id = "14",
      .ip = "localhost",
      .port = host_port,
  };

  // what is being distributed
  file_info file = {.file_hash = "13", .date_created = "November 6 2025"};
  src.sin_family = AF_INET;
  src.sin_port = htons(node.port);
  src.sin_addr.s_addr = INADDR_ANY;

  int s_fd = socket(AF_INET, SOCK_DGRAM, 0);
  int r = bind(s_fd, (struct sockaddr *)&src, sizeof(src));

  if (r != 0) {
    perror("[ERROR] Socket bind");
    exit(-1);
  }

  node_array *neighboring_nodes = new_node_array();

  // for now for every file, a new process is created instead
  // of reusing the same process
  // compare_hash(node.neighbors, N_COUNT, file.file_hash, closest_neighbors);

  push_node(neighboring_nodes,
            (node_t){.distance = 1, .ip = "localhost", .port = neighbor_port});

  // assigning the absolute address of the caller
  origin absolute_address = {.port = node.port};
  strcpy(absolute_address.ip, node.ip);
  init_peer_table(&node.peer_table);

  struct sockaddr_in get_sock;
  socklen_t len = sizeof(get_sock);
  int gr = getsockname(s_fd, (struct sockaddr *)&get_sock, &len);
  if (gr != 0) {
    perror("[ERROR]: getsockname");
    exit(gr);
  }

  printf("[TEST]: other nodes area reading big endianess of port -> %d\n",
         get_sock.sin_port);
  printf("[TEST]: getsock port: flipped %d\n", ntohs(get_sock.sin_port));
  get_peers(s_fd, &node, neighboring_nodes, file.file_hash, absolute_address,
            absolute_address);

  bool wait = true;
  rpc_msg msg_buffer;
  fd_set rfd;
  struct timeval t;
  uint8_t max_time = 4;

  // NOTICE: this only works for a single file hash
  peer_bucket_t *peer_bucket_buf = NULL;
  get_peer_bucket(&node.peer_table, file.file_hash, &peer_bucket_buf);
  if (peer_bucket_buf == NULL) {
    printf("[ERROR]: bucket was unintialized\n");
  }

  while (1) {

    if (wait) {
      FD_ZERO(&rfd);
      FD_SET(s_fd, &rfd);
      t.tv_sec = max_time;
      t.tv_usec = 0;

      int retval = select(s_fd + 1, &rfd, NULL, NULL, &t);
      if (retval < 0) {
        perror("[ERROR] select");
        exit(retval);
      };
      if (retval == 0) {
        printf("[INFO]: polling timed out\n");
        printf("[INFO]: joining %ld peers\n", peer_bucket_buf->len);
        int r = join_peers(s_fd, &node, file.file_hash);
        if (r != 0) {
          printf("[INFO]: there are no peers to join in hash=%s\n",
                 file.file_hash);
          printf("[INFO]: Exiting...\n");
          exit(r);
        }
        continue;
      }
    }
    int r = recvfrom(s_fd, &msg_buffer, sizeof(msg_buffer), 0, NULL, 0);
    if (r == -1) {
      perror("[ERROR] Socket bind");
      exit(-1);
    }
    printf("request received\n");
    recv_rpc(s_fd, &node, file.file_hash, &msg_buffer, neighboring_nodes,
             &wait);
  }

  return 0;
}
