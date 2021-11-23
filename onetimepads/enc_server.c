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

char* receiveData(int connectionSocket) {
    char* data;
    size_t chunk = 1024;
    int charsRead;
    char buffer[chunk];
    int newLine = 0;

    // Allocates space on the heap for socket data
    data = (char*)malloc(chunk * sizeof(char));
    char* p = data;

    while (true) {
        // Clear temporary buffer for socket chunk
        memset(buffer, '\0', chunk);
        charsRead = recv(connectionSocket, buffer, sizeof(buffer) -1, 0);
        
        // Error reading from socket
        if (charsRead < 0) { error("enc_servert: ERROR reading from socket"); }

        // Server has received all data
        if (charsRead == 0) { break; }
        
        else {
            // Text input and key both contain a new line char
            if (strchr(buffer, '\n') != NULL) { 
                newLine += 1;
                printf("buffer is %s\n", buffer);
                printf("newlines: %d\n", newLine);
            }

            // Copy buffer to heap memory, add more memory for next buffer
            if (newLine <= 2) {
                strncpy(p, buffer, strlen(buffer));
                p += charsRead;
                data = realloc(data, chunk * sizeof(char));
            }
            
            // Too many new line chars, exit loop
            else { 
                break; 
            }
        }
    }
    return data;
}

void performEncryption(int socketFD) {

}

int main(int argc, char *argv[]){
    int connectionSocket;
    char* data;
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
                            ntohs(clientAddress.sin_port));
        
        // Perform initial handshake to verify client/server identities
        bool handshake = performHandShake(connectionSocket);

        if (handshake == true) {
            printf("handshake succeeded\n");
            data = receiveData(connectionSocket);
            printf("data is: %s", data);
            performEncryption(connectionSocket);
            free(data);
            
        }
        else {
            error("enc_server: dec_client cannot use enc_server\n");
        }

        
        // Close the connection socket for this client
        // close(connectionSocket); 
    }
    // Close the listening socket
    close(listenSocket); 
    return EXIT_SUCCESS;
}
