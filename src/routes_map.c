#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "routes_map.h"
#include "string_util.h"
#include "configuration.h"
#include "log.h"

#define MAX_PARAMS 10

typedef struct route {
  action action;
  regex_t* matcher;

  char* named_params[MAX_PARAMS];
  int named_params_count;
} route;

static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static void *val_dup_cb(void *privdata, const void *src) {
  route *dup = malloc(sizeof(route));
  memcpy(dup, src, sizeof(*dup));

  ((route* )dup)->matcher = ((route* )src)->matcher;
  return dup;
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void key_destructor_cb(void *privdata, void *key) {
  free(key);
}

static void val_destructor_cb(void *privdata, void *val) {
  free(val);
}

static dictType routes_dict = {
  hash_cb,
  NULL,
  val_dup_cb,
  key_comapre_cb,
  key_destructor_cb,
  val_destructor_cb
};

static char* create_normalized_path(const char *path) {
  char *result;

  if (path[0] == '/') {
    return strdup(path);
  } else {
    result = malloc_str(strlen(path) + 1);
    sprintf(result, "/%s", path);
    return result;
  }
}

void compile_matcher(route* rt_entry, char *path) {
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

    rt_entry->named_params[rt_entry->named_params_count] = parameter;
    rt_entry->named_params_count++;

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

  rt_entry->matcher = result;
}

static bool execute_matcher(route* rt_entry, const char *path, params_map* params) {
  regmatch_t matches[rt_entry->named_params_count + 1];

  int status = regexec(rt_entry->matcher, path, rt_entry->named_params_count + 1, matches, 0);
  bool match = (status == 0);

  if (match && rt_entry->named_params_count > 0) {
    int i = 0;
    for (i = 0; i < rt_entry->named_params_count; i++) {
      int length = matches[i + 1].rm_eo - matches[i + 1].rm_so;
      char* param_value = malloc_str(length);

      memcpy(param_value, path + matches[i + 1].rm_so, length);
      params_map_add_str(params, rt_entry->named_params[i], param_value);

      free(param_value);
    }
  }

  return match;
}

routes_map* routes_map_init() {
  return dictCreate(&routes_dict, NULL);
}

void routes_map_free(routes_map* rt_map) {
  dictRelease(rt_map);
}

action routes_map_process_path(routes_map *rt_map, const char *path, params_map* params) {
  char *normalized_path = create_normalized_path(path);
  action matched_action = action_404;

  dictIterator* dict_iterator;
  dictEntry* dict_entry;
  route *rt_entry;

  dict_iterator = dictGetIterator(rt_map);
  while((dict_entry = dictNext(dict_iterator)) != NULL) {
    rt_entry = dictGetEntryVal(dict_entry);

    if (execute_matcher(rt_entry, normalized_path, params)) {
      matched_action = rt_entry->action;
      break;
    }
  }

  free(dict_iterator);
  free(normalized_path);
  return matched_action;
}

void routes_map_add(routes_map *rt_map, char *path, action action) {
  route* rt_entry = malloc(sizeof(route));

  rt_entry->action = action;
  rt_entry->named_params_count = 0;
  compile_matcher(rt_entry, path);

  dictReplace(rt_map, (void *)strdup(path), (void *)rt_entry);
  free(rt_entry);
}
