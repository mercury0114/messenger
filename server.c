// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

#define PORT 8080
#define MAX_LENGTH 1024
#define NUM_THREADS 2

#define HISTORY_FILE "history.txt"

void SendHistory(int socket) {
	FILE* history = fopen(HISTORY_FILE, "r");
	char line[MAX_LENGTH] = {0};
	while(fgets(line, sizeof(line), history)) {
		send(socket, line, strlen(line), 0);
		memset(line, 0, sizeof(line));
	}
	fclose(history);
}

static pthread_mutex_t mutex;
static int assigned_sockets[NUM_THREADS] = {0};
static char names[NUM_THREADS][MAX_LENGTH];

void* ReadFromClient(void* arguments) {
    int socket = *((int *) arguments);
	char name[MAX_LENGTH] = {0};
    printf("Client connected to socket %d\n", socket);
	read(socket, name, MAX_LENGTH);
	if (strlen(name) < 2) {
		printf("Client didn't provide a name, disconnecting him\n");
		return NULL;
	}
    SendHistory(socket);
    char buffer[MAX_LENGTH] = {0};
    while (read(socket, buffer, sizeof(buffer))) {
        Push(buffer, name, socket);
        memset(buffer, 0, sizeof(buffer));
    }
	pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUM_THREADS; i++) {
        if (assigned_sockets[i] == socket) {
            assigned_sockets[i] = 0;
        }
    }
	pthread_mutex_unlock(&mutex);
    printf("Client disconnected from socket %d\n", socket);
}

void* WriteToClients(void* arguments) {
	while (true) {
		FILE* history_file = fopen(HISTORY_FILE, "a");
		while (!QueueEmpty()) {
			char* message = FrontMessage();
			char* name = FrontName();
			int socket = FrontSocket();
			int index = 0;
			char name_message[MAX_LENGTH] = {0};
			sprintf(name_message, "%s: %s", name, message);
			fprintf(history_file, "%s", name_message);
			for (int i = 0; i < NUM_THREADS; i++) {
				if (assigned_sockets[i] && assigned_sockets[i] != socket) {
					send(assigned_sockets[i], name_message, strlen(name_message), 0);
				}
			}
			Pop();
		}
		fclose(history_file);
	}
}

int main(int argc, char const *argv[])
{
	int server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_socket, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
   
	pthread_t writer;
	pthread_create(&writer, NULL, WriteToClients, NULL);

	pthread_t readers[NUM_THREADS];
	int addrlen = sizeof(address);
	while (true) {
		int client_socket = accept(server_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		if (client_socket < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		pthread_mutex_lock(&mutex);
		int index = 0;
		while (assigned_sockets[index]) {
			index++;
		}
		assigned_sockets[index] = client_socket;
		pthread_mutex_unlock(&mutex);
		pthread_create(&readers[index], NULL, ReadFromClient, &assigned_sockets[index]);
	}
	return 0;
}
