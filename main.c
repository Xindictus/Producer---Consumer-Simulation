#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/msg.h>
#include <time.h>

#include "header.h"

#define RANDOM_FILE "file.txt"

int main (int argc, char *argv[])
{		
	int param_flag = 0, N, K, shmIn, shmOut, pid, i, semIn_P, semIn_C, semOut_P, semOut_C, 
	status, totalLines, pid_match = 0, totalOfPidMatch = 0;
	char *buff, ftokChar;
	time_t t;
	FILE* fp;
	key_t key[6];
	union semun arg;
	In_ds *in_ds;
	Out_ds *out_ds;

	/* Check number and kind of arguments */
	for(i = 0 ; i < argc ; i++){
		if(strcmp(argv[i] , "-N") == 0){
			N = atoi(argv[i+1]);
			param_flag++;
		}
		else if(strcmp(argv[i] , "-K") == 0){
			K = atoi(argv[i+1]);
			param_flag++;
		}
	}

	/*
		N: Number of processes
		K: Number of transactions
	*/
	if (param_flag != 2) {
		printf("-----------------------------------------------------------\n");
		printf("There is a missing parameter....Syntax is:\n");
		printf("main -N InputNumberOfProcesses -K InputNumberOfTransactions\n");
		printf("-----------------------------------------------------------\n");
		exit(1);
	}

	/* Initializes buffer */
	buff = (char *) malloc(sizeof(char)*255);

	/* Get total lines of text */
	totalLines = getLineNumber(RANDOM_FILE);
	if(totalLines == -1) {
		printf("Error in counting file lines.\n");
		exit(1);
	}

	/* Preparing keys */
	ftokChar = 'a';
	for (i = 0; i < 6; i++) {
		if ((key[i] = ftok("./ftok.txt", ftokChar)) == -1) {
			perror("Cannot create key from ./ftok.txt");
			exit(1);
		}
		ftokChar++;
	}

	//printf("Number of text lines are: %d\n", totalLines);

	/********************************************************/
	/* Creating memory segments */
	if ((shmIn = shmget(key[0], sizeof(In_ds), 0666|IPC_CREAT)) == -1) {
		perror("Error in creating memory segment");
		exit(1);
	}
	if ((shmOut = shmget(key[1], sizeof(Out_ds), 0666|IPC_CREAT)) == -1) {
		perror("Error in creating memory segment");
		exit(1);
	}

	/* Attaching to the segment */
	in_ds = (In_ds *) shmat(shmIn, (void *)0, 0);
	out_ds = (Out_ds *) shmat(shmOut, (void *)0, 0);
	// out_ds = (Out_ds *) (shmat(shmOut, (void *)0, 0) + sizeof(In_ds));

	/* Checking for errors in pointers */
	if (in_ds == (int *)(-1)) {
		perror("Error in getting pointer to the segment for IN_DS");
		exit(1);
	}
	if (out_ds == (int *)(-1)) {
		perror("Error in getting pointer to the segment for OUT_DS");
		exit(1);
	}

	/********************************************************/
	/* Creating semaphore sets */
	semIn_P = semget(key[2], 1, 0666|IPC_CREAT);
	semIn_C = semget(key[3], 1, 0666|IPC_CREAT);
	semOut_P = semget(key[4], 1, 0666|IPC_CREAT);
	semOut_C = semget(key[5], 1, 0666|IPC_CREAT);

	/* Initializing semaphores */
	arg.val = 1;
	semctl(semIn_P, 0, SETVAL, arg);
	semctl(semOut_C, 0, SETVAL, arg);

	arg.val = 0;
	semctl(semIn_C, 0, SETVAL, arg);
	semctl(semOut_P, 0, SETVAL, arg);

	/********************************************************/

	/* CREATING N PROCESSES (P) */
	pid = getpid();
	for (i = 0; i < N; i++) {

		if (pid !=0 )
			pid = fork();

		if (pid < 0) {
			printf("Error in creating N processes\n");
			exit(1);
		} else if (pid == 0) {

			/* Intializes random number generator */
			srand(time(NULL) ^ (getpid()<<16));

			/* PID from OUT_DS */
			int p_pid;

			/* Cheks if process has printed */

			while(1) {

				/* Get a random line number */
				int randomLine = rand() % totalLines + 1;

				/* Read the random line */
				readLine(randomLine, RANDOM_FILE, totalLines, buff);

				if (buff != NULL) {
					/* Block other P processes from writing in IN_DS*/
					semDown(semIn_P);

					/* Write to IN_DS */
					in_ds->p_pid = getpid();
					strncpy(in_ds->textLine, buff, 255);

					/* Allow C to read IN_DS*/
					semUp(semIn_C);
				}
				/*************************************************************/
				/* Reinitialize pointers */
				memset(buff, 0, 255 * (sizeof buff[0]));
				p_pid = 0;

				/* P binds to read OUT_DS */
				semDown(semOut_P);

				/* Read OUT_DS */
				p_pid = out_ds->s_pid;
				strncpy(buff, out_ds->textLine, 255);

				/* Reinitialize OUT_DS helper struct */
				out_ds->s_pid = 0;
				memset(out_ds->textLine, 0, 255 * (sizeof out_ds->textLine[0]));

				/* If PID matches, incremennt pid_match */
				if (p_pid == getpid()) {
					pid_match++;
					printf("------------------------------------------------------\n");
					printf("My PID: %d\nMessage: %s\n", p_pid, buff);
				}

				/* Otherwise, print my PID, sender PID and the message in CAPS */
				else if (p_pid != -300) {
					printf("------------------------------------------------------\n");
					printf("My PID: %d\nSender PID: %d\nMessage: %s\n", getpid(), p_pid, buff);
				}

				/* C can write to OUT_DS again */
				semUp(semOut_C);

				/* If PID equals -300, it is a signal from C to finish */
				if (p_pid == -300)
					break;
				
			}	

			/* If P -> has completed its job */
			if (pid == 0)
				exit(pid_match);

			break;
		} 
	}

	/* Creating C process */
	if (pid != 0) {

		pid = fork();

		if (pid < 0) {
			printf("Error in creating C process");
			exit(1);
		} else if (pid == 0) {

			int s_pid;

			for (i = 0; i < K; i++) {

				/* Reinitialize pointers */
				memset(buff, 0, 255 * (sizeof buff[0]));
				s_pid = 0;

				/* C binds to read IN_DS */
				semDown(semIn_C);
				
				/* Read from IN_DS */
				s_pid = in_ds->p_pid;
				strncpy(buff, in_ds->textLine, 255);

				/* Empty data of IN_DS */
				in_ds->p_pid = 0;
				memset(in_ds->textLine, 0, 255 * (sizeof in_ds->textLine[0]));

				/* P can now write again to IN_DS */
				semUp(semIn_P);
				/*************************************************************/
				/* Changes letters to capitals */
				capitalizeLine(buff);

				/* C binds shared memory to write to OUT_DS */
				semDown(semOut_C);

				/* C writes to OUT_DS */
				out_ds->s_pid = s_pid;
				strncpy(out_ds->textLine, buff, 255);

				/* P can now read OUT_DS */
				semUp(semOut_P);
			}

			/* Sending signals to P processes to finish */
			for (i = 0; i < N; i++) {
				semDown(semIn_C);
				semDown(semOut_C);
				out_ds->s_pid = -300;
				semUp(semIn_P);
				semUp(semOut_P);
			}

			exit(0);
		}
	}

	/********************************************************/
	/* Getting the return code from the P processes and adding it to the total sum */
	for (i = 0; i < N+1; i++) {
		wait(&status);
		if(WIFEXITED(status))
			totalOfPidMatch += WEXITSTATUS(status);
		else {
			printf("Child did not terminate with exit\n");
			exit(1);
		}
	}

	printf("------------------------------------------------------\n");
	printf("Total number of P processes %d\n", N);
	printf("Total number of transactions %d\n", K);
	printf("Total sum of pid matches %d (%.2f%%)\n", 
		totalOfPidMatch, (float) totalOfPidMatch * 100.00 / (float) K);

	/********************************************************/
	/* Destroying semaphores */
	if (semctl(semIn_P, 0, IPC_RMID, 0) == -1) {
		perror("Error in Deleting semaphore 'semIn_P'");
		exit(1);
	}
	if (semctl(semIn_C, 0, IPC_RMID, 0) == -1) {
		perror("Error in Deleting semaphore 'semIn_C'");
		exit(1);
	}
	if (semctl(semOut_P, 0, IPC_RMID, 0) == -1) {
		perror("Error in Deleting semaphore 'semOut_P'");
		exit(1);
	}
	if (semctl(semOut_C, 0, IPC_RMID, 0) == -1) {
		perror("Error in Deleting semaphore 'semOut_C'");
		exit(1);
	}
	/********************************************************/	
	/* Detaching from memory segments and checking for possible errors */
	if (shmdt((void *) in_ds) == -1) {
		perror("Error detaching from IN_DS");
		exit(1);
	}
	if (shmdt((void *) out_ds) == -1) {
		perror("Error detaching from OUT_DS");
		exit(1);
	}

	/* Deleting memory segments */
	if (shmctl(shmIn, IPC_RMID, NULL) == -1) {
		perror("Error deleting memory segment for IN_DS");
		exit(1);
	}
	if (shmctl(shmOut, IPC_RMID, NULL) == -1) {
		perror("Error deleting memory segment for OUT_DS");
		exit(1);
	}
	
	free(buff);
	exit(0);
}