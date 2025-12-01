#include "ui.h"
#include "client.h"
#include "account_utils.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME 50

void user_menu(Client* client) {
    while (1) {
        printf("\n--- Welcome %s! ---\n", client->client_name);
        show_client_accounts(client->client_id);

        printf("\nSelect an account by ID to operate on,\n");
        printf("or -1 to logout, or -2 to create a new account: ");
        int acc_id;
        scanf("%d", &acc_id);

        if (acc_id == -1) {
            printf("Logging out...\n");
            break;
        } 
        else if (acc_id == -2) {
            char acc_name[MAX_NAME];
            printf("Enter a name for your new account: ");
            scanf(" %49[^\n]", acc_name);
            
            int new_acc_id = client_create_account(client->client_id, acc_name);
            if (new_acc_id >= 0) {
                printf("Account '%s' created successfully with ID %d!\n", acc_name, new_acc_id);
            }
            continue;
        }

        // Vérifier que le compte appartient au client
        int found = 0;
        for (int i = 0; i < client->num_accounts; i++) {
            if (client->account_ids[i] == acc_id) {
                found = 1;
                break;
            }
        }
        
        if (!found) {
            printf("Invalid account ID. Try again.\n");
            continue;
        }

        int choice;
        while (1) {
            printf("\n--- Account #%d Menu ---\n", acc_id);
            printf("1. Deposit\n");
            printf("2. Withdraw\n");
            printf("3. Transfer\n");
            printf("4. Check Balance\n");
            printf("5. Switch account\n");
            printf("6. Logout\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            if (choice == 1) {
                int amount;
                printf("Enter deposit amount: ");
                scanf("%d", &amount);
                
                if (client_deposit(client->client_id, acc_id, amount) >= 0) {
                    printf("Deposit successful.\n");
                } else {
                    printf("Deposit failed.\n");
                }
            } 
            else if (choice == 2) {
                int amount;
                printf("Enter withdrawal amount: ");
                scanf("%d", &amount);
                
                if (client_withdraw(client->client_id, acc_id, amount) >= 0) {
                    printf("Withdrawal successful.\n");
                } else {
                    printf("Failed to withdraw.\n");
                }
            } 
            else if (choice == 3) {
                int dest_id, amount;
                printf("Enter destination account ID: ");
                scanf("%d", &dest_id);
                printf("Enter amount to transfer: ");
                scanf("%d", &amount);
                
                if (client_transfer(client->client_id, acc_id, dest_id, amount) == 0) {
                    printf("Transfer successful.\n");
                } else {
                    printf("Transfer failed.\n");
                }
            }
            else if (choice == 4) {
                int balance = client_get_balance(client->client_id, acc_id);
                if (balance >= 0) {
                    printf("Current balance: %d\n", balance);
                } else {
                    printf("Failed to get balance.\n");
                }
            }
            else if (choice == 5) { 
                break;
            } 
            else if (choice == 6) { 
                return;
            } 
            else {
                printf("Invalid choice.\n");
            }
        }
    }
}