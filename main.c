#include "dynamic_array.h"
#include "nodes.h"

int main(int argc, char *argv[]) {
  peer_arr_t *arr = new_array();
  for (int i = 0; i < INITIAL_CAP; i++) {
    const peer_t new_peer = {.ip = "192.168.1.1", .port = i};
    push(arr, new_peer);
  }
  peer_t peer_buf;
pop(arr, &peer_buf);
  return 0;
};
