CC=gcc
FLAGS=-c

all: programa.o imprime.o
	gcc programa.o imprime.o -o ex3

backend.o: backend.c backend.h
	gcc backend.c -c

frontend.o: frontend.c frontend.h
	gcc frontend.c -c

limpa:
	echo "A limpar..."
	rm *.o
