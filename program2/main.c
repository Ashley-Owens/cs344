// Name: Ashley Owens
// Date: 10/18/2021
// Assignment 2: Files and Directories

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h> 


/*
*   Interactive element of the program. Gets user input and returns
*   requested information from movie data by ......  fill me in ........
*/
void subMenu() {
    int userNum;
    int min = 1;
    int max = 3;
    char filename[100];
    bool flag = true;

    // Main loop for user interaction
    while(flag) {
        printf("\n***** Which file you want to process? *****\n");
        printf("Enter 1 to pick the largest file\n");
        printf("Enter 2 to pick the smallest file\n");
        printf("Enter 3 to specify the name of a file\n");
        printf("Enter a choice from 1 to 3: ");
        scanf("%d", &userNum);
        
        // Algorith from: https://bit.ly/3oEQHIK
        if ((userNum - min) * (userNum - max) > 0) {
            printf("Invalid entry, please try again.\n");
            continue;
        } else {
            // Cases for handling user input
            switch(userNum) {
                case 1:
                    printf("Now processing the largest file named ...\n");
                    flag = false;
                    break;
                case 2:
                    printf("Now processing the smallest file named ...\n");
                    flag = false;
                    break;
                case 3:
                    printf("Enter the complete file name: ");
                    scanf("%s", filename);
                    printf("You have entered %s", filename);
                    flag = false;
                    break;
            }
        } 
    }
}


/*
*   Interactive element of the program. Gets user input and returns
*   requested information from movie data by ......  fill me in ........
*/
void mainMenu() {
    // Initializes variables
    int userNum;
    int min = 1;
    int max = 2;
    char lang[25];
    bool flag = true;

    // Main loop for user interaction
    while(flag) {
        printf("\n***** Files and Directories Main Menu *****\n");
        printf("1. Select a file to process\n");
        printf("2. Exit the program\n");
        printf("Enter a choice from 1 or 2: ");
        scanf("%d", &userNum);
        
        // Algorith from: https://bit.ly/3oEQHIK
        if ((userNum - min) * (userNum - max) > 0) {
            printf("Invalid entry, please try again.\n");
            continue;
        } else {
            // Cases for handling user input
            switch(userNum) {
                case 1:
                    subMenu();
                    break;
                case 2:
                    printf("Program exiting...\n");
                    flag = false;
                    break;
            }
        } 
    }
}

/*
*   Initiatates user interaction loop
*/
int main() {
    mainMenu();
    return EXIT_SUCCESS;
}
