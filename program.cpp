#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // for sleep()

#include <pthread.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MSGBUFSIZE 256

int tempo = 0;
const int delay_secs = 5; // Unix sleep is seconds

void   incrementa()    
{   
	++tempo; 
	printf("t = %d\n", tempo); 
}
int    getTempo()      { return tempo; }
void   setTempo(int x) { tempo = x; }

int main(int argc, char const *argv[])
{
    /*
    if (argc != 3) {
       printf("Command line args should be multicast group and port\n");
       printf("(e.g. for SSDP, `sender 239.255.255.250 1900`)\n");
       return 1;
    }
    */

    int a = atoi(argv[1]);
    setTempo(a);

    pthread_t th;

    pthread_create( &th, NULL, incrementa, NULL);

    sleep(delay_secs);

    pthread_join(th, NULL);

	return 0;
}