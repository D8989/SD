#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESS 3

typedef struct {
    int i;
    int myTime;
    int process[MAX_PROCESS];
    int time[MAX_PROCESS];
    int qtd;
}process_t;

void addProcess(int a, process_t* p);
void printAllProcess(int id, process_t *p);
int haveProcess(int a, process_t* p);
void addTime(int id, int time, process_t* p);
// int qtdProcess(process_t* p);
int mediaTempo(process_t* p);
#endif