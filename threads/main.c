// Name: Ashley Owens
// Date: 11/18/2021
// Project 4: Multi-threaded Producer Consumer Pipeline


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_LINES     49
#define INPUT_LENGTH  1000
#define OUTPUT_LENGTH 81                                     // Plus 1 for \n?


char*  inputBuffer[MAX_LINES];                              // Input buffer from file or user
int    lineCount = 0;                                       // Need to reset this at some point?

char*  swapCharsBuffer[MAX_LINES];                          // Buffer to replace \n and ++


// For testing purposes
void printBuffer() {

    for (int i=0; i < MAX_LINES; i++) {
        printf("line %d: %s\n", i, swapCharsBuffer[i]);
    }
}

void replaceLineSeparators(void) {
    char buffer[INPUT_LENGTH];
    int i = 0;

    while (inputBuffer[i] != NULL && i < MAX_LINES) {
        strcpy(buffer, inputBuffer[i]);

        // Replaces newlines with an empty space char
        for (int i=0; i < strlen(buffer); i++) {
            if (strcmp(&buffer[i], "\n") == 0) {
                buffer[i] = ' ';
            }
        }
        swapCharsBuffer[i] = strdup(buffer);
        // printf("line %d: %s\n", i, swapCharsBuffer[i]);
        i++;
    }
}

/*
*   getInput()
*   Using fgets(), obtains input from stdin and copies it to 
*   the next index in the global array. 
*
*/
void getInput(void) {

    char buffer[INPUT_LENGTH];
    
    // Uses fgets to place input into a temporary buffer
    while (fgets(buffer, INPUT_LENGTH, stdin) != NULL) {

        // Stops processing input when STOP received
        if (strcmp(buffer, "STOP\n") == 0) {
            break;

        } else {
            // Copies parsed buffer string into global array
            inputBuffer[lineCount] = strdup(buffer);
            lineCount++;
        }
    }
}

/*
*   main()
*   Determines whether user has requested input redirect.
*   If so, uses helper function to parse the file input 
*   residing in the stdin buffer. Else, calls helper function
*   to obtain user input from the terminal.
*/
int main(void) {
    
    getInput();
    replaceLineSeparators();
    printBuffer();
    return EXIT_SUCCESS;
    
}
