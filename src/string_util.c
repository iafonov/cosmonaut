#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "string_util.h"

char* malloc_str(size_t len) {
  char* ptr = malloc(len + 1);
  memset(ptr, 0, len + 1);
  ptr[len] = '\0';

  return ptr;
}

int str_starts_with(const char *str, const char *substr) {
  return strncmp(str, substr, strlen(substr)) == 0;
}

int str_ends_with(const char *str, const char *substr) {
  if (!str || !substr) return 0;
  int string_len = strlen(str);
  int substr_len = strlen(substr);

  if (substr_len > string_len) return 0;

  return strncmp(str + string_len - substr_len, substr, substr_len) == 0;
}

int str_pos(char *str, char *chr)
{
   char *p = strstr(str, chr);
   if (p) {
     return p - str;
   }
   return -1;
}

char* create_str_from_int(int i) {
  char* result = malloc_str(2 + (i / 100));
  sprintf(result, "%d", i);

  return result;
}

char *str_trim(char *str)
{
  char *end;

  while(isspace(*str)) str++;

  if (*str == 0) return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  *(end + 1) = 0;

  return str;
}

void skip_spaces(char **ptr) {
  while (isspace(**ptr)) {
    *ptr = *ptr + 1;
  }
}