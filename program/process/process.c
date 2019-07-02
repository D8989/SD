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
            printf("%d[%d] ", p->process[i], p->time[i]);
        }
    }
    printf("\n");
}

int haveProcess(int a, process_t* p)
{
    int i;
    for(i = 0; i < MAX_PROCESS; ++i){
        if(p->process[i] == a){
            return 1;
        }
    }
    return 0;
}

void addTime(int id, int time, process_t* p)
{
    int i = 0;
    int j;
    while( i < MAX_PROCESS && p->process[i] != id){
        ++i;
    }
    
    if( i == MAX_PROCESS){ 
        p->myTime = time;    
    }
    else{
        p->time[i] = time;
    }   
}

double mediaTempo(process_t* p)
{
    int i;
    double sum = 0;
    for(i = 0; i < MAX_PROCESS; ++i){
        sum += p->time[i];
    }
    sum += p->myTime;
    return sum/(p->i+1);
}
