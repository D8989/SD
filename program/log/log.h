#ifndef LOG_H
#define LOG_H

#define MAX_MSG_SIZE 60

void escrever(const char* arqName, int id, int time, const char* msg);
void concat(char* newMsg, unsigned int size, char* msg1, char* msg2);

#endif // LOG_H
