#include "../nodes.h"
#include "../remote_procedure.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

typedef struct {
  char file_hash[32];
  char *date_created;
  struct node_t **known_nodes;
} file_info;

int main() {

  node_array *BOOTSTRAP_NODES = new_node_array();

  push_node(BOOTSTRAP_NODES, (node_t){.id = "55", .ip = "", .port = 5432});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "10", .ip = "", .port = 4292});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "30", .ip = "", .port = 3000});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "29", .ip = "", .port = 7010});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "24", .ip = "", .port = 7002});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "40", .ip = "", .port = 5060});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "49", .ip = "", .port = 5030});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "19", .ip = "", .port = 6939});
  push_node(BOOTSTRAP_NODES, (node_t){.id = "11", .ip = "", .port = 6942});

  node_t node = {
      .id = "12",
      .ip = "localhost",
      .port = 3000,
  };

  struct sockaddr_in src;
  src.sin_port = htons(node.port);
  src.sin_family = AF_INET;
  src.sin_addr.s_addr = INADDR_ANY;

  int sfd = socket(AF_INET, SOCK_DGRAM, 0);

  int r = bind(sfd, (struct sockaddr *)&src, sizeof(src));
  if (r == -1) {
    perror("[ERROR] Socket bind");
    exit(r);
  }

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

  printf("sorted neighbor len %ld", neighboring_nodes->len);
  init_table(&node.peer_table);

  set(&node.peer_table, file.file_hash,
      (peer_t){.ip = "some ip address",
               .port = 42069,
               .job_id = "job?",
               .state = PASSIVE_ST});

  set(&node.peer_table, file.file_hash,
      (peer_t){.ip = "that ip address",
               .port = 12345,
               .job_id = "job!!",
               .state = LEECH_ST});

  set(&node.peer_table, file.file_hash,
      (peer_t){.ip = "what ip address",
               .port = 52598,
               .job_id = "job?!?1",
               .state = SEED_ST});

  printf("[TEST] total size of rpc message =%ld\n", sizeof(rpc_msg));
  printf("[TEST] total size of reply body =%ld\n", sizeof(reply_body));
  printf("[TEST] total size of call body =%ld\n", sizeof(call_body));
  rpc_msg msg_buffer;
  while (1) {
    int r = recvfrom(sfd, &msg_buffer, sizeof(rpc_msg), 0, NULL, 0);
    if (r == -1) {
      perror("[ERROR] Socket bind");
      exit(-1);
    }

    recv_rpc(sfd,&node, &msg_buffer, neighboring_nodes);
  }
  return 0;
}
