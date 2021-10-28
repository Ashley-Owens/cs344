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
*  Constant declarations: command line input size
*/
#define MAX_LENGTH 2048
#define MAX_ARGS 512

/* 
*  Global variable declarations for shell processing
*/
int    runInBackground = 0;                                      // Flag for background process
char*  input[MAX_ARGS];                                          // Command prompt input arr
int    numOfArgs;                                                // Length of input arr
int    status = 0;                                               // Tracks exit status
int    maxPIDS = 10;                                             // Limit for number of forked PIDs
int    pidsCount = 0;                                            // Current number of PIDs in array
int*   forkedPIDS;                                               // Pointer to heap memory storing forked PIDs

/* 
*  Function declarations
*/
int   runShell();
int   getCommandInput();
void  changeDirectory();
void  createChildProcess();
void  executeChildProcess();
int   redirect(char *path, int fromFd, int closeFd);
void  storePID(int pid);
int   checkPIDs();
void  handle_SIGTSTP(int signo);

/*
*   Main Function: initializes signal structs, allocates
*   heap memory for child process array, and calls
*   helper function to run the shell.
*/
int main() {
    
    forkedPIDS = malloc(maxPIDS * sizeof(int));              // Initializes heap space for PID array
    struct sigaction SIGSTSTP_action = {0};                     // Initialize empty structs for signal handling
    struct sigaction SIGINT_action = {0};

    // Redirect ^Z to custom handle_SIGTSTP()
    SIGSTSTP_action.sa_handler = handle_SIGTSTP;                // Register handle_SIGTSTP as the signal handler
    sigfillset(&SIGSTSTP_action.sa_mask);                       // Block all catchable signals while handle_SIGINT is running
    SIGSTSTP_action.sa_flags = SA_RESTART;                      // Avoids error by restarting the interrupted system call 
    sigaction(SIGTSTP, &SIGSTSTP_action, NULL);                 // Install signal handler

    // Ignore sigint signals for child processes
    SIGINT_action.sa_handler = SIG_IGN;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;
    sigaction(SIGINT, &SIGINT_action, NULL);

    runShell();
    free(forkedPIDS);                                           // Frees heap memory
    return 0;
}

/*
*   runShell()
*   Runs the shell loop, calls helper functions to obtain
*   command input, parse commands, and perform requested actions.
*   Returns 0 for exit, otherwise continues running the program.
*/
int runShell() {
    
    while (true) {
        numOfArgs = 0;
        memset(input, '\0', MAX_LENGTH);
        getCommandInput();

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
            createChildProcess();
            fflush(stdout);
        }
        // Checks PID array for completed background processes
        while (checkPIDs());
    }
}

/*
*   getCommandInput()
*   Performs command line prompt, ignores newlines and code comments.
*   Appends stdin to global input array, converts $$ into PID, sets the
*   global background flag, and caculates number of input arguments.
* 
*   numOfArgs: length of the command input array
*/
int getCommandInput() {
    char buffer[MAX_LENGTH];

    // Command line prompt
    do {
        printf(": ");
        fgets(buffer, MAX_LENGTH +1, stdin);             // Stores input into buffer string
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
        input[numOfArgs] = strdup(token);                      // Sets the token to the next index in the input array 
        token = strtok(NULL, " ");	                    // Obtains the next token
        numOfArgs++;
    }

    // Checks for a run in the background request and sets the flag
    if (strcmp(input[numOfArgs-1], "&") == 0) {    
        numOfArgs--;
        input[numOfArgs] = "\0";                       // Replaces '&' with null pointer for later child processes
        runInBackground = 1;
    }

    return 0;
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
void createChildProcess() {
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
        executeChildProcess();
    }           

     // Parent executes this code 
    else {
        // spawnpid is the pid of the child
        if (runInBackground) {
            printf("I am the parent. My pid  = %d\n", getpid());
            waitpid(spawnpid, &waitStatus, WNOHANG);
            printf("Background pid is %d\n", spawnpid);
            fflush(stdout);
            storePID(spawnpid);
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
void executeChildProcess() {
    int     i = 0;
    int     fdsArr[2] = {-1};

    // Iterate through the input to get commands and filenames
    while (i < numOfArgs) {

        // Redirects input from file on the right
        if (strcmp(input[i], "<") == 0) {
            // We need execvp to ignore this symbol
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
            // We need execvp to ignore this symbol
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

    free(forkedPIDS);
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
*
*   Returns: file descriptor identifier, else EXIT_FAILURE
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

/*
*   storePID()
*   Function to append a newly created PID to the pid array. If 
*   If there are more processes than the array can hold, reallocates
*   heap memory to accommodate additional processes. 
*
*   pid: process ID number of the forked process
*/
void storePID(int pid) {
	// Allocate additional memory when the array reaches max allotment
	if (pidsCount == maxPIDS) {
		maxPIDS *= 2;
		forkedPIDS = realloc(forkedPIDS, maxPIDS * sizeof(int));
	}
    pidsCount++;
	forkedPIDS[pidsCount] = pid;
}

/*
*   checkPID()
*   Iterates through the pid array, checking for completed processes. 
*   Prints messages according to completed process termination type. 
*   Updates the pid array, removing the completed processes and moving
*   array elements up to eliminate empty indices in the array. 
*
*   Returns: 0 for completed process, else 1
*/
int checkPIDs() {
    int waitStatus;

    for (int i=0; i < pidsCount; i++) {
        if (waitpid(forkedPIDS[i], &waitStatus, WNOHANG) != 0) {
            
            // print the appropriate message based on termination type
			if (WIFEXITED(waitStatus)) {
				printf("background pid %d is done: exit value %d\n", forkedPIDS[i], WEXITSTATUS(waitStatus));
				fflush(stdout);
			}
			else if (WIFSIGNALED(waitStatus)) {
				printf("background pid %d is done: terminated by signal %d\n", forkedPIDS[i], WTERMSIG(waitStatus));
				fflush(stdout);
			}
            
            // Updates the pid array by moving processes one index to the left
			for (int j = i + 1; j < pidsCount; j++) {
				forkedPIDS[j - 1] = forkedPIDS[j];
			}

			pidsCount--;
			return 1;
        }
    }
    return 0;
}

/*
*   handle_SIGTSTP()
*   Toggles foreground process: when SIGTSTP is called, global 
*   varible is updated and message is written reentrantly. 
*   Code modified from signal handling expoloration: https://bit.ly/3BqLiIh
*
*   signo: specifies type of signal
*/

void handle_SIGTSTP(int signo) {

    // If true, set to false and display reentrant message
	if (runInBackground == 1) {
		char* message = "Entering foreground-only mode (& is now ignored)\n";
		write(1, message, 49);
		fflush(stdout);
		runInBackground = 0;
	}

	// If false, set to true and display reentrant message
	else {
		char* message = "Exiting foreground-only mode\n";
		write (1, message, 29);
		fflush(stdout);
		runInBackground = 1;
	}
}
