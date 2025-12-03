#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ipc.h"
#include "bank.h"
#include "client.h"
#include "ui.h"

int main(){
    char choice;
    while (1) { // top-level loop
        printf("\n=== Welcome to SY40 Banking Simulation! ===\n");
        printf("Do you want to create the (B)ank process \n");
        printf("or act as a (C)lient process : ");
        scanf(" %c", &choice);
	if (choice == 'B' || choice == 'b') {
		bank_process();
	}
        if (choice == 'C' || choice == 'c') {
		client_process();
        }
	return 0;
    }
}
