// Name: Ashley Owens
// Date: 11/29/2021
// Project 5: One-time Pads
// Encrypt Server: connects only to the encrypt client, encrypting
// the passed-in plaintext and key, returning ciphertext to enc_client.
// Server runs in the background and supports up to 5 concurrent socket connections.


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
    exit(EXIT_FAILURE);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber) {
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 
    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}

/*
*   performHandshake()
*   Performs initial handshake with server, sending client's program name
*   to verify that it is a valid client for connection.
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

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        error("enc_server: ERROR reading from socket\n");
    }
    // printf("Server: I received this from the client: \"%s\"\n", buffer);

    // Send message through socket to the server
    charsWritten = send(socketFD, server, strlen(server), 0);
    if (charsWritten < 0) {
        error("enc_server: ERROR writing to socket\n");
    }
    if (charsWritten < strlen(buffer)) {
        printf("enc_server: WARNING not all data written to socket!\n");
    }

    // Confirms that communication with this client is valid
	if (strcmp(buffer, client) == 0) {
		return true;
	}
	return false;
}

/*
*   receiveData()
*   Reads socket data into a temp buffer, writing it to heap memory
*   for later use.
*
*   arg:    socketFD - socket file descriptor 
*   return: pointer to heap memory containing socket data
*/
char* receiveData(int socketFD) {
    char*  data;
    size_t chunk = 1024;
    int    charsRead, newlines;
    char   buffer[chunk];  

    // Allocates space on the heap for socket data
    data = (char*)malloc(chunk * sizeof(char));
    char* p = data;

    while (true) {
        // Clear temporary buffer for socket chunk
        memset(buffer, '\0', chunk);
        charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0);
        
        // Error reading from socket
        if (charsRead < 0) { error("enc_server: ERROR reading from socket in receiveData()\n"); }

        // Server has received all data
        if (charsRead == 0) { break; }
        
        else {
            // Socket data will only have two newline chars
            for (int i=0; i < strlen(buffer); i++) {
                if (buffer[i] == '\n') {
                    newlines += 1;
                }
            }
            // Copies buffer string to heap memory, adds more memory for next buffer
            if (newlines < 3) {
                strncpy(p, buffer, strlen(buffer));
                p += charsRead;
                data = realloc(data, chunk * sizeof(char));
            } else { break; }
        }
    }
    return data;
}

/*
*   encryptData()
*   Encrypts socket data, storing encrypted text in heap memory.
*
*   arg:    data - plaintext and key received from socket
*   return: pointer to heap memory containing encrypted text
*/
char* encryptData(char* data) {
    // Finds start of key and copies to buffer
    // Doesn't copy newline chars at start and end of key
    char* k = strchr(data, '\n');
    int   keyLen = strlen(k);
    char  key[keyLen];
    memset(key, '\0', keyLen);
    strncpy(key, k + 1, keyLen - 2);
    // printf("key length is: %d\n", keyLen);
    // printf("key is: %s\n", key);

    // Copies text to buffer
    int  textLen = strlen(data) - keyLen;
    char text[textLen];
    memset(text, '\0', textLen + 1);
    strncpy(text, data, textLen);
    
    // printf("text is: %s\n", text);
    // printf("text length is: %d\n", textLen);
    char* encryptedText;
    // char buffer[int p];
    // char text[length];
    // char key[length];
    // memset(text, '\0', length);

     // Allocates heap memory to store encrypted text
    // encryptedText = (char*)malloc(strlen(data) * sizeof(char));
    // printf("data length is: %d\n", length);
    // printf("data is: %s", data);


    return encryptedText;
}

int main(int argc, char *argv[]){
    int connectionSocket;
    char* data;
    char* encryptedText;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc != 2) { 
        fprintf(stderr,"enc_server: USAGE %s port\n", argv[0]); 
        exit(EXIT_FAILURE);
    } 
    
    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("enc_server: ERROR opening server socket\n");
    }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));
    // Associate the socket to the port
    int bindSocket = bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    // Check for errors
    if (bindSocket < 0) {
        error("enc_server: ERROR on binding socket\n");
    }

    // Start listening for connections. Allow up to 5 connections to queue up
    listen(listenSocket, 5); 
    
    // Accept a connection, blocking if one is not available until one connects
    while(1){
        // Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
        if (connectionSocket < 0) {
            error("enc_server: ERROR on accept\n");
        }

        printf("enc_server: Connected to client running at host %d port %d\n", 
                ntohs(clientAddress.sin_addr.s_addr),
                ntohs(clientAddress.sin_port)
        );
        
        // Perform initial handshake to verify client/server identities
        bool handshake = performHandShake(connectionSocket);

        // Client is verified, receive data, encrypt it, and send back
        if (handshake == true) {
            data = receiveData(connectionSocket);
            encryptedText = encryptData(data);
            free(data);
            free(encryptedText);
        }
        // Client cannot be verified, display error and close socket
        else {
            error("enc_server: dec_client cannot use enc_server\n");
        }
        
        // Close the connection socket for this client
        close(connectionSocket); 
    }
    // Close the listening socket
    close(listenSocket); 
    return EXIT_SUCCESS;
}
