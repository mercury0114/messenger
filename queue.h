#pragma once

#include <stdbool.h>

bool QueueEmpty();
void Push(char* message, int socket);
char* FrontMessage();
int FrontSocket();
void Pop();
