#include "action.h"
#include "configuration.h"
#include "log.h"
#include "string_util.h"
#include "file_util.h"

#define MAP_CONTENT_TYPE(path, ext, content_type) if (str_ends_with(path, ext)) return strdup(content_type);

static char *content_type_by_path(const char *path) {
  MAP_CONTENT_TYPE(path, "html", "text/html; charset=UTF-8");
  MAP_CONTENT_TYPE(path, "css",  "text/css");
  MAP_CONTENT_TYPE(path, "js",   "application/javascript");
  MAP_CONTENT_TYPE(path, "jpg",  "image/jpg");
  MAP_CONTENT_TYPE(path, "png",  "image/png");
  MAP_CONTENT_TYPE(path, "mp3",  "audio/mp3");
  MAP_CONTENT_TYPE(path, "ico",  "image/x-icon");
  MAP_CONTENT_TYPE(path, "gif",  "image/gif");

  return strdup("application/octet-stream");
}

void render_file(http_response *response, const char *path) {
  char *relative_file_path = configuration_convert_path_to_local(path);

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

void render_json(http_response *response, const char *json_string) {
  response->code = 200;
  response->header_summary = "OK";
  response->file_path = NULL;
  response->content_length = strlen(json_string);
  response->content_type = strdup("application/json");
  response->raw_response = strdup(json_string);
}
