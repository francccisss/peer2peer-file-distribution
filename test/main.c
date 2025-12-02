

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_OBJECTS 3
#define STR_LEN 32

int fds[2];
int fork_proc(char *path, char *argc[]) {
  pid_t pid = fork();
  if (pid == 0) {
    printf("[TEST]: child process pid=%d\n", pid);
    // execv if identified as child to execute a c program

    int r = execv(path, argc);
    if (r < 0) {
      perror("[ERROR]: EXECV");
      exit(r);
    }
    close(fds[0]);
    exit(r);
  }
  if (pid < 0) {
    perror("[ERROR] fork error");
    exit(pid);
  }
  return 0;
};

typedef char **fork_args;

int main() {
  printf("[TEST]\n");
  int res = pipe(fds);
  if (res == -1) {
    perror("[ERROR] piping");
    exit(res);
  }

  char *peer_ports[] = {"6969", "4209"};
  char *node_ports[] = {"3000", "3001", "3002"};

  char *f_args[5];
  for (int i = 0; i < MAX_OBJECTS; i++) {
    *(f_args + i) = malloc(sizeof(char) * STR_LEN);
  }

  int const NUM_PEERS = 2;
  int const NUM_NODES = 3;
  // peers
  for (int i = 0; i < NUM_PEERS; i++) {
    f_args[0] = peer_ports[i];
    f_args[1] = "0";
    fork_proc("./rpc_test", f_args);
  }

  pid_t p = getpid();
  if (p > 0) {
    // nodes
    for (int i = 0; i < NUM_NODES; i++) {
      f_args[0] = node_ports[i];
      f_args[1] = "1";
      f_args[2] = node_ports[2];
      if (i == NUM_NODES - 1) {
				printf("[INFO TEST]: WITH PEER\n");
        f_args[1] = "2";
        f_args[2] = "3000";
        f_args[2] = "3002";
      }
      fork_proc("./rpc_test", f_args);
    }
  }

  printf("[TEST]: buidling nodes\n");
  return 0;
}
