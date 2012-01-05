#include <string.h>
#include <stdlib.h>

#include "params_map.h"
#include "string_util.h"
#include "log.h"

// dict's callbacks
static unsigned int hash_cb(const void *key) {
  return dictGenHashFunction((unsigned char*)key, strlen((char*)key));
}

static void *key_dup_cb(void *privdata, const void *src) {
  return strdup((char *)src);
}

static void *val_dup_cb(void *privdata, const void *src) {
  const param_entry* src_param = (param_entry *)src;
  param_entry *dup = malloc(sizeof(param_entry));

  dup->name = strdup(src_param->name);
  dup->val = strdup(src_param->val);
  dup->is_file = src_param->is_file;

  return dup;
}

static int key_comapre_cb(void *privdata, const void *key1, const void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

static void key_destructor_cb(void *privdata, void *key) {
  free(key);
}

static void val_destructor_cb(void *privdata, void *val) {
  free(((param_entry *)val)->val);
  free(val);
}

static dictType params_dict = {
  hash_cb,
  key_dup_cb,
  val_dup_cb,
  key_comapre_cb,
  key_destructor_cb,
  val_destructor_cb
};

param_entry* param_entry_reinit(param_entry* p, char *name, char *val, bool is_file) {
  free(p);

  p = malloc(sizeof(param_entry));
  p->name = name == NULL ? NULL : strdup(name);
  p->val = val == NULL ? NULL : strdup(val);
  p->is_file = is_file;

  return p;
}

void param_entry_free(param_entry* p) {
  free(p->name);
  free(p->val);
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
