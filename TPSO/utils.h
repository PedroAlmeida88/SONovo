#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct item
{
    int id;
    char nome[50];
    char categoria[50];
    int valAtual;
    int valCompreJa;
    int duracao;
    char usernameVendedor[50];
    char usernameLicitador[50];
} Item;

void leFichItens(char *filename);

int numArgumentos(char str[]);
