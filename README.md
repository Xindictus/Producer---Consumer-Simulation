# Producer-Consumer-Simulation

A program simulating the communication between a number of P (producer) processes and a single C (consumer) process for K transactions. For each transaction:

* A P process reads a file, chooses a random line and writes it to the shared memory. 
* C process reads from the shared memory, capitalizes the line and writes it back. 
* P process reads from the shared memory and prints the message forwarded by C. If PID of sender process matches with its own, the *match-counter* is incremented.

At the end of the transactions, each counter from every P process is added together and forwarded towards the parent process, which prints the statistics (percentage of P processes who received the line they sent to C capitalized).
- - -
Makefile is used to compile the program.

# Parameters and Running
Type `make` to compile and create the executable.

Run with `./main -N <Pprocesses> -K <transactions>`

`-N`: the number of P processes to be created

`-K`: the number of transactions between P and C.

# Running Examples

![PC Sim Eg.1](https://zwtytg.by3302.livefilestore.com/y3mPMPRf8z-Ij97DPbq6UkCEE_s4LDk9Y7YxI6Y0xMHSpRKnpFi6x22uVPnUb42jG__jcOdbeis02y6aCbr5mkaElGpGgO8rBNWYAMMy2KHFhAfMgvt2dgYei_ircruJVegABwXy9eRvgxJyX8OvxsG6-fcXM9yzdYXumalIql9dUg?width=790&height=211&cropmode=none)

![PC Sim Eg.2](https://zwrxqg.by3302.livefilestore.com/y3mz8iGI2dJ9tVNaINs32vlOYfiVAJzxMjLEsq8gcUaC5oN-SaDAdMbhl_ZdndI_FBsFKJvZuTmuoiNn7zsjDtlkbWWIBVf0bgITqX8YaCZfE2UKUka5OgBnRcQFv04amEeJB0wFq5gIh5j70FW1NE3ToVEHc0_EoC2pma5HeSl5B4?width=790&height=203&cropmode=none)
