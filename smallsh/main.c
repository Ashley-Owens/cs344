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
    for (int i=0; i < numOfArgs; i++) {
        printf("input is: %s\n", input[i]);
    }
    
    return EXIT_SUCCESS;
}

/*
*   getCommandInput()
*   Performs command line prompt, ignoring newlines and
*   code comments. Adds user input into input array, converts
*   $$ into PID, and caculates number of input arguments.
*/
int getCommandInput(char** input, int* numOfArgs) {
    char buffer[MAX_LENGTH];

    // Command line prompt
    do {
        printf(": ");
        fflush(stdout);
        fgets(buffer, MAX_LENGTH, stdin);               // Stores input into buffer array
        strtok(buffer, "\n");                           // The newline char ends token scan
    } while (buffer[0] == '\n' || buffer[0] == '#');    // Ignores empty lines and code comments


    // Puts PIDs in Buffer string
    for (int i = 1; i < strlen(buffer); i++) {
		// Checks for double dollar signs in the string
		if ((buffer[i - 1] == '$') && (buffer[i] == '$')) {
			char* temp = strdup(buffer);                // Creates a temp copy of buffer string
			temp[i-1] = '%';                            // Replaces $$ with %d for string insertion
			temp[i] = 'd';
			sprintf(buffer, temp, getpid());            // Prints the PID number into the string position
			free(temp);
		}
	}

    // Parses buffer arguments into input array
    char* token = strtok(buffer, " ");
    while (token != NULL) {
        input[*numOfArgs] = token;
        token = strtok(NULL, " ");	
        ++*numOfArgs;
    }
    return 0;
}
