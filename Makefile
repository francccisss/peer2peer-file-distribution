build:main.c
	gcc main.c nodes.c   -o ./bin/main 
	echo "main built in ./bin directory."

Wrun:main.c
	gcc -Wall main.c   nodes.c -o ./bin/main
	./bin/main
	echo "Compiling with warning args"

Wgdb:main.c
	echo "Compiling with warning args and gdb enabled"
	gcc -g    nodes.c main.c -o ./bin/main

run:./bin/main 
	./bin/main

test_build_gdb: ./test/rpc_test.c
	echo "Running RPC"
	gcc -Wall -g ./test/rpc_test.c peers.c remote_procedure.c   nodes.c -o ./test/main
	echo "Compiling with warning args"
	gdb ./test/main

test_rpc: ./test/rpc_test.c
	echo "Running RPC"
	gcc -Wall ./test/rpc_test.c peers.c remote_procedure.c   nodes.c -o ./test/main
	echo "Compiling with warning args"
	./test/main 3000

test_rpc_c: ./test/rpc_test.c
	echo "Running RPC client"
	gcc -Wall -g ./test/client_test.c peers.c remote_procedure.c   nodes.c -o ./test/client_test
	echo "Compiling with warning args"
	./test/client_test 6969


