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

//I define the head of the linked list
AccountList* head = NULL;

//I initialize the next id at 0
int next_id = 0;

//Deposit X amount of money inside an account
int deposit(int account_id,int X){
	Account* acc = find_account(account_id);
        if (acc==NULL){
                return -1;
        }
	else {
		acc->balance+=X;
	}
	return 0;
}

//Checks if the amount withdrawn dosen't exceed the balance and then withdraw the money
int withdraw(int account_id,int X){
 	Account* acc = find_account(account_id);
        if (acc==NULL){
                return -1;
        }
	if (acc->balance<X){
		printf("Amount exceeds the current account balance");
		return -1;
	}
	else {
		acc->balance-=X;
	}
	return 0;
}

//Uses the deposit and withdraw to make a transfer
int transfer(int source_id,int dest_id,int X){
	int res=withdraw(source_id,X);
	if (res!=0){
		return -1;
	}
	else {
		deposit(dest_id,X);
	}
	return 0;
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
	AccountList* new_account=(AccountList*) malloc(sizeof(AccountList));
	if (new_account == NULL){
		perror("Failled to malloc for new account");
		exit;
	}
	new_account->account.account_id = next_id;
	next_id= next_id+1;
	new_account->account.owner_id = client_id;
	new_account->account.balance=0;
	strncpy(new_account->account.account_name,name,50);
	new_account->next=head;
	head= new_account;
	return 0;
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
