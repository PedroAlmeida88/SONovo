#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
typedef struct promocao
{
    char categoria[50];
    int desconto;
    int duracao;       
} Promocao;

Promocao lancaPromotor(char *nome);
