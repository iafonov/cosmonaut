#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "string_util.h"

char* malloc_str(size_t len) {
  char* ptr = malloc(len + 1);
  memset(ptr, 0, len + 1);
  ptr[len] = '\0';

  return ptr;
}

int str_starts_with(const char *str, const char *substr) {
  if ((str == NULL) || (substr == NULL)) return 0;
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

char *str_trim(char *str) {
  char *end;

  while(isspace(*str)) str++;

  if (*str == 0) return str;

  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  *(end + 1) = 0;

  return str;
}

static bool is_quote(char c) {
  return (c == '"' || c == '\'');
}

char *str_strip_quotes(char *str) {
  char *end;

  while(is_quote(*str)) str++;

  if (*str == 0) return str;

  end = str + strlen(str) - 1;
  while(end > str && is_quote(*end)) end--;

  *(end + 1) = 0;

  return str;
}

void skip_spaces(char **ptr) {
  while (isspace(**ptr)) {
    *ptr = *ptr + 1;
  }
}

char *str_concat(char *str1, char *str2) {
  if (str1 == NULL) {
    return strdup(str2);
  }

  if (str2 == NULL) {
    return strdup(str1);
  }

  int len1 = strlen(str1), len2 = strlen(str2);
  char *result = malloc(strlen(str1) + strlen(str2) + 1);

  memcpy(result, str1, len1);
  memcpy(result + len1, str2, len2 + 1);

  return result;
}

char* str_safe_dup(char *src) {
  return (src == NULL) ? NULL : strdup(src);
}

char* str_random(int len) {
  static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char *s = malloc(len + 1);
  int i = 0;

  for (i = 0; i < len; ++i) {
    s[i] = alphabet[rand() % (sizeof(alphabet) - 1)];
  }

  s[len] = 0;
  return s;
}
