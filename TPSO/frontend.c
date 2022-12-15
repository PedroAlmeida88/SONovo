#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "utils.h"
char CLIENT_FIFO_FINAL[100];
int servPid=-1;

void pedeComandos(){
    while(1){
        int numArgumento;
        char buffer[50],nomeItem[50],categoria[50],aux[128];
        int precoBase,precoCompreJa,duracao,id,valor;
        char str[128];

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
                leFichItens(getenv("FITEMS"));
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
}

void handle_alarm(){
    //printf("\nEnviei um sinal ao servidor\n");
    union sigval info;
    struct sigaction sa;
    info.sival_int = getpid();  //enviar o PID ao backend
    sigqueue(servPid,SIGUSR1,info);
    alarm(5);
}

void handle_sig_servidorEncerrou(){
    printf("\nO servidor foi encerrado...\n");
    printf("A terminar...\n");
    unlink(CLIENT_FIFO_FINAL);
    exit(1);
}

void handle_sig_servidorExpulsou(){
    printf("\nVoce foi expulso pelo servidor...\n A sair...\n");
    unlink(CLIENT_FIFO_FINAL);
    exit(1);
}

void funcSinalSair(){
    printf("\nA encerrar o cliente...\n");
    unlink(CLIENT_FIFO_FINAL);
    exit(1);
}

int main(int argc, char **argv, char **envp){
    if(argc != 3) {
        printf("[ERRO] Numero invalido de argumentos");
        exit(1);
    }

    User a;
    strcpy(a.nome,argv[1]);
    strcpy(a.password,argv[2]);
    a.pid = getpid();

    ///Garantir que o servidor está ativo
    if(access(FIFO_SERVIDOR, F_OK) != 0){
        printf("O servidor nao esta ativo!!!(Pipe nao exite)");
        exit(1);
    }

    sprintf(CLIENT_FIFO_FINAL, FIFO_CLIENTE, getpid());
    if(mkfifo(CLIENT_FIFO_FINAL,0666) == -1){
        if(errno == EEXIST){
            printf("Fifo já está a correr");
        }
        printf("Erro abrir fifo");
        exit(1);
    }

    ///Envia o nome,password e PID do cliente do frontend para o backend
    int fdEnvia = open(FIFO_SERVIDOR,O_WRONLY);
    if(fdEnvia == -1){
        printf("Erro ao abrir o fifo");
    }
    int size = write(fdEnvia, &a, sizeof(User));
    close(fdEnvia);
    ///Receber a resposta do servidor
    int fdResposta = open(CLIENT_FIFO_FINAL, O_RDONLY);
    if(fdResposta == -1){
        printf("Erro ao abrir o fifo");
    }
    Resposta resposta;
    int size2 = read(fdResposta, &resposta, sizeof(Resposta));
    if(size2 == -1){
        fprintf(stderr,"Erro na leitura");
        exit(1);
    }

    if(resposta.num == 1){
        printf("Bem vindo %s!\n",argv[1]);
    }else{
        printf("Utilizador desconecido ou password errada!\n");
        exit(1);
    }
    close(fdResposta);
    unlink(CLIENT_FIFO_FINAL);

    //caso o servidor seja encerrado
    struct sigaction sa3;
    sa3.sa_sigaction = handle_sig_servidorEncerrou;
    sa3.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa3, NULL);

    //caso o cliente seja expulso pelo servidor
    struct sigaction sa4;
    sa4.sa_sigaction = handle_sig_servidorExpulsou;
    sa4.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR2, &sa4, NULL);

    //sinal(quando o cliente termina á força (ctrl + c))
    struct sigaction sa2;
    sa2.sa_sigaction = funcSinalSair;
    sa2.sa_flags = SA_SIGINFO;
    sigaction(SIGINT,&sa2,NULL);
    servPid = resposta.pid;


    struct sigaction saAlarm;
    saAlarm.sa_handler = handle_alarm;
    saAlarm.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGALRM,&saAlarm,NULL);
    //alarm(atoi(getenv("HEARTBEAT")));
    alarm(5);

    pedeComandos();


    printf("A avisar o servidor que irei sair\n");
    union sigval info;
    struct sigaction sa;
    info.sival_int = getpid();  //enviar o PID ao backend
    sigqueue(resposta.pid,SIGUSR2,info);


    printf("Adeus\n");


    exit(0);
}
