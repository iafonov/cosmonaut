#ifndef _networking_h
#define _networking_h

#include "networking.c"

int bind_server_socket_fd(char* port);
int accept_connection();

#endif