#include <stdio.h>
#include "client.h"
#include "ui.h"
#include "account_utils.h"

#define MAX_NAME 50

int main() {
    char choice;
    char name[MAX_NAME];

    while (1) { // top-level loop
        printf("\nWelcome to SY40 Banking Simulation!\n");
        printf("Do you want to (C)reate a new client, (L)og in, or (Q)uit? ");
        scanf(" %c", &choice);

        if (choice == 'C' || choice == 'c') {
            printf("Enter your name: ");
            scanf(" %49[^\n]", name);
            add_client(name);
            Client* client = find_client_by_name(name);

            char acc_name[MAX_NAME];
            printf("Enter name for your first account: ");
            scanf(" %49[^\n]", acc_name);
            add_account(client->client_id, acc_name);

            printf("Client and account created successfully!\n");
            user_menu(client);
        } 
        else if (choice == 'L' || choice == 'l') {
            printf("Enter your name to log in: ");
            scanf(" %49[^\n]", name);
            Client* client = find_client_by_name(name);
            if (client == NULL) {
                printf("Client not found. Please create an account first.\n");
            } else {
                printf("Welcome back, %s!\n", client->client_name);
                user_menu(client);
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

    return 0;
}
