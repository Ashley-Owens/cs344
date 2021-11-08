// Name: Ashley Owens
// Date: 11/18/2021
// Project 4: Multi-threaded Producer Consumer Pipeline


#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_LINES     50
#define INPUT_LENGTH  1000
#define OUTPUT_LENGTH 80
#define END_MARKER    "STOP\n"                              


// Buffer 1: shared resource between input and separator threads
char* inputBuffer[MAX_LINES];
int   count_1 = 0;                                          // Number of items in the buffer
int   prod_idx_1 = 0;                                       // Index where the input thread will put the next item
int   con_idx_1 = 0;                                        // Index where the separator thread will access the next item
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;        // Initialize the mutex for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;           // Initialize the condition variable for buffer 1                

// Buffer 2: shared resource between separator and sign threads
char* swapCharsBuffer[MAX_LINES];                           // Buffer to replace \n and ++
int   count_2 = 0;                                          // Number of items in the buffer
int   prod_idx_2 = 0;                                       // Index where the separator thread will put the next item
int   con_idx_2 = 0;                                        // Index where the sign thread will access the next item
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;        // Initialize the mutex for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;           // Initialize the condition variable for buffer 2    

// Buffer 3: shared resource between sign and output threads
char  outputBuffer[OUTPUT_LENGTH];                          // Buffer to hold output
int   count_3 = 0;                                          // Number of items in the buffer
int   prod_idx_3 = 0;                                       // Index where the sign thread will put the next item
int   con_idx_3 = 0;                                        // Index where the output thread will access the next item
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;        // Initialize the mutex for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;           // Initialize the condition variable for buffer 3


// For testing purposes
void printBuffer() {

    for (int i=0; i < MAX_LINES; i++) {
        printf("line %d: %s\n", i, swapCharsBuffer[i]);
    }
}

/*
*   freeBuffers()
*   Iterates through global buffers, freeing each string's
*   allocated memory from using strdup() earlier in the program.
*/
void freeBuffers(void) {
    for (int i=0; i < MAX_LINES; i++) {

        // Frees each string from earlier strdup() calls
        if (inputBuffer[i] != NULL) {
            free(inputBuffer[i]);
        }  
        if (swapCharsBuffer[i] != NULL) {
            free(swapCharsBuffer[i]);
        }
    }
}

/*
*   printOutputBuffer()
*   Acts as a consumer when outputBuffer string is filled, prints
*   the string, resets the buffer, and updates the boolean flag.
*/
// void printOutputBuffer(void) {

//     if (outputReady && strlen(outputBuffer) == OUTPUT_LENGTH) {
//         printf("%s\n", outputBuffer);
//         memset(outputBuffer, 0, sizeof(outputBuffer));
//         outputReady = false;
//     }
// }

/*
*   replacePlusSigns()
*   Acts as a consumer and producer: gets string from swapCharsBuffer,
*   replaces each ++ with a ^ char. Writes updated string to the 
*   outputBuffer for outputThread() to consume.
*   Code modified from Producer/Consumer example: https://bit.ly/3BW2423
*/
void replacePlusSigns(void) {
    char buffer[INPUT_LENGTH];
    int i = 0;

    // Iterates through swapCharsBuffer array
    while (swapCharsBuffer[i] != NULL && i < MAX_LINES) {
        strcpy(buffer, swapCharsBuffer[i]);                     // Copies each string into temp buffer

        // Replaces double plus signs with a caret char
        for (int j=1; j < strlen(buffer); j++) {
            if ((buffer[j - 1] == '+') && (buffer[j] == '+')) {
                char* temp = strdup(buffer);                    // Creates a temp copy of buffer string
                temp[j-1] = '%';                                // Replaces ++ with %c for char insertion
                temp[j] = 'c';
                sprintf(buffer, temp, '^');                     // Overwrites buffer with caret in the '++' position
                free(temp);                                     // Frees the temp variable from calling strdup()
            }
        }

        // Appends 80 chars to output buffer
        // for (int k=0; k < INPUT_LENGTH; k++) {
        //     if (!outputReady && strlen(outputBuffer) < OUTPUT_LENGTH) {
        //         strncat(outputBuffer, &buffer[k], 1);
        //     } else {
        //         outputReady = true;
        //     }
        // }
        i++;
    }
}

/*
*   replaceLineSeparators()
*   Acts as a consumer and producer: gets next string from inputBuffer,
*   replacing each \n with a space char (except for end marker newline). 
*   Writes string to the swapCharsBuffer for signThread() to consume.
*   Code modified from Producer/Consumer example: https://bit.ly/3BW2423
*
*   Return: 0 to end, else 1 to continue consuming and producing
*/
int replaceLineSeparators(void) {
    char buffer[INPUT_LENGTH];
    bool end = false;

    // Lock mutex_1 before checking the buffer for data
    pthread_mutex_lock(&mutex_1);               
    while (count_1 == 0) {
        // Buffer is empty. Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_1, &mutex_1);
    }

    // Copy data into temp string
    strcpy(buffer, inputBuffer[con_idx_1]);
    con_idx_1++;                                            // Increment consumer index for next str
    count_1--;                                              // Decrement count_1                                                  
    pthread_mutex_unlock(&mutex_1);                         // Unlock mutex_1

    if (strcmp(buffer, END_MARKER) == 0) {
        end = true;
    }

    // If string is not the end marker, replaces newlines with an empty space char
    if (!end) {
        for (int i=0; i < strlen(buffer); i++) {
            if (strcmp(&buffer[i], "\n") == 0) {
                buffer[i] = ' ';
            }
        }
    }

    // Lock mutex_2 in order to place string in the next buffer
    pthread_mutex_lock(&mutex_2);
    swapCharsBuffer[prod_idx_2] = strdup(buffer);
    count_2++;
    prod_idx_2++;
    pthread_cond_signal(&full_2);                           // Signal that 2nd buffer is no longer empty
    pthread_mutex_unlock(&mutex_2);
    if (end) {
        return 0;
    }
    return 1;
}

/*
*   getInput()
*   Using fgets(), obtains input from stdin in a temporary buffer.
*   Locks the mutex prior to accessing global buffer, then copies
*   the obtained string into the global buffer array. Finally, updates
*   signal to next thread that buffer isn't empty and unlocks the mutex.
*   Code modified from Producer/Consumer example: https://bit.ly/3BW2423
* 
*   Returns: 0 to end, else 1 to continue receiving input
*/
int getInput(void) {
    char buffer[INPUT_LENGTH];
    fgets(buffer, INPUT_LENGTH, stdin);

    pthread_mutex_lock(&mutex_1);                               // Lock mutex
    inputBuffer[prod_idx_1] = strdup(buffer);                   // Put string in global buffer
    prod_idx_1++;                                               // Increment index for next string
    count_1++;                                                  // Increment number of items in buffer

    pthread_cond_signal(&full_1);                               // Signal that buffer is no longer empty
    pthread_mutex_unlock(&mutex_1);                             // Unlock the mutex

    // Stop processing input when marker received
    if (strcmp(buffer, END_MARKER) == 0) {
        return 0;
    } 
    return 1;
}

void *outputThread(void *args) {
    return NULL;
}

void *signThread(void *args) {
    return NULL;
}

/*
*   separatorThread()
*   Uses helper function to obtain string from inputBuffer,
*   removes newline chars and adds string to a swapCharsBuffer
*   global buffer array. When end marker is received, returns
*   Null to exit the thread.
*/
void *separatorThread(void *args) {
    int res = 1;
    while (res) { res = replaceLineSeparators(); }
    return NULL;
}

/*
*   inputThread()
*   Uses helper function to obtain input from stdin, adding
*   it to a shared global buffer array. When end marker is
*   received, returns Null to exit the thread.
*/
void *inputThread(void *args) {
    int res = 1;
    while (res) { res = getInput(); }
    return NULL;
}

/*
*   main()
*   Determines whether user has requested input redirect.
*   If so, uses helper function to parse the file input 
*   residing in the stdin buffer. Else, calls helper function
*   to obtain user input from the terminal.
*/
int main(void) {
    pthread_t inputTid, separatorTid, signTid, outputTid;

    // Create threads
    pthread_create(&inputTid, NULL, inputThread, NULL);
    pthread_create(&separatorTid, NULL, separatorThread, NULL);
    pthread_create(&signTid, NULL, signThread, NULL);
    pthread_create(&outputTid, NULL, outputThread, NULL);
    
    // Wait for the thread to finish
    pthread_join(inputTid, NULL);
    pthread_join(separatorTid, NULL);
    pthread_join(signTid, NULL);
    pthread_join(outputTid, NULL);
    
    printBuffer();
    freeBuffers();
    
    return EXIT_SUCCESS;
}
