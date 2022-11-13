//#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//Cada linha do ficheiro descreve um item à venda, e os campos em cada linha são separados por um espaço.
//Cada linha deve conter, por esta ordem:
//identificador único do item, nome do item (uma palavra), categoria (uma palavra), valor atual (valor inicial ou valor
//da licitação mais elevada), valor “compre já”, duração do leilão (= tempo restante), username do utilizador que
//vende, username do utilizador que licitou o valor mais elevado (ou “-” se não tiver ainda sido licitado).

void leFichItens(char *filename) {
    int id, vAtual, vCompreJa, duracao, res, fd;
    char nome[50],categoria[50],usernameVendedor[50],usernameLicitador[50];
    FILE *f;
    f = fopen(filename,"rt");
    if(f == NULL){
        printf("Erro ao abrir o ficheiro");
        return;
    }else{
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&id,nome,categoria,&vAtual,&vCompreJa,&duracao,usernameVendedor,usernameLicitador) != EOF){
            printf("%d %s %s %d %d %d %s %s\n",id,nome,categoria,vAtual,vCompreJa,duracao,usernameVendedor,usernameLicitador);
            //TODO:Tratar informacao
        }
    }
}

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

int main(){
    //leFichItens("itens.txt");

    char str[128];
    scanf("%s", &str);

    printf("num de argumentos: %d\n", numArgumentos(str));
    printf("String: %s\n", str);
    char *token = strtok(str, " ");
    printf("Token: %s\n", token);
    token = strtok(NULL, " ");
    printf("Resto: %s\n", token);
    return 0;
}