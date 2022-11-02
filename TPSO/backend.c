#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    char str[128];

    printf("Bem vindo Administrador\n");

    while (1){
        printf("Comando:");
        fflush(stdout);
        scanf("%s", str);

        if(strcmp(str, "users") == 0){
            printf("Valido\n");
        } 
        else if(strcmp(str, "list") == 0){
            printf("Valido\n");
        } 
        else if(strcmp(str, "kick") == 0){
            printf("Valido\n");
        } 
        else if(strcmp(str, "prom") == 0){
            printf("Valido\n");
        } 
        else if(strcmp(str, "reprom") == 0){
            printf("Valido\n");
        } 
        else if(strcmp(str, "cancel") == 0){
            printf("Valido\n");
        } 
        else if(strcmp(str, "close") == 0){
            printf("Valido\n");
            break;
        } 
        else {
            printf("Comando nao Valido\n");
        }
    } 
    printf("A avisar os clientes que ira fechar\n");
    printf("Fechou\n");
    return 0;
}