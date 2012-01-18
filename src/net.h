#ifndef _networking_h
#define _networking_h

int net_bind_socket();
int net_accept_connection();
int net_recv(int s, char *buf, int len, int timeout);
char* net_get_hostname();

#endif
