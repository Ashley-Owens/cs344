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
        fprintf(stderr, "dec_client: ERROR no such host\n"); 
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
        fprintf(stderr, "dec_client: ERROR cannot open the file %s\n", fileName);
    }

    // Allocates space on the heap for plain text data
    text = (char*)malloc(size * sizeof(char));
    if (text == NULL) {
        fprintf(stderr, "dec_client: ERROR heap space full\n");
    }

    // Stores data on the heap
    getline(&text, &size, fd);
    return text;
}

/*
*   isValid()
*   Checks the given char arrays containing encrypted text and key
*   for correct length and valid characters. 
*
*   arg:    text - pointer to encrypted text char array  
*   arg:    key  - pointer to key char array
*   return: false for errors, else true for valid input
*/
bool isValid(char* text, char* key) {
    // Checks for error: key file is shorter than the encrypted text
    if (strlen(key) < strlen(text)) {
        fprintf(stderr,"dec_client: ERROR key is too short\n");
        return false;
    }

    // Checks for error: encrypted text file with ANY bad characters
    for (int i=0; i < strlen(text); i++) {
        if (isupper(text[i]) == 0 && text[i] != ' ' && text[i] != '\n') {
            fprintf(stderr, "dec_client: ERROR text file contains bad characters\n");
            return false;
        }
    }

    // Checks for error: key file with ANY bad characters
    for (int i=0; i < strlen(key); i++) {
        if (isupper(key[i]) == 0 && key[i] != ' ' && key[i] != '\n') {
            fprintf(stderr, "dec_client: ERROR key file contains bad characters\n");
            return false;
        }
    }
    return true;
}

/*
*   performHandshake()
*   Performs initial handshake with server, sending client's program name
*   to verify that only the decryption client/server can connect.
*
*   arg:    socketFD - socket file descriptor 
*   return: false for errors, else true for valid connection
*/
bool performHandShake(int socketFD) {
    char  buffer[1024];
    int   charsRead, charsWritten;
    char* client = "dec_client";
    char* server = "dec_server";

    // Clear buffer
    memset(buffer, '\0', 1024);

    // Send message through socket to the server
    charsWritten = send(socketFD, client, strlen(client), 0);
    if (charsWritten < 0) {
        error("dec_client: ERROR writing to socket\n");
    }
    if (charsWritten < strlen(buffer)) {
        printf("dec_client: WARNING not all data written to socket!\n");
    }

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        error("dec_client: ERROR reading from socket");
    }

    // Confirms that communication with this server is valid
	if (strcmp(buffer, server) == 0) {
		return true;
	}
	return false;
}

/*
*   sendAndReceiveData()
*   First sends the size of the outgoing data, then concatenates 
*   encrypted and key, sending all data through socket connection.
*   Reads socket data into a temp buffer, writing it to heap memory
*   for later use.
*
*   arg:    data - pointer to encrypted text char array  
*   arg:    key  - pointer to key char array
*   arg:    socketFD  - socket file descriptor
*
*   return: decryptedText - pointer to encrypted text data
*/
char* sendAndReceiveData(char* data, char* key, int socketFD) {
    size_t chunk = 1024;
    int    charsWritten, charsRead;
    int    length = strlen(data) + strlen(key);

    // Temp buffer stores length of outgoing data
    char dataSizeBuffer[15];
    sprintf(dataSizeBuffer, "%d", length);
    int bufferLen = strlen(dataSizeBuffer);
    char *p = dataSizeBuffer;
    
    // Sends data size to server to prevent socket from hanging
    while (bufferLen > 0) {
        charsWritten = send(socketFD, dataSizeBuffer, bufferLen, 0);
        bufferLen -= charsWritten;
        p += charsWritten;
    }

    // Concatenates encrypted text and key
    char* buffer = (char *)malloc(length);
    strcpy(buffer, data);
    strcat(buffer, key);
    char* pointer = buffer;

    // Sends concatenated data to the server
    while (length > 0) {
        charsWritten = send(socketFD, pointer, length, 0);
        length -= charsWritten;
        pointer += charsWritten;
    }
    free(buffer);

    // Get size of incoming data from server
    char recvBuffer[chunk];
    memset(recvBuffer, '\0', chunk);
    charsRead = recv(socketFD, recvBuffer, chunk, 0); 
    if (charsRead < 0) error("dec_client: ERROR reading from socket in sendAndReceiveData()\n");
    
    // Allocates space on the heap for socket data
    length = atoi(recvBuffer);
    char* decryptedText = (char*)malloc(length * sizeof(char));
    p = decryptedText;

    while (length > 0) {
        // Clear temporary buffer for incoming socket chunk
        memset(recvBuffer, '\0', chunk);
        charsRead = recv(socketFD, recvBuffer, chunk -1, 0);
        
        // Error reading from socket
        if (charsRead < 0) { error("dec_client: ERROR reading from socket in sendAndReceiveData()\n"); }
        
        // Copies buffer string to heap memory
        else {
            strncpy(p, recvBuffer, strlen(recvBuffer));
            p += charsRead;
            length -= charsRead;
        }
    }
    return decryptedText;
}

/*
*   main()
*   Runs main program loop for opening socket and connecting
*   to server. Calls helper functions for reading and storing
*   file data in order to sent to server. Receives decrypted 
*   data from server and prints it to stdout.
*
*   arg:    argc - number of CL arguments  
*   arg:    argv - string array of CL text arguments
*
*   return: EXIT_SUCCESS or EXIT_FAILURE
*/
int main(int argc, char *argv[]) {
    int    socketFD, option;
    struct sockaddr_in serverAddress;
    char*  decryptedText;

    // Check usage & args
    if (argc != 4) { 
        fprintf(stderr,"dec_client: USAGE %s encryptedText key port\n", argv[0]); 
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
        error("dec_client: ERROR opening socket");
    }

    // Allow the port to be reused and set up server address struct
    setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
    setupAddressStruct(&serverAddress, atoi(argv[3]));

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        error("dec_client: ERROR connecting to server");
    }
    
    // Perform initial handshake to verify client/server identities
    bool handshake = performHandShake(socketFD);

    // Server is verified, send data and receive decrypted text from server
    if (handshake == true) {
        decryptedText = sendAndReceiveData(text, key, socketFD);
        printf("%s", decryptedText);
    }
    else {
        fprintf(stderr, "dec_client: ERROR client failed handshake\n");
        exit(2);
    }

    // Close the socket
    close(socketFD);

    // Free heap memory
    free(text);
    free(key);
    free(decryptedText);
    return EXIT_SUCCESS;
}
