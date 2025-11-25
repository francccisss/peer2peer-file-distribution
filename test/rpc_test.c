#include "../nodes.h"
#include "../peers.h"
#include "../remote_procedure.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

typedef struct {
  char file_hash[ID_SIZE];
  char *date_created;
  struct node_t **known_nodes;
} file_info;

int main(int argc, char **argv) {

  node_array *BOOTSTRAP_NODES = new_node_array();
  int arg_port = atoi(argv[1]);

  // [TESTING] used for the neighbor of the closest node to the new node
  // for calling get_peers()
  // new node -> closest_node -> neighbor
  if (argc < 3) {
    push_node(BOOTSTRAP_NODES, (node_t){.id = "69", .ip = "localhost", .port = 3001});
  } else {
    push_node(BOOTSTRAP_NODES, (node_t){.id = "420", .ip = "localhost", .port = 3000});
  };

  node_t node = {
      .id = "12",
      .ip = "localhost",
      .port = arg_port,
  };
  printf("[TEST] arg_port=%d\n", arg_port);

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
    printf("[TEST]: Neighbor id =%s, port =%d\n", (*neighboring_nodes->data)[i].id,
           (*neighboring_nodes->data)[i].port);
  }

  compare_hash(neighboring_nodes, neighboring_nodes->len, file.file_hash);

  printf("sorted neighbor len %ld\n", neighboring_nodes->len);
  init_peer_table(&node.peer_table);

  // [TESTING] used for the neighbor of the closest node to the new node
  // for calling get_peers()
  // new node -> closest_node -> neighbor
  // if (argc > 2) {
  //   set_peer(&node.peer_table, file.file_hash,
  //            (peer_t){.ip = "localhost",
  //                     .port = 3000,
  //                     .job_id = "THiS NODE?",
  //                     .state = PASSIVE_ST});
  //
  //   set_peer(&node.peer_table, file.file_hash,
  //            (peer_t){.ip = "localhost",
  //                     .port = 3345,
  //                     .job_id = "job!!",
  //                     .state = LEECH_ST});
  //
  //   set_peer(&node.peer_table, file.file_hash,
  //            (peer_t){.ip = "localhost",
  //                     .port = 5598,
  //                     .job_id = "job?!?1",
  //                     .state = SEED_ST});
  // }

  rpc_msg msg_buffer;
  bool wait = false;
  while (1) {
    int r = recvfrom(sfd, &msg_buffer, sizeof(rpc_msg), 0, NULL, 0);
    if (r == -1) {
      perror("[ERROR] Socket bind");
      exit(-1);
    }

    int ri = recv_rpc(sfd, &node, file.file_hash, &msg_buffer,
                      neighboring_nodes, &wait);
    if (ri < 0) {
      printf("[ERROR RECV RPC]: Something went wrong");
    }
  }
  return 0;
}
