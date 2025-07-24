// colors.c
#include <stdio.h>
#include <stdarg.h>
#include "colors.h"

void print_colored(const char *color, const char *text) {
    printf("%s%s%s", color, text, COLOR_RESET);
}

void print_coloredf(const char *color, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("%s", color);
    vprintf(fmt, args);
    printf("%s", COLOR_RESET);
    va_end(args);
}
