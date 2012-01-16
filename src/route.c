#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <regex.h>

#include "log.h"
#include "route.h"
#include "string_util.h"
#include "configuration.h"

static void compile(route* route, char *path) {
  int status, pos = 0;
  regex_t* result = malloc(sizeof(regex_t));
  regmatch_t matches[2];
  char *named_param_re_src = "([^/]*)";
  char *matcher_re_src = malloc_str(strlen(path) * 2);

  // capture named params and build regex
  while ((status = regexec(configuration_get()->param_match_regex, path + pos, 2, matches, 0)) == 0) {
    int match_length = matches[1].rm_eo - matches[1].rm_so;

    // get everething before parameter in url - step 1: /foo/(:bar)/test -> /foo/
    memcpy(matcher_re_src + strlen(matcher_re_src), path + pos, matches[0].rm_so);
    // put parameter regex - step 2: /foo/(:bar)/test -> /foo/([^/]*)
    memcpy(matcher_re_src + strlen(matcher_re_src), named_param_re_src, strlen(named_param_re_src));
    // capture named parameter name to use later during match
    char *parameter = malloc_str(match_length);
    memcpy(parameter, path + pos + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);

    route->named_params[route->named_params_count] = parameter;
    route->named_params_count++;

    // advance
    pos += matches[0].rm_eo;
  }

  // remaining url part - step 3: /foo/(:bar)/test -> /foo/([^/]*)/test
  memcpy(matcher_re_src + strlen(matcher_re_src), path + pos, strlen(path) - pos);

  char *regex = malloc_str(strlen("^") + strlen(matcher_re_src) + strlen("$"));
  sprintf(regex, "^%s$", matcher_re_src);
  if(regcomp(result, regex, REG_EXTENDED) != 0) {
    die("Cannot compile dynamic matcher for route: %s", path);
  }
  free(regex);

  route->matcher = result;
}

bool route_match(route* route, const char *path, params_map* params) {
  regmatch_t matches[route->named_params_count + 1];

  int status = regexec(route->matcher, path, route->named_params_count + 1, matches, 0);
  bool match = (status == 0);

  if (match && route->named_params_count > 0) {
    int i = 0;
    for (i = 0; i < route->named_params_count; i++) {
      int length = matches[i + 1].rm_eo - matches[i + 1].rm_so;
      char* param_value = malloc_str(length);

      memcpy(param_value, path + matches[i + 1].rm_so, length);
      params_map_add_str(params, route->named_params[i], param_value);

      free(param_value);
    }
  }

  return match;
}

void route_execute_before_filter(route *route, http_request *request) {
  if (route && route->before_filter) {
    route->before_filter(request);
  }
}

route* route_init(char *path, action action) {
  route* result = malloc(sizeof(route));

  result->action = action;
  result->before_filter = NULL;
  result->after_filter = NULL;
  result->named_params_count = 0;

  compile(result, path);

  return result;
}

void route_free(route* route) {
  free(route);
}
