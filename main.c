#include "dynamic_array.h"
#include "nodes.h"

int main(int argc, char *argv[]) {
  peer_arr_t *arr = new_array();
  for (int i = 0; i < INITIAL_CAP + 2; i++) {
    const peer_t new_peer = {.ip = "192.168.1.1", .port = 3000};
    push(arr, &new_peer);
    printf("[TEST]:index=%d peer.id=%s\n", i, new_peer.id);
  }
  peer_t peer_buf;
  pop(arr,&peer_buf);


  return 0;
}
