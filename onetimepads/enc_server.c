// Name: Ashley Owens
// Date: 11/29/2021
// Project 5: One-time Pads
// Encrypt Server: connects only to the encrypt client, encrypting
// the passed-in plaintext and key, returning ciphertext to enc_client.
// Server runs in the background and supports up to 5 concurrent socket connections.

#include <ctype.h>
#include <netdb.h>       
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  //  ssize_t
#include <sys/socket.h> //  send(),recv()
#include <sys/wait.h>   //  waitpid
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
        error("enc_server: ERROR reading from socket");
    }

    // Send message through socket to the server
    charsWritten = send(socketFD, server, strlen(server), 0);
    if (charsWritten < 0) {
        error("enc_server: ERROR writing to socket");
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
    size_t chunk = 1024;
    int    charsRead, length;
    char   buffer[chunk];

    // Get size of incoming data from client
    memset(buffer, '\0', chunk);
	charsRead = recv(socketFD, buffer, chunk, 0); 
	if (charsRead < 0) error("enc_server: ERROR reading from socket in receiveData()");

    // Allocate space on the heap for incoming socket data
    length = atoi(buffer);
    char* data = (char*)malloc(length * sizeof(char));
    char* p = data;

    while (length > 0) {
        // Clear temporary buffer for incoming socket chunk
        memset(buffer, '\0', chunk);
        charsRead = recv(socketFD, buffer, sizeof(buffer) -1, 0);
        
        // Error reading from socket
        if (charsRead < 0) { error("enc_server: ERROR reading from socket in receiveData()"); }
        
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
*   encryptData()
*   Encrypts socket data, storing encrypted text in heap memory.
*
*   arg:    data - plaintext and key received from socket
*   return: pointer to heap memory containing encrypted text
*/
char* encryptData(char* data) {
    // Finds start and end of key by newline chars
    char* j = strchr(data, '\n');
    int   textLen = strlen(data) - strlen(j);
    char* k = strchr(j + 1, '\n');
    int   keyLen = (strlen(j) - 1) - strlen(k);
    
    // Copies key string to null terminated buffer
    char  key[keyLen + 1];
    memset(key, '\0', keyLen + 1);
    strncpy(key, j + 1, keyLen);
    
    // Copies plain text string to null terminated buffer
    char text[textLen + 1];
    memset(text, '\0', textLen + 1);
    strncpy(text, data, textLen);
    
    // Allocates heap memory to store encrypted text
    char* encryptedText = (char*)malloc((textLen +1) * sizeof(char));
    int temp;
    char letters[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    // Iterates through plain text, encrypting one char at a time
    for (int i=0; i < strlen(text); i++) {
        if (text[i] == ' ') {
            // Plaintext char and key char are both spaces
            if (key[i] == ' ') {
                temp = 0;
            // Plaintext char is a space but key char is not a space
            } else {
                temp = (key[i] - 64) % 27;
            }
        
        } else {
            // Plaintext char is not a space but key char is a space
            if (key[i] == ' ') {
                temp = (text[i] - 64) % 27;

            // Plaintext and key chars are not spaces
            } else {
                temp = ((text[i] - 64) + (key[i] - 64)) % 27;
            }
        }
        // Stores encrypted char in text buffer
        text[i] = letters[temp];
    }
    // Copies encrypted buffer to heap memory
    strcpy(encryptedText, text);
    strcat(encryptedText, "\n");
    return encryptedText;
}

/*
*   sendData()
*   First sends the size of the outgoing data, then sends
*   encrypted text through socket connection.
*
*   arg:    data - pointer to encrypted text  
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

    // Sends encrypted data through socket to client
    p = data;
    sleep(1);
    while (length > 0) {
        charsWritten = send(socketFD, p, length, 0);
        length -= charsWritten;
        p += charsWritten;
    }
}

/*
*   main()
*   Runs main program loop for opening socket, binding to given
*   port, and creating forks to connect to up to 5 clients. Calls 
*   helper functions for performing handshake, receiving, and 
*   sending data through socket. Receives plaintext data, 
*   encrypts it, and sends back to each client.
*
*   arg:    argc - number of CL arguments  
*   arg:    argv - string array of CL text arguments
*
*   return: EXIT_SUCCESS or EXIT_FAILURE
*/
int main(int argc, char *argv[]){
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo;

    // Check usage & args
    if (argc != 2) { 
        fprintf(stderr,"enc_server: USAGE %s port\n", argv[0]); 
        exit(EXIT_FAILURE);
    } 
    
    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) { error("enc_server: ERROR opening server socket"); }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));
    // Associate the socket to the port
    int bindSocket = bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    // Check for errors
    if (bindSocket < 0) { error("enc_server: ERROR on binding socket"); }

    // Start listening for connections. Allow up to 5 connections to queue up
    listen(listenSocket, 5); 
    
    // Initialize variables for tracking processes
    pid_t parentPid = getpid();
    pid_t spawnPid;
    int   connectionSocket, waitStatus;
    int   childPidArr[5];
	int   numOfSockets = 0;
    int   numOfProcesses = 0;
	
	while (parentPid == getpid()) {
        // Checks current processes for completion
		if (numOfProcesses > 0) {
			for (int i = 0; i < numOfProcesses; i++) {
                // Non-blocking check to see if process has completed
				if (waitpid(childPidArr[i], &waitStatus, WNOHANG) != 0) {
                    // Replace completed process with next child process
					numOfProcesses--;
					numOfSockets--;
                    childPidArr[i] = childPidArr[numOfProcesses];
				}
			}
		}
        // There are open sockets available for more child processes
		if (numOfSockets < 5) {
			// Gets client address size and accepts connection
			sizeOfClientInfo = sizeof(clientAddress); 
			connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
			if (connectionSocket < 0) { error("enc_server: ERROR on accept\n"); }

            // Creates a child process
			spawnPid = fork();
			switch (spawnPid) {
                // Child process tasks
                case 0:
                    // Valid handshake: receive, encrypt, and return data
                    if (performHandShake(connectionSocket) == true) {
                        char* data = receiveData(connectionSocket);
                        char* encryptedText = encryptData(data);
                        sendData(encryptedText, connectionSocket);
                        free(data);
                        free(encryptedText);
                    }
                    // Invalid client
                    else {
                        fprintf(stderr, "enc_server: ERROR decrypt client cannot use this server\n");
                    }
                    close(connectionSocket); 
                    return 0;
                    break;
                
                // Error handling for fork failure
                case -1:
                    error("enc_server: ERROR unable to fork");
                    break;
                
                // Parent PID: increment counters and add fork to pid array
                default:
                    childPidArr[numOfProcesses] = spawnPid;
                    numOfSockets++;
                    numOfProcesses++;
                    break;
			}
		}
	}		
    // Close the listening socket
    close(listenSocket); 
    return EXIT_SUCCESS;
}
