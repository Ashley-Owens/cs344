// Name: Ashley Owens
// Date: 11/29/2021
// Project 5: One-time Pads
// Decrypt Server: decrypts the passed-in ciphertext 
// and key, returning plaintext to dec_client.


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
    char* client = "dec_client";
    char* server = "dec_server";

    // Clear buffer
    memset(buffer, '\0', 4096);

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
    if (charsRead < 0){
        error("dec_server: ERROR reading from socket\n");
    }

    // Send message through socket to the server
    charsWritten = send(socketFD, server, strlen(server), 0);
    if (charsWritten < 0) {
        error("dec_server: ERROR writing to socket\n");
    }
    if (charsWritten < strlen(buffer)) {
        printf("dec_server: WARNING not all data written to socket!\n");
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
    size_t chunk = 1024;
    int    charsRead, length;
    char   buffer[chunk];

    // Get size of incoming data from client
    memset(buffer, '\0', chunk);
	charsRead = recv(socketFD, buffer, chunk, 0); 
	if (charsRead < 0) error("dec_server: ERROR reading from socket in receiveData()");

    // Allocate space on the heap for incoming socket data
    length = atoi(buffer);
    char* data = (char*)malloc(length * sizeof(char));
    char* p = data;

    while (length > 0) {
        // Clear temporary buffer for incoming socket chunk
        memset(buffer, '\0', chunk);
        charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0);
        
        // Error reading from socket
        if (charsRead < 0) { error("dec_server: ERROR reading from socket in receiveData()"); }
        
        // Copies buffer string to heap memory
        else {
            strncpy(p, buffer, strlen(buffer));
            p += charsRead;
            length -= charsRead;
        }
    }
    return data;
}

/*
*   decryptData()
*   Decrypts socket data, storing decrypted text in heap memory.
*
*   arg:    data - encrypted text and key received from socket
*   return: pointer to heap memory containing decrypted text
*/
char* decryptData(char* data) {
    // Finds start and end of key by newline chars
    char* j = strchr(data, '\n');
    int   textLen = strlen(data) - strlen(j);
    char* k = strchr(j + 1, '\n');
    int   keyLen = (strlen(j) - 1) - strlen(k);
    
    // Copies key string to null terminated buffer
    char  key[keyLen + 1];
    memset(key, '\0', sizeof(key));
    strncpy(key, j + 1, keyLen);
    
    // Copies plain text string to null terminated buffer
    char text[textLen + 1];
    memset(text, '\0', sizeof(text));
    strncpy(text, data, textLen);
    
    // Allocates heap memory to store decrypted text
    char* decryptedText = (char*)malloc((textLen +1) * sizeof(char));
    int temp;
    char letters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    // Iterates through encrypted text, decrypting one char at a time
    for (int i=0; i < strlen(text); i++) {
        
        if (text[i] == ' ') {
            // Cipher char and key char are both spaces
            if (key[i] == ' ') {
                temp = 0;
            // Cipher char is a space but key char is not a space
            } else {
                temp = 0 - (key[i] - 64);
                if (temp < 0) {
                    temp += 27;
                }
            }
        
        } else {
            // Cipher char is not a space but key char is a space
            if (key[i] == ' ') {
                temp = text[i] - 64;
                if (temp < 0) {
                    temp += 27;
                }

            // Cipher and key chars are not spaces
            } else {
                temp = (text[i] - 64) - (key[i] - 64);
                if (temp < 0) {
                    temp += 27;
                }
            }
        }
        // Stores decrypted char in text buffer
        text[i] = letters[abs(temp)];
    }
    // Copies decrypted buffer to heap memory
    strcpy(decryptedText, text);
    strcat(decryptedText, "\n");
    return decryptedText;
}

/*
*   sendData()
*   First sends the size of the outgoing data, then sends
*   decrypted text through socket connection.
*
*   arg:    data - pointer to decrypted text  
*   arg:    socketFD  - socket file descriptor
*/
void sendData(char* data, int socketFD) {
    int  charsWritten;
    int  length = strlen(data);

    // Temp buffer stores length of outgoing data
    char dataSizeBuffer[15];
    sprintf(dataSizeBuffer, "%d", length);
    int bufferLen = strlen(dataSizeBuffer);
    char *p = dataSizeBuffer;

    // Sends data size to client to prevent socket from hanging
    while (bufferLen > 0) {
        charsWritten = send(socketFD, dataSizeBuffer, bufferLen, 0);
        bufferLen -= charsWritten;
        p += charsWritten;
    }

    // Sends decrypted data through socket to client
    p = data;
    sleep(1);
    while (length > 0) {
        charsWritten = send(socketFD, p, length, 0);
        length -= charsWritten;
        p += charsWritten;
    }
}

int main(int argc, char *argv[]){
    int    connectionSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc != 2) { 
        fprintf(stderr,"dec_server: USAGE %s port\n", argv[0]); 
        exit(EXIT_FAILURE);
    } 
    
    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("dec_server: ERROR opening server socket\n");
    }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));
    // Associate the socket to the port
    int bindSocket = bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    // Check for errors
    if (bindSocket < 0) {
        error("dec_server: ERROR on binding socket\n");
    }

    // Start listening for connections. Allow up to 5 connections to queue up
    listen(listenSocket, 5); 
    
    // Accept a connection, blocking if one is not available until one connects
    while (true){
        // Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
        if (connectionSocket < 0) {
            error("dec_server: ERROR on accept\n");
        }
        
        // Perform initial handshake to verify client/server identities
        bool handshake = performHandShake(connectionSocket);

        // Client is verified, receive data, decrypt it, and send back
        if (handshake == true) {
            printf("dec_server: Connected to client running at host %d port %d\n", 
                ntohs(clientAddress.sin_addr.s_addr),
                ntohs(clientAddress.sin_port)
            );
            char* data = receiveData(connectionSocket);
            char* decryptedText = decryptData(data);
            sendData(decryptedText, connectionSocket);
            free(data);
            free(decryptedText);
            // Close the connection socket for this client
            close(connectionSocket); 
        }
        // Client cannot be verified, display error
        else {
            fprintf(stderr, "dec_server: ERROR client cannot use this server\n");
        }
    }
    // Close the listening socket
    close(listenSocket); 
    return EXIT_SUCCESS;
}
