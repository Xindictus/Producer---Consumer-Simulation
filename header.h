#ifndef __HEADER_H__
#define __HEADER_H__

/* Structs in_ds and out_ds are practically the same, 
 * but declared twice for clarification reasons. */
typedef struct in_ds {
	int p_pid;
	char textLine[255];
} In_ds;

typedef struct out_ds {
	int s_pid;
	char textLine[255];
} Out_ds;

union semun {
	int val;
	struct semid_ds* buf;
	unsigned short *array;
};

/* The functions for the up/down of semaphores */
void semUp(int);
void semDown(int);

/* Functions used to read lines from a file and capitalize a line */
int getLineNumber(char *);
char* readLine(int, char *, int, char *);
char* capitalizeLine(char *);

#endif