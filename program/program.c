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
#include <string.h> // bzero()

#define EXAMPLE_PORT 6000
#define EXAMPLE_GROUP "239.0.0.1"
#define MAX_MSG_SIZE 50

void mandar(const char *msg, unsigned long size, int sock, struct sockaddr_in addr)
{
   int cnt;
   cnt = sendto(sock, msg, size, 0,
                (struct sockaddr *)&addr, sizeof(addr));
   if (cnt < 0)
   {
      perror("sendto");
      exit(1);
   }
}

void escutar(int sock, struct sockaddr_in *addr, char *msg, unsigned long size)
{
   int cnt;
   int addrlen = sizeof(*addr);
   cnt = recvfrom(sock, msg, size, 0,
                  (struct sockaddr *)addr, &addrlen);
   if (cnt < 0)
   {
      perror("recvfrom");
      exit(1);
   }
   else if (cnt == 0)
   {
      strcpy(msg, "mensagem vazia.");
   }
}

int startSocket()
{
   /* set up socket */
   int sock = -1;
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0)
   {
      perror("socket");
      exit(1);
   }
   return sock;
}

void initSocketAddr(struct sockaddr_in *addr, const int port)
{
   bzero((char *)addr, sizeof(addr));
   addr->sin_family = AF_INET;
   addr->sin_addr.s_addr = htonl(INADDR_ANY);
   addr->sin_port = htons(port);
}

void configureToSend(struct sockaddr_in *addr, const char *group)
{
   addr->sin_addr.s_addr = inet_addr(group);
}

void configureToListen(int sock, struct sockaddr_in *addr, struct ip_mreqn *mreq, const char *group)
{
   // allow multiple sockets to use the same PORT number
   // https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6#file-listener-c-L63

   u_int yes = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0)
   {
      perror("Reusing ADDR failed");
      exit(1);
   }
   //------------------------------------------------------------

   if (bind(sock, (struct sockaddr *)addr, sizeof(*addr)) < 0)
   {
      perror("bind");
      exit(1);
   }
   mreq->imr_multiaddr.s_addr = inet_addr(group);
   //mreq.imr_interface.s_addr = htonl(INADDR_ANY);
   mreq->imr_ifindex = 0;
   if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                  mreq, sizeof(*mreq)) < 0)
   {
      perror("setsockopt mreq");
      exit(1);
   }
}

void closeSocket(int sock)
{
   close(sock);
}

int main(int argc)
{
   struct sockaddr_in addr;
   int addrlen, sock, cnt;
   struct ip_mreqn mreq;
   char message[MAX_MSG_SIZE];

   sock = startSocket();
   initSocketAddr(&addr, EXAMPLE_PORT);
   addrlen = sizeof(addr);

   if (argc > 1)
   {
      /* send */
      configureToSend(&addr, EXAMPLE_GROUP);
      int i = 0;
      while (i < 5)
      {
         time_t t = time(0);
         sprintf(message, "time is %-24.24s", ctime(&t));
         printf("sending: %s\n", message);
         mandar(message, sizeof(message), sock, addr);
         sleep(3);
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
         escutar(sock, &addr, m, MAX_MSG_SIZE);
         printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), m);
         free(m);
         ++i;
      }
      closeSocket(sock);
   }

   return 0;
}
