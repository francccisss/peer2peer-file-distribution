#include "nodes.h"
#include "peer_table.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

struct sockaddr sockaddr;
int main(int argc, char *argv[]) {

  bucket_t *table[MAX_SIZE_ARRAY];
  init_table(&table);
  node_t dht_node = {};
  int s_fd = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr sock;

  memset(&sock, 0, sizeof(sockaddr));

  int r = bind(s_fd, &sock, sizeof(sockaddr));

  while (true) {
    int r = listen(s_fd, 1);
    if (r < 0) {
      perror("[LIS ERROR]:");
			break;
    }
  }

  if (s_fd < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }

};
