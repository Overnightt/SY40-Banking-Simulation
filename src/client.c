/* Implement Client Process */

#include "client.h"
#include "ipc.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;  

//I define the head of the linked list
ClientList* client_head = NULL;

//I initialize the next id at 0
int next_client_id = 0;

int init_clients() {
        client_head = NULL;
        next_client_id = 0;
        return pthread_mutex_init(&client_mutex, NULL);
    }

void cleanup_clients() {
        pthread_mutex_lock(&client_mutex);
        ClientList* current = client_head;
        while (current != NULL) {
            ClientList* temp = current;
            current = current->next;
            free(temp);
        }
        client_head = NULL;
        pthread_mutex_unlock(&client_mutex);
        pthread_mutex_destroy(&client_mutex);
    }    

//A function to create a new client
int add_client(const char* name){
        pthread_mutex_lock(&client_mutex);


        ClientList* new_client=(ClientList*) malloc(sizeof(ClientList));
        if (new_client == NULL){
                perror("Failled to malloc for new client");
                exit(1);
        }
        //Init the new client
        new_client->client.client_id = next_client_id++;
        strncpy(new_client->client.client_name, name, 49);
        new_client->client.client_name[49] = '\0';
        new_client->client.num_accounts = 0; 

        //Add to list
        new_client->next=client_head;
        client_head= new_client;

        int id = new_client->client.client_id;
        pthread_mutex_unlock(&client_mutex);
        return id;
}


//A function that returns a pointer to the client with the target id
Client* find_client(int client_id){
        pthread_mutex_lock(&client_mutex);
        ClientList* current=client_head;
        while (current !=NULL){
                if (current->client.client_id == client_id){
                        pthread_mutex_unlock(&client_mutex);
                        return &current->client;
                }
                else{
                        current=current->next;
                }
        }
        pthread_mutex_unlock(&client_mutex);
        return NULL;
}

int add_account_to_client(int client_id, int account_id) {
        pthread_mutex_lock(&client_mutex);
        printf("DEBUG add_account_to_client: client_id=%d, account_id=%d\n", 
            client_id, account_id);

        ClientList* current = client_head;
        Client* client = NULL;  // find_client gère déjà son mutex
        
        while (current != NULL) {
            if (current->client.client_id == client_id) {
                client = &current->client;
                break;
            }
            current = current->next;
        }

        if (client == NULL) {
            pthread_mutex_unlock(&client_mutex);
            fprintf(stderr, "ERROR: Client %d not found\n", client_id);
            return -1;
        }
        if (client->num_accounts >= MAX_ACCOUNTS_PER_CLIENT) {
            pthread_mutex_unlock(&client_mutex);
            fprintf(stderr, "ERROR: Client %d has too many accounts\n", client_id);
            return -1;
        }

        //Add account into the list account
        client->account_ids[client->num_accounts] = account_id;
        client->num_accounts++;
    
        printf("DEBUG: Account %d added. Client now has %d accounts\n", 
             account_id, client->num_accounts);
    
        pthread_mutex_unlock(&client_mutex);
        return 0;
}   

// Helper: envoyer une requête et recevoir la réponse
static int send_and_receive(Request* req, Response* rep, int client_id) {
        if (send_request(req) != 0) {
            fprintf(stderr, "Failed to send request\n");
            return -1;
        }
        
        if (receive_response(rep, client_id) != 0) {
            fprintf(stderr, "Failed to receive response\n");
            return -1;
        }
        
        return 0;
    }
    
    // Déposer de l'argent
    int client_deposit(int client_id, int account_id, int amount) {
        Request req = {0};
        req.source_id = client_id;
        req.type = OP_DEPOSIT;
        req.target_id = account_id;
        req.amount = amount;
        
        Response rep = {0};
        if (send_and_receive(&req, &rep, client_id) != 0) {
            return -1;
        }
        
        if (rep.status == 0) {
            printf("✓ %s\n", rep.message);
            return rep.result;
        } else {
            fprintf(stderr, "✗ %s\n", rep.message);
            return -1;
        }
    }
    
    // Retirer de l'argent
    int client_withdraw(int client_id, int account_id, int amount) {
        Request req = {0};
        req.source_id = client_id;
        req.type = OP_WITHDRAW;
        req.target_id = account_id;
        req.amount = amount;
        
        Response rep = {0};
        if (send_and_receive(&req, &rep, client_id) != 0) {
            return -1;
        }
        
        if (rep.status == 0) {
            printf("✓ %s\n", rep.message);
            return rep.result;
        } else {
            fprintf(stderr, "✗ %s\n", rep.message);
            return -1;
        }
    }
    
    // Transférer de l'argent
    int client_transfer(int client_id, int source_id, int dest_id, int amount) {
        Request req = {0};
        req.source_id = client_id;
        req.type = OP_TRANSFER;
        req.target_id = source_id;
        req.dest_id = dest_id;
        req.amount = amount;
        
        Response rep = {0};
        if (send_and_receive(&req, &rep, client_id) != 0) {
            return -1;
        }
        
        if (rep.status == 0) {
            printf("✓ %s\n", rep.message);
            return 0;
        } else {
            fprintf(stderr, "✗ %s\n", rep.message);
            return -1;
        }
    }
    
    // Obtenir le solde
    int client_get_balance(int client_id, int account_id) {
        Request req = {0};
        req.source_id = client_id;
        req.type = OP_GET_BALANCE;
        req.target_id = account_id;
        
        Response rep = {0};
        if (send_and_receive(&req, &rep, client_id) != 0) {
            return -1;
        }
        
        if (rep.status == 0) {
            return rep.result;
        } else {
            fprintf(stderr, "✗ %s\n", rep.message);
            return -1;
        }
    }
    
    // Créer un compte
    int client_create_account(int client_id, const char* account_name) {
        
        if (account_name == NULL || strlen(account_name) == 0) {
            fprintf(stderr, "Invalid account name\n");
            return -1;
        }
        
        printf("DEBUG: Creating account for client_id=%d, name=%s\n", 
            client_id, account_name);

        Request req = {0};
        req.source_id = client_id;
        req.type = OP_CREATE_ACCOUNT;
        strncpy(req.name, account_name, 49);
        req.name[49] = '\0';
        
        Response rep = {0};

        printf("DEBUG: Sending request...\n");
        if (send_request(&req) != 0) {
            fprintf(stderr, "Failed to send request\n");
            return -1;
        }

        printf("DEBUG: Request sent, waiting for response...\n");
        if (receive_response(&rep, client_id) != 0) {  
            fprintf(stderr, "Failed to receive response\n");
            return -1;
        }

        printf("DEBUG: Response received! status=%d, result=%d\n", 
            rep.status, rep.result);
        
        if (rep.status == 0) {
            int account_id = rep.result;
            add_account_to_client(client_id, account_id);
            printf("Ok %s\n", rep.message);
            return account_id;
        } else {
            fprintf(stderr, "Error %s\n", rep.message);
            return -1;
        }
    }

    // Enregistrer un nouveau client auprès de la banque
int client_register(const char* name) {
    if (name == NULL || strlen(name) == 0) {
        fprintf(stderr, "Invalid client name\n");
        return -1;
    }
    
    printf("DEBUG: Registering client '%s' with bank...\n", name);
    
    Request req = {0};
    req.mtype = BANK_MTYPE;
    req.source_id = 0;  // Pas encore d'ID
    req.type = OP_REGISTER_CLIENT;
    strncpy(req.name, name, 49);
    req.name[49] = '\0';
    
    Response rep = {0};
    
    if (send_request(&req) != 0) {
        fprintf(stderr, "Failed to send registration request\n");
        return -1;
    }
    
    // Attendre la réponse avec mtype = CLIENT_MTYPE(0)
    if (msgrcv(msg_queue_id, &rep, sizeof(Response) - sizeof(long), 
               CLIENT_MTYPE(0), 0) == -1) {
        perror("Failed to receive registration response");
        return -1;
    }
    
    if (rep.status == 0) {
        int client_id = rep.result;
        printf("✓ Registered with bank as client ID %d\n", client_id);
        return client_id;
    } else {
        fprintf(stderr, "✗ Registration failed: %s\n", rep.message);
        return -1;
    }
}

// Se connecter avec un client existant
int client_login(const char* name) {
    if (name == NULL || strlen(name) == 0) {
        fprintf(stderr, "Invalid client name\n");
        return -1;
    }
    
    printf("DEBUG: Logging in as '%s'...\n", name);
    
    Request req = {0};
    req.mtype = BANK_MTYPE;
    req.source_id = 0;  // Pas encore d'ID
    req.type = OP_LOGIN_CLIENT;
    strncpy(req.name, name, 49);
    req.name[49] = '\0';
    
    Response rep = {0};
    
    if (send_request(&req) != 0) {
        fprintf(stderr, "Failed to send login request\n");
        return -1;
    }
    
    // Attendre la réponse avec mtype = CLIENT_MTYPE(0)
    if (msgrcv(msg_queue_id, &rep, sizeof(Response) - sizeof(long), 
               CLIENT_MTYPE(0), 0) == -1) {
        perror("Failed to receive login response");
        return -1;
    }
    
    if (rep.status == 0) {
        int client_id = rep.result;
        printf("✓ Logged in as client ID %d\n", client_id);
        return client_id;
    } else {
        fprintf(stderr, "✗ Login failed: %s\n", rep.message);
        return -1;
    }
}

int client_fetch_accounts(int client_id, Client* client) {
    Request req = {0};
    req.source_id = client_id;
    req.type = OP_FETCH_ACCOUNTS;

    Response rep = {0};

    if (send_request(&req) != 0) {
        fprintf(stderr, "Failed to send fetch accounts request\n");
        return -1;
    }

    if (receive_response(&rep, client_id) != 0) {
        fprintf(stderr, "Failed to receive fetch accounts response\n");
        return -1;
    }

    if (rep.status != 0) {
        fprintf(stderr, "Error fetching accounts: %s\n", rep.message);
        return -1;
    }

    // Parse les IDs depuis la chaîne message
    client->num_accounts = 0;
    char* token = strtok(rep.message, " ");
    while (token != NULL && client->num_accounts < MAX_ACCOUNTS_PER_CLIENT) {
        client->account_ids[client->num_accounts++] = atoi(token);
        token = strtok(NULL, " ");
    }

    return client->num_accounts;
}

// Add client with a specific ID (from bank)
int add_client_with_id(int client_id, const char* name) {
    pthread_mutex_lock(&client_mutex);

    ClientList* new_client = (ClientList*)malloc(sizeof(ClientList));
    if (!new_client) {
        perror("Failed to malloc for new client");
        exit(1);
    }

    new_client->client.client_id = client_id; // Use bank-assigned ID
    strncpy(new_client->client.client_name, name, 49);
    new_client->client.client_name[49] = '\0';
    new_client->client.num_accounts = 0;

    new_client->next = client_head;
    client_head = new_client;

    pthread_mutex_unlock(&client_mutex);
    return client_id;
}
