#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

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
    // memmove(&token[0], &token[1], strlen(token));
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // The last token is the rating, stored as a float
    token = strtok_r(NULL, "\n", &saveptr);
    // printf("token is %f", token);
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

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;
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

void userInteraction() {
    int userNum;
    int min = 1;
    int max = 4;
    bool flag = true;

    while(flag) {
        printf("1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n");
        printf("Enter a choice from 1 to 4: ");
        scanf("%d", &userNum);
        
        // Algorith from: https://bit.ly/3oEQHIK
        // if ((userNum - min) * (userNum - max) <= 0) {
        //     printf("You have selected option %d\n", userNum);
        //     flag = false;
        // } else {
        //     printf("Invalid entry, please try again.\n");
        // }
        switch(userNum) {

            case 1:
                printf("You have selected option %d\n", userNum);
                break;
            case 2:
                printf("You have selected option %d\n", userNum);
                break; 
            case 3:
                printf("You have selected option %d\n", userNum);
                break; 
            case 4:
                printf("You have selected option %d\n", userNum);
                printf("Program exiting...\n");
                flag = false;
                break; 
            default : 
                printf("Invalid entry, please try again.\n");
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
    printMovieList(list);
    userInteraction();
    return EXIT_SUCCESS;
    
    // printf("Give me a number please: ");
    // scanf("%d", &y);
    // printf("You chose the value: %d\n", y);
    // printf("%s\n", file);
    // scanf("%s", file);
    // printf("Your name is: %s\n", name);

}