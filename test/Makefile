c_files =  ../peers.c ../remote_procedure.c ../nodes.c ../lib/cjson/cJSON.c

build_run: rpc_test 
	echo "Runningrpc"
	gcc -Wall -g main.c -o main
	echo "Compiling with warning args"
	./main 

rpc_test: rpc_test.c
	echo "BUILDING NODES"
	gcc -Wall ./rpc_test.c $(c_files) -o rpc_test 
	echo "Compiled with warning args"

client_test: client_test.c
	echo "BUILDING PEERS"
	gcc -Wall -g  ./client_test.c $(c_files) -o client_test
	echo "Compiled with warning args"

file_test: file_test.c
	echo "BUILDING file_test"
	gcc -Wall -g  ./file_test.c $(c_files) -o file_test
	echo "Compiled with warning args"

.PHONY: clean
clean:
	rm -rf rpc_test client_test main


