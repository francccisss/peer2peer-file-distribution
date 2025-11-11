#include "../nodes.h"
#include "../remote_procedure.h"
#include <stdio.h>
#include <string.h>

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
      .port = 6969,
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

  call_body body = {
      .type = GET_PEERS,

  };
  rpc_msg call = {
      .segment_count = 0,
      .segment_number = 0,
      .correlation_id = "some random value",
      .msg_type = CALL,
      .body.cbody = body,
  };
  memcpy(body.payload, file.file_hash, 32);

  init_table(node.peer_table);

  recv_rpc(0, &call, neighboring_nodes, &node);
  return 0;
}
