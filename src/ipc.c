/* Handles inter process comunications */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int msg_queue_id=-1;

//Sets up the message queue that clients and then bank will use to comunicate
int init_ipc(key_t key,int create){
        if (create==1){    //(bank process) crates or open a queue if it already exists
                msg_queue_id=msgget(key,IPC_CREAT | 0666);
                if (msg_queue_id ==-1){
                        perror("error during message queue creation");
                        return -1;
                }
                else{
                        return 0;
                }
        }
        else{   //(client processes) opens an exisring message queue created by the bank
                msg_queue_id=msgget(key,0);
                if (msg_queue_id ==-1){
                        perror("error during message queue oppening");
                        return -1;
                }
                else{
                        return 0;
                }
        }
}

// used by a client to send a request message to the bank's message queue
int send_request(Request* req){
	req->mtype= BANK_MTYPE;
	if(msgsnd(msg_queue_id,req,sizeof(Request)-sizeof(long),0)==-1) {
		return -1;
	}
	else {
		return 0;
	}
}
//used by the bank to recieve and sore request on the message queue
int receive_request(Request* req){
	if(msgrcv(msg_queue_id,req,sizeof(Request)-sizeof(long),BANK_MTYPE,0)==-1){
		return -1;

	}
	else {
		return 0;
	}
}

//used by the bank to send a response to the client
int send_response(Response* rep){
	rep->mtype = CLIENT_MTYPE(rep->target_id);
        if(msgsnd(msg_queue_id,rep,sizeof(Response)-sizeof(long),0)==-1) {
                return -1;
        }
        else {
                return 0;
        }
}

//used by the client to recieve a response
int receive_response(Response* rep,int client_id){
        if(msgrcv(msg_queue_id,rep,sizeof(Response)-sizeof(long),CLIENT_MTYPE(client_id),0)==-1){
                return -1;
        }
        else {
                return 0;
        }
}
