#ifndef _string_util_h
#define _string_util_h

int str_starts_with(const char *str, const char *substr);
int str_ends_with(const char *str, const char *substr);
int str_pos(char *str, char *chr);
char *str_random(int len);
char *str_safe_dup(char *src);
char *str_concat(char *str1, char *str2);

char *str_strip_quotes(char *str);
char *str_trim(char *str);

void skip_spaces(char **ptr);

void str_sanitize(char *str);

// very unsafe
char *create_str_from_int(int i);

char *malloc_str(size_t len);

#endif
