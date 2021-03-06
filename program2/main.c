// Name: Ashley Owens
// Date: 10/15/2021
// Program 2: Files and Directories

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#define PREFIX "movies_"

/* Creates movie struct */
struct movie {
    char *title;
    char *year;
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
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

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
    return head;
}

/*
* Returns the name of the newly created directory that follows the
* naming convention of owensas.movies.<random number>. Code modified
* from module 3 Exploration on Directories: https://bit.ly/3mNYmC7
*/
char* makeDir() {
    // Generates a random num between 0 and 99999
    // Code modified from: https://bit.ly/3v7StTY
    srand(time(0));
    int r = rand() % 100000;
    char random[6];
    char *name = "owensas.movies.";
    char *pathname = malloc(strlen(name) + strlen(random) + 1);
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
* Iterates through the movie struct list, creating files in the chosen
* directory named by the year the movie was made and containing the movie
* titles matching that year. Code modified from Exploration: Files
* in module 3: https://bit.ly/3DHOz7q
*/
int makeFiles(char *dirName, struct movie *list) {

    char *separator = "/";
    char *fileExt = ".txt";
    char *newLine = "\n";
    char *yearPathName;
    char *movieTitle;
    mode_t mode = 0640;
    int fileDescriptor;

    // Iterates through the linked list movie struct
    while (list != NULL) {

        // Concatenates file directory + new filename
        yearPathName = malloc(strlen(dirName) + strlen(separator) + strlen(list->year) + strlen(fileExt) + 1);
        strcpy(yearPathName, dirName);
        strcat(yearPathName, separator);
        strcat(yearPathName, list->year);
        strcat(yearPathName, fileExt);

        // Adds a newline to the movie title
        movieTitle = malloc(strlen(list->title) + strlen(newLine) + 1);
        strcpy(movieTitle, list->title);
        strcat(movieTitle, newLine);
        
        // Creates a file if it doesn't exist or opens existing file
        fileDescriptor = open(yearPathName, O_RDWR | O_CREAT | O_APPEND, mode);
        if (fileDescriptor == -1){
            printf("open() failed on \"%s\"\n", yearPathName);
            perror("Error");
            exit(1);
	    }
        
        // Appends the movie title to the file and closes it.
        write(fileDescriptor, movieTitle, strlen(movieTitle));
        close(fileDescriptor);
        free(yearPathName);
        free(movieTitle);
        list = list->next;
    }
    return 0;
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
    off_t minSize = LONG_MAX;
    int i = 0;
    char *maxEntryName;
    char *minEntryName;
    char *notFound = "0";
    const char* fileExt = ".csv";
    bool flag = false;

    // Go through all the files in the directory
    while((aDir = readdir(currDir)) != NULL){

        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0){
            // Get meta-data for the current entry
            stat(aDir->d_name, &dirStat);
            
            // Confirm current file is a .csv
            char *isValid = strstr(aDir->d_name, fileExt);
            
            // File is a .csv: perform min/max comparisons and update variables
            if (isValid != NULL) {
                flag = true;
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
    // Close the directory and return file name or not found
    closedir(currDir);
    if (flag) {
        if (fileRequest) {
           return maxEntryName;
        } 
        else {
            return minEntryName;
        }
    } else {
        return notFound;
    }
}

/*
* Opens the inputted directory and searches for the requested filename.
* Returns 0 if file not found, returns 1 if file found. Code modified
* from module 3 Exploration on Directories: https://bit.ly/3mNYmC7
*/
int locateInputFile(char *dirName, char* filename) {

    // Open the current directory
    DIR* currDir = opendir(dirName);
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
*   Interactive element of the program. Gets user input and finds the 
*   largest, smallest, or user inputted .csv file containing a list of 
*   movie metadata. Creates a new directory and populates it with .txt 
*   files filtered by year.
*/
void subMenu() {
    int userNum;
    int min = 1;
    int max = 3;
    bool flag = true;
    char tempFile[100];
    char* notFound = "0";
    char* filepointer;
    char* dirName;
    char* currDir = ".";
    struct movie* list;

    // Sub menu loop for user interaction
    while(flag) {
        printf("\n***** Which file do you want to process? *****\n");
        printf("Enter 1 to pick the largest file.\n");
        printf("Enter 2 to pick the smallest file.\n");
        printf("Enter 3 to specify the name of a file.\n");
        printf("\nEnter a choice from 1 to 3: ");
        scanf("%d", &userNum);
        
        // Error handling algorith from: https://bit.ly/3oEQHIK
        if ((userNum - min) * (userNum - max) > 0) {
            printf("Invalid entry, please try again.\n");
            continue;
        } else {
            // Cases for handling user input
            switch(userNum) {
                case 1:
                    // Finds and processes largest file
                    filepointer = locateMinMaxFiles(1);
                    if (strcmp(notFound, filepointer) != 0) {
                        printf("Now processing the largest file named: %s\n", filepointer);
                        list = processFile(filepointer);
                        free(filepointer);
                        dirName = makeDir();
                        printf("Created directory with name: %s\n", dirName);
                        makeFiles(dirName, list);
                        flag = false;

                    // Error handling 
                    } else {
                        printf("Error, file not found. Please try again.\n");
                    }
                    break;
                case 2:
                    // Finds and processes smallest file
                    filepointer = locateMinMaxFiles(0);
                    if (strcmp(notFound, filepointer) != 0) {
                        printf("Now processing the smallest file named %s\n", filepointer);
                        list = processFile(filepointer);
                        free(filepointer);
                        dirName = makeDir();
                        printf("Created directory with name: %s\n", dirName);
                        makeFiles(dirName, list);
                        flag = false;

                    // Error handling
                    } else {
                        printf("Error, file not found. Please try again.\n");
                    }
                    break;
                case 3:
                    // Finds user entered filename and processes it
                    printf("Enter the complete file name: ");
                    scanf("%s", tempFile);
                    filepointer = calloc(strlen(tempFile) + 1, sizeof(char));
                    strcpy(filepointer, tempFile);
                    int ret = locateInputFile(currDir, filepointer);
                    if (ret == 1) {
                        printf("Now processing the file named: %s\n", filepointer);
                        list = processFile(filepointer);
                        free(filepointer);
                        dirName = makeDir();
                        printf("Created directory with name: %s\n", dirName);
                        makeFiles(dirName, list);
                        flag = false;

                    // Error handling
                    } else {
                        printf("Error, %s not found. Please try again.\n", filepointer);
                    }
                    break;
            }
        } 
    }
}

/*
*   Main Menu: first interactive element of the program. Gets user input 
*   and redirects to sub menu or allows user to exit the program. 
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
        
        // Error handling algorith from: https://bit.ly/3oEQHIK
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
