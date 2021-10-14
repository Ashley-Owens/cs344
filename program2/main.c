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

/* Creates movie struct */
struct movie {
    char *title;
    int year;
    char *languages;
    float rating;
    struct movie *next;
};

/* Parse the current line which is comma delimited and
* create a movie struct with the data. Code adapted from:
* https://replit.com/@cs344/studentsc#main.c
*/
struct movie *createMovie(char *currLine) {
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the movie title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the year, stored as an integer
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // The next token is the string of languages
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // The last token is the rating, stored as a float
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rating = atof(token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Returns a linked list of movies by parsing data from
* each line of the specified file. Code adapted from:
* https://replit.com/@cs344/studentsc#main.c
*/
struct movie *processFile(char *filePath) {
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");
    if (movieFile == NULL) {
        printf("Invalid filename entry, please try again.\n");
        printf("Example usage: ./movies movie_list.csv\n");
        return 0;
    }

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    int count = 0;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;
    
    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        // Ignores first line with Category Descriptors
        if(count == 0) {
            count++;
            continue;
        }
        // Get a new movie node corresponding to the current line
        struct movie *newNode = createMovie(currLine);

        // This is the first node in the linked link
        // Sets the head and the tail to this node
        if (head == NULL) {
            head = newNode;
            tail = newNode;
        }
        // This is not the first node.
        // Add this node to the list and advance the tail
        else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }
    free(currLine);
    fclose(movieFile);
    printf("Processed file %s and parsed data for %d movies \n", filePath, count -1);
    return head;
}

char* makeDir() {
    // Generates a random num between 0 and 99999
    // Code modified from: https://bit.ly/3v7StTY
    char random[6];
    char *name = "owensas.movies.";
    char *pathname = malloc(strlen(name) + strlen(random) + 1);
    int r = rand() % 100000;
    mode_t mode = 0750;

    // Converts num to string and concatenates to file pathname
    sprintf(random, "%d", r);
    strcpy(pathname, name);
    strcat(pathname, random);
    
    // Returns the new directory name
    mkdir(pathname, mode);
    return pathname;
}

/*
* Returns the name of the file in the current directory that is either
* the largest or the smallest in the directory. If requesting largest,
* fileRequest = 1, if requesting smallest, fileRequest = 0. Code modified
* from module 3 Exploration on Directories: https://bit.ly/3mNYmC7
*/
char* locateMinMaxFiles(int fileRequest) {
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
    if (fileRequest) {
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
* Print data for the given movie. Code adapted from:
* https://replit.com/@cs344/studentsc#main.c
*/
void printMovie(struct movie* aMovie){
  printf("%s, %i, %s, %.1f\n", aMovie->title,
               aMovie->year,
               aMovie->languages,
               aMovie->rating);
}

/*
* Print the linked list of movies. Code adapted from:
* https://replit.com/@cs344/studentsc#main.c
*/
void printMovieList(struct movie *list) {
    while (list != NULL) {
        printMovie(list);
        list = list->next;
    }
}

/*
*   Interactive element of the program. Gets user input and returns
*   requested information from movie data by ......  fill me in ........
*/
void subMenu() {
    int userNum;
    int min = 1;
    int max = 3;
    bool flag = true;
    char tempFile[100];
    char* filepointer;
    char* dirName;
    struct movie *list;

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
                    filepointer = locateMinMaxFiles(1);
                    printf("Now processing the largest file named: %s\n", filepointer);
                    list = processFile(filepointer);
                    free(filepointer);
                    dirName = makeDir();
                    printf("DirectoryName is: %s\n", dirName);
                    flag = false;
                    break;
                case 2:
                    filepointer = locateMinMaxFiles(0);
                    printf("Now processing the smallest file named %s\n", filepointer);
                    list = processFile(filepointer);
                    free(filepointer);
                    dirName = makeDir();
                    printf("DirectoryName is: %s\n", dirName);
                    flag = false;
                    break;
                case 3:
                    printf("Enter the complete file name: ");
                    scanf("%s", tempFile);
                    filepointer = calloc(strlen(tempFile) + 1, sizeof(char));
                    strcpy(filepointer, tempFile);
                    int ret = locateInputFile(filepointer);
                    if (ret == 1) {
                        printf("Now processing: %s\n", filepointer);
                        list = processFile(filepointer);
                        free(filepointer);
                        dirName = makeDir();
                        printf("DirectoryName is: %s\n", dirName);
                        flag = false;
                    } else {
                        printf("Error, %s not found. Please try again.\n", filepointer);
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
