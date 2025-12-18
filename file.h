
#ifndef FILES
#define FILES

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// a peer bucket that contains an array of peers
typedef struct {
  bool active;
  uint32_t key;
  size_t cap;
  size_t len; // always access last element at len-1 because of 0 indexing
  void *chunks;
} data_array_t;

typedef struct {
  char *name;
  char *key;
  data_array_t data_chunks;
} file_t;

void init_file_table(file_t **unint_table);
void set_file(file_t **table, const char *key, file_t data);
void get_file_bucket(data_array_t *(*table)[], const char *key,
                     data_array_t **file_bucket_buf);
uint32_t hash(const char *input);
void print_files_from_bucket(data_array_t *bucket, const char *key);

data_array_t *new_file_array();
void resize_file(data_array_t *d_arr);
void push_file(data_array_t *d_arr, file_t data);
void pop_file(data_array_t *d_arr, file_t *file_buf);

#endif
