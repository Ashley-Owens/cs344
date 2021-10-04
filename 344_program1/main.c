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

/* Parse the current line which is space delimited and create a
*  student struct with the data in this line
*/
struct movie *createMovie(char *currLine) {
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the movie title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the year, stored as integer
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = atoi(token);

    // The next token is the array of languages
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // The last token is the rating, stored as float
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rating = atof(token);

    // Set the next node to NULL in the newly created student entry
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

        // Is this the first node in the linked list?
        if (head == NULL) {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else {
            // This is not the first node.
            // Add this node to the list and advance the tail
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
* Print data for the given movie
*/
void printMovie(struct movie* aMovie){
  printf("%s, %i, %s, %.1f\n", aMovie->title,
               aMovie->year,
               aMovie->languages,
               aMovie->rating);
}

/*
* Print the linked list of movies
*/
void printMovieList(struct movie *list)
{
    while (list != NULL)
    {
        printMovie(list);
        list = list->next;
    }
}

/*
*   Process the file provided as an argument to the program to
*   create a linked list of movie structs and prints out the list.
*   Compile the program as follows:
*       gcc --std=gnu99 -o movies main.c
*/
int main(int argc, char **argv) {
    // argc is the number of inputs, char **argv are the string values 
    // From example program: https://replit.com/@cs344/studentsc#main.c
    if (argc < 2)
    {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movies movie_list.csv\n");
        return EXIT_FAILURE;
    }

    // Adds file input to struct and prints the struct
    struct movie *list = processFile(argv[1]);
    printMovieList(list);
    return EXIT_SUCCESS;





    // // Pointer to the inputted filename
    // char *fileName = argv[1];
    // // Create movie struct to hold movies
    // struct movie movie_list[1000];
    // char newline[4096];
    // int count;

    // // Opens the file in read mode
    // FILE *filePtr = fopen(fileName, "r");

    // // Exits the program if the file couldn't be opened
    // if (!filePtr){
    //         printf("Error: could not open the file %s\n", fileName);
    //         exit(1);
    // }

    // // File was opened properly, reading it in
    // while (fgets(newline, sizeof(newline), filePtr)) {
        
    //     // Ignores first line with Category Descriptors
    //     if (count >= 1) {

    //         // Returns a pointer to the new, duplicated string
    //         char *temp = strdup(newline);
    //         // Uses delimitter to break up strings into tokens 
    //         char *token = strtok(temp, ",\n");
    //         int i = 1;
            
    //         // Adds each token to the Movie Struct list
    //         // Modified from TutorialsPoint 10/2/21: https://bit.ly/3ipVjyo
    //         while (token != NULL) {
                
    //             // Handles movie title
    //             if(i == 1) {
    //                 strcpy(movie_list[count - 1].title, token);
    //             }

    //             // Handles movie year
    //             else if(i == 2) {
    //                 int year = atoi(token);
    //                 movie_list[count - 1].year = year;
    //             }

    //             // Handles movie languages
    //             else if(i == 3) {
    //                 strcpy(movie_list[count - 1].languages, token);
    //             }

    //             // Handles movie ratings
    //             else if(i == 4) {
    //                 movie_list[count - 1].rating = atof(token);
    //             }

    //             // printf(" %s\n", token);
    //             // Sets token to Null in order to point to next token
    //             token = strtok(NULL, ",\n");
    //             i++;
    //         }
    //         // frees up memory used by temp from strdup call
    //         void free(void *temp);
    //     }
    //     count++;
    // }

   
    // printf("Give me a number please: ");
    // scanf("%d", &y);
    // printf("You chose the value: %d\n", y);
    // printf("%s\n", file);
    // scanf("%s", file);
    // printf("Your name is: %s\n", name);

    return 0;
}