#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <threads.h>
#include <unistd.h>

#define MAX_OBJECTS 3
#define STR_LEN 32
#define CHILDREN_NUM 5

int fds[2];

pid_t child_pid_list[CHILDREN_NUM];
int child_pid_list_len = 0;

void push_child(pid_t child_pid) {
  child_pid_list[child_pid_list_len] = child_pid;

  child_pid_list_len++;
};

uint32_t pop_child() {
  if (child_pid_list_len == 0) {
    printf("[ERROR]: array is empty\n");
    return -1;
  }
  pid_t val = child_pid_list[child_pid_list_len - 1];
  child_pid_list[child_pid_list_len - 1] = 0;
  child_pid_list_len--;
  return val;
}

int fork_proc(char *path, char *argc[]) {
  pid_t pid = fork();
  fflush(NULL);
  if (pid < 0) {
    perror("[ERROR] fork error");
    exit(pid);
  }

  if (pid == 0) {
    // execv if identified as child to execute a c program

    close(fds[0]);
    int r = execv(path, argc);
    if (r < 0) {
      perror("[ERROR]: execv");
      printf("[ERROR]: Closing child\n");
      exit(1);
    }
  }
  push_child(pid);

  return 0;
};

typedef char **fork_args;

// use either SIGINT OR SIGTERM
void sig_handler(int signo) {
  printf("[INFO]: Interrupt received\n");
  if (signo == SIGINT || signo == SIGTERM) {
    printf("[INFO]: Closing child processes\n");
    for (int i = 0; i < child_pid_list_len; i++) {
      int status;
      pid_t c_pid = child_pid_list[i];
      int r = kill(0, SIGINT);
      if (r != 0) {
        printf("[ERROR]: unable to kill child process c_pid=%d\n", c_pid);
      }
      int w_cpid = waitpid(c_pid, &status, 0);
      if (WIFCONTINUED(status)) {
        printf("[ERROR]: returned c_pid from wait =%d, current c_pid =%d\n",
               w_cpid, c_pid);
        printf("[ERROR]: unable to wait for child process status=%d\n", status);
        exit(-1);
      }
      if (WIFEXITED(status) == 0 && w_cpid == c_pid) {
        printf("wcpid =%d, c_pid=%d\n", w_cpid, c_pid);
        printf("status =%d\n", status);
      }
    }
    printf("[INFO]: Closing processes\n");
    exit(0);
  }
}
int main() {

  int res = pipe(fds);
  if (res == -1) {
    perror("[ERROR] piping");
    exit(res);
  }

  char *peer_ports[] = {"6969", "4209"};
  char *node_ports[] = {"3000", "3001", "3002"};

  char *f_args[6];

  int const NUM_PEERS = 2;
  int const NUM_NODES = 3;
  // peers
  for (int i = 0; i < NUM_PEERS; i++) {
    f_args[0] = peer_ports[i];
    f_args[1] = "0";
    f_args[2] = NULL;
    fork_proc("./rpc_test", f_args);
  }

  pid_t p = getpid();

  // nodes
  // connects to 3001
  f_args[0] = node_ports[0];
  f_args[1] = "1";
  f_args[2] = node_ports[1];
  f_args[3] = NULL;
  fork_proc("./rpc_test", f_args);

  // connects to 3002
  f_args[0] = node_ports[1];
  f_args[1] = "1";
  f_args[2] = node_ports[2];
  f_args[3] = NULL;
  fork_proc("./rpc_test", f_args);

  // connects to 3001 and 3000
  printf("[INFO TEST]: WITH PEERS\n");
  f_args[0] = node_ports[2];
  f_args[1] = "2";
  f_args[2] = node_ports[0];
  f_args[3] = node_ports[1];
  f_args[4] = NULL;
  fork_proc("./rpc_test", f_args);

  sigset_t sig_set;
  int signop;

  sigemptyset(&sig_set);
  sigaddset(&sig_set, SIGINT);
  sigaddset(&sig_set, SIGTERM);
  sigprocmask(SIG_BLOCK, &sig_set, NULL);
  int status = sigwait(&sig_set, &signop);
  if (status != 0) {
    perror("[ERROR]: Sigwait\n");
    exit(status);
  }

  switch (signop) {
  case SIGINT:
    printf("[INFO] received SIGINT\n");

    for (int i = 0; i < child_pid_list_len; i++) {
      int status;
      pid_t c_pid = child_pid_list[i];
      int r = kill(c_pid, SIGINT);
      if (r != 0) {
        printf("[ERROR]: unable to kill child process c_pid=%d\n", c_pid);
      }
      int w_cpid = waitpid(c_pid, &status, 0);
      if (WIFCONTINUED(status)) {
        printf("[ERROR]: returned c_pid from wait =%d, current c_pid =%d\n",
               w_cpid, c_pid);
        printf("[ERROR]: unable to wait for child process status=%d\n", status);
        return -1;
      }
      if (WIFEXITED(status) == 0 && w_cpid == c_pid) {
        printf("wcpid =%d, c_pid=%d\n", w_cpid, c_pid);
        printf("status =%d\n", status);
      }
    }
    printf("[INFO]: Closing processes\n");
    break;
  }

  return 0;
}
