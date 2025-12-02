entry: test_rpc test_client
	echo "Runningrpc"
	gcc -Wall -g main.c -o main
	echo "Compiling with warning args"
	./main 

test_rpc: rpc_test.c
	echo "BUILDING NODES"
	gcc -Wall ./rpc_test.c ../peers.c ../remote_procedure.c ../nodes.c -o rpc_test 
	echo "Compiled with warning args"

test_client: rpc_test.c
	echo "BUILDING PEERS"
	gcc -Wall -g  ./client_test.c ../peers.c ../remote_procedure.c ../nodes.c -o  client_test
	echo "Compiled with warning args"


clean:
	rm -rf rpc_test client_test

