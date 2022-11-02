#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv){
    char str[128];

    if(argc != 2) {
        printf("[ERRO] Numero invalido de argumentos");
        return -1;
    }
    //verificaDadosUtilizador(argv);

    while(1){
        printf("Comando:");
        fflush(stdout);
        scanf("%s", str);

        if(strcmp(str, "sell") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "list") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "licat") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "lisel") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "litime") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "time") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "buy") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "cash") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "add") == 0){
            printf("Comando Valido\n");
        }
        else if(strcmp(str, "exit") == 0){
            break;
            printf("Comando Valido\n");
        }
        else {
            printf("Comando nao Valido\n");
        }
    }
    printf("A avisar o servidor que irei sair\n");
    printf("Adeus\n");
}