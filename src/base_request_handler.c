#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "base_request_handler.h"
#include "http_request.h"
#include "http_response.h"
#include "log.h"

extern int server_socket_fd;

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

void send_response(int socket_fd) {
  char* serialized_response = serialize_http_response();

  info("Sending:\n%s", serialized_response);

  if (send(socket_fd, serialized_response, strlen(serialized_response), 0) == -1) {
    err("can not send data to");
  }
}

// we are in a new fresh forked process
void handle_request(int socket_fd) {
  char request_buffer[MAX_DATA_SIZE];
  int received = 0;

  init_http_request();
  init_http_response();

  if ((received = recv(socket_fd, &request_buffer, MAX_DATA_SIZE, 0)) < 0) {
    die("something went completely wrong while receiving data");
  }

  parse_http_request(request_buffer, received);


  send_response(socket_fd);

  free_http_response();
  free_http_request();
  close(socket_fd);
}
