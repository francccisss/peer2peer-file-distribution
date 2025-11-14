
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "../nodes.h"
#include "../remote_procedure.h"

typedef struct {
  char file_hash[ID_SIZE];
  char *date_created;
  struct node_t **known_nodes;
} file_info;

int main() {
  struct sockaddr_in src;

  node_t node = {
      .id = "14",
      .ip = "localhost",
      .port = 6969,
  };

  // what is being distributed
  file_info file = {.file_hash = "13", .date_created = "November 6 2025"};
  src.sin_family = AF_INET;
  src.sin_port = htons(node.port);
  src.sin_addr.s_addr = INADDR_ANY;

  int sfd = socket(AF_INET, SOCK_DGRAM, 0);
  int r = bind(sfd, (struct sockaddr *)&src, sizeof(src));

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

  get_peers(sfd, &node, neighboring_nodes, file.file_hash);

  init_peer_table(&node.peer_table);

  rpc_msg msg_buffer;
  while (1) {
    int r = recvfrom(sfd, &msg_buffer, sizeof(msg_buffer), 0, NULL, 0);
    if (r == -1) {
      perror("[ERROR] Socket bind");
      exit(-1);
    }
    recv_rpc(sfd, &node, file.file_hash, &msg_buffer, neighboring_nodes);
  }

  return 0;
}
