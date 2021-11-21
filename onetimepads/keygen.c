// Name: Ashley Owens
// Date: 11/29/2021
// Project 5: One-time Pads
// Key Generator: creates a key file of specified length. The file characters 
// randomly generated include any of the 26 alphabet letters plus the space 
// character. The last character ouput is a newline.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LENGTH 27


/*
*   generateKey()
*   param - key_length: desired key length
*   
*   Generates a string of random characters to serve as an 
*   encryption key. Prints the key to stdout.
*   Returns: None
*/
void generateKey(int key_length) {
    srand(time(NULL));
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    char key[key_length + 1];
    int  i, index;

    // Generates a string of random characters
    for (i=0; i < key_length; i++) {
        index = rand() % LENGTH;
        key[i] = characters[index];
    }
    // Null terminates string and prints to stdout
    key[i] = '\0';
    printf("%s\n", key);
}

/*
*   main()
*   param - argc: count of CL input arguments
*   param - argv: array of CL string arguments
*   
*   Calls helper method to generate a random key
*   of specified length and prints it to stdout.
*   Return: stderr/failure if missing input, else success.
*/
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,"Please enter a numerical value. For example: keygen 10\n"); 
        return EXIT_FAILURE;
    }

    if (atoi(argv[1]) < 0) {
		fprintf(stderr, "Please enter a valid key length\n");
		return EXIT_FAILURE;
	}

    generateKey(atoi(argv[1]));
    return EXIT_SUCCESS;
}
