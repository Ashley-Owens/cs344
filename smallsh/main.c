// Name: Ashley Owens
// Date: 11/01/2021
// Program 3: Smallsh

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* 
*  Constant declarations: define shell input sizes
*/
#define MAX_LENGTH 2048
#define MAX_ARGS 512

/* 
*  Global variable declarations
*/
int runInBackground;


/* 
*  Function declarations
*/
int getCommandInput(char** input, int* numOfArgs);
int parseCommandInput(char** input, int numOfArgs);



/*
*   Main Function: *****
*/
int main() {
    // Initializes variables for handling user input
    char* input[MAX_ARGS];
    int numOfArgs;
    bool runShell = true;
    // char inputFilename[256];
    // char outputFilename[256];
    
    while (runShell) {
        numOfArgs = 0;
        memset(input, '\0', MAX_ARGS);
        fflush(stdout);
        fflush(stdin);
        getCommandInput(input, &numOfArgs);
        runShell = parseCommandInput(input, numOfArgs);

    }
    return EXIT_SUCCESS;
}

/*
*   getCommandInput()
*   Performs command line prompt, ignores newlines and code comments.
*   Appends stdin to input array, converts $$ into PID, sets the
*   background flag, and caculates number of input arguments.
*/
int getCommandInput(char** input, int* numOfArgs) {
    char buffer[MAX_LENGTH];

    // Command line prompt
    do {
        printf(": ");
        fflush(stdout);
        fgets(buffer, MAX_LENGTH, stdin);               // Stores input into buffer string
        strtok(buffer, "\n");                           // Ends token scan at the newline char
    } while (buffer[0] == '\n' || buffer[0] == '#');    // Ignores empty lines and code comments


    // Puts PID(s) in Buffer string
    for (int i = 1; i < strlen(buffer); i++) {
		// Checks for double dollar signs in the string
		if ((buffer[i - 1] == '$') && (buffer[i] == '$')) {
			char* temp = strdup(buffer);                // Creates a temp copy of buffer string
			temp[i-1] = '%';                            // Replaces $$ with %d for string insertion
			temp[i] = 'd';
			sprintf(buffer, temp, getpid());            // Overwrites buffer with PID number in the '$$' position
			free(temp);                                 // Frees the space used from calling strdup()
		}
	}

    // Parses buffer arguments into input array
    char* token = strtok(buffer, " ");
    while (token != NULL) {
        input[*numOfArgs] = token;                      // Sets the token to the next index in the input array 
        token = strtok(NULL, " ");	                    // Obtains the next token
        ++*numOfArgs;
    }

    // Checks for a run in the background request and sets the flag
    if (strcmp(input[*numOfArgs-1], "&") == 0) {    
        --*numOfArgs;
        input[*numOfArgs] = "\0";                       // Removes '&' from the input array
        runInBackground = 1;
    }

    return 0;
}

/*
*   parseCommandInput()
*   Iterates through user input, parses meaning and calls
*   helper functions to perform requested actions. Returns
*   0 for "exit" input or 1 to continue the running program.
*/
int parseCommandInput(char** input, int numOfArgs) {

    // Exits the shell
    if (strcmp(input[0], "exit") == 0) {
        printf("exiting, killing jobs...\n");
        fflush(stdout);
        return 0;
    }

    // Changes the current working directory
    else if (strcmp(input[0], "cd") == 0) {
        if (input[1]) {
            chdir(input[1]);
        } else {
            chdir(getenv("HOME"));
        }
        // For testing purposes: https://bit.ly/3vKjzAX
        char* buffer = getcwd(NULL, 0);
        if (buffer) {
            printf("Current working directory: %s\n", buffer);
            free(buffer);
            fflush(stdout);
        }
    }

    else if (strcmp(input[0], "status") == 0) {
        printf("status is true, printing out terminating signal...\n");
        fflush(stdout);
    }

    else {
        printf("executing other commands...\n");
        fflush(stdout);
    }

    return 1;
}

// Can't get this to work@
// void runCD(char** input) {
//     int errorFlag = chdir(input[1]);
//     printf("requested path is: %s\n", (input)[1]);

//     if (errorFlag == -1) {
//         printf("cd: no such file or directory: %s\n", input[1]);
//         fflush(stdout);
//     } else if (!input[1]) {
//         chdir(getenv("HOME"));
//     }

//     // For testing purposes: https://bit.ly/3vKjzAX
//     char* buffer = getcwd(NULL, 0);
//     if (buffer) {
//         printf("Current working directory: %s\n", buffer);
//         free(buffer);
//         fflush(stdout);
//     }
// }

void printStatus() {

}