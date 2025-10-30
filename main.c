#include "peer_table.h"
#include "nodes.h"

int main(int argc, char *argv[]) {
  // each entry is a pointer to an array of peers
  bucket_t *(*table)[MAX_SIZE_ARRAY] =
      malloc(sizeof(bucket_t) * MAX_SIZE_ARRAY);
  for (int i = 0; i < MAX_SIZE_ARRAY; ++i) {
    (*table)[i] = new_array();
  };

  free(table);
};
