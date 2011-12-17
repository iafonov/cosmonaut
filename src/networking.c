#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "networking.h"
#include "global_config.h"
#include "log.h"

extern sig_atomic_t server_socket_fd;
extern struct GlobalConfig global_config;

void reset_hints(struct addrinfo *hints) {
  memset(hints, 0, sizeof *hints);
  hints->ai_family   = AF_UNSPEC;      // v4/v6
  hints->ai_socktype = SOCK_STREAM;    // TCP stream sockets
  hints->ai_flags    = AI_PASSIVE;     // fill in my IP for me
}

void *get_in_addr(struct sockaddr *sa) {
  return &(((struct sockaddr_in*)sa)->sin_addr);
}

int bind_server_socket_fd(char* port) {
  int status;
  struct sockaddr_storage;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  reset_hints(&hints);

  if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    die(gai_strerror(status));
  }

  server_socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  if ((status = bind(server_socket_fd, servinfo->ai_addr, servinfo->ai_addrlen)) != 0) {
    die(gai_strerror(status));
  }

  if ((status = listen(server_socket_fd, global_config.socket_queue_size)) != 0) {
    die(gai_strerror(status));
  }

  info("starting on port %s", port);

  // clean up
  freeaddrinfo(servinfo);

  return server_socket_fd;
}

int accept_connection() {
  int new_fd;
  struct sockaddr their_addr;
  socklen_t addr_size = sizeof their_addr;
  char s[INET6_ADDRSTRLEN];

  new_fd = accept(server_socket_fd, (struct sockaddr *)&their_addr, &addr_size);

  inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
  info("server: got connection from %s", s);

  return new_fd;
}
