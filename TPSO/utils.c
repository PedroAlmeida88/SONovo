#include "utils.h"

//Cada linha do ficheiro descreve um item à venda, e os campos em cada linha são separados por um espaço.
//Cada linha deve conter, por esta ordem:
//identificador único do item, nome do item (uma palavra), categoria (uma palavra), valor atual (valor inicial ou valor
//da licitação mais elevada), valor “compre já”, duração do leilão (= tempo restante), username do utilizador que
//vende, username do utilizador que licitou o valor mais elevado (ou “-” se não tiver ainda sido licitado).



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

void modifyLineInFile(char *filename, int lineNumber, Item item)
{
    // Open the file for reading and writing
    FILE *file = fopen(filename, "r+");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    // Go to the beginning of the file
    rewind(file);

    // Read each line of the file and write it to the same file,
    // replacing the specified line with the new line
    char line[256];
    int currentLine = 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (currentLine == lineNumber)
        {
            // Write the new line over the old line
            fseek(file, -strlen(line), SEEK_CUR);
            fprintf(file,"%d %s %s %d %d %d %s %s",item.id,item.nome,item.categoria,item.valAtual,item.valCompreJa,item.duracao,item.usernameVendedor,item.usernameLicitador);
        }
        currentLine++;
    }

    // Close the file
    fclose(file);
}
void modifyLineInFileInt(char *filename, int lineNumber, int i)
{
    // Open the file for reading and writing
    FILE *file = fopen(filename, "r+");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    // Go to the beginning of the file
    rewind(file);

    // Read each line of the file and write it to the same file,
    // replacing the specified line with the new line
    char line[256];
    int currentLine = 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (currentLine == lineNumber)
        {
            // Write the new line over the old line
            fseek(file, -strlen(line), SEEK_CUR);
            fprintf(file,"%d",i);
        }
        currentLine++;
    }

    // Close the file
    fclose(file);
}
void deleteLineFromFile(char *filename, int lineNumber)
{
    // Open the file for reading
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    // Create a temporary file to hold the modified contents
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        printf("Error creating temporary file!\n");
        return;
    }

    // Read each line of the file and write it to the temporary file,
    // skipping the specified line
    char line[256];
    int currentLine = 1;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (currentLine != lineNumber)
        {
            fputs(line, tempFile);
        }
        currentLine++;
    }

    // Close both files
    fclose(file);
    fclose(tempFile);

    // Delete the original file and rename the temporary file to the original name
    remove(filename);
    rename("temp.txt", filename);
}