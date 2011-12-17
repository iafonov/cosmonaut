#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "./util/log.h"
#include "./util/signals.h"

#define SERVER_PORT "31337"
#define MAX_DATA_SIZE 5000

int server_socket;

void reset_hints(struct addrinfo *hints) {
  memset(hints, 0, sizeof *hints);
  hints->ai_family   = AF_UNSPEC;      // v4/v6
  hints->ai_socktype = SOCK_STREAM;    // TCP stream sockets
  hints->ai_flags    = AI_PASSIVE;     // fill in my IP for me
}

void *get_in_addr(struct sockaddr *sa) {
  return &(((struct sockaddr_in*)sa)->sin_addr);
}

int bind_server() {
  int status;
  struct sockaddr_storage;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  reset_hints(&hints);

  if ((status = getaddrinfo(NULL, SERVER_PORT, &hints, &servinfo)) != 0) {
    die(gai_strerror(status));
  }

  server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  if ((status = bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen)) != 0) {
    die(gai_strerror(status));
  }

  if ((status = listen(server_socket, 20)) != 0) {
    die(gai_strerror(status));
  }

  info("starting on port %s", SERVER_PORT);

  // clean up
  freeaddrinfo(servinfo);

  return server_socket;
}

int main(int argc, char *argv[]) {
  struct sockaddr their_addr;
  int new_fd;
  socklen_t addr_size = sizeof their_addr;
  setup_signal_listeners();
  int server_socket = bind_server();
  char s[INET6_ADDRSTRLEN];
  char buf[MAX_DATA_SIZE];
  int numbytes = 0;

  while(1) {
    new_fd = accept(server_socket, (struct sockaddr *)&their_addr, &addr_size);

    inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    info("server: got connection from %s", s);

    if (!fork()) {
      info("child process spawned");
      close(server_socket);

      numbytes = recv(new_fd, &buf, MAX_DATA_SIZE, 0);
      buf[numbytes] = '\0';
      warn("recv data:\n %s", buf);

      if (send(new_fd, "It works", 8, 0) == -1) {
        err("can not send data to %s", s);
      }
      close(new_fd);
      exit(0);
    }

    close(new_fd);
  }

  return 0;
}
