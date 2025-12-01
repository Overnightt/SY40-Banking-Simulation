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

        printf("DEBUG send_response: target_id=%d, mtype=%ld\n", 
                rep->target_id, rep->mtype);

        if(msgsnd(msg_queue_id,rep,sizeof(Response)-sizeof(long),0)==-1) {
                perror("Error sending response");
                return -1;
        }
        else {
                printf("DEBUG: Response sent successfully\n");
                return 0;
        }
}

//used by the client to recieve a response
int receive_response(Response* rep, int client_id) {
        long expected_mtype = CLIENT_MTYPE(client_id);
        
        
        printf("DEBUG receive_response: client_id=%d, expected_mtype=%ld\n", 
               client_id, expected_mtype);
        printf("DEBUG: Waiting for message...\n");
        
        if (msgrcv(msg_queue_id, rep, sizeof(Response) - sizeof(long), 
                   expected_mtype, 0) == -1) {
            perror("Error receiving response");
            return -1;
        }
        
        printf("DEBUG: Response received! status=%d, result=%d\n", 
               rep->status, rep->result);
        return 0;
    }

void cleanup_ipc(int remove_queue) {
    if (msg_queue_id == -1) {
        return;  // Pas initialisé
    }
    
    if (remove_queue) {
        // Supprimer la queue (uniquement pour la banque)
        if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
            perror("Error removing message queue");
        } else {
            printf(" Message queue removed\n");
        }
    }
    
    msg_queue_id = -1;
}

//Debug 
void print_request(const Request* req) {
    printf("[REQUEST] client=%d, type=%d, target=%d, amount=%d\n",
           req->source_id, req->type, req->target_id, req->amount);
}

void print_response(const Response* rep) {
    printf("[RESPONSE] target=%d, status=%d, result=%d, msg='%s'\n",
           rep->target_id, rep->status, rep->result, rep->message);
}