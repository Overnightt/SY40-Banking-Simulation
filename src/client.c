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

//I define the head of the linked list
ClientList* client_head = NULL;

//I initialize the next id at 0
int next_client_id = 0;

//A function to create a new client
int add_client(const char* name){
        ClientList* new_client=(ClientList*) malloc(sizeof(ClientList));
        if (new_client == NULL){
                perror("Failled to malloc for new client");
                exit(1);
        }
        new_client->client.client_id = next_client_id;
        next_client_id= next_client_id+1;
        strncpy(new_client->client.client_name,name,50);
        new_client->next=client_head;
        client_head= new_client;
        return 0;
}


//A function that returns a pointer to the client with the target id
Client* find_client(int client_id){
        ClientList* current=client_head;
        while (current !=NULL){
                if (current->client.client_id == client_id){
                        return &current->client;
                }
                else{
                        current=current->next;
                }
        }
        return NULL;
}
