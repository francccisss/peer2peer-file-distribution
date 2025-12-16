#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <bits/types/struct_timeval.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../lib/cjson/cJSON.h"
#include "../nodes.h"
#include "../remote_procedure.h"

typedef struct {
  char file_hash[ID_SIZE];
  uint32_t piece_length;
  uint64_t length;
  char *name;
  char *path;
  origin **nodes;
  uint16_t node_len;
} desciptor_file;

#define FILE_BUFFER_SIZE 256

int parse_json_descriptor(char *descriptor_path, desciptor_file *file);

int main(int argc, char **argv) {
  struct sockaddr_in src;
  if (argc < 2) {
    printf("[ERROR] port not defined\n");
    return -1;
  }
  char *descriptor_path = argv[2];
  desciptor_file file = {0};

  if (parse_json_descriptor(descriptor_path, &file) < 0) {
    fprintf(stderr, "[ERROR]: Unable to parse json descriptor\n");
    return 1;
  };

  // // what is being distributed
  int host_port = atoi(argv[1]);
  printf("[INFO]: host_port=%d\n", host_port);
  node_t node = {
      .id = "14",
      .ip = "localhost",
      .port = host_port,
  };
  src.sin_family = AF_INET;
  src.sin_port = htons(node.port);
  src.sin_addr.s_addr = INADDR_ANY;

  int s_fd = socket(AF_INET, SOCK_DGRAM, 0);
  int r = bind(s_fd, (struct sockaddr *)&src, sizeof(src));

  if (r != 0) {
    perror("[ERROR] Socket bind");
    exit(-1);
  }

  node_array *neighboring_nodes = new_node_array();

  // for now for every file, a new process is created instead
  // of reusing the same process
  // compare_hash(node.neighbors, N_COUNT, file.file_hash, closest_neighbors);

  printf("\n\n[INFO]: Bootstrapping nodes...\n");
  for (int i = 0; i < file.node_len; i++) {

    // node ID will be used to calculate
    // XOR and apply result to distance
    origin *current_node_origin = file.nodes[i];
    node_t new_node = {
        .id = "Assign an ID", .distance = 0, .port = file.nodes[i]->port};
    strcpy(current_node_origin->ip, new_node.ip);
    push_node(neighboring_nodes, new_node);
  }
  printf("[INFO]: Bootstrapping done.\n");
  printf("[INFO]: Neighboring nodes:\n");
  for (int i = 0; i < neighboring_nodes->len; i++) {
    printf("[TEST]: neighbor number=%d id=%s\n", i + 1,
           (*neighboring_nodes->data)[i].id);
    printf("[TEST]: port=%d\n\n", (*neighboring_nodes->data)[i].port);
  }

  // assigning the absolute address of the caller
  origin absolute_address = {.port = node.port};
  strcpy(absolute_address.ip, node.ip);
  init_peer_table(&node.peer_table);

  struct sockaddr_in get_sock;
  socklen_t len = sizeof(get_sock);
  int gr = getsockname(s_fd, (struct sockaddr *)&get_sock, &len);
  if (gr != 0) {
    perror("[ERROR]: getsockname");
    exit(gr);
  }

  printf("[TEST]: other nodes area reading big endianess of port -> %d\n",
         get_sock.sin_port);
  printf("[TEST]: getsock port: flipped %d\n", ntohs(get_sock.sin_port));
  get_peers(s_fd, &node, neighboring_nodes, file.file_hash, absolute_address,
            absolute_address);

  bool wait = true;
  rpc_msg msg_buffer;
  fd_set rfd;
  struct timeval t;
  uint8_t max_time = 4;

  // NOTICE: this only works for a single file hash
  peer_bucket_t *peer_bucket_buf = NULL;
  get_peer_bucket(&node.peer_table, file.file_hash, &peer_bucket_buf);
  if (peer_bucket_buf == NULL) {
    printf("[ERROR]: bucket was unintialized\n");
  }

  while (1) {

    if (wait) {
      FD_ZERO(&rfd);
      FD_SET(s_fd, &rfd);
      t.tv_sec = max_time;
      t.tv_usec = 0;

      int retval = select(s_fd + 1, &rfd, NULL, NULL, &t);
      if (retval < 0) {
        perror("[ERROR] select");
        exit(retval);
      };
      if (retval == 0) {
        printf("[INFO]: polling timed out\n");
        printf("[INFO]: joining %ld peers\n", peer_bucket_buf->len);
        int r = join_peers(s_fd, &node, file.file_hash);
        if (r != 0) {
          printf("[INFO]: there are no peers to join in hash=%s\n",
                 file.file_hash);
          printf("[INFO]: Exiting...\n");
          exit(r);
        }
        continue;
      }
    }
    int r = recvfrom(s_fd, &msg_buffer, sizeof(msg_buffer), 0, NULL, 0);
    if (r == -1) {
      perror("[ERROR] Socket bind");
      exit(-1);
    }
    printf("request received\n");
    recv_rpc(s_fd, &node, file.file_hash, &msg_buffer, neighboring_nodes,
             &wait);
  }

  return 0;
}

int parse_json_descriptor(char *descriptor_path, desciptor_file *file) {

  char buffer[FILE_BUFFER_SIZE];

  int df_file = open(descriptor_path, O_RDONLY);
  if (df_file < 0) {
    fprintf(stderr, "[ERROR]: \n");
    return df_file;
  }
  int read_bytes;
  while ((read_bytes = read(df_file, buffer, FILE_BUFFER_SIZE)) > 0) {
    if (read_bytes < 0) {
      perror("[ERROR] reading from df_file\n");
      return df_file;
    }
  };

  cJSON *df_json = cJSON_Parse(buffer);

  if (df_json == NULL) {
    fprintf(stderr, "[ERROR]: Unable to parse Json data\n");
    return 1;
  }

  cJSON *f = cJSON_GetObjectItemCaseSensitive(df_json, "file");

  if (f == NULL && !cJSON_IsObject(f)) {
    fprintf(stderr, "[ERROR]: {file} key is Null or not Object\n");
    return 1;
  }

  char *f_str = cJSON_Print(f);
  printf("[TEST]: print file object =%s\n", f_str);
  cJSON *f_path = cJSON_GetObjectItemCaseSensitive(f, "path");
  if (!cJSON_IsString(f_path)) {
    fprintf(stderr, "[ERROR]: {path} key is is not String\n");
    return 1;
  }

  cJSON *f_name = cJSON_GetObjectItemCaseSensitive(f, "name");

  if (!cJSON_IsString(f_name)) {
    fprintf(stderr, "[ERROR]: {name} key is is not String\n");
    return 1;
  }

  cJSON *f_len = cJSON_GetObjectItemCaseSensitive(f, "length");

  if (!cJSON_IsNumber(f_len)) {
    fprintf(stderr, "[ERROR]: {length} key is is not Number\n");
    return 1;
  }

  file->path = f_path->valuestring;
  file->name = f_name->valuestring;
  file->length = f_len->valueint;

  cJSON *f_hk = cJSON_GetObjectItemCaseSensitive(df_json, "hash_key");
  if (f_hk == NULL && !cJSON_IsString(f_hk)) {
    fprintf(stderr, "[ERROR]: {length} key is is not String\n");
    return 1;
  }

  cJSON *f_pclen = cJSON_GetObjectItemCaseSensitive(df_json, "piece_length");
  if (f_pclen == NULL && !cJSON_IsNumber(f_pclen)) {
    fprintf(stderr, "[ERROR]: {length} key is is not Number\n");
    return 1;
  }

  cJSON *f_nodes = cJSON_GetObjectItemCaseSensitive(df_json, "nodes");
  if (f_nodes == NULL) {
    fprintf(stderr, "[ERROR]: {nodes} key does not exist\n");
    return 1;
  }
  if (!cJSON_IsArray(f_nodes)) {
    fprintf(stderr, "[ERROR]: {nodes} key is is not Array\n");
    return 1;
  }

  cJSON *el = NULL;
  int arr_size = cJSON_GetArraySize(f_nodes);
  file->nodes = malloc(arr_size * sizeof(origin *));
  int i = 0;
  cJSON_ArrayForEach(el, f_nodes) {
    cJSON *f_ip = cJSON_GetObjectItemCaseSensitive(el, "ip");
    cJSON *f_port = cJSON_GetObjectItemCaseSensitive(el, "port");

    if (f_ip == NULL || f_port == NULL) {
      fprintf(stderr, "[ERROR]: does not exist\n");
      return 1;
    }

    if (!cJSON_IsString(f_ip)) {
      fprintf(stderr, "[ERROR]: ip not String\n");
      return 1;
    }

    if (!cJSON_IsNumber(f_port)) {
      fprintf(stderr, "[ERROR]: port not Number\n");
      return 1;
    }
    origin *new_node = malloc(sizeof(origin));
    (*new_node).port = f_port->valueint;
    strcpy(new_node->ip, f_ip->valuestring);
    file->nodes[i] = new_node;
  }

  file->node_len = arr_size;
  strcpy(file->file_hash, f_hk->valuestring);
  file->piece_length = f_pclen->valueint;

  printf("[TEST] path=%s\n", file->path);
  printf("[TEST] name=%s\n", file->name);
  printf("[TEST] length=%ld\n", file->length);
  printf("[TEST] hash=%s\n", file->file_hash);
  printf("[TEST] piece_length=%d\n", file->piece_length);

  for (int i = 0; i < file->node_len; i++) {
    printf("[TEST] node[%d].port=%d\n", i, file->nodes[i]->port);
  }

  return 0;
}
