#include <stdio.h>
#include <string.h>
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


//TODO: Ver quantos argumentos cada um dos comandos tem 
int main(int argc, char **argv){
    char str[128];
    int numArgumento;

    if(argc != 3) {
        printf("[ERRO] Numero invalido de argumentos");
        return -1;
    }
    //verificaDadosUtilizador(argv);

    while(1){
        printf("Comando:");
        fflush(stdout);
        fgets(str, 128, stdin);
        str[strcspn(str, "\n")] = 0;

        printf("String: %s\n", str);

        numArgumento = numArgumentos(str);
        printf("Num de Argumetos: %d\n", numArgumento);
        char *token = strtok(str, " ");

        if(strcmp(token, "sell") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "list") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "licat") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "lisel") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "litime") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "time") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "buy") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "cash") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "add") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "exit") == 0){
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
    printf("A avisar o servidor que irei sair\n");
    printf("Adeus\n");
}
