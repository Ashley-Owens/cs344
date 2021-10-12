// Name: Ashley Owens
// Date: 10/18/2021
// Assignment 2: Files and Directories

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define PREFIX "movies_"

/*
* Print the name of the file in the current directory that is either
* the largest or the smallest in the directory. If requesting largest,
* fileSize = 1, if requesting smallest, fileSize = 0. Code modified
* from module 3 Exploration on Directories: https://bit.ly/3mNYmC7
*/
int locateFile(int fileSize) {
    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    struct stat dirStat;
    off_t maxSize = 0;
    off_t minSize = 1000000;
    int i = 0;
    char maxEntryName[256];
    char minEntryName[256];


    // Go through all the files in the directory
    while((aDir = readdir(currDir)) != NULL){

        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
            // Get meta-data for the current entry
            stat(aDir->d_name, &dirStat);  
            
            // Uses the st_size function to compare the file size to the current max/min sizes
            // and the S_ISREG function to check that it is a normal file 
            if (dirStat.st_size > maxSize && S_ISREG(dirStat.st_mode)) {
                maxSize = dirStat.st_size;
                memset(maxEntryName, '\0', sizeof(maxEntryName));
                strcpy(maxEntryName, aDir->d_name);
            }
            else if (dirStat.st_size < minSize && S_ISREG(dirStat.st_mode)) {
                minSize = dirStat.st_size;
                memset(minEntryName, '\0', sizeof(minEntryName));
                strcpy(minEntryName, aDir->d_name);
            }
            i++;
        }
    }
    // Close the directory and prints file info
    closedir(currDir);
    if (fileSize) {
        printf("Now processing the largest file named: %s\n", maxEntryName);
    } else {
        printf("Now processing the smallest file named %s\n", minEntryName);
    }
    return 0;
}


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

    // Sub menu loop for user interaction
    while(flag) {
        printf("\n***** Which file do you want to process? *****\n");
        printf("Enter 1 to pick the largest file.\n");
        printf("Enter 2 to pick the smallest file.\n");
        printf("Enter 3 to specify the name of a file.\n");
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
                    locateFile(1);
                    flag = false;
                    break;
                case 2:
                    printf("Now processing the smallest file named ...\n");
                    locateFile(0);
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
