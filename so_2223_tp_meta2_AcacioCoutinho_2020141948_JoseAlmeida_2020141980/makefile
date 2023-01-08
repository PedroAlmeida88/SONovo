all: frontend backend

frontend: frontend.o utils.o
	gcc frontend.o utils.o -o frontend

backend: backend.o utils.o users_lib.o
	gcc backend.o utils.o users_lib.o -o backend

backend.o: backend.c backend.h utils.h users_lib.h
	gcc -c backend.c -o backend.o

frontend.o: frontend.c utils.h
	gcc -c frontend.c -o frontend.o 

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

limpa:
	echo "A limpar..."
	rm backend.o frontend.o utils.o
