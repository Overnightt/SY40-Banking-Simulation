#ifndef IPC_H
#define IPC_H

//The structure of a request
typedef struct {
	int source_id; //id of the client making the request
	int type;     // each type of request has a number
	int target_id; //account id only used for transfer
	int dest_id; //account id only used for transfer
	int amount;  //Used for deposit withdraw and transfer
	char name[50] //used to create new account
	long mtype; 
}Request;

//Every client uses this mtype to send to the bank
#define BANK_MTYPE 1

//The bank responds with a specific mtype for each client
#define CLIENT_MTYPE(client_id) ((long)(client_id)+2)

//the structure of a response
typedef struct {
	int status;         //0 if sucess, -1 if failure
	char message[100]; //text info if the request failed or suceded
	long mtype;       //mtype specific to the client
}response;

int init_ipc(key_t key,int create);
int send_request(Request* req);
int receive_request(Request* req);
int send_response(Response* rep);
int receive_response(Response* rep);

#endif
