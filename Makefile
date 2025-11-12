build:main.c
	gcc main.c nodes.c peer_table.c -o ./bin/main 
	echo "main built in ./bin directory."

Wrun:main.c
	gcc -Wall main.c peer_table.c nodes.c -o ./bin/main
	./bin/main
	echo "Compiling with warning args"

Wgdb:main.c
	echo "Compiling with warning args and gdb enabled"
	gcc -g  peer_table.c nodes.c main.c -o ./bin/main

run:./bin/main 
	./bin/main

test_build_gdb: ./test/rpc_test.c
	echo "Running RPC"
	gcc -Wall -g ./test/rpc_test.c peers.c remote_procedure.c peer_table.c nodes.c -o ./test/test
	echo "Compiling with warning args and gdb"
	gdb ./test/test



test_rpc: ./test/rpc_test.c
	echo "Running RPC"
	gcc -Wall ./test/rpc_test.c peers.c remote_procedure.c peer_table.c nodes.c -o ./test/test
	echo "Compiling with warning args"
	./test/test


