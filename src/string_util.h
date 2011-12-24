#ifndef _string_util_h
#define _string_util_h

int str_starts_with(char *str, char *substr);
int str_pos(char *str, char *chr);

// very unsafe
char* create_str_from_int(int i);
char* create_str_from_str(const char *src);

char* malloc_str(size_t len);

#endif
