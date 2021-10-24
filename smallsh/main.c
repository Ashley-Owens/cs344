// Name: Ashley Owens
// Date: 11/01/2021
// Program 3: Smallsh

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

/* 
*  Constant declarations: define shell input sizes
*/
#define MAX_LENGTH 2048
#define MAX_ARGS 512


/* 
*  Function declarations
*/
int getCommandInput(char** input, int* numOfArgs);


/*
*   Main Function: *****
*/
int main() {
    // Initializes variables for handling user input
    char* input[MAX_ARGS];
    int numOfArgs;
    // char inputFilename[256];
    // char outputFilename[256];

    numOfArgs = 0;
    memset(input, '\0', MAX_ARGS);
    fflush(stdout);
    fflush(stdin);
    getCommandInput(input, &numOfArgs);
    printf("num of args: %d\n", numOfArgs);
    printf("input is: %s\n", *input);
    for (int i=0; i < numOfArgs; i++) {
        printf("input is: %s\n", input[i]);
    }
    printf("command input received\n");
    
    return EXIT_SUCCESS;
}


int getCommandInput(char** input, int* numOfArgs) {
    // initialize variables to help parse input
    char buffer[MAX_ARGS];

    // Prompts and adds user input into buffer array
    do {
        printf(": ");
        fgets(buffer, MAX_LENGTH, stdin);               // Stores input into buffer array
        strtok(buffer, "\n");                           // Removes the newline char
    } while (buffer[0] == '\n' || buffer[0] == '#');    // Ignores empty lines and code comments

    // Parses first argument into input array
    char* token = strtok(buffer, " ");
    
    // Continues parsing each argument and adding to array
    while (token != NULL) {
        input[*numOfArgs] = strdup(token);
        printf("token is:  %s\n", token);
        token = strtok(NULL, " ");	
        ++*numOfArgs;
        printf("numOfArgs is: %d\n", *numOfArgs);
    }
    return 0;
}