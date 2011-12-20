#include <stdlib.h>

#include "http_response.h"

void init_http_response() {
  
}

char* serialize_http_response() {
  char* error_405 = "HTTP/1.1 405 Method Not Allowed\n"
                    "Content-Type: text/plain;\n"
                    "Content-Length:81\n"
                    "Server: Cosmonaut/0.0.1\n"
                    "\n"
                    "405 Method not allowed.1Supported methods are GET, POST and HEAD.\n"
                    "Cosmonaut/0.0.1\n";

  return error_405;
}

void free_http_response() {

}
