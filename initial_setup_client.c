#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>
// sys/socket.h and sys/types.h contains the necessary API


/*  Steps for creating and making it retrieve data: 
 *  - is created with a call on the function socket()
 *  - is connected to a remote address with a call on the function connect()
 *  - retrieve data with a call on the function recv()
 **/


int main(){

    //SOCKET <<<
    //integer for the file descriptor of the socket which contains the information of the socket
    int socket_descriptor;
    //string for storing the data retrieved from the server
    char server_response[1024];

    /**
     * create the socket with the socket()
     * @param AF_INET is the domain of the socket
     * @param SOCK_STREAM is the type of the socket
     * @param 0 definition of the protocol, 0 because it's the default protocol TCP/IP
    */
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    //ADDRESS <<<
    struct sockaddr_in server_address;
    //family of the address, what type of address we are working with
    server_address.sin_family = AF_INET;
    //set the port where to work on
    //htons() function is called because it knows how to convert an integer so that the structure can understand 
    server_address.sin_port = htons(9002);
    //set the ip address
    //sin_addr is a struct too
    //INADDR_ANY is a constant actually which is equal with 0
    //server_address.sin_addr.s_addr = inet_addr("127.0.0.1") <- for another ip address, also INET_ADDR user for IPv4 manipulation NOT IPv6
    server_address.sin_addr.s_addr = INADDR_ANY;

    //CONNECT <<<
    /** 
     * connect the socket with connect()
     * @param socket_descriptor the actual socket
     * @param server_address the server address is a struct of type sockaddr_in and we need to cast it to another struct type sockaddr\
     * @param size of the address
     * connect() return an integer which is helpful for a simple error handling -> 0 is OK, -1 is NOT OK
    */
   int connection_status = connect(socket_descriptor, (struct sockaddr * ) &server_address, sizeof(server_address));

    //check if the connection is ok
    if( connection_status == -1 ){
        printf("ERROR: the connection to the remote socket!\n");
    }

    //RECEIVE <<<
    /** 
     * receive data with recv()
     * @param socket_descritor the actual socket
     * @param server_response a place where to store the data which is retrieved from the server
     * @param sizeof the sizeof of the buffer
     * @param 0 for the flags
    */
    recv(socket_descriptor, &server_response, sizeof(server_response), 0);
    //print the data from the server
    printf("%s\n",server_response);

    //CLOSE <<<
    //close(socket_descriptor);

    return 0;
}