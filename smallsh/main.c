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
*  Global variable declarations: I tried to pass these around to my
*  helper functions but couldn't figure out how to stop pointer decay
*/
int runInBackground = 0;                                        // Flag for background process
char* input[MAX_ARGS];                                          // Command prompt input
int status = 0;
int maxChildPs = 10;
int childPsCount = 0;
int* childPsPtr;



/* 
*  Function declarations
*/
int getCommandInput(int* numOfArgs);
int parseCommandInput(int numOfArgs);
void changeDirectory();
void createChildProcess(int numOfArgs);
void executeChildProcess();
int redirect(char *path, int fromFd, int closeFd);
// void storePID(int pid);



/*
*   Main Function: Runs program loop for retrieving
*   shell input from user, parsing the input, and 
*   performing the requested actions.
*/
int main() {
    int numOfArgs;
    bool runShellProgram = true;
    // childPsPtr = malloc(maxChildPs * sizeof(int));
    
    while (runShellProgram) {
        fflush(stdout);
        fflush(stdin);
        numOfArgs = 0;                                          // Empties input array and arg count variables
        memset(input, '\0', MAX_LENGTH);
        getCommandInput(&numOfArgs);
        runShellProgram = parseCommandInput(numOfArgs);
        // free(childPsPtr);
    }
    return EXIT_SUCCESS;
}

/*
*   getCommandInput()
*   Performs command line prompt, ignores newlines and code comments.
*   Appends stdin to global input array, converts $$ into PID, sets the
*   global background flag, and caculates number of input arguments.
* 
*   numOfArgs: length of the command input array
*/
int getCommandInput(int* numOfArgs) {
    char buffer[MAX_LENGTH];

    // Command line prompt
    do {
        printf(": ");
        
        fgets(buffer, MAX_LENGTH +1, stdin);               // Stores input into buffer string
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
* 
*   numOfArgs: length of the command input array
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

    // Creates a fork and executes commands
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

/*
*   createChildProcess()
*   Creates a child process to execute user input commands. 
*   Uses global variable to determine if child process should
*   run in the foreground or background. Tracks child processes
*   in a global array.
* 
*   numOfArgs: length of the command input array
*/
void createChildProcess(int numOfArgs) {
    // Code modified from Exploration: Creating & terminating processes
    // https://bit.ly/2XOmwDV
    pid_t spawnpid = fork();                                    
    int   waitStatus;
    
    // Parent executes this code when child fork fails
    if (spawnpid < 0) {
        perror("fork() failed!");
        status = 1;
        exit(1);
    }

    // Fork succeeded: child executes this code
    else if (spawnpid == 0) {
        // printf("I am the child. My pid  = %d\n", getpid());
        executeChildProcess(numOfArgs);
    }           

     // Parent executes this code 
    else {
        // spawnpid is the pid of the child
        if (runInBackground) {
            printf("I am the parent. My pid  = %d\n", getpid());
            waitpid(spawnpid, &waitStatus, WNOHANG);
            printf("Background pid is %d\n", spawnpid);
            fflush(stdout);
            // storePID(spawnpid);
        } else {
            // blocks the shell process for foreground command 
            waitpid(spawnpid, &waitStatus, 0);
        }
    }
}

/*
*   executeChildProcess()
*   Iterates theough the user's input commands, uses a helper
*   function to handle file redirects, executes the parsed
*   command using execvp().
* 
*   numOfArgs: length of the command input array
*/
void executeChildProcess(int numOfArgs) {
    int     i = 0;
    int     fdsArr[2] = {-1};

    // Iterate through the input to get commands and filenames
    while (i < numOfArgs) {

        // Redirects input from file on the right
        if (strcmp(input[i], "<") == 0) {
            input[i] = NULL;

            // Calls helper function to redirect input
            if (input[i+1]) {
                fdsArr[0] = redirect(input[i+1], STDIN_FILENO, -1);
    
            } else {
                if (runInBackground) {
                    fdsArr[0] = redirect("/dev/null", STDIN_FILENO, -1);
                }
            }
            
        // Redirects output to file on the right
        } else if (strcmp(input[i], ">") == 0) {
            input[i] = NULL;

            // Calls helper function to redirect output
            if (input[i+1]) {
                fdsArr[1] = redirect(input[i+1], STDOUT_FILENO, fdsArr[0]);
    
            } else {
                if (runInBackground) {
                    fdsArr[1] = redirect("/dev/null", STDOUT_FILENO, fdsArr[0]);
                }
            }
        }

        i++;
    }

    // Executes command using input arguments
    execvp(input[0], input);                                            // execvp returns only on error
    fprintf(stderr, "%s: no such file or directory\n", input[0]);
	fflush(stderr);

    // If any fds are still open, closes them
    for (int i=0; i < 2; i++) {
        if (fdsArr[i] > -1) {
            close(fdsArr[i]);
        }
    }

    // free(childPsPtr);
    exit(EXIT_FAILURE);
}

/*
*   redirect()
*   Function to handle file redirects to the given filename. If 
*   the filename can't be opened, process is terminated.
*
*   path: filename to open for redirection
*   fromFD: file descriptor number (either 0 or 1)
*   closeFD: if redirection fails, the file descriptor to close
*/
int redirect(char *path, int fromFD, int closeFD) {
	int toFD;
	char redirStr[7] = "";

	// opens a file descriptor
	if (fromFD == STDIN_FILENO) {
		toFD = open(path, O_RDONLY);
		strcpy(redirStr, "input");
	}
	else {
		if (strcmp("/dev/null", path) == 0) {
			toFD = open(path, O_WRONLY);
		}
		else {
			toFD = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		}
		strcpy(redirStr, "output");
	}

	// terminate the process on failure
	if (toFD == -1) {
		fprintf(stderr, "cannot open %s for %s\n", path, redirStr);
		fflush(stderr);

		// close the specified open file if necessary to prevent errors
		if (closeFD >= 0) {
			close(closeFD);
		}
		exit(EXIT_FAILURE);
	}

    // Makes file descriptors equivalent
	dup2(toFD, fromFD);
	return toFD;
}


// void storePID(int pid) {
// 	// dynamically allocate additional memory when the array fills up
// 	if (childPsCount == maxChildPs) {
// 		maxChildPs *= 2;
// 		childPsPtr = realloc(childPsPtr, maxChildPs * sizeof(int));
// 	}

// 	childPsPtr[childPsCount++] = pid;
// }