#include "utils.h"

//Cada linha do ficheiro descreve um item à venda, e os campos em cada linha são separados por um espaço.
//Cada linha deve conter, por esta ordem:
//identificador único do item, nome do item (uma palavra), categoria (uma palavra), valor atual (valor inicial ou valor
//da licitação mais elevada), valor “compre já”, duração do leilão (= tempo restante), username do utilizador que
//vende, username do utilizador que licitou o valor mais elevado (ou “-” se não tiver ainda sido licitado).

void leFichItens(char *filename) {
    int res, fd;
    Item item;
    FILE *f;
    f = fopen(filename,"rt");
    if(f == NULL){
        printf("Erro ao abrir o ficheiro");
        return;
    }else{
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
            //TODO:Tratar informacao
        }
    }
}

//Retorna o numero de argumentos que o utilizador coloca nos comandos
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
