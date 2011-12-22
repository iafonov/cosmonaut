#include "action.h"
#include "configuration.h"
#include "log.h"
#include "string_util.h"
#include "file_util.h"

extern struct global_config* configuration;

char* create_local_path_from_request_path(char* request_path) {
  char *relative_path = malloc_str(strlen(configuration->public_root) + strlen("/") + strlen(request_path));
  sprintf(relative_path, "%s/%s", configuration->public_root, request_path);

  return relative_path;
}

void render_404(http_request* request, http_response *response) {
  char* relative_file_path = create_local_path_from_request_path("404.html");

  err("404 %s", relative_file_path);

  response->code = 404;
  response->header_summary = "Not Found";
  response->file_path = relative_file_path;
  response->content_length = file_size(relative_file_path);
  response->content_type = "text/html";
}

void render_file(http_request* request, http_response *response) {
  char* relative_file_path = create_local_path_from_request_path(request->url->path);

  response->code = 200;
  response->header_summary = "OK";
  response->file_path = relative_file_path;
  response->content_length = file_size(relative_file_path);
  response->content_type = "text/html";
}
