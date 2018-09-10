/*	
 *
 *	CS 2600 / CS 8803: Introduction to Operating Systems
 *  Georgia Tech OMSCS via Udacity
 *  Problem Set 1: Echo Protocol (Server)
 *  Author: Tomasz Foster
 * 
 */	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

/*	
 *	Define Constants
 */	

#define DEFAULT_SERVER_PORT 8888
#define BUFFER_SIZE 1024

/*	
 *	Error handler
 */	

void error(const char *message) {
    perror(message);
    exit(1);
}

/*
 *  Capitalize function
 */

void capitalizeMessage(char *message){
    do {
        *message = toupper((unsigned char) *message);
    } while(*message++);
}

/*	
 *  Echo Protocol Server:
 *  Optional Flags: -p [server_port]
 *          
 *  i.e. ./server -p 8888
 */	

int main (int argc, char *argv[]) {

    // default variables and values
    int socket_file_descriptor = 0;
    int client_socket_file_descriptor = 0;
    int max_connections = 1;
    int set_reuse_address = 1;
    int port_number = (int)DEFAULT_SERVER_PORT;

    struct sockaddr_in client;
    struct sockaddr_in server;

    socklen_t client_address_length;

    // extending simple socket server with the following variables
    int number_of_bytes = 0;
    char buffer[BUFFER_SIZE];
    char *client_ip_address;
    struct hostent *client_information;
    
    /*	
     *	Flags: -p to specify port
     *  Defaults to 8888
     */	

    if( argc > 1 ) {
        for( int i = 0; i < argc; i++ ){
            if( strcmp(argv[i],"-p") == 0 ){
                port_number = atoi(argv[i + 1]);
            }
        }
    }

    // create socket
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if ( socket_file_descriptor < 0) {
        close(socket_file_descriptor);
        error("Error creating listener socket\n");
    }

    // bzero: zero out all bytes in memory to `\0` so there's no junk
    bzero(&server, sizeof(server));
    // sin_family: set address family, most likely AF_INET
    server.sin_family = AF_INET;
    // htons: convert port number to Big endian format aka Network Byte Order
    server.sin_port = htons(port_number);
    // sin_addr: network address. s_addr: 32 bit IP address in Network Byte Order
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind the socket
    if( bind(socket_file_descriptor, (struct sockaddr *)&server , sizeof(server)) < 0 ){
        close(socket_file_descriptor);
        error("Server failed to bind\n");
    }

    // liston on the socket for up to n max connections
    if ( listen( socket_file_descriptor, max_connections ) < 0 ) {
        close(socket_file_descriptor);
        error("Server failed to listen\n");
    } else {
        printf("Server listening for connection on port %d\n",port_number);
    }

    client_address_length = sizeof(client);

    // accept a new client
    if ( ( client_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&client, &client_address_length )) < 0 ) {
        close(socket_file_descriptor);
        error("Server acceptance failed\n");
    } else {
        printf("Server successfully connected!\n");
    }

    // get client information
    client_information = gethostbyaddr((const char*) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), AF_INET);
    if ( client_information == NULL ){
        close(socket_file_descriptor);
        error("Could not reliably get client information. Aborting...\n");
    }

    client_ip_address = inet_ntoa(client.sin_addr);
    if ( client_ip_address == NULL ){
        close(socket_file_descriptor);
        error("Could not reliably get client IP Address. Aborting...\n");
    }
    // on success, print the client details
    printf("Client connection established with %s at %s\n", client_information->h_name, client_ip_address);

    // read message
    // clear buffer and replace all characters in it with `\0` so theres no garbage in memory
    bzero(buffer, BUFFER_SIZE);
    number_of_bytes = read( client_socket_file_descriptor, buffer, BUFFER_SIZE);
    if ( number_of_bytes == 0 ){
        close(socket_file_descriptor);
        close(client_socket_file_descriptor);
        error("Could not resolve message contents from the client\n");
    } else {
        printf("Message size %d bytes: %s\n", number_of_bytes, buffer);
    }

    // capitalize
    capitalizeMessage( buffer );

    // send message back to client
    if ( write( client_socket_file_descriptor, buffer, strlen(buffer) ) < 0 ) {
        error("Couldn't send message back to the client. Aborting...\n");
        close(socket_file_descriptor);
    } else {
        printf("Message sent\n");
    }

    // return and close
    
    return 0;

}