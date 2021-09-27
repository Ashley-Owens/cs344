#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// int main(char *argv[]) {
int main(int argc, char **argv) {
    // argc is the number of inputs
    // char **argv is the string values 

    // Pointer to the inputted filename
    char *fileName = argv[1];

    // Opens the file in read mode
    FILE *filePtr = fopen(fileName, "r");

    // Exits the program if the file couldn't be opened
    if (!filePtr){
            printf("Error: could not open the file %s\n", fileName);
            exit(1);
        }



    // fclose(fileName);
    
    
    // for (int i=0; i < argc; i++) {
    //     printf("arg %d : %s\n", i, argv[i]);
    // }


    // // Open the file "grades.txt"
    // // O_WRONLY means open the file only for writing
    // // O_CREAT means create the file if it doesn't exit
    // // O_TRUNC means truncate the file if it already exists
    // // 00600 is the mode which sets file permissions that we will study later
    
    // printf("Give me a number please: ");
    // scanf("%d", &y);
    // printf("You chose the value: %d\n", y);
    // printf("%s\n", file);
    // scanf("%s", file);
    // printf("Your name is: %s\n", name);

    return 0;
}