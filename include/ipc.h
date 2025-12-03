#ifndef IPC_H
#define IPC_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h> 
#include <sys/ipc.h>

extern int msg_queue_id;

//The structure of a request
typedef struct {
	long mtype;
	int source_id; //id of the client making the request
	int type;      //each type of request has a number
	int target_id; //account id only used for transfer
	int dest_id;   //account id only used for transfer
	int amount;    //Used for deposit withdraw and transfer
	char name[50]; //used to create new account 
}Request;

//Every client uses this mtype to send to the bank
#define BANK_MTYPE 1

//The bank responds with a specific mtype for each client
#define CLIENT_MTYPE(client_id) ((long)(client_id)+2)

//the structure of a response
typedef struct {
	long mtype;       //mtype specific to the client
	int status;         //0 if sucess, -1 if failure
	int target_id;      //id of the client that is recieving the response
	char message[512]; //text info if the request failed or suceded
}Response;

int init_ipc(key_t key,int create);
int send_request(Request* req);
int receive_request(Request* req);
int send_response(Response* rep);
int receive_response(Response* rep,int client_id);

#endif
