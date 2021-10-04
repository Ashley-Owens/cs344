#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
 
// argc is the number of inputs, char **argv are the string values 
int main(int argc, char **argv) {

    // Creates movie struct
    struct movie {
        char title[500];
        int year;
        char languages[500];
        float rating;
        struct movie *next;
    };

    // Pointer to the inputted filename
    char *fileName = argv[1];
    // Create movie struct to hold movies
    struct movie movie_list[1000];
    char newline[4096];
    int count;

    // Opens the file in read mode
    FILE *filePtr = fopen(fileName, "r");

    // Exits the program if the file couldn't be opened
    if (!filePtr){
            printf("Error: could not open the file %s\n", fileName);
            exit(1);
    }

    // File was opened properly, reading it in
    while (fgets(newline, sizeof(newline), filePtr)) {
        
        // Ignores first line with Category Descriptors
        if (count >= 1) {

            // Returns a pointer to the new, duplicated string
            char *temp = strdup(newline);
            // Uses delimitter to break up strings into tokens 
            char *token = strtok(temp, ",\n");
            int i = 1;
            
            // Adds each token to the Movie Struct list
            // Modified from TutorialsPoint 10/2/21: https://bit.ly/3ipVjyo
            while (token != NULL) {
                
                // Handles movie title
                if(i == 1) {
                    strcpy(movie_list[count - 1].title, token);
                }

                // Handles movie year
                else if(i == 2) {
                    int year = atoi(token);
                    movie_list[count - 1].year = year;
                }

                // Handles movie languages
                else if(i == 3) {
                    strcpy(movie_list[count - 1].languages, token);
                }

                // Handles movie ratings
                else if(i == 4) {
                    movie_list[count - 1].rating = atof(token);
                }

                // printf(" %s\n", token);
                // Sets token to Null in order to point to next token
                token = strtok(NULL, ",\n");
                i++;
            }
            // frees up memory used by temp from strdup call
            free(temp);
        }
        count++;
    }
    // Testing struct values
    for (int i=0; i < count -1; i++) {
        // Prints the rating to one decimal point
        printf("%i: %.1f\n", i, movie_list[i].rating);
    }
    
    printf("Processed file %s and parsed data for %d movies \n", fileName, count -1);
    
	


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