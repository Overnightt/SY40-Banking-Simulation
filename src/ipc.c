/* Handles inter process comunications */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int msg_queue_id;

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


int send_request(Request* req);

int receive_request(Request* req);

int send_response(Response* rep);

int receive_response(Response* rep);
