#ifndef BANK_H
#define BANK_H

#include <pthread.h> 



//The structure of an account
typedef struct {
        int account_id;
        int owner_id;
        int balance;
        char account_name[50];
} Account;


//I decided to created a linked list to navigate between accounts
typedef struct AccountList {
        Account account;
        struct AccountList* next;
} AccountList;

// I define the head of the linked list
extern AccountList* head;
extern int next_id;
extern pthread_mutex_t bank_mutex;

int deposit(int account_id,int X);
int withdraw(int account_id,int X);
int transfer(int source_id,int dest_id,int X);
int add_account(int owner_id,const char *name);
Account* find_account(int target_id);
int get_balance(int account_id);

typedef struct {
        int client_id;
        char client_name[50];
    } BankClient;
    
    typedef struct BankClientList {
        BankClient client;
        struct BankClientList* next;
    } BankClientList;


// Client management variables
extern BankClientList* client_list_head;
extern int next_bank_client_id;  

int bank_add_client(const char* name);
int bank_find_client_by_name(const char* name);

#endif

