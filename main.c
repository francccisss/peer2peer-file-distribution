#include "nodes.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

typedef struct {
  char *file_hash;
  char *date_created;
  struct node_t **known_nodes;
} file_info;

int main(int argc, char *argv[]) {

  node_array *BOOTSTRAP_NODES = new_node_array();

  push_node(BOOTSTRAP_NODES, (node_t){.id = "55", .ip = "", .port = 5432});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "10", .ip = "", .port = 4292});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "30", .ip = "", .port = 3000});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "29", .ip = "", .port = 7010});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "24", .ip = "", .port = 7002});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "40", .ip = "", .port = 5060});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "49", .ip = "", .port = 5030});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "19", .ip = "", .port = 6939});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "11", .ip = "", .port = 6969});

  node_t node = {
      .id = "12",
      .ip = "localhost",
      .port = 6942,
  };

  // what is being distributed
  file_info file = {.file_hash = "13", .date_created = "November 6 2025"};

  node_array *neighboring_nodes = new_node_array();

  // for now for every file, a new process is created instead
  // of reusing the same process
  // compare_hash(node.neighbors, N_COUNT, file.file_hash, closest_neighbors);
  bootstrap_neigbors(BOOTSTRAP_NODES, BOOTSTRAP_NODES->len, neighboring_nodes);

  for (int i = 0; i < BOOTSTRAP_NODES->len; i++) {
    printf("id =%s\n", (*neighboring_nodes->data)[i].id);
  }

  compare_hash(neighboring_nodes, neighboring_nodes->len, file.file_hash);
  return 0;

  struct sockaddr_in server;
  const int s_fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (s_fd < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }

  memset(&server, 0, sizeof(struct sockaddr));

  server.sin_family = AF_INET;
  server.sin_port = htons(node.port);
  server.sin_addr.s_addr = INADDR_ANY;

  inet_pton(AF_INET, node.ip, &(server.sin_addr));

  if (bind(s_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }

 uint8_t client_buf[1024];
  printf("[INFO]: Listening from port:6969\n");

  // main loop to listen for incoming datagrams from nodes and peers
  // to distinguish different operations from incoming datagrams,
  // we need to implement a basic RPC protocol mechanism
  // such that every node and peer are able to understand each other

  while (1) {
    uint32_t bytes_read = recvfrom(s_fd, client_buf, 1024, 0, NULL, NULL);
    printf("[TEST]: bytes read =%d", bytes_read);
    if (bytes_read == -1) {
      perror("[RECV FROM ERR]");
      break;
    }
    break;
  }
  printf("[TEST]: text from client =%s", client_buf);
};
