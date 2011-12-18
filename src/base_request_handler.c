#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "base_request_handler.h"
#include "log.h"

extern int server_socket_fd;

typedef enum { GET, POST, HEAD, UNSUPPORTED } HttpMethod;

HttpMethod get_http_method(char *request) {
  info("request:\n%s", request);
  if (request[0] == 'G' && request[1] == 'E' && request[2] == 'T') return GET;
  if (request[0] == 'P' && request[1] == 'O' && request[2] == 'S' && request[3] == 'T') return POST;
  if (request[0] == 'H' && request[1] == 'E' && request[2] == 'A' && request[3] == 'D') return HEAD;

  return UNSUPPORTED;
}

void send_405(int socket_fd) {
  char* error_405 = "HTTP/1.1 405 Method Not Allowed\n"
                    "Content-Type: text/plain;\n"
                    "Content-Length:81\n"
                    "Server: Cosmonaut/0.0.1\n"
                    "\n"
                    "405 Method not allowed. Supported methods are GET, POST and HEAD.\n"
                    "Cosmonaut/0.0.1\n";

  info("Sending: %s", error_405);
  if (send(socket_fd, error_405, strlen(error_405), 0) == -1) {
    err("can not send data to");
  }
}

void handle_request(int socket_fd) {
  // we are in a new child process
  char request_buffer[MAX_DATA_SIZE];
  int numbytes = 0;
  HttpMethod http_method;

  info("child process spawned");
  close(server_socket_fd);

  if (numbytes == 0) {
    numbytes = recv(socket_fd, &request_buffer, MAX_DATA_SIZE, 0);
    request_buffer[numbytes] = '\0';

    http_method = get_http_method(request_buffer);
    switch (http_method) {
      case GET:
        info("got GET request");
        break;
      case POST:
        info("got POST request");
        break;
      case HEAD:
        info("got HEAD request");
        break;
      default:
        send_405(socket_fd);
    }
  }

  close(socket_fd);
  exit(0);
}
