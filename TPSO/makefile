CC=gcc
FLAGS=-c

all: frontend backend

frontend: frontend.o utils.o
	gcc frontend.o utils.o -o frontend

backend: backend.o utils.o
	gcc backend.o utils.o -o backend

backend.o: backend.c backend.h utils.h
	gcc -c backend.c -o backend.o

frontend.o: frontend.c utils.h
	gcc -c frontend.c -o frontend.o 

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

limpa:
	echo "A limpar..."
	rm *.o
