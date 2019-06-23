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

void mandar(const char *msg, unsigned long size, int sock, struct sockaddr_in addr);
void escutar(int sock, struct sockaddr_in *addr, char *msg, unsigned long size);
int startSocket();
void initSocketAddr(struct sockaddr_in *addr, const int port);
void configureToSend(struct sockaddr_in *addr, const char *group);
void configureToListen(int sock, struct sockaddr_in *addr, struct ip_mreqn *mreq, const char *group);
void closeSocket(int sock);

#endif