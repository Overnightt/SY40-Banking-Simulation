#include "ui.h"
#include "account_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
            break; // back to main menu
        } 
        else if (acc_id == -2) {
            // create a new account
            char acc_name[MAX_NAME];
            printf("Enter a name for your new account: ");
            scanf(" %49[^\n]", acc_name);
            add_account(client->client_id, acc_name);
            printf("Account '%s' created successfully!\n", acc_name);
            continue; // show account list again
        }

        Account* selected_acc = find_account(acc_id);
        if (!selected_acc || selected_acc->owner_id != client->client_id) {
            printf("Invalid account ID. Try again.\n");
            continue;
        }

        int choice;
        while (1) { // account-specific operations
            show_account_details(client, selected_acc);
            printf("\n--- Account Menu ---\n");
            printf("1. Deposit\n");
            printf("2. Withdraw\n");
            printf("3. Transfer\n");
            printf("4. Switch account\n");
            printf("5. Logout\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            if (choice == 1) {
                int amount;
                printf("Enter deposit amount: ");
                scanf("%d", &amount);
                if (deposit(acc_id, amount) == 0)
                    printf("Deposit successful.\n");
                else
                    printf("Deposit failed.\n");
            } 
            else if (choice == 2) {
                int amount;
                printf("Enter withdrawal amount: ");
                scanf("%d", &amount);
                if (withdraw(acc_id, amount) == 0)
                    printf("Withdrawal successful.\n");
                else
                    printf("Failed to withdraw.\n");
            } 
            else if (choice == 3) {
                int dest_id, amount;
                printf("Enter destination account ID: ");
                scanf("%d", &dest_id);
                Account* dest_acc = find_account(dest_id);
                if (!dest_acc) {
                    printf("Destination account not found.\n");
                    continue;
                }
                printf("Enter amount to transfer: ");
                scanf("%d", &amount);
                if (transfer(acc_id, dest_id, amount) == 0)
                    printf("Transfer successful.\n");
                else
                    printf("Transfer failed.\n");
            } 
            else if (choice == 4) { 
                break; // switch account
            } 
            else if (choice == 5) { 
                return; // logout
            } 
            else {
                printf("Invalid choice.\n");
            }
        }
    }
}
