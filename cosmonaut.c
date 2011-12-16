#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "./util/log.c"
#include "./util/signals.c"

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
    print_log("error: %s\n", gai_strerror(status));
    exit(1);
  }

  server_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  if ((status = bind(server_socket, servinfo->ai_addr, servinfo->ai_addrlen)) != 0) {
    print_log("error: %s\n", gai_strerror(status));
    exit(1);
  }

  if ((status = listen(server_socket, 20)) != 0) {
    print_log("error: %s\n", gai_strerror(status));
    exit(1);
  }

  print_log("started on port %s", SERVER_PORT);

  // clean up
  freeaddrinfo(servinfo);

  return server_socket;
}

int main(int argc, char *argv[]) {
  struct sockaddr their_addr;
  int new_fd;
  socklen_t addr_size = sizeof their_addr;
  setup_signal_listener();
  int server_socket = bind_server();
  char s[INET6_ADDRSTRLEN];
  char buf[MAX_DATA_SIZE];
  int numbytes = 0;

  while(1) {
    print_log("running accept");
    new_fd = accept(server_socket, (struct sockaddr *)&their_addr, &addr_size);

    inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    print_log("server: got connection from %s", s);

    if (!fork()) {
      print_log("child process spawned");
      close(server_socket);

      numbytes = recv(new_fd, &buf, MAX_DATA_SIZE, 0);
      buf[numbytes] = '\0';
      print_log("%s", buf);

      if (send(new_fd, "It works", 8, 0) == -1) {
        print_log("error sending data");
      }
      close(new_fd);
      exit(0);
    }

    close(new_fd);
  }

  return 0;
}
