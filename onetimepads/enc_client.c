// Name: Ashley Owens
// Date: 11/29/2021
// Project 5: One-time Pads
// Encrypt Client: connects only to the encrypt server, requesting
// the server to encrypt ciphertext using passed-in text and key.


#include <ctype.h>
#include <netdb.h>      // gethostbyname()
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <unistd.h>


// Error function used for reporting issues
void error(const char *msg) { 
    perror(msg); 
    exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber) {
 
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 
    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname("localhost"); 
    if (hostInfo == NULL) { 
        fprintf(stderr, "enc_client error: no such host\n"); 
        exit(0); 
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, 
            hostInfo->h_addr_list[0],
            hostInfo->h_length);
}

/*
*   getFileText()
*   Opens a file in the current working directory. Allocates heap space
*   and stores the file's text to the heap. Assumes the file only has
*   one line of input of variable length.
*
*   args:   fileName - name of the file to open  
*   return: pointer to heap memory containing the file's text
*/
char* getFileText(char* fileName) {
    char *text;
    size_t size = 64;                               // Number of bytes to allocate
    FILE *fd = fopen(fileName, "r");

    // Error handling: fileName not in working directory
    if (fd == NULL) {
        fprintf(stderr, "enc_client: ERROR cannot open the file %s\n", fileName);
    }

    // Allocates space on the heap for plain text data
    text = (char*)malloc(size * sizeof(char));
    if (text == NULL) {
        fprintf(stderr, "enc_client: ERROR heap space full\n");
    }

    // Stores data on the heap
    getline(&text, &size, fd);
    return text;
}

/*
*   isValid()
*   Checks the given char arrays containing plaintext and key for correct
*   length and valid characters. 
*
*   arg:    text - pointer to plaintext char array  
*   arg:    key  - pointer to key char array
*   return: false for errors, else true for valid input
*/
bool isValid(char* text, char* key) {
    // Checks for error: key file is shorter than the plaintext
    if (strlen(key) < strlen(text)) {
        fprintf(stderr,"enc_client error: key is too short\n");
        return false;
    }

    // Checks for error: plaintext file with ANY bad characters
    for (int i=0; i < strlen(text); i++) {
        if (isupper(text[i]) == 0 && text[i] != ' ' && text[i] != '\n') {
            fprintf(stderr, "enc_client error: text file contains bad characters\n");
            return false;
        }
    }

    // Checks for error: key file with ANY bad characters
    for (int i=0; i < strlen(key); i++) {
        if (isupper(key[i]) == 0 && key[i] != ' ' && key[i] != '\n') {
            fprintf(stderr, "enc_client error: key file contains bad characters\n");
            return false;
        }
    }
    return true;
}

/*
*   performHandshake()
*   Performs initial handshake with server, sending client's program name
*   to verify that only the encryption client/server can connect.
*
*   arg:    socketFD - socket file descriptor 
*   return: false for errors, else true for valid connection
*/
bool performHandShake(int socketFD) {
    char  buffer[4096];
    int   charsRead, charsWritten;
    char* client = "enc_client";
    char* server = "enc_server";

    // Clear buffer
    memset(buffer, '\0', 4096);

    // Send message through socket to the server
    charsWritten = send(socketFD, client, strlen(client), 0);
    if (charsWritten < 0) {
        error("enc_client: ERROR writing to socket\n");
    }
    if (charsWritten < strlen(buffer)) {
        printf("enc_client: WARNING: Not all data written to socket!\n");
    }

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        error("enc_client: ERROR reading from socket");
    }
    // printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

    // Confirms that communication with this server is valid
	if (strcmp(buffer, server) == 0) {
		return true;
	}
	return false;
}

void sendData(char* data, int socketFD) {
    char buffer[4096];
    // TODO: send data to server
    
}

int main(int argc, char *argv[]) {
    int socketFD, option;
    struct sockaddr_in serverAddress;

    // Check usage & args
    if (argc != 4) { 
        fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]); 
        exit(EXIT_FAILURE); 
    }

    // Helper functions to save file input into char arrays in heap memory
    char *text = getFileText(argv[1]);
    char *key = getFileText(argv[2]);

    // Helper function to validate file input lengths and characters
    bool valid = isValid(text, key);
    if (valid == false) {
        exit(EXIT_FAILURE);
    }

    // Create a socket and check for errors
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0) {
        error("enc_client: ERROR opening socket\n");
    }

    // Allow the port to be reused
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]));

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        error("enc_client: ERROR connecting to server\n");
    }
    
    // Perform initial handshake to verify client/server identities
    bool handshake = performHandShake(socketFD);
    if (handshake == true) {
        sendData(text, socketFD);
        sendData(key, socketFD);
    }
    else {
        error("enc_server: client failed handshake\n");
        exit(EXIT_FAILURE);
    }

    // TODO: Receive DATA from server

    // Close the socket
    close(socketFD);

    // Free heap memory
    free(text);
    free(key);
    return EXIT_SUCCESS;
}