#include <unistd.h>

#include "request_handler.h"
#include "http_request.h"

extern int server_socket_fd;

void request_handler_handle(int socket_fd) {
  http_request *request = http_request_init(socket_fd);

  http_request_handle(request);

  http_request_free(request);
  close(socket_fd);
}
