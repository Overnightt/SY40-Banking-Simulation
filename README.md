# SY40-Banking-Simulation

A banking system simulation implemented in **C** that demonstrates inter-process communication (IPC), concurrency, and synchronization using System V message queues and mutexes.

This project was developed as part of the **SY40** course to apply operating system concepts on a larger scale through a practical simulation. It models a bank and multiple client processes that communicate and perform transactions in a concurrent environment.

---

## Project Overview

The simulation consists of two main processes:

- **Bank process** – Manages accounts, processes client requests, and ensures consistency using synchronization mechanisms.
- **Client processes** – Represent bank customers and provide an interactive console interface to send transaction requests.

Clients and the bank communicate through a **System V message queue**, allowing structured request/response handling. The bank supports operations such as:

- Creating bank accounts  
- Depositing money  
- Withdrawing funds  
- Transferring money between accounts  
- Checking account balances  
- Listing client accounts  

The ability for clients to view all accounts belonging to them is implemented and available.  
Client information (IDs and names) is stored persistently in a text file to ensure consistency across processes.

---

## 💡 Features

- IPC communication using System V message queues  
- Concurrent access protection with mutexes and semaphores  
- Dynamic account and client management  
- Request/response based command processing  
- Interactive text-based client interface  

---

## ⚙️ Compilation and Execution

The project is compiled using **GCC** and a provided **Makefile**.

To compile the project:

make

To start the simulation, first launch the bank process:

./bank_simulation bank


Then, in one or more separate terminals, launch client processes:

./bank_simulation client


Multiple clients can run simultaneously and interact with the bank concurrently.

## Technologies Used

- C   
- System V message queues  
- POSIX mutexes and semaphores  
- Git & GitHub  
- LaTeX / Overleaf (documentation)  
- Vim on WSL (development environment)  

---

## Conclusion

This project helped consolidate knowledge of the C programming language and reinforced key operating system concepts studied in class, notably inter-process communication, concurrency management, and synchronization mechanisms such as mutexes.


