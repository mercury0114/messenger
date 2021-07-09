// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

#define MAX_LENGTH 1024

void SendHistory(int socket) {
    FILE* history = fopen("history.txt", "r");
    char line[MAX_LENGTH] = {0};
    while(fgets(line, sizeof(line), history)) {
        send(socket, line, strlen(line), 0);
        memset(line, 0, sizeof(line));
    }
    fclose(history);
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char hello[] = "Hello message from server";
       
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
       
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("New connection\n");
        SendHistory(new_socket);
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);
            send(new_socket, buffer, strlen(buffer), 0);
        }
    }
    return 0;
}
