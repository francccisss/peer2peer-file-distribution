#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  struct sockaddr_in server;

  int s_fd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&server, 0, sizeof(struct sockaddr));

  server.sin_family = AF_INET;
  server.sin_port = htons(6969);
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(s_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }

  if (listen(s_fd, 1) < 0) {
    perror("[LIS ERROR]:");
    exit(1);
  }
  printf("[INFO]: Listening from port:6969\n");
  struct sockaddr_storage storage;
  socklen_t stor_sze = sizeof storage;
  int c_sd = 0;
  while (c_sd >= 0) {
    printf("[INFO]: Accepting connections\n");
    c_sd = accept(s_fd, (struct sockaddr *)&storage, &stor_sze);
  }

  if (s_fd < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }
};
