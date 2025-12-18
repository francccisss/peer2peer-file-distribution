#include "../file.h"
#include "../lib/cjson/cJSON.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char ip[INET_ADDRSTRLEN];
  uint16_t port;
} origin;

typedef struct {
  char file_hash[16];
  uint32_t piece_length;
  uint64_t length;
  char *name;
  char *path;
  origin **nodes;
  uint16_t node_len;
} desciptor_file;

#define FILE_BUFFER_SIZE 256
int parse_descriptor_file(char *descriptor_path, desciptor_file *file);

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("[ERROR] port not defined\n");
    return -1;
  }
  char *descriptor_path = argv[1];
  desciptor_file file = {0};

  if (parse_descriptor_file(descriptor_path, &file) < 0) {
    fprintf(stderr, "[ERROR]: Unable to parse json descriptor\n");
    return 1;
  };
}

int parse_descriptor_file(char *descriptor_path, desciptor_file *file) {

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
