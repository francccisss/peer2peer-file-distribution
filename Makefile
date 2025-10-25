main:main.c 
	gcc -o ./bin/main main.c

main-bin:main
	./bin/main

clean: