#ifndef _networking_h
#define _networking_h

#include "networking.c"

void *get_in_addr(struct sockaddr *sa);
int bind_server_socket(char* port);

#endif