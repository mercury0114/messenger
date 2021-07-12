#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void* WriteMessages(void* arguments) {
	int socket = *(int*)arguments;
	printf("%d\n", socket);
	while(true) {
		char buffer[1024] = {0};
		fgets(buffer, sizeof(buffer), stdin);
		if (strlen(buffer) > 1) {
			send(socket, buffer, strlen(buffer), 0);
		}
	}
}

int ConnectToServer() {
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8080);
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("Invalid address/ Address not supported\n");
		exit(EXIT_FAILURE);
	}

	int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
		printf("\n Socket creation error \n");
		exit(EXIT_FAILURE);
	}
	if (connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Connection Failed \n");
		exit(EXIT_FAILURE);
	}
	return s;
}

int main(int argc, char const *argv[])
{
	int socket = ConnectToServer();
	printf("%d\n", socket);
	pthread_t writer;
	pthread_create(&writer, NULL, WriteMessages, &socket);
	while (true) {
		char buffer[1024] = {0};
		read(socket, buffer, sizeof(buffer));
		printf("%s", buffer);
	}
    return 0;
}
