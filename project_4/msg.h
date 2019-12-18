#include "sem.h"
#define N 10

typedef struct msg_s{
    int arr[10];	// array of 10 integers
    int replyPort;	
}msg_t;

typedef struct port_s{
    msg_t *m;
    int wp;
    int rp;
    Semaphore_t *full,*empty;
    Semaphore_t* mutex;
}port_t;

port_t ports[100];

void initPorts(){
    int i;
    /* Initializing 100 ports */
    for(i=0;i<100;i++){
	ports[i].m = (msg_t*)malloc(N*sizeof(msg_t));
	ports[i].wp = 0;
	ports[i].rp = 0;
	ports[i].full = CreateSem(0);
	ports[i].empty = CreateSem(N);
	ports[i].mutex = CreateSem(1);
    }
}

/* Blocking send : places the message in the port and returns but blocks if port is full */
void Send(port_t *server, msg_t msg){
    P(server->empty);
    P(server->mutex);
    memcpy(&server->m[server->wp],&msg,sizeof(msg_t));
    server->wp = (server->wp+1)%N;
    V(server->mutex);
    V(server->full);
}

/* Blocking Receive : Block if port does not contains a message */
void Receive(port_t *server, msg_t* msg){
    P(server->full);
    P(server->mutex);
    memcpy(msg,&server->m[server->rp],sizeof(msg_t));
    server->rp = (server->rp+1)%N;
    V(server->mutex);
    V(server->empty);
}
