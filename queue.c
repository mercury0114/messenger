#include <pthread.h>
#include <string.h>
#include "queue.h"

#define N 10

static char messages[N][1024];
static char names[N][1024];
static int sockets[N];
static int first = 0, size = 0;
static pthread_mutex_t mutex;

bool QueueEmpty() {
	return size == 0;
}

void Push(char* message, char* name, int socket) {
	pthread_mutex_lock(&mutex);
	memcpy(&messages[(first+size)%N], message, strlen(message));
	memcpy(&names[(first+size)%N], name, strlen(name));
	sockets[(first + size)%N] = socket;
	size++;
	pthread_mutex_unlock(&mutex);
}

char* FrontMessage() {
	return messages[first];
}

char* FrontName() {
	return names[first];
}

int FrontSocket() {
	return sockets[first];
}

void Pop() {
	pthread_mutex_lock(&mutex);
	first = (first + 1) % N;
	size--;
	pthread_mutex_unlock(&mutex);
}
