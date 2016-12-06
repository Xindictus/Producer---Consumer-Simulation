# Producer-Consumer-Simulation
- - -
A program simulating the communication between a number of P (producer) processes and a single C (consumer) process for K transactions. For each transaction:

* A P process reads a file, chooses a random line and writes it to the shared memory. 
* C process reads from the shared memory, capitalizes the line and writes it back. 
* P process reads from the shared memory and prints the message forwarded by C. If PID of sender process matches with its own, a counter is incremented.

At the end of the transactions, each counter from every P process is added together and forwarded towards the parent process, which prints the statistics (percentage of P processes who received the line they sent to C capitalized).
- - -
Makefile is used to compile the program.

Type `make` to compile and create the executable.

Run with `./main -N <Pprocesses> -K <transactions>`

-N: the number of P processes to be created

-K: the number of transactions between P and C.
