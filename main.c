#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  struct sockaddr_in server;

  const int s_fd = socket(AF_INET, SOCK_DGRAM, 0);

  if (s_fd < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }

  memset(&server, 0, sizeof(struct sockaddr));

  server.sin_family = AF_INET;
  server.sin_port = htons(6969);
  server.sin_addr.s_addr = INADDR_ANY;

  if (bind(s_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("[SOCKET ERROR]:");
    exit(1);
  }


  char client_buf[1024];
  printf("[INFO]: Listening from port:6969\n");
  while (true) {
    uint32_t bytes_read = recvfrom(s_fd, client_buf,1024,0,nullptr,nullptr);
    printf("[TEST]: bytes read =%d", bytes_read);
    if(bytes_read == -1)
    {
      perror("[RECV FROM ERR]");
      break;
    }
    break;
  }
  printf("[TEST]: text from client =%s",client_buf);
};
