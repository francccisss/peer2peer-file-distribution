build:main.c
	gcc -o ./bin/main main.c dynamic_array.c nodes.c

go:./bin/main
	gcc -o ./bin/main main.c dynamic_array.c nodes.c && ./bin/main

clean: