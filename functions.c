#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "header.h"

/**
 * @brief SEMAPHORE UP
 * @param semid: semaphore id
 * @return void
 */
void semUp(int semid) 
{
    struct sembuf semopr;
    semopr.sem_num = 0;
    semopr.sem_op = 1;
    semopr.sem_flg = 0;
    semop(semid, &semopr, 1);
}

/**
 * @brief SEMAPHORE DOWN
 * @param semid: semaphore id
 * @return void
 */
void semDown(int semid)
{
    struct sembuf semopr;
    semopr.sem_num = 0;
    semopr.sem_op = -1;
    semopr.sem_flg = 0;
    semop(semid, &semopr, 1);
}

/**
 * @brief Returns the number of lines in a file
 * @param fileName: The file name
 * @return An integer depicting the total of lines 
 */
int getLineNumber(char *fileName)
{
    int lineNumber = 0;
    char line[255];

    FILE* fp;

    fp = fopen(fileName, "r");

    if(fp == NULL)
        return -1;

    do {
        if(fgets(line, 255, fp) != NULL)
            lineNumber++;
        else
            break;
    } while(1);

    if(fclose(fp) != 0)
        return -1;

    return lineNumber;
}

/**
 * @brief Reads a specific line from file 
 * @param lineNumber: The line number to read
 * @param fileName: The file name
 * @param totalLines: The total lines of the file
 * @return Returns the line read
 */
char* readLine(int lineNumber, char *fileName, int totalLines, char *buffer)
{
    int lineMatch = 0;
    FILE* fp;
    
    fp = fopen(fileName, "r");

    if(fp == NULL)
        return "ERROR";

    do {
        memset(buffer, 0, 255 * (sizeof buffer[0]));
        
        if(fgets(buffer, 255, fp) != NULL)
            lineMatch++;
        else
            break;
        
        if(lineMatch == lineNumber)
            break;
    } while(1);

    if(fclose(fp) != 0)
        return "ERROR";

    /*Removing '\n' from line if it's not the last line*/
    if (lineMatch != totalLines)
        buffer[(unsigned) strlen(buffer)-1] = 0;
    
    return buffer;
}

/**
 * @brief Gets a line and returns the same line with all letters in CAPITALS
 * @param line: A pointer to an array of characters
 * @return The capitalized line
 */
char* capitalizeLine(char *line)
{
    int i;
    
    for (i = 0; i < (unsigned) strlen(line); i++)
        line[i] = toupper(line[i]);
    
    return line;
}