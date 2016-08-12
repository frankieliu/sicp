#include <stdio.h>
#include <stdarg.h>

typedef int (*fprintf_t) (void * data, const char * format, ...);

int fprintf_like (void * data, const char * format, ...)
{
    int printed;
    va_list ap;
    va_start (ap, format);
    printed = vfprintf (stdout, format, ap);
    va_end (ap);
    return printed;
}

fprintf_t fprintf_ptr = fprintf_like;

int main (int argc, char ** argv)
{
    int i;
    for (i = 0; i < argc; i++) {
        (*fprintf_ptr) (0, "%d: %s\n", i, argv[i]);
    }
    return 0;
}
