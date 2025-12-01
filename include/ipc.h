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

// Clé IPC pour la file de messages (partagée entre banque et clients)
#define IPC_KEY 0x12345678

// Variable globale pour l'ID de la file de messages
extern int msg_queue_id;

//Every client uses this mtype to send to the bank
#define BANK_MTYPE 1

//The bank responds with a specific mtype for each client
#define CLIENT_MTYPE(client_id) ((long)(client_id)+2)


// Énumération des types d'opérations bancaires
#define OP_DEPOSIT         1
#define OP_WITHDRAW        2
#define OP_TRANSFER        3
#define OP_GET_BALANCE     4
#define OP_CREATE_ACCOUNT  5
#define OP_REGISTER_CLIENT 6  
#define OP_LOGIN_CLIENT    7  
#define OP_FETCH_ACCOUNTS  8 //To ask the bank the account list for a cient

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

//the structure of a response
typedef struct {
	long mtype;       //mtype specific to the client
	int status;         //0 if sucess, -1 if failure
	int target_id;      //id of the client that is recieving the response
	int result;
	char message[100]; //text info if the request failed or suceded
}Response;

//Init and cleanup 
int init_ipc(key_t key,int create);
void cleanup_ipc(int remove_queue);

// Send and receip
int send_request(Request* req);
int receive_request(Request* req);
int send_response(Response* rep);
int receive_response(Response* rep,int client_id);

// Debug function
void print_request(const Request* req);
void print_response(const Response* rep);

#endif