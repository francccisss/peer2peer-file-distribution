build:main.c
	gcc main.c dynamic_array.c nodes.c -o ./bin/main 
	echo "main built in ./bin directory."

Wrun:main.c
	gcc  -Wall main.c dynamic_array.c nodes.c -o ./bin/main
	./bin/main
	echo "Compiling with warning args"

Wgdb:main.c
	echo "Compiling with warning args and gdb enabled"
	gcc -g main.c dynamic_array.c nodes.c -o ./bin/main

run:./bin/main 
	./bin/main

go:./bin/main
	gcc main.c dynamic_array.c nodes.c -o ./bin/main  
	./bin/main
