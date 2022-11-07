//#include "utils.h"
#include <stdio.h>
#include <string.h>

int main(){
    char **string;
    char str[] = "HELLO WORLD AWJDB JAWD OIAWHD IOAWD";
    separaString(str, string);
    return 0;
}

void separaString(char str[], char** splitedString){
    char *token = strtok(str, " ");
    int i = 0;
    while(token != NULL){
        splitedString[i] = token;
        printf("%d - %s", i, splitedString[i]);
        i++;
        token = strtok(NULL, " ");
    }
}