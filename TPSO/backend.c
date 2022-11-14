#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int numArgumentos(char str[]){
    int tam = 0;
    char aux[128];
    strcpy(aux, str);
    char *token = strtok(aux, " ");
    
    while(token != NULL){
        tam++;
        token = strtok(NULL, " ");
    }
    return tam;
}

int main() {
    int numArgumento;
    char str[128];

    printf("Bem vindo Administrador\n");

    while (1){
        printf("Comando:");
        fflush(stdout);
        fgets(str, 128, stdin);
        str[strcspn(str, "\n")] = 0;

        printf("String: %s\n", str);

        numArgumento = numArgumentos(str);
        printf("Num de Argumetos: %d\n", numArgumento);
        char *token = strtok(str, " ");

        if(strcmp(token, "users") == 0)}        ///FEITO
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "list") == 0){    ///FEITO
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } 
        else if(strcmp(token, "kick") == 0){    ///TODO:COMPARAR COM TODOS OS CLIENTES LOGADOS
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else {
                token = strtok(NULL, " ");
                printf("User: %s\n", token);
                printf("Valido\n");
            }
        } 
        else if(strcmp(token, "prom") == 0){    ///Feito
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } 
        else if(strcmp(token, "reprom") == 0){  ///Feito
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        } 
        else if(strcmp(token, "cancel") == 0){ ///
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                printf("Nome do executavel: %s\n", token);
                printf("Valido\n");
            }
        }
        else if(strcmp(token, "close") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else {
                printf("Valido\n");
                break;
            }
        } 
        else {
            printf("Comando nao Valido\n");
        }
    } 
    printf("A avisar os clientes que ira fechar\n");
    printf("Fechou\n");
    return 0;
}