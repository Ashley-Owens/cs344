// Name: Ashley Owens
// Date: 11/18/2021
// Project 4: Multi-threaded Producer Consumer Pipeline


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_LINES     49
#define INPUT_LENGTH  1000
#define OUTPUT_LENGTH 81                                    // Plus 1 for \n?


char*  inputBuffer[MAX_LINES];
int    lineCount = 0;                                      // Need to reset this at some point?


// For testing purposes
void printBuffer() {

    for (int i=0; i < MAX_LINES; i++) {
        printf("line %d: %s\n", i, inputBuffer[i]);
    }
}

/*
*   getFileInput()
*   Using fgets(), obtains input from stdin and copies it to 
*   the next index in the global array. 
*
*/
void getFileInput(void) {

    char buffer[INPUT_LENGTH];
    
    // Uses fgets to place input into a temporary buffer
    while (fgets(buffer, INPUT_LENGTH, stdin) != NULL) {

        // Stops processing input when STOP received
        if (strcmp(buffer, "STOP\n") == 0) {
            break;

        } else {
            // *** This may need to go elsewhere
            // Replaces newlines with an empty space char
            for (int i=0; i < strlen(buffer); i++) {
                if (strcmp(&buffer[i], "\n") == 0) {
                    buffer[i] = ' ';
                }
            }

            // Copies parsed buffer string into global array
            inputBuffer[lineCount] = strdup(buffer);
            lineCount++;
        }
    }
    printBuffer();
}

/*
*   getUserInput()
*   Using scanf(), puts input from stdin into temporary buffer
*   and copies it to the next index in the global array. 
*/
void getUserInput(void) {
    char buffer[10000];
    scanf("%s", buffer);
    
    // Uses scanf to place input into a temporary buffer
    while (strcmp(buffer, "STOP\n") > 0) {

        // Copies buffer string into global array
        inputBuffer[lineCount] = strdup(buffer);
        lineCount++;
        scanf("%s", buffer);
    }
    printBuffer();
}

/*
*   main()
*   Determines whether user has requested input redirect.
*   If so, uses helper function to parse the file input 
*   residing in the stdin buffer. Else, calls helper function
*   to obtain user input from the terminal.
*/
int main(void) {
    int inputType = fileno(stdin);
    int fd = isatty(inputType);                                          // File input = 0, Terminal input = 1

    // Obtains input via a file or the terminal
    if (fd == 0) {
        getFileInput();
    } else {
        getUserInput();
    }

    return EXIT_SUCCESS;
}
