#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <poll.h>

#define SERVER_PORT 9003
#define MAX 1024
#define MAX_USERS 4

void verify_user(char* username, char* password){

    char temp_user[50] = "";
    char temp_pass[50] = "";
    int temp_flag;
    
    FILE *fp = fopen("loggedusers.txt","r+");
    
    while(!feof(fp)){
        fscanf(fp, "%s", temp_user);
        fscanf(fp, "%s", temp_pass);
        fscanf(fp, "%d", &temp_flag);

        if(strcmp(username, temp_user) == 0){
            if(strcmp(password, temp_pass) == 0){
                if(temp_flag){
                    fclose(fp);
                    printf("User %s already logged!\n", username);
                    exit(EXIT_SUCCESS);
                }else{
                    rewind(fp);
                    while(!feof(fp)){
                        fscanf(fp, "%s", temp_user);
                        fscanf(fp, "%s", temp_pass);
                        fscanf(fp, "%d", &temp_flag);
                        if(strcmp(username, temp_user) == 0){
                            fseek(fp, ftell(fp)-1, SEEK_SET );
                            fprintf(fp, "%d", 1);
                            fclose(fp);
                            return;
                        }
                    }
                }
            }else{
                printf("Password is wrong!\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("User doesn't exist!\n");
    exit(EXIT_FAILURE);
    
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: %s username password\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    verify_user(argv[1], argv[2]);

    /* AF_INET -> address familty for the internet protocol v4 */
    /* SOCK_STREAM -> socket type that open a TCP socket */
    /* 
        0 -> a particular protocol to be used with the socket, but usually only
             a single one exists to support the socket type so we specify it as 0

        socket() returns a file descriptor for the new socket
    */

    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (tcp_socket == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = SERVER_PORT;
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int conn = connect(tcp_socket, (struct sockaddr*)&server_address, (socklen_t)sizeof(server_address));
    
    if (conn == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }
    
    char message_to_send[MAX];
    char received_message[MAX];

    struct pollfd fds[2];

    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = tcp_socket;
    fds[1].events = POLLIN;

    if( send(tcp_socket, (void*)argv[1], (size_t)50, 0) == -1 ){
        perror("send()");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        if ( poll(fds, 2, -1) == -1 ) {
            perror("poll()");
        }
        
        if (fds[0].revents & POLLIN) {
            fgets(message_to_send, sizeof(message_to_send)+1, stdin);
            message_to_send[strlen(message_to_send)-1] = '\0';

            if ( send(tcp_socket, message_to_send, sizeof(message_to_send), 0) == -1 ) {
                perror("send()");
            }  
        }

        if (fds[1].revents & POLLIN) {
            int recv_bytes = recv(tcp_socket, received_message, sizeof(received_message), 0);
            if (recv_bytes == -1) {
                perror("recv message");
                continue;
            }
            else if (recv_bytes == 0) {

                char temp_user[50] = "";
                char temp_pass[50] = "";
                int temp_flag;
                int users_count = 4;

                FILE *fp = fopen("loggedusers.txt","r+");
                        
                while(!feof(fp) && users_count--){
                    fscanf(fp, "%s", temp_user);
                    fscanf(fp, "%s", temp_pass);
                    fscanf(fp, "%d", &temp_flag);
                
                    fseek(fp, ftell(fp)-1, SEEK_SET );
                    fprintf(fp, "%d", 0);
                }
                
                fprintf(stdout, "Server closed.\n");
                fclose(fp);
                close(tcp_socket);
                exit(EXIT_SUCCESS);
            }
            else {
                fprintf(stdout, "%s\n", received_message);
            }
        }
    }
    close(tcp_socket);
    return 0;
}