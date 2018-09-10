/*	
 *
 *	CS 2600 / CS 8803: Introduction to Operating Systems
 *  Georgia Tech OMSCS via Udacity
 *  Problem Set 1: Simple Socket Server
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

/*	
 *	Define Constants
 */	

#define DEFAULT_SERVER_PORT 8888

/*	
 *	Error handler
 */	

void error(const char *message) {
    perror(message);
    exit(1);
}

/*	
 *  Simple Socket Server:
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
        error("Server failed to bind\n");
    }

    // liston on the socket for up to n max connections
    if ( listen( socket_file_descriptor, max_connections ) < 0 ) {
        error("Server failed to listen\n");
    } else {
        printf("Server listening for connection on port %d\n",port_number);
    }

    client_address_length = sizeof(client);

    // accept a new client
    if ( ( client_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&client, &client_address_length )) < 0 ) {
        error("Server acceptance failed\n");
    } else {
        printf("Server successfully connected!\n");
        close(socket_file_descriptor);
    }
    
    return 0;

}