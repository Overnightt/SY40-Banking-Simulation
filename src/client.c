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

//I define the head of the linked list
ClientList* client_head = NULL;



//A function to create a new client
int add_client(const char* name){
        ClientList* new_client=(ClientList*) malloc(sizeof(ClientList));
        if (new_client == NULL){
                perror("Failled to malloc for new client");
                exit(1);
        }

        FILE* file = fopen("account.txt", "a+");
        if (file == NULL) {
            perror("Failed to open account.txt");
            free(new_client);
            exit(1);
        }

        int fd = fileno(file);
        struct flock lock;
        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;

        if (fcntl(fd, F_SETLKW, &lock) == -1) {
            perror("Failed to lock account.txt");
            fclose(file);
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

        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        fclose(file);

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


