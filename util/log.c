#include <stdarg.h>

void print_log(char *format, ...)  {
  char* formatted_message = malloc(12 + strlen(format));
  sprintf(formatted_message, "cosmonaut: %s\n", format);

  va_list args;
  va_start(args, format);
  vfprintf(stderr, formatted_message, args);
  va_end(args);
}