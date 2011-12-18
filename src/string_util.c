#include <string.h>

#include "string_util.h"

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
