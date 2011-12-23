#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_util.h"

char* malloc_str(size_t len) {
  char* ptr = malloc(len + 1);
  memset(ptr, 0, len + 1);
  ptr[len] = '\0';

  return ptr;
}

int str_starts_with(char *str, char *substr) {
  return strncmp(str, substr, strlen(substr)) == 0;
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

char* create_str_from_str(const char *src) {
  char* dst = malloc_str(strlen(src));
  strncpy(dst, src, strlen(src));
  return dst;
}
