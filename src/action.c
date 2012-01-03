#include "action.h"
#include "configuration.h"
#include "log.h"
#include "string_util.h"
#include "file_util.h"

extern struct global_config* configuration;

#define MAP_CONTENT_TYPE(path, ext, content_type) if (str_ends_with(path, ext)) return strdup(content_type);

static char* content_type_by_path(const char *path) {
  MAP_CONTENT_TYPE(path, "html", "text/html");
  MAP_CONTENT_TYPE(path, "css",  "text/css");
  MAP_CONTENT_TYPE(path, "js",   "application/javascript");
  MAP_CONTENT_TYPE(path, "jpg",  "image/jpg");
  MAP_CONTENT_TYPE(path, "png",  "image/png");
  MAP_CONTENT_TYPE(path, "mp3",  "audio/mp3");
  MAP_CONTENT_TYPE(path, "ico",  "image/x-icon");

  return strdup("application/octet-stream");
}

char* create_local_path_from_request_path(const char* request_path) {
  char *relative_path = malloc_str(strlen(configuration->public_root) + strlen("/") + strlen(request_path));
  sprintf(relative_path, "%s/%s", configuration->public_root, request_path);

  return relative_path;
}

void render_file(http_response *response, const char *path) {
  char* relative_file_path = create_local_path_from_request_path(path);

  response->code = 200;
  response->header_summary = "OK";
  response->file_path = relative_file_path;
  response->content_length = file_size(relative_file_path);
  response->content_type = content_type_by_path(path);
}

void render_text(http_response *response, const char *text) {
  response->code = 200;
  response->header_summary = "OK";
  response->file_path = NULL;
  response->content_length = strlen(text);
  response->content_type = strdup("text/html");
  response->raw_response = strdup(text);
}

void action_404(http_request* request, http_response *response) {
  render_file(response, "404.html");
}

void action_static_file(http_request* request, http_response *response) {
  render_file(response, request->url->path);
}
