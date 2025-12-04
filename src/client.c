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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

//I define the head of the linked list
ClientList* client_head = NULL;


//A function to create a new client
int add_client(const char* name){
        ClientList* new_client=(ClientList*) malloc(sizeof(ClientList));
        if (new_client == NULL){
                perror("Failled to malloc for new client");
                exit(1);
        }

        // Open (or create) a named semaphore to act as a mutex
        sem_t *mutex = sem_open("/client_file_mutex", O_CREAT, 0644, 1);
        if (mutex == SEM_FAILED) {
            perror("Failed to open semaphore");
            free(new_client);
            exit(1);
        }

        // Lock the mutex
        if (sem_wait(mutex) == -1) {
            perror("Failed to lock semaphore");
            sem_close(mutex);
            free(new_client);
            exit(1);
        }

        FILE* file = fopen("account.txt", "a+");
        if (file == NULL) {
            perror("Failed to open account.txt");
            sem_post(mutex);
            sem_close(mutex);
            free(new_client);
            exit(1);
        }

        int last_id = -1;
        char line[256];
        int temp_id;
        char temp_name[50];

        fseek(file, 0, SEEK_SET);
        while (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "%d %s", &temp_id, temp_name) == 2) {
                last_id = temp_id;
            }
        }

        int new_id = last_id + 1;

        fseek(file, 0, SEEK_END);
        fprintf(file, "%d %s\n", new_id, name);
        fflush(file);

        fclose(file);

        // Unlock the mutex
        if (sem_post(mutex) == -1) {
            perror("Failed to unlock semaphore");
        }
        sem_close(mutex);

        new_client->client.client_id = new_id;
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


