/* Implements the bank process */
#include "bank.h"      
#include "ipc.h"       
//#include "utils.h"    
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>   
#include <stdbool.h>   
#include <pthread.h>   
#include <semaphore.h> 

//I create a global mutex that i will use to make sure evvry operation is atomic
pthread_mutex_t bank_mutex;

//I define the head of the linked list
AccountList* head = NULL;

//I initialize the next id at 0
int next_id = 0;

//Deposit X amount of money inside an account
int deposit(int account_id,int X){
	pthread_mutex_lock(&bank_mutex);
	Account* acc = find_account(account_id);
        if (acc==NULL){
		pthread_mutex_unlock(&bank_mutex);
                return -1;
        }
	else {
		acc->balance+=X;
		pthread_mutex_unlock(&bank_mutex);
		return 0;
	}
}

//Checks if the amount withdrawn dosen't exceed the balance and then withdraw the money
int withdraw(int account_id,int X){
	pthread_mutex_lock(&bank_mutex);
 	Account* acc = find_account(account_id);
        if (acc==NULL){
		pthread_mutex_unlock(&bank_mutex);
                return -1;
        }
	if (acc->balance<X){
		printf("Amount exceeds the current account balance \n");
		pthread_mutex_unlock(&bank_mutex);
		return -1;
	}
	else {
		acc->balance-=X;
		pthread_mutex_unlock(&bank_mutex);
		return 0;
	}
}

//I didn't reuse deposit and withdraw to make a transfer because of the mutex
int transfer(int source_id,int dest_id,int X){
	pthread_mutex_lock(&bank_mutex);
    	Account* src = find_account(source_id);
    	Account* dst = find_account(dest_id);
	if (src==NULL || dst==NULL){
		pthread_mutex_unlock(&bank_mutex);
		return -1;
	}
        if (src->balance<X){
               pthread_mutex_unlock(&bank_mutex);
               return -1;
        }
	else {
		src->balance -=X;
		dst->balance +=X;
		pthread_mutex_unlock(&bank_mutex);
		return 0;
	}
}

//Returns the balance of the account
int get_balance(int account_id){
	Account* acc = find_account(account_id);
	if (acc==NULL){
		return -1;
	}
	else{
		return acc->balance;
	}
}

//The clients create a new account with this function 
int add_account(int client_id,const char *name){
	pthread_mutex_lock(&bank_mutex);
	AccountList* new_account=(AccountList*) malloc(sizeof(AccountList));
	if (new_account == NULL){
		perror("Failled to malloc for new account");
		pthread_mutex_unlock(&bank_mutex);
		return -1;
	}
	new_account->account.account_id = next_id;
	next_id= next_id+1;
	new_account->account.owner_id = client_id;
	new_account->account.balance=0;
	strncpy(new_account->account.account_name,name,50);
	new_account->next=head;
	head= new_account;
	pthread_mutex_unlock(&bank_mutex);
	return new_account->account.account_id;
}

//A function that returns a pointer to the account with the target id
Account* find_account(int target_id){
	AccountList* current=head;
	while (current !=NULL){
		if (current->account.account_id == target_id){
			return &current->account;	
		}
		else{
			current=current->next;
		}
	}
	return NULL;
}

int is_owner(int client_id, int account_id) {
	Account* acc = find_account(account_id);
    	if (!acc){ 
		return 0;
	}
    	return acc->owner_id == client_id;
}

// Populates buffer with a list of accounts owned by client_id
void get_client_accounts(int client_id, char* buffer, size_t buffer_size) {
    pthread_mutex_lock(&bank_mutex);
    AccountList* current = head;
    buffer[0] = '\0'; // Initialize empty string
    char temp[100];
    bool found = false;

    while (current != NULL) {
        if (current->account.owner_id == client_id) {
            snprintf(temp, sizeof(temp), "- %s (ID: %d) Balance: %d\n", 
                     current->account.account_name, 
                     current->account.account_id,
					 current->account.balance);
            
            // Check if we have space to append
            if (strlen(buffer) + strlen(temp) < buffer_size) {
                strcat(buffer, temp);
                found = true;
            } else {
                // Buffer full
                break;
            }
        }
        current = current->next;
    }
    
    if (!found) {
        snprintf(buffer, buffer_size, "No accounts found.\n");
    }
    
    pthread_mutex_unlock(&bank_mutex);
}
