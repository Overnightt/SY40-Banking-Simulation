#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

#define MAX_CLIENT_NAME 50
#define MAX_ACCOUNTS_PER_CLIENT 10

//the structure of a client
typedef struct {
        int client_id;
        char client_name[MAX_CLIENT_NAME];        
        int account_ids[MAX_ACCOUNTS_PER_CLIENT];  
        int num_accounts;     
} Client;

//A global counter for the client id
extern int next_client_id;

//I chose to make another linked list to navigate between the clients
typedef struct ClientList {
	Client client;
	struct ClientList* next;
}ClientList;

// I define the head of the linked list
extern ClientList* client_head;
// Compteur global pour générer les IDs de clients
extern int next_client_id;

//Mutex to protect acces to the client list
extern pthread_mutex_t client_mutex; 

//Init client system
int init_clients(void);
void cleanup_clients(void); 

int add_client(const char* name);
Client* find_client(int client_it);

//Debug
void list_all_clients(void); 

//Accoount management for client
// Ajouter un compte à la liste des comptes d'un client
int add_account_to_client(int client_id, int account_id);
// Obtenir le nombre de comptes d'un client
//int get_client_account_count(int client_id);

// ============================================================================
// BANKING OPERATIONS VIA IPC
// ============================================================================

// Déposer de l'argent sur un compte (via IPC)
int client_deposit(int client_id, int account_id, int amount);  

// Retirer de l'argent d'un compte (via IPC)
int client_withdraw(int client_id, int account_id, int amount); 

// Transférer de l'argent entre deux comptes (via IPC)
int client_transfer(int client_id, int source_id, int dest_id, int amount);  

// Obtenir le solde d'un compte (via IPC)
int client_get_balance(int client_id, int account_id); 

// Créer un nouveau compte (via IPC)
int client_create_account(int client_id, const char* account_name);  

// Register/Login with bank
int client_register(const char* name);
int client_login(const char* name);

int client_fetch_accounts(int client_id, Client* client);

int add_client_with_id(int client_id, const char* name);
#endif

