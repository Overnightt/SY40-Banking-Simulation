#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "ipc.h"
#include "bank.h"
#include "client.h"
#include "account_utils.h"
#include "ui.h"

#define MAX_NAME 50

int main() {

     // Initialiser le système client
     if (init_clients() != 0) {
        fprintf(stderr, "Failed to initialize client system\n");
        return 1;
    }

    // Se connecter à la banque (mode client, create=0)
    if (init_ipc(IPC_KEY, 0) != 0) {  // 0 = mode client
        fprintf(stderr, "Failed to connect to bank. Is the bank server running?\n");
        cleanup_clients();
        return 1;
    }

    printf("----- Connected to bank server -----\n");

    char choice;
    char name[MAX_NAME];

    

    while (1) { // top-level loop
        printf("\n=== Welcome to SY40 Banking Simulation! ===\n");
        printf("\n-> Do you want to (C)reate a new client, (L)og in, or (Q)uit? ");
        scanf(" %c", &choice);

        // Nettoyer le buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (choice == 'C' || choice == 'c') {
            printf("Enter your name: ");
            if (fgets(name, sizeof(name), stdin) != NULL) {
                name[strcspn(name, "\n")] = '\0';  // Enlever \n
                
                // S'enregistrer auprès de la BANQUE (pas localement)
                int client_id = client_register(name);
                if (client_id < 0) {
                    printf("Failed to register client.\n");
                    continue;
                }
                
                // Ajouter le client localement si absent
                Client* client = find_client(client_id);
                if (!client) {
                    add_client_with_id(client_id, name);
                    client = find_client(client_id);
                }
    
                char acc_name[MAX_NAME];
                printf("Enter name for your first account: ");
                if (fgets(acc_name, sizeof(acc_name), stdin) != NULL) {
                    acc_name[strcspn(acc_name, "\n")] = '\0';
                    
                    int account_id = client_create_account(client_id, acc_name);
                    if (account_id >= 0) {
                        client->account_ids[client->num_accounts++] = account_id;
                        printf(" Client and account created successfully!\n");
                        user_menu(client);
                    } else {
                        printf("Failed to create account.\n");
                    }
                }
                free(client);
            }
        
        }  else if (choice == 'L' || choice == 'l') {
            printf("Enter your name to log in: ");
            if (fgets(name, sizeof(name), stdin) != NULL) {
                name[strcspn(name, "\n")] = '\0';
            
                // Se connecter auprès de la BANQUE
                int client_id = client_login(name);
                if (client_id < 0) {
                    printf(" Client not found ! Please create an account first.\n");
                } else {
                     // Add client to local list if not present
                    add_client(name);
                    Client* client = find_client(client_id);
                    if (!client) {
                        fprintf(stderr, "Failed to find client locally after login.\n");
                        continue;
                    }
                    
                    // Récupérer la liste des comptes depuis la banque
                    if (client_fetch_accounts(client_id, client) < 0) {
                        printf("Failed to retrieve accounts from bank.\n");
                    } else {
                        printf("Fetched %d account(s) from bank.\n", client->num_accounts);
                    }
                    printf("=== Welcome back, %s! ===\n", client->client_name);
                    user_menu(client);
                    
                    free(client);
                }
            }
        }
        
        else if (choice == 'Q' || choice == 'q') {
            printf("Exiting program. Goodbye!\n");
            break;
        }
        else {
            printf("Invalid choice. Please try again.\n");
        }
    }
    // Nettoyage
    cleanup_ipc(0);  // Ne pas supprimer la queue (rôle de la banque)
    cleanup_clients();    
    
    return 0;

}
