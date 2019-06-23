/* 

multicast.c

The following program sends or receives multicast packets. If invoked
with one argument, it sends a packet containing the current time to an
arbitrarily chosen multicast group and UDP port. If invoked with no
arguments, it receives and prints these packets. Start it as a sender on
just one host and as a receiver on all the other hosts

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()
#include <string.h> 
#include <strings.h> // bzero()

#include "conection/conection.h"
#include "log/log.h"

#include <pthread.h>

#define EXAMPLE_PORT 6000
#define EXAMPLE_GROUP "239.0.0.1"
#define MAX_MSG_SIZE 60
#define MAX_NAME_SIZE 50

int time_global = 0; 

void setTime(int n);
int getTime();
void* incrementTime(void* v);

int main(int argc, char* argv[])
{
   if (argc < 4){
      printf("$ ./prog 'id' 'time' 'start'[1-send, 0-listen]\n");
      return 1;
   }

   int start = atoi(argv[3]);
   int timeLocal = atoi(argv[2]);
   int id = atoi(argv[1]);
   char arqPath[MAX_NAME_SIZE] = "saida/processo_";
   strcat(arqPath, argv[1]);
   strcat(arqPath, ".txt");
   printf("%s\n", arqPath);
   char msgAux[MAX_MSG_SIZE];
   pthread_t thread;

   setTime(timeLocal);
   pthread_create(&thread, NULL, incrementTime, NULL); // inicio da conta do tempo
   struct sockaddr_in addr;
   int sock, cnt;
   struct ip_mreqn mreq;
   char message[MAX_MSG_SIZE];

   sock = startSocket();
   initSocketAddr(&addr, EXAMPLE_PORT);

   if (start == 1)
   {
      /* send */
      configureToSend(&addr, EXAMPLE_GROUP);
      int i = 0;
      while (i < 5)
      {
         time_t t = time(0);
         sprintf(message, "time is %-24.24s", ctime(&t));
         printf("sending: %s\n", message);
         concat(msgAux, MAX_MSG_SIZE,"Preparando para mandar a msg: ", message);
         escrever(arqPath, id, getTime(), msgAux);
         //timeLocal++;
         mandar(message, sizeof(message), sock, addr);
         concat(msgAux, MAX_MSG_SIZE,"mensagem enviada enviado: ", message);
         escrever(arqPath, id, getTime(), msgAux);
         //timeLocal++;
         sleep(4);
         ++i;
      }
      closeSocket(sock);
   }
   else
   {
      configureToListen(sock, &addr, &mreq, EXAMPLE_GROUP);
      int i = 0;
      while (i < 5)
      {
         char *m = (char *)malloc(MAX_MSG_SIZE);
         escrever(arqPath, id, getTime(), "Escutando...");
         //timeLocal++;
         escutar(sock, &addr, m, MAX_MSG_SIZE);
         concat(msgAux, MAX_MSG_SIZE, "mensagem recebida: ", m);
         escrever(arqPath, id, getTime(), msgAux);
         //timeLocal++;
         printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), m);
         free(m);
         ++i;
      }
      closeSocket(sock);
   }

   setTime(9999999);
   pthread_exit(NULL);
   return 0;
}

void setTime(int n)
{
   time_global = n;
}

int getTime()
{
   int n = time_global;
   return n;
}

void* incrementTime(void* v)
{
   while(time_global < 9999999){
      time_global = time_global + 1;
      sleep(1);
   }
}
