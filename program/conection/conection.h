#ifndef CONECTION_H
#define CONECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h> // perror()
#include <stdlib.h>
#include <string.h> // bzero()
#include <unistd.h> // close()

#define APRESENTACAO "A"
#define BULLY "B"
#define BECLEY "C"
#define TIME "T"
#define NEW_TIME "N"
#define LIDER "L"
#define OK "K"
#define END "E"

void mandar(const char *msg, unsigned long size, int sock, struct sockaddr_in addr);
void escutar(int sock, struct sockaddr_in *addr, char *msg, unsigned long size);
int startSocket();
void initSocketAddr(struct sockaddr_in *addr, const int port);
void configureToSend(struct sockaddr_in *addr, const char *group);
void configureToListen(int sock, struct sockaddr_in *addr, struct ip_mreqn *mreq, const char *group);
void closeSocket(int *sock);

void sendApresentacao(int id, int sock, struct sockaddr_in addr, char* aux);
void sendEleicao(int id, int sock, struct sockaddr_in addr, char* aux);
void sendLider(int id, int sock, struct sockaddr_in addr, char* aux);
void sendBerkley(int sock, struct sockaddr_in addr, char* aux);
void sendTime(int id, int time, int sock, struct sockaddr_in addr, char* aux);
void sendNewTime(int id, int time, int sock, struct sockaddr_in addr, char* aux);
void sendEnd(int sock, struct sockaddr_in addr, char* aux);

int nextNumber(char* msg); // retorna a posicao do segundo numero da mensagem TIME

#endif