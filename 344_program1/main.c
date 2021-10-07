// Name: Ashley Owens
// Date: 10/7/2021
// Assignment 1: Movies 

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h> 

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
* Returns the length of the current movie struct
*/
int getListLength(struct movie *list) {
    int count = 0;
    while (list != NULL) {
        count++;
        list = list->next;
    }
    return count;
}

/*
*   Iterates through move struct. Prints year and title
*   for every movie matching user inputted language.
*/
void getMoviesByLanguage(struct movie *list, char* lang) {
    // Finds the matching language for each movie
    bool flag = true;

    while (list != NULL) {
        char *isValid = strstr(list->languages, lang);

        // Match found: print it
        if (isValid != NULL) {
            printf("%i %s\n", list->year, list->title);
            flag = false;
        }
        list = list->next;
    }
    if (flag) {
        printf("No data about movies released in %s\n", lang);
    }
}

/*
* Checks to see if a value is present in an array
* Returns 0 if False, 1 if True
*/
int findValueInArray(int val, int arr[], int length) {
    int i;
    for (i=0; i < length; i++) {
        if (arr[i] == val) {
            return 1;
        }
    }
    return 0;
}

/*
*   Iterates through move struct. Prints year, rating, and
*   movie titles for highest rated movies for each year.
*/
void getHighestRatedMovies(struct movie *list) {

    // Initializes variables
    float maxRating;
    int currYear;
    int buffer;
    struct movie *temp;
    int count = 0;
    int listLength = getListLength(list);
    int foundYears[listLength];

    // Iterates through LL printing out highest ratings
    while (list != NULL) {
        currYear = list->year;
        
        // Checks to see if the current year's highest rating has already been found.
        if (findValueInArray(currYear, foundYears, listLength) == 1) {
            foundYears[count] = currYear;
            list = list->next;
            count++;
            continue;

        // Finds the current year's highest rating by searching the rest of the LL.
        } else {
            maxRating = list->rating;
            buffer = strlen(list->title);
            char currTitle[buffer];
            strcpy(currTitle, list->title);
            temp = list->next;

            // Searches through the temp LL
            while (temp != NULL) {
                if (temp->year == currYear) {
                    if (temp->rating > maxRating) {
                        maxRating = temp->rating;
                        buffer = strlen(temp->title);
                        strcpy(currTitle, temp->title);
                    }
                }
                temp = temp->next;
            }

            // Stores the found year in an array to avoid duplicate searching
            foundYears[count] = currYear;
            printf("%i, %.1f, %s\n", currYear, maxRating, currTitle);
            count++;
            list = list->next;
        }
    }
}

/*
*   Iterates through move struct. Returns movie titles 
*   matching requested year by printing to the console.
*/
void getYear(struct movie *list, int year) {
    bool flag = true;
    
    // Iterates through linked list comparing year values
    while (list != NULL) {
        if (list->year == year) {
            printf("%s\n", list->title);
            flag = false;
        }
        list = list->next;
    }
    if (flag) {
        printf("No data about movies released in the year %i\n", year);
    }
}

/*
*   Interactive element of the program. Gets user input and returns
*   requested information from movie data by printing to the console.
*/
void userInteraction(struct movie *list) {
    // Initializes variables
    int userNum;
    int year;
    int min = 1;
    int max = 4;
    char lang[25];
    bool flag = true;

    // Main loop for user interaction
    while(flag) {
        printf("\n1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n");
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &userNum);
        
        // Algorith from: https://bit.ly/3oEQHIK
        if ((userNum - min) * (userNum - max) > 0) {
            printf("Invalid entry, please try again.\n");
            continue;
        }

        // Cases for handling user input
        switch(userNum) {
            case 1:
                printf("Enter the year for which you want to see movies: ");
                scanf("%d", &year);
                getYear(list, year);
                break;
            case 2:
                printf("These are the highest rated movies by year:\n");
                getHighestRatedMovies(list);
                break; 
            case 3:
                printf("Enter the language for which you want to see movies: ");
                scanf("%s", lang);
                getMoviesByLanguage(list, lang);
                break; 
            case 4:
                printf("Program exiting...\n");
                flag = false;
                break; 
        }
    }
}

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and prints out the list.
*   Compile the program as follows: gcc --std=gnu99 -o movies main.c
*   Code adapted from: https://replit.com/@cs344/studentsc#main.c
*/
int main(int argc, char **argv) {
    // argc is the number of inputs, char **argv are the string values 
    if (argc < 2) {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movie_list.csv\n");
        return EXIT_FAILURE;
    }

    // Adds file input to struct and prints the struct
    struct movie *list = processFile(argv[1]);
    
    // Ensures correct filename entry
    if (list == 0) {
        return EXIT_FAILURE;
    }

    userInteraction(list);
    return EXIT_SUCCESS;
}
