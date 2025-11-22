
#include <arpa/inet.h>
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
  int arg_port = atoi(argv[1]);

  printf("arg_port=%d\n", arg_port);
  node_t node = {
      .id = "14",
      .ip = "localhost",
      .port = arg_port,
  };

  // what is being distributed
  file_info file = {.file_hash = "13", .date_created = "November 6 2025"};
  src.sin_family = AF_INET;
  src.sin_port = htons(node.port);
  src.sin_addr.s_addr = INADDR_ANY;

  int s_fd = socket(AF_INET, SOCK_DGRAM, 0);
  int r = bind(s_fd, (struct sockaddr *)&src, sizeof(src));

  if (r == -1) {
    perror("[ERROR] Socket bind");
    exit(-1);
  }

  node_array *neighboring_nodes = new_node_array();

  // for now for every file, a new process is created instead
  // of reusing the same process
  // compare_hash(node.neighbors, N_COUNT, file.file_hash, closest_neighbors);

  push_node(neighboring_nodes,
            (node_t){.distance = 1, .ip = "localhost", .port = 3000});

  origin absolute_address = {.port = node.port};
  strcpy(absolute_address.ip, node.ip);
  init_peer_table(&node.peer_table);

  get_peers(s_fd, &node, neighboring_nodes, file.file_hash, absolute_address);

  bool wait = true;
  rpc_msg msg_buffer;
  fd_set rfd;
  struct timeval t;
  uint8_t max_time = 1;

  int retval;
  while (1) {
    FD_ZERO(&rfd);
    FD_SET(s_fd, &rfd);
    t.tv_sec = max_time;
    t.tv_usec = 0;
    retval = select(s_fd + 1, &rfd, NULL, NULL, &t);

    // when receiver receives a call to get_peers, this will also call join
    // peers which should not happen, join peers should only happen on the newly
    // connecting node
    if (wait) {
      if (retval < 0) {
        perror("[ERROR] select");
        exit(retval);
      };
      if (retval == 0) {
        // still need to read the buffer from network device
        int r = recvfrom(s_fd, &msg_buffer, sizeof(msg_buffer), 0, NULL, 0);
        if (r == -1) {
          perror("[ERROR] Socket bind");
          exit(-1);
        }
        // still need to read the buffer from network device
        printf("polling Timed out\n");
        join_peers(s_fd, &node, file.file_hash);
        wait = false;
        continue;
      };

      printf("request received\n");
    };

    int r = recvfrom(s_fd, &msg_buffer, sizeof(msg_buffer), 0, NULL, 0);
    if (r == -1) {
      perror("[ERROR] Socket bind");
      exit(-1);
    }
    recv_rpc(s_fd, &node, file.file_hash, &msg_buffer, neighboring_nodes,
             &wait);
  }

  return 0;
}
