#ifndef CLIENT_H
#define CLIENT_H

//the structure of a client
typedef struct {
        int client_id;
        char client_name[50];
} Client;



//I chose to make another linked list to navigate between the clients
typedef struct ClientList {
	Client client;
	struct ClientList* next;
}ClientList;

// I define the head of the linked list
extern ClientList* client_head;

int add_client(const char* name);
Client* find_client(int client_id);
#endif

