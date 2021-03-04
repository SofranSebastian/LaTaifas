#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/types.h>
// sys/socket.h and sys/types.h contains the necessary API


/*  Steps for creating and making it server:
 *  -create a socket here too with socket()
 *  -bind the socket to an IP and port with bind()
 *  -after the bind he can start to listen with listen()
 *  -it can accept a connection with accept()
 *  -after accept, it can send() or recv() data 
 **/


int main(){

    char message_from_server[1024] = "Welcome to La Taifas!";

    //SOCKET <<<
    //integer for the file descriptor of the socket which contains the information of the socket
    int socket_descriptor;
    //integer for the file descriptor of the client socket used for accept function    
    int client_socket_descriptor;

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

    //BIND <<<
    /**
     * bind the socket to the port and IP address specified
     * @param socket_descriptor the actual socket
     * @param server_address the server address is a struct of type sockaddr_in and we need to cast it to another struct type sockaddr\
     * @param size of the address
     */
    bind(socket_descriptor, (struct sockaddr * ) &server_address, sizeof(server_address));

    //LISTEN <<<
    /** 
     * start listening for connection
     * @param socket_descriptor the actual socket of the server
     * @param number_of_clients the number of number of clients
    */
    listen(socket_descriptor, 5);

    //ACCEPT <<<
    /** 
     * accept sockets from clients
     * @param socket_descriptor the actual socket of the server
     * @param NULL a structure which will store information about from where does the client connect
     * @param NULL the size of the struct already mentioned
    */
    client_socket_descriptor = accept(socket_descriptor, NULL, NULL);

    //SEND <<<
    /** 
     * send information to the client
     * @param client_socket_descriptor the socket of a client
     * @param message_from_server the data sent
     * @param sizeof the size of the data we sent
     * @param 0 flags
    */
    send(client_socket_descriptor, message_from_server, sizeof(message_from_server), 0);

    //CLOSE <<<
    //close(socket_descriptor);

    return 0;
}