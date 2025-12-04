CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g -pthread
SRCS = src/main.c src/bank.c src/client.c src/ipc.c src/ui.c src/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = bank_simulation

.PHONY: all clean

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)