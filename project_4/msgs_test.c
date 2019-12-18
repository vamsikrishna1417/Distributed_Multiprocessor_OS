/* PRUDHVI GALI - 1213366386
   VAMSI KRISHNA LANKA - 1215371417 
   */
#include <unistd.h>
#include "msg.h"

#define SERVERPORT 99

void printResponse(msg_t response){
    int i;
    for(i=0;i<10;i++)
	printf("%d  ",response.arr[i]);
    printf("\n");
}

void clientFunc(){
    int port;
    msg_t request,response;
    port_t *server = &ports[SERVERPORT];
    port_t *client;
    while(1){
	for(int i=0;i<10;i++)
	    request.arr[i]=rand()%100;
	port = rand()%99;
	request.replyPort=port;
	client = &ports[port];
	Send(server,request);
	printf("client port %d sending request to server port %d  Message:",request.replyPort, SERVERPORT);
	printResponse(request);
	Receive(client,&response);
	printf("client port %d received response from server port %d Message:",request.replyPort, SERVERPORT);
	printResponse(response);
	sleep(1);
    }
}

void serverFunc(){ 
    int port;
    msg_t response;
    port_t *server = &ports[SERVERPORT];
    port_t *client;
    while(1){
	Receive(server,&response);
	client = &ports[response.replyPort];
	printf("server port %d received request from client port %d  Message: ",SERVERPORT, response.replyPort);
	printResponse(response);
	Send(client,response);
	printf("server port %d sent response to Client port %d Message:",SERVERPORT, response.replyPort);
	printResponse(response);
	sleep(1);
    }
}

int main(){
    /* Initializing the ports */
    initPorts();
    /* creating 3 clients and 1 server threads */
    start_thread(clientFunc);
    start_thread(clientFunc);
    start_thread(clientFunc);
    start_thread(serverFunc);
    run();	
}
