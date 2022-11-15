CC=gcc
FLAGS=-c

all: frontend backend

backend.o: backend.c backend.h utils.h
	gcc backend.c -c

frontend.o: frontend.c utils.h
	gcc frontend.c -c

limpa:
	echo "A limpar..."
	rm *.o
