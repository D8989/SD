#include "conection.h"
#include "../log/log.h"

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
   /*  
   if (cnt < 0)
   {
      perror("recvfrom");
      exit(1);
   }
   */
   if (cnt == 0)
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
   bzero((char *)addr, sizeof(*addr));
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

   unsigned int yes = 1;
   if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0)
   {
      perror("Reusing ADDR failed");
      exit(1);
   }
   //------------------------------------------------------------

   // set time limit
   struct timeval timeout;
   timeout.tv_sec = 7; // tempo de espera
   timeout.tv_usec = 0;
   if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
   {
      perror("setsockopt failed\n");
   }

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

void closeSocket(int *sock)
{
   close(*sock);
   *sock = 0;
}

void sendApresentacao(int id, int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(APRESENTACAO) + sizeof(id);
   char msg[size];
   char number[12];
   sprintf(number, "%d", id);
   concat(msg, size, APRESENTACAO, number);
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

void sendEleicao(int id, int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(BULLY) + sizeof(id);
   char msg[size];
   char number[12];
   sprintf(number, "%d", id);
   concat(msg, size, BULLY, number);
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

void sendLider(int id, int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(LIDER) + sizeof(id);
   char msg[size];
   char number[12];
   sprintf(number, "%d", id);
   concat(msg, size, LIDER, number);
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

void sendBerkley(int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(BECLEY);
   char msg[size];
   concat(msg, size, BECLEY, "");
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

void sendTime(int id, int time, int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(TIME) + sizeof(id) + sizeof(char) + sizeof(time);
   char msg[size];
   char msg1[size];
   char msg2[size];
   char numberId[12];
   char numberTime[12];
   sprintf(numberId, "%d", id);
   sprintf(numberTime, "%d", time);
   concat(msg1, size, TIME, numberId);
   concat(msg2, size, ",", numberTime);
   concat(msg, size, msg1, msg2);
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

void sendNewTime(int id, int time, int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(NEW_TIME) + sizeof(id) + sizeof(char) + sizeof(time);
   char msg[size];
   char msg1[size];
   char msg2[size];
   char numberId[12];
   char numberTime[12];
   sprintf(numberId, "%d", id);
   sprintf(numberTime, "%d", time);
   concat(msg1, size, NEW_TIME, numberId);
   concat(msg2, size, ",", numberTime);
   concat(msg, size, msg1, msg2);
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

void sendEnd(int sock, struct sockaddr_in addr, char* aux)
{
   int size = sizeof(END);
   char msg[size];
   concat(msg, size, END, "");
   strcpy(aux, msg);
   mandar(msg, size, sock, addr);
}

int nextNumber(char *msg)
{
   int i = 0;
   while (msg[i] != '\n')
   {
      if (msg[i] == ',')
      {
         return i + 1;
      }
      ++i;
   }
   return -1; // NUNCA VAI ACONTECER.
}
