#include "utils.h"
#include "backend.h"
#include "users_lib.h"
char CLIENT_FIFO_FINAL[100];
User ListaClientes[20];
int nUsers=0;
int tempo=0;
int tempoInicial;
int id=1;
char listaPromotores[10][50];
int listaItems[30][2];//pode 30 id e 30 duracoes
pthread_t tid[11];

typedef struct {
    int continua;
    int numProm;
    pthread_mutex_t *ptrinco;
}TDATA;

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
    printf("\nA encerrar o servidor...\n");
    informaClienteFim();
    unlink(FIFO_SERVIDOR);
    exit(1);
}

void preencheLista(char * filename){
    FILE *f;Promocao prom;
    char nomePromotor[50];
    f= fopen(filename,"rt");
    int count=0;
    if(f == NULL) {
        printf("Erro ao abrir o ficheiro");
        return;
    }else{
        while (fscanf(f,"%s",nomePromotor) != EOF){
            strcpy(listaPromotores[count],nomePromotor);
            count++;
        }
        if(count > 10)
            return;
        do{
            strcpy(listaPromotores[count],"-");

            count++;
        } while (count < 10);

    }
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
            printf("Linha %d-> %d | %d\n",i+1,listaItems[i][0],listaItems[i][1]);
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
                        if(listaItems[i][0] == item.id){
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
                            mostraListaItens();

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
            printf("Nome do fifo %s\n",CLIENT_FIFO_FINAL);
            int size2 = write(fdEnvio, &resposta, sizeof(Resposta));
            if (size2 == -1) {
                printf("Nome do fifo %s\n",CLIENT_FIFO_FINAL );
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

void listTemp(char *filename,int val){
    Item item;
    int i;
    FILE *f;
    Resposta resposta;
    f = fopen(filename,"rt");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        while (fscanf(f,"%d %s %s %d %d %d %s %s",&item.id,item.nome,item.categoria,&item.valAtual,&item.valCompreJa,&item.duracao,item.usernameVendedor,item.usernameLicitador) != EOF){
            fscanf(f,"%d",&i);
            if(item.duracao <= val) {
                //printf("Valor %d\n",val);
                //printf("%d %s %s %d %d %d %s %s\n",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);

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
    Resposta resposta;
    f = fopen(filename,"a");
    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        return;
    }else{
        item.id = id++;
        fprintf(f,"\n%d %s %s %d %d %d %s %s",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
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
                    //retirar o dinheiro da conta do utilizador
                    updateUserBalance(nome,getUserBalance("TESTE")-item.valCompreJa);
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
            //TODO:Tratar informacao
        }
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
    printf("[Fazer] Atualizar o tempo...\n");
/*    FILE *f;
    f = fopen(filename,"wt");


    if(f == NULL){
        fprintf(stderr,"Ficheiro nao encontrado\n");
        funcSair();
        return;
    }else{
        //atualiza a primeira linha do ficheiro
        printf("[Fazer] Atualizar o tempo...\n");
    }
    fclose(f);
*/
}

void initLista(){
    User a;
    a.pid = -1;
    strcpy(a.nome," ");
    for(int i=0;i<20;i++){
        ListaClientes[i] = a;
    }
}

void printLista(){
    printf("Lista de Utilizadores:\n");
    for(int i=0;i<20;i++){
        if(ListaClientes[i].pid != -1)
            printf("%s\n",ListaClientes[i].nome);
    }
}

void adicionaUserLista(User a){
    int flag = 0;
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
    }
}


void paraPromotor(char *nomePromotor){
    //matar thread e processo?
    int flag=0;
    for(int i = 0;i<10;i++){
        if (strcmp(nomePromotor,listaPromotores[i]) ==0){
            pthread_cancel(tid[i+1]);
            strcpy(listaPromotores[i],"-");
            //kill(,SIGKILL);
            flag=1;
        }
    }
    //if(flag == 0)
    //    printf("Promotor desconhecido!\n");
}
/*
void executaPromotores(char* filename){
    FILE *f;Promocao prom;
    char nomePromotor[50];
    f= fopen(filename,"rt");
    if(f == NULL) {
        printf("Erro ao abrir o ficheiro");
        return;
    }else{
        while (fscanf(f,"%s",nomePromotor) != EOF){
            prom = lancaPromotor(nomePromotor);
            printf("Promotor: %s\n",nomePromotor);
            printf("Categoria: %s\n",prom.categoria);
            printf("Desconto: %d\n",prom.desconto);
            printf("Duracao: %d\n\n",prom.duracao);
        }
    }
}
*/
void listaPAtivos(){
    printf("Lista de promotores ativos:\n");
    for(int i =0;i<10;i++){
        if(strcmp("-",listaPromotores[i]) !=0){
            printf("%s\n",listaPromotores[i]);
        }
    }
}

void reprom(char *filename){
    char listaPromotoresAux[10][50];
    for (int i = 0; i < 10; i++) {
        strcpy(listaPromotoresAux[i],listaPromotores[i]);
    }//lista auxiliar tem a lista de promotores antigos
    preencheLista(filename);//lista original é atualizada
    int flag;
    for(int i = 0;i < 10; i++){
        flag=0;
        for(int j = 0;j < 10; j++){
            //compara lista velha com a nova
            if(strcmp(listaPromotoresAux[i],listaPromotores[j]) == 0){
                //printf("Existe o promotor %s na lista",listaPromotores[i]);
                flag = 1;//promotor existe
            }
        }
        if (flag==0){
            printf("O promotor[%s] nao existe no novo ficheiro.A eliminar...\n",listaPromotores[i]);
            //existia na velha, nao existe na nova->matar.
            paraPromotor(listaPromotoresAux[i]);
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
        else
            reprom(getenv("FPROMOTERS"));
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
            informaClienteFim();
            gravaTempo(getenv("FITEMS"));
        }
    } else if(strcmp(token,"promotores") == 0){
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else {
            //executaPromotores(getenv("FPROMOTERS"));
        }
    } else if(strcmp(token,"utilizadores") == 0){
        if (numArgumento != 1)
            printf("Nao Valido\n");
        else {
            loadUsersFile(getenv("FUSERS"));
            printf("Saldo do utilizador TESTE: %d\n", getUserBalance("TESTE"));
            updateUserBalance("TESTE", getUserBalance("TESTE") - 1);
            saveUsersFile(getenv("FUSERS"));
        }
    } else {
        printf("Comando nao Valido\n");
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
}

void handle_sig_sair(int sig,siginfo_t *info,void *old){
    printf("O cliente com o PID [%d] saiu\n",info->si_value.sival_int);
    ///TODO:Retirar o cliente da lista e reordenar a lista
}

void *temporizador(void *dados){
    TDATA *pd = dados;
    //TODO:Guardar o tempo e para ser inicializado depois
    do{
        sleep(1);//relogio do servidor(avancar um segundo)-decrementer um segundo ao leilao ativo;ter um int hora sempre a incrementar->fazer com o sleep
        pthread_mutex_lock(pd->ptrinco);
        tempo++;
        printf("Tempo %ds\n", tempo);
        pthread_mutex_unlock(pd->ptrinco);
        verificaDuracao();
    }while(pd->continua);
    pthread_exit(NULL);
}

void *trata_promos(void *dados){
    TDATA *pd = dados;
    //TODO:Guardar o tempo e para ser inicializado depois
    do{
        //pthread_mutex_lock(pd->ptrinco);
        Promocao prom;
        int count = 0;
        do {
            if (count == pd->numProm && strcmp(listaPromotores[count],"-") != 0 ) {
                pthread_mutex_lock(pd->ptrinco);
                printf("\n%s\n",listaPromotores[count]);
                prom = lancaPromotor(listaPromotores[count]);
                pthread_mutex_unlock(pd->ptrinco);
                printf("Promotor: %s\n", listaPromotores[count]);
                printf("Categoria: %s\n", prom.categoria);
                printf("Desconto: %d\n", prom.desconto);
                printf("Duracao: %d\n\n", prom.duracao);

            }
            count++;
        }while (count < 10);

    }while(pd->continua);

    pthread_exit(NULL);
}

void init(){
    initLista();
    leFichItens(getenv("FITEMS"));
    tempoInicial = tempo;
    getId(getenv("FITEMS"));//deixa o id com o valor correto
    preencheLista(getenv("FPROMOTERS"));
    preencheListaItems(getenv("FITEMS"));
    mostraListaItens();

}


int main(int argc,char *argv[],char *envp[]) {
    char str[128];
    init();
    Comando a;
    fd_set fds;
    pthread_mutex_t trinco;
    //pthread_t tid[11];
    TDATA data[11]; //1+10 timer + até 10 promotores
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


    //TODO:ACCESS
    if(mkfifo(FIFO_SERVIDOR, 0666) == -1){
        if(errno == EEXIST){
            printf("Servidor em execução ou fifo ja existe");
        }
        printf("Erro abrir fifo");
        funcSair();
    }
    printf("Bem vindo Administrador\n");
    int fdRecebe = open(FIFO_SERVIDOR, O_RDWR);
    if(fdRecebe == -1){
        printf("Erro ao abrir o fifo");
        funcSair();
    }

    pthread_mutex_init(&trinco,NULL);
    //T1->temporizador
    data[0].continua = 1;
    data[0].ptrinco = &trinco;
    pthread_create(&tid[0],NULL, temporizador,&data[0]);
    //T2->promotores
    /*
    data[1].continua = 1;
    data[1].ptrinco = &trinco;
    pthread_create(&tid[1],NULL, trata_promos,&data[1]);
    */
    FILE *f;
    f= fopen(getenv("FPROMOTERS"),"rt");
    if(f == NULL) {
        printf("Erro ao abrir o ficheiro");
        funcSair();
    }

/*
    for(int i=1;i < 10 + 1 ; i++){
        data[i].continua = 1;
        data[i].ptrinco = &trinco;
        data[i].numProm = i - 1;
        pthread_create(&tid[i],NULL, trata_promos,&data[i]);
        //printf("Id da thread %d -> %d",i,tid[i]);

    }
*/

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
            sprintf(CLIENT_FIFO_FINAL, FIFO_CLIENTE, a.user.pid);
            if (size > 0) {
                if(a.comando == 0){//Verificar login
                    printf("User pid1 %d",a.user.pid);
                    ///Tratar info
                    Resposta resposta;
                    loadUsersFile(getenv("FUSERS"));
                    resposta.comando = 1;
                    int aux = isUserValid(a.user.nome, a.user.password);
                    if (aux == -1) {
                        resposta.num = 0;
                        fprintf(stderr, "Erro(funcao isUserValid)!");
                        funcSair();
                    } else if (aux == 0) {
                        resposta.num = 0;
                        printf("Utilizador nao existe ou password errada!\n");
                    } else {
                        resposta.num = 1;
                        adicionaUserLista(a.user);
                        printLista();
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
    pthread_cancel(tid[0]);
    pthread_join(tid[0],NULL);
/*
    for(int i=1;i<10+1;i++){
        data[i].continua=0;
        pthread_join(tid[i],NULL);
    }
*/
    pthread_mutex_destroy(&trinco);
    close(fdRecebe);
    unlink(FIFO_SERVIDOR);
    printf("A avisar os clientes que ira fechar\n");
    printf("Fechou\n");
    return 0;

}