#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
 
// argc is the number of inputs, char **argv are the string values 
int main(int argc, char **argv) {

    // Pointer to the inputted filename
    char *fileName = argv[1];
    char newline[4096];
    int count;

    // Opens the file in read mode
    FILE *filePtr = fopen(fileName, "r");

    // Exits the program if the file couldn't be opened
    if (!filePtr){
            printf("Error: could not open the file %s\n", fileName);
            exit(1);
    }

    // File was valid, read it in
    while (fgets(newline, sizeof(newline), filePtr)) {
        if (count >= 1) {

            // Returns a pointer to the new, duplicated string
            char *temp = strdup(newline);
            printf("Printing the newline: %s\n", temp);
        }
        count++;
    }
	


    // fclose(filePtr);
    
    
    // for (int i=0; i < argc; i++) {
    //     printf("arg %d : %s\n", i, argv[i]);
    // }

    
    // printf("Give me a number please: ");
    // scanf("%d", &y);
    // printf("You chose the value: %d\n", y);
    // printf("%s\n", file);
    // scanf("%s", file);
    // printf("Your name is: %s\n", name);

    return 0;
}