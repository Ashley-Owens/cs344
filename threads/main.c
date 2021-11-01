// Name: Ashley Owens
// Date: 11/18/2021
// Project 4: Multi-threaded Producer Consumer Pipeline


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_LINES 49
#define INPUT_LENGTH 1000
#define OUTPUT_LENGTH 81                                    // Plus 1 for \n


char*   inputBuffer[MAX_LINES];


void getUserInput() {
    memset(inputBuffer, '\0', INPUT_LENGTH);
    printf("getting user input\n");

}

void getFileInput() {
    printf("getting file input\n");
    char buf[1000];
    fgets(buf, 1000 , stdin);
    printf("%s\n", buf);
}


/*
*   main()
*   Determines whether user has requested input redirect.
*   If so, uses helper function to parse the file input 
*   residing in the stdin buffer. Else, calls helper function
*   to obtain user input from the terminal.
*/
int main(int argc, char *argv[]) {
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
