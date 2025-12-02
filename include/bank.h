#ifndef BANK_H
#define BANK_H

//The structure of an account
typedef struct {
        int account_id;
        int owner_id;
        int balance;
        char account_name[50];
} Account;

//A global counter for the account id
extern int next_id;

//I decided to created a linked list to navigate between accounts
typedef struct AccountList {
        Account account;
        struct AccountList* next;
} AccountList;

// I define the head of the linked list
extern AccountList* head;

int deposit(int account_id,int X);
int withdraw(int account_id,int X);
int transfer(int source_id,int dest_id,int X);
int add_account(int owner_id,const char *name);
Account* find_account(int target_id);
int get_balance(int account_id);
int is_owner(int client_id, int account_id);

#endif

