#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int main() {

  struct sockaddr_in client, server;

  memset(&client, 0, sizeof client);
  memset(&server, 0, sizeof server);
  int s_fd = socket(AF_INET, SOCK_DGRAM, 0);
  client.sin_family = AF_INET;
  client.sin_addr.s_addr = INADDR_ANY;

  server.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

  printf("[TEST]: pton addr =%d\n", server.sin_addr.s_addr);

  return 0;
  if (bind(s_fd, (struct sockaddr *)&client, sizeof client) < 0) {
    perror("[SOCK ERROR]");
    return -1;
  }

  int BUFF_SIZE = 64;
  char buf[64] = "wtf is going on bruh";

  sendto(s_fd, &buf, BUFF_SIZE, 0, (struct sockaddr *)&client, sizeof(client));

  return 0;
}
