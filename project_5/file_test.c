/* PRUDHVI GALI - 1213366386
   VAMSI KRISHNA LANKA - 1215371417 
   */
#include "msgs.h"
#include<stdlib.h>
#include<stdio.h>
#include <stdbool.h> 

#define MAXLEN_FILENAME 15
#define MAX_NUM_FILE_TRANSFERS 99
#define SERVER_PORT 99

/*******************************************************
 Response from server
 msg[0] = 1 --> File transfer request accepted
 msg[0] = 2 --> server busy serving first 3 clients
 msg[0] = 3 --> server rejects file name len greater than 15 chars
 msg[0] = 4 --> server rejects file size len greater than 1 MB
 msg[0] = 5 --> further requests are denied
 Request from client
 msg[0] --> client port or reply port
 msg[1] = 999 --> file transfer request
 msg[2] to msg[10] --> file contents
********************************************************/

char *file_names[MAX_NUM_FILE_TRANSFERS]; // file_names accessed by client threads
static int client_port = 0;		// client_ports
long int file_sizes[MAX_NUM_FILE_TRANSFERS];
Semaphore_t* mutex;			// for changing client port
int numFileTransferDone=0;		// To keep track num of file transfers done
static long int num_bytes_transfered[MAX_NUM_FILE_TRANSFERS];
bool fail1 = false;			// fail1 = true when file size is greater than 1MB
bool fail2 = false;			// fail2 = true when file name has greater than 15 characters
long int findSize(char* filename) 	// caluculate num of bytes the file has
{
    FILE* fp = fopen(filename, "r"); 
    if (fp == NULL) { 
        printf("Error:File Not Found!\n"); 
        return -1; 
    }
    fseek(fp, 0L, SEEK_END); 
    long int ret = ftell(fp); 
    fclose(fp); 
    return ret; 
}
/*Server thread*/
void server_thread(){
	msg_t msg,reply;
	port_t *client,*server;
	server = &ports[SERVER_PORT];
	char* server_file_names[MAX_NUM_FILE_TRANSFERS];
	FILE* fp;
	int client_num;
	while(1){
		Receive(server,&msg);
		int reply_port = msg.arr[0];
		client_num = reply_port+1;
		client = &ports[reply_port];
		if(msg.arr[1]==999){
			char *temp = (char*)(&msg.arr[2]);
			int file_name_size = strlen(temp)*sizeof(char);
			if((!fail1&&!fail2)&&reply_port>2 && numFileTransferDone<3){
				reply.arr[0]=2;		// Server busy response
			}
			else if(strlen(temp)>15){
				printf("%s:file transfer request from client-%d rejected: file_name len greater than 15 chars\n",__FUNCTION__,client_num);
				reply.arr[0]=3;		// File name error response
			}
			else if((fail1 && reply_port>2)||(fail2 && reply_port>2)){
				reply.arr[0]=5;		// Already error happened further requests for files are ignored
				printf("%s:Further requests are denied!!!! to client-%d\n",__FUNCTION__,client_num);
			}
			else{
				reply.arr[0]=1;		// file transfer request accepted
				server_file_names[reply_port]=(char*)malloc(file_name_size);
				strcpy(server_file_names[reply_port],temp);
				strcat(server_file_names[reply_port],".server");
			}
			Send(client,reply);
			continue;
		}
		fp = fopen(server_file_names[reply_port],"a");	// appending
		char buffer[36];
		memcpy(buffer,&msg.arr[1],36);	// Received buffer copied to .server file
		for(int i=0;i<36;i++){
			if(num_bytes_transfered[reply_port]<file_sizes[reply_port]){
				fputc(buffer[i],fp);
				num_bytes_transfered[reply_port]++;
				if(num_bytes_transfered[reply_port]>=1048576){
					printf("%s:File:%s from client-%d rejected size greater than 1MB\n",__FUNCTION__,file_names[reply_port],client_num);		// File size greater than 1MB
					reply.arr[0]=4;
					break;
				}
				reply.arr[0]=1;
			}
			else{
				numFileTransferDone++;
				reply.arr[0]=1;	// File transfer successfully completed
				printf("%s:file_transfer completed for client-%d\n",__FUNCTION__,client_num);
				break;
			}
		}
		fclose(fp);
		Send(client,reply);
	}
}
void client_thread(){
	port_t *client,*server;
	msg_t request,response;
	char *file_name = file_names[client_port];
	FILE *fp = fopen(file_name,"r");
	int client_num = client_port+1;
	request.arr[0]=client_port; // index - 0 reply_port
	P(mutex);
	client = &ports[client_port++];
	V(mutex);
	server = &ports[SERVER_PORT];
	request.arr[1]=999;	// requesting server to file transfer
	memcpy(&request.arr[2],file_name,strlen(file_name)*sizeof(char));
	while(1){
		Send(server,request);
		Receive(client,&response);
		if(response.arr[0]==1){
			printf("%s:%d started sending file\n",__FUNCTION__,client_num);
			char buffer[36];int i=0;
			while((buffer[i++]=fgetc(fp))!=EOF){
				if(i==36){
					memcpy(&request.arr[1],buffer,36);			
					Send(server,request);
					Receive(client,&response);
					if(response.arr[0]==4) break;
					i=0;
				}
			}
			if(response.arr[0]==4){
				printf("%s:%d File size greater than 1MB, rejected by server!!!\n",__FUNCTION__,client_num);
				fail1=true;
				break;
			}
			if(i!=0){
				memcpy(&request.arr[1],buffer,i);
				Send(server,request);
				Receive(client,&response);
				printf("%s:%d file transfer done\n",__FUNCTION__,client_num);
				break;
			}
		}
		else if(response.arr[0]==2){
		//	printf("%s:Server busy message received\n",__FUNCTION__);
		}
		else if(response.arr[0]==3){
			printf("%s:%d Server rejected file transfer file name len greater than 15 chars\n",__FUNCTION__,client_num);
			fail2 = true;
			break;
		}
		else if(response.arr[0]==5){
			printf("%s:%d Server rejected further requests\n",__FUNCTION__,client_num);
			break;
		}
	}
	while(1){
		yeild();
	}
}
int main(int argc, char* argv[]){
	int num_files=atoi(argv[1]);
	initPorts();
	mutex = CreateSem(1);
	for(int i=0;i<num_files;i++){
		int file_name_size = strlen(argv[i+2])*sizeof(char);
		file_names[i]=(char*)malloc(file_name_size);
		strcpy(file_names[i],argv[i+2]);
		printf("file_name:%s\n",file_names[i]);
		file_sizes[i]=findSize(file_names[i]);
		printf("file_size:%ld\n",file_sizes[i]);
		start_thread(client_thread);			
	}
	start_thread(server_thread);
	run();
}
