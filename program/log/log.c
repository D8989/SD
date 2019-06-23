#include "log.h"

#include <stdio.h>
#include <string.h>

void escrever(const char* arqName, int id, int time, const char* msg)
{
    FILE *arq = NULL;
    arq = fopen(arqName, "a");
    fprintf(arq, "Processo: %d -> Tempo: %d -> %s\n", id, time, msg);
    fclose(arq);
}

void concat(char* newMsg, unsigned int size, char* msg1, char* msg2)
{
    bzero((char*)newMsg, size);
    strcpy(newMsg, msg1);
    strcat(newMsg, msg2);
}