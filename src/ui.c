#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ipc.h"
#include "client.h"
#include "bank.h"
#include "ui.h"
#define MAX_NAME 50

//handles the bank process
void bank_process(){
	printf("Welcome i'm the bank process i'll be running and processing request \n");
	key_t key = ftok("bank", 1);
        init_ipc(key, 1);
	Request req;
	Response rep;
	while (1){
		if (receive_request(&req)==0){
			rep.target_id=req.source_id;
			rep.status=0;           //we assume sucess
			strcpy(rep.message, "Request done \n");
			if (req.type==0){
				if (is_owner(req.source_id, req.target_id)==0) {
                    rep.status = -1;
                    strcpy(rep.message, "Unauthorized: You do not own this account");
					send_response(&rep);
					continue;
    				}
				if(deposit(req.target_id,req.amount)==-1){
					rep.status=-1;
					strcpy(rep.message, "Deposit failed");
				}
			}
			if (req.type==1){
				if (is_owner(req.source_id, req.target_id)==0) {
                    rep.status = -1;
                    strcpy(rep.message, "Unauthorized: You do not own this account");
					send_response(&rep);
					continue;
                                }
                if (withdraw(req.target_id,req.amount)==-1){
					rep.status=-1;
                    strcpy(rep.message, "Withdraw failed");
				}
            }
            if (req.type==2){
				if (is_owner(req.source_id, req.target_id)==0) {
                    rep.status = -1;
                    strcpy(rep.message, "Unauthorized: You do not own this account");
					send_response(&rep);
					continue;
                                }
                if (transfer(req.target_id,req.dest_id,req.amount)==-1){
					rep.status=-1;
                    strcpy(rep.message, "Transfer failed");
				}
            }
            if (req.type==3){
				if (is_owner(req.source_id, req.target_id)==0) {
                    rep.status = -1;
                    strcpy(rep.message, "Unauthorized: You do not own this account");
					send_response(&rep);
					continue;
				}
				int balance=get_balance(req.target_id);
                if(balance==-1){
					rep.status=-1;
                    strcpy(rep.message, "Balance check failed");
				}
                else {
					char buf[50];
					sprintf(buf,"Balance: %d \n",balance);
					strcpy(rep.message,buf);
				}
            }
            if (req.type==4){
                int add=add_account(req.source_id,req.name);
                if (add==-1){
					rep.status=-1;
                    strcpy(rep.message, "Add account failed");
				}
                else {
					char buf[50];
                    sprintf(buf,"Account id: %d \n",add);
                    strcpy(rep.message,buf);	
				}
                send_response(&rep);
			}
		}
	}
}

void client_process(){
        key_t key = ftok("bank", 1);
        init_ipc(key, 0);
	Client* cli = NULL;
        Request req;
        Response rep;
        while (cli==NULL){
		printf("Welcome i'm the Client process please login \n");
		printf("please enter your the id associated with your client space or enter -1 if you are new \n");
		int acc_id;
		scanf("%d", &acc_id);
		if (acc_id==-1){
			char acc_name[MAX_NAME];
			printf("Please enter your name\n");
			scanf(" %49[^\n]", acc_name);
            		add_client(acc_name);
			cli = &client_head->client;
			acc_id = cli->client_id;
			printf("Account created. Your client ID is %d\n", acc_id);
		}
		else {
			cli=find_client(acc_id);
			if (!cli) {
                		printf("Client ID not found. Try again.\n");
                		continue;
			}
		}
	}
	while (1) {
		printf("Hello %s (ID: %d)\n", cli->client_name, cli->client_id);
		printf("What do you want to do ? (D)eposit, (W)ithdraw, (T)ransfer, (A)dd account, (C)heck balance \n");
		char answ;
		scanf(" %c",&answ);
		memset(&req, 0, sizeof(req));
		if (answ=='D' || answ=='d'){
			printf("Enter the id of the account where you want to deposit \n");
			int id;
			scanf("%d",&id);
			printf("Enter the amount to deposit \n");
			int amt;
			scanf("%d",&amt);
			req.type=0;
			req.source_id=cli->client_id;
			req.target_id=id;
			req.amount=amt;
			send_request(&req);
		}
		else if (answ=='W' || answ=='w'){
			printf("Enter the id of the account where you want to withdraw \n");
                        int id;
                        scanf("%d",&id);
                        printf("Enter the amount to withdraw \n");
                        int amt;
                        scanf("%d",&amt);
	     		req.type=1;
                        req.source_id=cli->client_id;
                        req.target_id=id;
                        req.amount=amt;
                        send_request(&req);
                }
		else if (answ=='T' || answ=='t'){
			printf("Enter the id of the account where you want to take the money \n");
                        int sid;
                        scanf("%d",&sid);
			printf("Enter the id of the account where you want to send the money \n");
                        int did;
                        scanf("%d",&did);
                        printf("Enter the ammount of money to send \n");
                        int amt;
                        scanf("%d",&amt);
                        req.type=2;
                        req.source_id=cli->client_id;
                        req.target_id=sid;
			req.dest_id=did;
                        req.amount=amt;
                        send_request(&req);
                }
		else if (answ=='C' || answ=='c'){
			printf("Enter the id of the account where you want to check the balance \n");
                        int id;
                        scanf("%d",&id);
			req.type=3;
                        req.source_id=cli->client_id;
                        req.target_id=id;
                        send_request(&req);
                }
		else if (answ=='A' || answ=='a'){
			char acc_name[MAX_NAME];
			printf("Enter the name of the account you want to create \n");
			scanf(" %49[^\n]", acc_name);
                        req.type=4;
			strncpy(req.name, acc_name, sizeof(req.name));
			req.name[sizeof(req.name)-1] = '\0';
                        req.source_id=cli->client_id;
                        send_request(&req);
                }
		else {
			printf("invalid command \n");
			continue;
		}
		receive_response(&rep,cli->client_id);
		printf("Bank reply: %s", rep.message);
	}
}
