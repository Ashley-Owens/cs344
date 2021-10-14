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
* Returns the name of the file in the current directory that is either
* the largest or the smallest in the directory. If requesting largest,
* fileSize = 1, if requesting smallest, fileSize = 0. Code modified
* from module 3 Exploration on Directories: https://bit.ly/3mNYmC7
*/
char* locateMinMaxFiles(int fileSize) {
    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    struct stat dirStat;
    off_t maxSize = 0;
    off_t minSize = 1000000;
    int i = 0;
    char *maxEntryName;
    char *minEntryName;
    const char* fileExt = ".csv";

    // Go through all the files in the directory
    while((aDir = readdir(currDir)) != NULL){

        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
            // Get meta-data for the current entry
            stat(aDir->d_name, &dirStat);
            
            // Confirm current file is a .csv
            char *isValid = strstr(aDir->d_name, fileExt);
            
            // File is a .csv: perform min/max comparisons and update variables
            if (isValid != NULL) {

                // Uses the st_size function to compare the file size to the current max/min sizes
                // and the S_ISREG function to check that it is a normal file 
                if (dirStat.st_size > maxSize) {
                    maxSize = dirStat.st_size;
                    maxEntryName = calloc(strlen(aDir->d_name) + 1, sizeof(char));
                    strcpy(maxEntryName, aDir->d_name);
                }
                if (dirStat.st_size < minSize) {
                    minSize = dirStat.st_size;
                    minEntryName = calloc(strlen(aDir->d_name) + 1, sizeof(char));
                    strcpy(minEntryName, aDir->d_name);
                }
            }
        }
        i++;
    }
    // Close the directory and return file name
    closedir(currDir);
    if (fileSize) {
        return maxEntryName;
    } else {
        return minEntryName;
    }
}

/*
* Opens the current directory and searches for the inputted filename.
* Returns 0 if file not found, returns 1 if file found. Code modified
* from module 3 Exploration on Directories: https://bit.ly/3mNYmC7
*/
int locateInputFile(char* filename) {

    // Open the current directory
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    bool flag = false;

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL){

        // If inputted file is found
        if (!strcmp(aDir->d_name, filename)) {
            flag = true;
        }
    }
    // Close the directory
    closedir(currDir);

    // If file not found, return 0
    if (!flag) {
        return 0;
    }
    return 1;
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
    char* maxFile;
    char* minFile;

    // Sub menu loop for user interaction
    while(flag) {
        printf("\n***** Which file do you want to process? *****\n");
        printf("Enter 1 to pick the largest file.\n");
        printf("Enter 2 to pick the smallest file.\n");
        printf("Enter 3 to specify the name of a file.\n");
        printf("\nEnter a choice from 1 to 3: ");
        scanf("%d", &userNum);
        
        // Algorith from: https://bit.ly/3oEQHIK
        if ((userNum - min) * (userNum - max) > 0) {
            printf("Invalid entry, please try again.\n");
            continue;
        } else {
            // Cases for handling user input
            switch(userNum) {
                case 1:
                    maxFile = locateMinMaxFiles(1);
                    printf("Now processing the largest file named: %s\n", maxFile);
                    // processFile(filename);
                    // free(maxFile)
                    flag = false;
                    break;
                case 2:
                    minFile = locateMinMaxFiles(0);
                    printf("Now processing the smallest file named %s\n", minFile);
                    // processFile(filename);
                    // free(maxFile)
                    flag = false;
                    break;
                case 3:
                    printf("Enter the complete file name: ");
                    scanf("%s", filename);
                    int ret = locateInputFile(filename);
                    if (ret == 1) {
                        printf("Now processing: %s\n", filename);
                        // processFile(filename);
                        flag = false;
                    } else {
                        printf("Error, %s not found. Please try again.\n", filename);
                    }
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
