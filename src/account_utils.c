#include "account_utils.h"
#include "client.h"
#include <stdio.h>
#include <string.h>

Client* find_client_by_name(const char* name) {
    ClientList* current = client_head;
    while (current != NULL) {
        if (strcmp(current->client.client_name, name) == 0) {
            return &current->client;
        }
        current = current->next;
    }
    return NULL;
}

void show_client_accounts(int client_id) {
    Client* client = find_client(client_id);
    if (client == NULL) {
        printf("Client not found.\n");
        return;
    }
    
    printf("\n--- Your Accounts ---\n");
    if (client->num_accounts == 0) {
        printf("No accounts yet.\n");
    } else {
        for (int i = 0; i < client->num_accounts; i++) {
            int acc_id = client->account_ids[i];
            int balance = client_get_balance(client_id, acc_id);
            if (balance >= 0) {
                printf("  Account #%d: Balance = %d\n", acc_id, balance);
            }
        }
    }
    printf("---------------------\n");
}

void show_account_details(Client* client, Account* account) {
    printf("\n--- Account Info ---\n");
    printf("Client Name: %s\n", client->client_name);
    printf("Client ID: %d\n", client->client_id);
    printf("Account Name: %s\n", account->account_name);
    printf("Account ID: %d\n", account->account_id);
    printf("Balance: %d\n", account->balance);
}
