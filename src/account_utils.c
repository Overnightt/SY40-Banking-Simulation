#include "account_utils.h"
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
    printf("\nYour Accounts:\n");
    AccountList* current = head;
    int found = 0;
    while (current != NULL) {
        if (current->account.owner_id == client_id) {
            printf("Account Name: %s | Account ID: %d | Balance: %d\n",
                   current->account.account_name,
                   current->account.account_id,
                   current->account.balance);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("No accounts found.\n");
    }
}

void show_account_details(Client* client, Account* account) {
    printf("\n--- Account Info ---\n");
    printf("Client Name: %s\n", client->client_name);
    printf("Client ID: %d\n", client->client_id);
    printf("Account Name: %s\n", account->account_name);
    printf("Account ID: %d\n", account->account_id);
    printf("Balance: %d\n", account->balance);
}
