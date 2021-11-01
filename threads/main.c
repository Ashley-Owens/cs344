// Name: Ashley Owens
// Date: 11/18/2021
// Project 4: Multi-threaded Producer Consumer Pipeline


#include <stdio.h>
#include <stdlib.h>


#define MAX_LINES 49
#define INPUT_LENGTH 1000
#define OUTPUT_LENGTH 81                            // Plus 1 for \n





int main(int argc, char **argv) {
    // argc is the number of inputs, char **argv are the string values 
    if (argc < 2) {
        printf("You must provide input to process\n");
        printf("Example usage: ./line_processor Here's some input to process\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



