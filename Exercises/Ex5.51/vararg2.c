#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void format_string2(char *fmt, va_list argptr, char *formatted_string) {
  vsprintf(formatted_string, (const char *) fmt, argptr);
}

void format_string (char *fmt,va_list argptr, char *formatted_string) {
  format_string2(fmt, argptr, formatted_string);
}

#define MAX_FMT_SIZE 1000

void debug_print(int dbg_lvl, char *fmt, ...) 
{    
  char formatted_string[MAX_FMT_SIZE];

  va_list argptr;
  va_start(argptr,fmt);
  format_string(fmt, argptr, formatted_string);
  va_end(argptr);
  fprintf(stdout, "%s",formatted_string);

}

void main() {
  debug_print (0, "hello %d %s %d", 1, "is", 1);
}
