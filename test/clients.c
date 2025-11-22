

// called by rpc_client.c to spawn clients to connect nodes

#include <stdio.h>
int main(int argc, char **argv) {

  printf("[TEST]: argc should return number of arguments passed =%d\n", argc);

  //*argv  = malloced(sizeof(char * variadic length) * argc)
	// argv is a pointer to  
  for (int i = 0; i < argc; i++) {
    printf("[TEST]: arguments passed =%s\n", argv[i]);
  };

  return 0;
};
