#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define FIFO_SERVIDOR "SERVIDOR"
#define FIFO_CLIENTE "CLIENTE%d" // usar scanf com o pid que vem do utilizador

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

typedef struct Utilizador
{
    char nome[50];
    char password[50];
    pid_t pid;
} User;

typedef struct {
    Item item;
    User user;
    int comando;
}Comando;

typedef struct resposta
{
    int num;
    pid_t pid;
} Resposta;


int numArgumentos(char str[]);
