/* 
Trabalho de SD
Daniel Juventude Moreira
05/07/2019
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
#include "process/process.h"

#include <pthread.h>

#define EXAMPLE_PORT 6000
#define EXAMPLE_GROUP "239.0.0.1"

#define MAX_NAME_SIZE 50

int time_global = 0;
int stop = 1;          // tempo que o relogio vai para
int boolStop = 0;      // bool para controlar o atraso do tempo
int boolRunThread = 1; // bool que controla a saida da thread

void setTime(int n);
int getTime();
void *incrementTime(void *v);

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("$ ./prog 'id' 'time' 'eleicao'[1-send, 0-listen]\n");
        return 1;
    }

    int eleicao = atoi(argv[3]);
    int timeLocal = atoi(argv[2]);
    int id = atoi(argv[1]);
    int running = 1;
    int welcome = 1;     // inicia a apresentacao
    int welcomEnd = 0;   // bool que indica o fim da apresentacao.
    int vitoria = 0;     // bool que controla a mensagem de vitoria da eleicao.
    int lider = 0;       // booleano para controlar o algoritmo do bully
    int idLider = -1;    // quarda o id do lider do SD
    int berkleyAux = 1;  // bool auxiliar para começar o algoritmo de berkley.
    int sendTimeAux = 0; // bool para controlar o envio do seu relogio para o lider.
    int count = 0;       // conta quandos processos mandou o seu tempo
    double media = 0;    // media do tempo de berkeley
    int newTime = 0;     // bool que controla o envio de mensagens de atualizaçao do tempo
    int end = 0;         // bool que controla o final do algoritmo
    int sEnd = 0;        // bool que envia a menssagem de fim

    if (eleicao)
    { // nescessário para o caso do maior processo começar a eleicao.
        lider = 1;
    }
    else
    {
        lider = 0;
    }

    char arqPath[MAX_NAME_SIZE] = "saida/processo_";
    strcat(arqPath, argv[1]);
    strcat(arqPath, ".txt");
    printf("%s\n", arqPath);
    char msgAux[MAX_MSG_SIZE];
    pthread_t thread;
    process_t process;
    bzero((char *)&process, sizeof(process));
    process.qtd = 1; // ele proprio.

    //setTime(timeLocal);
    time_global = timeLocal;
    pthread_create(&thread, NULL, incrementTime, NULL); // inicio da conta do tempo
    struct sockaddr_in addr;
    bzero((char *)&addr, sizeof(addr));
    int sock, cnt;
    struct ip_mreqn mreq;
    bzero((char *)&mreq, sizeof(mreq));
    char message[MAX_MSG_SIZE];
    bzero(message, MAX_MSG_SIZE);

    sock = startSocket();
    initSocketAddr(&addr, EXAMPLE_PORT);
    configureToSend(&addr, EXAMPLE_GROUP);
    configureToListen(sock, &addr, &mreq, EXAMPLE_GROUP);

    while (running)
    {
        if (welcome)
        {
            configureToSend(&addr, EXAMPLE_GROUP);
            sendApresentacao(id, sock, addr, message);
            printf("mensagem de apresentacao enviada\n");
            concat(msgAux, MAX_MSG_SIZE, "Apresentacao enviada: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
        }

        if (eleicao && welcomEnd)
        {
            configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
            sendEleicao(id, sock, addr, message);
            printf("mensagem de eleicao enviada\n");
            concat(msgAux, MAX_MSG_SIZE, "Eleicao enviada: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
        }

        if (vitoria)
        {
            configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
            sendLider(id, sock, addr, message);
            printf("mensagem de vitoria enviada.\n");
            concat(msgAux, MAX_MSG_SIZE, "Vitoria enviada: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
            vitoria = 0;
        }

        if (berkleyAux && idLider == id)
        {
            configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
            sendBerkley(sock, addr, message);
            printf("mensagem de Berkley enviada.\n");
            concat(msgAux, MAX_MSG_SIZE, "inicio do Berkeley enviada: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
            berkleyAux = 0; // evita comecar o algoritmo de novo.
        }

        if (sendTimeAux)
        {
            configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
            sendTime(id, getTime(), sock, addr, message);
            printf("mensagem de tempo enviada.\n");
            concat(msgAux, MAX_MSG_SIZE, "Tempo atual enviada: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
            sendTimeAux = 0; // evitar mandar a mesma mensagem varias vezes.
        }

        if (sEnd)
        {
            configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
            sendEnd(sock, addr, message);
            printf("menssagem de fechamento enviada.\n");
            concat(msgAux, MAX_MSG_SIZE, "Fechamendo enviado: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
        }

        if (newTime)
        {
            int i = 0;
            double newT = 0; // novo tempo;
            while (i < process.i)
            {

                newT = media;
                //newT = abs(media - process.time[i]);
                configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
                sendNewTime(process.process[i], newT, sock, addr, message);
                concat(msgAux, MAX_MSG_SIZE, "Novo tempo enviado: ", message);
                escrever(arqPath, id, getTime(), msgAux);
                bzero(message, MAX_MSG_SIZE);
                ++i;
            }
            newT = media;

            configureToSend(&addr, EXAMPLE_GROUP); // não entendo a nescessidade desta linha...
            sendNewTime(id, newT, sock, addr, message);
            concat(msgAux, MAX_MSG_SIZE, "Novo tempo enviada: ", message);
            escrever(arqPath, id, getTime(), msgAux);
            bzero(message, MAX_MSG_SIZE);
            newTime = 0; // evita envio de mensagens repetidas
        }

        printf("escutando...\n");
        escutar(sock, &addr, msgAux, MAX_MSG_SIZE);
        char aux[2] = {msgAux[0]};

        if (strcmp(APRESENTACAO, aux) == 0)
        {
            printf("\tENTROU NA APRESENTACAO.\n");
            printf("msgAux=%s\n", msgAux);
            int a = atoi(&msgAux[1]);
            // printf("a=%d\n", a);
            if (a != id && !haveProcess(a, &process)) // processo não está na lista.
            {
                addProcess(a, &process);
                concat(message, MAX_MSG_SIZE, "Apresentacao recebida: ", msgAux);
                escrever(arqPath, id, getTime(), message);
                bzero(message, MAX_MSG_SIZE);
                welcome = 1;
            }
            else
            {
                welcome = 0;
            }

            bzero(msgAux, sizeof(msgAux));
            printAllProcess(id, &process);
        }
        else if (strcmp(BULLY, aux) == 0)
        {
            printf("\tENTROU NA ELEICAO.\n");
            printf("msgAux=%s\n", msgAux);
            int a = atoi(&msgAux[1]);
            //printf("a=%d\n", a);
            concat(message, MAX_MSG_SIZE, "Eleicao recebida: ", msgAux);
            escrever(arqPath, id, getTime(), message);
            bzero(message, MAX_MSG_SIZE);

            if (id > a)
            {
                eleicao = 1; // vai comecar uma nova eleicao.
                lider = 1;
            }
            else if (id < a)
            {
                eleicao = 0;
                lider = 0;
            }
            else
            {
                eleicao = 0; // pro processo não comecar a eleicao da propria chamada
            }
        }
        else if (strcmp(LIDER, aux) == 0)
        {
            printf("\tENTROU NA CONCLUSAO DA ELEICAO.\n");
            printf("msgAux=%s\n", msgAux);
            int a = atoi(&msgAux[1]);
            //printf("a=%d\n", a);
            idLider = a;
            lider = 0;
            eleicao = 0;
            printf("\tProcesso %d é o lider do SD.\n", idLider);
            concat(message, MAX_MSG_SIZE, "Lider recebida: ", msgAux);
            escrever(arqPath, id, getTime(), message);
            bzero(message, MAX_MSG_SIZE);
        }
        else if (strcmp(BECLEY, aux) == 0)
        {
            printf("\tENTROU NO ALGORITMO DE BERKELEY.\n");
            printf("msgAux=%s\n", msgAux);
            concat(message, MAX_MSG_SIZE, "Inicio do Berkeley recebida: ", msgAux);
            escrever(arqPath, id, getTime(), message);
            bzero(message, MAX_MSG_SIZE);
            sendTimeAux = 1;
        }
        else if (strcmp(TIME, aux) == 0)
        {
            if (id == idLider)
            { // Apenas o lider recebe esta mensagem.
                printf("\tENTROU NO CALCULO DO TEMPO.\n");
                int p_id = atoi(&msgAux[1]);
                int i = nextNumber(msgAux);
                int p_time = atoi(&msgAux[i]);
                printf("p_id=%d; p_time=%d\n", p_id, p_time);
                addTime(p_id, p_time, &process);
                concat(message, MAX_MSG_SIZE, "Tempo recebido: ", msgAux);
                escrever(arqPath, id, getTime(), message);
                bzero(message, MAX_MSG_SIZE);
                ++count;
                if (process.i + 1 == count) // já leu o tempo de todos os processos
                {
                    media = mediaTempo(&process);
                    //printf("media = %lf\n", media);
                    newTime = 1; // seta para mandar as mensagens de update de tempo.
                }
            }
        }

        else if (strcmp(NEW_TIME, aux) == 0)
        {
            int p_id = atoi(&msgAux[1]);
            int i = nextNumber(msgAux);
            int p_time = atoi(&msgAux[i]);
            printf("p_id=%d; p_time=%d\n", p_id, p_time);
            if (p_id == id)
            {
                printf("\tENTROU NO UPDATE DO TEMPO.\n");
                concat(message, MAX_MSG_SIZE, "Novo Tempo recebido: ", msgAux);
                escrever(arqPath, id, getTime(), message);
                bzero(message, MAX_MSG_SIZE);
                setTime(p_time);
                newTime = 0;
                end = 1;
            }
        }

        else if (strcmp(END, aux) == 0)
        {
            printf("\tENTROU NO FIM DO ALGORITMO.\n");
            printf("msgAux=%s\n", msgAux);
            concat(message, MAX_MSG_SIZE, "FIM recebido: ", msgAux);
            escrever(arqPath, id, getTime(), message);
            bzero(message, MAX_MSG_SIZE);
            running = 0;
        }
        else
        {
            printf("tempo estourado.\n");
            if (welcome == 0 && welcomEnd == 0) // todos os processos já se apresentaram. entra apenas uma vez.
            {
                welcomEnd = 1;
            }
            else if (lider)
            {
                vitoria = 1;
                lider = 0; // para não comecar uma nova eleicao.
            }

            if (id == idLider && end == 1)
            {
                sEnd = 1;
            }

            printAllProcess(id, &process);
        }
        bzero(msgAux, sizeof(msgAux)); // apaga a mensagem
    }

    concat(message, MAX_MSG_SIZE, "FIM do algoritmo.", "");
    escrever(arqPath, id, getTime(), message);
    bzero(message, MAX_MSG_SIZE);

    closeSocket(&sock);
    boolRunThread = 0;
    printf("FIM.\n");
    return 0;
}

void setTime(int new_time)
{
    int time_atual = getTime();
    if (new_time >= time_atual)
    {
        time_global =new_time;
    }
    else
    {
        int sleep_time = new_time - time_atual;
        stop = sleep_time; // variavel global
        boolStop = 1; // ativa a parada do relogio
        printf("\tRelogio irá pausar por %d segundos.\n", sleep_time);
    }
}

int getTime()
{
    int n = time_global;
    return n;
}

// Funcao rodando em thread
void *incrementTime(void *v)
{
    while (boolRunThread)
    {
        time_global = time_global + 1;
        if (boolStop)
        {
            sleep(stop);
            boolStop = 0;
        }
        else
        {
            sleep(1);
        }
    }
}
