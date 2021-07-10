#include "queue.h"
#include <string.h>

#define N 10

static char messages[N][1024];
static int sockets[N];
static int first = 0, size = 0;

bool QueueEmpty() {
    return size == 0;
}

void Push(char* message, int socket) {
    memcpy(&messages[(first+size)%N], message, strlen(message));
    sockets[(first + size)%N] = socket;
    size++;
}

char* FrontMessage() {
    return messages[first];
}

int FrontSocket() {
    return sockets[first];
}

void Pop() {
    first = (first + 1) % N;
    size--;
}

