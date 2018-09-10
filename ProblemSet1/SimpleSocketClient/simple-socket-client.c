/*	
 *
 *	CS 2600 / CS 8803: Introduction to Operating Systems
 *  Georgia Tech OMSCS via Udacity
 *  Problem Set 1: Simple Socket Client
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

#define DEFAULT_SERVER_ADDRESS "localhost"
#define DEFAULT_SERVER_PORT 8888

/*	
 *	Error handler
 */	

void error(const char *message) {
    perror(message);
    exit(1);
}

/*	
 *  Simple Socket Client:
 *  Optional Flags: -s [server_address] -p [server_port]
 *          
 *  i.e. ./client -s localhost -p 8888
 */	

int main (int argc, char *argv[]) {

    // a string containing a printable version of the server address to connect to
    char * server_address_string = NULL;

    // default variables and values
    int socket_file_descriptor, 
        port_number, 
        client_address_length = 0;
    struct hostent *server_address;
    struct sockaddr_in server_socket_address;
    port_number = (int)DEFAULT_SERVER_PORT;

    struct hostent *server;
    unsigned long server_address_nbo;
    
    /*	
     *	Flags: -s to specify server address and -p to specify port
     *  Defaults to localhost:8080
     */	

    if( argc > 1 ) {
        for( int i = 0; i < argc; i++ ){
            if( strcmp(argv[i],"-p") == 0 ){
                port_number = atoi(argv[i + 1]);
            }   
            if( strcmp(argv[i],"-s") == 0 ){
                server_address = gethostbyname(argv[i+1]);
                server_address_string = argv[i+1];
            }
        }
    }

    // convert localhost to 127.0.0.1 or configure user provided server address
    server_address_string = server_address_string == NULL ? DEFAULT_SERVER_ADDRESS : server_address_string;
    server = gethostbyname(server_address_string);
    server_address_nbo = *(unsigned long *)(server->h_addr_list[0]);

    // create socket
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if ( socket_file_descriptor < 0) {
        error("Error opening socket");
    }

    // bzero: zero out all bytes in memory to `\0` so there's no junk
    bzero(&server_socket_address, sizeof(server_socket_address));
    // sin_family: set address family, most likely AF_INET
    server_socket_address.sin_family = AF_INET;
    // htons: convert port number to Big endian format aka Network Byte Order
    server_socket_address.sin_port = htons(port_number);
    // sin_addr: network address. s_addr: 32 bit IP address in Network Byte Order
    server_socket_address.sin_addr.s_addr = server_address_nbo;

    // Connect socket to server
    if ( 0 > connect(socket_file_descriptor, (struct sockaddr *)&server_socket_address, sizeof(server_socket_address))) {
        printf("Client failed to connect to %s:%d\n",server_address_string,port_number);
    } else {
        printf("SUCCESS: Connected to %s:%d\n",server_address_string,port_number);
    }

    // Close socket and end program
    close(socket_file_descriptor);
    return 0;

}