#include "bank.h"
#include "ipc.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

volatile bool running = true;

// Gérer Ctrl+C pour arrêt propre
void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n Shutting down bank...\n");
        running = false;
    }
}

// Traiter une requête et générer une réponse
void process_request(Request* req, Response* rep) {
    rep->target_id = req->source_id;
    rep->status = 0;
    rep->result = 0;
    rep->message[0] = '\0';

    printf("DEBUG process_request: req->source_id=%d, rep->target_id=%d\n",
        req->source_id, rep->target_id);
    
    switch (req->type) {
        case OP_REGISTER_CLIENT:
            pthread_mutex_lock(&bank_mutex);
            rep->result = bank_add_client(req->name);
            pthread_mutex_unlock(&bank_mutex);
            
            if (rep->result >= 0) {
                rep->status = 0;
                rep->target_id = 0;  // still 0, client hasn’t got ID yet
                rep->mtype = CLIENT_MTYPE(0);  // send response to type 2

                snprintf(rep->message, 100, "Client registered with ID %d", rep->result);
            } else {
                rep->status = -1;
                snprintf(rep->message, 100, "Failed to register client");
            }
            break;
            
        case OP_LOGIN_CLIENT:
            pthread_mutex_lock(&bank_mutex);
            rep->result = bank_find_client_by_name(req->name);
            pthread_mutex_unlock(&bank_mutex);
            
            if (rep->result >= 0) {
                rep->status = 0;
                rep->target_id = rep->result;  // Le client_id existant
                snprintf(rep->message, 100, "Login successful, client ID %d", rep->result);
            } else {
                rep->status = -1;
                snprintf(rep->message, 100, "Client not found");
            }
            break;    

        case OP_DEPOSIT:
            rep->status = deposit(req->target_id, req->amount);
            if (rep->status == 0) {
                rep->result = get_balance(req->target_id);
                snprintf(rep->message, 100, "Deposited %d. New balance: %d",
                        req->amount, rep->result);
            } else {
                snprintf(rep->message, 100, "Deposit failed");
            }
            break;
            
        case OP_WITHDRAW:
            rep->status = withdraw(req->target_id, req->amount);
            if (rep->status == 0) {
                rep->result = get_balance(req->target_id);
                snprintf(rep->message, 100, "Withdrew %d. New balance: %d",
                        req->amount, rep->result);
            } else {
                snprintf(rep->message, 100, "Insufficient funds or error");
            }
            break;
            
        case OP_TRANSFER:
            rep->status = transfer(req->target_id, req->dest_id, req->amount);
            if (rep->status == 0) {
                snprintf(rep->message, 100, "Transferred %d from %d to %d",
                        req->amount, req->target_id, req->dest_id);
            } else {
                snprintf(rep->message, 100, "Transfer failed");
            }
            break;
            
        case OP_GET_BALANCE:
            rep->result = get_balance(req->target_id);
            if (rep->result >= 0) {
                rep->status = 0;
                snprintf(rep->message, 100, "Balance: %d", rep->result);
            } else {
                rep->status = -1;
                snprintf(rep->message, 100, "Account not found");
            }
            break;
            
        case OP_CREATE_ACCOUNT:
            rep->result = add_account(req->source_id, req->name);
            if (rep->result >= 0) {
                rep->status = 0;
                snprintf(rep->message, 100, "Account created with ID %d", rep->result);
            } else {
                rep->status = -1;
                snprintf(rep->message, 100, "Account creation failed");
            }
            break;
            
        case OP_FETCH_ACCOUNTS: {
            pthread_mutex_lock(&bank_mutex);
            
                // Find all accounts belonging to the client
            AccountList* current = head;
            char buffer[256] = "";
            char tmp[16];
            int count = 0;
            
            while (current != NULL) {
                if (current->account.owner_id == req->source_id) {
                    snprintf(tmp, sizeof(tmp), "%d ", current->account.account_id);
                    strncat(buffer, tmp, sizeof(buffer) - strlen(buffer) - 1);
                    count++;
                }
                current = current->next;
            }
            
            pthread_mutex_unlock(&bank_mutex);
            
            if (count > 0) {
                rep->status = 0;
                rep->result = count;           // number of accounts
                strncpy(rep->message, buffer, sizeof(rep->message));
                rep->message[sizeof(rep->message)-1] = '\0';
            } else {
                rep->status = -1;
                rep->result = 0;
                snprintf(rep->message, sizeof(rep->message), "No accounts found");
                }
            } 
            break;

        default:
            rep->status = -1;
            snprintf(rep->message, 100, "Unknown operation");
    }
}

int main() {
    // Installer le gestionnaire de signal
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    printf(" SY40 Bank Server Starting...\n");
    
    
    // Initialiser IPC (mode serveur)
    if (init_ipc(IPC_KEY, 1) != 0) {
        fprintf(stderr, " Failed to initialize IPC\n");
        return 1;
    }
    
    printf(" Bank server ready. Waiting for requests...\n");
    
    // Boucle principale
    Request req;
    Response rep;
    
    while (running) {
        // Recevoir une requête
        if (receive_request(&req) != 0) {
            if (!running) break;  // Arrêt demandé
            fprintf(stderr, "Error receiving request\n");
            continue;
        }
        
        printf("\n Received request from client %d\n", req.source_id);
        print_request(&req);
        
        // Traiter la requête
        process_request(&req, &rep);
        
        // Envoyer la réponse
        if (send_response(&rep) != 0) {
            fprintf(stderr, "Error sending response\n");
        } else {
            printf(" Sent response\n");
            print_response(&rep);
        }
    }
    
    // Nettoyage
    printf("\n Cleaning up...\n");
    cleanup_ipc(1);  // Supprimer la queue
    pthread_mutex_destroy(&bank_mutex);
    
    printf("Bank server stopped.\n");
    return 0;
}