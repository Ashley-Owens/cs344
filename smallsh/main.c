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
*  Constant declarations: shell input size
*/
#define MAX_LENGTH 2048
#define MAX_ARGS 512

/* 
*  Global variable declarations
*/
int runInBackground;
char* input[MAX_ARGS];


/* 
*  Function declarations
*/
int getCommandInput(int* numOfArgs);
int parseCommandInput(int numOfArgs);
void changeDirectory();



/*
*   Main Function: Runs program loop for retrieving
*   shell input from user, parsing the input, and 
*   performing the requested actions.
*/
int main() {
    int numOfArgs;
    bool runShellProgram = true;
    
    while (runShellProgram) {
        fflush(stdout);
        fflush(stdin);
        numOfArgs = 0;                                   // Reinitializes input array and arg count variables
        memset(input, '\0', MAX_LENGTH);
        getCommandInput(&numOfArgs);
        runShellProgram = parseCommandInput(numOfArgs);
    }
    return EXIT_SUCCESS;
}

/*
*   getCommandInput()
*   Performs command line prompt, ignores newlines and code comments.
*   Appends stdin to global input array, converts $$ into PID, sets the
*   global background flag, and caculates number of input arguments.
*/
int getCommandInput(int* numOfArgs) {
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
*   Iterates through user input, parses commands and calls
*   helper functions to perform requested actions. Returns
*   0 to "exit" or 1 to continue running the program.
*/
int parseCommandInput(int numOfArgs) {

    // Exits the shell
    if (strcmp(input[0], "exit") == 0) {
        printf("exiting, killing jobs...\n");
        fflush(stdout);
        return 0;
    }

    // Changes the current working directory
    else if (strcmp(input[0], "cd") == 0) {
        changeDirectory();
    }

    // Figure out status...
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

/*
*   changeDirectory()
*   Changes directories: accesses global input array to determine 
*   if user has entered a directory to perform requested action. 
*   No directory input automatically navigates to HOME directory.
*/
void changeDirectory() {
    int err;

    if (input[1]) {
        err = chdir(input[1]);
    } else {
        err = chdir(getenv("HOME"));
    }

    if (err == -1) {
        printf("cd: no such file or directory\n");
        fflush(stdout);
    }

    // For testing purposes: https://bit.ly/3vKjzAX
    char* buffer = getcwd(NULL, 0);
    if (buffer) {
        printf("Current working directory: %s\n", buffer);
        free(buffer);
        fflush(stdout);
    }
}

void printStatus() {

}