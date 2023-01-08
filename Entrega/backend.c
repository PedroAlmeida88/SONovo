#include "utils.h"
#include "backend.h"
#include "users_lib.h"
char CLIENT_FIFO_FINAL[100];
User ListaClientes[20];
int nUsers=0;
int tempo=0;
int tempoInicial; //tempo lido do fich
int tempoRestante; //->heartbeat
int id=1;
char listaPromotores[10][50];
int listaCliAtivos[20][2];//lista com o pid e tempo restante de cada cliente
int listaItems[30][2];//pode 30 id e 30 duracoes | duracao = tempoInicial + duracao do item
int nPromotores;
//Promocao promocoes[100];
//int numPromocoes = 0;
int itensPromocao[30][3];//id do item | valor original | duracao da promocao
///Qaundo o item venderm tirar do itensPromocao


typedef struct {
    int continua;
    int numProm;
    pthread_mutex_t *ptrinco;
}TDATA;

pthread_mutex_t trinco;
pthread_t tid[11];
TDATA data[11]; //1+10 timer + até 10 promotores

void informaClienteFim(){
    for(int i=0;i<20;i++){
        if(ListaClientes[i].pid != -1){
            //enviar para todos
            union sigval info;
            sigqueue(ListaClientes[i].pid,SIGUSR1,info);
        }
    }
}

void funcSair(){
    printf("\nA encerrar o servidor...");
    printf("A avisar os clientes que ira fechar\n");

    informaClienteFim();

    unlink(FIFO_SERVIDOR);
    exit(1);
}

int initItensPromocaoPromocao(){
    for(int i =0;i<30;i++){
        itensPromocao[i][0] = -1; //id = -1
    }
}

void listaItensComPromocoesAtivas(){
    for(int i =0;i<30;i++){
        if(itensPromocao[i][0] != -1) {
            printf("Id Item: %d\n", itensPromocao[i][0]);
            printf("Duracao: %d\n", itensPromocao[i][1]);
            printf("Duracao: %d\n\n", itensPromocao[i][2]);
        }
    }
}

void verificaFimPromocao(){
    for(int i =0;i<30;i++){
        if(itensPromocao[i][0] != -1) {
            itensPromocao[i][2] -= 10;
            if(itensPromocao[i][2] <= 0){
                printf("O item %d tem o valor original1\n",itensPromocao[i][0]);

                Item item;
                FILE *f;
                int i, count;
                f = fopen(getenv("FITEMS"), "rt");
                if (f == NULL) {
                    fprintf(stderr, "Ficheiro nao encontrado\n");
                    funcSair();
                    return;
                } else {
                    count = 0;
                    fscanf(f, "%d", &i);
                    while (fscanf(f, "%d %s %s %d %d %d %s %s", &item.id, item.nome, item.categoria, &item.valAtual,
                                  &item.valCompreJa, &item.duracao, item.usernameVendedor, item.usernameLicitador) != EOF) {
                        for(int j = 0; j<30;j++){
                            if (itensPromocao[j][0] == item.id) {
                                printf("O item %d tem o valor original2\n",itensPromocao[j][0]);
                                item.valCompreJa = itensPromocao[count][1];
                                modifyLineInFile(getenv("FITEMS"), count, item);
                                itensPromocao[j][0] = -1;
                            }
                        }
                        count++;
                    }
                }
                fclose(f);

            }
        }
    }
}

void trataPromocao(Promocao promocao) {
    Item item;
    FILE *f;
    int i, count;
    f = fopen(getenv("FITEMS"), "rt");
    if (f == NULL) {
        fprintf(stderr, "Ficheiro nao encontrado\n");
        funcSair();
        return;
    } else {
        count = 2;
        fscanf(f, "%d", &i);
        while (fscanf(f, "%d %s %s %d %d %d %s %s", &item.id, item.nome, item.categoria, &item.valAtual,
                      &item.valCompreJa, &item.duracao, item.usernameVendedor, item.usernameLicitador) != EOF) {
            if (strcmp(promocao.categoria, item.categoria) == 0) {
                for(int j =0;j<20;j++){
                    if(itensPromocao[j][0] == -1 && item.id != itensPromocao[j][0]) { //evitar aplicar promocoes a itens que ja tẽm uma promocap
                        itensPromocao[j][0] = item.id;
                        itensPromocao[j][1] = item.valCompreJa;
                        itensPromocao[j][2] = promocao.duracao;
                        listaItensComPromocoesAtivas();
                        break;
                    }
                }
                printf("O item %s tem prom de %d\n",item.nome,promocao.desconto);
                float valor_desconto = (float)item.valCompreJa* (1 - (float)promocao.desconto/100);
                int valorComDesconto =(int)valor_desconto; // arredonda para o inteiro mais próximo
                item.valCompreJa = valorComDesconto;
                modifyLineInFile(getenv("FITEMS"), count, item);
            }
            count++;
        }
    }
    fclose(f);
}

int existeUserLista(char *username){
    for(int i =0;i<20;i++){
        if(strcmp(username,ListaClientes[i].nome) == 0)
            return 1;//user ja esta logado
    }
    return 0;
}

//preenche a lista com os promotores e develvo o num de promotores
int preencheLista(char * filename){
    FILE *f;Promocao prom;
    char nomePromotor[50];
    f= fopen(filename,"rt");
    int count=0;int num=0;
    if(f == NULL) {
        printf("Erro ao abrir o ficheiro");
        funcSair();
    }else{
        while (fscanf(f,"%s",nomePromotor) != EOF){
            strcpy(listaPromotores[count],nomePromotor);
            count++;
            num++;
        }
        if(count > 10)
            return 10;
        do{
            strcpy(listaPromotores[count],"-");

            count++;
        } while (count < 10);

    }
    return num;
}

void mostraLista(){
    for(int i = 0;i<10;i++){
        printf("Promotor num %d: %s\n",i+1,listaPromotores[i]);
    }
}

void preencheListaItems(char *filename){
    Item item;
    FILE *f;
    int i=0,temp;
    //limpar lista
    for(int i = 0;i<30;i++)
        for(int j = 0;j<2;j++)
            listaItems[i][j]=0;

    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        fscanf(f,"%d",&temp);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            listaItems[i][0] = item.id;
            listaItems[i][1] = item.duracao + tempoInicial;
            i++;
        }
    }
    fclose(f);
}

void mostraListaItens(){
    for(int i = 0;i<30;i++)
        if(listaItems[i][0] != 0)
            printf("Linha %d-> %d | tempo -> %d | tempo real -> %d\n",
                   i+1,listaItems[i][0],listaItems[i][1],listaItems[i][1] - tempoInicial);
}

void removeUserFromList(int pid)
{
    int index = -1;
    // procura o user
    for (int i = 0; i < nUsers; i++)
    {
        if (ListaClientes[i].pid == pid)
        {
            index = i;
            break;
        }
    }

    // encontou o user
    if (index != -1)
    {
        // puxa todos um para tras
        int i;
        for (i = index; i < nUsers - 1; i++)
        {
            ListaClientes[i] = ListaClientes[i + 1];
        }

        // Decrement the user counter
        nUsers--;
        if(nUsers <= 20){
            ListaClientes[i].pid = -1;
            strcpy(ListaClientes[i].nome," ");
        }
    }

}

void atualizaTempoItens(char *filename){
    //tempo = tempo item - tempo
    //printf("Tempo atualizado -> %d",listaItems[0][1] - tempo);
    //mostraListaItens();
    Item item;
    FILE *f;
    int i,count;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        count = 2;
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            item.duracao = listaItems[count - 2][1] - tempo;
            //printf("Tempo atualizado -> %d",listaItems[0][1] - tempo);
            modifyLineInFile(filename,count,item);
            count++;
        }
    }


}

void verificaDuracao(){
    for(int i = 0;i<30 ; i++)
        if(listaItems[i][0] != 0){
            if(listaItems[i][1] <= tempo){
                //se tiver licitador retirar o item e reirar o dinheiro da conta
                Item item;
                FILE *f;
                int tem,count;
                f = fopen(getenv("FITEMS"),"rt");
                if(f == NULL){
                    fprintf(stderr,"Ficheiro nao encontrado\n");
                    funcSair();
                    return;
                }else{
                    count = 2;
                    fscanf(f,"%d",&tem);
                    while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
                        int flag = 0;
                        if(listaItems[i][0] == item.id && flag == 0){
                            //se tiver licitador retirar o item e reirar o dinheiro da conta
                            if(strcmp(item.usernameLicitador,"-") == 0){//n tem licitador
                                printf("O item %s de id %d foi devolvido ao cliente %s\n",item.nome,item.id,item.usernameVendedor);
                            }else{//tem licitador
                                printf("O item %s de id %d foi vendido ao cliente %s\n",item.nome,item.id,item.usernameLicitador);
                                loadUsersFile(getenv("FUSERS"));
                                updateUserBalance(item.usernameLicitador, getUserBalance(item.usernameLicitador)- item.valAtual);
                                saveUsersFile(getenv("FUSERS"));
                            }
                            deleteLineFromFile(getenv("FITEMS"),count);
                            preencheListaItems(getenv("FITEMS"));
                            //mostraListaItens();

                        }
                        count++;
                    }
                }
                fclose(f);

            }
        }
}

void listProdutos(char *filename){
    Item item;
    FILE *f;
    int i;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
        }
    }
}

void list(char *filename){
    Item item;
    FILE *f;
    int i;
    Resposta resposta;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
            resposta.pid = getpid();
            resposta.item = item;
            resposta.comando = 2;
            int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
            //printf("Nome do fifo %s\n",CLIENT_FIFO_FINAL);
            int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
            if (size2 == -1) {
                //printf("Nome do fifo %s\n",CLIENT_FIFO_FINAL );
                fprintf(stderr, "Erro a escrever");
                funcSair();
            }
            close(fdEnvio);
        }
    }
    fclose(f);
}

void listCat(char *filename,char *categoria){
    Item item;
    FILE *f;
    Resposta resposta;
    int i;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
            if(strcmp(categoria,item.categoria) == 0) {
                resposta.pid = getpid();
                resposta.item = item;
                resposta.comando = 3;
                int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
                int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
                if (size2 == -1) {
                    fprintf(stderr, "Erro a escrever");
                    funcSair();
                }
                close(fdEnvio);
            }
        }
    }
    fclose(f);
}

void listSel(char *filename,char *vendedor){
    Item item;
    FILE *f;
    int i;
    Resposta resposta;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
            if(strcmp(vendedor,item.usernameVendedor) == 0) {
                resposta.pid = getpid();
                resposta.item = item;
                resposta.comando = 4;
                int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
                int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
                if (size2 == -1) {
                    fprintf(stderr, "Erro a escrever");
                    funcSair();
                }
                close(fdEnvio);
            }
        }
    }
    fclose(f);
}

void listVal(char *filename,int val){
    Item item;
    FILE *f;
    int i;
    Resposta resposta;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
            if(item.valAtual <= val) {
                resposta.pid = getpid();
                resposta.item = item;
                resposta.comando = 5;
                int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
                int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
                if (size2 == -1) {
                    fprintf(stderr, "Erro a escrever");
                    funcSair();
                }
                close(fdEnvio);
            }
        }
    }
    fclose(f);
}

void listTemp(char *filename,int temp){
    Item item;
    int i;
    FILE *f;
    Resposta resposta;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){

            if(item.duracao <= temp) {
                printf("Valor %d\n",temp);
                printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);

                resposta.pid = getpid();
                resposta.item = item;
                resposta.comando = 6;
                int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
                int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
                if (size2 == -1) {
                    fprintf(stderr, "Erro a escrever");
                    funcSair();
                }
                close(fdEnvio);

            }
        }
    }
    fclose(f);
}

void devolveHora(){
    Resposta resposta;
    resposta.pid = getpid();
    resposta.num = tempo;
    resposta.comando = 7;
    int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
    int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
    if (size2 == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvio);
}

void consultaSaldo(char *nome){
    Resposta resposta;
    resposta.pid = getpid();
    resposta.comando = 9;
    loadUsersFile(getenv("FUSERS"));
    resposta.num = getUserBalance(nome);
    saveUsersFile(getenv("FUSERS"));
    int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
    int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
    if (size2 == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvio);
}

void adicionaSaldo(char *nome,int valor){
    Resposta resposta;
    resposta.pid = getpid();
    resposta.comando = 10;
    loadUsersFile(getenv("FUSERS"));
    updateUserBalance(nome, getUserBalance(nome)+valor);
    resposta.num = getUserBalance(nome);
    saveUsersFile(getenv("FUSERS"));
    int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
    int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
    if (size2 == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvio);
}

void addItemToFich(char *filename,Item item){
    FILE *f;
    Resposta resposta;int num=0;
    f = fopen(filename,"a");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        item.id = id++;
        fprintf(f,"\n%d %s %s %d %d %d %s %s",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
        num++;
    }
    fclose(f);
    resposta.comando = 0;
    resposta.item.id = id-1;

    int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
    int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
    if (size2 == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvio);

    for(int i = 0;i<30 ;i++){
        if(listaItems[i][0] == 0){
            listaItems[i][0] = item.id;
            listaItems[i][1] = item.duracao + tempoInicial;
            break;
        }
    }
    //mostraListaItens();

}

void fazLicitacao(char *filename,char *nome,int id, int valor){
    Item item;
    int i,nLinha=2;
    FILE *f;
    Resposta resposta;
    f = fopen(filename,"a+");
    int flag=0;
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            if(item.id == id) {
                flag=1;
                loadUsersFile(getenv("FUSERS"));
                if (valor < item.valAtual) {//ver se tem dinheiro
                    strcpy(resposta.item.categoria,"Valor inferior ao atual!");
                } else if (valor > getUserBalance(nome)) {//ver se o utilizador tem saldo suficiente
                    strcpy(resposta.item.categoria,"Dinheiro insuficiente na conta");
                } else if(valor >= item.valCompreJa){
                    strcpy(resposta.item.categoria,"Item adquirido!");
                    //remover linha do ficheiro
                    deleteLineFromFile(filename,nLinha);
                    preencheListaItems(filename);
                    //retirar o dinheiro da conta do utilizador
                    updateUserBalance(nome,getUserBalance("TESTE")-item.valCompreJa);
                    //adicionar á conta do vendedor
                    updateUserBalance(item.usernameVendedor,getUserBalance("TESTE")+item.valCompreJa);

                }else{
                    strcpy(resposta.item.categoria,"Licitacao efetuada!");
                    //updateUserBalance(nome,getUserBalance("TESTE")-valor);///Tiro o valor ja ou so se comprar?
                    //Atualizar o item antigo pelo item atualizado
                    item.valAtual = valor;
                    strcpy(item.usernameLicitador,nome);
                    //Substituir o item antigo pelo item atualizado
                    modifyLineInFile(filename,nLinha,item);
                }
                saveUsersFile(getenv("FUSERS"));
            }
            nLinha++;
        }
    }
    fclose(f);
    if(flag == 0)
        strcpy(resposta.item.categoria,"O item nao existe!");
    resposta.comando = 8;
    int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
    int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
    if (size2 == -1) {
        fprintf(stderr, "Erro a escrever");
        funcSair();
    }
    close(fdEnvio);

}

void getId(char *filename){
    Item item;
    FILE *f;
    int i;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        fscanf(f,"%d",&i);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
        }
        if(item.id > 30 || item.id <= 0)
            id = 1;
        else
            id = ++item.id;
    }
    fclose(f);
}

void leFichItens(char *filename) {
    Item item;
    FILE *f;
    int i;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        fscanf(f,"%d",&tempo);
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
        }
    }
    fclose(f);
}

void gravaTempo(char *filename){
    //printf("[Fazer] Atualizar o tempo...\n");
    //printf("Tempo->%d",tempo);
    modifyLineInFileInt(filename,1,tempo);
}

void initLista(){
    User a;
    a.pid = -1;
    strcpy(a.nome," ");
    for(int i=0;i<20;i++){
        ListaClientes[i] = a;
    }
    for(int i=0;i<20;i++){
        listaCliAtivos[i][0]=-1;
        listaCliAtivos[i][1]=-1;
    }
}

void printLista(){
    printf("Lista de Utilizadores:\n");
    for(int i=0;i<20;i++){
        if(ListaClientes[i].pid != -1)
            printf("%s\n",ListaClientes[i].nome);
    }
}

void mostraL(){
    for(int i= 0;i<20;i++){
        if(listaCliAtivos[i][0] != -1){
            printf("Cli %d tem %d sec restantes\n",listaCliAtivos[i][0],listaCliAtivos[i][1]);

        }
    }
}

void adicionaUserLista(User a){
    int flag = 0,flag2 = 0;
    //Verificar se o utlizadores ja esta na lista
    for(int i=0;i<20;i++){
        if(strcmp(ListaClientes[i].nome,a.nome) == 0){
            printf("Utilizador %s ja esta logado!\n",a.nome);
            flag = 1;
        }
    }
    if(nUsers >= 20)
        flag = 1;
    if(flag == 0){
        ListaClientes[nUsers++] = a;
        printf("O utilizador [%s] esta ligado!\n",a.nome);
        for(int i=0;i<20;i++) {
            if(listaCliAtivos[i][0] == -1 && flag2 == 0) {
                listaCliAtivos[i][0] = a.pid;
                listaCliAtivos[i][1] = tempoRestante + 1;
                flag2 = 1;
            }
        }
    }
}

void atualizaListaCliAtivosMenos(){
    for(int i= 0;i<20;i++){
        if(listaCliAtivos[i][0] != -1){
            listaCliAtivos[i][1] -= 1;
            if(listaCliAtivos[i][1] == 0){
                printf("O cliente [%d] nao sa sinal de vida a %d segundos...\n ",listaCliAtivos[i][0],tempoRestante);
                removeUserFromList(listaCliAtivos[i][0]); // remove da lista dos utilizadores
                //printLista();
                listaCliAtivos[i][0] = -1;
                listaCliAtivos[i][1] = -1;
                //mostraL();
            }
        }
    }
}

void atualizaListaCliAtivosMais(int pid){
    for(int i= 0;i<20;i++){
        if(listaCliAtivos[i][0] != -1){
            if(listaCliAtivos[i][0] == pid) {
                listaCliAtivos[i][1] = tempoRestante+1;
            }

        }
    }
}

void listaPAtivos(){
    printf("Lista de promotores ativos:\n");
    for(int i =0;i<10;i++){
        if(strcmp("-",listaPromotores[i]) !=0){
            printf("%s\n",listaPromotores[i]);
        }
    }
}

//Funcao que vai informar todos os clientes que o sevidor foi enceraado
void kickUser(char nome[]){
    int flag = 0,index=-1;//ativa se existir o utilizador
    for(int i=0;i<20;i++){
        if(strcmp(ListaClientes[i].nome,nome) == 0){
            flag = 1;//o utilizador esta logado
            index = i;
        }
    }
    if(flag==1){
        union sigval info;
        sigqueue(ListaClientes[index].pid,SIGUSR2,info);
        printf("A expulsar o utilizador...\n");
    }else{
        printf("Utilizador desconhecido\n");
    }
}

Promocao lancaPromotor(char *nomePromotor) {
    Promocao prom;
    char frase[100],categoria[50];
    int res,canal[2],bytes=0,desconto,duracao;
    if(pipe(canal) == -1){
        fprintf(stderr,"Erro ao criar o pipe");
        funcSair();
    }
    res = fork();
    if(res == -1)
        funcSair();
    if(res == 0){ //Processo filho
        close(1);           //redirecionamento
        dup(canal[1]);
        close(canal[0]);
        close(canal[1]);
        if(execl(nomePromotor,nomePromotor,NULL) == -1){
            fprintf(stderr,"ERRO");
        }
        exit(2);
    }else{//Processo pai
        bytes = read(canal[0],frase, sizeof (frase));
        if(bytes == -1){
            fprintf(stderr,"Erro na leitura");
            funcSair();
        }
        sscanf(frase,"%s %d %d",categoria,&desconto,&duracao);
        strcpy(prom.categoria,categoria);
        prom.desconto = desconto;
        prom.duracao = duracao;
    }
    close(canal[1]);
    close(canal[0]);


    union sigval a;
    if(sigqueue(res,SIGUSR1,a) == -1)
        fprintf(stderr,"Erro ao enviar o sinal");
    wait(&res);

    return prom;
}

void handle_sig(int sig,siginfo_t *info,void *old){
    if(sig==SIGUSR1) {
        printf("Cliente com o PID [%d] ainda esta vivo\n", info->si_value.sival_int);
    }
    atualizaListaCliAtivosMais(info->si_value.sival_int);

}

void handle_sig_sair(int sig,siginfo_t *info,void *old){
    printf("O cliente com o PID [%d]\n",info->si_value.sival_int);
    removeUserFromList(info->si_value.sival_int);
    //printLista();
}

void *temporizador(void *dados){
    TDATA *pd = dados;
    do{
        sleep(1);//relogio do servidor(avancar um segundo)-decrementer um segundo ao leilao ativo;ter um int hora sempre a incrementar->fazer com o sleep
        if(pthread_mutex_lock(pd->ptrinco) != 0){
            printf("Erro com o lock\n");
            funcSair();
        }
        tempo++;
        if(pthread_mutex_unlock(pd->ptrinco) != 0){
            printf("Erro com o lock\n");
            funcSair();
        }
        //printf("Tempo %ds\n", tempo);
        verificaDuracao();
        atualizaListaCliAtivosMenos();
        ///verificaFimPromocao(); ->Funciona incorretamente
    }while(pd->continua);
    pthread_exit(NULL);
}

void *trata_promos(void *dados){
    TDATA *pd = dados;
    //TODO:Guardar o tempo e para ser inicializado depois
    do{
        Promocao prom;
        int count = 0;
        do {
            if (count == pd->numProm && strcmp(listaPromotores[count],"-") != 0 ) {
                prom = lancaPromotor(listaPromotores[count]);
                //printf("\n%s\n",listaPromotores[count]);
                if(pthread_mutex_lock(pd->ptrinco) != 0){
                    printf("Erro com o lock\n");
                    funcSair();
                }
                //trataPromocao(prom);
                if(pthread_mutex_unlock(pd->ptrinco) != 0){
                    printf("Erro com o lock\n");
                    funcSair();
                }
                /*
                printf("Promotor: %s\n", listaPromotores[count]);
                printf("Categoria: %s\n", prom.categoria);
                printf("Desconto: %d\n", prom.desconto);
                printf("Duracao: %d\n\n", prom.duracao);
                */
            }
            count++;
        }while (count < 10);

    }while(pd->continua);

    pthread_exit(NULL);
}

void criaThreadsPromotores(){
    int num = preencheLista(getenv("FPROMOTERS"));
    nPromotores = num;
    //printf("Num de promotores -> %d",num);
    //mostraLista();
    for(int i = 1; i< num +1 ;i++){
        data[i].continua = 1;
        data[i].ptrinco = &trinco;
        data[i].numProm = i - 1;
        if(pthread_create(&tid[i],NULL, trata_promos,&data[i]) != 0){
            printf("Erro ao criar a thread\n");
            funcSair();
        }

    }

}

void fechaThreadsPromotores(){
    for(int i=1;i<nPromotores+1;i++){
        data[i].continua=0;

        if(pthread_cancel(tid[i]) != 0){
            printf("Erro com o pthread_cancel\n");
            funcSair();
        }
        pthread_cancel(tid[i]);
        if(pthread_join(tid[i],NULL) != 0){
            printf("Erro com o pthread_join\n");
            funcSair();
        }

    }

}

void paraPromotor(char *nomePromotor){

    for(int i = 0;i<10;i++){
        if (strcmp(nomePromotor,listaPromotores[i]) ==0){
            pthread_cancel(tid[i+1]);
            strcpy(listaPromotores[i],"-");
            /*
            if (kill(pid, SIGKILL) == -1) {
                perror("Error killing process");
            }
             */
            data[i].continua=0;

            pthread_cancel(tid[i]);
            pthread_join(tid[i],NULL);
        }
    }

}

void pedeComando(char *str,int numArgumento ){
    Promocao prom;
    //printf("Num de Argumetos: %d\n", numArgumento);
    char *token = strtok(str, " ");

    if (strcmp(token, "users") == 0) {///FEITO
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else
            printLista();
    } else if (strcmp(token, "list") == 0) {    ///FEITO
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else
            listProdutos(getenv("FITEMS"));
    } else if (strcmp(token, "kick") == 0) {
        if (numArgumento != 2)
            printf("Nao Valido\n");
        else {
            token = strtok(NULL, " ");
            kickUser(token);
        }
    } else if (strcmp(token,"prom") == 0) {    ///Feito
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else
            listaPAtivos();
    } else if (strcmp(token,"reprom") == 0) {  ///Feito
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else{
            fechaThreadsPromotores();
            criaThreadsPromotores();

            //reprom(getenv("FPROMOTERS"));
        }
    } else if (strcmp(token,"cancel") == 0) { ///
        if (numArgumento != 2)
            printf("Nao Valido\n");
        else {
            token = strtok(NULL, " ");
            //printf("Nome do executavel: %s\n", token);
            paraPromotor(token);
        }
    } else if (strcmp(token,"close") == 0) {
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else {
            gravaTempo(getenv("FITEMS"));
            atualizaTempoItens(getenv("FITEMS"));
        }
    }else {
        printf("Comando nao Valido\n");
    }
}

void init(){
    initLista();
    initItensPromocaoPromocao();
    leFichItens(getenv("FITEMS"));
    tempoInicial = tempo;
    getId(getenv("FITEMS"));//deixa o id com o valor correto
    preencheLista(getenv("FPROMOTERS"));
    preencheListaItems(getenv("FITEMS"));
    tempoRestante = atoi(getenv("HEARTBEAT"));
}

int main(int argc,char *argv[],char *envp[]) {
    char str[128];
    init();
    Comando a;
    fd_set fds;
    //pthread_t tid[11];

    //de n em n segundos
    struct sigaction sa;
    sa.sa_sigaction = handle_sig;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    //sinal(quando um cliente sai)
    struct sigaction sa2;
    sa2.sa_sigaction = handle_sig_sair;
    sa2.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sa2, NULL);


    //sinal(quando o backend termina á força (ctrl + c))
    struct sigaction sa3;
    sa3.sa_sigaction = funcSair;
    sa3.sa_flags = SA_SIGINFO;
    sigaction(SIGINT,&sa3,NULL);


    if(mkfifo(FIFO_SERVIDOR, 0666) == -1){
        if(errno == EEXIST){
            printf("Servidor em execução ou fifo ja existe");
        }
        printf("Erro abrir fifo");
        exit(1);
    }
    printf("Bem vindo Administrador\n");
    int fdRecebe = open(FIFO_SERVIDOR, O_RDWR);
    if(fdRecebe == -1){
        printf("Erro ao abrir o fifo");
        funcSair();
    }

    if(pthread_mutex_init(&trinco,NULL) != 0){
        printf("Erro no pthread_mutex_init\n");
        funcSair();
    }
    //T1->temporizador
    data[0].continua = 1;
    data[0].ptrinco = &trinco;
    if(pthread_create(&tid[0],NULL, temporizador,&data[0]) != 0){
        printf("Erro ao criar a thread\n");
        funcSair();
    }

    //chamar funcao que cria as threads dos promotores
    criaThreadsPromotores();

    do{
        //T0
        printf("Comando:\n");
        fflush(stdout);
        FD_ZERO(&fds);
        FD_SET(0,&fds);//tomar atenção ao 0(scanf)
        FD_SET(fdRecebe,&fds);//tomar atenção ao fdReceber -> read
        int res = select(fdRecebe+1,&fds,NULL,NULL,NULL);

        if(res >0 && FD_ISSET(0,&fds)){
            fflush(stdout);
            fgets(str, 128, stdin);
            str[strcspn(str, "\n")] = 0;
            int numArgumento = numArgumentos(str);

            pedeComando(str,numArgumento);
        }else if (res >0 && FD_ISSET(fdRecebe,&fds)) {
            ///Le a informacao do cliente
            int size = read(fdRecebe, &a, sizeof(Comando));
            if (size == -1) {
                fprintf(stderr, "Erro a escrever");
                funcSair();
            }
            sprintf(CLIENT_FIFO_FINAL, FIFO_CLIENTE, a.user.pid);
            if (size > 0) {
                if(a.comando == 0){//Verificar login
                    //printf("User pid1 %d",a.user.pid);
                    ///Tratar info
                    Resposta resposta;
                    loadUsersFile(getenv("FUSERS"));
                    resposta.comando = 1;
                    int aux = isUserValid(a.user.nome, a.user.password);
                    if (aux == -1) {
                        resposta.num = 2;
                        fprintf(stderr, "Erro(funcao isUserValid)!");
                        funcSair();
                    } else if (aux == 0) {
                        resposta.num = 3;
                        printf("Utilizador nao existe ou password errada!\n");
                    }else if(existeUserLista(a.user.nome)){
                        resposta.num = 4;
                    } else {
                        resposta.num = 1;
                        adicionaUserLista(a.user);
                        //printLista();
                    }
                    ///Enviar resposta ao cliente
                    resposta.pid = getpid();
                    int fdEnvio = open(CLIENT_FIFO_FINAL, O_WRONLY);
                    //printf("NOmde do fifo1 %s\n",CLIENT_FIFO_FINAL);
                    int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
                    if (size2 == -1) {
                        fprintf(stderr, "Erro a escrever");
                        funcSair();
                    }
                    close(fdEnvio);
                } else if(a.comando==1){//colocar item á venda
                    printf("Recebi um %s do utilizador %s\n",a.item.nome,a.item.usernameVendedor);
                    addItemToFich(getenv("FITEMS"),a.item);

                }else if(a.comando == 2) {
                    list(getenv("FITEMS"));
                }else if(a.comando == 3){
                    listCat(getenv("FITEMS"),a.item.categoria);
                }else if(a.comando == 4) {
                    listSel(getenv("FITEMS"), a.item.usernameVendedor);
                }else if(a.comando == 5){
                    listVal(getenv("FITEMS"), a.item.valAtual);
                }else if(a.comando == 6){
                    listTemp(getenv("FITEMS"), a.item.duracao);
                }else if(a.comando == 7){
                    devolveHora();
                }else if(a.comando == 8){
                    fazLicitacao(getenv("FITEMS"),a.user.nome,a.item.id,a.item.valAtual);
                }else if(a.comando == 9){
                    consultaSaldo(a.user.nome);
                }else if(a.comando == 10){
                    adicionaSaldo(a.user.nome,a.item.valAtual);
                }

            } else {
                fprintf(stderr,"Erro na leitura");
                funcSair();
            }
        }
    } while (strcmp(str,"close")!=0);

    data[0].continua=0;
    if(pthread_cancel(tid[0]) != 0){
        printf("Erro com o pthread_cancel\n");
        funcSair();
    }
    if(pthread_join(tid[0],NULL) != 0){
        printf("Erro com o pthread_join\n");
        funcSair();
    }

    fechaThreadsPromotores();

    if(pthread_mutex_destroy(&trinco) != 0){
        printf("Erro com o mutex_destry\n");
        funcSair();
    }

    close(fdRecebe);

    funcSair();
    printf("Fechou\n");
    return 0;
}