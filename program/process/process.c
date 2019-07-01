#include "process.h"

#include <stdio.h>

void addProcess(int a, process_t *p)
{
    p->process[p->i] = a;
    p->i += 1;
}

void printAllProcess(int id, process_t *p){
    printf("processo %d enxerga os processos: ", id);
    int i;
    for(i = 0; i < MAX_PROCESS; ++i){
        if(p->process[i] != 0){
            printf("%d ", p->process[i]);
        }
    }
    printf("\n");
}