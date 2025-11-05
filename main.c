#include "nodes.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {

  const int N_COUNT = 3;
  node_t **BOOTSTRAP_NODES = malloc(sizeof(node_t));

  BOOTSTRAP_NODES[0] = malloc(sizeof(node_t) * N_COUNT);

  BOOTSTRAP_NODES[0][0] = (node_t){.id = "10", .ip = "", .port = 5432};
  BOOTSTRAP_NODES[0][1] = (node_t){.id = "N_COUNT", .ip = "", .port = 5000};
  BOOTSTRAP_NODES[0][2] = (node_t){.id = "5", .ip = "", .port = 6969};

  node_t node = {
      .id = "1",
      .ip = "localhost",
      .port = 6969,
  };

  node.neighbors = malloc(sizeof(node_t));
  bootstrap_neigbors(BOOTSTRAP_NODES, N_COUNT, node.neighbors);

  for (int i = 0; i < N_COUNT; i++) {
    printf("id =%s\n", node.neighbors[0][i].id);
  }

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

  char client_buf[1024];
  printf("[INFO]: Listening from port:6969\n");

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
