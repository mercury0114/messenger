#pragma once

#include <stdbool.h>

bool QueueEmpty();
void Push(char* message, char* name, int socket);
char* FrontMessage();
char* FrontName();
int FrontSocket();
void Pop();
