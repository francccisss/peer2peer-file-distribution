build:main.c
	gcc -o ./bin/main main.c dynamic_array.c

go:./bin/main
	gcc -o ./bin/main main.c dynamic_array.c && ./bin/main

clean: