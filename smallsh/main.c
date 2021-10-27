// Name: Ashley Owens
// Date: 11/01/2021
// Program 3: Smallsh

// #include <errno.h>
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
*  Struct declarations: signal handlers
*/
// struct sigaction


/* 
*  Global variable declarations: I tried to pass these around to my
*  helper functions but couldn't figure out how to stop pointer decay
*/
int runInBackground;                                            // Flag for background process
char* input[MAX_ARGS];                                          // Command prompt input


/* 
*  Function declarations
*/
int getCommandInput(int* numOfArgs);
int parseCommandInput(int numOfArgs);
void changeDirectory();
void createChildProcess(int numOfArgs);
void executeChildProcess(int numOfArgs);



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
        numOfArgs = 0;                                          // Reinitializes input array and arg count variables
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

    // Parses buffer arguments into input char array
    char* token = strtok(buffer, " ");
    while (token != NULL) {
        input[*numOfArgs] = token;                      // Sets the token to the next index in the input array 
        token = strtok(NULL, " ");	                    // Obtains the next token
        ++*numOfArgs;
    }

    // Checks for a run in the background request and sets the flag
    if (strcmp(input[*numOfArgs-1], "&") == 0) {    
        --*numOfArgs;
        input[*numOfArgs] = "\0";                       // Replaces '&' with null pointer for later child processes
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

    // Exits the shell: need to figure out how to kill jobs
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
        createChildProcess(numOfArgs);
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

void createChildProcess(int numOfArgs) {
    
    
    // Code modified from Exploration: Creating & terminating processes
    // https://bit.ly/2XOmwDV
    pid_t spawnpid = -5;                                            // Sets spawnpid to placeholder value
    int   childExitStatus;
    int   childPID;

    spawnpid = fork();                                              // Creates a child fork from the smallsh process
    switch (spawnpid){
        // Parent executes this code when child fork fails
        case -1:
            perror("fork() failed!");
            exit(1);
            break;

        // Fork succeeded: child executes this code
        case 0:
            // printf("I am the child. My pid  = %d\n", getpid());
            executeChildProcess(numOfArgs);

        // Parent executes this code
        default:
            // printf("PARENT: making child a zombie for 10 seconds");
            // fflush(stdout);
            // sleep(10);
            // spawnpid = waitpid(spawnpid, &childExitStatus, 0);
            // spawnpid is the pid of the child
            // printf("I am the parent. My pid  = %d\n", getpid());
            childPID = wait(&childExitStatus);
            printf("Parent's waiting is done as the child with pid %d exited\n", childPID);
            break;
        }

    // both parent and child execute code here
}

void executeChildProcess(int numOfArgs) {
    int     i = 0;
    int     inputFD;
    int     outputFD;
    int     result;

    // Iterate through the input
    while (input[i] != NULL) {

        if (strcmp(input[i], "<") == 0) {
            printf("filename is: %s\n", input[i+1]);
            inputFD = open(input[i+1], O_RDONLY);
            printf("inputFD is %i\n", inputFD);
            fflush(stdout);

            // Catches error if unable to open given filename
            if (inputFD < 0) {
                perror("Error");
                exit(1);
            }
            
            // Use dup2 to point inputFD
            // result = dup2(inputFD, 0);
            // if (result < 0) {
            //     perror("dup2"); 
            //     exit(1); 
            // }
            printf("we made it here\n");
            close(inputFD);
            
            fflush(stdout);
        }
            // printf("inputFileName is: %s\n", inputFileName);
            // fflush(stdout);
        
        else if (strcmp(input[i], ">") == 0) {
            outputFD = open(input[i+1], O_RDONLY);
            close(outputFD);
        }

        i++;
    }


    execvp(input[0], input);                                 // execvp returns only on error
    perror("execvp");
}