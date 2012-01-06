#ifndef _params_map_h
#define _params_map_h

#include <stdbool.h>

#include "dict.h"

typedef dict params_map;

typedef struct param_entry {
  char* name;
  char* val;
  bool is_file;
} param_entry;

param_entry* param_entry_reinit(param_entry* p, char *name, char *val, bool is_file);
void param_entry_free(param_entry* p);

params_map* params_map_init();
void params_map_free(params_map* p_map);

param_entry* params_map_get(params_map *p_map, const char *name);
void params_map_add(params_map *p_map, param_entry *param);

#endif
