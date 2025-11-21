#ifndef ACCOUNT_UTILS_H
#define ACCOUNT_UTILS_H

#include "client.h"
#include "bank.h"

// Find client by name
Client* find_client_by_name(const char* name);

// Find all accounts for a given client
void show_client_accounts(int client_id);

// Display a single account's details
void show_account_details(Client* client, Account* account);

#endif
