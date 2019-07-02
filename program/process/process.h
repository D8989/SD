#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESS 10

typedef struct {
    int i;
    int process[MAX_PROCESS];
}process_t;

void addProcess(int a, process_t* p);
void printAllProcess(int id, process_t *p);
int haveProcess(int a, process_t* p);
#endif