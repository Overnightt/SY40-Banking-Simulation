# SY40 Banking Simulation

A robust multi-process banking system simulation developed in C for the SY40 course. This project demonstrates advanced operating system concepts including Inter-Process Communication (IPC), concurrency control, and process synchronization.

## 🚀 Features

### Core Architecture
- **Client-Server Model**: Implemented using distinct processes for the Bank (server) and Clients.
- **Multi-Process Support**: Multiple client processes can run simultaneously and interact with the single bank process.

### Banking Operations
- **Account Management**: Create new accounts linked to a client ID.
- **Transactions**:
  - **Deposit**: Add funds to an account.
  - **Withdraw**: Remove funds (with balance checks).
  - **Transfer**: Securely move funds between two accounts.
  - **Balance Check**: View current account balance.
- **Client Management**:
  - New clients are automatically assigned a unique ID and saved to `account.txt`.
  - Existing clients can log in using their ID.

### Technical Implementation

#### Inter-Process Communication (IPC)
- **Message Queues (System V)**: Used for all communication between Client and Bank processes.
  - **Requests**: Clients send operation requests (type `BANK_MTYPE`) to the bank's queue.
  - **Responses**: The Bank processes requests and sends replies back to specific clients using their unique Client ID as the message type (`CLIENT_MTYPE`).

#### Concurrency & Synchronization
- **Bank Side**:
  - **Thread Safety**: Uses `pthread_mutex_t` to ensure atomic operations on account data, preventing race conditions during concurrent transactions (e.g., simultaneous transfers).
- **Client Side**:
  - **File Locking**: Uses **Named Semaphores** (`/client_file_mutex`) to synchronize access to the `account.txt` file, ensuring safe concurrent client registration.

## 🛠️ Building the Project

The project includes a `Makefile` for easy compilation.

```bash
make
```

This will generate the executable `bank_simulation`.

## 💻 Usage

1. **Start the Bank Process**:
   The bank must be running first to initialize the message queues.
   ```bash
   ./bank_simulation
   ```
   Select **'B'** (or 'b') to start the Bank process.

2. **Start a Client Process**:
   Open a new terminal window and run the same executable.
   ```bash
   ./bank_simulation
   ```
   Select **'C'** (or 'c') to start a Client process.

3. **Client Interaction**:
   - **New User**: Enter `-1` to register. You will be assigned a Client ID.
   - **Existing User**: Enter your Client ID to log in.
   - **Menu Options**:
     - `(A)dd account`: Create a new bank account.
     - `(D)eposit`: Deposit money into an account.
     - `(W)ithdraw`: Withdraw money from an account.
     - `(T)ransfer`: Transfer money between accounts.
     - `(C)heck balance`: View account balance.

## 📂 Project Structure

- **`src/`**: Source code files.
  - `main.c`: Entry point, handles process selection.
  - `bank.c`: Bank process logic (account management, transaction processing).
  - `client.c`: Client process logic (user interface, input handling).
  - `ipc.c`: IPC implementation (message queue setup, send/receive functions).
  - `ui.c`: User interface logic.
- **`include/`**: Header files defining structures and function prototypes.
- **`account.txt`**: Persistent storage for registered clients.

## ⚠️ Notes
- Ensure the Bank process is running before starting any Clients.
- If the program crashes unexpectedly, you may need to manually remove the IPC message queue or the named semaphore (`/dev/shm/sem.client_file_mutex` on Linux/Unix).
