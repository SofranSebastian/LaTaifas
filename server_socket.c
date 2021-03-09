#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define SERVER_PORT 9003
#define MAX_QUEUE_SIZE 50

/* maximum clients */
#define MAX_C 5

#define MAX 1024

typedef struct user_info {
    int socket;
    char ip_address[16];
    char username[50];
} User;

int main() {
    
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

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = SERVER_PORT;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if ( bind(tcp_socket, (struct sockaddr*)&address, sizeof(address)) == -1 ) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }
    
    if ( listen(tcp_socket, MAX_QUEUE_SIZE) == -1 ) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in peer_address;
    socklen_t paddress_length = sizeof(peer_address);

    int new_socket;
    char client_ip4[20], received_message[MAX];
    User users[MAX_C];
    
    struct pollfd fds[MAX_C];
    int pos = 0;

    fds[pos].fd = tcp_socket;
    fds[pos].events = POLLIN;
    fds[pos].revents = 0;
    pos++;

    char username[50];

    while (1) {
        if ( poll(fds, pos, -1) == -1) {
            perror("poll");
        }

        for (int i = 0; i < pos; i++) {
            
            if (fds[i].fd <= 0) {
                continue;
            }

            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == tcp_socket) {
                    new_socket = accept(tcp_socket, (struct sockaddr*)&peer_address, &paddress_length);

                    if (new_socket == -1) {
                        perror("accept()");
                        continue;
                    }

                    if( recv(new_socket, (void*)username, sizeof(username), 0) == -1){
                        perror("recv()");
                    }
                    
                    fds[pos].fd = new_socket;
                    fds[pos].events = POLLIN; 
                    
                    strcpy(client_ip4, inet_ntoa(peer_address.sin_addr));
                    printf("Welcome %s to LaTaifas!\n", client_ip4);
                    
                    char welcoming_message[MAX];
                    snprintf(welcoming_message, sizeof(welcoming_message), "[%s] Welcome <%s> to LaTaifas!", client_ip4, username);
    
                    users[pos].socket = new_socket;
                    strcpy(users[pos].ip_address, client_ip4);
                    strcpy(users[pos].username, username);
                    strcpy(username, "\0");
                    pos++;
                    
                    for (int i = 0; i < pos; i++) {
                        if (fds[i].fd != tcp_socket) {
                            send(fds[i].fd, welcoming_message, sizeof(welcoming_message), 0);
                        }
                    }
                }
                else {
                    int read_bytes = recv(fds[i].fd, received_message, sizeof(received_message), 0);

                    if (read_bytes == -1) {
                        perror("Recv");
                    }
                    else if (read_bytes == 0) {
                        char goodbye_message[MAX];
                        snprintf(goodbye_message, sizeof(goodbye_message), "%s has left the chat.", users[i].username);
                        fprintf(stdout, "%s has left the chat.\n", users[i].ip_address);

                        for (int k = 0; k < pos; k++) {
                            if (fds[k].fd != tcp_socket && fds[k].fd != users[i].socket) {
                                send(fds[k].fd, goodbye_message, sizeof(goodbye_message), 0);
                            }
                        }

                        char temp_user[50] = "";
                        char temp_pass[50] = "";
                        int temp_flag;
                        
                        FILE *fp = fopen("loggedusers.txt","r+");
                        
                         while(!feof(fp)){
                            fscanf(fp, "%s", temp_user);
                            fscanf(fp, "%s", temp_pass);
                            fscanf(fp, "%d", &temp_flag);

                            if(strcmp(users[i].username, temp_user) == 0){
                                fseek(fp, ftell(fp)-1, SEEK_SET );
                                fprintf(fp, "%d", 0);
                                fclose(fp);
                            }
                        }

                        if (close(fds[i].fd) == -1) {
                            perror("close()");
                        }

                        fds[i].fd = -1;
                    }
                    else {                
                        fprintf(stdout, "[%s]: %s\n", users[i].username, received_message);
                        char username[50];
                        snprintf(username, 60, "[%s]: ", users[i].username);
                        strcat(username, received_message);
                        snprintf(received_message, sizeof(received_message), "%s", username);

                        for (int k = 0; k < pos; k++) {
                            if (fds[k].fd != tcp_socket && fds[k].fd != users[i].socket) {
                               send(fds[k].fd, received_message, sizeof(received_message), 0);
                            }
                        }
                        strcpy(received_message, "\0");
                    }
                }
            }
        }
    }
    close(tcp_socket);    
    return 0;
}