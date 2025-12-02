build:main.c
	gcc main.c nodes.c   -o ./bin/main 
	echo "main built in ./bin directory."

Wrun:main.c
	gcc -Wall main.c   nodes.c -o ./bin/main
	./bin/main
	echo "Compiling with warning args"

Wgdb:main.c
	echo "Compiling with warning args and gdb enabled"
	gcc -g nodes.c main.c -o ./bin/main

run:./bin/main 
	./bin/main

