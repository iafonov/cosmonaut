#include <stdlib.h>

#include "url.h"
#include "log.h"
#include "string_util.h"

#include "../deps/url_parser/url.h"

static url *copy_from_parsed_url(parsed_url *parsed_url) {
  url *result = malloc(sizeof(url));

  result->scheme   = str_safe_dup(parsed_url->scheme);
  result->host     = str_safe_dup(parsed_url->host);
  result->port     = str_safe_dup(parsed_url->port);
  result->path     = str_safe_dup(parsed_url->path);
  result->query    = str_safe_dup(parsed_url->query);
  result->fragment = str_safe_dup(parsed_url->fragment);
  result->username = str_safe_dup(parsed_url->username);
  result->password = str_safe_dup(parsed_url->password);

  return result;
}

url *url_init(char *raw_url) {
  parsed_url *parsed_url = parse_url(raw_url);

  url *result = copy_from_parsed_url(parsed_url);

  free_parsed_url(parsed_url);
  
  return result;
}

void url_free(url *url) {
  free(url->scheme);
  free(url->host);
  free(url->port);
  free(url->path);
  free(url->query);
  free(url->fragment);
  free(url->username);
  free(url->password);

  free(url);
}
