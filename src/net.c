#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#include "net.h"
#include "configuration.h"
#include "string_util.h"
#include "log.h"

#define HOSTNAME_MAX_LEN 1024

extern sig_atomic_t server_socket_fd;

static void reset_hints(struct addrinfo *hints, int ai_flags) {
  memset(hints, 0, sizeof *hints);
  hints->ai_family   = AF_UNSPEC;      // v4/v6
  hints->ai_socktype = SOCK_STREAM;    // TCP stream sockets
  hints->ai_flags    = ai_flags;     // fill in my IP for me
}

static void *get_in_addr(struct sockaddr *sa) {
  return &(((struct sockaddr_in *)sa)->sin_addr);
}

char *net_get_hostname() {
  struct addrinfo hints, *info;

  char *hostname = malloc_str(HOSTNAME_MAX_LEN);
  char *canon_hostname = malloc(HOSTNAME_MAX_LEN);
  gethostname(hostname, HOSTNAME_MAX_LEN);

  reset_hints(&hints, AI_CANONNAME);

  if (getaddrinfo(hostname, "http", &hints, &info) != 0) {
    die("cannot determine hostname");
  }

  strncpy(canon_hostname, info->ai_canonname, strlen(info->ai_canonname));

  freeaddrinfo(info);
  free(hostname);
  return canon_hostname;
}

int net_bind_socket() {
  int status;
  struct sockaddr_storage;
  struct addrinfo hints;
  struct addrinfo *servinfo;

  reset_hints(&hints, AI_PASSIVE);

  if ((status = getaddrinfo(NULL, configuration_get()->server_port, &hints, &servinfo)) != 0) {
    die("%s", gai_strerror(status));
  }

  server_socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

  if ((status = bind(server_socket_fd, servinfo->ai_addr, servinfo->ai_addrlen)) != 0) {
    die("%s", gai_strerror(status));
  }

  if ((status = listen(server_socket_fd, configuration_get()->socket_queue_size)) != 0) {
    die("%s", gai_strerror(status));
  }

  info("started cosmonaut on %s:%s", configuration_get()->server_name, configuration_get()->server_port);

  // clean up
  freeaddrinfo(servinfo);

  return server_socket_fd;
}

int net_accept_connection() {
  int new_fd;
  struct sockaddr their_addr;
  socklen_t addr_size = sizeof their_addr;
  char s[INET6_ADDRSTRLEN];

  new_fd = accept(server_socket_fd, (struct sockaddr *)&their_addr, &addr_size);

  inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
  info("serving request from %s", s);

  return new_fd;
}

int net_recv(int s, char *buf, int len, int timeout) {
  fd_set fds;
  int n;
  struct timeval tv;

  FD_ZERO(&fds);
  FD_SET(s, &fds);

  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  n = select(s + 1, &fds, NULL, NULL, &tv);
  if (n == 0) return -2;  // timeout
  if (n == -1) return -1; // error

  return recv(s, buf, len, 0);
}