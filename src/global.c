#include <stdio.h>

#include "global.h"

void _m(const char* t, const char *f, const char *fu, const int l, const char *fmt, ...) {

    va_list args;

    va_start(args, fmt);

    printf("[%s] (%s:%s:%d) ", t, fu, f, l);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

void plog(const char *fmt, ...) {

    va_list args;

    va_start(args, fmt);

    printf("%s: ", PROGNAME);
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}
