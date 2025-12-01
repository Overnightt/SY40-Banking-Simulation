CC = gcc
CFLAGS = -Wall -Wextra -pthread -g -Iinclude
LDFLAGS = -pthread

SRC_DIR = src
OBJ_DIR = build
INCLUDE_DIR = include

# Création du dossier build si inexistant
$(shell mkdir -p $(OBJ_DIR))

# Fichiers sources
IPC_SRC = $(SRC_DIR)/ipc.c
BANK_SRC = $(SRC_DIR)/bank.c $(SRC_DIR)/bank_process.c $(IPC_SRC)
CLIENT_SRC = $(SRC_DIR)/main.c $(SRC_DIR)/ui.c $(SRC_DIR)/client.c $(SRC_DIR)/account_utils.c $(IPC_SRC)

# Fichiers objets
BANK_OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(BANK_SRC))
CLIENT_OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CLIENT_SRC))

# Noms des exécutables
BANK_TARGET = bank_server
CLIENT_TARGET = client_app

# Commandes principales
all: server client

server: $(BANK_TARGET)
client: $(CLIENT_TARGET)

# Compilation des exécutables
$(BANK_TARGET): $(BANK_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Compilation des .c en .o dans build/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BANK_TARGET) $(CLIENT_TARGET)

rebuild: clean all

.PHONY: all server client clean rebuild
