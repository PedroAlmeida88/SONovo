#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
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
    char buffer[50],nomeItem[50],categoria[50],aux[128];
    int precoBase,precoCompreJa,duracao,id,valor;

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
        strcpy(aux,str);
        numArgumento = numArgumentos(str);
        printf("Num de Argumetos: %d\n", numArgumento);
        char *token = strtok(str, " ");

        if(strcmp(token, "sell") == 0){
            printf("String dentro: %s\n", str);

            if(numArgumento != 6)
                printf("Nao Valido\n");
            else if (sscanf(aux,"%s %s %s %d %d %d",buffer,nomeItem,categoria,&precoBase,&precoCompreJa,&duracao) != 6)
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
        else if(strcmp(token, "licat") == 0){       //TODO:Ver se a categoria existe
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else {
                token = strtok(NULL, " ");
                printf("Nome da categoria: %s\n", token);
                printf("Valido\n");
            }
        }
        else if(strcmp(token, "lisel") == 0){       //TODO:Ver se o vendedor existe
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                printf("Nome do vendedor: %s\n", token);
                printf("Valido\n");
            }

        }
        else if(strcmp(token, "litime") == 0){
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                duracao = atoi(token);
                printf("Hora em segundos: %d\n", duracao);
                printf("Valido\n");
            }

        }
        else if(strcmp(token, "time") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "buy") == 0){
            if(numArgumento != 3)
                printf("Nao Valido1\n");
            else if(sscanf(aux,"%s %d %d",buffer,&id,&valor) != 3)
                printf("Nao Valido2\n");
            else {
                printf("Valido\n");
            }
        }
        else if(strcmp(token, "cash") == 0){
            if(numArgumento != 1)
                printf("Nao Valido\n");
            else
                printf("Valido\n");
        }
        else if(strcmp(token, "add") == 0){
            if(numArgumento != 2)
                printf("Nao Valido\n");
            else{
                token = strtok(NULL, " ");
                valor = atoi(token);
                printf("Valor: %d\n", valor);
                printf("Valido\n");
            }

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
