#include <string.h>
#include <stdlib.h>

#include "params_map.h"
#include "string_util.h"
#include "log.h"

static char* copy_chunk_from_buffer(const char *buf, size_t len) {
  char *field = malloc_str(len);
  strncat(field, buf, len);

  return field;
}

// dict's callbacks
static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void key_destructor_cb(void *privdata, void *key) {
}

static void val_destructor_cb(void *privdata, void *val) {
  param_entry_free(val);
}

static dictType params_dict = {
  hash_cb,
  NULL,
  NULL,
  key_comapre_cb,
  key_destructor_cb,
  val_destructor_cb
};

param_entry* param_entry_init(char *name, char *val, bool is_file) {
  param_entry* p = malloc(sizeof(param_entry));
  p->name = name == NULL ? NULL : str_safe_dup(name);
  p->val = val == NULL ? NULL : str_safe_dup(val);
  p->is_file = is_file;

  return p;
}

void param_entry_free(param_entry* p) {
  free(p->name);
  free(p->val);

  free(p);
}

void param_entry_append(param_entry* p, const char *buf, size_t len) {
  if (p->is_file) {
    fwrite(buf, sizeof(char), len, p->file);
  } else {
    char *data_chunk = copy_chunk_from_buffer(buf, len);
    p->val = str_concat(p->val, data_chunk);
    free(data_chunk);
  }
}

// public api
params_map* params_map_init() {
  return dictCreate(&params_dict, NULL);
}

void params_map_free(params_map* p_map) {
  dictRelease(p_map);
}

param_entry* params_map_get(params_map *p_map, const char *name) {
  dictEntry* dict_entry = dictFind(p_map, (void *)name);
  return dict_entry != NULL ? dictGetEntryVal(dict_entry) : NULL;
}

void params_map_add(params_map *p_map, param_entry* param) {
  dictReplace(p_map, param->name, param);
}

void params_map_add_str(params_map *p_map, char* name, char *value) {
  param_entry* p = param_entry_init(name, value, false);
  params_map_add(p_map, p);
}

char* params_map_serialize(params_map *p_map) {
  dictIterator* dict_iterator;
  dictEntry* dict_entry;
  param_entry* param;
  char* line, *result = NULL;

  dict_iterator = dictGetIterator(p_map);

  while((dict_entry = dictNext(dict_iterator)) != NULL) {
    param = (param_entry*)dictGetEntryVal(dict_entry);

    line = malloc_str(strlen("[") + strlen(param->name) + strlen(":") + strlen(param->val) + strlen("]"));
    sprintf(line, "[%s:%s]", param->name, param->val);

    result = str_concat(result, line);

    free(line);
  }

  free(dict_iterator);

  if (result == NULL) result = strdup("Params hash is empty");
  return result;
}
