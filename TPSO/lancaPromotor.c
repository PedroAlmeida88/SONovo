#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    char frase[100];
    int res,canal[2];
    pipe(canal);
    res = fork();
    if(res == -1)
        return 1;
    if(res == 0){ //Processo filho
        close(1);           //redirecionamento
        dup(canal[1]);
        close(canal[0]);
        close(canal[1]);

        if(execl("promotorFake","promotorFake",NULL) == -1){
            fprintf(stderr,"ERRO");
        }
        exit(2);
    }else{//Processo pai
        //ler resultado do filho
        read(canal[0],frase, sizeof (frase));
        printf("Li: %s",frase);
    }

    close(canal[1]);
    close(canal[0]);


    return 0;
}